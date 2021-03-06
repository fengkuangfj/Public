
#pragma once

#include "..\\Wow64DisableWow64FsRedirection\\Wow64DisableWow64FsRedirection.h"

class CRegOperation
{
public:
	static
		LSTATUS
		APIENTRY
		RegOpenKeyEx(
		__in HKEY hKey,
		__in_opt LPCSTR lpSubKey,
		__reserved DWORD ulOptions,
		__in REGSAM samDesired,
		__out PHKEY phkResult
		);

	static
		LSTATUS
		APIENTRY
		RegOpenKeyEx(
		__in HKEY hKey,
		__in_opt LPCWSTR lpSubKey,
		__reserved DWORD ulOptions,
		__in REGSAM samDesired,
		__out PHKEY phkResult
		);

	static
		LSTATUS
		APIENTRY
		RegCreateKeyEx(
		__in HKEY hKey,
		__in LPCSTR lpSubKey,
		__reserved DWORD Reserved,
		__in_opt LPSTR lpClass,
		__in DWORD dwOptions,
		__in REGSAM samDesired,
		__in_opt CONST LPSECURITY_ATTRIBUTES lpSecurityAttributes,
		__out PHKEY phkResult,
		__out_opt LPDWORD lpdwDisposition
		);

	static
		LSTATUS
		APIENTRY
		RegCreateKeyEx(
		__in HKEY hKey,
		__in LPCWSTR lpSubKey,
		__reserved DWORD Reserved,
		__in_opt LPWSTR lpClass,
		__in DWORD dwOptions,
		__in REGSAM samDesired,
		__in_opt CONST LPSECURITY_ATTRIBUTES lpSecurityAttributes,
		__out PHKEY phkResult,
		__out_opt LPDWORD lpdwDisposition
		);

	static
		LSTATUS
		APIENTRY
		RegQueryValueEx(
		__in HKEY hKey,
		__in_opt LPCSTR lpValueName,
		__reserved LPDWORD lpReserved,
		__out_opt LPDWORD lpType,
		__out_bcount_part_opt(*lpcbData, *lpcbData) __out_data_source(REGISTRY) LPBYTE lpData,
		__inout_opt LPDWORD lpcbData
		);

	static
		LSTATUS
		APIENTRY
		RegQueryValueEx(
		__in HKEY hKey,
		__in_opt LPCWSTR lpValueName,
		__reserved LPDWORD lpReserved,
		__out_opt LPDWORD lpType,
		__out_bcount_part_opt(*lpcbData, *lpcbData) __out_data_source(REGISTRY) LPBYTE lpData,
		__inout_opt LPDWORD lpcbData
		);

private:
	CRegOperation();

	~CRegOperation();
};
