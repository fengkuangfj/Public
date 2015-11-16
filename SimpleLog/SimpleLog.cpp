#include "SimpleLog.h"

TCHAR				CSimpleLog::ms_LogPath[MAX_PATH] = { 0 };
CRITICAL_SECTION	CSimpleLog::ms_CriticalSection = { 0 };
BOOL				CSimpleLog::ms_WriteReady = FALSE;

BOOL
CSimpleLog::Init(
__in LPTSTR lpLogPath
)
{
	BOOL			bRet = FALSE;

	HANDLE			hFile = INVALID_HANDLE_VALUE;
	LPTSTR			lpPosition = NULL;
	TCHAR			lpDir[MAX_PATH] = { 0 };

	CStackBacktrace StackBacktrace;


	__try
	{
		if (!lpLogPath)
			__leave;

		if (_tcslen(ms_LogPath))
			__leave;

		_tcscat_s(ms_LogPath, _countof(ms_LogPath), lpLogPath);

		lpPosition = _tcsrchr(ms_LogPath, _T('\\'));
		if (!lpPosition)
			__leave;

		CopyMemory(lpDir, ms_LogPath, (lpPosition - ms_LogPath) * sizeof(TCHAR));

		SHCreateDirectoryEx(NULL, lpDir, NULL);

		if (PathFileExists(ms_LogPath))
		{
			if (!PathYetAnotherMakeUniqueName(ms_LogPath, ms_LogPath, NULL, NULL))
				__leave;
		}

		hFile = CreateFile(
			ms_LogPath,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			CREATE_NEW,
			FILE_ATTRIBUTE_NORMAL,
			NULL
			);
		if (INVALID_HANDLE_VALUE == hFile)
			__leave;

		InitializeCriticalSection(&ms_CriticalSection);

		ms_WriteReady = TRUE;

		setlocale(LC_ALL, "");

		StackBacktrace.Init(lpDir);

		bRet = TRUE;
	}
	__finally
	{
		if (INVALID_HANDLE_VALUE != hFile)
		{
			CloseHandle(hFile);
			hFile = INVALID_HANDLE_VALUE;
		}

		if (bRet)
			CSimpleLogSR(MOD_SIMPLE_LOG, LOG_LEVEL_INFORMATION, "初始化成功");
		else
			Unload();
	}

	return bRet;
}

BOOL
CSimpleLog::Unload()
{
	BOOL			bRet = FALSE;

	CStackBacktrace StackBacktrace;


	__try
	{
		if (!StackBacktrace.Unload())
			__leave;

		bRet = TRUE;
	}
	__finally
	{
		;
	}

	return bRet;
}

