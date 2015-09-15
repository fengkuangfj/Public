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
// 	BOOL
// 		Init();
// 
// 	BOOL
// 		Unload();

	static
		BOOL
		Query(
		__in LPSTR	lpClass,
		__in LPTSTR lpContent
		);

	static
		BOOL
		QueryCaption(
		__in	ULONG	ulDiskNumber,
		__out	LPTSTR	lpOutBuf,
		__in	ULONG	ulOutBufSizeCh
		);
};
