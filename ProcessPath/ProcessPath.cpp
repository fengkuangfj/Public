#include "ProcessPath.h"

BOOL
	CProcessPath::Get(
	__in	BOOL	bCurrentProc,
	__in	ULONG	ulPid,
	__out	LPTSTR	lpOutBuf,
	__in	ULONG	ulOutBufSizeCh
	)
{
	BOOL	bRet						= FALSE;

	HMODULE hModule						= NULL;
	HANDLE	hProc						= NULL;
	DWORD	dwProcPathLenCh				= 0;
	TCHAR	tchProcPathDev[MAX_PATH]	= {0};
	TCHAR	tchVolNameDev[MAX_PATH]		= {0};
	TCHAR	tchVolName[MAX_PATH]		= {0};


	__try
	{
		if (!lpOutBuf || !ulOutBufSizeCh || (!bCurrentProc && !ulPid))
		{
			printfEx(MOD_PROCESS_PATH, PRINTF_LEVEL_ERROR, "input arguments error. %d %d 0x%08p %d", bCurrentProc, ulPid, lpOutBuf, ulOutBufSizeCh);
			__leave;
		}

		ZeroMemory(lpOutBuf, ulOutBufSizeCh * sizeof(TCHAR));

		if (bCurrentProc)
		{
			if (!CModulePath::Get(NULL, lpOutBuf, ulOutBufSizeCh))
			{
				printfEx(MOD_PROCESS_PATH, PRINTF_LEVEL_ERROR, "Get failed");
				__leave;
			}

			printfEx(MOD_PROCESS_PATH, PRINTF_LEVEL_INFORMATION, "%S", lpOutBuf);

			bRet = TRUE;
			__leave;
		}

		hProc = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, ulPid);
		if (!hProc)
		{
			printfEx(MOD_PROCESS_PATH, PRINTF_LEVEL_ERROR, "OpenProcess failed. (%d)", GetLastError());
			__leave;
		}

		hModule = LoadLibrary(_T("Kernel32.dll"));
		if (!hModule)
		{
			printfEx(MOD_PROCESS_PATH, PRINTF_LEVEL_ERROR, "LoadLibrary failed. (%d)", GetLastError());
			__leave;
		}

// 		if (GetProcAddress(hModule, "QueryFullProcessImageNameA"))
// 		{
// 			dwProcPathLenCh = ulOutBufSizeCh;
// 			if (!QueryFullProcessImageName(hProc, 0, lpOutBuf, &dwProcPathLenCh))
// 			{
// 				printfEx(MOD_PROCESS_PATH, PRINTF_LEVEL_ERROR, "QueryFullProcessImageName failed. (%d)", GetLastError());
// 				__leave;
// 			}
// 
// 			printfEx(MOD_PROCESS_PATH, PRINTF_LEVEL_INFORMATION, "[QueryFullProcessImageName] [%d] %S", ulPid, lpOutBuf);
// 
// 			bRet = TRUE;
// 			__leave;
// 		}

		if (!GetProcessImageFileName(hProc, tchProcPathDev, _countof(tchProcPathDev)))
		{
			printfEx(MOD_PROCESS_PATH, PRINTF_LEVEL_ERROR, "GetProcessImageFileName failed. (%d)", GetLastError());
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
					printfEx(MOD_PROCESS_PATH, PRINTF_LEVEL_ERROR, "QueryDosDevice failed. (%d)", GetLastError());
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

			printfEx(MOD_PROCESS_PATH, PRINTF_LEVEL_INFORMATION, "[QueryDosDevice] [%d] %S", ulPid, lpOutBuf);
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
