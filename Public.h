#pragma once

#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <locale.h>
#include <time.h>
#include <stdio.h>
#include <objbase.h>

#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "User32.lib")

#ifndef MOD_PUBLIC
#define MOD_PUBLIC _T("¹«¹²")
#endif

#define printfPublic(FMT, ...) printfPublicEx(__FILE__, __FUNCSIG__, __LINE__, FMT, __VA_ARGS__)

VOID
printfPublicEx(
			   __in LPSTR lpFile,
			   __in LPSTR lpFunction,
			   __in ULONG ulLine,
			   __in LPSTR lpFmt,
			   ...
			   );

BOOL
GenGuid(
		__inout LPTSTR	lpGuid,
		__in	ULONG	ulBufSizeCh
		);
