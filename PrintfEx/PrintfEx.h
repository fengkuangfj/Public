#pragma once

#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <locale.h>
#include <time.h>

#define MOD_PRINTF_EX _T("PrintfEx")

typedef enum _PRINTF_LEVEL
{
	PRINTF_LEVEL_INFORMATION,
	PRINTF_LEVEL_WARNING,
	PRINTF_LEVEL_ERROR
} PRINTF_LEVEL, *PPRINTF_LEVEL, *LPPRINTF_LEVEL;

#define printfEx(lpMod, PrintfLevel, FMT, ...) CPrintfEx::PrintfInternal(lpMod, PrintfLevel, __FUNCTION__, FMT, __VA_ARGS__)

class CPrintfEx
{
public:
	static
		VOID
		PrintfInternal(
		__in LPTSTR			lpMod,
		__in PRINTF_LEVEL	PrintfLevel,
		__in LPSTR			pFuncName,
		__in LPSTR			Fmt,
		...
		);
};
