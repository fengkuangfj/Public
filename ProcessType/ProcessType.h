#pragma once

#include <Windows.h>
#include <stdio.h>

#pragma comment(lib, "Advapi32.lib")

#ifndef MOD_PROCESS_TYPE
#define MOD_PROCESS_TYPE _T("��������")
#endif

typedef enum _PROC_TYPE
{
	PROC_TYPE_UNKNOWN,
	PROC_TYPE_NORMAL,
	PROC_TYPE_CONSOLE,
	PROC_TYPE_SERVICE
} PROC_TYPE, *PPROC_TYPE, *LPPROC_TYPE;

class CProcessType
{
public:
	static
		PROC_TYPE
		GetProcType(
		__in BOOL	bCurrentProc,
		__in ULONG	ulPid
		);

private:
	static
		BOOL
		GetSessionId(
		__in	BOOL		bCurrentProc,
		__in	ULONG		ulPid,
		__out	int		*	pnSid
		);
};
