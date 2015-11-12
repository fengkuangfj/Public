#include "OperationSystemVersion.h"

OS_VERSION_USER_DEFINED			COperationSystemVersion::ms_OsVersionUserDefined = OS_VERSION_UNKNOWN;
OS_PROCESSOR_TYPE_USER_DEFINED	COperationSystemVersion::ms_ProcessorTypeUserDefined = OS_PROCESSOR_TYPE_UNKNOWN;

IS_WINDOWS_SERVER				COperationSystemVersion::IsWindowsServer = NULL;
IS_WINDOWS_10_OR_GREATER		COperationSystemVersion::IsWindows1OrGreater = NULL;
IS_WINDOWS_8_POINT_1_OR_GREATER	COperationSystemVersion::IsWindows8Point1OrGreater = NULL;
IS_WINDOWS_8_OR_GREATER			COperationSystemVersion::IsWindows8OrGreater = NULL;
IS_WINDOWS_7_SP_1_OR_GREATER	COperationSystemVersion::IsWindows7SP1OrGreater = NULL;
IS_WINDOWS_7_OR_GREATER			COperationSystemVersion::IsWindows7OrGreater = NULL;
IS_WINDOWS_VISTA_SP2_OR_GREATER	COperationSystemVersion::IsWindowsVistaSP2OrGreater = NULL;
IS_WINDOWS_VISTA_SP1_OR_GREATER	COperationSystemVersion::IsWindowsVistaSP1OrGreater = NULL;
IS_WINDOWS_VISTA_OR_GREATER		COperationSystemVersion::IsWindowsVistaOrGreater = NULL;
IS_WINDOWS_XP_SP3_OR_GREATER	COperationSystemVersion::IsWindowsXPSP3OrGreater = NULL;
IS_WINDOWS_XP_SP2_OR_GREATER	COperationSystemVersion::IsWindowsXPSP2OrGreater = NULL;
IS_WINDOWS_XP_SP1_OR_GREATER	COperationSystemVersion::IsWindowsXPSP1OrGreater = NULL;
IS_WINDOWS_XP_OR_GREATER		COperationSystemVersion::IsWindowsXPOrGreater = NULL;
GET_VERSION_EX					COperationSystemVersion::GetVersionEx = NULL;

BOOL
COperationSystemVersion::Init()
{
	BOOL	bRet = FALSE;

	HMODULE hModule = NULL;


	__try
	{
		hModule = LoadLibrary(_T("Kernel32.dll"));
		if (!hModule)
			__leave;

		IsWindowsServer = (IS_WINDOWS_SERVER)GetProcAddress(hModule, "IsWindowsServer");
		IsWindows1OrGreater = (IS_WINDOWS_10_OR_GREATER)GetProcAddress(hModule, "IsWindows1OrGreater");
		IsWindows8Point1OrGreater = (IS_WINDOWS_8_POINT_1_OR_GREATER)GetProcAddress(hModule, "IsWindows8Point1OrGreater");
		IsWindows8OrGreater = (IS_WINDOWS_8_OR_GREATER)GetProcAddress(hModule, "IsWindows8OrGreater");
		IsWindows7SP1OrGreater = (IS_WINDOWS_7_SP_1_OR_GREATER)GetProcAddress(hModule, "IsWindows7SP1OrGreater");
		IsWindows7OrGreater = (IS_WINDOWS_7_OR_GREATER)GetProcAddress(hModule, "IsWindows7OrGreater");
		IsWindowsVistaSP2OrGreater = (IS_WINDOWS_VISTA_SP2_OR_GREATER)GetProcAddress(hModule, "IsWindowsVistaSP2OrGreater");
		IsWindowsVistaSP1OrGreater = (IS_WINDOWS_VISTA_SP1_OR_GREATER)GetProcAddress(hModule, "IsWindowsVistaSP1OrGreater");
		IsWindowsVistaOrGreater = (IS_WINDOWS_VISTA_OR_GREATER)GetProcAddress(hModule, "IsWindowsVistaOrGreater");
		IsWindowsXPSP3OrGreater = (IS_WINDOWS_XP_SP3_OR_GREATER)GetProcAddress(hModule, "IsWindowsXPSP3OrGreater");
		IsWindowsXPSP2OrGreater = (IS_WINDOWS_XP_SP2_OR_GREATER)GetProcAddress(hModule, "IsWindowsXPSP2OrGreater");
		IsWindowsXPSP1OrGreater = (IS_WINDOWS_XP_SP1_OR_GREATER)GetProcAddress(hModule, "IsWindowsXPSP1OrGreater");
		IsWindowsXPOrGreater = (IS_WINDOWS_XP_OR_GREATER)GetProcAddress(hModule, "IsWindowsXPOrGreater");
		if (IsWindowsXPOrGreater)
			ms_OsVersionUserDefined = GetOSVersionByIsOrGreater();
		else
		{
			GetVersionEx = (GET_VERSION_EX)GetProcAddress(hModule, "GetVersionEx");
			if (!GetVersionEx)
				__leave;

			ms_OsVersionUserDefined = GetOSVersionByGetVersionEx();
		}

		ms_ProcessorTypeUserDefined = GetOSProcessorTypeInternal();

		bRet = TRUE;
	}
	__finally
	{
		if (hModule)
		{
			FreeLibrary(hModule);
			hModule = NULL;
		}
	}

	return bRet;
}

OS_VERSION_USER_DEFINED
COperationSystemVersion::GetOSVersion()
{
	return ms_OsVersionUserDefined;
}

