#include "StackBacktrace.h"

CStackBacktrace	* CStackBacktrace::ms_pInstance = NULL;

BOOL
CStackBacktrace::WalkFrameChaim()
{
	BOOL	bRet = FALSE;

	PVOID	ReturnAddress[MAX_PATH] = { 0 };
	ULONG	FrameCount = 0;
	ULONG	FrameNumber = 0;
	CHAR	chLog[MAX_PATH] = { 0 };

	printfEx(MOD_STACK_BACKTRACE, PRINTF_LEVEL_INFORMATION, "last");

	__try
	{
		if (!m_pfRtlWalkFrameChain)
		{
			bRet = TRUE;
			__leave;
		}

		FrameCount = m_pfRtlWalkFrameChain(ReturnAddress, _countof(ReturnAddress), 0);
		for (; FrameNumber < FrameCount; FrameNumber++)
			printfEx(MOD_STACK_BACKTRACE, PRINTF_LEVEL_INFORMATION, "[FrameNumber]%02d [ReturnAddress]0x%p", FrameNumber, ReturnAddress[FrameNumber]);

		bRet = TRUE;
	}
	__finally
	{
		;
	}

	printfEx(MOD_STACK_BACKTRACE, PRINTF_LEVEL_INFORMATION, "first");

	return bRet;
}

