#pragma once

#include <Windows.h>

#include "..\\PrintfEx\\PrintfEx.h"
#include "..\\VolumeDetector\\\VolumeDetector.h"

#define MOD_SERVICE _T("·þÎñ")

typedef struct _INIT_MOD_ARGUMENTS
{
	TCHAR			tchModuleName[MAX_PATH];

	BOOL			bService;

	union
	{
		struct 
		{
			HANDLE	hService;
		} Service;

		struct 
		{
			HANDLE	hWindow;
			WNDPROC	lpfnWndProc;
			BOOL	bCreateMassageLoop;
		} Window;
	};
} INIT_MOD_ARGUMENTS, *PINIT_MOD_ARGUMENTS, *LPINIT_MOD_ARGUMENTS;

typedef
	BOOL
	(* INITMOD)(
	__in LPINIT_MOD_ARGUMENTS lpInitModArguments
	);

typedef
	BOOL
	(* UNLOADMOD)();

typedef
	BOOL
	(* WOW64_DISABLE_WOW64_FS_REDIRECTION)(
	__out PVOID * OldValue
	);

typedef
	BOOL
	(* WOW64_REVERT_WOW64_FS_REDIRECTION)(
	__in PVOID OlValue
	);

class CService
{
public:
	BOOL
		Install(
		__in		LPWSTR	lpServiceName,
		__in		DWORD	dwServiceType,
		__in		DWORD	dwStartType,
		__in_opt	DWORD	dwErrorControl,
		__in		LPWSTR	lpPath,
		__in_opt	LPWSTR	lpLoadOrderGroup,
		__in_opt	LPWSTR	lpDependencies
		);

	BOOL
		Start(
		__in LPWSTR lpServiceName
		);

	BOOL
		Stop(
		__in LPWSTR lpServiceName
		);

	BOOL
		Delete(
		__in LPWSTR lpServiceName
		);

	BOOL
		Disable(
		__in LPWSTR lpServiceName
		);

	BOOL
		Register(
		__in LPTSTR		lpServiceName,
		__in INITMOD	InitMod,
		__in UNLOADMOD	UnloadMod
		);

private:
	static SERVICE_STATUS_HANDLE	ms_SvcStatusHandle;
	static TCHAR					ms_tchServiceName[MAX_PATH];
	static SERVICE_STATUS			ms_SvcStatus;
	static HANDLE					ms_hSvcStopEvent;
	static DWORD					ms_dwCheckPoint;
	static INITMOD					ms_InitMod;
	static UNLOADMOD				ms_UnloadMod;

	static
		VOID
		WINAPI
		Main(
		DWORD		dwArgc,
		LPTSTR *	lpszArgv
		);

	static
		VOID
		ReportSvcStatus(
		DWORD dwCurrentState,
		DWORD dwWin32ExitCode,
		DWORD dwWaitHint
		);

	static
		BOOL
		Init(
		DWORD		dwArgc,
		LPTSTR *	lpszArgv
		);

	static
		DWORD
		WINAPI
		CtrlHandler(
		_In_ DWORD	dwControl,
		_In_ DWORD	dwEventType,
		_In_ LPVOID	lpEventData,
		_In_ LPVOID	lpContext
		);
};
