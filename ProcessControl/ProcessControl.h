#pragma once

#include <Windows.h>
#include <tchar.h>
#include <Psapi.h>
#include <Shlobj.h>
#include <Shlwapi.h>
#include <Tlhelp32.h>

#include "..\\Public.h"
#include "..\\ModulePath\\ModulePath.h"

#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "Shlwapi.lib")

#ifndef MOD_PROCESS_CONTROL
#define MOD_PROCESS_CONTROL _T("½ø³Ì¿ØÖÆ")
#endif

typedef enum _PROC_TYPE
{
	PROC_TYPE_UNKNOWN,
	PROC_TYPE_NORMAL,
	PROC_TYPE_CONSOLE,
	PROC_TYPE_SERVICE
} PROC_TYPE, *PPROC_TYPE, *LPPROC_TYPE;

typedef
BOOL
(*QUERY_FULL_PROCESS_IMAGE_NAMEA)(
								  __in									HANDLE	hProcess,
								  __in									DWORD	dwFlags,
								  __out_ecount_part(*lpdwSize, *lpdwSize) LPSTR	lpExeName,
								  __inout									PDWORD	lpdwSize
								  );

typedef
BOOL
(*QUERY_FULL_PROCESS_IMAGE_NAME)(
								 __in									HANDLE	hProcess,
								 __in									DWORD	dwFlags,
								 __out_ecount_part(*lpdwSize, *lpdwSize) LPWSTR	lpExeName,
								 __inout									PDWORD	lpdwSize
								 );

typedef LONG  NTSTATUS;
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)

class CProcessControl
{
	typedef enum _PROCESSINFOCLASS
	{
		ProcessBasicInformation,
		ProcessQuotaLimits,
		ProcessIoCounters,
		ProcessVmCounters,
		ProcessTimes,
		ProcessBasePriority,
		ProcessRaisePriority,
		ProcessDebugPort,
		ProcessExceptionPort,
		ProcessAccessToken,
		ProcessLdtInformation,
		ProcessLdtSize,
		ProcessDefaultHardErrorMode,
		ProcessIoPortHandlers,          // Note: this is kernel mode only
		ProcessPooledUsageAndLimits,
		ProcessWorkingSetWatch,
		ProcessUserModeIOPL,
		ProcessEnableAlignmentFaultFixup,
		ProcessPriorityClass,
		ProcessWx86Information,
		ProcessHandleCount,
		ProcessAffinityMask,
		ProcessPriorityBoost,
		ProcessDeviceMap,
		ProcessSessionInformation,
		ProcessForegroundInformation,
		ProcessWow64Information,
		ProcessImageFileName,
		ProcessLUIDDeviceMapsEnabled,
		ProcessBreakOnTermination,
		ProcessDebugObjectHandle,
		ProcessDebugFlags,
		ProcessHandleTracing,
		ProcessIoPriority,
		ProcessExecuteFlags,
		ProcessTlsInformation,
		ProcessCookie,
		ProcessImageInformation,
		ProcessCycleTime,
		ProcessPagePriority,
		ProcessInstrumentationCallback,
		ProcessThreadStackAllocation,
		ProcessWorkingSetWatchEx,
		ProcessImageFileNameWin32,
		ProcessImageFileMapping,
		ProcessAffinityUpdateMode,
		ProcessMemoryAllocationMode,
		ProcessGroupInformation,
		ProcessTokenVirtualizationEnabled,
		ProcessConsoleHostProcess,
		ProcessWindowInformation,
		MaxProcessInfoClass             // MaxProcessInfoClass should always be the last enum
	} PROCESSINFOCLASS;

	typedef struct _PEB *PPEB;

	typedef struct _PROCESS_BASIC_INFORMATION
	{
		PVOID		Reserved1;
		PPEB		PebBaseAddress;
		PVOID		Reserved2[2];
		ULONG_PTR	UniqueProcessId;
		PVOID		Reserved3;
	} PROCESS_BASIC_INFORMATION, *PPROCESS_BASIC_INFORMATION, *LPPROCESS_BASIC_INFORMATION;

	typedef
		NTSTATUS
		(WINAPI * NT_QUERY_INFORMATION_PROCESS)(
		_In_      HANDLE			ProcessHandle,
		_In_      PROCESSINFOCLASS	ProcessInformationClass,
		_Out_     PVOID				ProcessInformation,
		_In_      ULONG				ProcessInformationLength,
		_Out_opt_ PULONG			ReturnLength
		);

public:
	static
		CProcessControl *
		GetInstance();

	static
		VOID
		ReleaseInstance();

	BOOL
		Get(
		__in	BOOL	bCurrentProc,
		__in	ULONG	ulPid,
		__out	LPTSTR	lpOutBuf,
		__in	ULONG	ulOutBufSizeCh
		);

	BOOL
		Get(
		__in	BOOL	bCurrentProc,
		__in	ULONG	ulPid,
		__out	LPSTR	lpOutBuf,
		__in	ULONG	ulOutBufSizeCh
		);

	PROC_TYPE
		GetProcType(
		__in BOOL	bCurrentProc,
		__in ULONG	ulPid
		);

	BOOL
		RunAs(
		__in LPTSTR lpPath
		);

	BOOL
		Raise(
		__in BOOL	bCurrentProcess,
		__in ULONG	ulPid
		);

	VOID
		DeleteMyselfByCreateProcess();

	BOOL
		GetParentPid(
		__in	ULONG	ulPid,
		__out	PULONG	pulParentPid
		);

	BOOL
		GetName(
		__in	BOOL	bCurrentProc,
		__in	ULONG	ulPid,
		__inout LPTSTR	lpBuf,
		__in	ULONG	ulBufSizeCh
		);

	BOOL
		Terminate(
		__in LPTSTR lpPath
		);

	BOOL
		Terminate(
		__in ULONG ulPid
		);

	BOOL
		SetAutoRun(
		__in LPTSTR lpPath
		);

private:
	static CProcessControl				*	ms_pInstance;

	HMODULE								m_hModuleKernel32;
	HMODULE								m_hModuleNtdll;
	QUERY_FULL_PROCESS_IMAGE_NAMEA		m_QueryFullProcessImageNameA;
	QUERY_FULL_PROCESS_IMAGE_NAME		m_QueryFullProcessImageName;
	NT_QUERY_INFORMATION_PROCESS		m_NtQueryInformationProcess;

	CProcessControl();

	~CProcessControl();

	BOOL
		Init();

	BOOL
		Unload();

	static
		BOOL
		GetSessionId(
		__in	BOOL		bCurrentProc,
		__in	ULONG		ulPid,
		__out	int		*	pnSid
		);

	static
		BOOL
		Adjust(
		__in BOOL	bCurrentProcess,
		__in ULONG	ulPid,
		__in LPTSTR lpName
		);
};
