#pragma once

#include <Windows.h>

#include "..\\PrintfEx\\PrintfEx.h"

#define MOD_PROCESS_PRIVILEGE _T("进程权限")

class CProcessPrivilege
{
public:
	static
		BOOL
		Adjust(
		__in BOOL	bCurrentProcess,
		__in ULONG	ulPid,
		__in LPTSTR lpName
		);

	static
		BOOL
		RunAs(
		__in LPTSTR lpPath
		);
};
