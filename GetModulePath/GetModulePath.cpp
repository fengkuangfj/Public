#include "GetModulePath.h"

BOOL
	CGetModulePath::GetModulePath(
	__in HMODULE	hModule,
	__in LPTSTR		lpInBuf,
	__in ULONG		ulInBufSizeCh
	)
{
	BOOL	bRet		= FALSE;

	DWORD	dwResult	= 0;


	printfEx("begin");

	__try
	{
		if (!lpInBuf || !ulInBufSizeCh)
		{
			printfEx("input arguments error. lpInBuf(0x%08p) ulInBufSizeCh(%d)", lpInBuf, ulInBufSizeCh);
			__leave;
		}

		if (!hModule)
		{
			printfEx("the file used to create the calling process");
			hModule = GetModuleHandle(NULL);
			if (!hModule)
			{
				printfEx("GetModuleHandle failed. (%d)", GetLastError());
				__leave;
			}
		}
		else
			printfEx("the fully qualified path of the module");

		dwResult = GetModuleFileName(hModule, lpInBuf, ulInBufSizeCh);
		if (!dwResult)
		{
			printfEx("GetModuleFileName failed. (%d)", GetLastError());
			__leave;
		}
		else
			printfEx("%S", lpInBuf);

		bRet = TRUE;
	}
	__finally
	{
		;
	}

	printfEx("end");

	return bRet;
}
