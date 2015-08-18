#pragma once

#include <Windows.h>
#include <strsafe.h>
#include <locale.h>

#define MOD_PRINTF_EX _T("PrintfEx")

class CPrintfEx
{
public:
	static
		VOID
		PrintfInternal(
		__in PCHAR	pFuncName,
		__in LPSTR	Fmt,
		...
		);
};

#define printfEx(FMT, ...) CPrintfEx::PrintfInternal(__FUNCTION__, FMT, __VA_ARGS__)
