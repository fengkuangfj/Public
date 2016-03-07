#include "Uac.h"

BOOL
	CUac::Set(
	__in UAC_LEVEL UacLevel
	)
{
	BOOL	bRet							= FALSE;

	HKEY	hKey							= NULL;
	LONG	lResult							= 0;
	DWORD	dwConsentPromptBehaviorAdmin	= 0;
	DWORD	dwEnableLUA						= 0;
	DWORD	dwPromptOnSecureDesktop			= 0;


	__try
	{
		lResult = RegOpenKeyEx(
			HKEY_LOCAL_MACHINE,
			_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System"),
			0,
			KEY_ALL_ACCESS,
			&hKey
			);
		if (ERROR_SUCCESS != lResult)
		{
			printfPublic("RegOpenKeyEx failed. (%d) \n", lResult);
			__leave;
		}

		switch (UacLevel)
		{
		case UAC_LEVEL_1:
			{
				// 2 1 1
				dwConsentPromptBehaviorAdmin = 2;
				dwEnableLUA = 1;
				dwPromptOnSecureDesktop = 1;

				break;
			}
		case UAC_LEVEL_2:
			{
				// 5 1 1
				dwConsentPromptBehaviorAdmin = 5;
				dwEnableLUA = 1;
				dwPromptOnSecureDesktop = 1;

				break;
			}
		case UAC_LEVEL_3:
			{
				// 5 1 0
				dwConsentPromptBehaviorAdmin = 5;
				dwEnableLUA = 1;
				dwPromptOnSecureDesktop = 0;

				break;
			}
		case UAC_LEVEL_4:
			{
				// 0 0 0
				dwConsentPromptBehaviorAdmin = 0;
				dwEnableLUA = 0;
				dwPromptOnSecureDesktop = 0;

				break;
			}
		default:
			{
				printfPublic("UacLevel error. (%d) \n", UacLevel);
				__leave;
			}
		}

		lResult = RegSetValueEx(
			hKey,
			_T("ConsentPromptBehaviorAdmin"),
			NULL,
			REG_DWORD,
			(const BYTE*)&dwConsentPromptBehaviorAdmin,
			sizeof(DWORD)
			);
		if (ERROR_SUCCESS != lResult)
		{
			printfPublic("RegSetValueEx failed. ConsentPromptBehaviorAdmin (%d) \n", lResult);
			__leave;
		}

		if (!hKey)
		{
			printfPublic("hKey error \n");
			__leave;
		}

		lResult = RegSetValueEx(
			hKey,
			_T("EnableLUA"),
			NULL,
			REG_DWORD,
			(const BYTE*)&dwEnableLUA,
			sizeof(DWORD)
			);
		if (ERROR_SUCCESS != lResult)
		{
			printfPublic("RegSetValueEx failed. EnableLUA (%d) \n", lResult);
			__leave;
		}

		lResult = RegSetValueEx(
			hKey,
			_T("PromptOnSecureDesktop"),
			NULL,
			REG_DWORD,
			(const BYTE*)&dwPromptOnSecureDesktop,
			sizeof(DWORD)
			);
		if (ERROR_SUCCESS != lResult)
		{
			printfPublic("RegSetValueEx failed. PromptOnSecureDesktop (%d) \n", lResult);
			__leave;
		}

		bRet = TRUE;
	}
	__finally
	{
		if (hKey)
		{
			if (bRet)
				RegFlushKey(hKey);

			RegCloseKey(hKey);
			hKey = NULL;
		}
	}

	return bRet;
}

CUac::CUac()
{
	;
}

CUac::~CUac()
{
	;
}
