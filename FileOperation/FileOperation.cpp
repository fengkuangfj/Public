
#include "FileOperation.h"

CFileOperation::CFileOperation()
{
	;
}

CFileOperation::~CFileOperation()
{
	;
}

BOOL
CFileOperation::CopyFile(
						 __in LPCSTR lpExistingFileName,
						 __in LPCSTR lpNewFileName,
						 __in BOOL bFailIfExists
						 )
{
	CWow64DisableWow64FsRedirection Wow64DisableWow64FsRedirection;
	return ::CopyFileA(lpExistingFileName, lpNewFileName, bFailIfExists);
}

BOOL
CFileOperation::CopyFile(
						 __in LPCWSTR lpExistingFileName,
						 __in LPCWSTR lpNewFileName,
						 __in BOOL bFailIfExists
						 )
{
	CWow64DisableWow64FsRedirection Wow64DisableWow64FsRedirection;
	return ::CopyFileW(lpExistingFileName, lpNewFileName, bFailIfExists);
}

BOOL
CFileOperation::DeleteFile(
						   __in LPCSTR lpFileName
						   )
{
	CWow64DisableWow64FsRedirection Wow64DisableWow64FsRedirection;
	return ::DeleteFileA(lpFileName);
}

BOOL
CFileOperation::DeleteFile(
						   __in LPCWSTR lpFileName
						   )
{
	CWow64DisableWow64FsRedirection Wow64DisableWow64FsRedirection;
	return ::DeleteFileW(lpFileName);
}

BOOL
CFileOperation::PathFileExists(
							   __in LPCSTR pszPath
							   )
{
	CWow64DisableWow64FsRedirection Wow64DisableWow64FsRedirection;
	return ::PathFileExistsA(pszPath);
}

BOOL
CFileOperation::PathFileExists(
							   __in LPCWSTR pszPath
							   )
{
	CWow64DisableWow64FsRedirection Wow64DisableWow64FsRedirection;
	return ::PathFileExistsW(pszPath);
}
