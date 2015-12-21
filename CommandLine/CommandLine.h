#pragma once

#include <Windows.h>
#include <tchar.h>

#include "..\\StringInternal\\StringInternal.h"

#define MOD_COMMAND_LINE _T("√¸¡Ó––"£©

typedef struct _CMD_RESULT_INFO
{
	BOOL	bResult;
	LPTSTR	lpResult;
	ULONG	ulResultBufferSizeCh;
	ULONG	ulReturnSizeCh;
} CMD_RESULT_INFO, *PCMD_RESULT_INFO, *LPCMD_RESULT_INFO;

class CCommandLine
{
public:
	static
		BOOL
		Execute(
		__in LPTSTR				lpCmdLine,
		__in BOOL				bWaitUntilCmdExit,
		__in BOOL				bCreateNewConsole,
		__in LPCMD_RESULT_INFO	lpCmdResultInfo
		);
};
