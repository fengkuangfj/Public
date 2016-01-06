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
		__out	LPTSTR	lpOutBuf,
		__in	ULONG	ulOutBufSizeCh,
		__in	LPSTR	lpInBuf
		);

private:
	CStringInternal();

	~CStringInternal();
};
