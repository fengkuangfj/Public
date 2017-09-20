#include "Public.h"

#pragma warning(disable : 4127)

VOID
printfPublicEx(
			   __in LPSTR lpFile,
			   __in LPSTR lpFunction,
			   __in ULONG ulLine,
			   __in LPSTR lpFmt,
			   ...
			   )
{
	va_list	Args;

	time_t	rawTime = 0;
	tm		timeInfo = { 0 };
	CHAR	chFmtInfo[MAX_PATH] = { 0 };
	CHAR	chLog[MAX_PATH * 2] = { 0 };


	__try
	{
		va_start(Args, lpFmt);

		time(&rawTime);
		localtime_s(&timeInfo, &rawTime);

		StringCchVPrintfA(chFmtInfo, _countof(chFmtInfo), lpFmt, Args);

		StringCchPrintfA(chLog, _countof(chLog), "[%04d/%02d/%02d][%02d:%02d:%02d][%05d][%05d][%hs][%d][%hs] %hs \n",
			timeInfo.tm_year + 1900,
			timeInfo.tm_mon + 1,
			timeInfo.tm_mday,
			timeInfo.tm_hour,
			timeInfo.tm_min,
			timeInfo.tm_sec,
			GetCurrentProcessId(),
			GetCurrentThreadId(),
			lpFile,
			ulLine,
			lpFunction,
			chFmtInfo
			);

		printf("%hs", chLog);
		OutputDebugStringA(chLog);
	}
	__finally
	{
		va_end(Args);
	}
}

BOOL
GenGuid(
		__inout LPTSTR	lpGuid,
		__in	ULONG	ulBufSizeCh
		)
{  
	BOOL bRet = FALSE;

	GUID Guid = {0}; 


	do 
	{
		if (S_OK != CoCreateGuid(&Guid)) 
			break;

		StringCchPrintf(lpGuid, ulBufSizeCh,
			_T("{%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X}"),
			Guid.Data1,
			Guid.Data2, 
			Guid.Data3, 
			Guid.Data4[0], Guid.Data4[1],
			Guid.Data4[2], Guid.Data4[3], Guid.Data4[4], Guid.Data4[5], Guid.Data4[6], Guid.Data4[7]
		);

		bRet = TRUE;
	} while (FALSE);

	return bRet;  
}  
