#pragma once

#define MOD_WMI _T("WMI")

#include <Windows.h>
#include <comdef.h>
#include <WbemIdl.h>

#include "..\\PrintfEx\\PrintfEx.h"

#pragma comment(lib, "Wbemuuid.lib")

class CWmi
{
public:
	static
		BOOL
		Query(
		__in LPSTR	lpClass,
		__in LPTSTR lpContent
		);
};
