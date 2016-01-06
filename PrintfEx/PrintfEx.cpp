#include "PrintfEx.h"

CPrintfEx * CPrintfEx::ms_pInstance = NULL;

VOID
CPrintfEx::PrintfInternal(
__in LPTSTR			lpMod,
__in PRINTF_LEVEL	PrintfLevel,
__in LPSTR			lpFile,
__in LPSTR			lpFunction,
__in ULONG			ulLine,
__in LPSTR			lpFmt,
...
)
{
	va_list	Args;

	time_t	rawTime = 0;
	tm		timeInfo = { 0 };
	CHAR	chFmtInfo[MAX_PATH] = { 0 };
	CHAR	chLog[MAX_PATH * 2] = { 0 };


	__try
	{
		va_start(Args, lpFmt);

		time(&rawTime);
		localtime_s(&timeInfo, &rawTime);

		StringCchVPrintfA(chFmtInfo, _countof(chFmtInfo), lpFmt, Args);

		StringCchPrintfA(chLog, _countof(chLog), "%hs[%04d/%02d/%02d][%02d:%02d:%02d][%05d][%05d][%lS][%hs][%d][%hs] %hs \n",
			(PRINTF_LEVEL_INFORMATION == PrintfLevel) ? "[INFO]" : ((PRINTF_LEVEL_WARNING == PrintfLevel) ? "[WARN]" : ((PRINTF_LEVEL_ERROR == PrintfLevel) ? "[ERRO]" : "[????]")),
			timeInfo.tm_year + 1900,
			timeInfo.tm_mon + 1,
			timeInfo.tm_mday,
			timeInfo.tm_hour,
			timeInfo.tm_min,
			timeInfo.tm_sec,
			GetCurrentProcessId(),
			GetCurrentThreadId(),
			lpMod ? lpMod : _T("Î´ÖªÄ£¿é"),
			lpFile,
			ulLine,
			lpFunction,
			chFmtInfo
			);

		switch (m_ProcType)
		{
		case PROC_TYPE_CONSOLE:
			{
				printf("%hs", chLog);
				break;
			}
		case PROC_TYPE_NORMAL:
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
	}
	__finally
	{
		va_end(Args);
	}
}

BOOL
CPrintfEx::ErrorCodeConnote(
__in	DWORD	dwErrorCode,
__out	LPTSTR	lpOutBuf,
__in	ULONG	ulOutBufSizeCh
)
{
	BOOL	bRet = FALSE;

	DWORD	dwLanguageId = 0;
	HLOCAL	hLocal = NULL;


	__try
	{
		dwLanguageId = MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL);

		if (!FormatMessage(
			FORMAT_MESSAGE_FROM_SYSTEM, // FORMAT_MESSAGE_IGNORE_INSERTS
			NULL,
			dwErrorCode,
			dwLanguageId,
			lpOutBuf,
			ulOutBufSizeCh,
			NULL
			))
		{
			printf("FormatMessage failed. (%d) \n", GetLastError());
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

BOOL
CPrintfEx::Init()
{
	BOOL	bRet = FALSE;

	TCHAR	tchProcPath[MAX_PATH] = { 0 };


	__try
	{
		setlocale(LC_ALL, "");

		if (!CProcessPath::GetInstance()->Get(TRUE, 0, tchProcPath, _countof(tchProcPath)))
			__leave;

		if (_tcslen(tchProcPath) >= _tcslen(_T("DbgView.exe")) &&
			(0 == _tcsnicmp(tchProcPath + (_tcslen(tchProcPath) - _tcslen(_T("DbgView.exe"))), _T("DbgView.exe"), _tcslen(_T("DbgView.exe")))))
			m_bOutputDebugString = FALSE;

		m_ProcType = CProcessType::GetProcType(TRUE, 0);

		bRet = TRUE;
	}
	__finally
	{
		;
	}

	return bRet;
}

BOOL
	CPrintfEx::Unload()
{
	BOOL bRet = TRUE;


	__try
	{
		CProcessPath::ReleaseInstance();
	}
	__finally
	{
		;
	}

	return bRet;
}

CPrintfEx *
	CPrintfEx::GetInstance()
{
	if (!ms_pInstance)
	{
		do 
		{
			ms_pInstance = new CPrintfEx;
			if (!ms_pInstance)
				Sleep(1000);
			else
				break;
		} while (TRUE);
	}

	return ms_pInstance;
}

VOID
	CPrintfEx::ReleaseInstance()
{
	if (ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = NULL;
	}
}

CPrintfEx::CPrintfEx()
{
	m_bOutputDebugString = TRUE;
	m_ProcType = PROC_TYPE_UNKNOWN;

	Init();
}

CPrintfEx::~CPrintfEx()
{
	Unload();

	m_bOutputDebugString = TRUE;
	m_ProcType = PROC_TYPE_UNKNOWN;
}
