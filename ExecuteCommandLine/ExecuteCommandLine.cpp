#include "stdafx.h"


BOOL
	ExecuteCmdLine(
	__in		LPTSTR	lpCmdLine,
	__in_opt	BOOL	bWaitUntilCmdExit
	)
{
	BOOL				bRet				= FALSE;

	UINT				uResult				= 0;
	TCHAR				CmdLine[MAX_PATH]	= {0};
	STARTUPINFO			StartupInfo			= {0};
	PROCESS_INFORMATION	ProcessInfo			= {0};


	__try
	{
		if (!lpCmdLine)
			__leave;

		uResult = GetSystemDirectory(CmdLine, _countof(CmdLine));
		if (!uResult)
			__leave;

		_tcscat_s(CmdLine, _countof(CmdLine), _T("\\cmd.exe /c "));
		_tcscat_s(CmdLine, _countof(CmdLine), lpCmdLine);

		StartupInfo.cb = sizeof(STARTUPINFO);
  		StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
  		StartupInfo.wShowWindow = SW_HIDE;

		if (!CreateProcess(
			NULL,
			CmdLine,
			NULL,
			NULL,
			FALSE,
			CREATE_NEW_CONSOLE,
			NULL,
			NULL,
			&StartupInfo,
			&ProcessInfo
			))
			__leave;

		if (bWaitUntilCmdExit)
		{
			if (ProcessInfo.hProcess)
			{
				if (WAIT_OBJECT_0 != WaitForSingleObject(ProcessInfo.hProcess, INFINITE))
					__leave;
			}
		}

		bRet = TRUE;
	}
	__finally
	{
		if (ProcessInfo.hThread)
			CloseHandle(ProcessInfo.hThread);

		if (ProcessInfo.hProcess)
			CloseHandle(ProcessInfo.hProcess);
	}

	return bRet;
}