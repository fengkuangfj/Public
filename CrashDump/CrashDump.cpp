#include "CrashDump.h"

BOOL
	CCrashDump::SetType(
	__in CRASH_DUMP_TYPE CrashDumpType
	)
{
	BOOL	bRet	= FALSE;

	LONG	lResult = ERROR_SUCCESS;
	HKEY	hKey	= NULL;
	DWORD	dwValue = 0;


	__try
	{

		switch (CrashDumpType)
		{
		case CRASH_DUMP_TYPE_NULL:
		case CRASH_DUMP_TYPE_FULL:
		case CRASH_DUMP_TYPE_MINI:
		case CRASH_DUMP_TYPE_AUTO:
			__leave;
		case CRASH_DUMP_TYPE_MAIN:
			{
				// 打开注册表\\Machine\\System\\CurrentControlSet\\Control\\CrashControl
				lResult = RegOpenKeyEx(
					HKEY_LOCAL_MACHINE,
					_T("System\\CurrentControlSet\\Control\\CrashControl"),
					0,
					KEY_ALL_ACCESS,
					&hKey
					);
				if (ERROR_SUCCESS != lResult)
					__leave;

				// 将CrashDumpEnabled的值置为2(核心内存转储)
				dwValue = 2;
				lResult = RegSetValueEx(
					hKey,
					_T("CrashDumpEnabled"),
					NULL,
					REG_DWORD,
					(const BYTE *)&dwValue,
					sizeof(DWORD)
					);
				if (ERROR_SUCCESS != lResult)
					__leave;

				// 将OverWrite的值置为1
				dwValue = 1;
				lResult = RegSetValueEx(
					hKey,
					_T("OverWrite"),
					NULL,
					REG_DWORD,
					(const BYTE *)&dwValue,
					sizeof(DWORD)
					);
				if (ERROR_SUCCESS != lResult)
					__leave;

				// 使注册表立即写入文件
				lResult = RegFlushKey(hKey);
				if (ERROR_SUCCESS != lResult)
					__leave;

				break;
			}
		default:
			__leave;
		}

		bRet = TRUE;
	}
	__finally
	{
		if (hKey)
		{
			RegCloseKey(hKey);
			hKey = NULL;
		}
	}

	return bRet;
}

FILE_MODIFY_TIME_CMP
	CCrashDump::EqualModifyTime(
	__in LPTSTR lpFrirst,
	__in LPTSTR lpSecend
	)
{
	FILE_MODIFY_TIME_CMP	ret					= FILE_MODIFY_TIME_CMP_NULL;

	WIN32_FIND_DATA			Win32FindDataFirst	= {0};
	WIN32_FIND_DATA			Win32FindDataSecend	= {0};
	HANDLE					hFindFirst			= INVALID_HANDLE_VALUE;
	HANDLE					hFindSecend			= INVALID_HANDLE_VALUE;


	__try
	{
		if (!lpFrirst || !lpSecend)
			__leave;

		if (!PathFileExists(lpFrirst))
			__leave;

		if (!PathFileExists(lpSecend))
			__leave;

		hFindFirst = FindFirstFile(lpFrirst, &Win32FindDataFirst);
		if (INVALID_HANDLE_VALUE == hFindFirst)
			__leave;

		hFindSecend = FindFirstFile(lpSecend, &Win32FindDataSecend);
		if (INVALID_HANDLE_VALUE == hFindSecend)
			__leave;

		if (Win32FindDataFirst.ftLastWriteTime.dwHighDateTime < Win32FindDataSecend.ftLastWriteTime.dwHighDateTime)
			ret = FILE_MODIFY_TIME_CMP_EARLIER;
		else if (Win32FindDataFirst.ftLastWriteTime.dwHighDateTime > Win32FindDataSecend.ftLastWriteTime.dwHighDateTime)
			ret = FILE_MODIFY_TIME_CMP_LATER;
		else
			ret = FILE_MODIFY_TIME_CMP_EQUAL;
	}
	__finally
	{
		if (INVALID_HANDLE_VALUE != hFindFirst)
		{
			FindClose(hFindFirst);
			hFindFirst = INVALID_HANDLE_VALUE;
		}

		if (INVALID_HANDLE_VALUE != hFindSecend)
		{
			FindClose(hFindSecend);
			hFindSecend = INVALID_HANDLE_VALUE;
		}
	}

	return ret;
}

