#pragma once

#include <Windows.h>
#include <tchar.h>

#define MOD_COMMAND_LINE _T("√¸¡Ó––"£©

class CCommandLine
{
public:
	static
		BOOL
		Execute(
		__in LPTSTR	lpCmdLine,
		__in BOOL	bWaitUntilCmdExit
		);
};
