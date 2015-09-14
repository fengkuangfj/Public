#pragma once

#define MOD_STRING_INTERNAL _T("ÄÚ²¿×Ö·û´®")

#include <Windows.h>
#include <tchar.h>

#include "..\\PrintfEx\\PrintfEx.h"

class CStringInternal
{
public:
	static
	BOOL
		ASCIIToUNICODE(
		__in	LPSTR	lpInBuf,
		__out	LPTSTR	lpOutBuf,
		__in	ULONG	ulOutBufSizeCh
		);
};
