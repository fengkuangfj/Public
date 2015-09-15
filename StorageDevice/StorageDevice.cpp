#include "StorageDevice.h"

BOOL
	CStorageDevice::GetBusType(
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


	__try
	{
		ZeroMemory(&StoragePropertyQuery, sizeof(STORAGE_PROPERTY_QUERY));

		if (!lpPath || !pStorageBusType)
		{
			printfEx(MOD_STORAGE_DEVICE, PRINTF_LEVEL_ERROR, "input arguments error. 0x%08p 0x%08p", lpPath, pStorageBusType);
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
			printfEx(MOD_STORAGE_DEVICE, PRINTF_LEVEL_ERROR, "CreateFile failed. %S (%d)", tchVolumeSym, GetLastError());
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
			printfEx(MOD_STORAGE_DEVICE, PRINTF_LEVEL_ERROR, "DeviceIoControl failed. (%d)", GetLastError());
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

	return bRet;
}

BOOL
	CStorageDevice::GetVolumePhysicalLlocation(
	__in	LPTSTR lpPath,
	__out	PULONG pDiskNumber
	)
{
	BOOL				bRet					= FALSE;

	TCHAR				tchPathSym[MAX_PATH]	= {0};
	TCHAR				tchVolumeSym[MAX_PATH]  = {0};
	HANDLE				hVolume                 = INVALID_HANDLE_VALUE;
	VOLUME_DISK_EXTENTS	VolumeDiskExtents		= {0};
	DWORD				dwBytesReturned         = 0;


	__try
	{
		if (!lpPath || !pDiskNumber)
		{
			printfEx(MOD_STORAGE_DEVICE, PRINTF_LEVEL_ERROR, "input arguments error. 0x%08p 0x%08p", lpPath, pDiskNumber);
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
			printfEx(MOD_STORAGE_DEVICE, PRINTF_LEVEL_ERROR, "CreateFile failed. %S (%d)", tchVolumeSym, GetLastError());
			__leave;
		}

		bRet = DeviceIoControl(
			hVolume,
			IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS,
			NULL,
			0,
			&VolumeDiskExtents,
			sizeof(VOLUME_DISK_EXTENTS),
			&dwBytesReturned,
			NULL
			);  
		if (!bRet)
		{
			printfEx(MOD_STORAGE_DEVICE, PRINTF_LEVEL_ERROR, "DeviceIoControl failed. (%d)", GetLastError());
			__leave;
		}

		*pDiskNumber = VolumeDiskExtents.Extents->DiskNumber;
	}
	__finally
	{
		if (INVALID_HANDLE_VALUE != hVolume)
		{
			CloseHandle(hVolume);
			hVolume = INVALID_HANDLE_VALUE;
		}
	}

	return bRet;
}

BOOL
	CStorageDevice::QueryCaption(
	__in	LPTSTR	lpInBuf,
	__out	LPTSTR	lpOutBuf,
	__in	ULONG	ulOutBufSizeCh
	)
{
	BOOL bRet = FALSE;

	ULONG ulDiskNumber = 0;


	__try
	{
		if (!lpInBuf || !lpOutBuf || !ulOutBufSizeCh)
		{
			printfEx(MOD_STORAGE_DEVICE, PRINTF_LEVEL_ERROR, "input arguments error. 0x%08p 0x%08p %d", lpInBuf, lpOutBuf, ulOutBufSizeCh);
			__leave;
		}

		if (!GetVolumePhysicalLlocation(lpInBuf, &ulDiskNumber))
		{
			printfEx(MOD_STORAGE_DEVICE, PRINTF_LEVEL_ERROR, "GetVolumePhysicalLlocation failed");
			__leave;
		}

		if (!CWmi::QueryCaption(ulDiskNumber, lpOutBuf, ulOutBufSizeCh))
		{
			printfEx(MOD_STORAGE_DEVICE, PRINTF_LEVEL_ERROR, "CWmi::QueryCaption failed");
			__leave;
		}

		bRet = TRUE;
	}
	__finally
	{
		;
	}

	return bRet;
}
