#include "ModulePath.h"

BOOL
	CModulePath::Get(
	__in_opt	HMODULE	hModule,
	__out		LPTSTR	lpOutBuf,
	__in		ULONG	ulOutBufSizeCh
	)
{
	BOOL	bRet		= FALSE;

	DWORD	dwResult	= 0;


	__try
	{
		if (!lpOutBuf || !ulOutBufSizeCh)
		{
			printfEx(MOD_MODULE_PATH, PRINTF_LEVEL_ERROR, "input arguments error. 0x%08p %d", lpOutBuf, ulOutBufSizeCh);
			__leave;
		}

		ZeroMemory(lpOutBuf, ulOutBufSizeCh * sizeof(TCHAR));

		if (!hModule)
		{
			hModule = GetModuleHandle(NULL);
			if (!hModule)
			{
				printfEx(MOD_MODULE_PATH, PRINTF_LEVEL_ERROR, "GetModuleHandle failed. (%d)", GetLastError());
				__leave;
			}
		}

		dwResult = GetModuleFileName(hModule, lpOutBuf, ulOutBufSizeCh);
		if (!dwResult)
		{
			printfEx(MOD_MODULE_PATH, PRINTF_LEVEL_ERROR, "GetModuleFileName failed. (%d)", GetLastError());
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
