#include "Public.h"

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
