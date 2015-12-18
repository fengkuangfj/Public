#pragma once

#ifndef MOD_PROCESS_TYPE
#define MOD_PROCESS_TYPE _T("进程类型")
#endif

#include <Windows.h>
#include <stdio.h>
#include <tchar.h>

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
