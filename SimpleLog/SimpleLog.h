#pragma once

#ifndef MOD_SIMPLE_LOG
#define MOD_SIMPLE_LOG	_T("ºÚµ•»’÷æ")
#endif

#include <Windows.h>
#include <tchar.h>
#include <Strsafe.h>
#include <locale.h>
#include <assert.h>
#include <time.h>
#include <Shlobj.h>
#include <Wtsapi32.h>

#include "..\\ProcessPath\\ProcessPath.h"
#include "..\\StackBacktrace\\StackBacktrace.h"
#include "..\\ProcessType\\ProcessType.h"

#pragma comment(lib, "Wtsapi32.lib")

typedef enum _LOG_LEVEL
{
	LOG_LEVEL_INFORMATION,
	LOG_LEVEL_WARNING,
	LOG_LEVEL_ERROR
} LOG_LEVEL, *PLOG_LEVEL, *LPLOG_LEVEL;

#define CSimpleLogSR(lpMod, PrintfLevel, FMT, ...) CSimpleLog::GetInstance()->Log(lpMod, PrintfLevel, __FILE__, __FUNCSIG__, __LINE__, FMT, __VA_ARGS__)

class CSimpleLog
{
public:
	static
		CSimpleLog *
		GetInstance();

	static
		VOID
		ReleaseInstance();

	BOOL
		Init(
		__in LPTSTR lpLogPath
		);

	BOOL
		Unload();

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
	static CSimpleLog	*	ms_pInstance;

	TCHAR					ms_LogPath[MAX_PATH];
	CRITICAL_SECTION		ms_CriticalSection;
	BOOL					ms_WriteReady;
	BOOL					ms_bOutputDebugString;
	PROC_TYPE				ms_ProcType;

	CSimpleLog();

	~CSimpleLog();

	BOOL
		Write(
		__in LPSTR lpLog
		);

	BOOL
		MessageBoxForService(
		__in LPTSTR lpTitle,
		__in LPTSTR lpMessage,
		__in DWORD	dwStyle
		);
};