BOOL
	CStackBacktrace::Init(
	__in LPTSTR lpSymDir
	)
{
	BOOL			bRet = FALSE;

	HMODULE			hModuleNtdll = NULL;
	DWORD			dwOptions = 0;
	HMODULE			hModuleDbghelp = NULL;
	LPAPI_VERSION	lpApiVersion = NULL;


	__try
	{
		if (!lpSymDir)
		{
			printfEx(MOD_STACK_BACKTRACE, PRINTF_LEVEL_ERROR, "input argument error");
			__leave;
		}

		if (!PathFileExists(lpSymDir))
			__leave;

		_tcscat_s(m_tchSymDir, _countof(m_tchSymDir), lpSymDir);

		if (!m_pfRtlWalkFrameChain)
		{
			hModuleNtdll = GetModuleHandle(L"ntdll.dll");
			if (!hModuleNtdll)
			{
				printfEx(MOD_STACK_BACKTRACE, PRINTF_LEVEL_ERROR, "GetModuleHandle failed. (%d)", GetLastError());
				__leave;
			}

			m_pfRtlWalkFrameChain = (RTLWALKFRAMECHAIN)GetProcAddress(hModuleNtdll, "RtlWalkFrameChain");
			if (!m_pfRtlWalkFrameChain)
			{
				printfEx(MOD_STACK_BACKTRACE, PRINTF_LEVEL_ERROR, "GetProcAddress RtlWalkFrameChain failed. (%d)", GetLastError());
				__leave;
			}
		}

		if (!m_hProcess)
		{
			m_hProcess = GetCurrentProcess();
			if (!m_hProcess)
			{
				printfEx(MOD_STACK_BACKTRACE, PRINTF_LEVEL_ERROR, "GetCurrentProcess failed. (%d)", GetLastError());
				__leave;
			}

			hModuleDbghelp = GetModuleHandle(L"Dbghelp.dll");
			if (!hModuleDbghelp)
			{
				printfEx(MOD_STACK_BACKTRACE, PRINTF_LEVEL_ERROR, "GetModuleHandle failed. (%d)", GetLastError());
				__leave;
			}

			m_pfImagehlpApiVersion = (IMAGEHLPAPIVERSION)GetProcAddress(hModuleDbghelp, "ImagehlpApiVersion");
			if (m_pfImagehlpApiVersion)
			{
				lpApiVersion = m_pfImagehlpApiVersion();
				if (!lpApiVersion)
				{
					printfEx(MOD_STACK_BACKTRACE, PRINTF_LEVEL_ERROR, "ImagehlpApiVersion failed. (%d)", GetLastError());
					__leave;
				}

				m_bCanUseStackBacktraceSym = TRUE;

				m_pfSymInitialize = (SYMINITIALIZE)GetProcAddress(hModuleDbghelp, "SymInitialize");
				if (!m_pfSymInitialize)
				{
					printfEx(MOD_STACK_BACKTRACE, PRINTF_LEVEL_ERROR, "GetProcAddress SymInitialize failed. (%d)", GetLastError());
					__leave;
				}

				m_pfSymCleanup = (SYMCLEANUP)GetProcAddress(hModuleDbghelp, "SymCleanup");
				if (!m_pfSymCleanup)
				{
					printfEx(MOD_STACK_BACKTRACE, PRINTF_LEVEL_ERROR, "GetProcAddress SymCleanup failed. (%d)", GetLastError());
					__leave;
				}

				m_pfSymSetOptions = (SYMSETOPTIONS)GetProcAddress(hModuleDbghelp, "SymSetOptions");
				if (!m_pfSymSetOptions)
				{
					printfEx(MOD_STACK_BACKTRACE, PRINTF_LEVEL_ERROR, "GetProcAddress SymSetOptions failed. (%d)", GetLastError());
					__leave;
				}

				m_pfSymGetOptions = (SYMGETOPTIONS)GetProcAddress(hModuleDbghelp, "SymGetOptions");
				if (!m_pfSymGetOptions)
				{
					printfEx(MOD_STACK_BACKTRACE, PRINTF_LEVEL_ERROR, "GetProcAddress SymGetOptions failed. (%d)", GetLastError());
					__leave;
				}

				m_pfStackWalk64 = (STACKWALK64)GetProcAddress(hModuleDbghelp, "StackWalk64");
				if (!m_pfStackWalk64)
				{
					printfEx(MOD_STACK_BACKTRACE, PRINTF_LEVEL_ERROR, "GetProcAddress StackWalk64 failed. (%d)", GetLastError());
					__leave;
				}

				m_pfSymFromAddr = (SYMFROMADDR)GetProcAddress(hModuleDbghelp, "SymFromAddr");
				if (!m_pfSymFromAddr)
				{
					printfEx(MOD_STACK_BACKTRACE, PRINTF_LEVEL_ERROR, "GetProcAddress SymFromAddr failed. (%d)", GetLastError());
					__leave;
				}

				m_pfUnDecorateSymbolName = (UNDECORATESYMBOLNAME)GetProcAddress(hModuleDbghelp, "UnDecorateSymbolName");
				if (!m_pfUnDecorateSymbolName)
				{
					printfEx(MOD_STACK_BACKTRACE, PRINTF_LEVEL_ERROR, "GetProcAddress UnDecorateSymbolName failed. (%d)", GetLastError());
					__leave;
				}

				m_pfSymGetLineFromAddr64 = (SYMGETLINEFROMADDR64)GetProcAddress(hModuleDbghelp, "SymGetLineFromAddr64");
				if (!m_pfSymGetLineFromAddr64)
				{
					printfEx(MOD_STACK_BACKTRACE, PRINTF_LEVEL_ERROR, "GetProcAddress SymGetLineFromAddr64 failed. (%d)", GetLastError());
					__leave;
				}

				if (!m_pfSymInitialize(m_hProcess, m_tchSymDir, TRUE))
				{
					if (87 != GetLastError())
					{
						printfEx(MOD_STACK_BACKTRACE, PRINTF_LEVEL_ERROR, "SymInitialize failed. (%d)", GetLastError());
						__leave;
					}
				}

				dwOptions = m_pfSymGetOptions();
				dwOptions |= SYMOPT_LOAD_LINES;
				m_pfSymSetOptions(dwOptions);
			}
		}

		bRet = TRUE;
	}
	__finally
	{
		if (!bRet)
		{
			if (!Unload())
				printfEx(MOD_STACK_BACKTRACE, PRINTF_LEVEL_ERROR, "Unload failed");
		}
	}

	return bRet;
}

