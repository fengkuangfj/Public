#pragma once

#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <locale.h>
#include <time.h>

#include "../StackBacktrace/StackBacktrace.h"

#define MOD_PRINTF_EX _T("PrintfEx")

typedef enum _PRINTF_LEVEL
{
	PRINTF_LEVEL_INFORMATION,
	PRINTF_LEVEL_WARNING,
	PRINTF_LEVEL_ERROR
} PRINTF_LEVEL, *PPRINTF_LEVEL, *LPPRINTF_LEVEL;

#define printfEx(lpMod, PrintfLevel, FMT, ...) CPrintfEx::PrintfInternal(lpMod, PrintfLevel, __FILE__, __FUNCTION__, __LINE__, FMT, __VA_ARGS__)

class CPrintfEx
{
public:
	BOOL
		Init(
		__in_opt LPSTR lpSymDir
		);

	BOOL
		Unload();

	static
		VOID
		PrintfInternal(
		__in LPTSTR			lpMod,
		__in PRINTF_LEVEL	PrintfLevel,
		__in LPSTR			lpFile,
		__in LPSTR			lpFunction,
		__in ULONG			ulLine,
		__in LPSTR			lpFmt,
		...
		);

	static
		BOOL
		ErrorCodeConnote(
		__in	DWORD	dwErrorCode,
		__out	LPTSTR	lpOutBuf,
		__in	ULONG	ulOutBufSizeCh
		);

private:
	static BOOL bUseStackBackTrace;
};
