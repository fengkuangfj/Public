
#include "FileVersion.h"

CFileVersion::CFileVersion()
{
	;
}

CFileVersion::~CFileVersion()
{
	;
}

BOOL
CFileVersion::Get(
				  __in		LPCTSTR	lpPath,
				  __inout	LPTSTR	lpVersion,
				  __in		ULONG	ulBufSizeCh
				  )
{
	BOOL				bRet = FALSE;

	FILE_VERSION_INFO	FileVersionInfo = {0};


	__try
	{
		if (!lpPath || !lpVersion || !ulBufSizeCh)
			__leave;

		if (!Get(lpPath, &FileVersionInfo))
			__leave;

		StringCchPrintf(lpVersion, ulBufSizeCh, _T("%d.%d.%d.%d"),
			FileVersionInfo.ulMajorVersion,
			FileVersionInfo.ulMinorVersion,
			FileVersionInfo.ulPatchVersion,
			FileVersionInfo.ulBuildVersion
			);

		bRet = TRUE;
	}
	__finally
	{
		;
	}

	return bRet;
}

BOOL
CFileVersion::Get(
				  __in		LPCTSTR				lpPath,
				  __inout	LPFILE_VERSION_INFO lpFileVersionInfo
				  )
{
	BOOL				bRet = FALSE;

	DWORD				dwResult = 0;
	LPVOID				lpData = NULL;
	VS_FIXEDFILEINFO *	pVsFixedFileInfo = NULL;
	UINT				nLen = 0;


	__try
	{
		if (!lpPath || !lpFileVersionInfo)
			__leave;

		dwResult = GetFileVersionInfoSize(lpPath, NULL);  
		if (!dwResult)
			__leave;

		lpData = calloc(1, dwResult);
		if (!lpData)
			__leave;

		if (!GetFileVersionInfo(lpPath, 0, dwResult, lpData))
			__leave;

		if (!VerQueryValue(lpData, _T("\\"), (LPVOID *)&pVsFixedFileInfo, &nLen))
			__leave;

		lpFileVersionInfo->ulMajorVersion = HIWORD(pVsFixedFileInfo->dwFileVersionMS);
		lpFileVersionInfo->ulMinorVersion = LOWORD(pVsFixedFileInfo->dwFileVersionMS);
		lpFileVersionInfo->ulPatchVersion = HIWORD(pVsFixedFileInfo->dwFileVersionLS);
		lpFileVersionInfo->ulBuildVersion = LOWORD(pVsFixedFileInfo->dwFileVersionLS);

		bRet = TRUE;
	}
	__finally
	{
		if (lpData)
		{
			free(lpData);
			lpData = NULL;
		}
	}

	return bRet;
}
