
#pragma once

#include <Windows.h>
#include <shlwapi.h>

#pragma comment(lib, "shlwapi.lib")

#include "..\\PrintfEx\\PrintfEx.h"
#include "..\\StorageDevice\\StorageDevice.h"

#define MOD_DIRECTORY_CONTROL _T("Ä¿Â¼¿ØÖÆ")

class CDirectoryControl
{
public:
	static
		BOOL
		Delete(
		__in LPTSTR	lptchDirPath,
		__in BOOL	bDealRemovableStorageDevice = FALSE
		);

	static
		BOOL
		Empty(
		__in LPTSTR	lptchDirPath,
		__in BOOL	bDealRemovableStorageDevice = FALSE
		);

	static
		BOOL
		DeleteInternalFile(
		__in LPTSTR	lptchDirPath,
		__in LPTSTR	lpFileName,
		__in BOOL	bWildcard,
		__in BOOL	bDealRemovableStorageDevice = FALSE
		);

	static
		BOOL
		DeleteInternalDir(
		__in LPTSTR	lptchDirPath,
		__in LPTSTR	lpDirName,
		__in BOOL	bWildcard,
		__in BOOL	bDealRemovableStorageDevice = FALSE
		);

	static
		BOOL
		EmptyExceptFile(
		__in LPTSTR	lptchDirPath,
		__in LPTSTR	lptchFileName,
		__in BOOL	bWildcard,
		__in BOOL	bDealRemovableStorageDevice = FALSE
		);

	static
		BOOL
		DeleteExceptFile(
		__in LPTSTR	lptchDirPath,
		__in LPTSTR	lptchFileName,
		__in BOOL	bWildcard,
		__in BOOL	bDealRemovableStorageDevice = FALSE
		);

	static
		BOOL
		Copy(
		__in LPTSTR lpSrc,
		__in LPTSTR lpDes
		);

private:
	static
		BOOL
		Control(
		__in LPTSTR	lptchDirPath,
		__in BOOL	bDelete,
		__in BOOL	bDealRemovableStorageDevice
		);
};
