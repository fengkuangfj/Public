
#pragma once

#ifndef MOD_NET_CONTROL
#define MOD_NET_CONTROL	_T("ÍøÂç")
#endif

#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>

#pragma comment(lib, "Ws2_32.lib")

class CNetControl
{
public:
	static
		BOOL
		GetLocalIP(
		__inout LPTSTR	lpIP,
		__in	ULONG	ulBufSizeCh
		);

private:
	CNetControl();

	~CNetControl();
};
