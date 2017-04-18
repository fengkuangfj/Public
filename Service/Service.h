#pragma once

#include <Windows.h>
#include <WinSvc.h>

#include "..\\PrintfEx\\PrintfEx.h"
#include "..\\OperationSystemVersion\\OperationSystemVersion.h"

#pragma comment(lib, "Advapi32.lib")

#ifndef MOD_SERVICE
#define MOD_SERVICE _T("·þÎñ")
#endif

#ifndef SERVICE_CONTROL_TIMECHANGE
#define SERVICE_CONTROL_TIMECHANGE             0x00000010
#endif

#ifndef SERVICE_CONTROL_TRIGGEREVENT
#define SERVICE_CONTROL_TRIGGEREVENT           0x00000020
#endif

#ifndef SERVICE_ACCEPT_TIMECHANGE
#define SERVICE_ACCEPT_TIMECHANGE              0x00000200
#endif

#ifndef SERVICE_ACCEPT_TRIGGEREVENT
#define SERVICE_ACCEPT_TRIGGEREVENT            0x00000400
#endif

typedef struct _INIT_MOD_ARGUMENTS
{
	TCHAR	tchModuleName[MAX_PATH];
	HWND	hWindow;
	WNDPROC	lpfnWndProc;
	BOOL	bCreateMassageLoop;
} INIT_MOD_ARGUMENTS, *PINIT_MOD_ARGUMENTS, *LPINIT_MOD_ARGUMENTS;

typedef
BOOL
(* INITMOD)(
			__in_opt LPINIT_MOD_ARGUMENTS lpInitModArguments
			);

typedef
BOOL
(* UNLOADMOD)();

typedef
BOOL
(WINAPI * WOW64_DISABLE_WOW64_FS_REDIRECTION)(
	__out PVOID * OldValue
	);

typedef
BOOL
(WINAPI * WOW64_REVERT_WOW64_FS_REDIRECTION)(
	__in PVOID OlValue
	);

class CService
{
public:
	static
		CService *
		GetInstance();

	static
		VOID
		ReleaseInstance();

	BOOL
		Install(
		__in		LPWSTR	lpServiceName,
		__in_opt	LPWSTR	lpDisplayName,
		__in_opt	LPWSTR	lpDescription,
		__in		DWORD	dwServiceType,
		__in		DWORD	dwStartType,
		__in_opt	DWORD	dwErrorControl,
		__in		LPWSTR	lpPath,
		__in_opt	LPWSTR	lpLoadOrderGroup,
		__in_opt	LPWSTR	lpDependencies,
		__in_opt	BOOL	bInteractWithTheDesktop
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
		DeleteFileInDrivers(
		__in LPTSTR lpServiceName
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
		Enable(
		__in LPWSTR lpServiceName
		);

	BOOL
		Exist(
		__in LPTSTR lpServiceName
		);

	BOOL
		Restart(
		__in	LPTSTR	lpServiceName,
		__inout PBOOL	pbReboot
		);

	BOOL
		ChangeLoadOrderGroup(
		__in LPTSTR lpServiceName,
		__in LPTSTR lpLoadOrderGroup
		);

	BOOL
		Register(
		__in		LPTSTR					lpServiceName,
		__in		INITMOD					InitMod,
		__in_opt	LPINIT_MOD_ARGUMENTS	lpInitModArguments,
		__in		UNLOADMOD				UnloadMod
		);

	BOOL
		CanInteractWithTheDesktop(
		__in LPTSTR lpServiceName
		);

private:
	static CService						*	ms_pInstance;

	SERVICE_STATUS_HANDLE					m_SvcStatusHandle;
	TCHAR									m_tchServiceName[MAX_PATH];
	SERVICE_STATUS							m_SvcStatus;
	HANDLE									m_hSvcStopEvent;
	DWORD									m_dwCheckPoint;
	INITMOD									m_pfInitMod;
	UNLOADMOD								m_pfUnloadMod;
	INIT_MOD_ARGUMENTS						m_InitModArguments;

	WOW64_DISABLE_WOW64_FS_REDIRECTION		m_pfWow64DisableWow64FsRedirection;
	WOW64_REVERT_WOW64_FS_REDIRECTION		m_pfWow64RevertWow64FsRedirection;
	HMODULE									m_hModule;

	CService();

	~CService();

	BOOL
		Init();

	BOOL
		Unload();

	static
		VOID
		WINAPI
		Main(
		DWORD		dwArgc,
		LPTSTR *	lpszArgv
		);

	VOID
		ReportSvcStatus(
		DWORD dwCurrentState,
		DWORD dwWin32ExitCode,
		DWORD dwWaitHint
		);

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
