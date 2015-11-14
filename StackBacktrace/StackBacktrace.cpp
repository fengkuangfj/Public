#include "StackBacktrace.h"

RTLWALKFRAMECHAIN		CStackBacktrace::RtlWalkFrameChain = NULL;
HANDLE					CStackBacktrace::ms_hProcess = NULL;
BOOL					CStackBacktrace::ms_bCanUseStackBacktraceSym = FALSE;
IMAGEHLPAPIVERSION		CStackBacktrace::ImagehlpApiVersion = NULL;
SYMINITIALIZE			CStackBacktrace::SymInitialize = NULL;
SYMCLEANUP				CStackBacktrace::SymCleanup = NULL;
SYMSETOPTIONS			CStackBacktrace::SymSetOptions = NULL;
SYMGETOPTIONS			CStackBacktrace::SymGetOptions = NULL;
STACKWALK64				CStackBacktrace::StackWalk64 = NULL;
SYMFROMADDR				CStackBacktrace::SymFromAddr = NULL;
UNDECORATESYMBOLNAME	CStackBacktrace::UnDecorateSymbolName = NULL;
SYMGETLINEFROMADDR64	CStackBacktrace::SymGetLineFromAddr64 = NULL;

BOOL
CStackBacktrace::WalkFrameChaim()
{
	BOOL	bRet = FALSE;

	PVOID	ReturnAddress[MAX_PATH] = { 0 };
	ULONG	FrameCount = 0;
	ULONG	FrameNumber = 0;
	CHAR	chLog[MAX_PATH] = { 0 };

	CSimpleLogSR(MOD_STACK_BACKTRACE, LOG_LEVEL_INFORMATION_STACK_BACKTRACE, "last");

	__try
	{
		if (!RtlWalkFrameChain)
		{
			bRet = TRUE;
			__leave;
		}

		FrameCount = RtlWalkFrameChain(ReturnAddress, _countof(ReturnAddress), 0);
		for (; FrameNumber < FrameCount; FrameNumber++)
			CSimpleLogSR(MOD_STACK_BACKTRACE, LOG_LEVEL_INFORMATION_STACK_BACKTRACE, "[FrameNumber]%02d [ReturnAddress]0x%08p", FrameNumber, ReturnAddress[FrameNumber]);

		bRet = TRUE;
	}
	__finally
	{
		;
	}

	CSimpleLogSR(MOD_STACK_BACKTRACE, LOG_LEVEL_INFORMATION_STACK_BACKTRACE, "first");

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
		if (!RtlWalkFrameChain)
		{
			hModuleNtdll = GetModuleHandle(L"ntdll.dll");
			if (!hModuleNtdll)
			{
				CSimpleLogSR(MOD_STACK_BACKTRACE, LOG_LEVEL_ERROR_STACK_BACKTRACE, "GetModuleHandle failed. (%d)", GetLastError());
				__leave;
			}

			RtlWalkFrameChain = (RTLWALKFRAMECHAIN)GetProcAddress(hModuleNtdll, "RtlWalkFrameChain");
			if (!RtlWalkFrameChain)
			{
				CSimpleLogSR(MOD_STACK_BACKTRACE, LOG_LEVEL_ERROR_STACK_BACKTRACE, "GetProcAddress failed. (%d)", GetLastError());
				__leave;
			}
		}

		if (!lpSymDir)
		{
			CSimpleLogSR(MOD_STACK_BACKTRACE, LOG_LEVEL_ERROR_STACK_BACKTRACE, "input argument error");
			__leave;
		}

		if (!PathFileExists(lpSymDir))
			__leave;

		if (!ms_hProcess)
		{
			ms_hProcess = GetCurrentProcess();
			if (!ms_hProcess)
			{
				CSimpleLogSR(MOD_STACK_BACKTRACE, LOG_LEVEL_ERROR_STACK_BACKTRACE, "GetCurrentProcess failed. (%d)", GetLastError());
				__leave;
			}

			hModuleDbghelp = GetModuleHandle(L"Dbghelp.dll");
			if (!hModuleDbghelp)
			{
				CSimpleLogSR(MOD_STACK_BACKTRACE, LOG_LEVEL_ERROR_STACK_BACKTRACE, "GetModuleHandle failed. (%d)", GetLastError());
				__leave;
			}

			ImagehlpApiVersion = (IMAGEHLPAPIVERSION)GetProcAddress(hModuleDbghelp, "ImagehlpApiVersion");
			if (ImagehlpApiVersion)
			{
				lpApiVersion = ImagehlpApiVersion();
				if (!lpApiVersion)
				{
					CSimpleLogSR(MOD_STACK_BACKTRACE, LOG_LEVEL_ERROR_STACK_BACKTRACE, "ImagehlpApiVersion failed. (%d)", GetLastError());
					__leave;
				}

				ms_bCanUseStackBacktraceSym = TRUE;

				SymInitialize = (SYMINITIALIZE)GetProcAddress(hModuleDbghelp, "SymInitializeW");
				if (!SymInitialize)
				{
					CSimpleLogSR(MOD_STACK_BACKTRACE, LOG_LEVEL_ERROR_STACK_BACKTRACE, "GetProcAddress failed. (%d)", GetLastError());
					__leave;
				}

				SymCleanup = (SYMCLEANUP)GetProcAddress(hModuleDbghelp, "SymCleanup");
				if (!SymCleanup)
				{
					CSimpleLogSR(MOD_STACK_BACKTRACE, LOG_LEVEL_ERROR_STACK_BACKTRACE, "GetProcAddress failed. (%d)", GetLastError());
					__leave;
				}

				SymSetOptions = (SYMSETOPTIONS)GetProcAddress(hModuleDbghelp, "SymSetOptions");
				if (!SymSetOptions)
				{
					CSimpleLogSR(MOD_STACK_BACKTRACE, LOG_LEVEL_ERROR_STACK_BACKTRACE, "GetProcAddress failed. (%d)", GetLastError());
					__leave;
				}

				SymGetOptions = (SYMGETOPTIONS)GetProcAddress(hModuleDbghelp, "SymGetOptions");
				if (!SymGetOptions)
				{
					CSimpleLogSR(MOD_STACK_BACKTRACE, LOG_LEVEL_ERROR_STACK_BACKTRACE, "GetProcAddress failed. (%d)", GetLastError());
					__leave;
				}

				StackWalk64 = (STACKWALK64)GetProcAddress(hModuleDbghelp, "StackWalk64");
				if (!StackWalk64)
				{
					CSimpleLogSR(MOD_STACK_BACKTRACE, LOG_LEVEL_ERROR_STACK_BACKTRACE, "GetProcAddress failed. (%d)", GetLastError());
					__leave;
				}

				SymFromAddr = (SYMFROMADDR)GetProcAddress(hModuleDbghelp, "SymFromAddr");
				if (!SymFromAddr)
				{
					CSimpleLogSR(MOD_STACK_BACKTRACE, LOG_LEVEL_ERROR_STACK_BACKTRACE, "GetProcAddress failed. (%d)", GetLastError());
					__leave;
				}

				UnDecorateSymbolName = (UNDECORATESYMBOLNAME)GetProcAddress(hModuleDbghelp, "UnDecorateSymbolName");
				if (!UnDecorateSymbolName)
				{
					CSimpleLogSR(MOD_STACK_BACKTRACE, LOG_LEVEL_ERROR_STACK_BACKTRACE, "GetProcAddress failed. (%d)", GetLastError());
					__leave;
				}

				SymGetLineFromAddr64 = (SYMGETLINEFROMADDR64)GetProcAddress(hModuleDbghelp, "SymGetLineFromAddr64");
				if (!SymGetLineFromAddr64)
				{
					CSimpleLogSR(MOD_STACK_BACKTRACE, LOG_LEVEL_ERROR_STACK_BACKTRACE, "GetProcAddress failed. (%d)", GetLastError());
					__leave;
				}

				if (!SymInitialize(ms_hProcess, lpSymDir, TRUE))
				{
					CSimpleLogSR(MOD_STACK_BACKTRACE, LOG_LEVEL_ERROR_STACK_BACKTRACE, "SymInitialize failed. (%d)", GetLastError());
					__leave;
				}

				dwOptions = SymGetOptions();
				dwOptions |= SYMOPT_LOAD_LINES;
				SymSetOptions(dwOptions);
			}
		}

		bRet = TRUE;
	}
	__finally
	{
		if (!bRet)
		{
			if (!Unload())
				CSimpleLogSR(MOD_STACK_BACKTRACE, LOG_LEVEL_INFORMATION_STACK_BACKTRACE, "Unload failed");
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
			if (SymCleanup)
				SymCleanup(ms_hProcess);

			ms_hProcess = NULL;
		}

		RtlWalkFrameChain = NULL;
		ms_bCanUseStackBacktraceSym = NULL;
		ImagehlpApiVersion = NULL;
		SymInitialize = NULL;
		SymCleanup = NULL;
		SymSetOptions = NULL;
		SymGetOptions = NULL;
		StackWalk64 = NULL;
		SymFromAddr = NULL;
		UnDecorateSymbolName = NULL;
		SymGetLineFromAddr64 = NULL;

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
	BOOL					bRet = FALSE;

	STACKFRAME64			StackFrame64 = { 0 };

	DWORD64					dw64Displacement = 0;
	PSYMBOL_INFO			pSymbol = NULL;
	DWORD					dwDisplacement = 0;
	IMAGEHLP_LINE64			Line = { 0 };
	CHAR					chDecoratedName[MAX_PATH] = { 0 };
	LPEXCEPTION_POINTERS	lpExceptionPointers = NULL;
	HANDLE					hThread = NULL;
	CONTEXT					Context = { 0 };
	CHAR					chHomeDir[MAX_PATH] = { 0 };
	CHAR					chLog[MAX_PATH] = { 0 };

	CSimpleLogSR(MOD_STACK_BACKTRACE, LOG_LEVEL_INFORMATION_STACK_BACKTRACE, "last");

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
			CSimpleLogSR(MOD_STACK_BACKTRACE, LOG_LEVEL_ERROR_STACK_BACKTRACE, "calloc failed. (%d)", GetLastError());
			__leave;
		}

		do
		{
			ZeroMemory(pSymbol, sizeof(SYMBOL_INFO) - sizeof(CHAR) + MAX_PATH);

			pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
			pSymbol->MaxNameLen = MAX_PATH;

			if (!StackWalk64(
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

				CSimpleLogSR(MOD_STACK_BACKTRACE, LOG_LEVEL_ERROR_STACK_BACKTRACE, "StackWalk64 failed. (%d)", GetLastError());
				__leave;
			}

			if (!SymFromAddr(
				ms_hProcess,
				StackFrame64.AddrPC.Offset,
				&dw64Displacement,
				pSymbol
				))
			{
				CSimpleLogSR(MOD_STACK_BACKTRACE, LOG_LEVEL_ERROR_STACK_BACKTRACE, "SymFromAddr failed. (%d)", GetLastError());
				__leave;
			}

			if (!UnDecorateSymbolName(
				pSymbol->Name,
				chDecoratedName,
				_countof(chDecoratedName),
				0
				))
			{
				CSimpleLogSR(MOD_STACK_BACKTRACE, LOG_LEVEL_ERROR_STACK_BACKTRACE, "UnDecorateSymbolName failed. (%d)", GetLastError());
				__leave;
			}

			ZeroMemory(&Line, sizeof(Line));

			Line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

			if (!SymGetLineFromAddr64(
				ms_hProcess,
				StackFrame64.AddrPC.Offset,
				&dwDisplacement,
				&Line
				))
			{
				if (487 != GetLastError())
				{
					CSimpleLogSR(MOD_STACK_BACKTRACE, LOG_LEVEL_ERROR_STACK_BACKTRACE, "SymGetLineFromAddr64 failed. (%d)", GetLastError());
					__leave;
				}
			}
			else
				CSimpleLogSR(MOD_STACK_BACKTRACE, LOG_LEVEL_INFORMATION_STACK_BACKTRACE, "[%s][%s][%d]", chDecoratedName, Line.FileName, Line.LineNumber);
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

	CSimpleLogSR(MOD_STACK_BACKTRACE, LOG_LEVEL_INFORMATION_STACK_BACKTRACE, "first");

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
			CSimpleLogSR(MOD_STACK_BACKTRACE, LOG_LEVEL_ERROR_STACK_BACKTRACE, "ReadProcessMemory failed. (%d)", GetLastError());
			__leave;
		}
	}
	__finally
	{
		;
	}

	return bRet;
}
