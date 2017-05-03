
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
