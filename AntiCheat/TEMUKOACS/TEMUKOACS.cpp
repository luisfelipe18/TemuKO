#include "stdafx.h"
#include "PearlGui.h"
#include "Splash.h"
#include "HDRReader.h"
#define CREATE_THREAD_ACCESS (PROCESS_TERMINATE | PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ)

TEMUKOACSEngine* Engine = NULL;
HDRReader* hdrReader = NULL;
std::string basePath = "";
void Shutdown(string message = "");

extern bool HideThread(HANDLE hThread);

uint8 g_bRunning = 0;
void SetMemBYTE1(DWORD Addr, BYTE Value)
{
	HANDLE KO_HANDLE = OpenProcess(CREATE_THREAD_ACCESS, FALSE, GetCurrentProcessId());
	WriteProcessMemory(KO_HANDLE, (LPVOID)(Addr), &Value, 1, NULL);
	CloseHandle(KO_HANDLE);
}

inline void SetMemString(DWORD Adres, char* Deger, DWORD size)
{
	char send_Buffix[20]; memset(send_Buffix, 0x00, 20);

	for (int i = 0; i < size; i++)
		SetMemBYTE1(Adres + i, (BYTE)Deger[i]);

	SetMemBYTE1(Adres + size, 0x00);
}

__inline DWORD GetMemDWORD(DWORD ulBase)
{
	if (!IsBadReadPtr((VOID*)ulBase, sizeof(DWORD)))
		return(*(DWORD*)(ulBase));

	return 0;
}

__inline DWORD GetMemBYTE(DWORD ulBase)
{
	if (!IsBadReadPtr((VOID*)ulBase, sizeof(BYTE)))
		return(*(BYTE*)(ulBase));

	return 0;
}

DWORD CalculateCallAddr(DWORD Addr)
{
	return GetMemDWORD(Addr) + Addr + 5;
}

bool _fexists(std::string& filename) {
	std::ifstream ifile(filename.c_str());
	return (bool)ifile;
}

std::string getexepath()
{
	char result[MAX_PATH];
	string ret = std::string(result, GetModuleFileName(NULL, result, MAX_PATH));
	return ret.substr(0, ret.find_last_of("\\"));
}

DWORD GetFileSize(const char* FileName)
{
	std::ifstream in(FileName, std::ifstream::ate | std::ifstream::binary);
	return in.tellg();
}

void Shutdown(string message)
{
	Engine->power = false;
	string s1 = xorstr("The file is corrupted. Please contact with administrator.\n");
	string s2 = xorstr("If you constantly see this message, ");
	string s3 = xorstr("disable the anti-virus program.");

	if (message == "")
		MessageBoxA(NULL, string(s1+s2+s3).c_str(), xorstr("KnightOnLine.exe"), MB_OK | MB_ICONEXCLAMATION);
	else
		MessageBoxA(NULL, message.c_str(), xorstr("KnightOnLine.exe"), MB_OK | MB_ICONEXCLAMATION);

	exit(0);
	FreeLibrary(GetModuleHandle(NULL));
	TerminateProcess(GetCurrentProcess(), 0);
}

