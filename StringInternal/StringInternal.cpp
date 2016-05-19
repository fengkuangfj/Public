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
			printfPublic("input arguments error. lpInBuf(0x%p) lpOutBuf(0x%p) ulOutBufSizeCh(%d)", lpInBuf, lpOutBuf, ulOutBufSizeCh);
			__leave;
		}

		ulSizeCh = MultiByteToWideChar(CP_ACP, 0, lpInBuf, -1, NULL, 0);
		if (!ulSizeCh)
		{
			printfPublic("pre MultiByteToWideChar failed. (%d)", GetLastError());
			__leave;
		}

		lpTemp = (LPTSTR)calloc(1, ulSizeCh * sizeof(TCHAR));
		if (!lpTemp)
		{
			printfPublic("calloc failed. (%d)", GetLastError());
			__leave;
		}

		ulSizeCh = MultiByteToWideChar(CP_ACP, 0, lpInBuf, -1, lpTemp, ulSizeCh);
		if (!ulSizeCh)
		{
			printfPublic("post MultiByteToWideChar failed. (%d)", GetLastError());
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

BOOL
CStringInternal::UNICODEToASCII(
__out	LPSTR	lpOutBuf,
__in	ULONG	ulOutBufSizeCh,
__in	LPTSTR	lpInBuf
)
{
	BOOL	bRet = FALSE;

	ULONG	ulSizeCh = 0;
	LPSTR	lpTemp = NULL;


	__try
	{
		if (!lpInBuf || !lpOutBuf || !ulOutBufSizeCh)
		{
			printfPublic("input arguments error. lpInBuf(0x%p) lpOutBuf(0x%p) ulOutBufSizeCh(%d)", lpInBuf, lpOutBuf, ulOutBufSizeCh);
			__leave;
		}

		ulSizeCh = WideCharToMultiByte(CP_ACP, 0, lpInBuf, -1, NULL, 0, NULL, NULL);
		if (!ulSizeCh)
		{
			printfPublic("pre WideCharToMultiByte failed. (%d)", GetLastError());
			__leave;
		}

		lpTemp = (LPSTR)calloc(1, ulSizeCh * sizeof(CHAR));
		if (!lpTemp)
		{
			printfPublic("calloc failed. (%d)", GetLastError());
			__leave;
		}

		ulSizeCh = WideCharToMultiByte(CP_ACP, 0, lpInBuf, -1, lpTemp, ulSizeCh, NULL, NULL);
		if (!ulSizeCh)
		{
			printfPublic("post WideCharToMultiByte failed. (%d)", GetLastError());
			__leave;
		}

		strcat_s(lpOutBuf, ulOutBufSizeCh, lpTemp);

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

CStringInternal::CStringInternal()
{
	;
}

CStringInternal::~CStringInternal()
{
	;
}
