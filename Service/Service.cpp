#include "Service.h"

WOW64_DISABLE_WOW64_FS_REDIRECTION	g_Wow64DisableWow64FsRedirection		= NULL;
WOW64_REVERT_WOW64_FS_REDIRECTION	g_Wow64RevertWow64FsRedirection			= NULL;

TCHAR								CService::ms_tchServiceName[MAX_PATH]	= {0};
SERVICE_STATUS_HANDLE				CService::ms_SvcStatusHandle			= NULL;
SERVICE_STATUS						CService::ms_SvcStatus					= {0};
HANDLE								CService::ms_hSvcStopEvent				= NULL;
DWORD								CService::ms_dwCheckPoint				= 1;
INITMOD								CService::ms_InitMod					= NULL;
UNLOADMOD							CService::ms_UnloadMod					= NULL;
INIT_MOD_ARGUMENTS					CService::ms_InitModArguments			= {0};

BOOL
	CService::Install(
	__in		LPWSTR	lpServiceName,
	__in		DWORD	dwServiceType,
	__in		DWORD	dwStartType,
	__in_opt	DWORD	dwErrorControl,
	__in		LPWSTR	lpPath,
	__in_opt	LPWSTR	lpLoadOrderGroup,
	__in_opt	LPWSTR	lpDependencies
	)
{
	BOOL		bRet							= FALSE;

	SC_HANDLE	hScManager						= NULL;
	SC_HANDLE	hService						= NULL;
	WCHAR		wchTemp[MAX_PATH]				= {0};
	HKEY		hkResult						= NULL;
	DWORD		dwData							= 0;
	LONG		lResult							= 0;
	WCHAR		wchPath[MAX_PATH]				= {0};
	LPWSTR		lpPosition						= NULL;
	PVOID		pOldValue						= NULL;
	BOOL		bWow64DisableWow64FsRedirection = FALSE;
	HMODULE		hModule							= NULL;


	__try
	{
		if (!lpServiceName || !lpPath || !dwServiceType)
		{
			printfEx(MOD_SERVICE, PRINTF_LEVEL_ERROR, "input arguments error. 0x%08p 0x%08p %d", lpServiceName, lpPath, dwServiceType);

			__leave;
		}

		hModule = LoadLibrary(_T("Kernel32.dll"));
		if (!hModule)
		{
			printfEx(MOD_SERVICE, PRINTF_LEVEL_ERROR, "LoadLibrary failed. (%d)", GetLastError());
			__leave;
		}

		if (!g_Wow64DisableWow64FsRedirection)
		{
			g_Wow64DisableWow64FsRedirection = (WOW64_DISABLE_WOW64_FS_REDIRECTION)GetProcAddress(hModule, "Wow64DisableWow64FsRedirection");
			if (g_Wow64DisableWow64FsRedirection)
			{
				if (!g_Wow64DisableWow64FsRedirection(&pOldValue))
				{
					printfEx(MOD_SERVICE, PRINTF_LEVEL_ERROR, "Wow64DisableWow64FsRedirection failed. (%d)", GetLastError());
					__leave;
				}

				bWow64DisableWow64FsRedirection = TRUE;

				g_Wow64RevertWow64FsRedirection = (WOW64_REVERT_WOW64_FS_REDIRECTION)GetProcAddress(hModule, "Wow64RevertWow64FsRedirection");
				if (!g_Wow64RevertWow64FsRedirection)
				{
					printfEx(MOD_SERVICE, PRINTF_LEVEL_ERROR, "GetProcAddress failed. (%d)", GetLastError());
					__leave;
				}
			}
		}

		hScManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
		if (!hScManager) 
		{
			printfEx(MOD_SERVICE, PRINTF_LEVEL_ERROR, "OpenSCManager failed. (%d)", GetLastError());
			__leave;
		}

		switch (dwStartType)
		{
		case SERVICE_BOOT_START:
		case SERVICE_SYSTEM_START:
			{
				if (!(SERVICE_FILE_SYSTEM_DRIVER & dwServiceType ||
					SERVICE_KERNEL_DRIVER & dwServiceType ||
					SERVICE_RECOGNIZER_DRIVER & dwServiceType))
				{
					printfEx(MOD_SERVICE, PRINTF_LEVEL_ERROR, "dwServiceType and dwStartType not match. 0x%08x 0x%08x", dwServiceType, dwStartType);
					__leave;
				}

				if (!GetSystemDirectory(wchPath, _countof(wchPath)))
				{
					printfEx(MOD_SERVICE, PRINTF_LEVEL_ERROR, "GetSystemDirectory failed. (%d)", GetLastError());
					__leave;
				}

				wcscat_s(wchPath, _countof(wchPath), L"\\drivers\\");

				if (0 == _wcsnicmp(wchPath, lpPath, wcslen(wchPath)))
					wcscpy_s(wchPath, _countof(wchPath), lpPath);
				else
				{
					lpPosition = wcsrchr(lpPath, L'\\');
					if (!lpPosition)
					{
						printfEx(MOD_SERVICE, PRINTF_LEVEL_ERROR, "lpPath error. %S", lpPath);
						__leave;
					}

					wcscat_s(wchPath, _countof(wchPath), lpPosition + 1);

					if (!CopyFile(lpPath, wchPath, TRUE))
					{
						printfEx(MOD_SERVICE, PRINTF_LEVEL_ERROR, "CopyFile failed. %S -> %S", lpPath, wchPath);
						__leave;
					}
				}

				break;
			}
		case SERVICE_AUTO_START:
		case SERVICE_DEMAND_START:
		case SERVICE_DISABLED:
			{
				wcscat_s(wchPath, _countof(wchPath), lpPath);
				break;
			}
		default:
			{
				printfEx(MOD_SERVICE, PRINTF_LEVEL_ERROR, "dwStartType error. 0x%08x", dwStartType);
				__leave;
			}
		}

		switch (dwErrorControl)
		{
		case SERVICE_ERROR_IGNORE:
		case SERVICE_ERROR_NORMAL:
		case SERVICE_ERROR_SEVERE:
		case SERVICE_ERROR_CRITICAL:
			break;
		default:
			{
				printfEx(MOD_SERVICE, PRINTF_LEVEL_ERROR, "dwErrorControl error. 0x%08x", dwErrorControl);
				__leave;
			}
		}

		hService = CreateService(
			hScManager,
			lpServiceName,
			lpServiceName,
			SERVICE_ALL_ACCESS,
			dwServiceType,
			dwStartType,
			dwErrorControl,
			wchPath,
			lpLoadOrderGroup,
			NULL,
			lpDependencies,
			NULL,
			NULL
			);
		if (!hService)
		{
			printfEx(MOD_SERVICE, PRINTF_LEVEL_ERROR, "CreateService failed. (%d)", GetLastError());
			__leave;
		}

		switch (dwServiceType)
		{
		case SERVICE_KERNEL_DRIVER:
			{
				dwData = sizeof(wchTemp);
				lResult = RegGetValue(
					HKEY_LOCAL_MACHINE,
					L"SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e967-e325-11ce-bfc1-08002be10318}",
					L"UpperFilters",
					RRF_RT_REG_MULTI_SZ,
					NULL,
					wchTemp,
					&dwData				
					);
				if (ERROR_SUCCESS != lResult)
				{
					printfEx(MOD_SERVICE, PRINTF_LEVEL_ERROR, "RegGetValue failed. (0x%08x)", lResult);
					__leave;
				}

				lResult = RegOpenKeyEx(
					HKEY_LOCAL_MACHINE,
					L"SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e967-e325-11ce-bfc1-08002be10318}",
					0,
					KEY_ALL_ACCESS,
					&hkResult
					);
				if (ERROR_SUCCESS != lResult)
				{
					printfEx(MOD_SERVICE, PRINTF_LEVEL_ERROR, "RegOpenKeyEx failed. (0x%08x)", lResult);
					__leave;
				}

				if (!hkResult)
				{
					printfEx(MOD_SERVICE, PRINTF_LEVEL_ERROR, "hkResult error");
					__leave;
				}

				wcscpy_s(wchTemp + dwData / sizeof(WCHAR) - 1, _countof(wchTemp) - dwData / sizeof(WCHAR) + 1, lpServiceName);
				*(wchTemp + dwData / sizeof(WCHAR) + wcslen(lpServiceName)) = L'\0';

				lResult = RegSetValueEx(
					hkResult,
					L"UpperFilters",
					0,
					REG_MULTI_SZ,
					(const BYTE*)wchTemp,
					dwData + (wcslen(lpServiceName) + 1) * sizeof(WCHAR)
					);
				if (ERROR_SUCCESS != lResult)
				{
					printfEx(MOD_SERVICE, PRINTF_LEVEL_ERROR, "RegSetValueEx failed. (0x%08x)", lResult);
					__leave;
				}

				lResult = RegFlushKey(hkResult);
				if (ERROR_SUCCESS != lResult)
				{
					printfEx(MOD_SERVICE, PRINTF_LEVEL_ERROR, "RegFlushKey failed. (0x%08x)", lResult);
					__leave;
				}

				break;
			}
		case SERVICE_FILE_SYSTEM_DRIVER:
			{
				wcscat_s(wchTemp, _countof(wchTemp), L"SYSTEM\\CurrentControlSet\\services\\");
				wcscat_s(wchTemp, _countof(wchTemp), lpServiceName);
				wcscat_s(wchTemp, _countof(wchTemp), L"\\Instances");

				lResult = RegCreateKeyEx(
					HKEY_LOCAL_MACHINE,
					wchTemp,
					0,
					NULL,
					REG_OPTION_NON_VOLATILE,
					KEY_ALL_ACCESS,
					NULL,
					&hkResult,
					NULL
					);
				if (ERROR_SUCCESS != lResult)
				{
					printfEx(MOD_SERVICE, PRINTF_LEVEL_ERROR, "RegCreateKeyEx failed. (0x%08x)", lResult);
					__leave;
				}

				if (!hkResult)
				{
					printfEx(MOD_SERVICE, PRINTF_LEVEL_ERROR, "hkResult error");
					__leave;
				}

				lResult = RegFlushKey(hkResult);
				if (ERROR_SUCCESS != lResult)
				{
					printfEx(MOD_SERVICE, PRINTF_LEVEL_ERROR, "RegFlushKey failed. (0x%08x)", lResult);
					__leave;
				}

				ZeroMemory(wchTemp, sizeof(wchTemp));
				wcscat_s(wchTemp, _countof(wchTemp), lpServiceName);
				wcscat_s(wchTemp, _countof(wchTemp), L" Instance");

				lResult = RegSetValueEx(
					hkResult,
					L"DefaultInstance",
					0,
					REG_SZ,
					(const BYTE*)wchTemp,
					wcslen(wchTemp) * sizeof(WCHAR)
					);
				if (ERROR_SUCCESS != lResult)
				{
					printfEx(MOD_SERVICE, PRINTF_LEVEL_ERROR, "RegSetValueEx failed. (0x%08x)", lResult);
					__leave;
				}

				lResult = RegFlushKey(hkResult);
				if (ERROR_SUCCESS != lResult)
				{
					printfEx(MOD_SERVICE, PRINTF_LEVEL_ERROR, "RegFlushKey failed. (0x%08x)", lResult);
					__leave;
				}

				lResult = RegCloseKey(hkResult);
				if (ERROR_SUCCESS != lResult)
				{
					printfEx(MOD_SERVICE, PRINTF_LEVEL_ERROR, "RegCloseKey failed. (0x%08x)", lResult);
					__leave;
				}

				hkResult = NULL;

				ZeroMemory(wchTemp, sizeof(wchTemp));
				wcscat_s(wchTemp, _countof(wchTemp), L"SYSTEM\\CurrentControlSet\\services\\");
				wcscat_s(wchTemp, _countof(wchTemp), lpServiceName);
				wcscat_s(wchTemp, _countof(wchTemp), L"\\Instances\\");
				wcscat_s(wchTemp, _countof(wchTemp), lpServiceName);
				wcscat_s(wchTemp, _countof(wchTemp), L" Instance");

				lResult = RegCreateKeyEx(
					HKEY_LOCAL_MACHINE,
					wchTemp,
					0,
					NULL,
					REG_OPTION_NON_VOLATILE,
					KEY_ALL_ACCESS,
					NULL,
					&hkResult,
					NULL
					);
				if (ERROR_SUCCESS != lResult)
				{
					printfEx(MOD_SERVICE, PRINTF_LEVEL_ERROR, "RegCreateKeyEx failed. (0x%08x)", lResult);
					__leave;
				}

				if (!hkResult)
				{
					printfEx(MOD_SERVICE, PRINTF_LEVEL_ERROR, "hkResult error");
					__leave;
				}

				lResult = RegFlushKey(hkResult);
				if (ERROR_SUCCESS != lResult)
				{
					printfEx(MOD_SERVICE, PRINTF_LEVEL_ERROR, "RegFlushKey failed. (0x%08x)", lResult);
					__leave;
				}

				ZeroMemory(wchTemp, sizeof(wchTemp));
				wcscat_s(wchTemp, _countof(wchTemp), L"370030");

				lResult = RegSetValueEx(
					hkResult,
					L"Altitude",
					0,
					REG_SZ,
					(const BYTE*)wchTemp,
					wcslen(wchTemp) * sizeof(WCHAR)
					);
				if (ERROR_SUCCESS != lResult)
				{
					printfEx(MOD_SERVICE, PRINTF_LEVEL_ERROR, "RegSetValueEx failed. (0x%08x)", lResult);
					__leave;
				}

				lResult = RegFlushKey(hkResult);
				if (ERROR_SUCCESS != lResult)
				{
					printfEx(MOD_SERVICE, PRINTF_LEVEL_ERROR, "RegFlushKey failed. (0x%08x)", lResult);
					__leave;
				}

				dwData = 0x0;

				lResult = RegSetValueEx(
					hkResult,
					L"Flags",
					0,
					REG_DWORD,
					(const BYTE*)&dwData,
					sizeof(DWORD)
					);
				if (ERROR_SUCCESS != lResult)
				{
					printfEx(MOD_SERVICE, PRINTF_LEVEL_ERROR, "RegSetValueEx failed. (0x%08x)", lResult);
					__leave;
				}

				lResult = RegFlushKey(hkResult);
				if (ERROR_SUCCESS != lResult)
				{
					printfEx(MOD_SERVICE, PRINTF_LEVEL_ERROR, "RegFlushKey failed. (0x%08x)", lResult);
					__leave;
				}

				break;
			}
		case SERVICE_WIN32_OWN_PROCESS:
		case SERVICE_WIN32_SHARE_PROCESS:
		case SERVICE_INTERACTIVE_PROCESS:
			break;
		default:
			{
				if (SERVICE_WIN32_OWN_PROCESS & dwServiceType || SERVICE_WIN32_SHARE_PROCESS & dwServiceType)
					break;

				printfEx(MOD_SERVICE, PRINTF_LEVEL_ERROR, "dwServiceType error. 0x%08x", dwServiceType);
				__leave;
			}
		}

		bRet = TRUE;
	}
	__finally
	{
		if (hkResult)
		{
			RegCloseKey(hkResult);
			hkResult = NULL;
		}

		if (hService)
		{
			CloseServiceHandle(hService);
			hService = NULL;
		}

		if (hScManager)
		{
			CloseServiceHandle(hScManager);
			hScManager = NULL;
		}

		if (bWow64DisableWow64FsRedirection)
		{
			if (g_Wow64RevertWow64FsRedirection)
			{
				g_Wow64RevertWow64FsRedirection(pOldValue);
				pOldValue = NULL;
			}
		}

		if (hModule)
		{
			FreeLibrary(hModule);
			hModule = NULL;
		}
	}

	return bRet;
}

