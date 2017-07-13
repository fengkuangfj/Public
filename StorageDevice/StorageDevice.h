#pragma once

#include <Windows.h>
#include <tchar.h>
#include <WinIoCtl.h>

#include "..\\PrintfEx\\PrintfEx.h"
#include "..\\Wmi\\Wmi.h"

#ifndef MOD_STORAGE_DEVICE
#define MOD_STORAGE_DEVICE _T("¥Ê¥¢…Ë±∏")
#endif

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
		GetVolumePhysicalLocation(
		__in	LPTSTR lpPath,
		__out	PULONG pDiskNumber
		);

	static
		BOOL
		QueryCaption(
		__in	LPTSTR	lpInBuf,
		__out	LPTSTR	lpOutBuf,
		__in	ULONG	ulOutBufSizeCh
		);

private:
	CStorageDevice();

	~CStorageDevice();
};