OS_VERSION_USER_DEFINED
COperationSystemVersion::GetOSVersionByIsOrGreater()

{
	OS_VERSION_USER_DEFINED	ret = OS_VERSION_UNKNOWN;


	__try
	{
		if (IsWindowsServer())
		{
			ret = OS_VERSION_WINDOWS_SERVER;
			__leave;
		}

		if (IsWindows1OrGreater())
		{
			ret = OS_VERSION_WINDOWS_10;
			__leave;
		}

		if (IsWindows8Point1OrGreater())
		{
			ret = OS_VERSION_WINDOWS_8_POINT1;
			__leave;
		}

		if (IsWindows8OrGreater())
		{
			ret = OS_VERSION_WINDOWS_8;
			__leave;
		}

		if (IsWindows7SP1OrGreater())
		{
			ret = OS_VERSION_WINDOWS_7_SP1;
			__leave;
		}

		if (IsWindows7OrGreater())
		{
			ret = OS_VERSION_WINDOWS_7;
			__leave;
		}

		if (IsWindowsVistaSP2OrGreater())
		{
			ret = OS_VERSION_WINDOWS_VISTA_PS2;
			__leave;
		}

		if (IsWindowsVistaSP1OrGreater())
		{
			ret = OS_VERSION_WINDOWS_VISTA_PS1;
			__leave;
		}

		if (IsWindowsVistaOrGreater())
		{
			ret = OS_VERSION_WINDOWS_VISTA;
			__leave;
		}

		if (IsWindowsXPSP3OrGreater())
		{
			ret = OS_VERSION_WINDOWS_XP_SP3;
			__leave;
		}

		if (IsWindowsXPSP2OrGreater())
		{
			ret = OS_VERSION_WINDOWS_XP_SP2;
			__leave;
		}

		if (IsWindowsXPSP1OrGreater())
		{
			ret = OS_VERSION_WINDOWS_XP_SP1;
			__leave;
		}

		if (IsWindowsXPOrGreater())
		{
			ret = OS_VERSION_WINDOWS_XP;
			__leave;
		}
	}
	__finally
	{
		;
	}

	return ret;
}

OS_VERSION_USER_DEFINED
COperationSystemVersion::GetOSVersionByGetVersionEx()
{
	OS_VERSION_USER_DEFINED	ret = OS_VERSION_UNKNOWN;

	OSVERSIONINFOEX			OsVersionInfoEx = { 0 };


	__try
	{
		OsVersionInfoEx.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		if (!GetVersionEx((LPOSVERSIONINFO)&OsVersionInfoEx))
			__leave;

		switch (OsVersionInfoEx.dwPlatformId)
		{
			case VER_PLATFORM_WIN32s:
				break;
			case VER_PLATFORM_WIN32_WINDOWS:
				break;
			case VER_PLATFORM_WIN32_NT:
			{
				switch (OsVersionInfoEx.dwMajorVersion)
				{
					case 5:
					{
						switch (OsVersionInfoEx.dwMinorVersion)
						{
							case 0:
							{
								ret = OS_VERSION_WINDOWS_2000;
								break;
							}
							case 1:
							{
								ret = OS_VERSION_WINDOWS_XP;
								break;
							}
							case 2:
							{
								if (VER_NT_WORKSTATION == OsVersionInfoEx.wProductType)
									ret = OS_VERSION_WINDOWS_XP;
								else
								{
									if (VER_SUITE_WH_SERVER == OsVersionInfoEx.wSuiteMask)
										ret = OS_VERSION_WINDOWS_HOME_SERVER;
									else
										ret = OS_VERSION_WINDOWS_SERVER_2003;
								}

								break;
							}
							default:
								break;
						}

						break;
					}
					case 6:
					{
						switch (OsVersionInfoEx.dwMinorVersion)
						{
							case 0:
							{
								if (VER_NT_WORKSTATION == OsVersionInfoEx.wProductType)
									ret = OS_VERSION_WINDOWS_VISTA;
								else
									ret = OS_VERSION_WINDOWS_SERVER_2008;

								break;
							}
							case 1:
							{
								if (VER_NT_WORKSTATION == OsVersionInfoEx.wProductType)
									ret = OS_VERSION_WINDOWS_7;
								else
									ret = OS_VERSION_WINDOWS_SERVER_2008_R2;

								break;
							}
							case 2:
							{
								ret = OS_VERSION_WINDOWS_8;
								break;
							}
							default:
								break;
						}

						break;
					}
					default:
						break;
				}

				break;
			}
			default:
				break;
		}
	}
	__finally
	{
		;
	}

	return ret;
}

OS_PROCESSOR_TYPE_USER_DEFINED
COperationSystemVersion::GetOSProcessorType()
{
	return ms_ProcessorTypeUserDefined;
}

OS_PROCESSOR_TYPE_USER_DEFINED
COperationSystemVersion::GetOSProcessorTypeInternal()
{
	OS_PROCESSOR_TYPE_USER_DEFINED	ret = OS_PROCESSOR_TYPE_UNKNOWN;

	SYSTEM_INFO						systemInfo = { 0 };


	__try
	{
		GetNativeSystemInfo(&systemInfo);
		switch (systemInfo.wProcessorArchitecture)
		{
			case PROCESSOR_ARCHITECTURE_INTEL:
			{
				ret = OS_PROCESSOR_TYPE_X86;
				break;
			}
			case PROCESSOR_ARCHITECTURE_AMD64:
			{
				ret = OS_PROCESSOR_TYPE_X64;
				break;
			}
			default:
				__leave;
		}
	}
	__finally
	{
		;
	}

	return ret;
}
