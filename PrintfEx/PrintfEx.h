#pragma once

#include <Windows.h>
#include <strsafe.h>

#define MOD_PRINTF_EX _T("printf")

#define printfEx(FMT, ...) PrintfInternal(__FUNCTION__, FMT, __VA_ARGS__)

VOID
	PrintfInternal(
	__in								PCHAR	pFuncName,
	__in __drv_formatString(printfEx)	LPTSTR	Fmt,
	...
	)
{
	TCHAR tchInfo[MAX_PATH] = {0};

	va_list Args;

	va_start(Args, Fmt);

	StringCchPrintf(tchInfo, _countof(tchInfo), Fmt, Args);

	printf("[%s] %S \n", pFuncName, tchInfo);

	va_end(Args);
}
