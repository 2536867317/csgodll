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
	LPVOID pRemoteBuf = NULL;  // �洢��Ŀ�����������ڴ��ַ  
	DWORD dwBufSize = (DWORD)(_tcslen(lpszfile) + 1) * sizeof(TCHAR);  // �洢DLL�ļ�·��������ڴ�ռ��С  
	LPTHREAD_START_ROUTINE pThreadProc;

	if (!(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID))) {
		_tprintf(L"OpenProcess(%d) failed!!! [%d]\n", dwPID, GetLastError());
		return FALSE;
	}

	pRemoteBuf = VirtualAllocEx(hProcess, NULL, dwBufSize, MEM_COMMIT, PAGE_READWRITE);  // ��Ŀ����̿ռ��������ڴ�  

	WriteProcessMemory(hProcess, pRemoteBuf, (LPVOID)lpszfile, dwBufSize, NULL);  // ����Ŀ�����������ڴ�ռ���д��DLL�ļ���·��  

	hMod = GetModuleHandle(L"kernel32.dll");
	pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(hMod, "LoadLibraryW");  // ���LoadLibrary()�����ĵ�ַ  

	hThread = CreateRemoteThread(hProcess, NULL, 0, pThreadProc, pRemoteBuf, 0, NULL);

	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	CloseHandle(hProcess);

	printf("ע�����");
	Sleep(1000);

	return 0;
}