#pragma once

#define MOD_STRING_INTERNAL _T("ÄÚ²¿×Ö·û´®")

#include <Windows.h>
#include <tchar.h>

#include "..\\Public.h"

class CStringInternal
{
public:
	static
		BOOL
		ASCIIToUNICODE(
		__out	LPTSTR	lpOutBuf,
		__inout	PULONG	pulOutBufSizeCh,
		__in	LPSTR	lpInBuf,
		__in	ULONG	ulInBufSizeCh,
		__in	UINT	CodePage
		);

	static
		BOOL
		UNICODEToASCII(
		__out	LPSTR	lpOutBuf,
		__inout	PULONG	pulOutBufSizeCh,
		__in	LPTSTR	lpInBuf,
		__in	ULONG	ulInBufSizeCh,
		__in	UINT	CodePage
		);

	static
		BOOL
		UTF8ToMB(
		__out	LPSTR	lpOutBuf,
		__inout	PULONG	pulOutBufSizeCh,
		__in	LPSTR	lpInBuf,
		__in	ULONG	ulInBufSizeCh
		);

	static
		BOOL
		Equal(
		__in LPSTR lpSrc,
		__in LPSTR lpDes
		);

	static
		LPSTR
		Find(
		__in LPSTR lpSrc,
		__in ULONG ulSrcSizeCh,
		__in LPSTR lpDes,
		__in ULONG ulDesSizeCh
		);

private:
	CStringInternal();

	~CStringInternal();
};
