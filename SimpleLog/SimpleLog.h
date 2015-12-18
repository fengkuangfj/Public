#pragma once

#include <Windows.h>
#include <tchar.h>
#include <Strsafe.h>
#include <locale.h>
#include <assert.h>
#include <time.h>
#include <Shlobj.h>

#include "..\\ProcessPath\\ProcessPath.h"
#include "..\\StackBacktrace\\StackBacktrace.h"
#include "..\\ProcessType\\ProcessType.h"

#ifndef MOD_SIMPLE_LOG
#define MOD_SIMPLE_LOG	_T("ºÚµ•»’÷æ")
#endif

typedef enum _LOG_LEVEL
{
	LOG_LEVEL_INFORMATION,
	LOG_LEVEL_WARNING,
	LOG_LEVEL_ERROR
} LOG_LEVEL, *PLOG_LEVEL, *LPLOG_LEVEL;

#define CSimpleLogSR(lpMod, PrintfLevel, FMT, ...) CSimpleLog::Log(lpMod, PrintfLevel, __FILE__, __FUNCSIG__, __LINE__, FMT, __VA_ARGS__)

class CSimpleLog
{
public:
	BOOL
		Init(
		__in LPTSTR lpLogPath
		);

	BOOL
		Unload();

	static
		BOOL
		Log(
		__in LPTSTR		lpMod,
		__in LOG_LEVEL	PrintfLevel,
		__in LPSTR		lpFile,
		__in LPSTR		lpFunction,
		__in ULONG		ulLine,
		__in LPSTR		lpFmt,
		...
		);

private:
	static TCHAR			ms_LogPath[MAX_PATH];
	static CRITICAL_SECTION	ms_CriticalSection;
	static BOOL				ms_WriteReady;
	static BOOL				ms_bOutputDebugString;
	static PROC_TYPE		ms_ProcType;

	BOOL
		Write(
		__in LPSTR lpLog
		);
};
