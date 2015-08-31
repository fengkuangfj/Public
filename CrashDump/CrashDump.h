#pragma once

#define MOD_CRASH_DUMP _T("����ת��")

#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <Shlobj.h>
#include <Shlwapi.h>

#pragma comment(lib, "Shlwapi.lib")

typedef enum _CRASH_DUMP_TYPE
{
	CRASH_DUMP_TYPE_NULL = 0,		// ��
	CRASH_DUMP_TYPE_FULL = 1,		// ��ȫ�ڴ�ת��
	CRASH_DUMP_TYPE_MAIN = 2,		// �����ڴ�ת��
	CRASH_DUMP_TYPE_MINI = 3,		// С�ڴ�ת��
	CRASH_DUMP_TYPE_AUTO = 7		// �Զ��ڴ�ת��
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
	BOOL
		SetType(
		__in CRASH_DUMP_TYPE CrashDumpType
		);

	BOOL
		Store(
		__in LPTSTR lpDumpDir,
		__in LPTSTR lpDriverPath
		);

private:
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