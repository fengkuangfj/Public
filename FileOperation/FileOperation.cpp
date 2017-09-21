
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

BOOL
CFileOperation::MoveFileEx(
						   __in     LPCSTR lpExistingFileName,
						   __in_opt LPCSTR lpNewFileName,
						   __in     DWORD    dwFlags
						   )
{
	CWow64DisableWow64FsRedirection Wow64DisableWow64FsRedirection;
	return ::MoveFileExA(lpExistingFileName, lpNewFileName, dwFlags);
}

BOOL
CFileOperation::MoveFileEx(
						   __in     LPCWSTR lpExistingFileName,
						   __in_opt LPCWSTR lpNewFileName,
						   __in     DWORD    dwFlags
						   )
{
	CWow64DisableWow64FsRedirection Wow64DisableWow64FsRedirection;
	return ::MoveFileExW(lpExistingFileName, lpNewFileName, dwFlags);
}

BOOL
CFileOperation::MoveFile(
						 __in		LPCTSTR	lpSrc,
						 __in		LPCTSTR	lpDes,
						 __inout	PBOOL	pbReboot
						 )
{
	BOOL	bRet = FALSE;

	TCHAR	tchDesTemp[MAX_PATH] = {0};
	TCHAR	tchTempDir[MAX_PATH] = {0};
	TCHAR	tchGuid[MAX_PATH] = {0};


	__try
	{
		if (!lpSrc || !lpDes || !pbReboot)
			__leave;

		if (!GetTempPath(_countof(tchTempDir), tchTempDir))
			__leave;

		if (!MoveFileEx(lpSrc, lpDes, MOVEFILE_REPLACE_EXISTING))
		{
			if (!GenGuid(tchGuid, _countof(tchGuid)))
				__leave;

			StringCchPrintf(tchDesTemp, _countof(tchDesTemp), _T("%s%s"), tchTempDir, tchGuid);

			if (!MoveFile(lpDes, tchDesTemp))
			{
				if (!MoveFileEx(lpSrc, lpDes, MOVEFILE_REPLACE_EXISTING | MOVEFILE_DELAY_UNTIL_REBOOT))
					__leave;

				*pbReboot = TRUE;
			}
			else
			{
				MoveFileEx(tchDesTemp, NULL, MOVEFILE_DELAY_UNTIL_REBOOT);

				if (!MoveFile(lpSrc, lpDes))
					__leave;
			}
		}

		bRet = TRUE;
	}
	__finally
	{
		;
	}

	return bRet;
}

BOOL
CFileOperation::CopyFile(
						 __in		LPCTSTR	lpSrc,
						 __in		LPCTSTR	lpDes,
						 __inout	PBOOL	pbReboot
						 )
{
	BOOL	bRet = FALSE;

	TCHAR	tchSrcTemp[MAX_PATH] = {0};
	TCHAR	tchDesTemp[MAX_PATH] = {0};
	TCHAR	tchTempDir[MAX_PATH] = {0};
	TCHAR	tchGuid[MAX_PATH] = {0};


	__try
	{
		if (!lpSrc || !lpDes || !pbReboot)
			__leave;

		if (!GetTempPath(_countof(tchTempDir), tchTempDir))
			__leave;

		if (!CopyFile(lpSrc, lpDes, FALSE))
		{
			if (!GenGuid(tchGuid, _countof(tchGuid)))
				__leave;

			StringCchPrintf(tchDesTemp, _countof(tchDesTemp), _T("%s%s"), tchTempDir, tchGuid);

			if (!MoveFile(lpDes, tchDesTemp))
			{
				if (!GenGuid(tchGuid, _countof(tchGuid)))
					__leave;

				StringCchPrintf(tchSrcTemp, _countof(tchSrcTemp), _T("%s%s"), tchTempDir, tchGuid);

				if (!CopyFile(lpSrc, tchSrcTemp, FALSE))
					__leave;

				if (!MoveFileEx(tchSrcTemp, lpDes, MOVEFILE_REPLACE_EXISTING | MOVEFILE_DELAY_UNTIL_REBOOT))
					__leave;

				*pbReboot = TRUE;
			}
			else
			{
				MoveFileEx(tchDesTemp, NULL, MOVEFILE_DELAY_UNTIL_REBOOT);

				if (!CopyFile(lpSrc, lpDes, FALSE))
					__leave;
			}
		}

		bRet = TRUE;
	}
	__finally
	{
		;
	}

	return bRet;
}

BOOL
CFileOperation::MoveFile(
						 __in LPCSTR lpExistingFileName,
						 __in LPCSTR lpNewFileName
						 )
{

	CWow64DisableWow64FsRedirection Wow64DisableWow64FsRedirection;
	return ::MoveFileA(lpExistingFileName, lpNewFileName);
}

BOOL
CFileOperation::MoveFile(
						 __in LPCWSTR lpExistingFileName,
						 __in LPCWSTR lpNewFileName
						 )
{

	CWow64DisableWow64FsRedirection Wow64DisableWow64FsRedirection;
	return ::MoveFileW(lpExistingFileName, lpNewFileName);
}
