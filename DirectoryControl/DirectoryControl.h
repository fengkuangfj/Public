
#pragma once

#include <Windows.h>
#include <shlwapi.h>

#pragma comment(lib, "shlwapi.lib")

#include "..\\PrintfEx\\PrintfEx.h"

#define MOD_DIRECTORY_CONTROL _T("Ä¿Â¼¿ØÖÆ")

class CDirectoryControl
{
public:
	static
		BOOL
		Delete(
		__in LPTSTR lptchDirPath
		);

	static
		BOOL
		Empty(
		__in LPTSTR lptchDirPath
		);

	static
		BOOL
		DeleteInternalFile(
		__in LPTSTR	lptchDirPath,
		__in LPTSTR	lpFileName,
		__in BOOL	bWildcard
		);

	static
		BOOL
		DeleteInternalDir(
		__in LPTSTR	lptchDirPath,
		__in LPTSTR	lpDirName,
		__in BOOL		bWildcard
		);

	static
		BOOL
		EmptyExceptFile(
		__in LPTSTR	lptchDirPath,
		__in LPTSTR	lptchFileName,
		__in BOOL	bWildcard
		);

	static
		BOOL
		DeleteExceptFile(
		__in LPTSTR	lptchDirPath,
		__in LPTSTR	lptchFileName,
		__in BOOL	bWildcard
		);

private:
	static
		BOOL
		Control(
		__in LPTSTR lptchDirPath,
		__in BOOL	bDelete
		);
};
