#pragma once

#include <Windows.h>

#include "../../Public/PrintfEx/PrintfEx.h"

#define MOD_GET_MODULE_PATH _T("��ȡģ��·��")

class CGetModulePath
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
