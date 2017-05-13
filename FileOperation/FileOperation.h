
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

	static
		BOOL
		WINAPI
		MoveFileEx(
		__in     LPCSTR lpExistingFileName,
		__in_opt LPCSTR lpNewFileName,
		__in     DWORD    dwFlags
		);

	static
		BOOL
		WINAPI
		MoveFileEx(
		__in     LPCWSTR lpExistingFileName,
		__in_opt LPCWSTR lpNewFileName,
		__in     DWORD    dwFlags
		);

	static
		BOOL
		MoveFile(
		__in	LPCTSTR	lpSrc,
		__in	LPCTSTR	lpDes,
		__inout	PBOOL	pbReboot
		);

	static
		BOOL
		CopyFile(
		__in	LPCTSTR	lpSrc,
		__in	LPCTSTR	lpDes,
		__inout	PBOOL	pbReboot
		);

	static
		BOOL
		WINAPI
		MoveFile(
		__in LPCSTR lpExistingFileName,
		__in LPCSTR lpNewFileName
		);

	static
		BOOL
		WINAPI
		MoveFile(
		__in LPCWSTR lpExistingFileName,
		__in LPCWSTR lpNewFileName
		);

private:
	CFileOperation();

	~CFileOperation();
};
