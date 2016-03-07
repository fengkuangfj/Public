#pragma once

#include <Windows.h>
#include <tchar.h>
#include <Psapi.h>
#include <Shlobj.h>

#include "..\\Public.h"
#include "..\\ModulePath\\ModulePath.h"

#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "Advapi32.lib")

#define MOD_PROCESS_CONTROL _T("½ø³Ì¿ØÖÆ")

typedef enum _PROC_TYPE
{
	PROC_TYPE_UNKNOWN,
	PROC_TYPE_NORMAL,
	PROC_TYPE_CONSOLE,
	PROC_TYPE_SERVICE
} PROC_TYPE, *PPROC_TYPE, *LPPROC_TYPE;

typedef
	BOOL
	(* QUERY_FULL_PROCESS_IMAGE_NAME)(
	__in									HANDLE	hProcess,
	__in									DWORD	dwFlags,
	__out_ecount_part(*lpdwSize, *lpdwSize) LPWSTR	lpExeName,
	__inout									PDWORD	lpdwSize
	);

class CProcessControl
{
public:
	static
		CProcessControl *
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

	static
	PROC_TYPE
		GetProcType(
		__in BOOL	bCurrentProc,
		__in ULONG	ulPid
		);

	static
	BOOL
		RunAs(
		__in LPTSTR lpPath
		);

	static
	BOOL
		Raise(
		__in BOOL	bCurrentProcess,
		__in ULONG	ulPid
		);

	static
	VOID
		DeleteMyselfBySHChangeNotify();

	static
	VOID
		DeleteMyselfByCreateProcess();

// 	BOOL
// 		GetParentPid(
// 		__in	ULONG	ulPid,
// 		__out	PULONG	pulParentPid 
// 		);

private:
	static CProcessControl				*	ms_pInstance;

	HMODULE								m_hModule;
	QUERY_FULL_PROCESS_IMAGE_NAME		m_QueryFullProcessImageName;

	CProcessControl();

	~CProcessControl();

	BOOL
		Init();

	BOOL
		Unload();

	static
	BOOL
		GetSessionId(
		__in	BOOL		bCurrentProc,
		__in	ULONG		ulPid,
		__out	int		*	pnSid
		);

	static
	BOOL
		Adjust(
		__in BOOL	bCurrentProcess,
		__in ULONG	ulPid,
		__in LPTSTR lpName
		);
};
