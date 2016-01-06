#include "ProcessPrivilege.h"

BOOL
	CProcessPrivilege::Adjust(
	__in BOOL	bCurrentProcess,
	__in ULONG	ulPid,
	__in LPTSTR lpName
	)
{
	BOOL				bRet			= FALSE;

	HANDLE				hProcess		= NULL;
	HANDLE				hToken			= NULL;
	TOKEN_PRIVILEGES	TokenPrivileges = {0};


	__try
	{
		if (!bCurrentProcess && !ulPid)
		{
			printfEx(MOD_PROCESS_PRIVILEGE, PRINTF_LEVEL_ERROR, "input arguments error");
			__leave;
		}

		if (bCurrentProcess)
			hProcess = GetCurrentProcess();
		else
		{
			hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ulPid);
			if (!hProcess)
			{
				printfEx(MOD_PROCESS_PRIVILEGE, PRINTF_LEVEL_ERROR, "OpenProcess failed. (%d)", GetLastError());
				__leave;
			}
		}

		if (!OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES, &hToken))
		{
			printfEx(MOD_PROCESS_PRIVILEGE, PRINTF_LEVEL_ERROR, "OpenProcessToken failed. (%d)", GetLastError());
			__leave;
		}

		if (!LookupPrivilegeValue(NULL, lpName, &TokenPrivileges.Privileges[0].Luid))
		{
			printfEx(MOD_PROCESS_PRIVILEGE, PRINTF_LEVEL_ERROR, "LookupPrivilegeValue failed. (%d)", GetLastError());
			__leave;
		}

		TokenPrivileges.PrivilegeCount = 1;
		TokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		if (!AdjustTokenPrivileges(hToken, FALSE, &TokenPrivileges, sizeof(TokenPrivileges), NULL, NULL))
		{
			printfEx(MOD_PROCESS_PRIVILEGE, PRINTF_LEVEL_ERROR, "AdjustTokenPrivileges failed. (%d)", GetLastError());
			__leave;
		}

		bRet = TRUE;
	}
	__finally
	{
		if (hToken)
		{
			CloseHandle(hToken);
			hToken = NULL;
		}
	}

	return bRet;
}

BOOL
	CProcessPrivilege::RunAs(
	__in LPTSTR lpPath
	)
{
	BOOL				bRet				= FALSE;

	SHELLEXECUTEINFO	ShellExecuteInfo	= {0};


	__try
	{
		if (!lpPath)
		{
			printfEx(MOD_PROCESS_PRIVILEGE, PRINTF_LEVEL_ERROR, "input argument error");
			__leave;
		}

		ShellExecuteInfo.cbSize = sizeof(ShellExecuteInfo);
		ShellExecuteInfo.lpVerb = _T("runas");
		ShellExecuteInfo.lpFile = lpPath;
		ShellExecuteInfo.nShow = SW_SHOWNORMAL;

		if (!ShellExecuteEx(&ShellExecuteInfo))
		{
			printfEx(MOD_PROCESS_PRIVILEGE, PRINTF_LEVEL_ERROR, "ShellExecuteEx failed. (%d)", GetLastError());
			__leave;
		}

		bRet = TRUE;
	}
	__finally
	{
		;
	}

	return bRet;
}

BOOL
	CProcessPrivilege::Raise(
	__in BOOL	bCurrentProcess,
	__in ULONG	ulPid
	)
{
	BOOL bRet = FALSE;


	__try
	{
		if (!bCurrentProcess && !ulPid)
		{
			printfEx(MOD_PROCESS_PRIVILEGE, PRINTF_LEVEL_ERROR, "input arguments error");
			__leave;
		}

		Adjust(bCurrentProcess, ulPid, SE_CREATE_TOKEN_NAME          );
		Adjust(bCurrentProcess, ulPid, SE_ASSIGNPRIMARYTOKEN_NAME    );
		Adjust(bCurrentProcess, ulPid, SE_LOCK_MEMORY_NAME           );
		Adjust(bCurrentProcess, ulPid, SE_INCREASE_QUOTA_NAME        );
		// Adjust(bCurrentProcess, ulPid, SE_UNSOLICITED_INPUT_NAME     );
		Adjust(bCurrentProcess, ulPid, SE_MACHINE_ACCOUNT_NAME       );
		Adjust(bCurrentProcess, ulPid, SE_TCB_NAME                   );
		Adjust(bCurrentProcess, ulPid, SE_SECURITY_NAME              );
		Adjust(bCurrentProcess, ulPid, SE_TAKE_OWNERSHIP_NAME        );
		Adjust(bCurrentProcess, ulPid, SE_LOAD_DRIVER_NAME           );
		Adjust(bCurrentProcess, ulPid, SE_SYSTEM_PROFILE_NAME        );
		Adjust(bCurrentProcess, ulPid, SE_SYSTEMTIME_NAME            );
		Adjust(bCurrentProcess, ulPid, SE_PROF_SINGLE_PROCESS_NAME   );
		Adjust(bCurrentProcess, ulPid, SE_INC_BASE_PRIORITY_NAME     );
		Adjust(bCurrentProcess, ulPid, SE_CREATE_PAGEFILE_NAME       );
		Adjust(bCurrentProcess, ulPid, SE_CREATE_PERMANENT_NAME      );
		Adjust(bCurrentProcess, ulPid, SE_BACKUP_NAME                );
		Adjust(bCurrentProcess, ulPid, SE_RESTORE_NAME               );
		Adjust(bCurrentProcess, ulPid, SE_SHUTDOWN_NAME              );
		Adjust(bCurrentProcess, ulPid, SE_DEBUG_NAME                 );
		Adjust(bCurrentProcess, ulPid, SE_AUDIT_NAME                 );
		Adjust(bCurrentProcess, ulPid, SE_SYSTEM_ENVIRONMENT_NAME    );
		Adjust(bCurrentProcess, ulPid, SE_CHANGE_NOTIFY_NAME         );
		Adjust(bCurrentProcess, ulPid, SE_REMOTE_SHUTDOWN_NAME       );
		Adjust(bCurrentProcess, ulPid, SE_UNDOCK_NAME                );
		Adjust(bCurrentProcess, ulPid, SE_SYNC_AGENT_NAME            );
		Adjust(bCurrentProcess, ulPid, SE_ENABLE_DELEGATION_NAME     );
		Adjust(bCurrentProcess, ulPid, SE_MANAGE_VOLUME_NAME         );
		Adjust(bCurrentProcess, ulPid, SE_IMPERSONATE_NAME           );
		Adjust(bCurrentProcess, ulPid, SE_CREATE_GLOBAL_NAME         );
		Adjust(bCurrentProcess, ulPid, SE_TRUSTED_CREDMAN_ACCESS_NAME);
		Adjust(bCurrentProcess, ulPid, SE_RELABEL_NAME               );
		Adjust(bCurrentProcess, ulPid, SE_INC_WORKING_SET_NAME       );
		Adjust(bCurrentProcess, ulPid, SE_TIME_ZONE_NAME             );
		Adjust(bCurrentProcess, ulPid, SE_CREATE_SYMBOLIC_LINK_NAME  );

		bRet = TRUE;
	}
	__finally
	{
		;
	}

	return bRet;
}

CProcessPrivilege::CProcessPrivilege()
{
	;
}

CProcessPrivilege::~CProcessPrivilege()
{
	;
}
