#pragma once

#include <Windows.h>
#include <tchar.h>

#include "..\\Public.h"

#ifndef MOD_UAC
#define MOD_UAC _T("UAC")
#endif

typedef enum _UAC_LEVEL
{
	UAC_LEVEL_1,
	UAC_LEVEL_2,
	UAC_LEVEL_3,
	UAC_LEVEL_4
} UAC_LEVEL, *PUAC_LEVEL, *LPUAC_LEVEL;

class CUac
{
public:
	static
		BOOL
		Set(
		__in UAC_LEVEL UacLevel
		);

private:
	CUac();

	~CUac();
};
