#include "StringInternal.h"

BOOL
	CStringInternal::ASCIIToUNICODE(
	__out	LPTSTR	lpOutBuf,
	__in	ULONG	ulOutBufSizeCh,
	__in	LPSTR	lpInBuf
	)
{
	BOOL	bRet		= FALSE;

	ULONG	ulSizeCh	= 0;
	LPTSTR	lpTemp		= NULL;


	__try
	{
		if (!lpInBuf || !lpOutBuf || !ulOutBufSizeCh)
		{
			printfEx(MOD_STRING_INTERNAL, PRINTF_LEVEL_ERROR, "input arguments error. 0x%08p 0x%08p %d", lpInBuf, lpOutBuf, ulOutBufSizeCh);
			__leave;
		}

		ulSizeCh = MultiByteToWideChar(CP_ACP, 0, lpInBuf, -1, NULL, 0);
		if (!ulSizeCh)
		{
			printfEx(MOD_STRING_INTERNAL, PRINTF_LEVEL_ERROR, "pre MultiByteToWideChar failed. (%d)", GetLastError());
			__leave;
		}

		lpTemp = (LPTSTR)calloc(1, ulSizeCh * sizeof(TCHAR));
		if (!lpTemp)
		{
			printfEx(MOD_STRING_INTERNAL, PRINTF_LEVEL_ERROR, "calloc failed. (%d)", GetLastError());
			__leave;
		}

		ulSizeCh = MultiByteToWideChar(CP_ACP, 0, lpInBuf, -1, lpTemp, ulSizeCh);
		if (!ulSizeCh)
		{
			printfEx(MOD_STRING_INTERNAL, PRINTF_LEVEL_ERROR, "post MultiByteToWideChar failed. (%d)", GetLastError());
			__leave;
		}

		_tcscat_s(lpOutBuf, ulOutBufSizeCh, lpTemp);

		bRet = TRUE;
	}
	__finally
	{
		if (lpTemp)
		{
			free(lpTemp);
			lpTemp = NULL;
		}
	}

	return bRet;
}
