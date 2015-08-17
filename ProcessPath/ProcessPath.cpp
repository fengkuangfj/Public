#include "ProcessPath.h"

BOOL
	CProcessPath::Get(
	__in	BOOL	bCurrentProc,
	__in	ULONG	ulPid,
	__out	LPTSTR	lpInBuf,
	__in	ULONG	ulInBufSizeCh
	)
{
	BOOL	bRet						= FALSE;

	HMODULE hModule						= NULL;
	HANDLE	hProc						= NULL;
	DWORD	dwProcPathLenCh				= 0;
	TCHAR	tchProcPathDev[MAX_PATH]	= {0};
	TCHAR	tchVolNameDev[MAX_PATH]		= {0};
	TCHAR	tchVolName[MAX_PATH]		= {0};


	printfEx("begin");

	__try
	{
		if (!lpInBuf || !ulInBufSizeCh || (!bCurrentProc && !ulPid))
		{
			printfEx("input arguments error. %d %d 0x%08p %d", bCurrentProc, ulPid, lpInBuf, ulInBufSizeCh);
			__leave;
		}

		if (bCurrentProc)
		{
			if (!CModulePath::Get(NULL, lpInBuf, ulInBufSizeCh))
			{
				printfEx("Get failed");
				__leave;
			}

			printfEx("%S", lpInBuf);

			bRet = TRUE;
			__leave;
		}

		hProc = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, ulPid);
		if (!hProc)
		{
			printfEx("OpenProcess failed. (%d)", GetLastError());
			__leave;
		}

		hModule = LoadLibrary(_T("Kernel32.dll"));
		if (!hModule)
		{
			printfEx("LoadLibrary failed. (%d)", GetLastError());
			__leave;
		}

		if (GetProcAddress(hModule, "QueryFullProcessImageNameA"))
		{
			dwProcPathLenCh = ulInBufSizeCh;
			if (!QueryFullProcessImageName(hProc, 0, lpInBuf, &dwProcPathLenCh))
			{
				printfEx("QueryFullProcessImageName failed. (%d)", GetLastError());
				__leave;
			}

			printfEx("[QueryFullProcessImageName] [%d] %S", ulPid, lpInBuf);

			bRet = TRUE;
			__leave;
		}

		if (!GetProcessImageFileName(hProc, tchProcPathDev, _countof(tchProcPathDev)))
		{
			printfEx("GetProcessImageFileName failed. (%d)", GetLastError());
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
					printfEx("QueryDosDevice failed. (%d)", GetLastError());
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
			_tcscat_s(lpInBuf, ulInBufSizeCh, tchVolName);
			_tcscat_s(lpInBuf, ulInBufSizeCh, tchProcPathDev + _tcslen(tchVolNameDev));

			printfEx("[QueryDosDevice] [%d] %S", ulPid, lpInBuf);
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

	printfEx("end");

	return bRet;
}
