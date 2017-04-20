#include "CommandLine.h"

BOOL
CCommandLine::Execute(
					  __in LPTSTR				lpCmdLine,
					  __in BOOL				bWaitUntilCmdExit,
					  __in BOOL				bCreateNewConsole,
					  __in LPCMD_RESULT_INFO	lpCmdResultInfo
					  )
{
	BOOL				bRet							= FALSE;

	UINT				uResult							= 0;
	TCHAR				CmdLine[MAX_PATH]				= {0};
	STARTUPINFO			StartupInfo						= {0};
	SECURITY_ATTRIBUTES SecurityAttributes				= {0};
	HANDLE				hReadStdOutput					= NULL;
	HANDLE				hReadStdError					= NULL;
	PROCESS_INFORMATION	ProcessInfo						= {0};
	CHAR				chStdOutputRead[4096]			= {0};
	TCHAR				tchStdOutputRead[4096]			= {0};
	ULONG				ulStdOutputReadSizeCh			= 0;
	DWORD				dwStdOutputNumberOfBytesRead	= 0;
	ULONG				ulLoopReadStdOutput				= 0;
	DWORD				dwResultWaitForSingleObject		= 0;


	__try
	{
		if (!lpCmdLine)
		{
			printfPublic("input argument error");
			__leave;
		}

		if (lpCmdResultInfo)
		{
			if (!bWaitUntilCmdExit)
			{
				printfPublic("input argument error");
				__leave;
			}
		}

		uResult = GetSystemDirectory(CmdLine, _countof(CmdLine));
		if (!uResult)
		{
			printfPublic("GetSystemDirectory failed. (%d)", GetLastError());
			__leave;
		}

		_tcscat_s(CmdLine, _countof(CmdLine), _T("\\cmd.exe /c "));
		_tcscat_s(CmdLine, _countof(CmdLine), lpCmdLine);

		GetStartupInfo(&StartupInfo);

		StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
		StartupInfo.wShowWindow = SW_HIDE;

		if (lpCmdResultInfo)
		{
			StartupInfo.dwFlags |= STARTF_USESTDHANDLES;

			SecurityAttributes.nLength = sizeof(SecurityAttributes);
			SecurityAttributes.lpSecurityDescriptor = NULL;
			SecurityAttributes.bInheritHandle = TRUE;

			if (!CreatePipe(&hReadStdOutput, &StartupInfo.hStdOutput, &SecurityAttributes, 0))
			{
				printfPublic("CreatePipe failed. (%d)", GetLastError());
				__leave;
			}

			if (!SetHandleInformation(hReadStdOutput, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT))
			{
				printfPublic("SetHandleInformation failed. (%d)", GetLastError());
				__leave;
			}

			if (!CreatePipe(&hReadStdError, &StartupInfo.hStdError, &SecurityAttributes, 0))
			{
				printfPublic("CreatePipe failed. (%d)", GetLastError());
				__leave;
			}

			if (!SetHandleInformation(hReadStdError, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT))
			{
				printfPublic("SetHandleInformation failed. (%d)", GetLastError());
				__leave;
			}
		}

		if (!CreateProcess(
			NULL,
			CmdLine,
			NULL,
			NULL,
			TRUE,
			bCreateNewConsole ? CREATE_NEW_CONSOLE : DETACHED_PROCESS,
			NULL,
			NULL,
			&StartupInfo,
			&ProcessInfo
			))
		{
			printfPublic("CreateProcess failed. (%d)", GetLastError());
			__leave;
		}

		if (lpCmdResultInfo)
		{
			if (StartupInfo.hStdOutput)
			{
				CloseHandle(StartupInfo.hStdOutput);
				StartupInfo.hStdOutput = NULL;
			}

			if (StartupInfo.hStdError)
			{
				CloseHandle(StartupInfo.hStdError);
				StartupInfo.hStdError = NULL;
			}
		}

		if (bWaitUntilCmdExit)
		{
			if (ProcessInfo.hProcess)
			{
				dwResultWaitForSingleObject = WaitForSingleObject(ProcessInfo.hProcess, INFINITE);
				if (WAIT_OBJECT_0 != dwResultWaitForSingleObject)
				{
					printfPublic("WaitForSingleObject failed. (%d)", dwResultWaitForSingleObject);
					__leave;
				}
			}
		}

		if (lpCmdResultInfo)
		{
			__try
			{
				while (TRUE)
				{
					if (!ReadFile(hReadStdOutput, chStdOutputRead, 4096, &dwStdOutputNumberOfBytesRead, NULL))
					{
						if (ulLoopReadStdOutput)
							lpCmdResultInfo->bResult = TRUE;

						break;
					}

					ulStdOutputReadSizeCh = _countof(tchStdOutputRead);
					if (!CStringInternal::ASCIIToUNICODE(tchStdOutputRead, &ulStdOutputReadSizeCh, chStdOutputRead, strlen(chStdOutputRead), CP_ACP))
					{
						printfPublic("CStringInternal::ASCIIToUNICODE failed");
						__leave;
					}

					if (lpCmdResultInfo->lpResult && lpCmdResultInfo->ulResultBufferSizeCh)
					{
						if (ulLoopReadStdOutput)
						{
							lpCmdResultInfo->ulReturnSizeCh += (ULONG)_tcslen(_T("\n"));
							if (lpCmdResultInfo->ulReturnSizeCh > lpCmdResultInfo->ulResultBufferSizeCh)
							{
								lpCmdResultInfo->bResult = TRUE;
								__leave;
							}

							CopyMemory(lpCmdResultInfo->lpResult + lpCmdResultInfo->ulReturnSizeCh - _tcslen(_T("\n")), _T("\n"), _tcslen(_T("\n")) * sizeof(TCHAR));
						}

						lpCmdResultInfo->ulReturnSizeCh += (ULONG)_tcslen(tchStdOutputRead);
						if (lpCmdResultInfo->ulReturnSizeCh > lpCmdResultInfo->ulResultBufferSizeCh)
						{
							lpCmdResultInfo->bResult = TRUE;
							__leave;
						}

						CopyMemory(lpCmdResultInfo->lpResult + lpCmdResultInfo->ulReturnSizeCh - _tcslen(tchStdOutputRead), tchStdOutputRead, _tcslen(tchStdOutputRead) * sizeof(TCHAR));
					}

					ulLoopReadStdOutput++;

					ZeroMemory(chStdOutputRead, sizeof(chStdOutputRead));
					ZeroMemory(tchStdOutputRead, sizeof(tchStdOutputRead));
				}

				if (!ulLoopReadStdOutput)
				{
					while (TRUE)
					{
						if (!ReadFile(hReadStdError, chStdOutputRead, 4096, &dwStdOutputNumberOfBytesRead, NULL))
							break;

						ulStdOutputReadSizeCh = _countof(tchStdOutputRead);
						if (!CStringInternal::ASCIIToUNICODE(tchStdOutputRead, &ulStdOutputReadSizeCh, chStdOutputRead, strlen(chStdOutputRead), CP_ACP))
						{
							printfPublic("CStringInternal::ASCIIToUNICODE failed");
							__leave;
						}

						if (lpCmdResultInfo->lpResult && lpCmdResultInfo->ulResultBufferSizeCh)
						{
							if (ulLoopReadStdOutput)
							{
								lpCmdResultInfo->ulReturnSizeCh += (ULONG)_tcslen(_T("\n"));
								if (lpCmdResultInfo->ulReturnSizeCh > lpCmdResultInfo->ulResultBufferSizeCh)
									__leave;

								CopyMemory(lpCmdResultInfo->lpResult + lpCmdResultInfo->ulReturnSizeCh - _tcslen(_T("\n")), _T("\n"), _tcslen(_T("\n")) * sizeof(TCHAR));
							}

							lpCmdResultInfo->ulReturnSizeCh += (ULONG)_tcslen(tchStdOutputRead);
							if (lpCmdResultInfo->ulReturnSizeCh > lpCmdResultInfo->ulResultBufferSizeCh)
								__leave;

							CopyMemory(lpCmdResultInfo->lpResult + lpCmdResultInfo->ulReturnSizeCh - _tcslen(tchStdOutputRead), tchStdOutputRead, _tcslen(tchStdOutputRead) * sizeof(TCHAR));
						}

						ulLoopReadStdOutput++;

						ZeroMemory(chStdOutputRead, sizeof(chStdOutputRead));
						ZeroMemory(tchStdOutputRead, sizeof(tchStdOutputRead));
					}
				}

				bRet = TRUE;
			}
			__finally
			{
				;
			}
		}
		else
			bRet = TRUE;
	}
	__finally
	{
		if (StartupInfo.hStdOutput)
		{
			CloseHandle(StartupInfo.hStdOutput);
			StartupInfo.hStdOutput = NULL;
		}

		if (StartupInfo.hStdError)
		{
			CloseHandle(StartupInfo.hStdError);
			StartupInfo.hStdError = NULL;
		}

		if (hReadStdOutput)
		{
			CloseHandle(hReadStdOutput);
			hReadStdOutput = NULL;
		}

		if (ProcessInfo.hThread)
		{
			CloseHandle(ProcessInfo.hThread);
			ProcessInfo.hThread = NULL;
		}

		if (ProcessInfo.hProcess)
		{
			CloseHandle(ProcessInfo.hProcess);
			ProcessInfo.hProcess = NULL;
		}
	}

	return bRet;
}

CCommandLine::CCommandLine()
{
	;
}

CCommandLine::~CCommandLine()
{
	;
}
