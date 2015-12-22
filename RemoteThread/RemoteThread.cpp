#include "RemoteThread.h"

BOOL
	CRemoteThread::Inject(
	__in DWORD					dwPid,
	__in LPTHREAD_START_ROUTINE lpStartAddress,
	__in LPTHREAD_START_ROUTINE	lpStartAddressFollow,
	__in LPVOID					lpParameter,
	__in DWORD					dwParameterSizeB
	)
{
	BOOL		bRet						= FALSE;

	HANDLE		hProcess					= NULL;
	HANDLE		hThread						= NULL;
	LPVOID		lpFunctionAddress			= NULL;
	LPVOID		lpFunctionParameterAddress	= NULL;
	DWORD		dwFunctionSizeB				= 0;
	BYTE	*	pbyStartAddress				= NULL;
	BYTE	*	pbyStartAddressFollow		= NULL;


	__try
	{
		if (!dwPid || !lpStartAddress || !lpStartAddressFollow || !lpParameter || !dwParameterSizeB)
		{
			printfEx(MOD_REMOTE_THREAD, PRINTF_LEVEL_ERROR, "input arguments error. dwPid(%d) lpStartAddress(0x%p) lpStartAddressFollow(0x%p) lpParameter(%x%p) dwParameterSizeB(%d)",
				dwPid, lpStartAddress, lpStartAddressFollow, lpParameter, dwParameterSizeB);

			__leave;
		}

		hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
		if (!hProcess)
		{
			printfEx(MOD_REMOTE_THREAD, PRINTF_LEVEL_ERROR, "OpenProcess failed. (%d)", GetLastError());
			__leave;
		}

		//在目标进程内分配存放线程函数起始地址的内存
#ifdef _DEBUG
		pbyStartAddress = (BYTE *)lpStartAddress;		//DEBUG模式下编译器会有一个函数跳转表
		if (0xe9 == *pbyStartAddress)	//jmp的机器码为0xE9
		{
			pbyStartAddress++;	
			pbyStartAddress += *(int *)pbyStartAddress;				//当前地址+偏移地址
			pbyStartAddress += 4;				//此处内容为CC CC CC CC再加真实函数机器码,所以要跳过这4个0xCC,
		}
		lpStartAddress = (LPTHREAD_START_ROUTINE)pbyStartAddress;

		pbyStartAddressFollow = (BYTE *)lpStartAddressFollow;		//DEBUG模式下编译器会有一个函数跳转表
		if (0xe9 == *pbyStartAddressFollow)	//jmp的机器码为0xE9
		{
			pbyStartAddressFollow++;	
			pbyStartAddressFollow += *(int *)pbyStartAddressFollow;				//当前地址+偏移地址
			pbyStartAddressFollow += 4;				//此处内容为CC CC CC CC再加真实函数机器码,所以要跳过这4个0xCC,
		}
		lpStartAddressFollow = (LPTHREAD_START_ROUTINE)pbyStartAddressFollow;
#endif

		dwFunctionSizeB = ((BYTE *)(DWORD)lpStartAddressFollow - (BYTE *)(DWORD)lpStartAddress); 

		lpFunctionAddress = VirtualAllocEx(
			hProcess,
			NULL,
			dwFunctionSizeB,
			MEM_COMMIT,
			PAGE_READWRITE
			);
		if (!lpFunctionAddress)
		{
			printfEx(MOD_REMOTE_THREAD, PRINTF_LEVEL_ERROR, "VirtualAllocEx failed. (%d)", GetLastError());
			__leave;
		}

		if (!WriteProcessMemory(
			hProcess,
			lpFunctionAddress,
			(LPCVOID)lpStartAddress,
			dwFunctionSizeB,
			NULL
			))
		{
			printfEx(MOD_REMOTE_THREAD, PRINTF_LEVEL_ERROR, "WriteProcessMemory failed. (%d)", GetLastError());
			__leave;
		}

		lpFunctionParameterAddress = VirtualAllocEx(
			hProcess,
			NULL,
			dwParameterSizeB,
			MEM_COMMIT,
			PAGE_READWRITE
			);
		if (!lpFunctionParameterAddress)
		{
			printfEx(MOD_REMOTE_THREAD, PRINTF_LEVEL_ERROR, "VirtualAllocEx failed. (%d)", GetLastError());
			__leave;
		}

		if (!WriteProcessMemory(
			hProcess,
			lpFunctionParameterAddress,
			(LPCVOID)lpParameter,
			dwParameterSizeB,
			NULL
			))
		{
			printfEx(MOD_REMOTE_THREAD, PRINTF_LEVEL_ERROR, "WriteProcessMemory failed. (%d)", GetLastError());
			__leave;
		}

		hThread = CreateRemoteThread(
			hProcess,
			NULL,
			0,
			(LPTHREAD_START_ROUTINE)lpFunctionAddress,
			lpFunctionParameterAddress,
			0,
			NULL
			);
		if (!hThread)
		{
			printfEx(MOD_REMOTE_THREAD, PRINTF_LEVEL_ERROR, "CreateRemoteThread failed. (%d)", GetLastError());
			__leave;
		}

		bRet = TRUE;
	}
	__finally
	{
		if (lpFunctionAddress)
		{
			VirtualFreeEx(hProcess, lpFunctionAddress, sizeof(DWORD), MEM_RELEASE);
			lpFunctionAddress = NULL;
		}

		if (lpFunctionParameterAddress)
		{
			VirtualFreeEx(hProcess, lpFunctionParameterAddress, sizeof(DWORD), MEM_RELEASE);
			lpFunctionParameterAddress = NULL;
		}

		if (hProcess)
		{
			CloseHandle(hProcess);
			hProcess = NULL;
		}
	}

	return bRet;
}
