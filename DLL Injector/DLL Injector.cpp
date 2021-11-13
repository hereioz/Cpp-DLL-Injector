#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>

DWORD GetProcId(const char* procName)
{
	DWORD procId = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof(procEntry);

		if (Process32First(hSnap, &procEntry))
		{
			do
			{
				if (!_stricmp(procEntry.szExeFile, procName))
				{
					procId = procEntry.th32ProcessID;
					break;
				}
			} while (Process32Next(hSnap, &procEntry));
		}
	}
	CloseHandle(hSnap);
	return procId;

}


int main()
{
	char DLLPATH[999] = "";
	char PROCNAME[999] = "";

	std::cout << "Enter DLL Path: ";
	std::cin >> DLLPATH;
	std::cout << "Enter Game.exe Name: ";
	std::cin >> PROCNAME;

	const char* dllPath = (char*)DLLPATH;
	const char* procName = (char*)PROCNAME;

	system("cls");
	std::cout << "Injecting: " << procName << ", With: " << dllPath << std::endl;

	DWORD procId = 0;

	while (!procId)
	{
		procId = GetProcId(procName);
		Sleep(30);
	}

	HANDLE hProc;

	try
	{
		hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, procId);
		throw (hProc);
	}
	catch (const std::exception& e)
	{
		std::cout << "Failed to OpenProcess to " << procName << std::endl;
		Sleep(3000);
		exit(-1);
	}

	if (hProc && hProc != INVALID_HANDLE_VALUE)
	{
		void* loc;

		try
		{
			loc = VirtualAllocEx(hProc, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		}
		catch (const std::exception& e)
		{
			std::cout << "Failed to locate " << procName << " Memory" << std::endl;
			Sleep(3000);
			exit(-1);
		}

		if (loc)
		{
			try
			{
				WriteProcessMemory(hProc, loc, dllPath, strlen(dllPath) + 1, 0);
			}
			catch (const std::exception& e)
			{
				std::cout << "Injecting Failed to " << procName << ", With: " << dllPath << std::endl;
				Sleep(3000);
				exit(-1);
			}
		}

		HANDLE hThread;

		try
		{
			HANDLE hThread = CreateRemoteThread(hProc, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, loc, 0, 0);
		}
		catch (const std::exception& e)
		{
			std::cout << "Failed to Create Remote Thread to " << procName << std::endl;
			Sleep(3000);
			exit(-1);
		}

		if (hThread)
		{
			CloseHandle(hThread);
		}
		std::cout << "Injecting Sucessfully to " << procName << ", With: " << dllPath << std::endl;
	}
	else
	{
		std::cout << procName << " Not Found ): " << std::endl;
	}
	if (hProc)
	{
		CloseHandle(hProc);
	}

	char pauser;
	std::cin >> pauser;

	return 0;
}
