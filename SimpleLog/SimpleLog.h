#pragma once

#include <Windows.h>
#include <tchar.h>
#include <Strsafe.h>
#include <locale.h>
#include <assert.h>
#include <time.h>
#include <Shlobj.h>
#include <Wtsapi32.h>

#include "..\\PrintfEx\\PrintfEx.h"
#include "..\\ProcessControl\\ProcessControl.h"
#include "..\\StackBacktrace\\StackBacktrace.h"

#pragma comment(lib, "Wtsapi32.lib")

#ifndef MOD_SIMPLE_LOG
#define MOD_SIMPLE_LOG	_T("¼òµ¥ÈÕÖ¾")
#endif

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
		GetInstance(
		__in LPTSTR lpLogPath = NULL
		);

	static
		VOID
		ReleaseInstance();

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

	TCHAR					m_LogPath[MAX_PATH];
	CRITICAL_SECTION		m_CriticalSection;
	BOOL					m_WriteReady;
	BOOL					m_bOutputDebugString;
	PROC_TYPE				m_ProcType;

	CSimpleLog(
		__in LPTSTR lpLogPath
		);

	~CSimpleLog();

	BOOL
		Init(
		__in LPTSTR lpLogPath
		);

	BOOL
		Unload();

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
