#pragma once

#include <Windows.h>

#include "../../Public/PrintfEx/PrintfEx.h"

#define MOD_MODULE_PATH _T("ģ��·��")

class CModulePath
{
public:
	static
		BOOL
		Get(
		__in_opt	HMODULE	hModule,
		__in		LPTSTR	lpInBuf,
		__in		ULONG	ulInBufSizeCh
		);
};
