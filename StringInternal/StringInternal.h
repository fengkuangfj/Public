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
		__in	ULONG	ulOutBufSizeCh,
		__in	LPSTR	lpInBuf
		);

	static
		BOOL
		UNICODEToASCII(
		__out	LPSTR	lpOutBuf,
		__in	ULONG	ulOutBufSizeCh,
		__in	LPTSTR	lpInBuf
		);

private:
	CStringInternal();

	~CStringInternal();
};
