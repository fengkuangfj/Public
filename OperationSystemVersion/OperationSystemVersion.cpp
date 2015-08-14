#include "stdafx.h"


OS_VER			COsVersion::ms_OsVer			= OS_VER_UNKNOWN;
OS_PROC_TYPE	COsVersion::ms_OsProcType		= OS_PROC_TYPE_UNKNOWN;
BOOL			COsVersion::ms_bInitOsVer		= FALSE;
BOOL			COsVersion::ms_bInitOsProcType	= FALSE;
OSVERSIONINFO	COsVersion::ms_OsVerInfo		= {0};


BOOL
	COsVersion::Init()
{
	BOOL bRet = FALSE;


	__try
	{
		if (!ms_bInitOsVer)
		{
			if (OS_VER_UNKNOWN == GetOSVer())
				__leave;
		}

		if (!ms_bInitOsProcType)
		{
			if (OS_PROC_TYPE_UNKNOWN == GetOSProcType())
				__leave;
		}

		bRet = TRUE;
	}
	__finally
	{
		;
	}

	return bRet;
}

OS_VER
	COsVersion::GetOSVer(
	__out_opt OSVERSIONINFO* pOsVersionInfo
	)
{
	OS_VER ret = OS_VER_UNKNOWN;


	__try
	{
		if (!ms_bInitOsVer)
		{
			ms_OsVerInfo.dwOSVersionInfoSize = sizeof(ms_OsVerInfo);

			if (!GetVersionEx(&ms_OsVerInfo))
				__leave;

			if (VER_PLATFORM_WIN32_NT != ms_OsVerInfo.dwPlatformId)
				__leave;

			if (5 == ms_OsVerInfo.dwMajorVersion && 1 == ms_OsVerInfo.dwMinorVersion)
				ms_OsVer = OS_VER_WINDOWS_XP;
			else if (6 == ms_OsVerInfo.dwMajorVersion && 1 == ms_OsVerInfo.dwMinorVersion)
				ms_OsVer = OS_VER_WINDOWS_7;
			else if (6 == ms_OsVerInfo.dwMajorVersion && 2 == ms_OsVerInfo.dwMinorVersion)
				ms_OsVer = OS_VER_WINDOWS_8;
			else
				__leave;

			ms_bInitOsVer = TRUE;
		}

		ret = ms_OsVer;
	}
	__finally
	{
		if (pOsVersionInfo)
			CopyMemory(pOsVersionInfo, &ms_OsVerInfo, sizeof(ms_OsVerInfo));
	}

	return ret;
}

OS_PROC_TYPE
	COsVersion::GetOSProcType()
{
	OS_PROC_TYPE	ret			= OS_PROC_TYPE_UNKNOWN;

	SYSTEM_INFO		systemInfo	= {0};


	__try
	{
		if (!ms_bInitOsProcType)
		{
			GetNativeSystemInfo(&systemInfo);

			if (PROCESSOR_ARCHITECTURE_INTEL == systemInfo.wProcessorArchitecture)
				ms_OsProcType = OS_PROC_TYPE_X86;
			else if (PROCESSOR_ARCHITECTURE_AMD64 == systemInfo.wProcessorArchitecture)
				ms_OsProcType = OS_PROC_TYPE_X64;
			else
				__leave;

			ms_bInitOsProcType = TRUE;
		}

		ret = ms_OsProcType;
	}
	__finally
	{
		;
	}

	return ret;
}

OS_VER_AND_PROC_TYPE
	COsVersion::GetOsVerAndProcType()
{
	OS_VER_AND_PROC_TYPE	OsVerAndProcType	= OS_VER_AND_PROC_TYPE_UNKNOWN;

	OS_VER					OsVer				= OS_VER_UNKNOWN;
	OS_PROC_TYPE			OsProcType			= OS_PROC_TYPE_UNKNOWN;


	__try
	{
		OsVer = GetOSVer();
		OsProcType = GetOSProcType();

		if (OS_VER_WINDOWS_XP == OsVer && OS_PROC_TYPE_X86 == OsProcType)
			OsVerAndProcType = OS_VER_AND_PROC_TYPE_WINDOWS_XP_X86;
		else if (OS_VER_WINDOWS_7 == OsVer && OS_PROC_TYPE_X86 == OsProcType)
			OsVerAndProcType = OS_VER_AND_PROC_TYPE_WINDOWS_7_X86;
		else if (OS_VER_WINDOWS_7 == OsVer && OS_PROC_TYPE_X64 == OsProcType)
			OsVerAndProcType = OS_VER_AND_PROC_TYPE_WINDOWS_7_X64;
	}
	__finally
	{
		;
	}

	return OsVerAndProcType;
}
