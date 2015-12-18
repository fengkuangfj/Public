#include "PrintfEx.h"

BOOL							CPrintfEx::ms_bOutputDebugString = TRUE;
QUERY_FULL_PROCESS_IMAGE_NAME	CPrintfEx::ms_QueryFullProcessImageName = NULL;
PROC_TYPE						CPrintfEx::ms_ProcType = PROC_TYPE_UNKNOWN;

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

		switch (ms_ProcType)
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
				printf("ms_ProcType error. (%d) \n", ms_ProcType);
				break;
			}
		}

		if (ms_bOutputDebugString)
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

		if (!GetProcPath(TRUE, 0, tchProcPath, _countof(tchProcPath)))
			__leave;

		if (_tcslen(tchProcPath) >= _tcslen(_T("DbgView.exe")) &&
			(0 == _tcsnicmp(tchProcPath + (_tcslen(tchProcPath) - _tcslen(_T("DbgView.exe"))), _T("DbgView.exe"), _tcslen(_T("DbgView.exe")))))
			ms_bOutputDebugString = FALSE;

		ms_ProcType = CProcessType::GetProcType(TRUE, 0);

		bRet = TRUE;
	}
	__finally
	{
		;
	}

	return bRet;
}

BOOL
CPrintfEx::GetProcPath(
__in	BOOL	bCurrentProc,
__in	ULONG	ulPid,
__out	LPTSTR	lpOutBuf,
__in	ULONG	ulOutBufSizeCh
)
{
	BOOL	bRet = FALSE;

	HMODULE hModule = NULL;
	HANDLE	hProc = NULL;
	DWORD	dwProcPathLenCh = 0;
	TCHAR	tchProcPathDev[MAX_PATH] = { 0 };
	TCHAR	tchVolNameDev[MAX_PATH] = { 0 };
	TCHAR	tchVolName[MAX_PATH] = { 0 };


	__try
	{
		if (!lpOutBuf || !ulOutBufSizeCh || (!bCurrentProc && !ulPid))
		{
			printf("input arguments error. %d %d 0x%08p %d \n", bCurrentProc, ulPid, lpOutBuf, ulOutBufSizeCh);
			__leave;
		}

		ZeroMemory(lpOutBuf, ulOutBufSizeCh * sizeof(TCHAR));

		if (bCurrentProc)
		{
			if (!GetModulePath(NULL, lpOutBuf, ulOutBufSizeCh))
			{
				printf("Get failed \n");
				__leave;
			}

			bRet = TRUE;
			__leave;
		}

		hProc = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, ulPid);
		if (!hProc)
		{
			printf("OpenProcess failed. (%d) \n", GetLastError());
			__leave;
		}

		hModule = LoadLibrary(_T("Kernel32.dll"));
		if (!hModule)
		{
			printf("LoadLibrary failed. (%d) \n", GetLastError());
			__leave;
		}

		if (!ms_QueryFullProcessImageName)
		{
			ms_QueryFullProcessImageName = (QUERY_FULL_PROCESS_IMAGE_NAME)GetProcAddress(hModule, "QueryFullProcessImageName");
			if (ms_QueryFullProcessImageName)
			{
				dwProcPathLenCh = ulOutBufSizeCh;
				if (!ms_QueryFullProcessImageName(hProc, 0, lpOutBuf, &dwProcPathLenCh))
				{
					printf("QueryFullProcessImageName failed. (%d) \n", GetLastError());
					__leave;
				}

				bRet = TRUE;
				__leave;
			}
		}

		if (!GetProcessImageFileName(hProc, tchProcPathDev, _countof(tchProcPathDev)))
		{
			printf("GetProcessImageFileName failed. (%d) \n", GetLastError());
			__leave;
		}

		_tcscat_s(tchVolName, _countof(tchVolName), _T("A:"));
		for (; _T('Z') >= *tchVolName; (*tchVolName)++)
		{
			ZeroMemory(tchVolNameDev, sizeof(tchVolNameDev));
			if (!QueryDosDevice(tchVolName, tchVolNameDev, _countof(tchVolNameDev)))
			{
				if (2 == GetLastError())
					continue;
				else
				{
					printf("QueryDosDevice failed. (%d) \n", GetLastError());
					__leave;
				}
			}

			if (0 == _tcsnicmp(tchProcPathDev, tchVolNameDev, _tcslen(tchVolNameDev)))
			{
				bRet = TRUE;
				break;
			}
		}

		if (bRet)
		{
			_tcscat_s(lpOutBuf, ulOutBufSizeCh, tchVolName);
			_tcscat_s(lpOutBuf, ulOutBufSizeCh, tchProcPathDev + _tcslen(tchVolNameDev));
		}
	}
	__finally
	{
		if (hModule)
		{
			FreeLibrary(hModule);
			hModule = NULL;
		}

		if (hProc)
		{
			CloseHandle(hProc);
			hProc = NULL;
		}
	}

	return bRet;
}

BOOL
CPrintfEx::GetModulePath(
__in_opt	HMODULE	hModule,
__out		LPTSTR	lpOutBuf,
__in		ULONG	ulOutBufSizeCh
)
{
	BOOL	bRet = FALSE;

	DWORD	dwResult = 0;


	__try
	{
		if (!lpOutBuf || !ulOutBufSizeCh)
		{
			printf("input arguments error. 0x%08p %d \n", lpOutBuf, ulOutBufSizeCh);
			__leave;
		}

		ZeroMemory(lpOutBuf, ulOutBufSizeCh * sizeof(TCHAR));

		if (!hModule)
		{
			hModule = GetModuleHandle(NULL);
			if (!hModule)
			{
				printf("GetModuleHandle failed. (%d) \n", GetLastError());
				__leave;
			}
		}

		dwResult = GetModuleFileName(hModule, lpOutBuf, ulOutBufSizeCh);
		if (!dwResult)
		{
			printf("GetModuleFileName failed. (%d) \n", GetLastError());
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
