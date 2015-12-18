#include "ProcessType.h"

PROC_TYPE
	CProcessType::GetProcType(
	__in BOOL	bCurrentProc,
	__in ULONG	ulPid
	)
{
	PROC_TYPE	ProcType	= PROC_TYPE_UNKNOWN;

	HANDLE		hOutPut		= INVALID_HANDLE_VALUE;


	__try
	{
		ProcType = PROC_TYPE_SERVICE;

		hOutPut = GetStdHandle(STD_OUTPUT_HANDLE);
		if (INVALID_HANDLE_VALUE == hOutPut)
		{
			printf("GetStdHandle failed. (%d) \n", GetLastError());
			__leave;
		}

		if (hOutPut)
			ProcType = PROC_TYPE_CONSOLE;
		else
			ProcType = PROC_TYPE_NORMAL;
	}
	__finally
	{
		;
	}

	return ProcType;
}
