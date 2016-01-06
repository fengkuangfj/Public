#include "ProcessPath.h"

CProcessPath * CProcessPath::ms_pInstance = NULL;

BOOL
	CProcessPath::Get(
	__in	BOOL	bCurrentProc,
	__in	ULONG	ulPid,
	__out	LPTSTR	lpOutBuf,
	__in	ULONG	ulOutBufSizeCh
	)
{
	BOOL	bRet						= FALSE;

	HANDLE	hProc						= NULL;
	DWORD	dwProcPathLenCh				= 0;
	TCHAR	tchProcPathDev[MAX_PATH]	= {0};
	TCHAR	tchVolNameDev[MAX_PATH]		= {0};
	TCHAR	tchVolName[MAX_PATH]		= {0};


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
			if (!CModulePath::Get(NULL, lpOutBuf, ulOutBufSizeCh))
			{
				printf("Get failed. \n");
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

		if (m_QueryFullProcessImageName)
		{
			dwProcPathLenCh = ulOutBufSizeCh;
			if (!m_QueryFullProcessImageName(hProc, 0, lpOutBuf, &dwProcPathLenCh))
			{
				printf("QueryFullProcessImageName failed. (%d) \n", GetLastError());
				__leave;
			}

			bRet = TRUE;
			__leave;
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
		if (hProc)
		{
			CloseHandle(hProc);
			hProc = NULL;
		}
	}

	return bRet;
}

CProcessPath *
	CProcessPath::GetInstance()
{
	if (!ms_pInstance)
	{
		do 
		{
			ms_pInstance = new CProcessPath;
			if (!ms_pInstance)
				Sleep(1000);
			else
				break;
		} while (TRUE);
	}

	return ms_pInstance;
}

VOID
	CProcessPath::ReleaseInstance()
{
	if (ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = NULL;
	}
}

CProcessPath::CProcessPath()
{
	if (!Init())
		printf("Init failed \n");
}

CProcessPath::~CProcessPath()
{
	if (!Unload())
		printf("Unload failed \n");
}

BOOL
	CProcessPath::Init()
{
	BOOL bRet = FALSE;


	__try
	{
		m_hModule = LoadLibrary(_T("Kernel32.dll"));
		if (!m_hModule)
		{
			printf("LoadLibrary failed. (%d) \n", GetLastError());
			__leave;
		}

		m_QueryFullProcessImageName = (QUERY_FULL_PROCESS_IMAGE_NAME)GetProcAddress(m_hModule, "QueryFullProcessImageName");

		bRet = TRUE;
	}
	__finally
	{
		if (!bRet)
		{
			if (!Unload())
				printf("Unload failed \n");
		}
	}

	return bRet;
}

BOOL
	CProcessPath::Unload()
{
	BOOL bRet = TRUE;


	__try
	{
		m_QueryFullProcessImageName = NULL;

		if (m_hModule)
		{
			FreeLibrary(m_hModule);
			m_hModule = NULL;
		}
	}
	__finally
	{
		;
	}

	return bRet;
}
