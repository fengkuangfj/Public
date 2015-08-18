#pragma once

#include <Windows.h>
#include <time.h>
#include <tchar.h>
#include <strsafe.h>

#include <DbgHelp.h>
#include <Shlwapi.h>
#include <Winver.h>
#include <Psapi.h>

#pragma comment(lib, "DbgHelp.lib")
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Version.lib")
#pragma comment(lib, "Psapi.lib")

#define MOD_SIMPLE_DUMP							_T("简单转储")

#define	CMD_LINE_MAX_CHARS 						32768

#define WER_P0									0
#define WER_P1									1
#define WER_P2									2
#define WER_P3									3
#define WER_P4									4
#define WER_P5									5
#define WER_P6									6
#define WER_P7									7
#define WER_P8									8
#define WER_P9									9

#define WER_DUMP_MASK_START						1
#define WER_DUMP_MASK_DUMPTYPE					(WER_DUMP_MASK_START << 0)
#define WER_DUMP_MASK_ONLY_THISTHREAD			(WER_DUMP_MASK_START << 1)
#define WER_DUMP_MASK_THREADFLAGS				(WER_DUMP_MASK_START << 2)
#define WER_DUMP_MASK_THREADFLAGS_EX			(WER_DUMP_MASK_START << 3)
#define WER_DUMP_MASK_OTHERTHREADFLAGS			(WER_DUMP_MASK_START << 4)
#define WER_DUMP_MASK_OTHERTHREADFLAGS_EX		(WER_DUMP_MASK_START << 5)
#define WER_DUMP_MASK_PREFERRED_MODULESFLAGS	(WER_DUMP_MASK_START << 6)
#define WER_DUMP_MASK_OTHER_MODULESFLAGS		(WER_DUMP_MASK_START << 7)
#define WER_DUMP_MASK_PREFERRED_MODULE_LIST		(WER_DUMP_MASK_START << 8)

#define WER_SUBMIT_HONOR_RECOVERY               1   
#define WER_SUBMIT_HONOR_RESTART                2   
#define WER_SUBMIT_QUEUE                        4   
#define WER_SUBMIT_SHOW_DEBUG                   8   
#define WER_SUBMIT_ADD_REGISTERED_DATA          16  
#define WER_SUBMIT_OUTOFPROCESS                 32  
#define WER_SUBMIT_NO_CLOSE_UI                  64  
#define WER_SUBMIT_NO_QUEUE                     128 
#define WER_SUBMIT_NO_ARCHIVE                   256 
#define WER_SUBMIT_START_MINIMIZED              512 
#define WER_SUBMIT_OUTOFPROCESS_ASYNC           1024
#define WER_SUBMIT_BYPASS_DATA_THROTTLING       2048
#define WER_SUBMIT_ARCHIVE_PARAMETERS_ONLY      4096
#define WER_SUBMIT_REPORT_MACHINE_ID            8192

#define WER_MAX_PREFERRED_MODULES_BUFFER		256

typedef enum _OS_VERSION_USER_DEFINED
{
	OS_VERSION_UNKNOWN,
	OS_VERSION_WINDOWS_2000,
	OS_VERSION_WINDOWS_SERVER,
	OS_VERSION_WINDOWS_HOME_SERVER,
	OS_VERSION_WINDOWS_SERVER_2003,
	OS_VERSION_WINDOWS_SERVER_2008,
	OS_VERSION_WINDOWS_SERVER_2008_R2,
	OS_VERSION_WINDOWS_XP,
	OS_VERSION_WINDOWS_XP_SP1,
	OS_VERSION_WINDOWS_XP_SP2,
	OS_VERSION_WINDOWS_XP_SP3,
	OS_VERSION_WINDOWS_VISTA,
	OS_VERSION_WINDOWS_VISTA_PS1,
	OS_VERSION_WINDOWS_VISTA_PS2,
	OS_VERSION_WINDOWS_7,
	OS_VERSION_WINDOWS_7_SP1,
	OS_VERSION_WINDOWS_8,
	OS_VERSION_WINDOWS_8_POINT1,
	OS_VERSION_WINDOWS_10	
} OS_VERSION_USER_DEFINED, *POS_VERSION_USER_DEFINED;

