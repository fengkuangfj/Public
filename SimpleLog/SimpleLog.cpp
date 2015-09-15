#include "SimpleLog.h"

TCHAR				CSimpleLog::ms_LogPath[MAX_PATH]	= {0};
CRITICAL_SECTION	CSimpleLog::ms_CriticalSection		= {0};
BOOL				CSimpleLog::ms_Ready				= FALSE;

BOOL
	CSimpleLog::Init(
	__in LPTSTR lpLogPath
	)
{
	BOOL	bRet			= FALSE;

	HANDLE	hFile			= INVALID_HANDLE_VALUE;
	LPTSTR	lpPosition		= NULL;
	TCHAR	lpDir[MAX_PATH] = {0};


	__try
	{
		if (!lpLogPath)
			__leave;

		if (_tcslen(ms_LogPath))
			__leave;

		_tcscat_s(ms_LogPath, _countof(ms_LogPath), lpLogPath);

		lpPosition = wcsrchr(ms_LogPath, _T('\\'));
		if (!lpPosition)
			__leave;

		CopyMemory(lpDir, ms_LogPath, (lpPosition - ms_LogPath) * sizeof(TCHAR));

		SHCreateDirectoryEx(NULL, lpDir, NULL);

		hFile = CreateFile(
			ms_LogPath,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL
			);
		if (hFile == INVALID_HANDLE_VALUE)
			__leave;

		InitializeCriticalSection(&ms_CriticalSection);

		ms_Ready = TRUE;

		bRet = TRUE;
	}
	__finally
	{
		if (hFile != INVALID_HANDLE_VALUE)
			CloseHandle(hFile);

		if (bRet)
			CSimpleLogWrite(MOD_SIMPLE_LOG, _T("日志模块初始化成功"));
	}

	return bRet;
}

BOOL
	CSimpleLog::Write(
	__in LPTSTR	lpMod,
	__in LPSTR	lpFuncName,
	__in LPTSTR	lpFmt,
	...
	)
{
	BOOL			bRet							= FALSE;

	HANDLE			hFile							= INVALID_HANDLE_VALUE;
	TCHAR			tchLog[MAX_PATH]				= {0};
	TCHAR			Tmp[MAX_PATH]					= {0};
	CHAR			chLog[MAX_PATH * sizeof(TCHAR)] = {0};
	LARGE_INTEGER	FileSize						= {0};
	DWORD			dwWrite							= 0;
	time_t			rawTime							= 0;
	tm				timeInfo						= {0};
	LPTSTR			lpPositon						= NULL;
	TCHAR			lpDir[MAX_PATH]					= {0};
	TCHAR			tchFuncName[MAX_PATH]			= {0};

	va_list			Args;


	if (!ms_Ready)
		return TRUE;

	__try
	{
		EnterCriticalSection(&CSimpleLog::ms_CriticalSection);

		va_start(Args, lpFmt);

		if (!_tcslen(CSimpleLog::ms_LogPath))
			__leave;

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
		if (hFile == INVALID_HANDLE_VALUE)
			__leave;

		FileSize.LowPart = GetFileSize(hFile, NULL);
		if (SetFilePointer(hFile, FileSize.LowPart, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
			__leave;

		// 时间
		time(&rawTime);
		localtime_s(&timeInfo, &rawTime);

		StringCbPrintf(Tmp, sizeof(Tmp), _T("[%04d-%02d-%02d][%02d:%02d:%02d]"),
			timeInfo.tm_year + 1900,
			timeInfo.tm_mon + 1,
			timeInfo.tm_mday,
			timeInfo.tm_hour,
			timeInfo.tm_min,
			timeInfo.tm_sec
			);
		_tcscat_s(tchLog, _countof(tchLog), Tmp);

		// 线程
		StringCbPrintf(Tmp, sizeof(Tmp), _T("[%05d]"), GetCurrentThreadId());
		_tcscat_s(tchLog, _countof(tchLog), Tmp);

		// 模块
		if (lpMod)
			StringCbPrintf(Tmp, sizeof(Tmp), _T("[%s]"), lpMod);
		else
			StringCbPrintf(Tmp, sizeof(Tmp), _T("未指定"));
		_tcscat_s(tchLog, _countof(tchLog), Tmp);

		// 函数名
		if (lpFuncName)
		{
			MultiByteToWideChar(CP_ACP, 0, lpFuncName, -1, tchFuncName, _countof(tchFuncName));

			StringCbPrintf(Tmp, sizeof(Tmp), _T("[%s]"), tchFuncName);
		}
		else
			StringCbPrintf(Tmp, sizeof(Tmp), _T("未指定"));

		_tcscat_s(tchLog, _countof(tchLog), Tmp);

		// 日志
		if (lpFmt)
			StringCbVPrintf(Tmp, sizeof(Tmp), lpFmt, Args);
		else
			StringCbPrintf(Tmp, sizeof(Tmp), _T("参数非法"));
		_tcscat_s(tchLog, _countof(tchLog), Tmp);

		// 换行
		_tcscat_s(tchLog, _countof(tchLog), _T("\r\n"));

		// 转换
		WideCharToMultiByte(CP_ACP, 0, tchLog, -1, chLog, sizeof(chLog), NULL, NULL);

		// 写日志
		if (!WriteFile(hFile, chLog, strlen(chLog), &dwWrite, NULL))
			__leave;

		bRet = TRUE;
	}
	__finally
	{
		if (hFile != INVALID_HANDLE_VALUE)
		{
			FlushFileBuffers(hFile);
			CloseHandle(hFile);
			hFile = INVALID_HANDLE_VALUE;
		}

		va_end(Args);

		LeaveCriticalSection(&CSimpleLog::ms_CriticalSection);
	}

	return bRet;
}