int DeleteDirectory(const std::string& refcstrRootDirectory,
	bool              bDeleteSubdirectories = true)
{
	bool            bSubdirectory = false;       // Flag, indicating whether
												 // subdirectories have been found
	HANDLE          hFile;                       // Handle to directory
	std::string     strFilePath;                 // Filepath
	std::string     strPattern;                  // Pattern
	WIN32_FIND_DATA FileInformation;             // File information


	strPattern = refcstrRootDirectory + "\\*.*";
	hFile = ::FindFirstFile(strPattern.c_str(), &FileInformation);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (FileInformation.cFileName[0] != '.')
			{
				strFilePath.erase();
				strFilePath = refcstrRootDirectory + "\\" + FileInformation.cFileName;

				if (FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					if (bDeleteSubdirectories)
					{
						// Delete subdirectory
						int iRC = DeleteDirectory(strFilePath, bDeleteSubdirectories);
						if (iRC)
							return iRC;
					}
					else
						bSubdirectory = true;
				}
				else
				{
					// Set file attributes
					if (::SetFileAttributes(strFilePath.c_str(),
						FILE_ATTRIBUTE_NORMAL) == FALSE)
						return ::GetLastError();

					// Delete file
					if (::DeleteFile(strFilePath.c_str()) == FALSE)
						return ::GetLastError();
				}
			}
		} while (::FindNextFile(hFile, &FileInformation) == TRUE);

		// Close handle
		::FindClose(hFile);

		DWORD dwError = ::GetLastError();
		if (dwError != ERROR_NO_MORE_FILES)
			return dwError;
		else
		{
			if (!bSubdirectory)
			{
				// Set directory attributes
				if (::SetFileAttributes(refcstrRootDirectory.c_str(),
					FILE_ATTRIBUTE_NORMAL) == FALSE)
					return ::GetLastError();

				// Delete directory
				if (::RemoveDirectory(refcstrRootDirectory.c_str()) == FALSE)
					return ::GetLastError();
			}
		}
	}

	return 0;
}

