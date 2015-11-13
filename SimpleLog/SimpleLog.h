#pragma once

#include <Windows.h>
#include <tchar.h>
#include <Strsafe.h>
#include <locale.h>
#include <assert.h>
#include <time.h>
#include <Shlobj.h>

#include "..\\StackBacktrace\\StackBacktrace.h"

#ifndef MOD_SIMPLE_LOG
#define MOD_SIMPLE_LOG	_T("ºÚµ•»’÷æ")
#endif

typedef enum _LOG_LEVEL
{
	LOG_LEVEL_INFORMATION					= 0x00000001,
	LOG_LEVEL_WARNING						= 0x00000002,
	LOG_LEVEL_ERROR							= 0x00000004,
	LOG_LEVEL_STACK_BACKTRACE				= 0x00000010,
	LOG_LEVEL_INFORMATION_STACK_BACKTRACE	= LOG_LEVEL_STACK_BACKTRACE + LOG_LEVEL_INFORMATION,
	LOG_LEVEL_WARNING_STACK_BACKTRACE		= LOG_LEVEL_STACK_BACKTRACE + LOG_LEVEL_WARNING,
	LOG_LEVEL_ERROR_STACK_BACKTRACE			= LOG_LEVEL_STACK_BACKTRACE + LOG_LEVEL_ERROR
} LOG_LEVEL, *PLOG_LEVEL, *LPLOG_LEVEL;

#define CSimpleLogSR(lpMod, PrintfLevel, FMT, ...) CSimpleLog::Log(lpMod, PrintfLevel, __FILE__, __FUNCTION__, __LINE__, FMT, __VA_ARGS__)

class CSimpleLog
{
public:
	BOOL
		Init(
		__in LPTSTR lpLogPath
		);

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

	BOOL
		Write(
		__in LPSTR lpLog
		);
};
