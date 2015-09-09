#include "PrintfEx.h"

BOOL CPrintfEx::ms_bUseStackBackTrace = FALSE;
BOOL CPrintfEx::ms_bOutputDebugString = TRUE;

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
	time_t			rawTime					= 0;
	tm				timeInfo				= {0};
	CHAR			chFmtInfo[MAX_PATH]		= {0};
	CHAR			chLogTemp[MAX_PATH * 2]	= {0};
	CHAR			chLog[MAX_PATH * 2]		= {0};

	va_list			Args;

	CStackBacktrace StackBackTrace;

	__try
	{
		va_start(Args, lpFmt);

		// 时间
		time(&rawTime);
		localtime_s(&timeInfo, &rawTime);

		StringCchVPrintfA(chFmtInfo, _countof(chFmtInfo), lpFmt, Args);

		StringCchPrintfA(chLogTemp, _countof(chLogTemp), "[%04d/%02d/%02d][%02d:%02d:%02d][%05d][%lS][%hs][%d][%hs] %hs \n",
			timeInfo.tm_year + 1900,
			timeInfo.tm_mon + 1,
			timeInfo.tm_mday,
			timeInfo.tm_hour,
			timeInfo.tm_min,
			timeInfo.tm_sec,
			GetCurrentThreadId(),
			lpMod ? lpMod : _T("未知模块"),
			lpFile,
			ulLine,
			lpFunction,
			chFmtInfo
			);

		switch (PrintfLevel)
		{
		case PRINTF_LEVEL_INFORMATION:
			{
				strcat_s(chLog, _countof(chLog), "[INFO]");
				strcat_s(chLog, _countof(chLog), chLogTemp);

				printf(chLog);

				if (ms_bOutputDebugString)
					OutputDebugStringA(chLog);

				break;
			}
		case PRINTF_LEVEL_WARNING:
			{
				strcat_s(chLog, _countof(chLog), "[WARN]");
				strcat_s(chLog, _countof(chLog), chLogTemp);

				printf(chLog);

				if (ms_bOutputDebugString)
					OutputDebugStringA(chLog);

				break;
			}
		case PRINTF_LEVEL_ERROR:
			{
				strcat_s(chLog, _countof(chLog), "[ERRO]");
				strcat_s(chLog, _countof(chLog), chLogTemp);

				printf(chLog);

				if (ms_bOutputDebugString)
					OutputDebugStringA(chLog);

				if (ms_bUseStackBackTrace)
				{
					if (!StackBackTrace.StackBacktrace())
					{
						printf("StackBackTrace.StackBacktrace failed \n");
						__leave;
					}
				}

#ifdef _DEBUG
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
#endif

				break;
			}
		default:
			{
				printf("PrintfLevel error. %d \n", PrintfLevel);
				__leave;
			}
		}
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
	BOOL	bRet			= FALSE;

	DWORD	dwLanguageId	= 0;
	HLOCAL	hLocal			= NULL;


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
	CPrintfEx::Init(
	__in_opt LPTSTR	lpSymDir,
	__in_opt BOOL	bOutputDebugString
	)
{
	BOOL			bRet			= FALSE;

	CStackBacktrace StackBackTrace;


	__try
	{
		if (lpSymDir)
		{
			if (!StackBackTrace.Init(lpSymDir))
				printf("StackBackTrace.Init failed \n");
			else
				ms_bUseStackBackTrace = TRUE;
		}

		// ms_bOutputDebugString = bOutputDebugString;

		setlocale(LC_ALL, "");

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
	BOOL			bRet			= FALSE;

	CStackBacktrace StackBackTrace;


	__try
	{
		if (!StackBackTrace.Unload())
		{
			printf("StackBackTrace.Unload failed \n");
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