BOOL
	CService::Start(
	__in LPWSTR lpServiceName
	)
{
	BOOL		bRet		= FALSE;

	SC_HANDLE	hScManager	= NULL;
	SC_HANDLE	hService	= NULL;


	__try
	{
		hScManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
		if (!hScManager)
		{
			printf("OpenSCManager failed. (%d) \n", GetLastError());
			__leave;
		}

		hService = OpenService(hScManager, lpServiceName, SERVICE_ALL_ACCESS);
		if (!hService)
		{
			printf("OpenService failed. (%d) \n", GetLastError());
			__leave;
		}

		if (!StartService(hService, 0, NULL))
		{
			printf("StartService failed. (%d) \n", GetLastError());
			__leave;
		}

		bRet = TRUE;
	}
	__finally
	{
		if (hService)
		{
			CloseServiceHandle(hService);
			hService = NULL;
		}

		if (hScManager)
		{
			CloseServiceHandle(hScManager);
			hScManager = NULL;
		}
	}

	return bRet;
}

BOOL
	CService::Stop(
	__in LPWSTR lpServiceName
	)
{
	BOOL			bRet			= FALSE;

	SC_HANDLE		hScManager		= NULL;
	SC_HANDLE		hService		= NULL;
	SERVICE_STATUS	ServiceStatus	= {0};


	__try
	{
		hScManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
		if (!hScManager)
		{
			printf("OpenSCManager failed. (%d) \n", GetLastError());
			__leave;
		}

		hService = OpenService(hScManager, lpServiceName, SERVICE_ALL_ACCESS);
		if (!hService)
		{
			printf("OpenService failed. (%d) \n", GetLastError());
			__leave;
		}

		if (!ControlService(hService, SERVICE_CONTROL_STOP, &ServiceStatus))
		{
			printf("ControlService failed. (%d) \n", GetLastError());
			__leave;
		}

		bRet = TRUE;
	}
	__finally
	{
		if (hService)
		{
			CloseServiceHandle(hService);
			hService = NULL;
		}

		if (hScManager)
		{
			CloseServiceHandle(hScManager);
			hScManager = NULL;
		}
	}

	return bRet;
}

