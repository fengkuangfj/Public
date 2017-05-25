
#pragma once

#ifndef MOD_NET_CONTROL
#define MOD_NET_CONTROL	_T("ÍøÂç")
#endif

#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <Winnetwk.h>

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mpr.lib")

#include "..\\PrintfEx\\PrintfEx.h"

class CNetControl
{
public:
	static
		BOOL
		GetLocalIP(
		__inout LPTSTR	lpIP,
		__in	ULONG	ulBufSizeCh
		);

	static
		BOOL
		Connection(
		__in LPTSTR lpLocalName,
		__in LPTSTR lpRemoteName,
		__in LPTSTR lpUserName,
		__in LPTSTR lpPassword,
		__in BOOL	bRemembered = FALSE
		);

	static
		BOOL
		DisConnection(
		__in LPTSTR lpRemoteName
		);

private:
	CNetControl();

	~CNetControl();
};
