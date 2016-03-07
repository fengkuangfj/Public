#include "ProcessControl.h"

CProcessControl * CProcessControl::ms_pInstance = NULL;

BOOL
	CProcessControl::Get(
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
			printfPublic("input arguments error. %d %d 0x%p %d", bCurrentProc, ulPid, lpOutBuf, ulOutBufSizeCh);
			__leave;
		}

		ZeroMemory(lpOutBuf, ulOutBufSizeCh * sizeof(TCHAR));

		if (bCurrentProc)
		{
			if (!CModulePath::Get(NULL, lpOutBuf, ulOutBufSizeCh))
			{
				printfPublic("Get failed");
				__leave;
			}

			bRet = TRUE;
			__leave;
		}

		hProc = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, ulPid);
		if (!hProc)
		{
			printfPublic("OpenProcess failed. (%d)", GetLastError());
			__leave;
		}

		if (m_QueryFullProcessImageName)
		{
			dwProcPathLenCh = ulOutBufSizeCh;
			if (!m_QueryFullProcessImageName(hProc, 0, lpOutBuf, &dwProcPathLenCh))
			{
				printfPublic("QueryFullProcessImageName failed. (%d)", GetLastError());
				__leave;
			}

			bRet = TRUE;
			__leave;
		}

		if (!GetProcessImageFileName(hProc, tchProcPathDev, _countof(tchProcPathDev)))
		{
			printfPublic("GetProcessImageFileName failed. (%d)", GetLastError());
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
					printfPublic("QueryDosDevice failed. (%d)", GetLastError());
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

CProcessControl *
	CProcessControl::GetInstance()
{
	if (!ms_pInstance)
	{
		do 
		{
			ms_pInstance = new CProcessControl;
			if (!ms_pInstance)
				Sleep(1000);
			else
				break;
		} while (TRUE);
	}

	return ms_pInstance;
}

VOID
	CProcessControl::ReleaseInstance()
{
	if (ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = NULL;
	}
}

CProcessControl::CProcessControl()
{
	if (!Init())
		printfPublic("Init failed");
}

CProcessControl::~CProcessControl()
{
	if (!Unload())
		printfPublic("Unload failed");
}

BOOL
	CProcessControl::Init()
{
	BOOL bRet = FALSE;


	__try
	{
		m_hModuleKernel32 = LoadLibrary(_T("Kernel32.dll"));
		if (!m_hModuleKernel32)
		{
			printfPublic("LoadLibrary failed. (%d)", GetLastError());
			__leave;
		}

		m_QueryFullProcessImageName = (QUERY_FULL_PROCESS_IMAGE_NAME)GetProcAddress(m_hModuleKernel32, "QueryFullProcessImageName");

		m_hModuleNtdll = LoadLibrary(_T("Ntdll.dll"));
		if (!m_hModuleNtdll)
		{
			printfPublic("LoadLibrary failed. (%d)", GetLastError());
			__leave;
		}

		m_NtQueryInformationProcess = (NT_QUERY_INFORMATION_PROCESS)GetProcAddress(m_hModuleNtdll, "NtQueryInformationProcess");
		if (!m_NtQueryInformationProcess)
		{
			printfPublic("GetProcAddress failed. (%d)", GetLastError());
			__leave;
		}

		bRet = TRUE;
	}
	__finally
	{
		if (!bRet)
		{
			if (!Unload())
				printfPublic("Unload failed");
		}
	}

	return bRet;
}

BOOL
	CProcessControl::Unload()
{
	BOOL bRet = TRUE;


	__try
	{
		m_QueryFullProcessImageName = NULL;

		if (m_hModuleKernel32)
		{
			FreeLibrary(m_hModuleKernel32);
			m_hModuleKernel32 = NULL;
		}

		m_NtQueryInformationProcess = NULL;

		if (m_hModuleNtdll)
		{
			FreeLibrary(m_hModuleNtdll);
			m_hModuleNtdll = NULL;
		}
	}
	__finally
	{
		;
	}

	return bRet;
}

BOOL
	CProcessControl::GetSessionId(
	__in	BOOL		bCurrentProc,
	__in	ULONG		ulPid,
	__out	int		* 	pnSid
	)
{
	BOOL	bRet			= FALSE;

	HANDLE	hProcess		= NULL;
	HANDLE	hToken			= NULL;
	DWORD	dwReturnLength	= 0;


	__try
	{
		if ((!bCurrentProc && !ulPid) || !pnSid)
		{
			printfPublic("input arguments error");
			__leave;
		}

		if (bCurrentProc)
			hProcess = GetCurrentProcess();
		else
		{
			hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ulPid);
			if (!hProcess)
			{
				printfPublic("OpenProcess failed. (%d)", GetLastError());
				__leave;
			}
		}

		if (!OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hToken))
		{
			printfPublic("OpenProcessToken failed. (%d)", GetLastError());
			__leave;
		}

		if (!GetTokenInformation(
			hToken,
			TokenSessionId,
			pnSid,
			sizeof(int),
			&dwReturnLength		
			))
		{
			printfPublic("GetTokenInformation failed. (%d)", GetLastError());
			__leave;
		}

		bRet = TRUE;
	}
	__finally
	{
		if (hToken)
		{
			CloseHandle(hToken);
			hToken = NULL;
		}
	}

	return bRet;
}