BOOL
CStackBacktrace::Unload()
{
	BOOL bRet = FALSE;


	__try
	{
		if (m_hProcess)
		{
			if (m_pfSymCleanup)
				m_pfSymCleanup(m_hProcess);

			m_hProcess = NULL;
		}

		m_pfRtlWalkFrameChain = NULL;
		m_bCanUseStackBacktraceSym = NULL;
		m_pfImagehlpApiVersion = NULL;
		m_pfSymInitialize = NULL;
		m_pfSymCleanup = NULL;
		m_pfSymSetOptions = NULL;
		m_pfSymGetOptions = NULL;
		m_pfStackWalk64 = NULL;
		m_pfSymFromAddr = NULL;
		m_pfUnDecorateSymbolName = NULL;
		m_pfSymGetLineFromAddr64 = NULL;

		CPrintfEx::ReleaseInstance();

		bRet = TRUE;
	}
	__finally
	{
		;
	}

	return bRet;
}

BOOL
CStackBacktrace::StackBacktrace()
{
	return m_bCanUseStackBacktraceSym ? StackBacktraceSym() : WalkFrameChaim();
}

BOOL
CStackBacktrace::StackBacktraceSym()
{
	BOOL						bRet = FALSE;

	STACKFRAME64				StackFrame64 = { 0 };

	DWORD64						dw64Displacement = 0;
	PSYMBOL_INFO				pSymbol = NULL;
	DWORD						dwDisplacement = 0;
	IMAGEHLP_LINE64				Line = { 0 };
	CHAR						chDecoratedName[MAX_PATH] = { 0 };
	LPEXCEPTION_POINTERS		lpExceptionPointers = NULL;
	HANDLE						hThread = NULL;
	CONTEXT						Context = { 0 };
	CHAR						chHomeDir[MAX_PATH] = { 0 };
	CHAR						chLog[MAX_PATH] = { 0 };
	DWORD						dwMachineType = 0;
	MEMORY_BASIC_INFORMATION	MemoryBasicInfo = { 0 };
	TCHAR						tchModulePath[MAX_PATH] = { 0 };
	LPTSTR						lpModuleFileName = NULL;

	printfEx(MOD_STACK_BACKTRACE, PRINTF_LEVEL_INFORMATION, "last");

	__try
	{
		if (!m_bCanUseStackBacktraceSym)
		{
			bRet = TRUE;
			__leave;
		}

		if (!SymRefreshModuleList(m_hProcess))
		{
			printfEx(MOD_STACK_BACKTRACE, PRINTF_LEVEL_ERROR, "SymRefreshModuleList failed. (%d)", GetLastError());
			__leave;
		}

		hThread = GetCurrentThread();

		RtlCaptureContext(&Context);

		Context.ContextFlags = CONTEXT_ALL;

#ifdef _X86_
		dwMachineType = IMAGE_FILE_MACHINE_I386;
		StackFrame64.AddrPC.Offset = Context.Eip;
		StackFrame64.AddrPC.Mode = AddrModeFlat;
		StackFrame64.AddrStack.Offset = Context.Esp;
		StackFrame64.AddrStack.Mode = AddrModeFlat;
		StackFrame64.AddrFrame.Offset = Context.Ebp;
		StackFrame64.AddrFrame.Mode = AddrModeFlat;
#else
#ifdef _AMD64_
		dwMachineType = IMAGE_FILE_MACHINE_AMD64;
		StackFrame64.AddrPC.Offset = Context.Rip;
		StackFrame64.AddrPC.Mode = AddrModeFlat;
		StackFrame64.AddrFrame.Offset = Context.Rsp;
		StackFrame64.AddrFrame.Mode = AddrModeFlat;
		StackFrame64.AddrStack.Offset = Context.Rsp;
		StackFrame64.AddrStack.Mode = AddrModeFlat;
#else
#ifdef _IA64_
		dwMachineType = IMAGE_FILE_MACHINE_IA64;
		StackFrame64.AddrPC.Offset = Context.StIIP;
		StackFrame64.AddrPC.Mode = AddrModeFlat;
		StackFrame64.AddrFrame.Offset = Context.IntSp;
		StackFrame64.AddrFrame.Mode = AddrModeFlat;
		StackFrame64.AddrBStore.Offset = Context.RsBSP;
		StackFrame64.AddrBStore.Mode = AddrModeFlat;
		StackFrame64.AddrStack.Offset = Context.IntSp;
		StackFrame64.AddrStack.Mode = AddrModeFlat;
#else
		#error "Platform is not supported" 
#endif
#endif
#endif

		pSymbol = (PSYMBOL_INFO)calloc(1, sizeof(SYMBOL_INFO) - sizeof(CHAR) + MAX_PATH);
		if (!pSymbol)
		{
			printfEx(MOD_STACK_BACKTRACE, PRINTF_LEVEL_ERROR, "calloc failed. (%d)", GetLastError());
			__leave;
		}

		do
		{
			ZeroMemory(pSymbol, sizeof(SYMBOL_INFO) - sizeof(CHAR) + MAX_PATH);
			ZeroMemory(&MemoryBasicInfo, sizeof(MemoryBasicInfo));
			ZeroMemory(tchModulePath, sizeof(tchModulePath));

			pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
			pSymbol->MaxNameLen = MAX_PATH;

			if (!m_pfStackWalk64(
				dwMachineType,
				m_hProcess,
				hThread,
				&StackFrame64,
				&Context,
				ReadProcessMemoryProc64,
				SymFunctionTableAccess64,
				SymGetModuleBase64,
				NULL
				))
			{
				if (0 == GetLastError())
					break;

				printfEx(MOD_STACK_BACKTRACE, PRINTF_LEVEL_ERROR, "StackWalk64 failed. (%d)", GetLastError());
				__leave;
			}

			if (!VirtualQuery((LPCVOID)StackFrame64.AddrPC.Offset, &MemoryBasicInfo, sizeof(MemoryBasicInfo)))
			{
				printfEx(MOD_STACK_BACKTRACE, PRINTF_LEVEL_ERROR, "VirtualQuery failed. (%d)", GetLastError());
				__leave;
			}

			if (!GetModuleFileName((HMODULE)MemoryBasicInfo.AllocationBase, tchModulePath, _countof(tchModulePath)))
			{
				printfEx(MOD_STACK_BACKTRACE, PRINTF_LEVEL_ERROR, "GetModuleFileName failed. (%d)", GetLastError());
				__leave;
			}

			lpModuleFileName = PathFindFileName(tchModulePath);

			if (!m_pfSymFromAddr(
				m_hProcess,
				StackFrame64.AddrPC.Offset,
				&dw64Displacement,
				pSymbol
				))
			{
				if (6 == GetLastError() || 487 == GetLastError())
				{
					if (lpModuleFileName)
						printfEx(MOD_STACK_BACKTRACE, PRINTF_LEVEL_INFORMATION, "[0x%x][%S]", (DWORD_PTR)StackFrame64.AddrPC.Offset, lpModuleFileName);
					else
						printfEx(MOD_STACK_BACKTRACE, PRINTF_LEVEL_INFORMATION, "[0x%x]", (DWORD_PTR)StackFrame64.AddrPC.Offset);

					continue;
				}

				printfEx(MOD_STACK_BACKTRACE, PRINTF_LEVEL_ERROR, "SymFromAddr failed. (%d)", GetLastError());
				__leave;
			}

			if (!m_pfUnDecorateSymbolName(
				pSymbol->Name,
				chDecoratedName,
				_countof(chDecoratedName),
				0
				))
			{
				printfEx(MOD_STACK_BACKTRACE, PRINTF_LEVEL_ERROR, "UnDecorateSymbolName failed. (%d)", GetLastError());
				__leave;
			}

			ZeroMemory(&Line, sizeof(Line));

			Line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

			if (!m_pfSymGetLineFromAddr64(
				m_hProcess,
				StackFrame64.AddrPC.Offset,
				&dwDisplacement,
				&Line
				))
			{
				if (487 != GetLastError())
				{
					printfEx(MOD_STACK_BACKTRACE, PRINTF_LEVEL_ERROR, "SymGetLineFromAddr64 failed. (%d)", GetLastError());
					__leave;
				}
			}
			else
			{
				if (lpModuleFileName)
					printfEx(MOD_STACK_BACKTRACE, PRINTF_LEVEL_INFORMATION, "[0x%x][%S][%s][%s][%d]", (DWORD_PTR)StackFrame64.AddrPC.Offset, lpModuleFileName, chDecoratedName, Line.FileName, Line.LineNumber);
				else
					printfEx(MOD_STACK_BACKTRACE, PRINTF_LEVEL_INFORMATION, "[0x%x][%s][%s][%d]", (DWORD_PTR)StackFrame64.AddrPC.Offset, chDecoratedName, Line.FileName, Line.LineNumber);
			}
		} while (TRUE);

		bRet = TRUE;
	}
	__finally
	{
		if (pSymbol)
		{
			free(pSymbol);
			pSymbol = NULL;
		}
	}

	printfEx(MOD_STACK_BACKTRACE, PRINTF_LEVEL_INFORMATION, "first");

	return bRet;
}

