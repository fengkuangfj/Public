#include "StorageDeviceBusType.h"

BOOL
	CStorageDeviceBusType::Get(
	__in	LPTSTR				lpPath,
	__out	PSTORAGE_BUS_TYPE	pStorageBusType
	)
{
	BOOL						bRet					= FALSE;

	TCHAR                       tchPathSym[MAX_PATH]	= {0};
	TCHAR                       tchVolumeSym[MAX_PATH]  = {0};
	HANDLE                      hVolume                 = INVALID_HANDLE_VALUE;
	STORAGE_DEVICE_DESCRIPTOR   StorageDeviceDescriptor = {0};
	DWORD                       dwBytesReturned         = 0;

	STORAGE_PROPERTY_QUERY      StoragePropertyQuery;


	printfEx("begin");

	__try
	{
		ZeroMemory(&StoragePropertyQuery, sizeof(STORAGE_PROPERTY_QUERY));

		if (!lpPath || !pStorageBusType)
		{
			printfEx("input arguments error. 0x%08p 0x%08p", lpPath, pStorageBusType);
			__leave;
		}

		_tcscat_s(tchPathSym, _countof(tchPathSym), _T("\\\\.\\"));
		_tcscat_s(tchPathSym, _countof(tchPathSym), lpPath);

		CopyMemory(tchVolumeSym, tchPathSym, 5 * sizeof(TCHAR));
		_tcscat_s(tchVolumeSym, _countof(tchVolumeSym), _T(":"));

		hVolume = CreateFile(
			tchVolumeSym,
			SYNCHRONIZE,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL
			);
		if (INVALID_HANDLE_VALUE == hVolume)
		{
			printfEx("CreateFile failed. %S (%d)", tchVolumeSym, GetLastError());
			__leave;
		}

		StoragePropertyQuery.PropertyId = StorageDeviceProperty;
		StoragePropertyQuery.QueryType = PropertyStandardQuery;

		StorageDeviceDescriptor.Size = sizeof(STORAGE_DEVICE_DESCRIPTOR);

		bRet = DeviceIoControl(
			hVolume,
			IOCTL_STORAGE_QUERY_PROPERTY,
			&StoragePropertyQuery,
			sizeof(STORAGE_PROPERTY_QUERY),
			&StorageDeviceDescriptor,
			StorageDeviceDescriptor.Size,
			&dwBytesReturned,
			NULL
			);  
		if (!bRet)
		{
			printfEx("DeviceIoControl failed. (%d)", GetLastError());
			__leave;
		}

		*pStorageBusType = StorageDeviceDescriptor.BusType;
	}
	__finally
	{
		if (INVALID_HANDLE_VALUE != hVolume)
		{
			CloseHandle(hVolume);
			hVolume = INVALID_HANDLE_VALUE;
		}
	}

	printfEx("end");

	return bRet;
}
