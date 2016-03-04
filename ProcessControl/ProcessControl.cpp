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
		printf("Init failed \n");
}

CProcessControl::~CProcessControl()
{
	if (!Unload())
		printf("Unload failed \n");
}

BOOL
	CProcessControl::Init()
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
	CProcessControl::Unload()
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
			printf("input arguments error. \n");
			__leave;
		}

		if (bCurrentProc)
			hProcess = GetCurrentProcess();
		else
		{
			hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ulPid);
			if (!hProcess)
			{
				printf("OpenProcess failed. (%d) \n", GetLastError());
				__leave;
			}
		}

		if (!OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hToken))
		{
			printf("OpenProcessToken failed. (%d) \n", GetLastError());
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
			printf("GetTokenInformation failed. (%d) \n", GetLastError());
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
			printf("GetStdHandle failed. (%d) \n", GetLastError());
			__leave;
		}

		if (hOutPut)
			ProcType = PROC_TYPE_CONSOLE;
		else
		{
			ProcType = PROC_TYPE_NORMAL;

			if (!GetSessionId(TRUE, 0, &nSid))
			{
				printf("GetSessionId failed. \n");
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
			// printfEx(MOD_PROCESS_CONTROL, PRINTF_LEVEL_ERROR, "input arguments error");
			__leave;
		}

		if (bCurrentProcess)
			hProcess = GetCurrentProcess();
		else
		{
			hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ulPid);
			if (!hProcess)
			{
				// printfEx(MOD_PROCESS_CONTROL, PRINTF_LEVEL_ERROR, "OpenProcess failed. (%d)", GetLastError());
				__leave;
			}
		}

		if (!OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES, &hToken))
		{
			// printfEx(MOD_PROCESS_CONTROL, PRINTF_LEVEL_ERROR, "OpenProcessToken failed. (%d)", GetLastError());
			__leave;
		}

		if (!LookupPrivilegeValue(NULL, lpName, &TokenPrivileges.Privileges[0].Luid))
		{
			// printfEx(MOD_PROCESS_CONTROL, PRINTF_LEVEL_ERROR, "LookupPrivilegeValue failed. (%d)", GetLastError());
			__leave;
		}

		TokenPrivileges.PrivilegeCount = 1;
		TokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		if (!AdjustTokenPrivileges(hToken, FALSE, &TokenPrivileges, sizeof(TokenPrivileges), NULL, NULL))
		{
			// printfEx(MOD_PROCESS_CONTROL, PRINTF_LEVEL_ERROR, "AdjustTokenPrivileges failed. (%d)", GetLastError());
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
			// printfEx(MOD_PROCESS_CONTROL, PRINTF_LEVEL_ERROR, "input argument error");
			__leave;
		}

		ShellExecuteInfo.cbSize = sizeof(ShellExecuteInfo);
		ShellExecuteInfo.lpVerb = _T("runas");
		ShellExecuteInfo.lpFile = lpPath;
		ShellExecuteInfo.nShow = SW_SHOWNORMAL;

		if (!ShellExecuteEx(&ShellExecuteInfo))
		{
			// printfEx(MOD_PROCESS_CONTROL, PRINTF_LEVEL_ERROR, "ShellExecuteEx failed. (%d)", GetLastError());
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
			// printfEx(MOD_PROCESS_CONTROL, PRINTF_LEVEL_ERROR, "input arguments error");
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
		if (!GetModuleFileName(NULL, tchProcPath, MAX_PATH))
		{
			printf("[DeleteMyself] : GetModuleFileName failed. (%d) \n", GetLastError());
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
		if (!GetEnvironmentVariable(_T("COMSPEC"), tchCmd, MAX_PATH))
		{
			printf("[DeleteMyself] : GetEnvironmentVariable failed. (%d) \n", GetLastError());
			__leave;
		}

		if (!GetModuleFileName(NULL, tchProcPath, MAX_PATH))
		{
			printf("[DeleteMyself] : GetModuleFileName failed. (%d) \n", GetLastError());
			__leave;
		}

		_tcscat_s(tchCmd, MAX_PATH, _T(" /c del \""));
		_tcscat_s(tchCmd, MAX_PATH, tchProcPath);
		_tcscat_s(tchCmd, MAX_PATH, _T("\""));

		// 设置本程序进程的执行级别为实时执行，这本程序马上获取CPU执行权，快速退出。
		if (!SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS))
		{
			printf("[DeleteMyself] : SetPriorityClass failed. (%d) \n", GetLastError());
			__leave;
		}

		// 		if (!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL))
		// 		{
		// 			printf("[DeleteMyself] : SetThreadPriority failed. (%d) \n", GetLastError());
		// 			__leave;
		// 		}

		StartupInfo.cb = sizeof(STARTUPINFO);
		StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
		StartupInfo.wShowWindow = SW_HIDE;

		if (!CreateProcess(NULL, tchCmd, NULL, NULL, FALSE, CREATE_SUSPENDED | CREATE_NO_WINDOW | DETACHED_PROCESS, NULL, NULL, &StartupInfo, &ProcInfo))
		{
			printf("[DeleteMyself] : CreateProcess failed. (%d) \n", GetLastError());
			__leave;
		}

		// 设置命令行进程的执行级别为空闲执行,这使本程序有足够的时间从内存中退出。
		if (!SetPriorityClass(ProcInfo.hProcess, IDLE_PRIORITY_CLASS))
		{
			printf("[DeleteMyself] : SetPriorityClass failed. (%d) \n", GetLastError());
			__leave;
		}

		// 		if (!SetThreadPriority(ProcInfo.hThread, THREAD_PRIORITY_IDLE))
		// 		{
		// 			printf("[DeleteMyself] : SetThreadPriority failed. (%d) \n", GetLastError());
		// 			__leave;
		// 		}

		if (ResumeThread(ProcInfo.hThread) == -1)
		{
			printf("[DeleteMyself] : ResumeThread failed. (%d) \n", GetLastError());
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

	ExitProcess(ERROR_SUCCESS);

	return ;
}