BOOL
CSimpleLog::Log(
__in LPTSTR		lpMod,
__in LOG_LEVEL	LogLevel,
__in LPSTR		lpFile,
__in LPSTR		lpFunction,
__in ULONG		ulLine,
__in LPSTR		lpFmt,
...
)
{
	BOOL			bRet = FALSE;

	va_list			Args;

	time_t			rawTime = 0;
	tm				timeInfo = { 0 };
	CHAR			chFmtInfo[MAX_PATH] = { 0 };
	CHAR			chLog[MAX_PATH * 2] = { 0 };
	HANDLE			hOutput = INVALID_HANDLE_VALUE;
	BOOL			bNeedStackBacktrace = TRUE;

	CSimpleLog		SimpleLog;
	CStackBacktrace	StackBacktrace;


	__try
	{
		if (LOG_LEVEL_INFORMATION_STACK_BACKTRACE == LogLevel ||
			LOG_LEVEL_WARNING_STACK_BACKTRACE == LogLevel ||
			LOG_LEVEL_ERROR_STACK_BACKTRACE == LogLevel)
		{
			LogLevel = (LOG_LEVEL)(LogLevel - LOG_LEVEL_STACK_BACKTRACE);
			bNeedStackBacktrace = FALSE;
		}

		va_start(Args, lpFmt);

		time(&rawTime);
		localtime_s(&timeInfo, &rawTime);

		StringCchVPrintfA(chFmtInfo, _countof(chFmtInfo), lpFmt, Args);

		StringCchPrintfA(chLog, _countof(chLog), "%hs[%04d/%02d/%02d][%02d:%02d:%02d][%05d][%05d][%lS][%hs][%d][%hs] %hs ",
			(0 == _tcsicmp(lpMod, MOD_STACK_BACKTRACE)) ? ((LOG_LEVEL_INFORMATION == LogLevel) ? "\t[INFO]" : ((LOG_LEVEL_WARNING == LogLevel) ? "\t[WARN]" : ((LOG_LEVEL_ERROR == LogLevel) ? "\t[ERRO]" : "\t[????]"))) : ((LOG_LEVEL_INFORMATION == LogLevel) ? "[INFO]" : ((LOG_LEVEL_WARNING == LogLevel) ? "[WARN]" : ((LOG_LEVEL_ERROR == LogLevel) ? "[ERRO]" : "[????]"))),
			timeInfo.tm_year + 1900,
			timeInfo.tm_mon + 1,
			timeInfo.tm_mday,
			timeInfo.tm_hour,
			timeInfo.tm_min,
			timeInfo.tm_sec,
			GetCurrentProcessId(),
			GetCurrentThreadId(),
			lpMod ? lpMod : _T("未知模块"),
			lpFile,
			ulLine,
			lpFunction,
			chFmtInfo
			);

		SimpleLog.Write(chLog);

		strcat_s(chLog, _countof(chLog), "\n");

		hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
		if (INVALID_HANDLE_VALUE != hOutput)
		{
			if (hOutput)
				printf("%hs", chLog);
		}

		OutputDebugStringA(chLog);

		if (bNeedStackBacktrace && (LOG_LEVEL_ERROR == LogLevel))
		{
			StackBacktrace.StackBacktrace();

			if (IsDebuggerPresent())
			{
				__asm
				{
					int 3
				}
			}
			else
			{
#ifdef _ASSERT
				VERIFY(FALSE);
#else
				assert(FALSE);
#endif
			}
		}

		bRet = TRUE;
	}
	__finally
	{
		va_end(Args);
	}

	return bRet;
}

BOOL
CSimpleLog::Write(
__in LPSTR lpLog
)
{
	BOOL			bRet = FALSE;

	CHAR			chLog[MAX_PATH * 2] = { 0 };
	LPTSTR			lpPositon = NULL;
	TCHAR			lpDir[MAX_PATH] = { 0 };
	HANDLE			hFile = INVALID_HANDLE_VALUE;
	LARGE_INTEGER	FileSize = { 0 };
	DWORD			dwWrite = 0;


	if (!ms_WriteReady)
		return TRUE;

	__try
	{
		EnterCriticalSection(&CSimpleLog::ms_CriticalSection);

		if (!lpLog)
			__leave;

		StringCchPrintfA(chLog, _countof(chLog), "%hs\r\n", lpLog);

		lpPositon = wcsrchr(CSimpleLog::ms_LogPath, _T('\\'));
		if (!lpPositon)
			__leave;

		CopyMemory(lpDir, ms_LogPath, (lpPositon - CSimpleLog::ms_LogPath) * sizeof(TCHAR));

		SHCreateDirectoryEx(NULL, lpDir, NULL);

		hFile = CreateFile(
			CSimpleLog::ms_LogPath,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL
			);
		if (INVALID_HANDLE_VALUE == hFile)
			__leave;

		FileSize.LowPart = GetFileSize(hFile, NULL);
		if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, FileSize.LowPart, NULL, FILE_BEGIN))
			__leave;

		if (!WriteFile(hFile, chLog, strlen(chLog), &dwWrite, NULL))
			__leave;

		bRet = TRUE;
	}
	__finally
	{
		if (INVALID_HANDLE_VALUE != hFile)
		{
			FlushFileBuffers(hFile);
			CloseHandle(hFile);
			hFile = INVALID_HANDLE_VALUE;
		}

		LeaveCriticalSection(&CSimpleLog::ms_CriticalSection);
	}

	return bRet;
}
