#pragma once

#include <Windows.h>
#include <stdio.h>
#include <Dbghelp.h>
#include <Shlwapi.h>
#include <tchar.h>
#include <strsafe.h>

#pragma comment(lib, "Dbghelp.lib")
#pragma comment(lib, "Shlwapi.lib")

#include "..\\PrintfEx\\PrintfEx.h"

#define MOD_STACK_BACKTRACE _T("Õ»»ØËÝ")

/*++
*
* Routine Description:
*
*		This is a wrapper function for walk frame chain. It's purpose is to
*		prevent entering a function that has a huge stack usage to test some
*		if the current code path can take page faults.
*
*		N.B. This is an exported function that MUST probe the ability to take
*		page faults.
*
* Arguments:
*
*		Callers -	Supplies a pointer to an array that is to received the return
*					address values.
*
*		Count	-	Supplies the number of frames to be walked.
*
*		Flags	-	Supplies the flags value (unused).
*
* Return value:
*
*		Any return value from RtlpWalkFrameChain.
*
--*/
typedef
ULONG
(WINAPI * RTLWALKFRAMECHAIN)(
__out	PVOID *	Callers,
__in	ULONG	Count,
__in	ULONG	Flags
);

typedef
LPAPI_VERSION
(WINAPI * IMAGEHLPAPIVERSION)(
VOID
);

typedef
BOOL
(WINAPI * SYMINITIALIZE)(
__in		HANDLE	hProcess,
__in_opt	PCWSTR	UserSearchPath,
__in		BOOL	fInvadeProcess
);

typedef
BOOL
(WINAPI * SYMCLEANUP)(
__in HANDLE hProcess
);

typedef
DWORD
(WINAPI * SYMSETOPTIONS)(
__in DWORD SymOptions
);

typedef
DWORD
(WINAPI * SYMGETOPTIONS)(
VOID
);

typedef
BOOL
(WINAPI * STACKWALK64)(
__in		DWORD								MachineType,
__in		HANDLE								hProcess,
__in		HANDLE								hThread,
__inout		LPSTACKFRAME64						StackFrame,
__inout		PVOID								ContextRecord,
__in_opt	PREAD_PROCESS_MEMORY_ROUTINE64		ReadMemoryRoutine,
__in_opt	PFUNCTION_TABLE_ACCESS_ROUTINE64	FunctionTableAccessRoutine,
__in_opt	PGET_MODULE_BASE_ROUTINE64			GetModuleBaseRoutine,
__in_opt	PTRANSLATE_ADDRESS_ROUTINE64		TranslateAddress
);

typedef
BOOL
(WINAPI * SYMFROMADDR)(
__in		HANDLE			hProcess,
__in		DWORD64			Address,
__out_opt	PDWORD64		Displacement,
__inout		PSYMBOL_INFO	Symbol
);

typedef
DWORD
(WINAPI * UNDECORATESYMBOLNAME)(
__in							PCSTR	name,
__out_ecount(maxStringLength)	PSTR	outputString,
__in							DWORD	maxStringLength,
__in							DWORD	flags
);

typedef
BOOL
(WINAPI * SYMGETLINEFROMADDR64)(
__in	HANDLE				hProcess,
__in	DWORD64				qwAddr,
__out	PDWORD				pdwDisplacement,
__out	PIMAGEHLP_LINE64	Line64
);

BOOL
CALLBACK
ReadProcessMemoryProc64(
_In_  HANDLE  hProcess,
_In_  DWORD64 lpBaseAddress,
_Out_ PVOID   lpBuffer,
_In_  DWORD   nSize,
_Out_ LPDWORD lpNumberOfBytesRead
);

class CStackBacktrace
{
public:
	static
		CStackBacktrace *
		GetInstance();

	static
		VOID
		ReleaseInstance();

	BOOL
		SetArguments(
		__in LPTSTR lpSymDir
		);

	BOOL
		StackBacktrace();

private:
	static CStackBacktrace	*	ms_pInstance;

	TCHAR						m_tchSymDir[MAX_PATH];
	RTLWALKFRAMECHAIN			m_pfRtlWalkFrameChain;
	HANDLE						m_hProcess;
	BOOL						m_bCanUseStackBacktraceSym;
	IMAGEHLPAPIVERSION			m_pfImagehlpApiVersion;
	SYMINITIALIZE				m_pfSymInitialize;
	SYMCLEANUP					m_pfSymCleanup;
	SYMSETOPTIONS				m_pfSymSetOptions;
	SYMGETOPTIONS				m_pfSymGetOptions;
	STACKWALK64					m_pfStackWalk64;
	SYMFROMADDR					m_pfSymFromAddr;
	UNDECORATESYMBOLNAME		m_pfUnDecorateSymbolName;
	SYMGETLINEFROMADDR64		m_pfSymGetLineFromAddr64;

	CStackBacktrace();

	~CStackBacktrace();

	BOOL
		Init();

	BOOL
		Unload();

	BOOL
		WalkFrameChaim();

	BOOL
		StackBacktraceSym();
};
