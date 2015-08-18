#include "PrintfEx.h"

VOID
	CPrintfEx::PrintfInternal(
	__in LPTSTR			lpMod,
	__in PRINTF_LEVEL	PrintfLevel,
	__in LPSTR			pFuncName,
	__in LPSTR			Fmt,
	...
	)
{
	time_t	rawTime				= 0;
	tm		timeInfo			= {0};
	CHAR	chFmtInfo[MAX_PATH]	= {0};

	va_list Args;


	__try
	{
		va_start(Args, Fmt);

		setlocale(LC_ALL, "");

		// ʱ��
		time(&rawTime);
		localtime_s(&timeInfo, &rawTime);

		StringCchVPrintfA(chFmtInfo, _countof(chFmtInfo), Fmt, Args);

		// ʱ�� �߳� ģ�� ���� ��Ϣ
		printf("[%04d/%02d/%02d][%02d:%02d:%02d][%05d][%lS][%hs] %hs \n", 
			timeInfo.tm_year + 1900,
			timeInfo.tm_mon + 1,
			timeInfo.tm_mday,
			timeInfo.tm_hour,
			timeInfo.tm_min,
			timeInfo.tm_sec,
			GetCurrentThreadId(),
			lpMod ? lpMod : _T("δָ��"),
			pFuncName,
			chFmtInfo
			);
	}
	__finally
	{
		va_end(Args);
	}
}
