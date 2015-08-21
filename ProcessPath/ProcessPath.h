#pragma once

#include <Windows.h>
#include <tchar.h>
#include <Psapi.h>

#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "Kernel32.lib")

#include "..\\PrintfEx\\PrintfEx.h"
#include "..\\ModulePath\\ModulePath.h"

#define MOD_PROCESS_PATH _T("½ø³ÌÂ·¾¶")

class CProcessPath
{
public:
	static
		BOOL
		Get(
		__in	BOOL	bCurrentProc,
		__in	ULONG	ulPid,
		__out	LPTSTR	lpOutBuf,
		__in	ULONG	ulOutBufSizeCh
		);
};