BOOL
	CService::Delete(
	__in LPWSTR lpServiceName
	)
{
	BOOL		bRet		= FALSE;

	SC_HANDLE	hScManager	= NULL;
	SC_HANDLE	hService	= NULL;


	__try
	{
		hScManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
		if (!hScManager)
		{
			printf("OpenSCManager failed. (%d) \n", GetLastError());
			__leave;
		}

		hService = OpenService(hScManager, lpServiceName, SERVICE_ALL_ACCESS);
		if (!hService)
		{
			printf("OpenService failed. (%d) \n", GetLastError());
			__leave;
		}

		if (!DeleteService(hService))
		{
			printf("DeleteService failed. (%d) \n", GetLastError());
			__leave;
		}

		bRet = TRUE;
	}
	__finally
	{
		if (hService)
		{
			CloseServiceHandle(hService);
			hService = NULL;
		}

		if (hScManager)
		{
			CloseServiceHandle(hScManager);
			hScManager = NULL;
		}
	}

	return bRet;
}

BOOL
	CService::Disable(
	__in LPWSTR lpServiceName
	)
{
	BOOL		bRet		= FALSE;

	SC_HANDLE	hScManager	= NULL;
	SC_HANDLE	hService	= NULL;


	__try
	{
		hScManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
		if (!hScManager)
		{
			printf("OpenSCManager failed. (%d) \n", GetLastError());
			__leave;
		}

		hService = OpenService(hScManager, lpServiceName, SERVICE_ALL_ACCESS);
		if (!hService)
		{
			printf("OpenService failed. (%d) \n", GetLastError());
			__leave;
		}

		if (!ChangeServiceConfig(
			hService,
			SERVICE_NO_CHANGE,
			SERVICE_DISABLED,
			SERVICE_NO_CHANGE,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL
			))
		{
			printf("ChangeServiceConfig failed. (%d) \n", GetLastError());
			__leave;
		}

		bRet = TRUE;
	}
	__finally
	{
		if (hService)
		{
			CloseServiceHandle(hService);
			hService = NULL;
		}

		if (hScManager)
		{
			CloseServiceHandle(hScManager);
			hScManager = NULL;
		}
	}

	return bRet;
}

