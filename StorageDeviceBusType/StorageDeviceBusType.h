#pragma once

#include <Windows.h>
#include <tchar.h>

#include "../PrintfEx/PrintfEx.h"

#define MOD_GET_STORAGE_DEVICE_BUS_TYPE _T("存储设备总线类型")

class CStorageDeviceBusType
{
public:
	static
		BOOL
		Get(  
		__in	LPTSTR				lpPath,
		__out	PSTORAGE_BUS_TYPE	pStorageBusType
		);
};
