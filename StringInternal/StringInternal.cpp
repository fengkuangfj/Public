#include "StringInternal.h"

BOOL
CStringInternal::ASCIIToUNICODE(
								__out	LPTSTR	lpOutBuf,
								__inout	PULONG	pulOutBufSizeCh,
								__in	LPSTR	lpInBuf,
								__in	ULONG	ulInBufSizeCh,
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

		ulSizeCh = MultiByteToWideChar(CodePage, 0, lpInBuf, ulInBufSizeCh * sizeof(CHAR), NULL, 0);
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

		ulSizeCh = MultiByteToWideChar(CodePage, 0, lpInBuf, ulInBufSizeCh * sizeof(CHAR), lpTemp, ulSizeCh);
		if (!ulSizeCh)
		{
			printfPublic("post MultiByteToWideChar failed. (%d)", GetLastError());
			__leave;
		}

		memcpy(lpOutBuf, lpTemp, ulSizeCh * sizeof(TCHAR));

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
								__in	ULONG	ulInBufSizeCh,
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

		ulSizeCh = WideCharToMultiByte(CodePage, 0, lpInBuf, ulInBufSizeCh * sizeof(TCHAR), NULL, 0, NULL, NULL);
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

		ulSizeCh = WideCharToMultiByte(CodePage, 0, lpInBuf, ulInBufSizeCh * sizeof(TCHAR), lpTemp, ulSizeCh, NULL, NULL);
		if (!ulSizeCh)
		{
			printfPublic("post WideCharToMultiByte failed. (%d)", GetLastError());
			__leave;
		}

		memcpy(lpOutBuf, lpTemp, ulSizeCh * sizeof(CHAR));

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
						  __in	LPSTR	lpInBuf,
						  __in	ULONG	ulInBufSizeCh
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

		if (!ASCIIToUNICODE(lpBufTmp, &ulSizeCh, lpInBuf, ulInBufSizeCh, CP_UTF8))
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

			if (!ASCIIToUNICODE(lpBufTmp, &ulSizeCh, lpInBuf, ulInBufSizeCh, CP_UTF8))
			{
				printfPublic("ASCIIToUNICODE failed");
				__leave;
			}
		}

		if (!UNICODEToASCII(lpOutBuf, pulOutBufSizeCh, lpBufTmp, ulSizeCh, CP_ACP))
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

BOOL
CStringInternal::Equal(
					   __in LPSTR lpSrc,
					   __in LPSTR lpDes
					   )
{
	BOOL bRet = FALSE;


	__try
	{
		if (!lpSrc || !lpDes)
			__leave;

		if (*lpSrc == *lpDes)
			bRet = TRUE;
		else
		{
			if ('a' <= *lpSrc && 'z' >= *lpSrc)
			{
				if ('a' <= *lpDes && 'z' >= *lpDes)
				{
					if (*lpSrc == *lpDes)
						bRet = TRUE;
				}
				else if ('A' <= *lpDes && 'Z' >= *lpDes)
				{
					if (*lpSrc == *lpDes + 'a' - 'A')
						bRet = TRUE;
				}
			}
			else if ('A' <= *lpSrc && 'Z' >= *lpSrc)
			{
				if ('a' <= *lpDes && 'z' >= *lpDes)
				{
					if (*lpSrc + 'a' - 'A' == *lpDes)
						bRet = TRUE;
				}
				else if ('A' <= *lpDes && 'Z' >= *lpDes)
				{
					if (*lpSrc == *lpDes)
						bRet = TRUE;
				}
			}
		}
	}
	__finally
	{
		;
	}

	return bRet;
}

LPSTR
CStringInternal::Find(
					  __in LPSTR lpSrc,
					  __in ULONG ulSrcSizeCh,
					  __in LPSTR lpDes,
					  __in ULONG ulDesSizeCh
					  )
{
	LPSTR	lpRet = NULL;

	ULONG	i = 0;
	ULONG	j = 0;


	__try
	{
		if (!lpSrc || !ulSrcSizeCh || !lpDes || !ulDesSizeCh)
			__leave;

		for (; i < ulSrcSizeCh; i++)
		{
			if (!Equal(lpSrc + i, lpDes))
				continue;

			for (j = 1; j < ulDesSizeCh; j++)
			{
				if (!Equal(lpSrc + i + j, lpDes + j))
					continue;
			}

			if (j >= ulDesSizeCh)
			{
				lpRet = lpSrc + i;
				__leave;
			}
		}
	}
	__finally
	{
		;
	}

	return lpRet;
}
