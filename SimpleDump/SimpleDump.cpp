#include "stdafx.h"
#include "SimpleDump.h"


MINIDUMP_TYPE					CSimpleDump::ms_MinidumpType						= MiniDumpNormal;
BOOL							CSimpleDump::ms_bRestart							= FALSE;
HMODULE							CSimpleDump::ms_hModule							= NULL;
APPLICATION_TYPE				CSimpleDump::ms_ApplicationType					= APPLICATION_TYPE_UNKNOWN;
PROGRAM_TYPE					CSimpleDump::ms_ProgramType						= PROGRAM_TYPE_TYPE_UNKNOWN;
LPTSTR							CSimpleDump::ms_lpCmdLine						= NULL;
TCHAR							CSimpleDump::ms_tchRestartTag[MAX_PATH]			= {0};

RESTART							CSimpleDump::ms_Restart							= NULL;

HMODULE							CSimpleDump::ms_hModuleKernel32Dll				= NULL;
BOOL							CSimpleDump::ms_bCanUseGetOSVersionByIsOrGreater = FALSE;
IS_WINDOWS_SERVER				CSimpleDump::IsWindowsServer						= NULL;
IS_WINDOWS_10_OR_GREATER		CSimpleDump::IsWindows1OrGreater					= NULL;
IS_WINDOWS_8_POINT_1_OR_GREATER	CSimpleDump::IsWindows8Point1OrGreater			= NULL;
IS_WINDOWS_8_OR_GREATER			CSimpleDump::IsWindows8OrGreater					= NULL;
IS_WINDOWS_7_SP_1_OR_GREATER	CSimpleDump::IsWindows7SP1OrGreater				= NULL;
IS_WINDOWS_7_OR_GREATER			CSimpleDump::IsWindows7OrGreater					= NULL;
IS_WINDOWS_VISTA_SP2_OR_GREATER	CSimpleDump::IsWindowsVistaSP2OrGreater			= NULL;
IS_WINDOWS_VISTA_SP1_OR_GREATER	CSimpleDump::IsWindowsVistaSP1OrGreater			= NULL;
IS_WINDOWS_VISTA_OR_GREATER		CSimpleDump::IsWindowsVistaOrGreater				= NULL;
IS_WINDOWS_XP_SP3_OR_GREATER	CSimpleDump::IsWindowsXPSP3OrGreater				= NULL;
IS_WINDOWS_XP_SP2_OR_GREATER	CSimpleDump::IsWindowsXPSP2OrGreater				= NULL;
IS_WINDOWS_XP_SP1_OR_GREATER	CSimpleDump::IsWindowsXPSP1OrGreater				= NULL;
IS_WINDOWS_XP_OR_GREATER		CSimpleDump::IsWindowsXPOrGreater				= NULL;

BOOL							CSimpleDump::ms_bCanUseRegisterRestart			= FALSE;
REGISTER_APPLICATION_RESTART	CSimpleDump::RegisterApplicationRestart			= NULL;

HMODULE							CSimpleDump::ms_hModuleWerDll					= NULL;
BOOL							CSimpleDump::ms_bCanUseGenWerReport				= FALSE;
WER_REPORT_CREATE				CSimpleDump::WerReportCreate						= NULL;
WER_REPORT_SET_PARAMETER		CSimpleDump::WerReportSetParameter				= NULL;
WER_REPORT_ADD_DUMP				CSimpleDump::WerReportAddDump					= NULL;
WER_REPORT_SUBMIT				CSimpleDump::WerReportSubmit						= NULL;
WER_REPORT_CLOSE_HANDLE			CSimpleDump::WerReportCloseHandle				= NULL;


BOOL
	CSimpleDump::GenRestartCmdLine(
	__inout LPTSTR	pCmdLine,
	__in	ULONG	ulCharacters
	)
{
	BOOL	bRet				= FALSE;

	int		i					= 0;
	TCHAR	tchArgv[MAX_PATH]	= {0};


	__try
	{
		if (!pCmdLine || !ulCharacters)
			__leave;

		if (APPLICATION_TYPE_NOT_CONSOLE == ms_ApplicationType)
			_tcscat_s(pCmdLine, ulCharacters, _T(" "));	

		if (ms_lpCmdLine && _tcslen(ms_lpCmdLine))
		{
			_tcscat_s(pCmdLine, ulCharacters, ms_lpCmdLine);
			_tcscat_s(pCmdLine, ulCharacters, _T(" "));
		}

		_tcscat_s(pCmdLine, ulCharacters, ms_tchRestartTag);

		bRet = TRUE;
	}
	__finally
	{
		;
	}

	return bRet;
}

