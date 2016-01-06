#include "ProcessType.h"

BOOL
	CProcessType::GetSessionId(
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
	CProcessType::GetProcType(
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

CProcessType::CProcessType()
{
	;
}

CProcessType::~CProcessType()
{
	;
}
