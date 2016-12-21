#pragma once

#include <Windows.h>
#include <stdio.h>
#include <Psapi.h>
#include <Shlwapi.h>

#include "..\\Public.h"

#ifndef MOD_MODULE_PATH
#define MOD_MODULE_PATH _T("Ä£¿éÂ·¾¶")
#endif

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

	static
		BOOL
		Get(
		__in_opt	HMODULE	hModule,
		__out		LPSTR	lpOutBuf,
		__in		ULONG	ulOutBufSizeCh
		);

	static
		BOOL
		Enum();

	static
		BOOL
		GetName(
			__in	DWORD_PTR	dwAddr,
			__out	LPTSTR		lpModuleName,
			__in	ULONG		ulModuleNameBufSizeCh
		);

private:
	CModulePath();

	~CModulePath();
};
