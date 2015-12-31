#pragma once

#include <Windows.h>
#include <tchar.h>

#ifndef MOD_OPERATION_SYSTEM_VERSION
#define MOD_OPERATION_SYSTEM_VERSION _T("����ϵͳ�汾")
#endif

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
	OS_VERSION_WINDOWS_SERVER_2012,
	OS_VERSION_WINDOWS_8_POINT1,
	OS_VERSION_WINDOWS_10
} OS_VERSION_USER_DEFINED, *POS_VERSION_USER_DEFINED;

typedef enum _OS_PROCESSOR_TYPE_USER_DEFINED
{
	OS_PROCESSOR_TYPE_UNKNOWN,
	OS_PROCESSOR_TYPE_X86,
	OS_PROCESSOR_TYPE_X64
} OS_PROCESSOR_TYPE_USER_DEFINED, *POS_PROCESSOR_TYPE_USER_DEFINED;

typedef BOOL(WINAPI *IS_WINDOWS_SERVER)(void);
typedef BOOL(WINAPI *IS_WINDOWS_10_OR_GREATER)(void);
typedef BOOL(WINAPI *IS_WINDOWS_8_POINT_1_OR_GREATER)(void);
typedef BOOL(WINAPI *IS_WINDOWS_8_OR_GREATER)(void);
typedef BOOL(WINAPI *IS_WINDOWS_7_SP_1_OR_GREATER)(void);
typedef BOOL(WINAPI *IS_WINDOWS_7_OR_GREATER)(void);
typedef BOOL(WINAPI *IS_WINDOWS_VISTA_SP2_OR_GREATER)(void);
typedef BOOL(WINAPI *IS_WINDOWS_VISTA_SP1_OR_GREATER)(void);
typedef BOOL(WINAPI *IS_WINDOWS_VISTA_OR_GREATER)(void);
typedef BOOL(WINAPI *IS_WINDOWS_XP_SP3_OR_GREATER)(void);
typedef BOOL(WINAPI *IS_WINDOWS_XP_SP2_OR_GREATER)(void);
typedef BOOL(WINAPI *IS_WINDOWS_XP_SP1_OR_GREATER)(void);
typedef BOOL(WINAPI *IS_WINDOWS_XP_OR_GREATER)(void);
typedef BOOL(WINAPI *GET_VERSION_EX)(LPOSVERSIONINFO);

class COperationSystemVersion
{
public:
	BOOL
		Init();

	OS_VERSION_USER_DEFINED
		GetOSVersion();

	OS_PROCESSOR_TYPE_USER_DEFINED
		GetOSProcessorType();

private:
	static OS_VERSION_USER_DEFINED			ms_OsVersionUserDefined;
	static OS_PROCESSOR_TYPE_USER_DEFINED	ms_ProcessorTypeUserDefined;

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
	static GET_VERSION_EX					GetVersionEx;

	OS_VERSION_USER_DEFINED
		GetOSVersionByIsOrGreater();

	OS_VERSION_USER_DEFINED
		GetOSVersionByGetVersionEx();

	OS_PROCESSOR_TYPE_USER_DEFINED
		GetOSProcessorTypeInternal();
};