BOOL
	CService::Register(
	__in		LPTSTR					lpServiceName,
	__in		INITMOD					InitMod,
	__in_opt	LPINIT_MOD_ARGUMENTS	lpInitModArguments,
	__in		UNLOADMOD				UnloadMod
	)
{
	BOOL					bRet				= FALSE;

	SERVICE_TABLE_ENTRY		ServiceTableEntry[]	= 
	{
		{lpServiceName, (LPSERVICE_MAIN_FUNCTION)Main},
		{NULL, NULL}
	};


	__try
	{
		printfEx(MOD_SERVICE, PRINTF_LEVEL_INFORMATION, "begin");

		if (!lpServiceName || !InitMod || !UnloadMod)
		{
			printfEx(MOD_SERVICE, PRINTF_LEVEL_ERROR, "input arguments error. 0x%08p 0x%08p 0x%08p",lpServiceName, InitMod, UnloadMod);
			__leave;
		}

		ms_InitMod = InitMod;
		ms_UnloadMod = UnloadMod;

		if (lpInitModArguments)
		{
			if (_tcslen(lpInitModArguments->tchModuleName))
				_tcscat_s(ms_InitModArguments.tchModuleName, _countof(ms_InitModArguments.tchModuleName), lpInitModArguments->tchModuleName);

			ms_InitModArguments.hWindow = lpInitModArguments->hWindow;
			ms_InitModArguments.lpfnWndProc = lpInitModArguments->lpfnWndProc;
			ms_InitModArguments.bCreateMassageLoop = lpInitModArguments->bCreateMassageLoop;
		}

		_tcscat_s(ms_tchServiceName, _countof(ms_tchServiceName), lpServiceName);

		printfEx(MOD_SERVICE, PRINTF_LEVEL_INFORMATION, "StartServiceCtrlDispatcher begin");
		if (!StartServiceCtrlDispatcher(ServiceTableEntry))
		{
			printfEx(MOD_SERVICE, PRINTF_LEVEL_ERROR, "StartServiceCtrlDispatcher failed. (%d)", GetLastError());
			__leave;
		}
		printfEx(MOD_SERVICE, PRINTF_LEVEL_INFORMATION, "StartServiceCtrlDispatcher end");

		bRet = TRUE;
	}
	__finally
	{
		printfEx(MOD_SERVICE, PRINTF_LEVEL_INFORMATION, "end");
	}

	return bRet;
}

