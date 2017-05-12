
#include "RegOperation.h"

CRegOperation::CRegOperation()
{
	;
}

CRegOperation::~CRegOperation()
{
	;
}

LSTATUS
CRegOperation::RegOpenKeyEx(
							__in HKEY hKey,
							__in_opt LPCSTR lpSubKey,
							__reserved DWORD ulOptions,
							__in REGSAM samDesired,
							__out PHKEY phkResult
							)
{
	CWow64DisableWow64FsRedirection Wow64DisableWow64FsRedirection;
	return ::RegOpenKeyExA(hKey, lpSubKey, ulOptions, samDesired, phkResult);
}

LSTATUS
CRegOperation::RegOpenKeyEx(
							__in HKEY hKey,
							__in_opt LPCWSTR lpSubKey,
							__reserved DWORD ulOptions,
							__in REGSAM samDesired,
							__out PHKEY phkResult
							)
{
	CWow64DisableWow64FsRedirection Wow64DisableWow64FsRedirection;
	return ::RegOpenKeyExW(hKey, lpSubKey, ulOptions, samDesired, phkResult);
}

LSTATUS
CRegOperation::RegCreateKeyEx(
							  __in HKEY hKey,
							  __in LPCSTR lpSubKey,
							  __reserved DWORD Reserved,
							  __in_opt LPSTR lpClass,
							  __in DWORD dwOptions,
							  __in REGSAM samDesired,
							  __in_opt CONST LPSECURITY_ATTRIBUTES lpSecurityAttributes,
							  __out PHKEY phkResult,
							  __out_opt LPDWORD lpdwDisposition
							  )
{
	CWow64DisableWow64FsRedirection Wow64DisableWow64FsRedirection;
	return ::RegCreateKeyExA(hKey, lpSubKey, Reserved, lpClass, dwOptions, samDesired, lpSecurityAttributes, phkResult, lpdwDisposition);
}

LSTATUS
CRegOperation::RegCreateKeyEx(
							  __in HKEY hKey,
							  __in LPCWSTR lpSubKey,
							  __reserved DWORD Reserved,
							  __in_opt LPWSTR lpClass,
							  __in DWORD dwOptions,
							  __in REGSAM samDesired,
							  __in_opt CONST LPSECURITY_ATTRIBUTES lpSecurityAttributes,
							  __out PHKEY phkResult,
							  __out_opt LPDWORD lpdwDisposition
							  )
{
	CWow64DisableWow64FsRedirection Wow64DisableWow64FsRedirection;
	return ::RegCreateKeyExW(hKey, lpSubKey, Reserved, lpClass, dwOptions, samDesired, lpSecurityAttributes, phkResult, lpdwDisposition);
}

LSTATUS
CRegOperation::RegQueryValueEx(
							   __in HKEY hKey,
							   __in_opt LPCSTR lpValueName,
							   __reserved LPDWORD lpReserved,
							   __out_opt LPDWORD lpType,
							   __out_bcount_part_opt(*lpcbData, *lpcbData) __out_data_source(REGISTRY) LPBYTE lpData,
							   __inout_opt LPDWORD lpcbData
							   )
{
	CWow64DisableWow64FsRedirection Wow64DisableWow64FsRedirection;
	return ::RegQueryValueExA(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
}

LSTATUS
CRegOperation::RegQueryValueEx(
							   __in HKEY hKey,
							   __in_opt LPCWSTR lpValueName,
							   __reserved LPDWORD lpReserved,
							   __out_opt LPDWORD lpType,
							   __out_bcount_part_opt(*lpcbData, *lpcbData) __out_data_source(REGISTRY) LPBYTE lpData,
							   __inout_opt LPDWORD lpcbData
							   )
{
	CWow64DisableWow64FsRedirection Wow64DisableWow64FsRedirection;
	return ::RegQueryValueExW(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
}
