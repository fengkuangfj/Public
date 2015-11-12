#pragma once

#include <Windows.h>
#include <tchar.h>
#include <Strsafe.h>
#include <time.h>
#include <string.h>
#include <Shlobj.h>

#ifndef MOD_SIMPLE_LOG
#define MOD_SIMPLE_LOG	_T("ºÚµ•»’÷æ")
#endif

#define CSimpleLogWrite(lpMod, FMT, ...) CSimpleLog::Write(lpMod, __FUNCTION__, FMT, __VA_ARGS__)

class CSimpleLog
{
public:
	BOOL
		Init(
		__in LPTSTR lpLogPath
		);

	static
		BOOL
		Write(
		__in LPTSTR	lpMod,
		__in LPSTR	lpFuncName,
		__in LPTSTR	lpFmt,
		...
		);

private:
	static TCHAR			ms_LogPath[MAX_PATH];
	static CRITICAL_SECTION	ms_CriticalSection;
	static BOOL				ms_Ready;
};
