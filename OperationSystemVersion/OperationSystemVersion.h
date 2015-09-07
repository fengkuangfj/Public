#pragma once

#include <Windows.h>

#define MOD_OPERATION_SYSTEM_VERSION _T("操作系统版本")

enum OS_VER
{
	OS_VER_UNKNOWN,
	OS_VER_WINDOWS_XP,
	OS_VER_WINDOWS_7,
	OS_VER_WINDOWS_8
};

enum OS_PROC_TYPE
{
	OS_PROC_TYPE_UNKNOWN,
	OS_PROC_TYPE_X86,
	OS_PROC_TYPE_X64
};

enum OS_VER_AND_PROC_TYPE
{
	OS_VER_AND_PROC_TYPE_UNKNOWN,
	OS_VER_AND_PROC_TYPE_WINDOWS_XP_X86,
	OS_VER_AND_PROC_TYPE_WINDOWS_XP_X64,
	OS_VER_AND_PROC_TYPE_WINDOWS_7_X86,
	OS_VER_AND_PROC_TYPE_WINDOWS_7_X64
};

class COperationSystemVersion
{
public:
	BOOL
		Init();

	OS_VER
		GetOSVer(
		__out_opt OSVERSIONINFO* pOsVerInfo = NULL
		);

	OS_PROC_TYPE
		GetOSProcType();

	OS_VER_AND_PROC_TYPE
		GetOsVerAndProcType();

private:
	static OS_VER			ms_OsVer;
	static OS_PROC_TYPE		ms_OsProcType;
	static BOOL				ms_bInitOsVer;
	static BOOL				ms_bInitOsProcType;
	static OSVERSIONINFO	ms_OsVerInfo;
};
