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
			printfPublic("input arguments error. 0x%p %d", lpOutBuf, ulOutBufSizeCh);
			__leave;
		}

		ZeroMemory(lpOutBuf, ulOutBufSizeCh * sizeof(TCHAR));

		if (!hModule)
		{
			hModule = GetModuleHandle(NULL);
			if (!hModule)
			{
				printfPublic("GetModuleHandle failed. (%d)", GetLastError());
				__leave;
			}
		}

		dwResult = GetModuleFileName(hModule, lpOutBuf, ulOutBufSizeCh);
		if (!dwResult)
		{
			printfPublic("GetModuleFileName failed. (%d)", GetLastError());
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

CModulePath::CModulePath()
{
	;
}

CModulePath::~CModulePath()
{
	;
}
