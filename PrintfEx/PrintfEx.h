#pragma once

#include <Windows.h>
#include <strsafe.h>

#define MOD_PRINTF_EX _T("printf")

#define printfEx(FMT, ...) PrintfInternal(__FUNCTION__, FMT, __VA_ARGS__)

VOID
	PrintfInternal(
	__in PCHAR	pFuncName,
	__in LPSTR	Fmt,
	...
	);
