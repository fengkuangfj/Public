#pragma once

#ifndef MOD_MODULE_PATH
#define MOD_MODULE_PATH _T("Ä£¿éÂ·¾¶")
#endif

#include <Windows.h>
#include <stdio.h>

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
