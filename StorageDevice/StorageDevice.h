#pragma once

#include <Windows.h>
#include <tchar.h>

#include "..\\PrintfEx\\PrintfEx.h"

#define MOD_STORAGE_DEVICE _T("¥Ê¥¢…Ë±∏")

typedef struct _MEDIA_SERIAL_NUMBER_DATA
{
	ULONG SerialNumberLength;
	ULONG Result;
	ULONG Reserved[2];
	UCHAR SerialNumberData[1];
} MEDIA_SERIAL_NUMBER_DATA, *PMEDIA_SERIAL_NUMBER_DATA;

class CStorageDevice
{
public:
	static
		BOOL
		GetBusType(  
		__in	LPTSTR				lpPath,
		__out	PSTORAGE_BUS_TYPE	pStorageBusType
		);

	static
		BOOL
		GetVolumePhysicalLlocation(  
		__in LPTSTR lpPath
		);
};
