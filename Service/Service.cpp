#include "Service.h"

WOW64_DISABLE_WOW64_FS_REDIRECTION	g_Wow64DisableWow64FsRedirection	= NULL;
WOW64_REVERT_WOW64_FS_REDIRECTION	g_Wow64RevertWow64FsRedirection		= NULL;

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
