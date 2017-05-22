
#pragma once

#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>

#pragma comment(lib, "Version.lib")

typedef struct _FILE_VERSION_INFO
{
	ULONG ulMajorVersion;
	ULONG ulMinorVersion;
	ULONG ulPatchVersion;
	ULONG ulBuildVersion;
} FILE_VERSION_INFO, *PFILE_VERSION_INFO, *LPFILE_VERSION_INFO;

class CFileVersion
{
public:
	static
		BOOL
		Get(
		__in	LPCTSTR	lpPath,
		__inout LPTSTR	lpVersion,
		__in	ULONG	ulBufSizeCh
		);

	static
		BOOL
		Get(
		__in	LPCTSTR				lpPath,
		__inout LPFILE_VERSION_INFO lpFileVersionInfo
		);

private:
	CFileVersion();

	~CFileVersion();
};