BOOL
CALLBACK
ReadProcessMemoryProc64(
_In_  HANDLE  hProcess,
_In_  DWORD64 lpBaseAddress,
_Out_ PVOID   lpBuffer,
_In_  DWORD   nSize,
_Out_ LPDWORD lpNumberOfBytesRead
)
{
	BOOL	bRet = FALSE;

	SIZE_T	NumberOfBytesRead = 0;


	__try
	{
		bRet = ReadProcessMemory(
			hProcess,
			(LPCVOID)lpBaseAddress,
			lpBuffer,
			nSize,
			&NumberOfBytesRead
			);
		if (!bRet)
		{
			if (299 != GetLastError())
				printfEx(MOD_STACK_BACKTRACE, PRINTF_LEVEL_ERROR, "ReadProcessMemory failed. (%d)", GetLastError());

			__leave;
		}

		*lpNumberOfBytesRead = (DWORD)NumberOfBytesRead;
	}
	__finally
	{
		;
	}

	return bRet;
}

CStackBacktrace *
	CStackBacktrace::GetInstance(
	__in LPTSTR lpSymDir
	)
{
	if (!ms_pInstance)
	{
		do 
		{
			new CStackBacktrace(lpSymDir);
			if (!ms_pInstance)
				Sleep(1000);
			else
				break;
		} while (TRUE);
	}

	return ms_pInstance;
}

