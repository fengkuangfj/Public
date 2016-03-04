#include "SimpleDump.h"

CSimpleDump * CSimpleDump::ms_pInstance = NULL;

BOOL
CSimpleDump::GenRestartCmdLine(
__inout LPTSTR	pCmdLine,
__in	ULONG	ulCharacters
)
{
	BOOL		bRet = FALSE;

	int			i = 0;
	TCHAR		tchArgv[MAX_PATH]	= { 0 };
	FILETIME	FileTime			= {0};
	FILETIME	ExitTime			= {0};
	FILETIME	KernelTime			= {0};
	FILETIME	UserTime			= {0};
	TCHAR		tchTemp[MAX_PATH]	= {0};


	__try
	{
		if (!pCmdLine || !ulCharacters)
			__leave;

		if (PROC_TYPE_NORMAL == m_ProcType)
			_tcscat_s(pCmdLine, ulCharacters, _T(" "));

		if (m_lpCmdLine && _tcslen(m_lpCmdLine))
		{
			_tcscat_s(pCmdLine, ulCharacters, m_lpCmdLine);
			_tcscat_s(pCmdLine, ulCharacters, _T(" "));
		}

		_tcscat_s(pCmdLine, ulCharacters, m_tchRestartTag);

		_tcscat_s(pCmdLine, ulCharacters, _T("_"));
		_itow_s(GetCurrentProcessId(), tchTemp, _countof(tchTemp), 10);
		_tcscat_s(pCmdLine, ulCharacters, tchTemp);

		if (!GetProcessTimes(GetCurrentProcess(), &FileTime, &ExitTime, &KernelTime, &UserTime))
			__leave;

		_tcscat_s(pCmdLine, ulCharacters, _T("_"));
		ZeroMemory(tchTemp, sizeof(tchTemp));
		_itow_s(FileTime.dwLowDateTime, tchTemp, _countof(tchTemp), 10);
		_tcscat_s(pCmdLine, ulCharacters, tchTemp);

		_tcscat_s(pCmdLine, ulCharacters, _T("_"));
		ZeroMemory(tchTemp, sizeof(tchTemp));
		_itow_s(FileTime.dwHighDateTime, tchTemp, _countof(tchTemp), 10);
		_tcscat_s(pCmdLine, ulCharacters, tchTemp);

		bRet = TRUE;
	}
	__finally
	{
		;
	}

	return bRet;
}

