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
			printf("input arguments error. 0x%08p %d \n", lpOutBuf, ulOutBufSizeCh);
			__leave;
		}

		ZeroMemory(lpOutBuf, ulOutBufSizeCh * sizeof(TCHAR));

		if (!hModule)
		{
			hModule = GetModuleHandle(NULL);
			if (!hModule)
			{
				printf("GetModuleHandle failed. (%d) \n", GetLastError());
				__leave;
			}
		}

		dwResult = GetModuleFileName(hModule, lpOutBuf, ulOutBufSizeCh);
		if (!dwResult)
		{
			printf("GetModuleFileName failed. (%d) \n", GetLastError());
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
