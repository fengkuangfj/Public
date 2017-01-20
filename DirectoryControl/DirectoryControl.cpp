
#include "DirectoryControl.h"

BOOL
CDirectoryControl::Delete(
						  __in LPTSTR lptchDirPath
						  )
{
	return Control(lptchDirPath, TRUE);
}

BOOL
CDirectoryControl::Empty(
						 __in LPTSTR lptchDirPath
						 )
{
	return Control(lptchDirPath, FALSE);
}

BOOL
CDirectoryControl::Control(
						   __in LPTSTR	lptchDirPath,
						   __in BOOL	bDelete
						   )
{
	BOOL			bRet = FALSE;

	TCHAR			tchDirExpression[MAX_PATH] = {0};
	HANDLE			hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA ffd = { 0 };
	TCHAR			tchPath[MAX_PATH] = {0};


	__try
	{
		if (!lptchDirPath)
		{
			printfEx(MOD_DIRECTORY_CONTROL, PRINTF_LEVEL_ERROR, "input argument error");
			__leave;
		}

		if (!PathFileExists(lptchDirPath))
		{
			printfEx(MOD_DIRECTORY_CONTROL, PRINTF_LEVEL_WARNING, "not exist. (%S)",
				lptchDirPath);

			__leave;
		}

		StringCchPrintf(tchDirExpression, _countof(tchDirExpression), _T("%s\\*"), lptchDirPath);

		hFind = FindFirstFile(tchDirExpression, &ffd);
		if (INVALID_HANDLE_VALUE == hFind)
		{
			printfEx(MOD_DIRECTORY_CONTROL, PRINTF_LEVEL_ERROR, "FindFirstFile failed. (%S) (%d)",
				tchDirExpression, GetLastError());

			__leave;
		}

		do
		{
			StringCchPrintf(tchPath, _countof(tchPath), _T("%s\\%s"), lptchDirPath, ffd.cFileName);

			if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (0 != _tcsnicmp(ffd.cFileName, _T("."), wcslen(_T("."))) &&
					0 != _tcsnicmp(ffd.cFileName, _T(".."), wcslen(_T(".."))))
				{
					if (!Delete(tchPath))
						printfEx(MOD_DIRECTORY_CONTROL, PRINTF_LEVEL_WARNING, "Delete failed. (%S)",
						tchPath);
				}
			}
			else
			{
				if (!DeleteFile(tchPath))
				{
					if (!MoveFileEx(tchPath, NULL, MOVEFILE_DELAY_UNTIL_REBOOT))
						printfEx(MOD_DIRECTORY_CONTROL, PRINTF_LEVEL_WARNING, "MoveFileEx failed. (%S) (%d)",
						tchPath, GetLastError());
				}
			}
		} while (0 != FindNextFile(hFind, &ffd));

		if (ERROR_NO_MORE_FILES != GetLastError())
		{
			printfEx(MOD_DIRECTORY_CONTROL, PRINTF_LEVEL_ERROR, "FindNextFile failed. 2 (%S) (%d)",
				tchDirExpression, GetLastError());

			__leave;
		}

		if (bDelete)
		{
			if (!RemoveDirectory(lptchDirPath))
			{
				if (145 != GetLastError())
				{
					printfEx(MOD_DIRECTORY_CONTROL, PRINTF_LEVEL_ERROR, "RemoveDirectory failed. (%S) (%d)",
						lptchDirPath, GetLastError());

					__leave;
				}
				else
					printfEx(MOD_DIRECTORY_CONTROL, PRINTF_LEVEL_WARNING, "RemoveDirectory failed. (%S) (%d)",
					lptchDirPath, GetLastError());
			}
		}

		bRet = TRUE;
	}
	__finally
	{
		if (INVALID_HANDLE_VALUE != hFind)
		{
			FindClose(hFind);
			hFind = INVALID_HANDLE_VALUE;
		}
	}

	return bRet;
}

