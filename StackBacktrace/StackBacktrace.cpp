#include "StackBacktrace.h"

RTLWALKFRAMECHAIN		CStackBacktrace::ms_RtlWalkFrameChain			= NULL;
HANDLE					CStackBacktrace::ms_hProcess					= NULL;
BOOL					CStackBacktrace::ms_bCanUseStackBacktraceSym	= FALSE;
IMAGEHLPAPIVERSION		CStackBacktrace::ms_ImagehlpApiVersion			= NULL;
SYMINITIALIZE			CStackBacktrace::ms_SymInitialize				= NULL;
SYMCLEANUP				CStackBacktrace::ms_SymCleanup					= NULL;
SYMSETOPTIONS			CStackBacktrace::ms_SymSetOptions				= NULL;
SYMGETOPTIONS			CStackBacktrace::ms_SymGetOptions				= NULL;
STACKWALK64				CStackBacktrace::ms_StackWalk64					= NULL;
SYMFROMADDR				CStackBacktrace::ms_SymFromAddr					= NULL;
UNDECORATESYMBOLNAME	CStackBacktrace::ms_UnDecorateSymbolName		= NULL;
SYMGETLINEFROMADDR64	CStackBacktrace::ms_SymGetLineFromAddr64		= NULL;

BOOL
	CStackBacktrace::WalkFrameChaim()
{
	BOOL	bRet					= FALSE;

	PVOID	ReturnAddress[MAX_PATH]	= {0};
	ULONG	FrameCount				= 0;
	ULONG	FrameNumber				= 0;
	CHAR	chLog[MAX_PATH]			= {0};

	printf("begin \n");

	__try
	{
		if (!ms_RtlWalkFrameChain)
		{
			bRet = TRUE;
			__leave;
		}

		FrameCount = ms_RtlWalkFrameChain(ReturnAddress, _countof(ReturnAddress), 0);
		for (; FrameNumber < FrameCount; FrameNumber++)
		{
			printf("[FrameNumber]%02d [ReturnAddress]0x%08p \n", FrameNumber, ReturnAddress[FrameNumber]);

#ifdef _DEBUG
			StringCchPrintfA(chLog, _countof(chLog), "[FrameNumber]%02d [ReturnAddress]0x%08p \n", FrameNumber, ReturnAddress[FrameNumber]);
			OutputDebugStringA(chLog);
#endif
		}

		bRet = TRUE;
	}
	__finally
	{
		;
	}

	printf("end \n");

	return bRet;
}

