#pragma once

#ifndef MOD_PROCESS_PATH
#define MOD_PROCESS_PATH _T("½ø³ÌÂ·¾¶")
#endif

#include <Windows.h>
#include <tchar.h>
#include <Psapi.h>

#include "..\\ModulePath\\ModulePath.h"

#pragma comment(lib, "Psapi.lib")

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
		CProcessPath *
		GetInstance();

	static
		VOID
		ReleaseInstance();

	BOOL
		Get(
		__in	BOOL	bCurrentProc,
		__in	ULONG	ulPid,
		__out	LPTSTR	lpOutBuf,
		__in	ULONG	ulOutBufSizeCh
		);

private:
	static CProcessPath				*	ms_pInstance;

	HMODULE								m_hModule;
	QUERY_FULL_PROCESS_IMAGE_NAME		ms_QueryFullProcessImageName;

	CProcessPath();

	~CProcessPath();

	BOOL
		Init();

	BOOL
		Unload();
};