BOOL
CDirectoryControl::DeleteInternalFile(
									  __in LPTSTR	lptchDirPath,
									  __in LPTSTR	lpFileName,
									  __in BOOL		bWildcard
									  )
{
	BOOL			bRet = FALSE;

	TCHAR			tchDirExpression[MAX_PATH] = {0};
	HANDLE			hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA ffd = { 0 };
	TCHAR			tchPath[MAX_PATH] = {0};


	__try
	{
		if (!lptchDirPath || !lpFileName)
		{
			printfEx(MOD_DIRECTORY_CONTROL, PRINTF_LEVEL_ERROR, "input arguments error. lptchDirPath(%p) lpFileName(%p)",
				lptchDirPath, lpFileName);

			__leave;
		}

		if (!PathFileExists(lptchDirPath))
		{
			printfEx(MOD_DIRECTORY_CONTROL, PRINTF_LEVEL_WARNING, "not exist. (%S)",
				lptchDirPath);

			__leave;
		}

		StringCchPrintf(tchDirExpression, _countof(tchDirExpression), _T("%s\\*"), lptchDirPath);

		hFind = FindFirstFile(tchDirExpression, &ffd);
		if (INVALID_HANDLE_VALUE == hFind)
		{
			printfEx(MOD_DIRECTORY_CONTROL, PRINTF_LEVEL_ERROR, "FindFirstFile failed. (%S) (%d)",
				tchDirExpression, GetLastError());

			__leave;
		}

		do
		{
			StringCchPrintf(tchPath, _countof(tchPath), _T("%s\\%s"), lptchDirPath, ffd.cFileName);

			if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (0 != _tcsnicmp(ffd.cFileName, _T("."), wcslen(_T("."))) &&
					0 != _tcsnicmp(ffd.cFileName, _T(".."), wcslen(_T(".."))))
				{
					if (!DeleteInternalFile(tchPath, lpFileName, bWildcard))
						printfEx(MOD_DIRECTORY_CONTROL, PRINTF_LEVEL_WARNING, "DeleteInternalFile failed. (%S)",
						tchPath);
				}
			}
			else
			{
				if (bWildcard)
				{
					if (NULL == StrRStrI(ffd.cFileName, NULL, lpFileName))
						continue;
				}
				else
				{
					if (0 != _tcsicmp(ffd.cFileName, lpFileName))
						continue;
				}

				if (!DeleteFile(tchPath))
				{
					if (!MoveFileEx(tchPath, NULL, MOVEFILE_DELAY_UNTIL_REBOOT))
						printfEx(MOD_DIRECTORY_CONTROL, PRINTF_LEVEL_WARNING, "MoveFileEx failed. (%S) (%d)",
						tchPath, GetLastError());
				}
			}
		} while (0 != FindNextFile(hFind, &ffd));

		if (ERROR_NO_MORE_FILES != GetLastError())
		{
			printfEx(MOD_DIRECTORY_CONTROL, PRINTF_LEVEL_ERROR, "FindNextFile failed. 2 (%S) (%d)",
				tchDirExpression, GetLastError());

			__leave;
		}

		bRet = TRUE;
	}
	__finally
	{
		if (INVALID_HANDLE_VALUE != hFind)
		{
			FindClose(hFind);
			hFind = INVALID_HANDLE_VALUE;
		}
	}

	return bRet;
}

BOOL
CDirectoryControl::DeleteInternalDir(
									 __in LPTSTR	lptchDirPath,
									 __in LPTSTR	lpDirName,
									 __in BOOL		bWildcard
									 )
{
	BOOL			bRet = FALSE;

	TCHAR			tchDirExpression[MAX_PATH] = {0};
	HANDLE			hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA ffd = { 0 };
	TCHAR			tchPath[MAX_PATH] = {0};


	__try
	{
		if (!lptchDirPath || !lpDirName)
		{
			printfEx(MOD_DIRECTORY_CONTROL, PRINTF_LEVEL_ERROR, "input arguments error. lptchDirPath(%p) lpDirName(%p)",
				lptchDirPath, lpDirName);

			__leave;
		}

		if (!PathFileExists(lptchDirPath))
		{
			printfEx(MOD_DIRECTORY_CONTROL, PRINTF_LEVEL_WARNING, "not exist. (%S)",
				lptchDirPath);

			__leave;
		}

		StringCchPrintf(tchDirExpression, _countof(tchDirExpression), _T("%s\\*"), lptchDirPath);

		hFind = FindFirstFile(tchDirExpression, &ffd);
		if (INVALID_HANDLE_VALUE == hFind)
		{
			printfEx(MOD_DIRECTORY_CONTROL, PRINTF_LEVEL_ERROR, "FindFirstFile failed. (%S) (%d)",
				tchDirExpression, GetLastError());

			__leave;
		}

		do
		{
			StringCchPrintf(tchPath, _countof(tchPath), _T("%s\\%s"), lptchDirPath, ffd.cFileName);

			if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (0 != _tcsnicmp(ffd.cFileName, _T("."), wcslen(_T("."))) &&
					0 != _tcsnicmp(ffd.cFileName, _T(".."), wcslen(_T(".."))))
				{
					if (bWildcard)
					{
						if (NULL == StrRStrI(ffd.cFileName, NULL, lpDirName))
						{
							if (!DeleteInternalDir(tchPath, lpDirName, bWildcard))
								printfEx(MOD_DIRECTORY_CONTROL, PRINTF_LEVEL_WARNING, "DeleteInternalDir failed. (%S)",
								tchPath);
						}
						else
						{
							if (!Delete(tchPath))
								printfEx(MOD_DIRECTORY_CONTROL, PRINTF_LEVEL_WARNING, "Delete failed. (%S)",
								tchPath);
						}
					}
					else
					{
						if (0 != _tcsicmp(ffd.cFileName, lpDirName))
						{
							if (!DeleteInternalDir(tchPath, lpDirName, bWildcard))
								printfEx(MOD_DIRECTORY_CONTROL, PRINTF_LEVEL_WARNING, "DeleteInternalDir failed. (%S)",
								tchPath);
						}
						else
						{
							if (!Delete(tchPath))
								printfEx(MOD_DIRECTORY_CONTROL, PRINTF_LEVEL_WARNING, "Delete failed. (%S)",
								tchPath);
						}
					}
				}
			}
		} while (0 != FindNextFile(hFind, &ffd));

		if (ERROR_NO_MORE_FILES != GetLastError())
		{
			printfEx(MOD_DIRECTORY_CONTROL, PRINTF_LEVEL_ERROR, "FindNextFile failed. 2 (%S) (%d)",
				tchDirExpression, GetLastError());

			__leave;
		}

		bRet = TRUE;
	}
	__finally
	{
		if (INVALID_HANDLE_VALUE != hFind)
		{
			FindClose(hFind);
			hFind = INVALID_HANDLE_VALUE;
		}
	}

	return bRet;
}
