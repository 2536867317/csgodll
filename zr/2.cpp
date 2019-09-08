#include <windows.h>  
#include <tchar.h>  
#include <stdio.h>
#include <stdlib.h>

int main() {
	const char* pf = "D:\\C\\zr\\Debug\\skin.dll";
	int nlen = strlen(pf) + 1;
	int nwlen = MultiByteToWideChar(CP_ACP, 0, pf, nlen, NULL, 0);
	TCHAR lpszfile[256];
	MultiByteToWideChar(CP_ACP, 0, pf, nlen, lpszfile, nwlen);

	DWORD dwPID;
	HWND i = FindWindowA(NULL, "Counter-Strike: Global Offensive");

	GetWindowThreadProcessId(i, &dwPID);

	//printf("%d", pid);

	HANDLE hProcess = NULL;
	HANDLE hThread = NULL;
	HMODULE hMod = NULL;
	LPVOID pRemoteBuf = NULL;  // 存储在目标进程申请的内存地址  
	DWORD dwBufSize = (DWORD)(_tcslen(lpszfile) + 1) * sizeof(TCHAR);  // 存储DLL文件路径所需的内存空间大小  
	LPTHREAD_START_ROUTINE pThreadProc;

	if (!(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID))) {
		_tprintf(L"OpenProcess(%d) failed!!! [%d]\n", dwPID, GetLastError());
		return FALSE;
	}

	pRemoteBuf = VirtualAllocEx(hProcess, NULL, dwBufSize, MEM_COMMIT, PAGE_READWRITE);  // 在目标进程空间中申请内存  

	WriteProcessMemory(hProcess, pRemoteBuf, (LPVOID)lpszfile, dwBufSize, NULL);  // 向在目标进程申请的内存空间中写入DLL文件的路径  

	hMod = GetModuleHandle(L"kernel32.dll");
	pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(hMod, "LoadLibraryW");  // 获得LoadLibrary()函数的地址  

	hThread = CreateRemoteThread(hProcess, NULL, 0, pThreadProc, pRemoteBuf, 0, NULL);

	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	CloseHandle(hProcess);

	printf("注入完成");
	Sleep(1000);

	return 0;
}