//
// Purpose: 
//   Called by SCM whenever a control code is sent to the service
//   using the ControlService function.
//
// Parameters:
//   dwCtrl - control code
// 
// Return value:
//   None
//
DWORD
	WINAPI
	CService::CtrlHandler(
	_In_ DWORD	dwControl,
	_In_ DWORD	dwEventType,
	_In_ LPVOID	lpEventData,
	_In_ LPVOID	lpContext
	)
{
	DWORD dwRet = NO_ERROR;


	__try
	{
		// Handle the requested control code. 
		switch (dwControl)
		{
		case SERVICE_CONTROL_STOP:
			{
				printfEx(MOD_SERVICE, PRINTF_LEVEL_INFORMATION, "SERVICE_CONTROL_STOP");

				if (!ms_UnloadMod())
					printfEx(MOD_SERVICE, PRINTF_LEVEL_ERROR, "[SERVICE_CONTROL_STOP] ms_UnloadMod failed");

				ReportSvcStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);

				// Signal the service to stop.
				SetEvent(ms_hSvcStopEvent);
				ReportSvcStatus(ms_SvcStatus.dwCurrentState, NO_ERROR, 0);
				break;
			}
		case SERVICE_CONTROL_PAUSE:
		case SERVICE_CONTROL_CONTINUE:
		case SERVICE_CONTROL_INTERROGATE:
			break;
		case SERVICE_CONTROL_SHUTDOWN:
			{
				printfEx(MOD_SERVICE, PRINTF_LEVEL_INFORMATION, "SERVICE_CONTROL_SHUTDOWN");

				if (!ms_UnloadMod())
					printfEx(MOD_SERVICE, PRINTF_LEVEL_ERROR, "[SERVICE_CONTROL_SHUTDOWN] ms_UnloadMod failed");

				break;
			}
		case SERVICE_CONTROL_PARAMCHANGE:
		case SERVICE_CONTROL_NETBINDADD:
		case SERVICE_CONTROL_NETBINDREMOVE:
		case SERVICE_CONTROL_NETBINDENABLE:
		case SERVICE_CONTROL_NETBINDDISABLE:
		case SERVICE_CONTROL_DEVICEEVENT:
		case SERVICE_CONTROL_HARDWAREPROFILECHANGE:
			break;
		case SERVICE_CONTROL_POWEREVENT:
			{
				printfEx(MOD_SERVICE, PRINTF_LEVEL_INFORMATION, "SERVICE_CONTROL_POWEREVENT");
				break;
			}
		case SERVICE_CONTROL_SESSIONCHANGE:
			break;
		case SERVICE_CONTROL_PRESHUTDOWN:
			{
				printfEx(MOD_SERVICE, PRINTF_LEVEL_INFORMATION, "SERVICE_CONTROL_PRESHUTDOWN");

				if (!ms_UnloadMod())
					printfEx(MOD_SERVICE, PRINTF_LEVEL_ERROR, "[SERVICE_CONTROL_PRESHUTDOWN] ms_UnloadMod failed");

				break;
			}
		case SERVICE_CONTROL_TIMECHANGE:
		case SERVICE_CONTROL_TRIGGEREVENT:
			break;
		default:
			{
				printfEx(MOD_SERVICE, PRINTF_LEVEL_ERROR, "dwControl error. 0x%08x", dwControl);
				__leave;
			}
		}
	}
	__finally
	{
		;
	}

	return dwRet;
}

