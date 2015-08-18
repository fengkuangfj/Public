#include "StackBacktrace.h"

RTLWALKFRAMECHAIN	CStackBacktrace::ms_RtlWalkFrameChain	= NULL;
HANDLE				CStackBacktrace::ms_hProcess			= NULL;

BOOL
	CStackBacktrace::WalkFrameChaim()
{
	BOOL	bRet					= FALSE;

	PVOID	ReturnAddress[MAX_PATH]	= {0};
	ULONG	FrameCount				= 0;
	ULONG	FrameNumber				= 0;


	__try
	{
		FrameCount = ms_RtlWalkFrameChain(ReturnAddress, _countof(ReturnAddress), 0);
		printf("StackCount %d \n", FrameCount);
		for (; FrameNumber < FrameCount; FrameNumber++)
			printf("Stack[%d] 0x%08p \n", FrameNumber, ReturnAddress[FrameNumber]);

		bRet = TRUE;
	}
	__finally
	{
		;
	}

	return bRet;
}

BOOL
	CStackBacktrace::Init(
	__in LPSTR lpSymDir
	)
{
	BOOL	bRet		= FALSE;

	HMODULE	hModule		= NULL;
	DWORD	dwOptions	= 0;


	__try
	{
		if (!ms_RtlWalkFrameChain)
		{
			hModule = GetModuleHandle(L"ntdll.dll");
			if (!hModule)
			{
				printf("GetModuleHandle failed. (%d) \n", GetLastError());
				__leave;
			}

			ms_RtlWalkFrameChain = (RTLWALKFRAMECHAIN)GetProcAddress(hModule, "RtlWalkFrameChain");
			if (!ms_RtlWalkFrameChain)
			{
				printf("GetProcAddress failed. (%d) \n", GetLastError());
				__leave;
			}
		}

		if (!lpSymDir)
		{
			printf("input argument error \n");
			__leave;
		}

		if (!PathFileExistsA(lpSymDir))
			__leave;

		if (!ms_hProcess)
		{
			ms_hProcess = GetCurrentProcess();
			if (!ms_hProcess)
			{
				printf("GetCurrentProcess failed. (%d) \n", GetLastError());
				__leave;
			}

			if (!SymInitialize(ms_hProcess, lpSymDir, TRUE))
			{
				printf("SymInitialize failed. (%d) \n", GetLastError());
				__leave;
			}

			dwOptions = SymGetOptions();
			dwOptions |= SYMOPT_LOAD_LINES;
			SymSetOptions(dwOptions);
		}

		bRet = TRUE;
	}
	__finally
	{
		if (!bRet)
		{
			if (ms_hProcess)
			{
				SymCleanup(ms_hProcess);
				ms_hProcess = NULL;
			}

			ms_RtlWalkFrameChain = NULL;
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
			SymCleanup(ms_hProcess);
			ms_hProcess = NULL;
		}

		ms_RtlWalkFrameChain = NULL;

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

	printf("begin \n");
	
	__try
	{
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

				printf("StackWalk64 failed. (%d) \n", GetLastError());
				__leave;
			}

			if (!SymFromAddr(
				ms_hProcess,
				StackFrame64.AddrPC.Offset,
				&dw64Displacement,
				pSymbol
				))
			{
				printf("SymFromAddr failed. (%d) \n", GetLastError());
				__leave;
			}

			if (!UnDecorateSymbolName(
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

			if (!SymGetLineFromAddr64(
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
				printf("[%s][%s][%d] \n", chDecoratedName, Line.FileName, Line.LineNumber);
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
