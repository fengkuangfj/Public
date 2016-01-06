#pragma once

#ifndef MOD_PRINTF_EX
#define MOD_PRINTF_EX _T("PrintfEx")
#endif

#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <locale.h>
#include <time.h>

#include "..\\ProcessPath\\ProcessPath.h"
#include "..\\ProcessType\\ProcessType.h"

typedef
BOOL
(*QUERY_FULL_PROCESS_IMAGE_NAME)(
__in									HANDLE	hProcess,
__in									DWORD	dwFlags,
__out_ecount_part(*lpdwSize, *lpdwSize) LPWSTR	lpExeName,
__inout									PDWORD	lpdwSize
);

typedef enum _PRINTF_LEVEL
{
	PRINTF_LEVEL_INFORMATION,
	PRINTF_LEVEL_WARNING,
	PRINTF_LEVEL_ERROR
} PRINTF_LEVEL, *PPRINTF_LEVEL, *LPPRINTF_LEVEL;

#define printfEx(lpMod, PrintfLevel, FMT, ...) CPrintfEx::GetInstance()->PrintfInternal(lpMod, PrintfLevel, __FILE__, __FUNCSIG__, __LINE__, FMT, __VA_ARGS__)

class CPrintfEx
{
public:
	static
		CPrintfEx *
		GetInstance();

	static
		VOID
		ReleaseInstance();

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
	static CPrintfEx *	ms_pInstance;

	BOOL				m_bOutputDebugString;
	PROC_TYPE			m_ProcType;

	CPrintfEx();

	~CPrintfEx();

	BOOL
		Init();

	BOOL
		Unload();
};