//
// Purpose: 
//   Sets the current service status and reports it to the SCM.
//
// Parameters:
//   dwCurrentState - The current state (see SERVICE_STATUS)
//   dwWin32ExitCode - The system error code
//   dwWaitHint - Estimated time for pending operation, 
//     in milliseconds
// 
// Return value:
//   None
//
VOID
	CService::ReportSvcStatus(
	DWORD dwCurrentState,
	DWORD dwWin32ExitCode,
	DWORD dwWaitHint
	)
{
	__try
	{
		// Fill in the SERVICE_STATUS structure.
		ms_SvcStatus.dwCurrentState = dwCurrentState;
		ms_SvcStatus.dwWin32ExitCode = dwWin32ExitCode;
		ms_SvcStatus.dwWaitHint = dwWaitHint;

		if (SERVICE_START_PENDING == dwCurrentState)
			ms_SvcStatus.dwControlsAccepted = 0;
		else
			ms_SvcStatus.dwControlsAccepted =
			SERVICE_ACCEPT_STOP                  |  // 0x00000001
			SERVICE_ACCEPT_PAUSE_CONTINUE        |  // 0x00000002
			SERVICE_ACCEPT_SHUTDOWN              |  // 0x00000004
			SERVICE_ACCEPT_PARAMCHANGE           |  // 0x00000008
			SERVICE_ACCEPT_NETBINDCHANGE         |  // 0x00000010
			SERVICE_ACCEPT_HARDWAREPROFILECHANGE |  // 0x00000020
			SERVICE_ACCEPT_POWEREVENT            |  // 0x00000040
			SERVICE_ACCEPT_SESSIONCHANGE         |  // 0x00000080
			SERVICE_ACCEPT_PRESHUTDOWN           |  // 0x00000100
			SERVICE_ACCEPT_TIMECHANGE            |  // 0x00000200
			SERVICE_ACCEPT_TRIGGEREVENT;            // 0x00000400

		if ((SERVICE_RUNNING == dwCurrentState) || (SERVICE_STOPPED == dwCurrentState))
			ms_SvcStatus.dwCheckPoint = 0;
		else
			ms_SvcStatus.dwCheckPoint = ms_dwCheckPoint++;

		// Report the status of the service to the SCM.
		SetServiceStatus(ms_SvcStatusHandle, &ms_SvcStatus);
	}
	__finally
	{
		;
	}

	return ;
}

