#pragma once

#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <Shlobj.h>
#include <Shlwapi.h>

#include "..\\Public.h"
#include "..\\OperationSystemVersion\\OperationSystemVersion.h"

#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Shell32.lib")

#ifndef MOD_CRASH_DUMP
#define MOD_CRASH_DUMP _T("蓝屏转储")
#endif

typedef enum _CRASH_DUMP_TYPE
{
	CRASH_DUMP_TYPE_NULL = 0,		// 无
	CRASH_DUMP_TYPE_FULL = 1,		// 完全内存转储
	CRASH_DUMP_TYPE_MAIN = 2,		// 核心内存转储
	CRASH_DUMP_TYPE_MINI = 3,		// 小内存转储
	CRASH_DUMP_TYPE_AUTO = 7		// 自动内存转储
} CRASH_DUMP_TYPE, *PCRASH_DUMP_TYPE;

typedef enum _FILE_MODIFY_TIME_CMP
{
	FILE_MODIFY_TIME_CMP_NULL,
	FILE_MODIFY_TIME_CMP_EARLIER,
	FILE_MODIFY_TIME_CMP_EQUAL,
	FILE_MODIFY_TIME_CMP_LATER
} FILE_MODIFY_TIME_CMP, *PFILE_MODIFY_TIME_CMP;

class CCrashDump
{
public:
	static
		BOOL
		SetType(
		__in CRASH_DUMP_TYPE CrashDumpType
		);

	BOOL
		Store(
		__in LPTSTR lpDumpDir,
		__in LPTSTR lpDriverPath
		);

	static
		BOOL
		SetCrashOnRightCtrlAndScrollLockDouble(
		__in PBOOL pbReboot
		);

private:
	CCrashDump();

	~CCrashDump();

	FILE_MODIFY_TIME_CMP
		EqualModifyTime(
		__in LPTSTR lpFrirst,
		__in LPTSTR lpSecend
		);

	BOOL
		GenerateDumpFilePath(
		__in	LPTSTR	lpDumpDir,
		__in	LPTSTR	lpOrgDumpPath,
		__inout LPTSTR	lpNewDumpPath
		);
};
