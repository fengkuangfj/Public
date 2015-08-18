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

		if (!hModule)
		{
			printfEx(MOD_MODULE_PATH, PRINTF_LEVEL_INFORMATION, "the file used to create the calling process");
			hModule = GetModuleHandle(NULL);
			if (!hModule)
			{
				printfEx(MOD_MODULE_PATH, PRINTF_LEVEL_ERROR, "GetModuleHandle failed. (%d)", GetLastError());
				__leave;
			}
		}
		else
			printfEx(MOD_MODULE_PATH, PRINTF_LEVEL_INFORMATION, "the fully qualified path of the module");

		dwResult = GetModuleFileName(hModule, lpOutBuf, ulOutBufSizeCh);
		if (!dwResult)
		{
			printfEx(MOD_MODULE_PATH, PRINTF_LEVEL_ERROR, "GetModuleFileName failed. (%d)", GetLastError());
			__leave;
		}
		else
			printfEx(MOD_MODULE_PATH, PRINTF_LEVEL_INFORMATION, "%S", lpOutBuf);

		bRet = TRUE;
	}
	__finally
	{
		;
	}

	return bRet;
}