PROC_TYPE
	CProcessControl::GetProcType(
	__in BOOL	bCurrentProc,
	__in ULONG	ulPid
	)
{
	PROC_TYPE	ProcType	= PROC_TYPE_UNKNOWN;

	HANDLE		hOutPut		= INVALID_HANDLE_VALUE;
	int			nSid		= -1;


	__try
	{
		hOutPut = GetStdHandle(STD_OUTPUT_HANDLE);
		if (INVALID_HANDLE_VALUE == hOutPut)
		{
			printfPublic("GetStdHandle failed. (%d)", GetLastError());
			__leave;
		}

		if (hOutPut)
			ProcType = PROC_TYPE_CONSOLE;
		else
		{
			ProcType = PROC_TYPE_NORMAL;

			if (!GetSessionId(TRUE, 0, &nSid))
			{
				printfPublic("GetSessionId failed");
				__leave;
			}

			if (0 == nSid)
				ProcType = PROC_TYPE_SERVICE;
		}
	}
	__finally
	{
		;
	}

	return ProcType;
}

BOOL
	CProcessControl::Adjust(
	__in BOOL	bCurrentProcess,
	__in ULONG	ulPid,
	__in LPTSTR lpName
	)
{
	BOOL				bRet			= FALSE;

	HANDLE				hProcess		= NULL;
	HANDLE				hToken			= NULL;
	TOKEN_PRIVILEGES	TokenPrivileges = {0};


	__try
	{
		if (!bCurrentProcess && !ulPid)
		{
			printfPublic("input arguments error");
			__leave;
		}

		if (bCurrentProcess)
			hProcess = GetCurrentProcess();
		else
		{
			hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ulPid);
			if (!hProcess)
			{
				printfPublic("OpenProcess failed. (%d)", GetLastError());
				__leave;
			}
		}

		if (!OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES, &hToken))
		{
			printfPublic("OpenProcessToken failed. (%d)", GetLastError());
			__leave;
		}

		if (!LookupPrivilegeValue(NULL, lpName, &TokenPrivileges.Privileges[0].Luid))
		{
			printfPublic("LookupPrivilegeValue failed. (%d)", GetLastError());
			__leave;
		}

		TokenPrivileges.PrivilegeCount = 1;
		TokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		if (!AdjustTokenPrivileges(hToken, FALSE, &TokenPrivileges, sizeof(TokenPrivileges), NULL, NULL))
		{
			printfPublic("AdjustTokenPrivileges failed. (%d)", GetLastError());
			__leave;
		}

		bRet = TRUE;
	}
	__finally
	{
		if (hToken)
		{
			CloseHandle(hToken);
			hToken = NULL;
		}
	}

	return bRet;
}

