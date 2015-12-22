#pragma once

#include <Windows.h>
#include <tchar.h>
#include <Psapi.h>

#include "..\\ModulePath\\ModulePath.h"

#pragma comment(lib, "Psapi.lib")

#define MOD_PROCESS_PATH _T("����·��")

typedef
	BOOL
	(* QUERY_FULL_PROCESS_IMAGE_NAME)(
	__in									HANDLE	hProcess,
	__in									DWORD	dwFlags,
	__out_ecount_part(*lpdwSize, *lpdwSize) LPWSTR	lpExeName,
	__inout									PDWORD	lpdwSize
	);

class CProcessPath
{
public:
	static
		BOOL
		Get(
		__in	BOOL	bCurrentProc,
		__in	ULONG	ulPid,
		__out	LPTSTR	lpOutBuf,
		__in	ULONG	ulOutBufSizeCh
		);
};