BOOL
	CCrashDump::GenerateDumpFilePath(
	__in	LPTSTR	lpDumpDir,
	__in	LPTSTR	lpOrgDumpPath,
	__inout LPTSTR	lpNewDumpPath
	)
{
	BOOL			bRet			= FALSE;

	SYSTEMTIME		LocalSystemTime	= {0};
	TCHAR			Tmp[MAX_PATH]	= {0};
	WIN32_FIND_DATA	Win32FindData	= {0};
	FILETIME		LocalFileTime	= {0};
	HANDLE			hFind			= INVALID_HANDLE_VALUE;


	__try
	{
		if (!lpDumpDir || !lpOrgDumpPath || !lpNewDumpPath)
			__leave;

		if (!PathFileExists(lpOrgDumpPath))
			__leave;

		hFind = FindFirstFile(lpOrgDumpPath, &Win32FindData);
		if (INVALID_HANDLE_VALUE == hFind)
			__leave;

		_tcscat_s(lpNewDumpPath, MAX_PATH, lpDumpDir);

		if (!FileTimeToLocalFileTime(&Win32FindData.ftLastWriteTime, &LocalFileTime))
			__leave;

		if (!FileTimeToSystemTime(&LocalFileTime, &LocalSystemTime))
			__leave;

		StringCbPrintf(Tmp, MAX_PATH * sizeof(TCHAR), _T("\\%04d"), LocalSystemTime.wYear);
		_tcscat_s(lpNewDumpPath, MAX_PATH, Tmp);

		StringCbPrintf(Tmp, MAX_PATH * sizeof(TCHAR), _T("-%02d"), LocalSystemTime.wMonth);
		_tcscat_s(lpNewDumpPath, MAX_PATH, Tmp);

		StringCbPrintf(Tmp, MAX_PATH * sizeof(TCHAR), _T("-%02d"), LocalSystemTime.wDay);
		_tcscat_s(lpNewDumpPath, MAX_PATH, Tmp);

		StringCbPrintf(Tmp, MAX_PATH * sizeof(TCHAR), _T("__%02d"), LocalSystemTime.wHour);
		_tcscat_s(lpNewDumpPath, MAX_PATH, Tmp);

		StringCbPrintf(Tmp, MAX_PATH * sizeof(TCHAR), _T("-%02d"), LocalSystemTime.wMinute);
		_tcscat_s(lpNewDumpPath, MAX_PATH, Tmp);

		StringCbPrintf(Tmp, MAX_PATH * sizeof(TCHAR), _T("-%02d"), LocalSystemTime.wSecond);
		_tcscat_s(lpNewDumpPath, MAX_PATH, Tmp);

		StringCbPrintf(Tmp, MAX_PATH * sizeof(TCHAR), _T("__%03d"), LocalSystemTime.wMilliseconds);
		_tcscat_s(lpNewDumpPath, MAX_PATH, Tmp);

		_tcscat_s(lpNewDumpPath, MAX_PATH, _T(".dmp"));

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
	CCrashDump::Store(
	__in LPTSTR lpDumpDir,
	__in LPTSTR lpDriverPath
	)
{
	BOOL					bRet						= FALSE;

	LONG					lResult						= ERROR_SUCCESS;
	HKEY					hKey						= NULL;
	LPVOID					lpBuf						= NULL;
	DWORD					dwQuery						= 0;
	TCHAR					tchSystemRoot[MAX_PATH]		= {0};
	TCHAR					tchDumpFile[MAX_PATH]		= {0};
	TCHAR					tchNewDumpFile[MAX_PATH]	= {0};
	FILE_MODIFY_TIME_CMP	FileModifyTimeCmp			= FILE_MODIFY_TIME_CMP_NULL;
	LPTSTR					lpPosition					= NULL;


	__try
	{
		lResult = RegOpenKeyEx(
			HKEY_LOCAL_MACHINE,
			_T("System\\CurrentControlSet\\Control\\CrashControl"),
			0,
			KEY_ALL_ACCESS,
			&hKey
			);
		if (ERROR_SUCCESS != lResult)
			__leave;

		dwQuery = 1;
		do
		{
			lpBuf = calloc(1, dwQuery);
			if (!lpBuf)
				__leave;

			lResult = RegQueryValueEx(
				hKey,
				_T("DumpFile"),
				NULL,
				NULL,
				(LPBYTE)lpBuf,
				&dwQuery
				);
			if (ERROR_SUCCESS != lResult)
			{
				if (ERROR_MORE_DATA != lResult)
					__leave;

				if (lpBuf)
				{
					free(lpBuf);
					lpBuf = NULL;
				}
			}
			else
				break;
		} while (TRUE);

		// 启动时判断DumpFile是否存在
		if (0 == _tcsncmp((LPTSTR)lpBuf, _T("%SystemRoot%"), _tcslen(_T("%SystemRoot%"))))
		{
			if (!GetSystemWindowsDirectory(tchDumpFile, _countof(tchDumpFile)))
				__leave;

			_tcscat_s(tchDumpFile, _countof(tchDumpFile), (LPTSTR)lpBuf + _tcslen(_T("%SystemRoot%")));
		}
		else
			_tcscat_s(tchDumpFile, _countof(tchDumpFile), (LPTSTR)lpBuf);

		if (!PathFileExists((LPTSTR)tchDumpFile))
		{
			// 不存在，不处理
			__leave;
		}

		FileModifyTimeCmp = EqualModifyTime(tchDumpFile, lpDriverPath);
		if (FILE_MODIFY_TIME_CMP_NULL == FileModifyTimeCmp)
			__leave;

		if (FILE_MODIFY_TIME_CMP_EARLIER == FileModifyTimeCmp)
		{
			// 比我们的软件的安装时间早，重命名
			_tcscat_s(tchNewDumpFile, _countof(tchNewDumpFile), tchDumpFile);
			if (PathFileExists(tchNewDumpFile))
			{
				lpPosition = _tcsrchr(tchNewDumpFile, _T('\\'));
				if (!lpPosition)
					__leave;

				if (!PathYetAnotherMakeUniqueName(tchNewDumpFile, tchNewDumpFile, NULL, lpPosition + 1))
					__leave;
			}

			if (!MoveFile(tchDumpFile, tchNewDumpFile))
				__leave;

			__leave;
		}

		// 比我们的软件的安装时间晚，先复制至安装目录下，再重命名
		if (!GenerateDumpFilePath(lpDumpDir, tchDumpFile, tchNewDumpFile))
			__leave;

		if (!CopyFile(tchDumpFile, tchNewDumpFile, TRUE))
			__leave;

		_tcscpy_s(tchNewDumpFile, _countof(tchNewDumpFile), tchDumpFile);
		if (PathFileExists(tchNewDumpFile))
		{
			lpPosition = _tcsrchr(tchNewDumpFile, _T('\\'));
			if (!lpPosition)
				__leave;

			if (!PathYetAnotherMakeUniqueName(tchNewDumpFile, tchNewDumpFile, NULL, lpPosition + 1))
				__leave;
		}

		if (!MoveFile(tchDumpFile, tchNewDumpFile))
			__leave;

		bRet = TRUE;
	}
	__finally
	{
		if (lpBuf)
		{
			free(lpBuf);
			lpBuf = NULL;
		}

		if (hKey)
		{
			RegCloseKey(hKey);
			hKey = NULL;
		}
	}

	return bRet;
}