/*
*Ĭ�������ͻ��˴�����
*/
VOID
	CSimpleDump::DefaultRestartFunc()
{
	STARTUPINFO			si						= {0};
	PROCESS_INFORMATION pi						= {0};
	TCHAR				tchProcPath[MAX_PATH]	= {0};
	TCHAR				tchInfo[MAX_PATH]		= {0};
	LPTSTR				lpProcName				= NULL;
	LPTSTR				lpCmdLine				= NULL;


	__try
	{
		if (!GetModuleFileName(NULL, tchProcPath, _countof(tchProcPath)))
			__leave;

		lpProcName = StrRChr(tchProcPath, tchProcPath + _tcslen(tchProcPath), _T('\\'));
		if (!lpProcName)
			__leave;

		lpProcName++;
		_tcscat_s(tchInfo, _countof(tchInfo), lpProcName);
		_tcscat_s(tchInfo, _countof(tchInfo), _T(" �ѱ�����"));

		if (IDYES == MessageBox(NULL, _T("�Ƿ���Ҫ������"), tchInfo, MB_YESNO | MB_ICONWARNING))
		{
			if (BeenRunningMinimum60Seconds())
			{
				if (ms_bCanUseRegisterRestart)
					__leave;
			}

			GetStartupInfo(&si);

			lpCmdLine = (LPTSTR)calloc(1, CMD_LINE_MAX_CHARS * sizeof(TCHAR));
			if (!lpCmdLine)
				__leave;

			if (!GenRestartCmdLine(lpCmdLine, CMD_LINE_MAX_CHARS))
				__leave;

			if (APPLICATION_TYPE_CONSOLE == ms_ApplicationType)
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
	BOOL		bRet				= FALSE;

	FILETIME	CreationTime		= {0};
	FILETIME	ExitTime			= {0};
	FILETIME	KernelTime			= {0};
	FILETIME	UserTime			= {0};
	SYSTEMTIME	SystemTimeCreate	= {0};
	SYSTEMTIME	SystemTimeNow		= {0};
	WORD		wMilliseconds		= 0;


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
	PEXCEPTION_RECORD	pExceptionRecored = NULL;

#ifdef _CORE_
	CCommUm				CommUm;
#endif


	__try
	{
		if (!pExceptionInfo || !pExceptionInfo->ExceptionRecord)
			__leave;

		pExceptionRecored = pExceptionInfo->ExceptionRecord;

		do 
		{
			__try
			{
				// ����4��RPC�쳣��һ�����Դ����쳣(OutputDebugString)
				if (RPC_S_SERVER_UNAVAILABLE	== pExceptionRecored->ExceptionCode ||
					RPC_S_SERVER_TOO_BUSY		== pExceptionRecored->ExceptionCode ||
					RPC_S_CALL_FAILED_DNE		== pExceptionRecored->ExceptionCode ||
					RPC_S_CALL_FAILED			== pExceptionRecored->ExceptionCode ||
					DBG_PRINTEXCEPTION_C		== pExceptionRecored->ExceptionCode)
					__leave;

#ifdef _CORE_
				CommUm.Stop();
#endif

				GenDump(pExceptionInfo);

				if (ms_bCanUseGenWerReport)
					GenWerReport(pExceptionInfo);
			}
			__finally
			{
				;
			}

			pExceptionRecored = pExceptionRecored->ExceptionRecord;
		} while (pExceptionRecored);

		if (OS_VERSION_WINDOWS_VISTA > GetOSVersion())
		{
			if (ms_bRestart)
			{
				if (ms_Restart)
					ms_Restart();
				else
					DefaultRestartFunc();
			}
		}
		else
		{
			if (ms_bRestart)
			{
				if (!BeenRunningMinimum60Seconds())
				{
					if (ms_Restart)
						ms_Restart();
					else
						DefaultRestartFunc();
				}
			}
		}
	}
	__finally
	{
		if (ms_hModuleKernel32Dll)
		{
			FreeLibrary(ms_hModuleKernel32Dll);
			ms_hModuleKernel32Dll = NULL;
		}

		if (ms_hModuleWerDll)
		{
			FreeLibrary(ms_hModuleWerDll);
			ms_hModuleWerDll = NULL;
		}

		if (ms_lpCmdLine)
		{
			free(ms_lpCmdLine);
			ms_lpCmdLine = NULL;
		}
	}

	return EXCEPTION_CONTINUE_SEARCH;
}

BOOL
	CSimpleDump::RegisterRestart()
{
	BOOL	bRet								= FALSE;

	HRESULT	hResult								= S_FALSE;
	TCHAR	tchCmdLine[RESTART_MAX_CMD_LINE]	= {0};


	__try
	{
		StringCchPrintf(tchCmdLine, _countof(tchCmdLine), L"/restart -r");

		hResult = RegisterApplicationRestart(tchCmdLine, RESTART_NO_PATCH | RESTART_NO_REBOOT);
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
	BOOL	bRet		= FALSE;

	ULONG	ulLen		= 0;
	int		i			= 0;
	LPTSTR	lpPosition	= NULL;


	__try
	{
		if (APPLICATION_TYPE_CONSOLE == ms_ApplicationType)
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
			ms_lpCmdLine = (LPTSTR)calloc(1, ulLen * sizeof(TCHAR));
			if (!ms_lpCmdLine)
				__leave;

			if (APPLICATION_TYPE_CONSOLE == ms_ApplicationType)
			{
				if (nArgc && pArgv)
				{
					i = 0;
					while (i < nArgc)
					{
						_tcscat_s(ms_lpCmdLine, ulLen, pArgv[i++]);

						if (i < nArgc)
							_tcscat_s(ms_lpCmdLine, ulLen, _T(" "));
					}
				}
			}
			else
			{
				if (lpCmdLine)
					_tcscat_s(ms_lpCmdLine, ulLen, lpCmdLine);
			}

			lpPosition = StrRStrI(ms_lpCmdLine, NULL, ms_tchRestartTag);
			if (lpPosition)
			{
				*lpPosition = _T('\0');

				if (lpPosition > ms_lpCmdLine && _T(' ') == *(lpPosition - 1))
					*(lpPosition - 1) = _T('\0');
			}
		}

		bRet = TRUE;
	}
	__finally
	{
		if (!bRet && ms_lpCmdLine)
		{
			free(ms_lpCmdLine);
			ms_lpCmdLine = NULL;
		}
	}

	return bRet;
}

/*
*ע������dmp�������ص�
*
*����
*	MinidumpType	���ɵ�dump������
*	Handler			�����ͻ��˻ص��ĺ�����
*		NULL	���ñ�.h�ļ��е�DefaultRestartClient�����ͻ���
*		������	�����û��Զ��������ͻ��˻ص�
*			
*����ֵ
*	��
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

		hOutPut = GetStdHandle(STD_OUTPUT_HANDLE);
		if (INVALID_HANDLE_VALUE == hOutPut)
			__leave;

		if (hOutPut)
			ms_ApplicationType = APPLICATION_TYPE_CONSOLE;
		else
			ms_ApplicationType = APPLICATION_TYPE_NOT_CONSOLE;

		if (pCrushHandlerInfo)
		{
			ms_ProgramType = pCrushHandlerInfo->ProgramType;
			if (PROGRAM_TYPE_TYPE_DLL == ms_ProgramType)
				ms_hModule = pCrushHandlerInfo->hDllModule;

			switch (pCrushHandlerInfo->EhType)
			{
			case EH_TYPE_S:
				{
					SetUnhandledExceptionFilter((PTOP_LEVEL_EXCEPTION_FILTER)ExceptionHandler);
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

			ms_MinidumpType = pCrushHandlerInfo->MiniDumpType;

			ms_bRestart = pCrushHandlerInfo->bRestart;
			if (ms_bRestart)
			{
				ms_Restart = pCrushHandlerInfo->Restart;
				_tcscat_s(ms_tchRestartTag, _countof(ms_tchRestartTag), pCrushHandlerInfo->tchRestartTag);

				if (APPLICATION_TYPE_CONSOLE == ms_ApplicationType)
				{
					if (!InitCmdLine(pCrushHandlerInfo->Arg.nArgc, pCrushHandlerInfo->Arg.plpArgv, NULL))
						__leave;
				}
				else
				{
					if (!InitCmdLine(NULL, NULL, pCrushHandlerInfo->lpCmdLine))
						__leave;
				}

				if (ms_bCanUseRegisterRestart)
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
	__inout LPTSTR	lpDumpFilePath,
	__in	ULONG	ulBufferLen
	)
{
	BOOL		bRet									= FALSE;

	TCHAR		tchModulePath[MAX_PATH]					= {0};
	TCHAR		tchProcPath[MAX_PATH]					= {0};
	TCHAR		tchDumpFilePath[MAX_PATH]				= {0};
	TCHAR		tchTmp[MAX_PATH]						= {0};
	TCHAR		tchDumpFilePathWithoutCount[MAX_PATH]	= {0};
	TCHAR*		pModuleName								= NULL;
	TCHAR*		pProcName								= NULL;
	SYSTEMTIME	SystemTime								= {0};
	HANDLE		hFile									= INVALID_HANDLE_VALUE;
	ULONG		ulCount									= 0;


	__try
	{
		if (!lpDumpFilePath || !ulBufferLen)
			__leave;

		if (ms_hModule)
		{
			if (!GetModuleFileName(ms_hModule, tchModulePath, _countof(tchModulePath)))
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
		}
		else
		{
			if (!GetModuleFileName(NULL, tchDumpFilePathWithoutCount, _countof(tchDumpFilePathWithoutCount)))
				__leave;
		}

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
				GENERIC_ALL,
				FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
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

				// �ļ�����
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
	CSimpleDump::GenWerReport(
	__in _EXCEPTION_POINTERS* pExceptionInfo
	)
{
	BOOL						bRet						= FALSE;

	HRESULT						hResult						= S_FALSE;
	WER_REPORT_INFORMATION		WerReportInfo				= {0};
	HREPORT						hReport						= NULL;
	WER_EXCEPTION_INFORMATION	WerExceptionInfo			= {0};
	WER_DUMP_CUSTOM_OPTIONS		WerDumpCustomOptions		= {0};
	WER_SUBMIT_RESULT			WerSubmitResult				= (WER_SUBMIT_RESULT)0;
	TCHAR						tchFileVersion[MAX_PATH]	= {0};
	TCHAR						tchExceptionCode[MAX_PATH]	= {0};
	TCHAR						tchModuleName[MAX_PATH]		= {0};
	TCHAR						tchTid[MAX_PATH]			= {0};
	TCHAR						tchPid[MAX_PATH]			= {0};


	__try
	{
		if (!pExceptionInfo)
			__leave;

		WerReportInfo.dwSize = sizeof(WER_REPORT_INFORMATION);
		WerReportInfo.hProcess = GetCurrentProcess();

		if (!WerReportInfo.hProcess)
			__leave;

		_tcscat_s(WerReportInfo.wzConsentKey, _countof(WerReportInfo.wzConsentKey), L"Problem");
		_tcscat_s(WerReportInfo.wzFriendlyEventName, _countof(WerReportInfo.wzFriendlyEventName), L"FriendlyEventName");
		_tcscat_s(WerReportInfo.wzApplicationName, _countof(WerReportInfo.wzApplicationName), L"ApplicationName");

		if (!GetModuleFileName(NULL, WerReportInfo.wzApplicationPath, _countof(WerReportInfo.wzApplicationPath)))
			__leave;

		hResult = WerReportCreate(
			L"ProblemEventName",
			WerReportCritical,
			&WerReportInfo,
			&hReport
			);
		if (FAILED(hResult))
			__leave;

		if (!hReport)
			__leave;

		StringCchPrintf(tchExceptionCode, _countof(tchExceptionCode), _T("%08x"), pExceptionInfo->ExceptionRecord->ExceptionCode);
		_tcscat_s(WerReportInfo.wzDescription, _countof(WerReportInfo.wzDescription), L"Description");

		hResult = WerReportSetParameter(
			hReport,
			WER_P0,
			L"ExceptionCode",
			tchExceptionCode
			);
		if (FAILED(hResult))
			__leave;

		hResult = WerReportSetParameter(
			hReport,
			WER_P1,
			L"ApplicationName",
			WerReportInfo.wzApplicationPath
			);
		if (FAILED(hResult))
			__leave;

		if (!GetApplicationVersion(
			WerReportInfo.wzApplicationPath,
			tchFileVersion,
			sizeof(tchFileVersion)
			))
			__leave;

		hResult = WerReportSetParameter(
			hReport,
			WER_P2,
			L"ApplicationVersion",
			tchFileVersion
			);
		if (FAILED(hResult))
			__leave;

		if (!GetModuleName(
			pExceptionInfo->ExceptionRecord->ExceptionAddress,
			tchModuleName,
			sizeof(tchModuleName)
			))
			__leave;

		hResult = WerReportSetParameter(
			hReport,
			WER_P3,
			L"MoudleName",
			tchModuleName
			);
		if (FAILED(hResult))
			__leave;

		StringCchPrintf(tchPid, _countof(tchPid), _T("%d"), GetCurrentProcessId());

		hResult = WerReportSetParameter(
			hReport,
			WER_P4,
			L"ProcessId",
			tchPid
			);
		if (FAILED(hResult))
			__leave;

		StringCchPrintf(tchTid, _countof(tchTid), _T("%d"), GetCurrentThreadId());

		hResult = WerReportSetParameter(
			hReport,
			WER_P5,
			L"ThreadId",
			tchTid
			);
		if (FAILED(hResult))
			__leave;

		WerExceptionInfo.pExceptionPointers = pExceptionInfo;
		WerExceptionInfo.bClientPointers = FALSE;

		WerDumpCustomOptions.dwSize = sizeof(WER_DUMP_CUSTOM_OPTIONS);
		WerDumpCustomOptions.dwMask =
			WER_DUMP_MASK_DUMPTYPE |
			WER_DUMP_MASK_ONLY_THISTHREAD |
			WER_DUMP_MASK_OTHER_MODULESFLAGS |
			WER_DUMP_MASK_OTHERTHREADFLAGS |
			WER_DUMP_MASK_OTHERTHREADFLAGS_EX |
			WER_DUMP_MASK_PREFERRED_MODULE_LIST |
			WER_DUMP_MASK_PREFERRED_MODULESFLAGS |
			WER_DUMP_MASK_THREADFLAGS |
			WER_DUMP_MASK_THREADFLAGS_EX;

		WerDumpCustomOptions.dwDumpFlags = ms_MinidumpType;

		WerDumpCustomOptions.dwExceptionThreadFlags =
			ThreadWriteThread |
			ThreadWriteStack |
			ThreadWriteContext |
			ThreadWriteBackingStore |
			ThreadWriteInstructionWindow |
			ThreadWriteThreadData |
			ThreadWriteThreadInfo;

		WerDumpCustomOptions.dwOtherThreadFlags =
			ThreadWriteThread |
			ThreadWriteStack |
			ThreadWriteContext |
			ThreadWriteBackingStore |
			ThreadWriteInstructionWindow |
			ThreadWriteThreadData |
			ThreadWriteThreadInfo;

		WerDumpCustomOptions.dwExceptionThreadExFlags =
			ThreadWriteThread |
			ThreadWriteStack |
			ThreadWriteContext |
			ThreadWriteBackingStore |
			ThreadWriteInstructionWindow |
			ThreadWriteThreadData |
			ThreadWriteThreadInfo;

		WerDumpCustomOptions.dwOtherThreadExFlags =
			ThreadWriteThread |
			ThreadWriteStack |
			ThreadWriteContext |
			ThreadWriteBackingStore |
			ThreadWriteInstructionWindow |
			ThreadWriteThreadData |
			ThreadWriteThreadInfo;

		WerDumpCustomOptions.dwPreferredModuleFlags =
			ModuleWriteModule |
			ModuleWriteDataSeg |
			ModuleWriteMiscRecord |
			ModuleWriteCvRecord |
			ModuleReferencedByMemory |
			ModuleWriteTlsData |
			ModuleWriteCodeSegs;

		WerDumpCustomOptions.dwOtherModuleFlags =
			ModuleWriteModule |
			ModuleWriteDataSeg |
			ModuleWriteMiscRecord |
			ModuleWriteCvRecord |
			ModuleReferencedByMemory |
			ModuleWriteTlsData |
			ModuleWriteCodeSegs;

		_tcscat_s(WerDumpCustomOptions.wzPreferredModuleList, _countof(WerDumpCustomOptions.wzPreferredModuleList), L"*\0\0");

		hResult = WerReportAddDump(
			hReport,
			WerReportInfo.hProcess,
			GetCurrentThread(),
			WerDumpTypeHeapDump,
			&WerExceptionInfo,
			&WerDumpCustomOptions,
			0
			);
		if (FAILED(hResult))
			__leave;

		hResult = WerReportSubmit(
			hReport,
			WerConsentAlwaysPrompt,
			WER_SUBMIT_QUEUE | WER_SUBMIT_OUTOFPROCESS,
			&WerSubmitResult
			);
		if (FAILED(hResult))
			__leave;

		if (WerReportQueued != WerSubmitResult)
			__leave;

		bRet = TRUE;
	}
	__finally
	{
		if (hReport)
		{
			WerReportCloseHandle(hReport);
			hReport = NULL;
		}
	}

	return bRet;
}

BOOL
	CSimpleDump::GenDump(
	__in _EXCEPTION_POINTERS* pExceptionInfo
	)
{
	BOOL							bRet						= FALSE;

	MINIDUMP_EXCEPTION_INFORMATION	MiniExceptionInfo			= {0};
	HANDLE							hFile						= INVALID_HANDLE_VALUE;
	TCHAR							tchDumpFilePath[MAX_PATH]	= {0};


	__try
	{
		if (!pExceptionInfo)
			__leave;

		if (!CreateDumpFile(tchDumpFilePath, sizeof(tchDumpFilePath)))
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
			ms_MinidumpType,
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

OS_VERSION_USER_DEFINED
	CSimpleDump::GetOSVersion()
{
	OS_VERSION_USER_DEFINED ret = OS_VERSION_UNKNOWN;


	__try
	{
		if (ms_bCanUseGetOSVersionByIsOrGreater)
		{
			ret = GetOSVersionByIsOrGreater();
			if (OS_VERSION_UNKNOWN != ret)
				__leave;
		}

		ret = GetOSVersionByGetVersionEx();
	}
	__finally
	{
		;
	}

	return ret;
}

OS_VERSION_USER_DEFINED
	CSimpleDump::GetOSVersionByIsOrGreater()

{
	OS_VERSION_USER_DEFINED	ret = OS_VERSION_UNKNOWN;


	__try
	{
		if (IsWindowsServer())
		{
			ret = OS_VERSION_WINDOWS_SERVER;
			__leave;
		}

		if (IsWindows1OrGreater())
		{
			ret = OS_VERSION_WINDOWS_10;
			__leave;
		}

		if (IsWindows8Point1OrGreater())
		{
			ret = OS_VERSION_WINDOWS_8_POINT1;
			__leave;
		}

		if (IsWindows8OrGreater())
		{
			ret = OS_VERSION_WINDOWS_8;
			__leave;
		}

		if (IsWindows7SP1OrGreater())
		{
			ret = OS_VERSION_WINDOWS_7_SP1;
			__leave;
		}

		if (IsWindows7OrGreater())
		{
			ret = OS_VERSION_WINDOWS_7;
			__leave;
		}

		if (IsWindowsVistaSP2OrGreater())
		{
			ret = OS_VERSION_WINDOWS_VISTA_PS2;
			__leave;
		}

		if (IsWindowsVistaSP1OrGreater())
		{
			ret = OS_VERSION_WINDOWS_VISTA_PS1;
			__leave;
		}

		if (IsWindowsVistaOrGreater())
		{
			ret = OS_VERSION_WINDOWS_VISTA;
			__leave;
		}

		if (IsWindowsXPSP3OrGreater())
		{
			ret = OS_VERSION_WINDOWS_XP_SP3;
			__leave;
		}

		if (IsWindowsXPSP2OrGreater())
		{
			ret = OS_VERSION_WINDOWS_XP_SP2;
			__leave;
		}

		if (IsWindowsXPSP1OrGreater())
		{
			ret = OS_VERSION_WINDOWS_XP_SP1;
			__leave;
		}

		if (IsWindowsXPOrGreater())
		{
			ret = OS_VERSION_WINDOWS_XP;
			__leave;
		}
	}
	__finally
	{
		;
	}

	return ret;
}

OS_VERSION_USER_DEFINED
	CSimpleDump::GetOSVersionByGetVersionEx()
{
	OS_VERSION_USER_DEFINED	ret				=  OS_VERSION_UNKNOWN;

	OSVERSIONINFOEX			OsVersionInfoEx	= {0};


	__try
	{
		OsVersionInfoEx.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		if (!GetVersionEx((LPOSVERSIONINFO)&OsVersionInfoEx))
			__leave;

		switch (OsVersionInfoEx.dwPlatformId)
		{
		case VER_PLATFORM_WIN32s:
			break;
		case VER_PLATFORM_WIN32_WINDOWS:
			break;
		case VER_PLATFORM_WIN32_NT:
			{
				switch (OsVersionInfoEx.dwMajorVersion)
				{
				case 5:
					{
						switch (OsVersionInfoEx.dwMinorVersion)
						{
						case 0:
							{
								ret = OS_VERSION_WINDOWS_2000;
								break;
							}
						case 1:
							{
								ret = OS_VERSION_WINDOWS_XP;
								break;
							}
						case 2:
							{
								if (VER_NT_WORKSTATION == OsVersionInfoEx.wProductType)
									ret = OS_VERSION_WINDOWS_XP;
								else
								{
									if (VER_SUITE_WH_SERVER == OsVersionInfoEx.wSuiteMask)
										ret = OS_VERSION_WINDOWS_HOME_SERVER;
									else
										ret = OS_VERSION_WINDOWS_SERVER_2003;
								}

								break;
							}
						default:
							break;
						}

						break;
					}
				case 6:
					{
						switch (OsVersionInfoEx.dwMinorVersion)
						{
						case 0:
							{
								if (VER_NT_WORKSTATION == OsVersionInfoEx.wProductType)
									ret = OS_VERSION_WINDOWS_VISTA;
								else
									ret = OS_VERSION_WINDOWS_SERVER_2008;

								break;
							}
						case 1:
							{
								if (VER_NT_WORKSTATION == OsVersionInfoEx.wProductType)
									ret = OS_VERSION_WINDOWS_7;
								else
									ret = OS_VERSION_WINDOWS_SERVER_2008_R2;

								break;
							}
						case 2:
							{
								ret = OS_VERSION_WINDOWS_8;
								break;
							}
						default:
							break;
						}

						break;
					}
				default:
					break;
				}

				break;
			}
		default:
			break;
		}
	}
	__finally
	{
		;
	}

	return ret;
}

OS_PROCESSOR_TYPE_USER_DEFINED
	CSimpleDump::GetOSProcessorType()
{
	OS_PROCESSOR_TYPE_USER_DEFINED	ret			= OS_PROCESSOR_TYPE_UNKNOWN;

	SYSTEM_INFO						systemInfo	= {0};


	__try
	{
		GetNativeSystemInfo(&systemInfo);
		switch (systemInfo.wProcessorArchitecture)
		{
		case PROCESSOR_ARCHITECTURE_INTEL:
			{
				ret = OS_PROCESSOR_TYPE_X86;
				break;
			}
		case PROCESSOR_ARCHITECTURE_AMD64:
			{
				ret = OS_PROCESSOR_TYPE_X64;
				break;
			}
		default:
			__leave;
		}
	}
	__finally
	{
		;
	}

	return ret;
}

BOOL
	CSimpleDump::GetApplicationVersion(
	__in	LPTSTR	lpApplicationPath,
	__inout LPTSTR	lpApplicationVersion,
	__in	ULONG	ulApplicationVersionLen
	)
{
	BOOL				bRet				= FALSE;

	LPVOID				lpFileVerInfo		= NULL;
	DWORD				dwFileVerInfoLen	= 0;
	LPVOID				lpFileVersion		= NULL;
	UINT				nFileVersionLen		= 0;
	VS_FIXEDFILEINFO	VsFixedFileInfo		= {0};


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
	BOOL		bRet		= FALSE;

	HMODULE*	phModule	= NULL;
	DWORD		dwRet		= 0;
	ULONG		ulIndex		= 0;


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
	BOOL	bRet	= FALSE;

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
	BOOL	bRet	= FALSE;

	ULONG	i		= 0;
	HMODULE hModule = NULL;
	ULONG	ulLoop	= 0;


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
		if (!GetWerDllFunc())
			__leave;

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
		ms_hModuleKernel32Dll = LoadLibrary(_T("Kernel32.dll"));
		if (!ms_hModuleKernel32Dll)
			__leave;

		IsWindowsServer = (IS_WINDOWS_SERVER)GetProcAddress(ms_hModuleKernel32Dll, "IsWindowsServer");
		IsWindows1OrGreater = (IS_WINDOWS_10_OR_GREATER)GetProcAddress(ms_hModuleKernel32Dll, "IsWindows1OrGreater");
		IsWindows8Point1OrGreater = (IS_WINDOWS_8_POINT_1_OR_GREATER)GetProcAddress(ms_hModuleKernel32Dll, "IsWindows8Point1OrGreater");
		IsWindows8OrGreater = (IS_WINDOWS_8_OR_GREATER)GetProcAddress(ms_hModuleKernel32Dll, "IsWindows8OrGreater");
		IsWindows7SP1OrGreater = (IS_WINDOWS_7_SP_1_OR_GREATER)GetProcAddress(ms_hModuleKernel32Dll, "IsWindows7SP1OrGreater");
		IsWindows7OrGreater = (IS_WINDOWS_7_OR_GREATER)GetProcAddress(ms_hModuleKernel32Dll, "IsWindows7OrGreater");
		IsWindowsVistaSP2OrGreater = (IS_WINDOWS_VISTA_SP2_OR_GREATER)GetProcAddress(ms_hModuleKernel32Dll, "IsWindowsVistaSP2OrGreater");
		IsWindowsVistaSP1OrGreater = (IS_WINDOWS_VISTA_SP1_OR_GREATER)GetProcAddress(ms_hModuleKernel32Dll, "IsWindowsVistaSP1OrGreater");
		IsWindowsVistaOrGreater = (IS_WINDOWS_VISTA_OR_GREATER)GetProcAddress(ms_hModuleKernel32Dll, "IsWindowsVistaOrGreater");
		IsWindowsXPSP3OrGreater = (IS_WINDOWS_XP_SP3_OR_GREATER)GetProcAddress(ms_hModuleKernel32Dll, "IsWindowsXPSP3OrGreater");
		IsWindowsXPSP2OrGreater = (IS_WINDOWS_XP_SP2_OR_GREATER)GetProcAddress(ms_hModuleKernel32Dll, "IsWindowsXPSP2OrGreater");
		IsWindowsXPSP1OrGreater = (IS_WINDOWS_XP_SP1_OR_GREATER)GetProcAddress(ms_hModuleKernel32Dll, "IsWindowsXPSP1OrGreater");
		IsWindowsXPOrGreater = (IS_WINDOWS_XP_OR_GREATER)GetProcAddress(ms_hModuleKernel32Dll, "IsWindowsXPOrGreater");
		if (IsWindowsXPOrGreater)
			ms_bCanUseGetOSVersionByIsOrGreater = TRUE;

		RegisterApplicationRestart = (REGISTER_APPLICATION_RESTART)GetProcAddress(ms_hModuleKernel32Dll, "RegisterApplicationRestart");
		if (RegisterApplicationRestart)
			ms_bCanUseRegisterRestart = TRUE;

		bRet = TRUE;
	}
	__finally
	{
		;
	}

	return bRet;
}

BOOL
	CSimpleDump::GetWerDllFunc()
{
	BOOL bRet = FALSE;


	__try
	{
		ms_hModuleWerDll = LoadLibrary(_T("Wer.dll"));
		if (!ms_hModuleWerDll)
			__leave;

		WerReportCreate = (WER_REPORT_CREATE)GetProcAddress(ms_hModuleWerDll, "WerReportCreate");
		WerReportSetParameter = (WER_REPORT_SET_PARAMETER)GetProcAddress(ms_hModuleWerDll, "WerReportSetParameter");
		WerReportAddDump = (WER_REPORT_ADD_DUMP)GetProcAddress(ms_hModuleWerDll, "WerReportAddDump");
		WerReportSubmit = (WER_REPORT_SUBMIT)GetProcAddress(ms_hModuleWerDll, "WerReportSubmit");
		WerReportCloseHandle = (WER_REPORT_CLOSE_HANDLE)GetProcAddress(ms_hModuleWerDll, "WerReportCloseHandle");
		if (WerReportCloseHandle)
			ms_bCanUseGenWerReport = TRUE;

		bRet = TRUE;
	}
	__finally
	{
		;
	}

	return bRet;
}

APPLICATION_TYPE
	CSimpleDump::GetApplicationType()
{
	APPLICATION_TYPE	ApplicationType = APPLICATION_TYPE_UNKNOWN;

	HANDLE				hOutput			= INVALID_HANDLE_VALUE;


	__try
	{
		hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
		if (INVALID_HANDLE_VALUE == hOutput)
			__leave;

		if (hOutput)
			ApplicationType = APPLICATION_TYPE_CONSOLE;
		else
			ApplicationType = APPLICATION_TYPE_NOT_CONSOLE;
	}
	__finally
	{
		;
	}

	return ApplicationType;
}

BOOL
	CSimpleDump::InitArgCmdlineInfo(
	__in		APPLICATION_TYPE	ApplicationType,
	__in_opt	int					nArgc,
	__in_opt	LPTSTR				lpArgv[],
	__in_opt	LPTSTR				lpCmdLine,
	__inout_opt	PARG_CMDLINE_INFO	pArgCmdLineInfo,
	__inout		ULONG*				pulBufLen
	)
{
	BOOL	bRet		= FALSE;

	ULONG	ulBufLen	= 0;
	int		i			= 0;


	__try
	{
		// �����С
		switch (ApplicationType)
		{
		case APPLICATION_TYPE_CONSOLE:
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
		case APPLICATION_TYPE_NOT_CONSOLE:
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

		// �Ƚϴ�С
		if (pulBufLen && (*pulBufLen < ulBufLen))
		{
			*pulBufLen = ulBufLen;
			__leave;
		}

		// ��ʼ��
		if (!pArgCmdLineInfo)
		{
			if (pulBufLen)
				*pulBufLen = ulBufLen;

			__leave;
		}

		pArgCmdLineInfo->ApplicationType = ApplicationType;
		switch (pArgCmdLineInfo->ApplicationType)
		{
		case APPLICATION_TYPE_CONSOLE:
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
		case APPLICATION_TYPE_NOT_CONSOLE:
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
	BOOL	bRet				= FALSE;

	ULONG	ulBufLen			= 0;
	int		i					= 0;
	LPTSTR	lpPrePosition		= NULL;
	LPTSTR	lpCurrentPosition	= NULL;
	LPTSTR	lpTemp				= NULL;
	ULONG	ulCalloc			= 0;


	__try
	{
		if (!pArgCmdlineInfo)
			__leave;

		// �����С
		switch (pArgCmdlineInfo->ApplicationType)
		{
		case APPLICATION_TYPE_CONSOLE:
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
		case APPLICATION_TYPE_NOT_CONSOLE:
			{
				ulBufLen = _tcslen(pArgCmdlineInfo->NotConsole.tchCmdLine) * sizeof(TCHAR);
				ulBufLen += sizeof(TCHAR);

				break;
			}
		default:
			__leave;
		}

		// �Ƚϴ�С
		if (pulBufLen && *pulBufLen < ulBufLen)
		{
			*pulBufLen = ulBufLen;
			__leave;
		}

		// ��ʼ��
		if (!lpResult)
		{
			if (pulBufLen)
				*pulBufLen = ulBufLen;

			__leave;
		}

		switch (pArgCmdlineInfo->ApplicationType)
		{
		case APPLICATION_TYPE_CONSOLE:
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
		case APPLICATION_TYPE_NOT_CONSOLE:
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