VOID
	CStackBacktrace::ReleaseInstance()
{
	if (ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = NULL;
	}
}

CStackBacktrace::CStackBacktrace(
	__in LPTSTR lpSymDir
	)
{
	ms_pInstance = this;

	ZeroMemory(m_tchSymDir, sizeof(m_tchSymDir));

	m_pfRtlWalkFrameChain = NULL;
	m_hProcess = NULL;
	m_bCanUseStackBacktraceSym = FALSE;
	m_pfImagehlpApiVersion = NULL;
	m_pfSymInitialize = NULL;
	m_pfSymCleanup = NULL;
	m_pfSymSetOptions = NULL;
	m_pfSymGetOptions = NULL;
	m_pfStackWalk64 = NULL;
	m_pfSymFromAddr = NULL;
	m_pfUnDecorateSymbolName = NULL;
	m_pfSymGetLineFromAddr64 = NULL;

	if (!Init(lpSymDir))
		printfEx(MOD_STACK_BACKTRACE, PRINTF_LEVEL_ERROR, "Init failed");
}

CStackBacktrace::~CStackBacktrace()
{
	if (!Unload())
		printfEx(MOD_STACK_BACKTRACE, PRINTF_LEVEL_ERROR, "Unload failed");

	ZeroMemory(m_tchSymDir, sizeof(m_tchSymDir));

	m_pfRtlWalkFrameChain = NULL;
	m_hProcess = NULL;
	m_bCanUseStackBacktraceSym = FALSE;
	m_pfImagehlpApiVersion = NULL;
	m_pfSymInitialize = NULL;
	m_pfSymCleanup = NULL;
	m_pfSymSetOptions = NULL;
	m_pfSymGetOptions = NULL;
	m_pfStackWalk64 = NULL;
	m_pfSymFromAddr = NULL;
	m_pfUnDecorateSymbolName = NULL;
	m_pfSymGetLineFromAddr64 = NULL;
}
