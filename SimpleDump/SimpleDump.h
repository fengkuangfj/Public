#pragma once

#include <Windows.h>
#include <time.h>
#include <tchar.h>
#include <strsafe.h>
#include <DbgHelp.h>
#include <Shlwapi.h>
#include <Winver.h>
#include <Psapi.h>

#include "..\\PrintfEx\\PrintfEx.h"
#include "..\\OperationSystemVersion\\OperationSystemVersion.h"
#include "..\\ProcessControl\\ProcessControl.h"

#pragma comment(lib, "DbgHelp.lib")
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Version.lib")
#pragma comment(lib, "Psapi.lib")

#ifndef MOD_SIMPLE_DUMP
#define MOD_SIMPLE_DUMP							_T("简单转储")
#endif

#ifndef CMD_LINE_MAX_CHARS
#define	CMD_LINE_MAX_CHARS 						32768
#endif

typedef enum _EH_TYPE
{
	EH_TYPE_UNKNOWN,
	EH_TYPE_S,
	EH_TYPE_V
} EH_TYPE, *PEH_TYPE;

typedef enum _PARAMETER_TYPE
{
	PARAMETER_TYPE_UNKNOWN,
	PARAMETER_TYPE_ARG,
	PARAMETER_TYPE_CMDLINE
} PARAMETER_TYPE, *PPARAMETER_TYPE;

typedef VOID(*RESTART) (VOID);

typedef struct _CRUSH_HANDLER_INFO
{
	EH_TYPE					EhType;
	BOOL					bFirstHandler;

	MINIDUMP_TYPE			MiniDumpType;

	BOOL					bRestart;
	RESTART					Restart;
	PARAMETER_TYPE			ParameterType;
	TCHAR					tchRestartTag[MAX_PATH];
	union
	{
		struct
		{
			int				nArgc;
			LPTSTR*			plpArgv;
		} Arg;

		LPTSTR				lpCmdLine;
	};
} CRUSH_HANDLER_INFO, *PCRUSH_HANDLER_INFO;

typedef struct _ARG_CMDLINE_INFO
{
	PROC_TYPE			ProcType;

	union
	{
		struct
		{
			int			nArgc;
			LPTSTR		lpArgv[1];
		} Console;

		struct
		{
			TCHAR		tchCmdLine[1];
		} NotConsole;
	};
} ARG_CMDLINE_INFO, *PARG_CMDLINE_INFO;

typedef
HRESULT
(WINAPI * REGISTER_APPLICATION_RESTART)(
__in_opt	PCWSTR	pwzCommandline,
__in		DWORD	dwFlags
);

typedef HRESULT(WINAPI * UNREGISTER_APPLICATION_RESTART)(void);

class CSimpleDump
{
public:
	static
		CSimpleDump *
		GetInstance();

	static
		VOID
		ReleaseInstance();

	VOID
		RegisterCrushHandler(
		__in PCRUSH_HANDLER_INFO pCrushHandlerInfo = NULL
		);

	static
		BOOL
		InitArgCmdlineInfo(
		__in		PROC_TYPE			ProcType,
		__in_opt	int					nArgc,
		__in_opt	LPTSTR				lpArgv[],
		__in_opt	LPTSTR				lpCmdLine,
		__inout_opt	PARG_CMDLINE_INFO	pArgCmdLineInfo,
		__inout		ULONG*				pulBufLen
		);

	static
		BOOL
		ParseArgOrCmdLine(
		__in		PARG_CMDLINE_INFO	pArgCmdlineInfo,
		__inout_opt LPTSTR				lpResult,
		__inout		ULONG*				pulBufLen
		);

private:
	static CSimpleDump				*	ms_pInstance;

	MINIDUMP_TYPE						m_MinidumpType;
	BOOL								m_bRestart;
	PROC_TYPE							m_ProcType;
	LPTSTR								m_lpCmdLine;
	TCHAR								m_tchRestartTag[MAX_PATH];

	RESTART								m_pfRestart;

	HMODULE								m_hModuleKernel32Dll;
	BOOL								m_bCanUseRegisterRestart;
	REGISTER_APPLICATION_RESTART		m_pfRegisterApplicationRestart;

	PTOP_LEVEL_EXCEPTION_FILTER			m_pfTopLevelExceptionFilter;

	CSimpleDump();

	~CSimpleDump();

	BOOL
		GetFunc();

	BOOL
		GetKernel32DllFunc();

	VOID
		DefaultRestartFunc();

	static
	LONG
		WINAPI
		ExceptionHandler(
		_In_ struct _EXCEPTION_POINTERS* pExceptionInfo
		);

	BOOL
		CreateDumpFile(
		__in	PVOID	pExceptionAddress,
		__inout LPTSTR	lpDumpFilePath,
		__in	ULONG	ulBufferLen
		);

	BOOL
		GenDump(
		__in _EXCEPTION_POINTERS* pExceptionInfo
		);

	BOOL
		GetApplicationVersion(
		__in	LPTSTR	lpApplicationPath,
		__inout LPTSTR	lpApplicationVersion,
		__in	ULONG	ulApplicationVersionLen
		);

	BOOL
		GetModuleName(
		__in	PVOID	pAddress,
		__inout LPTSTR	lpMoudleName,
		__in	ULONG	ulMoudleNameLen
		);

	BOOL
		GetModuleNameIndex(
		__in	PVOID		pAddress,
		__in	HMODULE*	phMoudle,
		__in	ULONG		ulCount,
		__inout ULONG*		pIndex
		);

	BOOL
		SortModule(
		__inout	HMODULE*	phMoudle,
		__in	ULONG		ulCount
		);

	BOOL
		RegisterRestart();

	BOOL
		BeenRunningMinimum60Seconds();

	BOOL
		GenRestartCmdLine(
		__inout LPTSTR	pCmdLine,
		__in	ULONG	ulCharacters
		);

	BOOL
		InitCmdLine(
		__in_opt int	nArgc,
		__in_opt TCHAR*	pArgv[],
		__in_opt LPTSTR	lpCmdLine
		);

	void
		WaitForOldProcExit();
};
