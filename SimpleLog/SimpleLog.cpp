 #include "SimpleLog.h"

CSimpleLog * CSimpleLog::ms_pInstance = NULL;

BOOL
	CSimpleLog::Init(
	__in		LPTSTR	lpLogPath,
	__in_opt	LPTSTR	lpServiceName
	)
{
	BOOL			bRet = FALSE;

	LPTSTR			lpPosition = NULL;
	TCHAR			lpDir[MAX_PATH] = { 0 };
	TCHAR			tchProcPath[MAX_PATH] = { 0 };


	__try
	{
		if (!lpLogPath)
		{
			printfEx(MOD_SIMPLE_LOG, PRINTF_LEVEL_ERROR, "input argument error");
			__leave;
		}

		_tcscat_s(m_LogPath, _countof(m_LogPath), lpLogPath);

		lpPosition = _tcsrchr(m_LogPath, _T('\\'));
		if (!lpPosition)
		{
			printfEx(MOD_SIMPLE_LOG, PRINTF_LEVEL_ERROR, "_tcsrchr failed");
			__leave;
		}

		CopyMemory(lpDir, m_LogPath, (lpPosition - m_LogPath) * sizeof(TCHAR));

		SHCreateDirectoryEx(NULL, lpDir, NULL);

		if (PathFileExists(m_LogPath))
		{
			if (!PathYetAnotherMakeUniqueName(m_LogPath, m_LogPath, NULL, NULL))
			{
				printfEx(MOD_SIMPLE_LOG, PRINTF_LEVEL_ERROR, "PathYetAnotherMakeUniqueName failed. (%d)", GetLastError());
				__leave;
			}
		}

		m_hFile = CreateFile(
			m_LogPath,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			CREATE_NEW,
			FILE_ATTRIBUTE_NORMAL,
			NULL
			);
		if (INVALID_HANDLE_VALUE == m_hFile)
		{
			printfEx(MOD_SIMPLE_LOG, PRINTF_LEVEL_ERROR, "CreateFile failed. (%d)", GetLastError());
			__leave;
		}

		InitializeCriticalSection(&m_CriticalSection);

		m_WriteReady = TRUE;

		setlocale(LC_ALL, "");

		CStackBacktrace::GetInstance(lpDir);

		if (!CProcessControl::GetInstance()->Get(TRUE, 0, tchProcPath, _countof(tchProcPath)))
		{
			printfEx(MOD_SIMPLE_LOG, PRINTF_LEVEL_ERROR, "CProcessControl::GetInstance()->Get failed");
			__leave;
		}

		if (_tcslen(tchProcPath) >= _tcslen(_T("DbgView.exe")) &&
			(0 == _tcsnicmp(tchProcPath + (_tcslen(tchProcPath) - _tcslen(_T("DbgView.exe"))), _T("DbgView.exe"), _tcslen(_T("DbgView.exe")))))
			m_bOutputDebugString = FALSE;

		m_ProcType = CProcessControl::GetInstance()->GetProcType(TRUE, 0);

		if (lpServiceName)
			_tcscat_s(m_tchServiceName, _countof(m_tchServiceName), lpServiceName);

		bRet = TRUE;
	}
	__finally
	{
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
		CSimpleLogSR(MOD_SIMPLE_LOG, LOG_LEVEL_INFORMATION, "卸载成功");

		if (INVALID_HANDLE_VALUE != m_hFile)
		{
			CloseHandle(m_hFile);
			m_hFile = INVALID_HANDLE_VALUE;
		}

		CService::ReleaseInstance();
		CProcessControl::ReleaseInstance();
		CStackBacktrace::ReleaseInstance();
		CPrintfEx::ReleaseInstance();
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

	SYSTEMTIME		SystemTime = { 0 };
	CHAR			chFmtInfo[MAX_PATH] = { 0 };
	CHAR			chLog[MAX_PATH * 2] = { 0 };
	HANDLE			hOutput = INVALID_HANDLE_VALUE;


	__try
	{
		va_start(Args, lpFmt);

		GetLocalTime(&SystemTime);

		StringCchVPrintfA(chFmtInfo, _countof(chFmtInfo), lpFmt, Args);

		StringCchPrintfA(chLog, _countof(chLog), "%hs[%04d/%02d/%02d][%02d:%02d:%02d.%03d][%05d][%05d][%lS][%hs][%d][%hs] %hs ",
			(0 == _tcsicmp(lpMod, MOD_STACK_BACKTRACE)) ? ((LOG_LEVEL_INFORMATION == LogLevel) ? "\t[INFO]" : ((LOG_LEVEL_WARNING == LogLevel) ? "\t[WARN]" : ((LOG_LEVEL_ERROR == LogLevel) ? "\t[ERRO]" : "\t[????]"))) : ((LOG_LEVEL_INFORMATION == LogLevel) ? "[INFO]" : ((LOG_LEVEL_WARNING == LogLevel) ? "[WARN]" : ((LOG_LEVEL_ERROR == LogLevel) ? "[ERRO]" : "[????]"))),
			SystemTime.wYear,
			SystemTime.wMonth,
			SystemTime.wDay,
			SystemTime.wHour,
			SystemTime.wMinute,
			SystemTime.wSecond,
			SystemTime.wMilliseconds,
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
				printfEx(MOD_SIMPLE_LOG, PRINTF_LEVEL_ERROR, "ms_ProcType error. (%d)", m_ProcType);
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
				__debugbreak();
// 				__asm
// 				{
// 					int 3
// 				}
			}
			else
			{
				if (PROC_TYPE_SERVICE == m_ProcType)
				{
					if (!MessageBoxForService(_T("错误"), _T("发生严重错误"), MB_OK | MB_ICONERROR, m_tchServiceName))
						printfEx(MOD_SIMPLE_LOG, PRINTF_LEVEL_ERROR, "MessageBoxForService failed");
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
	LARGE_INTEGER	FileSize = { 0 };
	DWORD			dwWrite = 0;


	if (!m_WriteReady)
		return TRUE;

	__try
	{
		EnterCriticalSection(&m_CriticalSection);

		if (!lpLog)
		{
			printfEx(MOD_SIMPLE_LOG, PRINTF_LEVEL_ERROR, "input argument error");
			__leave;
		}

		StringCchPrintfA(chLog, _countof(chLog), "%hs\r\n", lpLog);

		FileSize.LowPart = GetFileSize(m_hFile, NULL);
		if (INVALID_SET_FILE_POINTER == SetFilePointer(m_hFile, FileSize.LowPart, NULL, FILE_BEGIN))
		{
			printfEx(MOD_SIMPLE_LOG, PRINTF_LEVEL_ERROR, "SetFilePointer failed. (%d)", GetLastError());
			__leave;
		}

		if (!WriteFile(m_hFile, chLog, strlen(chLog), &dwWrite, NULL))
		{
			printfEx(MOD_SIMPLE_LOG, PRINTF_LEVEL_ERROR, "WriteFile failed. (%d)", GetLastError());
			__leave;
		}

		bRet = TRUE;
	}
	__finally
	{
		if (INVALID_HANDLE_VALUE != m_hFile)
			FlushFileBuffers(m_hFile);

		LeaveCriticalSection(&m_CriticalSection);
	}

	return bRet;
}

BOOL
	CSimpleLog::MessageBoxForService(
	__in LPTSTR lpTitle,
	__in LPTSTR lpMessage,
	__in DWORD	dwStyle,
	__in LPTSTR	lpServiceName
	)
{
	BOOL	bRet		= FALSE;

	DWORD	dwResponse	= 0;


	__try
	{
		if (!lpTitle || !lpMessage || !lpServiceName)
		{
			printfEx(MOD_SIMPLE_LOG, PRINTF_LEVEL_ERROR, "input arguments error. lpTitle(0x%p) lpMessage(0x%p) lpServiceName(0x%p)",
				lpTitle, lpMessage, lpServiceName);

			__leave;
		}

		if (!CService::GetInstance()->CanInteractWithTheDesktop(lpServiceName))
		{
			printfEx(MOD_SIMPLE_LOG, PRINTF_LEVEL_ERROR, "Can not interact with the desktop. %s", lpServiceName);
			__leave;
		}

		if (!WTSSendMessage(
			WTS_CURRENT_SERVER_HANDLE,
			WTSGetActiveConsoleSessionId(),
			lpTitle,
			_tcslen(lpTitle) * sizeof(TCHAR),
			lpMessage,
			_tcslen(lpMessage) * sizeof(TCHAR),
			dwStyle | MB_SERVICE_NOTIFICATION,
			0,
			&dwResponse,
			TRUE
			))
		{
			printfEx(MOD_SIMPLE_LOG, PRINTF_LEVEL_ERROR, "WTSSendMessage failed. (%d)", GetLastError());
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
			new CSimpleLog(lpLogPath);
			if (!ms_pInstance)
				Sleep(1000);
			else
				break;
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

CSimpleLog::CSimpleLog(
	__in LPTSTR lpLogPath
	)
{
	ms_pInstance = this;

	ZeroMemory(m_LogPath, sizeof(m_LogPath));
	ZeroMemory(&m_CriticalSection, sizeof(m_CriticalSection));
	m_WriteReady = FALSE;
	m_bOutputDebugString = TRUE;
	m_ProcType = PROC_TYPE_UNKNOWN;

	if (!Init(lpLogPath))
		printfEx(MOD_SIMPLE_LOG, PRINTF_LEVEL_ERROR, "Init failed");
}

CSimpleLog::~CSimpleLog()
{
	if (!Unload())
		printfEx(MOD_SIMPLE_LOG, PRINTF_LEVEL_ERROR, "Unload failed");

	ZeroMemory(m_LogPath, sizeof(m_LogPath));
	ZeroMemory(&m_CriticalSection, sizeof(m_CriticalSection));
	m_WriteReady = FALSE;
	m_bOutputDebugString = TRUE;
	m_ProcType = PROC_TYPE_UNKNOWN;
}