//
// Purpose: 
//   Entry point for the service
//
// Parameters:
//   dwArgc - Number of arguments in the lpszArgv array
//   lpszArgv - Array of strings. The first string is the name of
//     the service and subsequent strings are passed by the process
//     that called the StartService function to start the service.
// 
// Return value:
//   None.
//
VOID
	WINAPI
	CService::Main(
	DWORD		dwArgc,
	LPTSTR *	lpszArgv
	)
{
	HANDLE			hDataMgr			=	NULL;
	HANDLE			hRpcServer			=	NULL;
	CHAR			DbPath[MAX_PATH]	=	{0};


	__try
	{
		printfEx(MOD_SERVICE, PRINTF_LEVEL_INFORMATION, "begin");

		// Register the handler function for the service
		ms_SvcStatusHandle = RegisterServiceCtrlHandlerEx(ms_tchServiceName, CtrlHandler, NULL);
		if (!ms_SvcStatusHandle )
		{
			printfEx(MOD_SERVICE, PRINTF_LEVEL_ERROR, "RegisterServiceCtrlHandler failed. (%d)", GetLastError());
			__leave;
		}

		// These SERVICE_STATUS members remain as set here
		ms_SvcStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
		ms_SvcStatus.dwServiceSpecificExitCode = 0;

		// Report initial status to the SCM
		ReportSvcStatus(SERVICE_START_PENDING, NO_ERROR, 3000);

		// Perform service-specific initialization and work.
		if (!Init(dwArgc, lpszArgv))
		{
			printfEx(MOD_SERVICE, PRINTF_LEVEL_ERROR, "Init failed");
			__leave;
		}
	}
	__finally
	{
		printfEx(MOD_SERVICE, PRINTF_LEVEL_INFORMATION, "end");
	}

	return ;
}