BOOL
	CStackBacktrace::Init(
	__in LPTSTR lpSymDir
	)
{
	BOOL			bRet			= FALSE;

	HMODULE			hModuleNtdll	= NULL;
	DWORD			dwOptions		= 0;
	HMODULE			hModuleDbghelp	= NULL;
	LPAPI_VERSION	lpApiVersion	= NULL;


	__try
	{
		if (!ms_RtlWalkFrameChain)
		{
			hModuleNtdll = GetModuleHandle(L"ntdll.dll");
			if (!hModuleNtdll)
			{
				printf("[ntdll] GetModuleHandle failed. (%d) \n", GetLastError());
				__leave;
			}

			ms_RtlWalkFrameChain = (RTLWALKFRAMECHAIN)GetProcAddress(hModuleNtdll, "RtlWalkFrameChain");
			if (!ms_RtlWalkFrameChain)
			{
				printf("[RtlWalkFrameChain] GetProcAddress failed. (%d) \n", GetLastError());
				__leave;
			}
		}

		if (!lpSymDir)
		{
			printf("input argument error \n");
			__leave;
		}

		if (!PathFileExists(lpSymDir))
			__leave;

		if (!ms_hProcess)
		{
			ms_hProcess = GetCurrentProcess();
			if (!ms_hProcess)
			{
				printf("GetCurrentProcess failed. (%d) \n", GetLastError());
				__leave;
			}

			hModuleDbghelp = GetModuleHandle(L"Dbghelp.dll");
			if (!hModuleDbghelp)
			{
				printf("[Dbghelp] GetModuleHandle failed. (%d) \n", GetLastError());
				__leave;
			}

			ms_ImagehlpApiVersion = (IMAGEHLPAPIVERSION)GetProcAddress(hModuleDbghelp, "ImagehlpApiVersion");
			if (ms_ImagehlpApiVersion)
			{
				lpApiVersion = ms_ImagehlpApiVersion();
				if (!lpApiVersion)
				{
					printf("ms_ImagehlpApiVersion failed. (%d) \n", GetLastError());
					__leave;
				}

				ms_bCanUseStackBacktraceSym = TRUE;

				ms_SymInitialize = (SYMINITIALIZE)GetProcAddress(hModuleDbghelp, "SymInitializeW");
				if (!ms_SymInitialize)
				{
					printf("[SymInitializeW] GetProcAddress failed. (%d) \n", GetLastError());
					__leave;
				}

				ms_SymCleanup = (SYMCLEANUP)GetProcAddress(hModuleDbghelp, "SymCleanup");
				if (!ms_SymCleanup)
				{
					printf("[SymCleanup] GetProcAddress failed. (%d) \n", GetLastError());
					__leave;
				}

				ms_SymSetOptions = (SYMSETOPTIONS)GetProcAddress(hModuleDbghelp, "SymSetOptions");
				if (!ms_SymSetOptions)
				{
					printf("[SymSetOptions] GetProcAddress failed. (%d) \n", GetLastError());
					__leave;
				}

				ms_SymGetOptions = (SYMGETOPTIONS)GetProcAddress(hModuleDbghelp, "SymGetOptions");
				if (!ms_SymGetOptions)
				{
					printf("[SymGetOptions] GetProcAddress failed. (%d) \n", GetLastError());
					__leave;
				}

				ms_StackWalk64 = (STACKWALK64)GetProcAddress(hModuleDbghelp, "StackWalk64");
				if (!ms_StackWalk64)
				{
					printf("[StackWalk64] GetProcAddress failed. (%d) \n", GetLastError());
					__leave;
				}

				ms_SymFromAddr = (SYMFROMADDR)GetProcAddress(hModuleDbghelp, "SymFromAddr");
				if (!ms_SymFromAddr)
				{
					printf("[SymFromAddr] GetProcAddress failed. (%d) \n", GetLastError());
					__leave;
				}

				ms_UnDecorateSymbolName = (UNDECORATESYMBOLNAME)GetProcAddress(hModuleDbghelp, "UnDecorateSymbolName");
				if (!ms_UnDecorateSymbolName)
				{
					printf("[UnDecorateSymbolName] GetProcAddress failed. (%d) \n", GetLastError());
					__leave;
				}

				ms_SymGetLineFromAddr64 = (SYMGETLINEFROMADDR64)GetProcAddress(hModuleDbghelp, "SymGetLineFromAddr64");
				if (!ms_SymGetLineFromAddr64)
				{
					printf("[SymGetLineFromAddr64] GetProcAddress failed. (%d) \n", GetLastError());
					__leave;
				}

				if (!ms_SymInitialize(ms_hProcess, lpSymDir, TRUE))
				{
					printf("SymInitialize failed. (%d) \n", GetLastError());
					__leave;
				}

				dwOptions = ms_SymGetOptions();
				dwOptions |= SYMOPT_LOAD_LINES;
				ms_SymSetOptions(dwOptions);
			}
		}

		bRet = TRUE;
	}
	__finally
	{
		if (!bRet)
		{
			if (!Unload())
				printf("Unload failed \n");
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
		if (ms_hProcess)
		{
			if (ms_SymCleanup)
				ms_SymCleanup(ms_hProcess);
	
			ms_hProcess = NULL;
		}

		ms_RtlWalkFrameChain = NULL;
		ms_bCanUseStackBacktraceSym = NULL;
		ms_ImagehlpApiVersion = NULL;
		ms_SymInitialize = NULL;
		ms_SymCleanup = NULL;
		ms_SymSetOptions = NULL;
		ms_SymGetOptions = NULL;
		ms_StackWalk64 = NULL;
		ms_SymFromAddr = NULL;
		ms_UnDecorateSymbolName = NULL;
		ms_SymGetLineFromAddr64 = NULL;

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
	return ms_bCanUseStackBacktraceSym ? StackBacktraceSym() : WalkFrameChaim();
}

BOOL
	CStackBacktrace::StackBacktraceSym()
{
	BOOL					bRet						= FALSE;

	STACKFRAME64			StackFrame64				= {0};

	DWORD64					dw64Displacement			= 0;
	PSYMBOL_INFO			pSymbol						= NULL;
	DWORD					dwDisplacement				= 0;
	IMAGEHLP_LINE64			Line						= {0};
	CHAR					chDecoratedName[MAX_PATH]	= {0};
	LPEXCEPTION_POINTERS	lpExceptionPointers			= NULL;
	HANDLE					hThread						= NULL;
	CONTEXT					Context						= {0};
	CHAR					chHomeDir[MAX_PATH]			= {0};
	CHAR					chLog[MAX_PATH]				= {0};

	printf("begin \n");

	__try
	{
		if (!ms_bCanUseStackBacktraceSym)
		{
			bRet = TRUE;
			__leave;
		}

		hThread = GetCurrentThread();

		RtlCaptureContext(&Context);

		Context.ContextFlags = CONTEXT_ALL;

		StackFrame64.AddrPC.Mode = AddrModeFlat;
		StackFrame64.AddrPC.Offset = Context.Eip;
		StackFrame64.AddrStack.Mode = AddrModeFlat;
		StackFrame64.AddrStack.Offset = Context.Esp;
		StackFrame64.AddrFrame.Mode = AddrModeFlat;
		StackFrame64.AddrFrame.Offset = Context.Ebp;

		pSymbol = (PSYMBOL_INFO)calloc(1, sizeof(SYMBOL_INFO) - sizeof(CHAR) + MAX_PATH);
		if (!pSymbol)
		{
			printf("calloc failed. (%d) \n", GetLastError());
			__leave;
		}

		do 
		{
			ZeroMemory(pSymbol, sizeof(SYMBOL_INFO) - sizeof(CHAR) + MAX_PATH);

			pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
			pSymbol->MaxNameLen = MAX_PATH;

			if (!ms_StackWalk64(
				IMAGE_FILE_MACHINE_I386,
				ms_hProcess,
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

				printf("StackWalk64 failed. (%d) \n", GetLastError());
				__leave;
			}

			if (!ms_SymFromAddr(
				ms_hProcess,
				StackFrame64.AddrPC.Offset,
				&dw64Displacement,
				pSymbol
				))
			{
				printf("SymFromAddr failed. (%d) \n", GetLastError());
				__leave;
			}

			if (!ms_UnDecorateSymbolName(
				pSymbol->Name,
				chDecoratedName,
				_countof(chDecoratedName),
				0
				))
			{
				printf("UnDecorateSymbolName failed. (%d) \n", GetLastError());
				__leave;
			}

			ZeroMemory(&Line, sizeof(Line));

			Line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

			if (!ms_SymGetLineFromAddr64(
				ms_hProcess,
				StackFrame64.AddrPC.Offset,
				&dwDisplacement,
				&Line
				))
			{
				if (487 != GetLastError())
				{
					printf("SymGetLineFromAddr64 failed. (%d) \n", GetLastError());
					__leave;
				}
			}
			else
			{
				printf("[%s][%s][%d] \n", chDecoratedName, Line.FileName, Line.LineNumber);

#ifdef _DEBUG
				StringCchPrintfA(chLog, _countof(chLog), "[%s][%s][%d] \n", chDecoratedName, Line.FileName, Line.LineNumber);
				OutputDebugStringA(chLog);
#endif
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

	printf("end \n");

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
	BOOL bRet = FALSE;


	__try
	{
		bRet = ReadProcessMemory(
			hProcess,
			(LPCVOID)lpBaseAddress,
			lpBuffer,
			nSize,
			lpNumberOfBytesRead
			);
		if (!bRet)
		{
			printf("ReadProcessMemory failed. (%d) \n", GetLastError());
			__leave;
		}
	}
	__finally
	{
		;
	}

	return bRet;
}
