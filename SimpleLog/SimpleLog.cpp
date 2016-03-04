 #include "SimpleLog.h"

CSimpleLog * CSimpleLog::ms_pInstance = NULL;

BOOL
	CSimpleLog::Init(
	__in LPTSTR lpLogPath
	)
{
	BOOL			bRet = FALSE;

	HANDLE			hFile = INVALID_HANDLE_VALUE;
	LPTSTR			lpPosition = NULL;
	TCHAR			lpDir[MAX_PATH] = { 0 };
	TCHAR			tchProcPath[MAX_PATH] = { 0 };


	__try
	{
		if (!lpLogPath)
			__leave;

		_tcscat_s(m_LogPath, _countof(m_LogPath), lpLogPath);

		lpPosition = _tcsrchr(m_LogPath, _T('\\'));
		if (!lpPosition)
			__leave;

		CopyMemory(lpDir, m_LogPath, (lpPosition - m_LogPath) * sizeof(TCHAR));

		SHCreateDirectoryEx(NULL, lpDir, NULL);

		if (PathFileExists(m_LogPath))
		{
			if (!PathYetAnotherMakeUniqueName(m_LogPath, m_LogPath, NULL, NULL))
				__leave;
		}

		hFile = CreateFile(
			m_LogPath,
			0,
			0,
			NULL,
			CREATE_NEW,
			FILE_ATTRIBUTE_NORMAL,
			NULL
			);
		if (INVALID_HANDLE_VALUE == hFile)
			__leave;

		InitializeCriticalSection(&m_CriticalSection);

		m_WriteReady = TRUE;

		setlocale(LC_ALL, "");

		CStackBacktrace::GetInstance(lpDir);

		if (!CProcessControl::GetInstance()->Get(TRUE, 0, tchProcPath, _countof(tchProcPath)))
			__leave;

		if (_tcslen(tchProcPath) >= _tcslen(_T("DbgView.exe")) &&
			(0 == _tcsnicmp(tchProcPath + (_tcslen(tchProcPath) - _tcslen(_T("DbgView.exe"))), _T("DbgView.exe"), _tcslen(_T("DbgView.exe")))))
			m_bOutputDebugString = FALSE;

		m_ProcType = CProcessControl::GetProcType(TRUE, 0);

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
		{
			CSimpleLogSR(MOD_SIMPLE_LOG, LOG_LEVEL_INFORMATION, "%lS", tchProcPath);
			CSimpleLogSR(MOD_SIMPLE_LOG, LOG_LEVEL_INFORMATION, "初始化成功");
		}
		else
			Unload();
	}

	return bRet;
}

BOOL
CSimpleLog::Unload()
{
	BOOL bRet = TRUE;


	__try
	{
		CProcessControl::ReleaseInstance();
		CStackBacktrace::ReleaseInstance();
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


	__try
	{
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

		Write(chLog);

		strcat_s(chLog, _countof(chLog), "\n");

		switch (m_ProcType)
		{
		case PROC_TYPE_NORMAL:
		case PROC_TYPE_CONSOLE:
			{
				printf("%hs", chLog);
				break;
			}
		case PROC_TYPE_SERVICE:
			break;
		default:
			{
				printf("ms_ProcType error. (%d) \n", m_ProcType);
				break;
			}
		}

		if (m_bOutputDebugString)
			OutputDebugStringA(chLog);

		if (LOG_LEVEL_ERROR == LogLevel)
		{
			CStackBacktrace::GetInstance()->StackBacktrace();

			if (IsDebuggerPresent())
			{
				__asm
				{
					int 3
				}
			}
			else
			{
				if (PROC_TYPE_SERVICE == m_ProcType)
				{
					if (!MessageBoxForService(_T("错误"), _T("发生严重错误"), MB_OK | MB_SERVICE_NOTIFICATION | MB_ICONERROR))
						printf("MessageBoxForService failed. \n");
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


	if (!m_WriteReady)
		return TRUE;

	__try
	{
		EnterCriticalSection(&m_CriticalSection);

		if (!lpLog)
			__leave;

		StringCchPrintfA(chLog, _countof(chLog), "%hs\r\n", lpLog);

		lpPositon = wcsrchr(m_LogPath, _T('\\'));
		if (!lpPositon)
			__leave;

		CopyMemory(lpDir, m_LogPath, (lpPositon - m_LogPath) * sizeof(TCHAR));

		SHCreateDirectoryEx(NULL, lpDir, NULL);

		hFile = CreateFile(
			m_LogPath,
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

		LeaveCriticalSection(&m_CriticalSection);
	}

	return bRet;
}

BOOL
	CSimpleLog::MessageBoxForService(
	__in LPTSTR lpTitle,
	__in LPTSTR lpMessage,
	__in DWORD	dwStyle
	)
{
	BOOL	bRet		= FALSE;

	DWORD	dwResponse	= 0;


	__try
	{
		if (!lpTitle || !lpMessage)
		{
			printf("input arguments error. \n");
			__leave;
		}

		if (!WTSSendMessage(
			WTS_CURRENT_SERVER_HANDLE,
			WTSGetActiveConsoleSessionId(),
			lpTitle,
			_tcslen(lpTitle) * sizeof(TCHAR),
			lpMessage,
			_tcslen(lpMessage) * sizeof(TCHAR),
			dwStyle,
			0,
			&dwResponse,
			TRUE
			))
		{
			printf("WTSSendMessage failed. (%d) \n", GetLastError());
			__leave;
		}

		bRet = TRUE;
	}
	__finally
	{
		;
	}

	return bRet;
}

CSimpleLog *
	CSimpleLog::GetInstance(
	__in LPTSTR lpLogPath
	)
{
	if (!ms_pInstance)
	{
		do 
		{
			ms_pInstance = new CSimpleLog();
			if (!ms_pInstance)
				Sleep(1000);
			else
			{
				ms_pInstance->Init(lpLogPath);
				break;
			}
		} while (TRUE);
	}

	return ms_pInstance;
}

VOID
	CSimpleLog::ReleaseInstance()
{
	if (ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = NULL;
	}
}

CSimpleLog::CSimpleLog()
{
	ZeroMemory(m_LogPath, sizeof(m_LogPath));
	ZeroMemory(&m_CriticalSection, sizeof(m_CriticalSection));
	m_WriteReady = FALSE;
	m_bOutputDebugString = TRUE;
	m_ProcType = PROC_TYPE_UNKNOWN;
}

CSimpleLog::~CSimpleLog()
{
	Unload();

	ZeroMemory(m_LogPath, sizeof(m_LogPath));
	ZeroMemory(&m_CriticalSection, sizeof(m_CriticalSection));
	m_WriteReady = FALSE;
	m_bOutputDebugString = TRUE;
	m_ProcType = PROC_TYPE_UNKNOWN;
}
