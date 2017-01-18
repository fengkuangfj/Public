
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

private:
	static
		BOOL
		Delete(
		__in LPTSTR lptchDirPath,
		__in BOOL	bDelete
		);
};