BOOL
	CProcessControl::RunAs(
	__in LPTSTR lpPath
	)
{
	BOOL				bRet				= FALSE;

	SHELLEXECUTEINFO	ShellExecuteInfo	= {0};


	__try
	{
		if (!lpPath)
		{
			printfPublic("input argument error");
			__leave;
		}

		ShellExecuteInfo.cbSize = sizeof(ShellExecuteInfo);
		ShellExecuteInfo.lpVerb = _T("runas");
		ShellExecuteInfo.lpFile = lpPath;
		ShellExecuteInfo.nShow = SW_SHOWNORMAL;

		if (!ShellExecuteEx(&ShellExecuteInfo))
		{
			printfPublic("ShellExecuteEx failed. (%d)", GetLastError());
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
	CProcessControl::Raise(
	__in BOOL	bCurrentProcess,
	__in ULONG	ulPid
	)
{
	BOOL bRet = FALSE;


	__try
	{
		if (!bCurrentProcess && !ulPid)
		{
			printfPublic("input arguments error");
			__leave;
		}

		Adjust(bCurrentProcess, ulPid, SE_CREATE_TOKEN_NAME          );
		Adjust(bCurrentProcess, ulPid, SE_ASSIGNPRIMARYTOKEN_NAME    );
		Adjust(bCurrentProcess, ulPid, SE_LOCK_MEMORY_NAME           );
		Adjust(bCurrentProcess, ulPid, SE_INCREASE_QUOTA_NAME        );
		// Adjust(bCurrentProcess, ulPid, SE_UNSOLICITED_INPUT_NAME     );
		Adjust(bCurrentProcess, ulPid, SE_MACHINE_ACCOUNT_NAME       );
		Adjust(bCurrentProcess, ulPid, SE_TCB_NAME                   );
		Adjust(bCurrentProcess, ulPid, SE_SECURITY_NAME              );
		Adjust(bCurrentProcess, ulPid, SE_TAKE_OWNERSHIP_NAME        );
		Adjust(bCurrentProcess, ulPid, SE_LOAD_DRIVER_NAME           );
		Adjust(bCurrentProcess, ulPid, SE_SYSTEM_PROFILE_NAME        );
		Adjust(bCurrentProcess, ulPid, SE_SYSTEMTIME_NAME            );
		Adjust(bCurrentProcess, ulPid, SE_PROF_SINGLE_PROCESS_NAME   );
		Adjust(bCurrentProcess, ulPid, SE_INC_BASE_PRIORITY_NAME     );
		Adjust(bCurrentProcess, ulPid, SE_CREATE_PAGEFILE_NAME       );
		Adjust(bCurrentProcess, ulPid, SE_CREATE_PERMANENT_NAME      );
		Adjust(bCurrentProcess, ulPid, SE_BACKUP_NAME                );
		Adjust(bCurrentProcess, ulPid, SE_RESTORE_NAME               );
		Adjust(bCurrentProcess, ulPid, SE_SHUTDOWN_NAME              );
		Adjust(bCurrentProcess, ulPid, SE_DEBUG_NAME                 );
		Adjust(bCurrentProcess, ulPid, SE_AUDIT_NAME                 );
		Adjust(bCurrentProcess, ulPid, SE_SYSTEM_ENVIRONMENT_NAME    );
		Adjust(bCurrentProcess, ulPid, SE_CHANGE_NOTIFY_NAME         );
		Adjust(bCurrentProcess, ulPid, SE_REMOTE_SHUTDOWN_NAME       );
		Adjust(bCurrentProcess, ulPid, SE_UNDOCK_NAME                );
		Adjust(bCurrentProcess, ulPid, SE_SYNC_AGENT_NAME            );
		Adjust(bCurrentProcess, ulPid, SE_ENABLE_DELEGATION_NAME     );
		Adjust(bCurrentProcess, ulPid, SE_MANAGE_VOLUME_NAME         );
		Adjust(bCurrentProcess, ulPid, SE_IMPERSONATE_NAME           );
		Adjust(bCurrentProcess, ulPid, SE_CREATE_GLOBAL_NAME         );
		Adjust(bCurrentProcess, ulPid, SE_TRUSTED_CREDMAN_ACCESS_NAME);
		Adjust(bCurrentProcess, ulPid, SE_RELABEL_NAME               );
		Adjust(bCurrentProcess, ulPid, SE_INC_WORKING_SET_NAME       );
		Adjust(bCurrentProcess, ulPid, SE_TIME_ZONE_NAME             );
		Adjust(bCurrentProcess, ulPid, SE_CREATE_SYMBOLIC_LINK_NAME  );

		bRet = TRUE;
	}
	__finally
	{
		;
	}

	return bRet;
}

VOID
	CProcessControl::DeleteMyselfBySHChangeNotify()
{
	TCHAR tchProcPath[MAX_PATH]	= {0};


	__try
	{
		if (!CModulePath::Get(NULL, tchProcPath, _countof(tchProcPath)))
		{
			printfPublic("CModulePath::Get failed");
			__leave;
		}

		// 直接调用SHChangeNotify可以立即将进程的exe删掉，但有条件
		// 1、可以调用SHChangeNotify
		// 2、explorer正在运行
		SHChangeNotify(SHCNE_DELETE, SHCNF_PATH, tchProcPath, NULL);
	}
	__finally
	{
		;
	}

	return ;
}

VOID
	CProcessControl::DeleteMyselfByCreateProcess()
{
	TCHAR				tchCmd[MAX_PATH]		= {0};
	TCHAR				tchProcPath[MAX_PATH]	= {0};
	STARTUPINFO			StartupInfo				= {0};
	PROCESS_INFORMATION	ProcInfo				= {0};


	__try
	{
		if (!GetEnvironmentVariable(_T("COMSPEC"), tchCmd, _countof(tchCmd)))
		{
			printfPublic("GetEnvironmentVariable failed. (%d)", GetLastError());
			__leave;
		}

		if (!CModulePath::Get(NULL, tchProcPath, _countof(tchProcPath)))
		{
			printfPublic("GetModuleFileName failed. (%d)", GetLastError());
			__leave;
		}

		_tcscat_s(tchCmd, _countof(tchCmd), _T(" /c del \""));
		_tcscat_s(tchCmd, _countof(tchCmd), tchProcPath);
		_tcscat_s(tchCmd, _countof(tchCmd), _T("\" /f /q"));

		if (!SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS))
		{
			printfPublic("SetPriorityClass failed. (%d)", GetLastError());
			__leave;
		}

		if (!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL))
		{
			printfPublic("SetThreadPriority failed. (%d)", GetLastError());
			__leave;
		}

		StartupInfo.cb = sizeof(STARTUPINFO);
		StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
		StartupInfo.wShowWindow = SW_HIDE;

		if (!CreateProcess(
			NULL,
			tchCmd,
			NULL,
			NULL,
			FALSE,
			CREATE_SUSPENDED | DETACHED_PROCESS,
			NULL,
			NULL,
			&StartupInfo,
			&ProcInfo
			))
		{
			printfPublic("CreateProcess failed. (%d)", GetLastError());
			__leave;
		}

		if (!SetPriorityClass(ProcInfo.hProcess, IDLE_PRIORITY_CLASS))
		{
			printfPublic("SetPriorityClass failed. (%d)", GetLastError());
			__leave;
		}

		if (!SetThreadPriority(ProcInfo.hThread, THREAD_PRIORITY_IDLE))
		{
			printfPublic("SetThreadPriority failed. (%d)", GetLastError());
			__leave;
		}

		if (-1 == ResumeThread(ProcInfo.hThread))
		{
			printfPublic("ResumeThread failed. (%d)", GetLastError());
			__leave;
		}
	}
	__finally
	{
		if (ProcInfo.hProcess)
			CloseHandle(ProcInfo.hProcess);

		if (ProcInfo.hThread)
			CloseHandle(ProcInfo.hThread);
	}

	return ;
}

BOOL
	CProcessControl::GetParentPid(
	__in	ULONG	ulPid,
	__out	PULONG	pulParentPid 
	)
{
	BOOL						bRet = FALSE;

	HANDLE						hProcess = NULL;
	HANDLE						hProcessParent = NULL;
	PROCESS_BASIC_INFORMATION	ProcessBasicInfo = {0};
	ULONG						ulRet = 0;
	NTSTATUS					ntStatus = 0;
	FILETIME					CreateTime		= {0};
	FILETIME					ExitTime			= {0};
	FILETIME					KernelTime		= {0};
	FILETIME					USerTime			= {0};
	FILETIME					ParentCreateTime		= {0};
	FILETIME					ParentExitTime			= {0};
	FILETIME					ParentKernelTime		= {0};
	FILETIME					ParentUSerTime			= {0};


	__try
	{
		if (!ulPid || !pulParentPid)
		{
			printfPublic("input arguments error. ulPid(%d) pulParentPid(0x%p)", ulPid, pulParentPid);
			__leave;
		}

		hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, ulPid);
		if (!hProcess)
		{
			printfPublic("OpenProcess failed. (%d)", GetLastError());
			__leave;
		}

		ntStatus = m_NtQueryInformationProcess(
			 hProcess,
			 ProcessBasicInformation,
			 &ProcessBasicInfo,
			 sizeof(ProcessBasicInfo),
			 &ulRet			 
			 );
		if (!NT_SUCCESS(ntStatus))
		{
			printfPublic("m_NtQueryInformationProcess failed. (0x%x)", ntStatus);
			__leave;
		}

		hProcessParent = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, (DWORD)ProcessBasicInfo.Reserved3);
		if (!hProcessParent)
			__leave;

		if (!ProcessBasicInfo.UniqueProcessId)
		{
			bRet = TRUE;
			__leave;
		}
	
		if (!GetProcessTimes(
			hProcessParent,
			&ParentCreateTime,
			&ParentExitTime,
			&ParentKernelTime,
			&ParentUSerTime
			))
		{
			printfPublic("GetProcessTimes failed. (%d)", GetLastError());
			__leave;
		}

		if (!GetProcessTimes(
			hProcess,
			&CreateTime,
			&ExitTime,
			&KernelTime,
			&USerTime
			))
		{
			printfPublic("GetProcessTimes failed. (%d)", GetLastError());
			__leave;
		}

		if (-1 != CompareFileTime(&ParentCreateTime, &CreateTime))
			__leave;

		*pulParentPid = (ULONG)ProcessBasicInfo.Reserved3;

		bRet = TRUE;
	}
	__finally
	{
		if (hProcess)
		{
			CloseHandle(hProcess);
			hProcess = NULL;
		}

		if (hProcessParent)
		{
			CloseHandle(hProcessParent);
			hProcessParent = NULL;
		}
	}
	
	return bRet;
}
