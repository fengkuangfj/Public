#include "StringInternal.h"

BOOL
	CStringInternal::ASCIIToUNICODE(
	__out	LPTSTR	lpOutBuf,
	__inout	PULONG	pulOutBufSizeCh,
	__in	LPSTR	lpInBuf,
	__in	UINT	CodePage
	)
{
	BOOL	bRet		= FALSE;

	ULONG	ulSizeCh	= 0;
	LPTSTR	lpTemp		= NULL;


	__try
	{
		if (!lpInBuf || !pulOutBufSizeCh)
		{
			printfPublic("input arguments error. lpInBuf(0x%p) pulOutBufSizeCh(0x%p)", lpInBuf, pulOutBufSizeCh);
			__leave;
		}

		ulSizeCh = MultiByteToWideChar(CodePage, 0, lpInBuf, -1, NULL, 0);
		if (!ulSizeCh)
		{
			printfPublic("pre MultiByteToWideChar failed. (%d)", GetLastError());
			__leave;
		}

		if (*pulOutBufSizeCh < ulSizeCh || !lpOutBuf)
		{
			*pulOutBufSizeCh = ulSizeCh;
			__leave;
		}

		lpTemp = (LPTSTR)calloc(1, ulSizeCh * sizeof(TCHAR));
		if (!lpTemp)
		{
			printfPublic("calloc failed. (%d)", GetLastError());
			__leave;
		}

		ulSizeCh = MultiByteToWideChar(CodePage, 0, lpInBuf, -1, lpTemp, ulSizeCh);
		if (!ulSizeCh)
		{
			printfPublic("post MultiByteToWideChar failed. (%d)", GetLastError());
			__leave;
		}

		_tcscpy_s(lpOutBuf, *pulOutBufSizeCh, lpTemp);

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
								__inout	PULONG	pulOutBufSizeCh,
								__in	LPTSTR	lpInBuf,
								__in	UINT	CodePage
)
{
	BOOL	bRet = FALSE;

	ULONG	ulSizeCh = 0;
	LPSTR	lpTemp = NULL;


	__try
	{
		if (!lpInBuf || !pulOutBufSizeCh)
		{
			printfPublic("input arguments error. lpInBuf(0x%p) pulOutBufSizeCh(0x%p)", lpInBuf, pulOutBufSizeCh);
			__leave;
		}

		ulSizeCh = WideCharToMultiByte(CodePage, 0, lpInBuf, -1, NULL, 0, NULL, NULL);
		if (!ulSizeCh)
		{
			printfPublic("pre WideCharToMultiByte failed. (%d)", GetLastError());
			__leave;
		}

		if (*pulOutBufSizeCh < ulSizeCh || !lpOutBuf)
		{
			*pulOutBufSizeCh = ulSizeCh;
			__leave;
		}

		lpTemp = (LPSTR)calloc(1, ulSizeCh * sizeof(CHAR));
		if (!lpTemp)
		{
			printfPublic("calloc failed. (%d)", GetLastError());
			__leave;
		}

		ulSizeCh = WideCharToMultiByte(CodePage, 0, lpInBuf, -1, lpTemp, ulSizeCh, NULL, NULL);
		if (!ulSizeCh)
		{
			printfPublic("post WideCharToMultiByte failed. (%d)", GetLastError());
			__leave;
		}

		strcpy_s(lpOutBuf, *pulOutBufSizeCh, lpTemp);

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
CStringInternal::UTF8ToMB(
								__out	LPSTR	lpOutBuf,
								__inout	PULONG	pulOutBufSizeCh,
								__in	LPSTR	lpInBuf
								)
{
	BOOL	bRet = FALSE;

	LPTSTR	lpBufTmp = NULL;
	ULONG	ulSizeCh = 0;


	__try
	{
		if (!pulOutBufSizeCh || !lpInBuf)
		{
			printfPublic("input arguments error. pulOutBufSizeCh(0x%p) lpInBuf(0x%p)", pulOutBufSizeCh, lpInBuf);
			__leave;
		}

		if (!ASCIIToUNICODE(lpBufTmp, &ulSizeCh, lpInBuf, CP_UTF8))
		{
			if (!ulSizeCh)
			{
				printfPublic("ASCIIToUNICODE failed");
				__leave;
			}

			lpBufTmp = (LPTSTR)calloc(1, ulSizeCh * sizeof(TCHAR));
			if (!lpBufTmp)
			{
				printfPublic("calloc failed. (%d)", GetLastError());
				__leave;
			}

			if (!ASCIIToUNICODE(lpBufTmp, &ulSizeCh, lpInBuf, CP_UTF8))
			{
				printfPublic("ASCIIToUNICODE failed");
				__leave;
			}
		}

		if (!UNICODEToASCII(lpOutBuf, pulOutBufSizeCh, lpBufTmp, CP_ACP))
			__leave;

		bRet = TRUE;
	}
	__finally
	{
		if (lpBufTmp)
		{
			free(lpBufTmp);
			lpBufTmp = NULL;
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