//
// Purpose: 
//   The service code
//
// Parameters:
//   dwArgc - Number of arguments in the lpszArgv array
//   lpszArgv - Array of strings. The first string is the name of
//     the service and subsequent strings are passed by the process
//     that called the StartService function to start the service.
// 
// Return value:
//   None
//
BOOL
	CService::Init(
	DWORD		dwArgc,
	LPTSTR *	lpszArgv
	)
{
	BOOL bRet = FALSE;


	__try
	{
		printfEx(MOD_SERVICE, PRINTF_LEVEL_INFORMATION, "begin");

		// TO_DO: Declare and set any required variables.
		//   Be sure to periodically call ReportSvcStatus() with 
		//   SERVICE_START_PENDING. If initialization fails, call
		//   ReportSvcStatus with SERVICE_STOPPED.

		// Create an event. The control handler function, SvcCtrlHandler,
		// signals this event when it receives the stop control code.
		ms_hSvcStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		if (!ms_hSvcStopEvent)
		{
			printfEx(MOD_SERVICE, PRINTF_LEVEL_ERROR, "CreateEvent failed. (%d)", GetLastError());
			ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);
			__leave;
		}

		// Report running status when initialization is complete.
		ReportSvcStatus(SERVICE_RUNNING, NO_ERROR, 0);

		// TO_DO: Perform work until service stops.
		if (!ms_InitMod(&ms_InitModArguments))
		{
			printfEx(MOD_SERVICE, PRINTF_LEVEL_ERROR, "ms_Test failed");
			__leave;
		}






		// Check whether to stop the service.
		printfEx(MOD_SERVICE, PRINTF_LEVEL_INFORMATION, "开始等待");
		WaitForSingleObject(ms_hSvcStopEvent, INFINITE);
		printfEx(MOD_SERVICE, PRINTF_LEVEL_INFORMATION, "等待成功");

		ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);
		bRet = TRUE;
	}
	__finally
	{
		printfEx(MOD_SERVICE, PRINTF_LEVEL_INFORMATION, "end");
	}

	return bRet;
}
