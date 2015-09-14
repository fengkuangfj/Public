#pragma once

#define MOD_WMI _T("WMI")

#include <Windows.h>
#include <comdef.h>
#include <WbemIdl.h>

#pragma comment(lib, "Wbemuuid.lib")

class CWmi
{
public:
	static
	BOOL
		Query();
};