typedef enum _OS_PROCESSOR_TYPE_USER_DEFINED
{
	OS_PROCESSOR_TYPE_UNKNOWN,
	OS_PROCESSOR_TYPE_X86,
	OS_PROCESSOR_TYPE_X64
} OS_PROCESSOR_TYPE_USER_DEFINED, *POS_PROCESSOR_TYPE_USER_DEFINED;

typedef enum _WER_REPORT_TYPE
{   
	WerReportNonCritical		= 0,
	WerReportCritical			= 1,
	WerReportApplicationCrash	= 2,
	WerReportApplicationHang	= 3,
	WerReportKernel				= 4,
	WerReportInvalid
} WER_REPORT_TYPE, *PWER_REPORT_TYPE;

typedef enum _WER_DUMP_TYPE
{
	WerDumpTypeMicroDump	= 1,
	WerDumpTypeMiniDump		= 2,
	WerDumpTypeHeapDump		= 3,
	WerDumpTypeMax			= 4
} WER_DUMP_TYPE, *PWER_DUMP_TYPE;

typedef enum _WER_CONSENT
{
	WerConsentNotAsked		= 1,
	WerConsentApproved		= 2,
	WerConsentDenied		= 3,
	WerConsentAlwaysPrompt	= 4,
	WerConsentMax
} WER_CONSENT, *PWER_CONSENT;

typedef enum _WER_SUBMIT_RESULT
{
	WerReportQueued		= 1,
	WerReportUploaded	= 2,
	WerReportDebug		= 3,
	WerReportFailed		= 4,
	WerDisabled			= 5,
	WerReportCancelled	= 6,
	WerDisabledQueue	= 7,
	WerReportAsync		= 8,
	WerCustomAction		= 9
} WER_SUBMIT_RESULT, *PWER_SUBMIT_RESULT;

typedef enum _APPLICATION_TYPE
{
	APPLICATION_TYPE_UNKNOWN,
	APPLICATION_TYPE_CONSOLE,
	APPLICATION_TYPE_NOT_CONSOLE
} APPLICATION_TYPE, *PAPPLICATION_TYPE;

typedef enum _PROGRAM_TYPE
{
	PROGRAM_TYPE_TYPE_UNKNOWN,
	PROGRAM_TYPE_TYPE_CONSOLE,
	PROGRAM_TYPE_TYPE_DLL,
	PROGRAM_TYPE_TYPE_NOT_CONSOLE_OR_DLL
} PROGRAM_TYPE, *PPROGRAM_TYPE;

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

typedef VOID (*RESTART) (VOID);

