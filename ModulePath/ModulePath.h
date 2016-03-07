#pragma once

#include <Windows.h>
#include <stdio.h>

#include "..\\Public.h"

#define MOD_MODULE_PATH _T("Ä£¿éÂ·¾¶")

class CModulePath
{
public:
	static
		BOOL
		Get(
		__in_opt	HMODULE	hModule,
		__out		LPTSTR	lpOutBuf,
		__in		ULONG	ulOutBufSizeCh
		);

private:
	CModulePath();

	~CModulePath();
};
