#include "OperationSystemVersion.h"

COperationSystemVersion	* COperationSystemVersion::ms_pInstance = NULL;

BOOL
COperationSystemVersion::Init()
{
	BOOL bRet = FALSE;

	
	__try
	{
		m_hModule = LoadLibrary(_T("Kernel32.dll"));
		if (!m_hModule)
			__leave;

		m_pfIsWindowsServer = (IS_WINDOWS_SERVER)GetProcAddress(m_hModule, "IsWindowsServer");
		m_pfIsWindows1OrGreater = (IS_WINDOWS_10_OR_GREATER)GetProcAddress(m_hModule, "IsWindows1OrGreater");
		m_pfIsWindows8Point1OrGreater = (IS_WINDOWS_8_POINT_1_OR_GREATER)GetProcAddress(m_hModule, "IsWindows8Point1OrGreater");
		m_pfIsWindows8OrGreater = (IS_WINDOWS_8_OR_GREATER)GetProcAddress(m_hModule, "IsWindows8OrGreater");
		m_pfIsWindows7SP1OrGreater = (IS_WINDOWS_7_SP_1_OR_GREATER)GetProcAddress(m_hModule, "IsWindows7SP1OrGreater");
		m_pfIsWindows7OrGreater = (IS_WINDOWS_7_OR_GREATER)GetProcAddress(m_hModule, "IsWindows7OrGreater");
		m_pfIsWindowsVistaSP2OrGreater = (IS_WINDOWS_VISTA_SP2_OR_GREATER)GetProcAddress(m_hModule, "IsWindowsVistaSP2OrGreater");
		m_pfIsWindowsVistaSP1OrGreater = (IS_WINDOWS_VISTA_SP1_OR_GREATER)GetProcAddress(m_hModule, "IsWindowsVistaSP1OrGreater");
		m_pfIsWindowsVistaOrGreater = (IS_WINDOWS_VISTA_OR_GREATER)GetProcAddress(m_hModule, "IsWindowsVistaOrGreater");
		m_pfIsWindowsXPSP3OrGreater = (IS_WINDOWS_XP_SP3_OR_GREATER)GetProcAddress(m_hModule, "IsWindowsXPSP3OrGreater");
		m_pfIsWindowsXPSP2OrGreater = (IS_WINDOWS_XP_SP2_OR_GREATER)GetProcAddress(m_hModule, "IsWindowsXPSP2OrGreater");
		m_pfIsWindowsXPSP1OrGreater = (IS_WINDOWS_XP_SP1_OR_GREATER)GetProcAddress(m_hModule, "IsWindowsXPSP1OrGreater");
		m_pfIsWindowsXPOrGreater = (IS_WINDOWS_XP_OR_GREATER)GetProcAddress(m_hModule, "IsWindowsXPOrGreater");
		if (m_pfIsWindowsXPOrGreater)
			m_OsVersionUserDefined = GetOSVersionByIsOrGreater();
		else
		{
			m_pfGetVersionEx = (GET_VERSION_EX)GetProcAddress(m_hModule, "GetVersionExW");
			if (!m_pfGetVersionEx)
				__leave;

			m_OsVersionUserDefined = GetOSVersionByGetVersionEx();
		}

		m_ProcessorTypeUserDefined = GetOSProcessorTypeInternal();

		bRet = TRUE;
	}
	__finally
	{
		if (!bRet)
			Unload();
	}

	return bRet;
}

OS_VERSION_USER_DEFINED
COperationSystemVersion::GetOSVersion()
{
	return m_OsVersionUserDefined;
}

OS_VERSION_USER_DEFINED
COperationSystemVersion::GetOSVersionByIsOrGreater()