typedef struct _CRUSH_HANDLER_INFO
{
	EH_TYPE					EhType;
	BOOL					bFirstHandler;

	MINIDUMP_TYPE			MiniDumpType;
	HMODULE					hDllModule;

	BOOL					bRestart;
	RESTART					Restart;
	PROGRAM_TYPE			ProgramType;
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
	APPLICATION_TYPE	ApplicationType;

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

typedef HANDLE HREPORT;

typedef struct _WER_REPORT_INFORMATION
{
	DWORD	dwSize;
	HANDLE	hProcess;
	WCHAR	wzConsentKey[64];
	WCHAR	wzFriendlyEventName[128];
	WCHAR	wzApplicationName[128];
	WCHAR	wzApplicationPath[MAX_PATH];
	WCHAR	wzDescription[512];
	HWND	hwndParent;    
} WER_REPORT_INFORMATION, *PWER_REPORT_INFORMATION;

typedef struct _WER_EXCEPTION_INFORMATION
{   
	PEXCEPTION_POINTERS pExceptionPointers;
	BOOL				bClientPointers;
} WER_EXCEPTION_INFORMATION, *PWER_EXCEPTION_INFORMATION;

typedef struct _WER_DUMP_CUSTOM_OPTIONS 
{
	DWORD dwSize;   
	DWORD dwMask;
	DWORD dwDumpFlags;
	BOOL  bOnlyThisThread;
	DWORD dwExceptionThreadFlags;
	DWORD dwOtherThreadFlags;
	DWORD dwExceptionThreadExFlags;
	DWORD dwOtherThreadExFlags;
	DWORD dwPreferredModuleFlags;
	DWORD dwOtherModuleFlags;
	WCHAR wzPreferredModuleList[WER_MAX_PREFERRED_MODULES_BUFFER];
} WER_DUMP_CUSTOM_OPTIONS, *PWER_DUMP_CUSTOM_OPTIONS;

typedef BOOL (*IS_WINDOWS_SERVER)(void);
typedef BOOL (*IS_WINDOWS_10_OR_GREATER)(void);
typedef BOOL (*IS_WINDOWS_8_POINT_1_OR_GREATER)(void);
typedef BOOL (*IS_WINDOWS_8_OR_GREATER)(void);
typedef BOOL (*IS_WINDOWS_7_SP_1_OR_GREATER)(void);
typedef BOOL (*IS_WINDOWS_7_OR_GREATER)(void);
typedef BOOL (*IS_WINDOWS_VISTA_SP2_OR_GREATER)(void);
typedef BOOL (*IS_WINDOWS_VISTA_SP1_OR_GREATER)(void);
typedef BOOL (*IS_WINDOWS_VISTA_OR_GREATER)(void);
typedef BOOL (*IS_WINDOWS_XP_SP3_OR_GREATER)(void);
typedef BOOL (*IS_WINDOWS_XP_SP2_OR_GREATER)(void);
typedef BOOL (*IS_WINDOWS_XP_SP1_OR_GREATER)(void);
typedef BOOL (*IS_WINDOWS_XP_OR_GREATER)(void);

typedef
	HRESULT
	(WINAPI * REGISTER_APPLICATION_RESTART)(
	__in_opt	PCWSTR	pwzCommandline,
	__in		DWORD	dwFlags
	);

typedef HRESULT (WINAPI * UNREGISTER_APPLICATION_RESTART)(void);

typedef
	HRESULT
	(WINAPI * WER_REPORT_CREATE)(
	__in		PCWSTR					pwzEventType, 
	__in		WER_REPORT_TYPE			repType,
	__in_opt	PWER_REPORT_INFORMATION pReportInformation,
	__out		HREPORT*				phReportHandle
	);

typedef
	HRESULT
	(WINAPI * WER_REPORT_SET_PARAMETER)(
	__in		HREPORT hReportHandle, 
	__in		DWORD	dwparamID, 
	__in_opt	PCWSTR	pwzName,
	__in		PCWSTR	pwzValue
	);

typedef
	HRESULT  
	(WINAPI * WER_REPORT_ADD_DUMP)(
	__in		HREPORT						hReportHandle, 
	__in		HANDLE						hProcess,
	__in_opt	HANDLE						hThread,
	__in		WER_DUMP_TYPE				dumpType,
	__in_opt	PWER_EXCEPTION_INFORMATION	pExceptionParam,
	__in_opt	PWER_DUMP_CUSTOM_OPTIONS	pDumpCustomOptions,
	__in		DWORD						dwFlags
	);

typedef
	HRESULT 
	(WINAPI * WER_REPORT_SUBMIT)(
	__in		HREPORT				hReportHandle,
	__in		WER_CONSENT			consent,
	__in		DWORD				dwFlags,
	__out_opt	PWER_SUBMIT_RESULT	pSubmitResult
	);

typedef
	HRESULT 
	(WINAPI * WER_REPORT_CLOSE_HANDLE)(
	__in HREPORT hReportHandle
	);

class CSimpleDump
{
public:
	static
		VOID
		RegisterCrushHandler(
		__in PCRUSH_HANDLER_INFO pCrushHandlerInfo = NULL
		);

	static
		APPLICATION_TYPE
		GetApplicationType();

	static
		BOOL
		InitArgCmdlineInfo(
		__in		APPLICATION_TYPE	ApplicationType,
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
	static MINIDUMP_TYPE					ms_MinidumpType;
	static BOOL								ms_bRestart;
	static HMODULE							ms_hModule;
	static APPLICATION_TYPE					ms_ApplicationType;
	static PROGRAM_TYPE						ms_ProgramType;
	static LPTSTR							ms_lpCmdLine;
	static TCHAR							ms_tchRestartTag[MAX_PATH];

	static RESTART							ms_Restart;

	static HMODULE							ms_hModuleKernel32Dll;
	static BOOL								ms_bCanUseGetOSVersionByIsOrGreater;
	static IS_WINDOWS_SERVER				IsWindowsServer;
	static IS_WINDOWS_10_OR_GREATER			IsWindows1OrGreater;
	static IS_WINDOWS_8_POINT_1_OR_GREATER	IsWindows8Point1OrGreater;
	static IS_WINDOWS_8_OR_GREATER			IsWindows8OrGreater;
	static IS_WINDOWS_7_SP_1_OR_GREATER		IsWindows7SP1OrGreater;
	static IS_WINDOWS_7_OR_GREATER			IsWindows7OrGreater;
	static IS_WINDOWS_VISTA_SP2_OR_GREATER	IsWindowsVistaSP2OrGreater;
	static IS_WINDOWS_VISTA_SP1_OR_GREATER	IsWindowsVistaSP1OrGreater;
	static IS_WINDOWS_VISTA_OR_GREATER		IsWindowsVistaOrGreater;
	static IS_WINDOWS_XP_SP3_OR_GREATER		IsWindowsXPSP3OrGreater;
	static IS_WINDOWS_XP_SP2_OR_GREATER		IsWindowsXPSP2OrGreater;
	static IS_WINDOWS_XP_SP1_OR_GREATER		IsWindowsXPSP1OrGreater;
	static IS_WINDOWS_XP_OR_GREATER			IsWindowsXPOrGreater;

	static BOOL								ms_bCanUseRegisterRestart;
	static REGISTER_APPLICATION_RESTART		RegisterApplicationRestart;

	static HMODULE							ms_hModuleWerDll;
	static BOOL								ms_bCanUseGenWerReport;
	static WER_REPORT_CREATE				WerReportCreate;
	static WER_REPORT_SET_PARAMETER			WerReportSetParameter;
	static WER_REPORT_ADD_DUMP				WerReportAddDump;
	static WER_REPORT_SUBMIT				WerReportSubmit;
	static WER_REPORT_CLOSE_HANDLE			WerReportCloseHandle;

	static
		BOOL
		GetFunc();

	static
		BOOL
		GetKernel32DllFunc();

	static
		BOOL
		GetWerDllFunc();

	static
		VOID
		DefaultRestartFunc();

	static
		LONG
		WINAPI
		ExceptionHandler(
		_In_ struct _EXCEPTION_POINTERS* pExceptionInfo
		);

	static
		BOOL
		CreateDumpFile(
		__inout LPTSTR	lpDumpFilePath,
		__in	ULONG	ulBufferLen
		);

	static
		BOOL
		GenWerReport(
		__in _EXCEPTION_POINTERS* pExceptionInfo
		);

	static
		BOOL
		GenDump(
		__in _EXCEPTION_POINTERS* pExceptionInfo
		);

	static
		OS_VERSION_USER_DEFINED
		GetOSVersion();

	static
		OS_VERSION_USER_DEFINED
		GetOSVersionByIsOrGreater();

	static
		OS_VERSION_USER_DEFINED
		GetOSVersionByGetVersionEx();

	OS_PROCESSOR_TYPE_USER_DEFINED
		GetOSProcessorType();

	static
		BOOL
		GetApplicationVersion(
		__in	LPTSTR	lpApplicationPath,
		__inout LPTSTR	lpApplicationVersion,
		__in	ULONG	ulApplicationVersionLen
		);

	static
		BOOL
		GetModuleName(
		__in	PVOID	pAddress,
		__inout LPTSTR	lpMoudleName,
		__in	ULONG	ulMoudleNameLen
		);

	static
		BOOL
		GetModuleNameIndex(
		__in	PVOID		pAddress,
		__in	HMODULE*	phMoudle,
		__in	ULONG		ulCount,
		__inout ULONG*		pIndex
		);

	static
		BOOL
		SortModule(
		__inout	HMODULE*	phMoudle,
		__in	ULONG		ulCount
		);

	static
		BOOL
		RegisterRestart();

	static
		BOOL
		BeenRunningMinimum60Seconds();

	static
		BOOL
		GenRestartCmdLine(
		__inout LPTSTR	pCmdLine,
		__in	ULONG	ulCharacters
		);

	static
		BOOL
		InitCmdLine(
		__in_opt int	nArgc,
		__in_opt TCHAR*	pArgv[],
		__in_opt LPTSTR	lpCmdLine
		);
};