/*
*默认重启客户端处理函数
*/
VOID
CSimpleDump::DefaultRestartFunc()
{
	STARTUPINFO			si = { 0 };
	PROCESS_INFORMATION pi = { 0 };
	TCHAR				tchProcPath[MAX_PATH] = { 0 };
	TCHAR				tchInfo[MAX_PATH] = { 0 };
	LPTSTR				lpProcName = NULL;
	LPTSTR				lpCmdLine = NULL;


	__try
	{
		if (!GetModuleFileName(NULL, tchProcPath, _countof(tchProcPath)))
			__leave;

		lpProcName = StrRChr(tchProcPath, tchProcPath + _tcslen(tchProcPath), _T('\\'));
		if (!lpProcName)
			__leave;

		lpProcName++;
		_tcscat_s(tchInfo, _countof(tchInfo), lpProcName);
		_tcscat_s(tchInfo, _countof(tchInfo), _T(" 已崩溃！"));

		if (IDYES == MessageBox(NULL, _T("是否需要重启？"), tchInfo, MB_YESNO | MB_ICONWARNING))
		{
			if (BeenRunningMinimum60Seconds())
			{
				if (m_bCanUseRegisterRestart)
					__leave;
			}

			GetStartupInfo(&si);

			lpCmdLine = (LPTSTR)calloc(1, CMD_LINE_MAX_CHARS * sizeof(TCHAR));
			if (!lpCmdLine)
				__leave;

			if (!GenRestartCmdLine(lpCmdLine, CMD_LINE_MAX_CHARS))
				__leave;

			if (PROC_TYPE_CONSOLE == m_ProcType || PROC_TYPE_SERVICE == m_ProcType)
			{
				if (!CreateProcess(NULL, lpCmdLine, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
					__leave;
			}
			else
			{
				if (!CreateProcess(tchProcPath, lpCmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
					__leave;
			}
		}
	}
	__finally
	{
		if (pi.hThread)
		{
			CloseHandle(pi.hThread);
			pi.hThread = NULL;
		}

		if (pi.hProcess)
		{
			CloseHandle((pi.hProcess));
			pi.hProcess = NULL;
		}

		if (lpCmdLine)
		{
			free(lpCmdLine);
			lpCmdLine = NULL;
		}
	}
}

BOOL
CSimpleDump::BeenRunningMinimum60Seconds()
{
	BOOL		bRet = FALSE;

	FILETIME	CreationTime = { 0 };
	FILETIME	ExitTime = { 0 };
	FILETIME	KernelTime = { 0 };
	FILETIME	UserTime = { 0 };
	SYSTEMTIME	SystemTimeCreate = { 0 };
	SYSTEMTIME	SystemTimeNow = { 0 };
	WORD		wMilliseconds = 0;


	__try
	{
		if (!GetProcessTimes(
			GetCurrentProcess(),
			&CreationTime,
			&ExitTime,
			&KernelTime,
			&UserTime
			))
			__leave;

		if (!FileTimeToSystemTime(&CreationTime, &SystemTimeCreate))
			__leave;

		GetSystemTime(&SystemTimeNow);

		if (SystemTimeNow.wYear > SystemTimeCreate.wYear)
		{
			bRet = TRUE;
			__leave;
		}
		else if (SystemTimeNow.wMonth > SystemTimeCreate.wMonth)
		{
			bRet = TRUE;
			__leave;
		}
		else if (SystemTimeNow.wDay > SystemTimeCreate.wDay)
		{
			bRet = TRUE;
			__leave;
		}
		else if (SystemTimeNow.wHour > SystemTimeCreate.wHour)
		{
			bRet = TRUE;
			__leave;
		}
		else if (SystemTimeNow.wMinute > SystemTimeCreate.wMinute)
		{
			if (SystemTimeCreate.wMilliseconds)
			{
				wMilliseconds = (SystemTimeNow.wSecond + 59 - SystemTimeCreate.wSecond) * 1000 +
					SystemTimeNow.wMilliseconds + 1000 - SystemTimeCreate.wMilliseconds;
			}
			else
			{
				wMilliseconds = (SystemTimeNow.wSecond + 60 - SystemTimeCreate.wSecond) * 1000 +
					SystemTimeNow.wMilliseconds;
			}

			if (60 * 1000 < wMilliseconds)
			{
				bRet = TRUE;
				__leave;
			}
		}
	}
	__finally
	{
		;
	}

	return bRet;
}

LONG
WINAPI
CSimpleDump::ExceptionHandler(
_In_ struct _EXCEPTION_POINTERS* pExceptionInfo
)
{
	PEXCEPTION_RECORD pExceptionRecored = NULL;


	__try
	{
		if (!pExceptionInfo || !pExceptionInfo->ExceptionRecord)
			__leave;

		pExceptionRecored = pExceptionInfo->ExceptionRecord;

		do
		{
			__try
			{
				// 过滤4个RPC异常和一个调试处理异常(OutputDebugString)
				if (RPC_S_SERVER_UNAVAILABLE == pExceptionRecored->ExceptionCode ||
					RPC_S_SERVER_TOO_BUSY == pExceptionRecored->ExceptionCode ||
					RPC_S_CALL_FAILED_DNE == pExceptionRecored->ExceptionCode ||
					RPC_S_CALL_FAILED == pExceptionRecored->ExceptionCode ||
					DBG_PRINTEXCEPTION_C == pExceptionRecored->ExceptionCode)
					__leave;

				CSimpleDump::GetInstance()->GenDump(pExceptionInfo);
			}
			__finally
			{
				;
			}

			pExceptionRecored = pExceptionRecored->ExceptionRecord;
		} while (pExceptionRecored);

		if (OS_VERSION_WINDOWS_VISTA > COperationSystemVersion::GetInstance()->GetOSVersion())
		{
			if (CSimpleDump::GetInstance()->m_bRestart)
			{
				if (CSimpleDump::GetInstance()->m_pfRestart)
					CSimpleDump::GetInstance()->m_pfRestart();
				else
					CSimpleDump::GetInstance()->DefaultRestartFunc();
			}
		}
		else
		{
			if (CSimpleDump::GetInstance()->m_bRestart)
			{
				if (!CSimpleDump::GetInstance()->BeenRunningMinimum60Seconds())
				{
					if (CSimpleDump::GetInstance()->m_pfRestart)
						CSimpleDump::GetInstance()->m_pfRestart();
					else
						CSimpleDump::GetInstance()->DefaultRestartFunc();
				}
			}
		}
	}
	__finally
	{
		if (CSimpleDump::GetInstance()->m_hModuleKernel32Dll)
		{
			FreeLibrary(CSimpleDump::GetInstance()->m_hModuleKernel32Dll);
			CSimpleDump::GetInstance()->m_hModuleKernel32Dll = NULL;
		}

		if (CSimpleDump::GetInstance()->m_lpCmdLine)
		{
			free(CSimpleDump::GetInstance()->m_lpCmdLine);
			CSimpleDump::GetInstance()->m_lpCmdLine = NULL;
		}
	}

	if (CSimpleDump::GetInstance()->m_pfTopLevelExceptionFilter)
		return CSimpleDump::GetInstance()->m_pfTopLevelExceptionFilter(pExceptionInfo);
	else
		return EXCEPTION_CONTINUE_SEARCH;
}

BOOL
CSimpleDump::RegisterRestart()
{
	BOOL	bRet = FALSE;

	HRESULT	hResult = E_UNEXPECTED;
	TCHAR	tchCmdLine[RESTART_MAX_CMD_LINE] = { 0 };


	__try
	{
		StringCchPrintf(tchCmdLine, _countof(tchCmdLine), L"/restart -r");

		hResult = m_pfRegisterApplicationRestart(tchCmdLine, RESTART_NO_PATCH | RESTART_NO_REBOOT);
		if (FAILED(hResult))
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
CSimpleDump::InitCmdLine(
__in_opt int	nArgc,
__in_opt TCHAR*	pArgv[],
__in_opt LPTSTR	lpCmdLine
)
{
	BOOL	bRet = FALSE;

	ULONG	ulLen = 0;
	int		i = 0;
	LPTSTR	lpPosition = NULL;


	__try
	{
		if (PROC_TYPE_CONSOLE == m_ProcType || PROC_TYPE_SERVICE == m_ProcType)
		{
			if (nArgc && pArgv)
			{
				while (i < nArgc)
				{
					ulLen += _tcslen(pArgv[i++]);
					ulLen++;
				}
			}
		}
		else
		{
			if (lpCmdLine)
			{
				ulLen = _tcslen(lpCmdLine);
				ulLen++;
			}
		}

		if (ulLen)
		{
			m_lpCmdLine = (LPTSTR)calloc(1, ulLen * sizeof(TCHAR));
			if (!m_lpCmdLine)
				__leave;

			if (PROC_TYPE_CONSOLE == m_ProcType || PROC_TYPE_SERVICE == m_ProcType)
			{
				if (nArgc && pArgv)
				{
					i = 0;
					while (i < nArgc)
					{
						_tcscat_s(m_lpCmdLine, ulLen, pArgv[i++]);

						if (i < nArgc)
							_tcscat_s(m_lpCmdLine, ulLen, _T(" "));
					}
				}
			}
			else
			{
				if (lpCmdLine)
					_tcscat_s(m_lpCmdLine, ulLen, lpCmdLine);
			}

			lpPosition = StrRStrI(m_lpCmdLine, NULL, m_tchRestartTag);
			if (lpPosition)
			{
				*lpPosition = _T('\0');

				if (lpPosition > m_lpCmdLine && _T(' ') == *(lpPosition - 1))
					*(lpPosition - 1) = _T('\0');

				WaitForOldProcExit();
			}
		}

		bRet = TRUE;
	}
	__finally
	{
		if (!bRet && m_lpCmdLine)
		{
			free(m_lpCmdLine);
			m_lpCmdLine = NULL;
		}
	}

	return bRet;
}

/*
*注册生成dmp和重启回调
*
*参数
*	MinidumpType	生成的dump的类型
*	Handler			重启客户端回调的函数名
*		NULL	调用本.h文件中的DefaultRestartClient重启客户端
*		函数名	调用用户自定义重启客户端回调
*
*返回值
*	无
*/
VOID
CSimpleDump::RegisterCrushHandler(
__in PCRUSH_HANDLER_INFO pCrushHandlerInfo
)
{
	HANDLE hOutPut = INVALID_HANDLE_VALUE;


	__try
	{
		if (!GetFunc())
			__leave;

		m_ProcType = CProcessControl::GetProcType(TRUE, 0);

		if (pCrushHandlerInfo)
		{
			switch (pCrushHandlerInfo->EhType)
			{
				case EH_TYPE_S:
				{
					m_pfTopLevelExceptionFilter = SetUnhandledExceptionFilter((PTOP_LEVEL_EXCEPTION_FILTER)ExceptionHandler);
					break;
				}
				case EH_TYPE_V:
				{
					AddVectoredExceptionHandler(pCrushHandlerInfo->bFirstHandler ? 1 : 0, (PVECTORED_EXCEPTION_HANDLER)ExceptionHandler);
					break;
				}
				default:
				{
					AddVectoredExceptionHandler(1, (PVECTORED_EXCEPTION_HANDLER)ExceptionHandler);
					// AddVectoredExceptionHandler(0, (PVECTORED_EXCEPTION_HANDLER)ExceptionHandler);
					// SetUnhandledExceptionFilter((PTOP_LEVEL_EXCEPTION_FILTER)ExceptionHandler);
					break;
				}
			}

			m_MinidumpType = pCrushHandlerInfo->MiniDumpType;

			m_bRestart = pCrushHandlerInfo->bRestart;
			if (m_bRestart)
			{
				m_pfRestart = pCrushHandlerInfo->Restart;
				_tcscat_s(m_tchRestartTag, _countof(m_tchRestartTag), pCrushHandlerInfo->tchRestartTag);

				if (PROC_TYPE_CONSOLE == m_ProcType || PROC_TYPE_SERVICE == m_ProcType)
				{
					if (!InitCmdLine(pCrushHandlerInfo->Arg.nArgc, pCrushHandlerInfo->Arg.plpArgv, NULL))
						__leave;
				}
				else
				{
					if (!InitCmdLine(NULL, NULL, pCrushHandlerInfo->lpCmdLine))
						__leave;
				}

				if (m_bCanUseRegisterRestart)
				{
					if (!RegisterRestart())
						__leave;
				}
			}
		}
		else
		{
			AddVectoredExceptionHandler(1, (PVECTORED_EXCEPTION_HANDLER)ExceptionHandler);
			// AddVectoredExceptionHandler(0, (PVECTORED_EXCEPTION_HANDLER)ExceptionHandler);
			// SetUnhandledExceptionFilter((PTOP_LEVEL_EXCEPTION_FILTER)ExceptionHandler);
		}
	}
	__finally
	{
		;
	}

	return;
}

BOOL
CSimpleDump::CreateDumpFile(
__in	PVOID	pExceptionAddress,
__inout LPTSTR	lpDumpFilePath,
__in	ULONG	ulBufferLen
)
{
	BOOL						bRet = FALSE;

	TCHAR						tchModulePath[MAX_PATH] = { 0 };
	TCHAR						tchProcPath[MAX_PATH] = { 0 };
	TCHAR						tchDumpFilePath[MAX_PATH] = { 0 };
	TCHAR						tchTmp[MAX_PATH] = { 0 };
	TCHAR						tchDumpFilePathWithoutCount[MAX_PATH] = { 0 };
	TCHAR*						pModuleName = NULL;
	TCHAR*						pProcName = NULL;
	SYSTEMTIME					SystemTime = { 0 };
	HANDLE						hFile = INVALID_HANDLE_VALUE;
	ULONG						ulCount = 0;
	MEMORY_BASIC_INFORMATION	MemoryBasicInfo = { 0 };


	__try
	{
		if (!pExceptionAddress || !lpDumpFilePath || !ulBufferLen)
			__leave;

		if (!VirtualQuery(pExceptionAddress, &MemoryBasicInfo, sizeof(MemoryBasicInfo)))
			__leave;

		if (!GetModuleFileName((HMODULE)MemoryBasicInfo.AllocationBase, tchModulePath, _countof(tchModulePath)))
			__leave;

		pModuleName = StrRChr(tchModulePath, tchModulePath + _tcsclen(tchModulePath), _T('\\'));
		if (!pModuleName)
			__leave;

		if (!GetModuleFileName(NULL, tchProcPath, _countof(tchProcPath)))
			__leave;

		pProcName = StrRChr(tchProcPath, tchProcPath + _tcsclen(tchProcPath), _T('\\'));
		if (!pProcName)
			__leave;

		MoveMemory(tchDumpFilePathWithoutCount, tchModulePath, (pModuleName - tchModulePath) * sizeof(TCHAR));
		_tcscat_s(tchDumpFilePathWithoutCount, _countof(tchDumpFilePathWithoutCount), pProcName);
		_tcscat_s(tchDumpFilePathWithoutCount, _countof(tchDumpFilePathWithoutCount), _T("__"));
		_tcscat_s(tchDumpFilePathWithoutCount, _countof(tchDumpFilePathWithoutCount), pModuleName + 1);

		GetLocalTime(&SystemTime);

		StringCbPrintf(tchTmp, sizeof(tchTmp), _T("__%04d"), SystemTime.wYear);
		_tcscat_s(tchDumpFilePathWithoutCount, _countof(tchDumpFilePathWithoutCount), tchTmp);

		StringCbPrintf(tchTmp, sizeof(tchTmp), _T("-%02d"), SystemTime.wMonth);
		_tcscat_s(tchDumpFilePathWithoutCount, _countof(tchDumpFilePathWithoutCount), tchTmp);

		StringCbPrintf(tchTmp, sizeof(tchTmp), _T("-%02d"), SystemTime.wDay);
		_tcscat_s(tchDumpFilePathWithoutCount, _countof(tchDumpFilePathWithoutCount), tchTmp);

		StringCbPrintf(tchTmp, sizeof(tchTmp), _T("__%02d"), SystemTime.wHour);
		_tcscat_s(tchDumpFilePathWithoutCount, _countof(tchDumpFilePathWithoutCount), tchTmp);

		StringCbPrintf(tchTmp, sizeof(tchTmp), _T("-%02d"), SystemTime.wMinute);
		_tcscat_s(tchDumpFilePathWithoutCount, _countof(tchDumpFilePathWithoutCount), tchTmp);

		StringCbPrintf(tchTmp, sizeof(tchTmp), _T("-%02d"), SystemTime.wSecond);
		_tcscat_s(tchDumpFilePathWithoutCount, _countof(tchDumpFilePathWithoutCount), tchTmp);

		StringCbPrintf(tchTmp, sizeof(tchTmp), _T("__%03d"), SystemTime.wMilliseconds);
		_tcscat_s(tchDumpFilePathWithoutCount, _countof(tchDumpFilePathWithoutCount), tchTmp);

		do
		{
			_tcscat_s(tchDumpFilePath, _countof(tchDumpFilePath), tchDumpFilePathWithoutCount);

			if (ulCount)
			{
				StringCbPrintf(tchTmp, sizeof(tchTmp), _T("__%d"), ulCount);
				_tcscat_s(tchDumpFilePath, _countof(tchDumpFilePath), tchTmp);
			}

			_tcscat_s(tchDumpFilePath, _countof(tchDumpFilePath), _T(".dmp"));

			hFile = CreateFileW(
				tchDumpFilePath,
				0,
				0,
				NULL,
				CREATE_NEW,
				FILE_ATTRIBUTE_NORMAL,
				NULL
				);
			if (INVALID_HANDLE_VALUE != hFile)
				break;
			else
			{
				if (80 != GetLastError())
					__leave;

				// 文件存在
				ZeroMemory(tchDumpFilePath, sizeof(tchDumpFilePath));
				ulCount++;
			}
		} while (TRUE);

		if (ulBufferLen <= _tcsclen(tchDumpFilePath) * sizeof(TCHAR))
			__leave;

		_tcscat_s(lpDumpFilePath, ulBufferLen / sizeof(TCHAR), tchDumpFilePath);

		bRet = TRUE;
	}
	__finally
	{
		if (INVALID_HANDLE_VALUE != hFile)
		{
			CloseHandle(hFile);
			hFile = INVALID_HANDLE_VALUE;
		}
	}

	return bRet;
}

BOOL
CSimpleDump::GenDump(
__in _EXCEPTION_POINTERS* pExceptionInfo
)
{
	BOOL							bRet = FALSE;

	MINIDUMP_EXCEPTION_INFORMATION	MiniExceptionInfo = { 0 };
	HANDLE							hFile = INVALID_HANDLE_VALUE;
	TCHAR							tchDumpFilePath[MAX_PATH] = { 0 };


	__try
	{
		if (!pExceptionInfo)
			__leave;

		if (!CreateDumpFile(pExceptionInfo->ExceptionRecord->ExceptionAddress, tchDumpFilePath, sizeof(tchDumpFilePath)))
			__leave;

		hFile = CreateFileW(
			tchDumpFilePath,
			GENERIC_ALL,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL
			);
		if (INVALID_HANDLE_VALUE == hFile)
			__leave;

		MiniExceptionInfo.ThreadId = GetCurrentThreadId();
		MiniExceptionInfo.ExceptionPointers = pExceptionInfo;
		MiniExceptionInfo.ClientPointers = TRUE;

		if (!MiniDumpWriteDump(
			GetCurrentProcess(),
			GetCurrentProcessId(),
			hFile,
			m_MinidumpType,
			&MiniExceptionInfo,
			NULL,
			NULL
			))
			__leave;

		bRet = TRUE;
	}
	__finally
	{
		if (INVALID_HANDLE_VALUE != hFile)
		{
			CloseHandle(hFile);
			hFile = INVALID_HANDLE_VALUE;
		}
	}

	return bRet;
}

BOOL
CSimpleDump::GetApplicationVersion(
__in	LPTSTR	lpApplicationPath,
__inout LPTSTR	lpApplicationVersion,
__in	ULONG	ulApplicationVersionLen
)
{
	BOOL				bRet = FALSE;

	LPVOID				lpFileVerInfo = NULL;
	DWORD				dwFileVerInfoLen = 0;
	LPVOID				lpFileVersion = NULL;
	UINT				nFileVersionLen = 0;
	VS_FIXEDFILEINFO	VsFixedFileInfo = { 0 };


	__try
	{
		if (!lpApplicationPath || !lpApplicationVersion || !ulApplicationVersionLen)
			__leave;

		dwFileVerInfoLen = GetFileVersionInfoSize(lpApplicationPath, NULL);
		if (dwFileVerInfoLen)
		{
			lpFileVerInfo = calloc(1, dwFileVerInfoLen);
			if (!lpFileVerInfo)
				__leave;

			if (!GetFileVersionInfo(
				lpApplicationPath,
				NULL,
				dwFileVerInfoLen,
				lpFileVerInfo
				))
				__leave;

			if (!VerQueryValue(
				lpFileVerInfo,
				L"\\",
				&lpFileVersion,
				&nFileVersionLen
				))
				__leave;

			MoveMemory(&VsFixedFileInfo, lpFileVersion, sizeof(VsFixedFileInfo));
		}

		StringCbPrintf(lpApplicationVersion, ulApplicationVersionLen, _T("%d.%d.%d.%d"),
			HIWORD(VsFixedFileInfo.dwFileVersionMS),
			LOWORD(VsFixedFileInfo.dwFileVersionMS),
			HIWORD(VsFixedFileInfo.dwFileVersionLS),
			LOWORD(VsFixedFileInfo.dwFileVersionLS));

		bRet = TRUE;
	}
	__finally
	{
		if (lpFileVerInfo)
		{
			free(lpFileVerInfo);
			lpFileVerInfo = NULL;
		}
	}

	return bRet;
}

BOOL
CSimpleDump::GetModuleName(
__in	PVOID	pAddress,
__inout LPTSTR	lpModuleName,
__in	ULONG	ulModuleNameLen
)
{
	BOOL		bRet = FALSE;

	HMODULE*	phModule = NULL;
	DWORD		dwRet = 0;
	ULONG		ulIndex = 0;


	__try
	{
		if (!pAddress || !lpModuleName || !ulModuleNameLen)
			__leave;

		phModule = (HMODULE*)calloc(1, sizeof(HMODULE));
		if (!phModule)
			__leave;

		if (!EnumProcessModules(GetCurrentProcess(), phModule, sizeof(HMODULE), &dwRet))
			__leave;

		if (!dwRet)
			__leave;

		if (dwRet > sizeof(HMODULE))
		{
			free(phModule);
			phModule = NULL;

			phModule = (HMODULE*)calloc(1, dwRet);
			if (!phModule)
				__leave;
		}

		if (!EnumProcessModules(GetCurrentProcess(), phModule, dwRet, &dwRet))
			__leave;

		if (!SortModule(phModule, dwRet / sizeof(HMODULE)))
			__leave;

		if (!GetModuleNameIndex(pAddress, phModule, dwRet / sizeof(HMODULE), &ulIndex))
			__leave;

		if (!GetModuleFileNameEx(
			GetCurrentProcess(),
			phModule[ulIndex],
			lpModuleName,
			ulModuleNameLen
			))
			__leave;

		bRet = TRUE;
	}
	__finally
	{
		if (phModule)
		{
			free(phModule);
			phModule = NULL;
		}
	}

	return bRet;
}

BOOL
CSimpleDump::GetModuleNameIndex(
__in	PVOID		pAddress,
__in	HMODULE*	phMoudle,
__in	ULONG		ulCount,
__inout ULONG*		pIndex
)
{
	BOOL	bRet = FALSE;

	ULONG	ulIndex = 0;


	__try
	{
		if (!pAddress || !phMoudle || !ulCount || !pIndex)
			__leave;

		for (; ulIndex < ulCount; ulIndex++)
		{
			if (pAddress >= phMoudle[ulIndex])
			{
				*pIndex = ulIndex;
				bRet = TRUE;
				__leave;
			}
		}
	}
	__finally
	{
		;
	}

	return bRet;
}

BOOL
CSimpleDump::SortModule(
__inout	HMODULE*	phMoudle,
__in	ULONG		ulCount
)
{
	BOOL	bRet = FALSE;

	ULONG	i = 0;
	HMODULE hModule = NULL;
	ULONG	ulLoop = 0;


	__try
	{
		if (!phMoudle || !ulCount)
			__leave;

		while (ulCount--)
		{
			for (; i < ulCount; i++)
			{
				if (phMoudle[i] < phMoudle[i + 1])
				{
					hModule = phMoudle[i + 1];
					phMoudle[i + 1] = phMoudle[i];
					phMoudle[i] = hModule;
				}
			}
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
CSimpleDump::GetFunc()
{
	BOOL bRet = FALSE;


	__try
	{
		if (!GetKernel32DllFunc())
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
CSimpleDump::GetKernel32DllFunc()
{
	BOOL bRet = FALSE;


	__try
	{
		m_hModuleKernel32Dll = LoadLibrary(_T("Kernel32.dll"));
		if (!m_hModuleKernel32Dll)
			__leave;

		m_pfRegisterApplicationRestart = (REGISTER_APPLICATION_RESTART)GetProcAddress(m_hModuleKernel32Dll, "RegisterApplicationRestart");
		if (m_pfRegisterApplicationRestart)
			m_bCanUseRegisterRestart = TRUE;

		bRet = TRUE;
	}
	__finally
	{
		;
	}

	return bRet;
}

BOOL
CSimpleDump::InitArgCmdlineInfo(
__in		PROC_TYPE			ProcType,
__in_opt	int					nArgc,
__in_opt	LPTSTR				lpArgv[],
__in_opt	LPTSTR				lpCmdLine,
__inout_opt	PARG_CMDLINE_INFO	pArgCmdLineInfo,
__inout		ULONG*				pulBufLen
)
{
	BOOL	bRet = FALSE;

	ULONG	ulBufLen = 0;
	int		i = 0;


	__try
	{
		// 计算大小
		switch (ProcType)
		{
			case PROC_TYPE_CONSOLE:
			case PROC_TYPE_SERVICE:
			{
				if (nArgc && lpArgv)
				{
					while (i < nArgc)
						ulBufLen += (_tcslen(lpArgv[i++]) * sizeof(TCHAR));

					if (ulBufLen)
					{
						ulBufLen += (nArgc * sizeof(TCHAR));
						ulBufLen += (nArgc * sizeof(LPTSTR));
						ulBufLen += sizeof(ARG_CMDLINE_INFO) - sizeof(LPTSTR);
					}
				}

				break;
			}
			case PROC_TYPE_NORMAL:
			{
				if (lpCmdLine)
				{
					ulBufLen = _tcslen(lpCmdLine) * sizeof(TCHAR);
					if (ulBufLen)
					{
						ulBufLen += sizeof(TCHAR);
						ulBufLen += sizeof(ARG_CMDLINE_INFO);
					}
				}

				break;
			}
			default:
				__leave;
		}

		// 比较大小
		if (pulBufLen && (*pulBufLen < ulBufLen))
		{
			*pulBufLen = ulBufLen;
			__leave;
		}

		// 初始化
		if (!pArgCmdLineInfo)
		{
			if (pulBufLen)
				*pulBufLen = ulBufLen;

			__leave;
		}

		pArgCmdLineInfo->ProcType = ProcType;
		switch (pArgCmdLineInfo->ProcType)
		{
			case PROC_TYPE_CONSOLE:
			case PROC_TYPE_SERVICE:
			{
				if (nArgc && lpArgv)
				{
					pArgCmdLineInfo->Console.nArgc = nArgc;

					i = 0;
					while (i < nArgc)
					{
						if (0 == i)
							pArgCmdLineInfo->Console.lpArgv[i] = (LPTSTR)(&(pArgCmdLineInfo->Console.lpArgv[0]) + nArgc);
						else
							pArgCmdLineInfo->Console.lpArgv[i] = pArgCmdLineInfo->Console.lpArgv[i - 1] + _tcslen(pArgCmdLineInfo->Console.lpArgv[i - 1]) + 1;

						MoveMemory(pArgCmdLineInfo->Console.lpArgv[i], lpArgv[i], _tcslen(lpArgv[i]) * sizeof(TCHAR));
						*(pArgCmdLineInfo->Console.lpArgv[i] + _tcslen(lpArgv[i])) = _T('\0');

						i++;
					}
				}

				break;
			}
			case PROC_TYPE_NORMAL:
			{
				if (lpCmdLine)
				{
					MoveMemory(pArgCmdLineInfo->NotConsole.tchCmdLine, lpCmdLine, _tcslen(lpCmdLine) * sizeof(TCHAR));
					*(pArgCmdLineInfo->NotConsole.tchCmdLine + _tcslen(lpCmdLine)) = _T('\0');
				}

				break;
			}
			default:
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
CSimpleDump::ParseArgOrCmdLine(
__in		PARG_CMDLINE_INFO	pArgCmdlineInfo,
__inout_opt LPTSTR				lpResult,
__inout		ULONG*				pulBufLen
)
{
	BOOL	bRet = FALSE;

	ULONG	ulBufLen = 0;
	int		i = 0;
	LPTSTR	lpPrePosition = NULL;
	LPTSTR	lpCurrentPosition = NULL;
	LPTSTR	lpTemp = NULL;
	ULONG	ulCalloc = 0;


	__try
	{
		if (!pArgCmdlineInfo)
			__leave;

		// 计算大小
		switch (pArgCmdlineInfo->ProcType)
		{
			case PROC_TYPE_CONSOLE:
			case PROC_TYPE_SERVICE:
			{
				if (pArgCmdlineInfo->Console.nArgc)
				{
					while (i < pArgCmdlineInfo->Console.nArgc)
					{
						ulBufLen += _tcslen(pArgCmdlineInfo->Console.lpArgv[i++]) * sizeof(TCHAR);
						ulBufLen += sizeof(TCHAR);
					}
				}

				break;
			}
			case PROC_TYPE_NORMAL:
			{
				ulBufLen = _tcslen(pArgCmdlineInfo->NotConsole.tchCmdLine) * sizeof(TCHAR);
				ulBufLen += sizeof(TCHAR);

				break;
			}
			default:
				__leave;
		}

		// 比较大小
		if (pulBufLen && *pulBufLen < ulBufLen)
		{
			*pulBufLen = ulBufLen;
			__leave;
		}

		// 初始化
		if (!lpResult)
		{
			if (pulBufLen)
				*pulBufLen = ulBufLen;

			__leave;
		}

		switch (pArgCmdlineInfo->ProcType)
		{
			case PROC_TYPE_CONSOLE:
			case PROC_TYPE_SERVICE:
			{
				i = 0;
				while (i < pArgCmdlineInfo->Console.nArgc)
				{
					_tcscat_s(lpResult, *pulBufLen / sizeof(TCHAR), pArgCmdlineInfo->Console.lpArgv[i++]);

					if (i < pArgCmdlineInfo->Console.nArgc)
						_tcscat_s(lpResult, *pulBufLen / sizeof(TCHAR), _T("\n"));
				}

				break;
			}
			case PROC_TYPE_NORMAL:
			{
				lpPrePosition = pArgCmdlineInfo->NotConsole.tchCmdLine;
				lpCurrentPosition = pArgCmdlineInfo->NotConsole.tchCmdLine;
				while (lpCurrentPosition <= pArgCmdlineInfo->NotConsole.tchCmdLine + _tcslen(pArgCmdlineInfo->NotConsole.tchCmdLine))
				{
					if (_T(' ') == *lpCurrentPosition || _T('\0') == *lpCurrentPosition)
					{
						if (lpCurrentPosition > pArgCmdlineInfo->NotConsole.tchCmdLine)
						{
							if (ulCalloc < (lpCurrentPosition - lpPrePosition) * sizeof(TCHAR) + sizeof(TCHAR))
							{
								ulCalloc = (lpCurrentPosition - lpPrePosition) * sizeof(TCHAR) + sizeof(TCHAR);

								if (lpTemp)
								{
									free(lpTemp);
									lpTemp = NULL;
								}

								lpTemp = (LPTSTR)calloc(1, ulCalloc);
								if (!lpTemp)
									__leave;
							}

							MoveMemory(lpTemp, lpPrePosition, (lpCurrentPosition - lpPrePosition) * sizeof(TCHAR));
							*(lpTemp + (lpCurrentPosition - lpPrePosition)) = _T('\0');

							_tcscat_s(lpResult, *pulBufLen / sizeof(TCHAR), lpTemp);

							if (_T(' ') == *lpCurrentPosition)
								_tcscat_s(lpResult, *pulBufLen / sizeof(TCHAR), _T("\n"));

							if (lpCurrentPosition < pArgCmdlineInfo->NotConsole.tchCmdLine + _tcslen(pArgCmdlineInfo->NotConsole.tchCmdLine))
								lpPrePosition = lpCurrentPosition + 1;

							ZeroMemory(lpTemp, ulCalloc);
						}
					}

					lpCurrentPosition++;
				}

				break;
			}
			default:
				__leave;
		}

		bRet = TRUE;
	}
	__finally
	{
		if (lpTemp)
		{
			free(lpTemp);
			lpTemp = NULL;
		}
	}

	return bRet;
}

void
	CSimpleDump::WaitForOldProcExit()
{
	LPTSTR		lpPosition	= NULL;
	DWORD		dwPidPre	= 0;
	FILETIME	FileTimePre	= {0};
	HANDLE		hProc		= NULL;
	FILETIME	FileTime	= {0};
	FILETIME	ExitTime	= {0};
	FILETIME	KernelTime	= {0};
	FILETIME	UserTime	= {0};


	__try
	{
		if (!_tcslen(m_tchRestartTag))
			__leave;

		lpPosition = StrRChr(m_tchRestartTag, NULL, _T('_'));
		if (!lpPosition)
			__leave;

		FileTimePre.dwHighDateTime = _wtoi(lpPosition + 1);

		*lpPosition = _T('\0');

		lpPosition = NULL;
		lpPosition = StrRChr(m_tchRestartTag, NULL, _T('_'));
		if (!lpPosition)
			__leave;

		FileTimePre.dwLowDateTime = _wtoi(lpPosition + 1);

		*lpPosition = _T('\0');

		lpPosition = NULL;
		lpPosition = StrRChr(m_tchRestartTag, NULL, _T('_'));
		if (!lpPosition)
			__leave;

		dwPidPre = _wtoi(lpPosition + 1);

		*lpPosition = _T('\0');

		do 
		{
			hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPidPre);
			if (!hProc)
				__leave;

			if (!GetProcessTimes(hProc, &FileTime, &ExitTime, &KernelTime, &UserTime))
				__leave;

			CloseHandle(hProc);
			hProc = NULL;

			if (FileTime.dwLowDateTime != FileTimePre.dwLowDateTime ||
				FileTime.dwHighDateTime != FileTimePre.dwHighDateTime)
				__leave;

			Sleep(1000);
			continue;
		} while (TRUE);
	}
	__finally
	{
		if (hProc)
		{
			CloseHandle(hProc);
			hProc = NULL;
		}
	}

	return ;
}

CSimpleDump *
	CSimpleDump::GetInstance()
{
	if (!ms_pInstance)
	{
		do 
		{
			ms_pInstance = new CSimpleDump;
			if (!ms_pInstance)
				Sleep(1000);
			else
				break;
		} while (TRUE);
	}

	return ms_pInstance;
}

VOID
	CSimpleDump::ReleaseInstance()
{
	if (ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = NULL;
	}
}

CSimpleDump::CSimpleDump()
{
	m_MinidumpType = MiniDumpNormal;
	m_bRestart = FALSE;
	m_ProcType = PROC_TYPE_UNKNOWN;
	m_lpCmdLine = NULL;

	ZeroMemory(m_tchRestartTag, sizeof(m_tchRestartTag));

	m_pfRestart = NULL;

	m_hModuleKernel32Dll = NULL;
	m_bCanUseRegisterRestart = FALSE;
	m_pfRegisterApplicationRestart = NULL;

	m_pfTopLevelExceptionFilter = NULL;
}

CSimpleDump::~CSimpleDump()
{
	COperationSystemVersion::ReleaseInstance();

	m_MinidumpType = MiniDumpNormal;
	m_bRestart = FALSE;
	m_ProcType = PROC_TYPE_UNKNOWN;
	m_lpCmdLine = NULL;

	ZeroMemory(m_tchRestartTag, sizeof(m_tchRestartTag));

	m_pfRestart = NULL;

	m_hModuleKernel32Dll = NULL;
	m_bCanUseRegisterRestart = FALSE;
	m_pfRegisterApplicationRestart = NULL;

	m_pfTopLevelExceptionFilter = NULL;
}
