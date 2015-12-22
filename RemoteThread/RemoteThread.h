#pragma once

#ifndef MOD_REMOTE_THREAD
#define MOD_REMOTE_THREAD _T("Ô¶³ÌÏß³Ì")
#endif

#include <Windows.h>

#include "..\\PrintfEx\\PrintfEx.h"

class CRemoteThread
{
public:
	static
		BOOL
		Inject(
		__in DWORD					dwPid,
		__in LPTHREAD_START_ROUTINE lpStartAddress,
		__in LPTHREAD_START_ROUTINE	lpStartAddressFollow,
		__in LPVOID					lpParameter,
		__in DWORD					dwParameterSizeB
		);
};
