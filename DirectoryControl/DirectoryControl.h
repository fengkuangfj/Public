
#pragma once

#include <Windows.h>
#include <shlwapi.h>

#pragma comment(lib, "shlwapi.lib")

#include "..\\PrintfEx\\PrintfEx.h"

#define MOD_DIRECTORY_CONTROL _T("Ŀ¼����")

class CDirectoryControl
{
public:
	static
		BOOL
		Delete(
		__in LPTSTR lptchDirPath
		);
};