{
	OS_VERSION_USER_DEFINED	ret = OS_VERSION_UNKNOWN;


	__try
	{
		if (m_pfIsWindowsServer())
		{
			ret = OS_VERSION_WINDOWS_SERVER;
			__leave;
		}

		if (m_pfIsWindows1OrGreater())
		{
			ret = OS_VERSION_WINDOWS_10;
			__leave;
		}

		if (m_pfIsWindows8Point1OrGreater())
		{
			ret = OS_VERSION_WINDOWS_8_POINT1;
			__leave;
		}

		if (m_pfIsWindows8OrGreater())
		{
			ret = OS_VERSION_WINDOWS_8;
			__leave;
		}

		if (m_pfIsWindows7SP1OrGreater())
		{
			ret = OS_VERSION_WINDOWS_7_SP1;
			__leave;
		}

		if (m_pfIsWindows7OrGreater())
		{
			ret = OS_VERSION_WINDOWS_7;
			__leave;
		}

		if (m_pfIsWindowsVistaSP2OrGreater())
		{
			ret = OS_VERSION_WINDOWS_VISTA_PS2;
			__leave;
		}

		if (m_pfIsWindowsVistaSP1OrGreater())
		{
			ret = OS_VERSION_WINDOWS_VISTA_PS1;
			__leave;
		}

		if (m_pfIsWindowsVistaOrGreater())
		{
			ret = OS_VERSION_WINDOWS_VISTA;
			__leave;
		}

		if (m_pfIsWindowsXPSP3OrGreater())
		{
			ret = OS_VERSION_WINDOWS_XP_SP3;
			__leave;
		}

		if (m_pfIsWindowsXPSP2OrGreater())
		{
			ret = OS_VERSION_WINDOWS_XP_SP2;
			__leave;
		}

		if (m_pfIsWindowsXPSP1OrGreater())
		{
			ret = OS_VERSION_WINDOWS_XP_SP1;
			__leave;
		}

		if (m_pfIsWindowsXPOrGreater())
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
		if (!m_pfGetVersionEx((LPOSVERSIONINFO)&OsVersionInfoEx))
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
								if (VER_NT_WORKSTATION == OsVersionInfoEx.wProductType)
									ret = OS_VERSION_WINDOWS_8;
								else
									ret = OS_VERSION_WINDOWS_SERVER_2012;

								break;
							}
							case 3:
							{
								if (VER_NT_WORKSTATION == OsVersionInfoEx.wProductType)
									ret = OS_VERSION_WINDOWS_8_POINT1;

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

		if (OS_VERSION_UNKNOWN < ret)
			ret = (OS_VERSION_USER_DEFINED)(ret + OsVersionInfoEx.wServicePackMajor);
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
	return m_ProcessorTypeUserDefined;
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

BOOL
	COperationSystemVersion::Unload()
{
	BOOL bRet = TRUE;


	__try
	{
		m_pfIsWindowsServer = NULL;
		m_pfIsWindows1OrGreater = NULL;
		m_pfIsWindows8Point1OrGreater = NULL;
		m_pfIsWindows8OrGreater = NULL;
		m_pfIsWindows7SP1OrGreater = NULL;
		m_pfIsWindows7OrGreater = NULL;
		m_pfIsWindowsVistaSP2OrGreater = NULL;
		m_pfIsWindowsVistaSP1OrGreater = NULL;
		m_pfIsWindowsVistaOrGreater = NULL;
		m_pfIsWindowsXPSP3OrGreater = NULL;
		m_pfIsWindowsXPSP2OrGreater = NULL;
		m_pfIsWindowsXPSP1OrGreater = NULL;
		m_pfIsWindowsXPOrGreater = NULL;
		m_pfGetVersionEx = NULL;

		if (m_hModule)
		{
			FreeLibrary(m_hModule);
			m_hModule = NULL;
		}
	}
	__finally
	{
		;
	}

	return bRet;
}

COperationSystemVersion *
	COperationSystemVersion::GetInstance()
{
	if (!ms_pInstance)
	{
		do 
		{
			ms_pInstance = new COperationSystemVersion;
			if (!ms_pInstance)
				Sleep(1000);
			else
				break;
		} while (TRUE);
	}

	return ms_pInstance;
}

VOID
	COperationSystemVersion::ReleaseInstance()
{
	if (ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = NULL;
	}
}

COperationSystemVersion::COperationSystemVersion()
{
	m_OsVersionUserDefined = OS_VERSION_UNKNOWN;
	m_ProcessorTypeUserDefined = OS_PROCESSOR_TYPE_UNKNOWN;

	Init();
}

COperationSystemVersion::~COperationSystemVersion()
{
	Unload();

	m_OsVersionUserDefined = OS_VERSION_UNKNOWN;
	m_ProcessorTypeUserDefined = OS_PROCESSOR_TYPE_UNKNOWN;
}
