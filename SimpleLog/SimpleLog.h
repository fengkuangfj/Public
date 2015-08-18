#pragma once

#include <Windows.h>
#include <tchar.h>
#include <Strsafe.h>
#include <time.h>
#include <string.h>
#include <Shlobj.h>

#define _LOG_MOD_	_T("»’÷æ")

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
		__in								LPTSTR	lpMod,
		__in								LPCSTR	lpFuncName,
		__in_z __drv_formatString(printf)	LPTSTR	Fmt,
		...
		);

private:
	static TCHAR			ms_LogPath[MAX_PATH];
	static CRITICAL_SECTION	ms_CriticalSection;
	static BOOL				ms_Ready;
};
