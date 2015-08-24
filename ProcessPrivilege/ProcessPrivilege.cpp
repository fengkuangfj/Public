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