extern  "C"  __declspec(dllexport) void __cdecl Init() {
	VIRTUALIZER_START
		/*AllocConsole();
		freopen(xorstr("CONOUT$"), xorstr("w"), stdout);
		/* <multiclient check> */
		CreateMutexA(0, FALSE, "Local\\$TEMUKOACS$");
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			Shutdown();
			return;
		}
		/* </multiclient check> */
		basePath = getexepath();
		string myname(basePath + xorstr("\\KnightOnLine.exe"));
		if (!_fexists(myname)) {
			Shutdown();
			return;
		}
		int	iRC = 0;
		std::string strDirectoryToDelete(basePath + xorstr("\\Info"));
		iRC = DeleteDirectory(strDirectoryToDelete, false);
		if (iRC){
			//std::cout << "Error " << iRC << std::endl;
			printf("Error %u\n", iRC);
		}
		// Delete 'c:\mydir' and its subdirectories
		iRC = DeleteDirectory(strDirectoryToDelete);
		if (iRC){
			//std::cout << "Error " << iRC << std::endl;
			printf("Error %u\n", iRC);
		}
		if (GetFileSize(string(basePath + xorstr("\\KnightOnLine.exe")).c_str()) != 0xB5FA00)
		{
			printf("0x%X\n", GetFileSize(string(basePath + "\\KnightOnLine.exe").c_str()));
			Shutdown();
			return;
		}
		myname = basePath + xorstr("\\TEMUKOACS.dll");
		if (!_fexists(myname)) {
			Shutdown();
			return;
		}
		char nm[100];
		GetModuleBaseNameA(GetCurrentProcess(), GetModuleHandle(NULL), nm, sizeof(nm));
		string _nm(nm);
		if (_nm != xorstr("KnightOnLine.exe")) {
			Shutdown();
			return;
		}
		Engine = new TEMUKOACSEngine(basePath + xorstr("\\"));
		hdrReader = new HDRReader(basePath + xorstr("\\"));
		UIMain();
	VIRTUALIZER_END
}

 BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpvReserved)
{
	 if (dwReason == DLL_PROCESS_ATTACH) 
	 {
		 DisableThreadLibraryCalls(hModule);
		 Init();
		 HideThread(GetCurrentThread());
		 Engine->HideModule(hModule);
		 Engine->AntiHeaders(hModule);
		 Engine->AntiRevers(hModule);
		 Engine->HideModuleFromPEB(hModule);
		 Engine->CloakDll(hModule);
		 Engine->Checking(hModule);
		 Engine->RemovePeHeader((DWORD)hModule);
		 Engine->EraseHeaders(hModule);
		 Engine->ScanCheck();

		 HANDLE hProcess = 0;
		 hProcess = GetCurrentProcess();

		 // 2228
		 BYTE G_private_key2245[] = { 0x68,0x85,0x45,0x54,0x75 };//Jvcrypton	
		 BYTE G_private_keyop2245[] = { 0x68,0x49,0x85,0x65,0x12 };//Crypton

		 WriteProcessMemory(hProcess, (void*)0x497BB0, (void*)&G_private_key2245, sizeof(G_private_key2245), NULL);
		 WriteProcessMemory(hProcess, (void*)0x49893E, (void*)&G_private_key2245, sizeof(G_private_key2245), NULL);
		 WriteProcessMemory(hProcess, (void*)0x4BE49F, (void*)&G_private_key2245, sizeof(G_private_key2245), NULL);

		 WriteProcessMemory(hProcess, (void*)0x4BE4A4, (void*)&G_private_keyop2245, sizeof(G_private_keyop2245), NULL);
		 WriteProcessMemory(hProcess, (void*)0x498943, (void*)&G_private_keyop2245, sizeof(G_private_keyop2245), NULL);
		 WriteProcessMemory(hProcess, (void*)0x497BB5, (void*)&G_private_keyop2245, sizeof(G_private_keyop2245), NULL);

		 // 2228
		 BYTE G_GameExePort_2239[] = { 0x68,0x99,0x3A };// Game Port	15001
		 WriteProcessMemory(hProcess, (void*)0x4C66FD, (void*)&G_GameExePort_2239, sizeof(G_GameExePort_2239), NULL);
		 WriteProcessMemory(hProcess, (void*)0x7E4B62, (void*)&G_GameExePort_2239, sizeof(G_GameExePort_2239), NULL);
		 WriteProcessMemory(hProcess, (void*)0x7E3F88, (void*)&G_GameExePort_2239, sizeof(G_GameExePort_2239), NULL);

		 // VERSİON
		 BYTE G_GameExeV1[] = { 0x81, 0x7C, 0x24, 0x14, 0xBF, 0x08 };
		 WriteProcessMemory(hProcess, (void*)0x4BE527, (void*)&G_GameExeV1, sizeof(G_GameExeV1), NULL);

		 // VERSİON
		 BYTE G_GameExeV2[] = { 0xB8, 0xBF, 0x08 };
		 WriteProcessMemory(hProcess, (void*)0x4B57B0, (void*)&G_GameExeV2, sizeof(G_GameExeV2), NULL);

		/* BYTE g_private_key2228[] = { 0x68,0x01,0x60,0x07,0x12 };	//Jvcryptıon	
		 BYTE g_private_keyop2228[] = { 0x68,0x66,0x89,0x13,0x20 };//Crypton

		 HANDLE hProcess = 0;
		 hProcess = GetCurrentProcess();
		 WriteProcessMemory(hProcess, (void*)0x497BB0, (void*)&g_private_key2228, sizeof(g_private_key2228), NULL);
		 WriteProcessMemory(hProcess, (void*)0x49893E, (void*)&g_private_key2228, sizeof(g_private_key2228), NULL);
		 WriteProcessMemory(hProcess, (void*)0x4BE49F, (void*)&g_private_key2228, sizeof(g_private_key2228), NULL);

		 WriteProcessMemory(hProcess, (void*)0x4BE4A4, (void*)&g_private_keyop2228, sizeof(g_private_keyop2228), NULL);
		 WriteProcessMemory(hProcess, (void*)0x498943, (void*)&g_private_keyop2228, sizeof(g_private_keyop2228), NULL);
		 WriteProcessMemory(hProcess, (void*)0x497BB5, (void*)&g_private_keyop2228, sizeof(g_private_keyop2228), NULL);*/

		 char* ESN = "http://hooksuz.saganetwork.net:90/?";
		 char* Spinwheels = "http://hooksuz.saganetwork.net/Spinwheels/?";

		 SetMemString(GetMemDWORD(0x0085EE92 + 1), ESN, strlen(ESN));
		 SetMemString(GetMemDWORD(0x0085EEA3 + 1), ESN, strlen(ESN));
		 SetMemString(GetMemDWORD(0x0089E754 + 1), Spinwheels, strlen(Spinwheels));
		 SetMemString(GetMemDWORD(0x008A284A + 1), Spinwheels, strlen(Spinwheels));

		 if (!Engine->Securty())
			 return false;
	 }
	return TRUE;
}