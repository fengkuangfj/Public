
#pragma once

#include "..\\Wow64DisableWow64FsRedirection\\Wow64DisableWow64FsRedirection.h"

class CFileOperation
{
public:
	static
		BOOL
		WINAPI
		CopyFile(
		__in LPCSTR lpExistingFileName,
		__in LPCSTR lpNewFileName,
		__in BOOL bFailIfExists
		);

	static
		BOOL
		WINAPI
		CopyFile(
		__in LPCWSTR lpExistingFileName,
		__in LPCWSTR lpNewFileName,
		__in BOOL bFailIfExists
		);

	static
		BOOL
		WINAPI
		DeleteFile(
		__in LPCSTR lpFileName
		);

	static
		BOOL
		WINAPI
		DeleteFile(
		__in LPCWSTR lpFileName
		);

	static
		BOOL
		WINAPI
		PathFileExists(
		__in LPCSTR pszPath
		);

	static
		BOOL
		WINAPI
		PathFileExists(
		__in LPCWSTR pszPath
		);

private:
	CFileOperation();

	~CFileOperation();
};
