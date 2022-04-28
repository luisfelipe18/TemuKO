#include "stdafx.h"
#include "TEMUKOACS Engine.h"

void TEMUKOACSEngine::CheckRunTimeTBLs()
{
	DWORD skillbase = 0;
	DWORD skillid = 0;
	DWORD value = 0;
	int counter = 0;
	while (g_bRunning == INITIATE)
	{
		if (Engine->Player.isGameStat)
		{
			counter = 0;
			for (uint16 i = 0; i < skillIDs_size; i++)
			{
				if (g_bRunning != GuardOpCodes::INITIATE)
					continue;

				skillid = skillIDs[i];
				skillbase = 0;
				_asm
				{
					mov ecx, [KO_SMMB]
					mov ecx, [ecx]
					push skillid
					call[KO_SMMF]
					mov skillbase, eax
				}

				if (skillbase)
				{
					for (uint8 j = 0; j < sBaseOff_size; j++)
					{
						if (g_bRunning != GuardOpCodes::INITIATE)
							continue;

						value = ReadDWord(skillbase + skillBaseOffsets[j]);
						if (skillIDs[counter++] != value)
						{
							g_bRunning = TBL;
							Engine->OnLog(true, CHILD_SCAN_DETECTED);
						}
					}
				}
			}
		}

		bool isFound = false;

		foreach(itr, threadArray)
		{
			if (itr->second.Id == 54342)
			{
				itr->second.updateTime = (uint32)time(nullptr);
				isFound = true;
				break;
			}
		}

		if (!isFound)
		{
			g_bRunning = THREAD;
			return;
		}

		Sleep(10 * SECOND);
	}
}

BOOL CALLBACK TEMUKOACSEngine::HeuristicClassCheck(HWND hwnd, LPARAM lParam)
{
	static TCHAR y[60];
	GetClassName(hwnd, y, 60);

	if (_tcsstr(y, "ProcessHacker") ||
		_tcsstr(y, "PROCEXP") ||
		_tcsstr(y, "TFormMain.UnicodeClass") ||
		_tcsstr(y, "Debugger") ||
		_tcsstr(y, "debugger") ||
		_tcsstr(y, "hack"))
	{
		g_bRunning = UGLY_PROCESS;
		OnLog(true, "%s Harmful Class (zv3-2)", y);
		return FALSE;
	}
	//cout <<"true";
	return TRUE;
}


extern BOOL CALLBACK wHeuristicClassCheck(HWND hwnd, LPARAM lParam);

void TEMUKOACSEngine::HeuristicClassCheckProc()
{
	do
	{
		EnumWindows(wHeuristicClassCheck, LPARAM(this));
		Sleep(2 * SECOND);
	} while (g_bRunning == GuardOpCodes::INITIATE);
}

void TEMUKOACSEngine::ReadClientName(std::string& strname)
{
	char ClientName[25];
	ReadProcessMemory(GetCurrentProcess(), (LPVOID)KO_WND, ClientName, sizeof(ClientName), NULL);
	strname = string(ClientName);
}

bool TEMUKOACSEngine::isClientNameCorrect()
{
	return strClientName.compare(GAME_CLIENT_NAME) == 0;
}

void TEMUKOACSEngine::UserCheckThreadProc()
{
	ReadClientName(strClientName);

	if (!isClientNameCorrect())
	{
		g_bRunning = CLIENTNAME_ERROR;
		OnLog(true, CLIENT_NAME_INVALID);
	}

#ifdef __DEBUG
	OnLog(true, CLIENT_NAME_VALID, strClientName);
#endif
	uint32 timer_counter = 0;
	do
	{
		if (Player.isGameStat)
		{
			bool isFound = false;
			foreach(itr, threadArray)
			{
				if (itr->second.Id == 53999004)
				{
					itr->second.updateTime = (uint32)time(nullptr);
					isFound = true;
					break;
				}
			}

			if (!isFound)
			{
				g_bRunning = THREAD;
				return;
			}

			if (!(DWORD)GetModuleHandle("KnightOnLine.exe"))
			{
				g_bRunning = MULTI_CLIENT;
				OnLog(true, CLIENT_NAME_INVALID);
			}
		}Sleep(30 * SECOND);
	} while (g_bRunning == GuardOpCodes::INITIATE);
}

DWORD TEMUKOACSEngine::GetProcessIntegrityLevel()
{
	DWORD dwIntegrityLevel = 0;
	DWORD dwError = ERROR_SUCCESS;
	HANDLE hToken = NULL;
	DWORD cbTokenIL = 0;
	PTOKEN_MANDATORY_LABEL pTokenIL = NULL;

	// Open the primary access token of the process with TOKEN_QUERY.
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
	{
		dwError = GetLastError();
		goto Cleanup;
	}

	// Query the size of the token integrity level information. Note that 
	// we expect a FALSE result and the last error ERROR_INSUFFICIENT_BUFFER
	// from GetTokenInformation because we have given it a NULL buffer. On 
	// exit cbTokenIL will tell the size of the integrity level information.
	if (!GetTokenInformation(hToken, TokenIntegrityLevel, NULL, 0, &cbTokenIL))
	{
		if (ERROR_INSUFFICIENT_BUFFER != GetLastError())
		{
			// When the process is run on operating systems prior to Windows 
			// Vista, GetTokenInformation returns FALSE with the 
			// ERROR_INVALID_PARAMETER error code because TokenElevation 
			// is not supported on those operating systems.
			dwError = GetLastError();
			goto Cleanup;
		}
	}

	// Now we allocate a buffer for the integrity level information.
	pTokenIL = (TOKEN_MANDATORY_LABEL*)LocalAlloc(LPTR, cbTokenIL);
	if (pTokenIL == NULL)
	{
		dwError = GetLastError();
		goto Cleanup;
	}

	// Retrieve token integrity level information.
	if (!GetTokenInformation(hToken, TokenIntegrityLevel, pTokenIL,
		cbTokenIL, &cbTokenIL))
	{
		dwError = GetLastError();
		goto Cleanup;
	}

	// Integrity Level SIDs are in the form of S-1-16-0xXXXX. (e.g. 
	// S-1-16-0x1000 stands for low integrity level SID). There is one and 
	// only one subauthority.
	dwIntegrityLevel = *GetSidSubAuthority(pTokenIL->Label.Sid, 0);

Cleanup:
	// Centralized cleanup for all allocated resources.
	if (hToken)
	{
		CloseHandle(hToken);
		hToken = NULL;
	}
	if (pTokenIL)
	{
		LocalFree(pTokenIL);
		pTokenIL = NULL;
		cbTokenIL = 0;
	}

	// Throw the error if something failed in the function.
	if (ERROR_SUCCESS != dwError)
	{
		throw dwError;
	}

	return dwIntegrityLevel;
}

DWORD TEMUKOACSEngine::Get_proc_integrity_level()
{
	try
	{
		DWORD dwIntegrityLevel = GetProcessIntegrityLevel();
		return dwIntegrityLevel;
	}
	catch (DWORD dwError)
	{
		ShowDbgMsg(DEBUG_LEVEL::DBG_ERROR, "GetProcessIntegrityLevel failed w/err %lu", dwError);
		return -4;
	}
}

void TEMUKOACSEngine::ProcessIntegrityLevelCheck()
{
	enum PROTECTION_LEVEL
	{
		Untrusted = 0,
		Low = 4096,
		Medium = 8192,
		MediumPlus = 8448,
		High = 12288,
		System = 16384,
		ProtectedProcess = 20480,
		SecureProcess = 28672
	};
	OSVERSIONINFO OS;
	OS.dwOSVersionInfoSize = sizeof(OS);
	GetVersionEx(&OS);
	if (OS.dwMajorVersion >= 6)
	{
		do
		{

			if (Get_proc_integrity_level() == PROTECTION_LEVEL::Untrusted)
				ShowDbgMsg(DEBUG_LEVEL::DBG_WARNING, "Protection Authorization -> UNTRUSTED");
			else if (Get_proc_integrity_level() == PROTECTION_LEVEL::Low)
				ShowDbgMsg(DEBUG_LEVEL::DBG_WARNING, "Protection Authorization -> LOW");
			else if (Get_proc_integrity_level() == PROTECTION_LEVEL::Medium)
				ShowDbgMsg(DEBUG_LEVEL::DBG_WARNING, "Protection Authorization -> MEDIUM");
			else if (Get_proc_integrity_level() == PROTECTION_LEVEL::MediumPlus)
				ShowDbgMsg(DEBUG_LEVEL::DBG_WARNING, "Protection Authorization -> MEDIUM PLUS");
			else if (Get_proc_integrity_level() == PROTECTION_LEVEL::High)
				ShowDbgMsg(DEBUG_LEVEL::DBG_SYS, "Protection Authorization -> HIGH");
			else if (Get_proc_integrity_level() == PROTECTION_LEVEL::System)
				ShowDbgMsg(DEBUG_LEVEL::DBG_SYS, "Protection Authorization -> SYSTEM");
			else if (Get_proc_integrity_level() == PROTECTION_LEVEL::ProtectedProcess)
				ShowDbgMsg(DEBUG_LEVEL::DBG_SYS, "Protection Authorization -> PROTECTED PROCESS");
			else if (Get_proc_integrity_level() == PROTECTION_LEVEL::SecureProcess)
				ShowDbgMsg(DEBUG_LEVEL::DBG_SYS, "Protection Authorization -> SECURE PROCESS");
			else
				ShowDbgMsg(DEBUG_LEVEL::DBG_SYS, "Protection Authorization -> %d", Get_proc_integrity_level());

			if (Get_proc_integrity_level() != -4)
			{
				DWORD const dwIntegrityLevel = GetProcessIntegrityLevel();
				if (dwIntegrityLevel != 12288)
				{
					g_bRunning = BYPASS;
					OnLog(true, "Koruma yetkisi s�n�rland�r�lm��(2), L�tfen oyunu tekrar ba�lat�n. Sorun devam ederse y�neticiye ba�vurun.");

				}
			}

			bool isFound = false;

			foreach(itr, threadArray)
			{
				if (itr->second.Id == 833245)
				{
					itr->second.updateTime = (uint32)time(nullptr);
					isFound = true;
					break;
				}
			}

			if (!isFound)
			{
				g_bRunning = THREAD;
				return;
			}

			Sleep(5 * SECOND);

		} while (g_bRunning == GuardOpCodes::INITIATE);
	}
	else
	{
		do
		{

			bool isFound = false;

			foreach(itr, threadArray)
			{
				if (itr->second.Id == 833245)
				{
					itr->second.updateTime = (uint32)time(nullptr);
					isFound = true;
					break;
				}
			}

			if (!isFound)
			{
				g_bRunning = THREAD;
				return;
			}

			Sleep(5 * SECOND);

		} while (g_bRunning == GuardOpCodes::INITIATE);
	}
}

void TEMUKOACSEngine::HookChecker()
{
	//FARPROC WSA_RECV_ADDR = GetProcAddress(GetModuleHandle("Ws2_32.dll"), "WSARecv");
	//FARPROC WSA_SEND_ADDR = GetProcAddress(GetModuleHandle("Ws2_32.dll"), "WSASend");
	//FARPROC RECV_ADDR = GetProcAddress(GetModuleHandle("Ws2_32.dll"), "recv");
	//FARPROC SEND_ADDR = GetProcAddress(GetModuleHandle("Ws2_32.dll"), "send");
	//FARPROC CONNECT_ADDR = GetProcAddress(GetModuleHandle("Ws2_32.dll"), "connect");
	//FARPROC ACTIVWIND_ADDR = GetProcAddress(GetModuleHandle("User32.dll"), "GetActiveWindow");



	FARPROC MSGBOX_ADDR = GetProcAddress(GetModuleHandle("User32.dll"), "MessageBoxA");
	FARPROC GetWindow_ADDR = GetProcAddress(GetModuleHandle("user32.dll"), "GetWindowTextA");
	FARPROC FindWindow_ADDR = GetProcAddress(GetModuleHandle("user32.dll"), "FindWindowA");
	FARPROC FindWindowEx_ADDR = GetProcAddress(GetModuleHandle("user32.dll"), "FindWindowExA");
	FARPROC EnumWindows_ADDR = GetProcAddress(GetModuleHandle("user32.dll"), "EnumWindows");
	FARPROC IsWindowVisible_ADDR = GetProcAddress(GetModuleHandle("user32.dll"), "IsWindowVisible");
	FARPROC GetWindowThreadProcessId_ADDR = GetProcAddress(GetModuleHandle("user32.dll"), "GetWindowThreadProcessId");

	while (g_bRunning == GuardOpCodes::INITIATE)
	{
		AlreadyRing3Hook(MSGBOX_ADDR);
		AlreadyRing3Hook(GetWindow_ADDR);
		AlreadyRing3Hook(FindWindow_ADDR);
		AlreadyRing3Hook(FindWindowEx_ADDR);
		AlreadyRing3Hook(EnumWindows_ADDR);
		AlreadyRing3Hook(IsWindowVisible_ADDR);
		AlreadyRing3Hook(GetWindowThreadProcessId_ADDR);

		bool isFound = false;

		foreach(itr, threadArray)
		{
			if (itr->second.Id == 231245)
			{
				itr->second.updateTime = (uint32)time(nullptr);
				isFound = true;
				break;
			}
		}

		if (!isFound)
		{
			g_bRunning = THREAD;
			return;
		}

		Sleep(3 * SECOND);
	}
}

void TEMUKOACSEngine::AlreadyRing3Hook(PVOID FuncAddr)
{
	LPBYTE Addr = (LPBYTE)FuncAddr;
	if (Addr[0] == 0xEB 
		|| Addr[0] == 0xE9 
		|| Addr[0] == 0x90)
	{
		//OnLog(true, "TEST g_prunning false 1");
		g_bRunning = UGLY_DRIVER;
	}
}

void TEMUKOACSEngine::ClassScanProc()
{
	do
	{
		bool isChecked = false;

		ClassWindow("TfrmSlotRota", isChecked);
		ClassWindow("TfrmItemList", isChecked);
		ClassWindow("PROCEXPL", isChecked);             // Process explorer
		ClassWindow("TreeListWindowClass", isChecked);  // Process explorer (Process windows)
		ClassWindow("ProcessHacker", isChecked);        // Process Hacker	
		ClassWindow("PhTreeNew", isChecked);            // Process Hakcer (Process windows)
		ClassWindow("SysListView32", isChecked);        // Lista de processos do process explorer
		ClassWindow("TformSettings", isChecked);
		ClassWindow("TUserdefinedform", isChecked);
		ClassWindow("TformAddressChange", isChecked);
		ClassWindow("TMemoryBrowser", isChecked);
		ClassWindow("TFoundCodeDialog", isChecked);
		ClassWindow("TXmHexEdit", isChecked);
		ClassWindow("TFormRAMOpen.UnicodeClass", isChecked);
		ClassWindow("OLLYDBG", isChecked);
		ClassWindow("Dll-Injector", isChecked);
		ClassWindow("N-Jector by NOOB", isChecked);
		ClassWindow("Twnd_tm_Main.ProcessHacker", isChecked);
		ClassWindow("ProcessHacker", isChecked);
		ClassWindow("OllyDbg", isChecked);
		ClassWindow("ollydbg", isChecked);
		ClassWindow("procmon_window_class", isChecked);
		ClassWindow("processhacker", isChecked);
		ClassWindow("WindbgMainClass", isChecked);
		ClassWindow("procexpl", isChecked);
		ClassWindow("unhookerclass", isChecked);
		ClassWindow("WinDbgFrameClass", isChecked);
		ClassWindow("Zeta Debugger", isChecked);
		ClassWindow("ObsidianGUI", isChecked);
		ClassWindow("Rock Debugger", isChecked);
		ClassWindow("idawindow", isChecked);
		ClassWindow("idaview", isChecked);
		ClassWindow("Window", isChecked);
		ClassWindow("TM4DB0T", isChecked);
		ClassWindow("WdcWindow", isChecked);
		ClassWindow("TaskManagerWindow", isChecked);
		ClassWindow("TMainForm.UnicodeClass", isChecked);
		ClassWindow("Class_PLMain", isChecked);
		ClassWindow("WindowsForms10.Window.8.app.0.378734a", isChecked);
		ClassWindow("xf1h2cad", isChecked);

		bool isFound = false;

		foreach(itr, threadArray)
		{
			if (itr->second.Id == 493334)
			{
				itr->second.updateTime = (uint32)time(nullptr);
				isFound = true;
				break;
			}
		}

		if (!isChecked || !isFound)
		{
			g_bRunning = THREAD;
			return;
		}
		Sleep(3 * SECOND);
	} while (g_bRunning == GuardOpCodes::INITIATE);
}

void TEMUKOACSEngine::ClassWindow(string WindowClasse, bool& isChecked)
{
	HWND WinClasse = FindWindowExA(NULL, NULL, WindowClasse.c_str(), NULL);

	if (WinClasse > 0)
	{
		if (WindowClasse == "TfrmSlotRota" || WindowClasse == "TfrmItemList")
		{
			//OnLog(true, "TEST g_prunning false 3");
			g_bRunning = UGLY_PROCESS;
		}
		else
		{
			g_bRunning = UGLY_PROCESS;
			OnLog(true, PROTECTION_AUTH_ERROR);//, WindowClasse);
		}
	}

	isChecked = true;
}

BOOL CALLBACK TEMUKOACSEngine::HeuristicWindowCheck(HWND hwnd, LPARAM lParam)
{
	static TCHAR x[40];
	GetWindowText(hwnd, x, 40);
	if (_tcsstr(x, "byPass") ||
		_tcsstr(x, "cheat") ||
		_tcsstr(x, "hack") ||
		_tcsstr(x, "inject") ||
		_tcsstr(x, "ollydbg") ||
		_tcsstr(x, "suspend") ||
		_tcsstr(x, "sandbox") ||
		_tcsstr(x, "BVKHEX") ||
		_tcsstr(x, "Bvkhex") ||
		_tcsstr(x, "bvkhex") ||
		_tcsstr(x, "bvkhex") ||
		_tcsstr(x, "bvkhex") ||
		_tcsstr(x, "autoclicker") ||
		_tcsstr(x, "debug"))
	{
		g_bRunning = UGLY_PROCESS;
		OnLog(true, "%s Sezgisel Tarama (z3-2)", x);
		return FALSE;
	}
	return TRUE;
}
extern BOOL CALLBACK wHeuristicWindowCheck(HWND hwnd, LPARAM lParam);

void TEMUKOACSEngine::HeuristicWindowCheckProc()
{
	do
	{
		EnumWindows(wHeuristicWindowCheck, LPARAM(this));
		Sleep(5 * SECOND);
	} while (g_bRunning == GuardOpCodes::INITIATE);
}

void TEMUKOACSEngine::InjectScanProc()
{
	do
	{
		if (
			(GetModuleHandle("SbieDll.dll")) ||/*sandbox*/
			(GetModuleHandle("sbiedll.dll")) ||/*sandbox*/
			(GetModuleHandle("vmbusres.dll")) ||/*vmware*/
			(GetModuleHandle("vmGuestLib.dll")) ||/*//vmware*/
			(GetModuleHandle("smoll.dll")) ||
			(GetModuleHandle("SwishDbgExt.dll")) || /*windbg*/
			(GetModuleHandle("cd.cv")) || /*hackloader*/
			(GetModuleHandle("VBoxHook.dll")) ||
			(GetModuleHandle("phtrenbot.dll")))

		{
			g_bRunning = DLL_INJECT;
			OnLog(true, TT24_DLL_INJ_DETECTED);
		}


		bool isFound = false;

		foreach(itr, threadArray)
		{
			if (itr->second.Id == 50454)
			{
				itr->second.updateTime = (uint32)time(nullptr);
				isFound = true;
				break;
			}
		}

		if (!isFound)
		{
			g_bRunning = THREAD;
			return;
		}
		Sleep(7 * SECOND);
	} while (g_bRunning == GuardOpCodes::INITIATE);
}

bool TEMUKOACSEngine::TitleWindow(bool& isChecked, LPCSTR WindowTitle)
{
	if (FindWindowA(NULL, WindowTitle) > 0)
	{

		g_bRunning = UGLY_PROCESS;
		OnLog(true, CHILD_SCAN_DETECTED);
		return false;
	}
	isChecked = true;
	return true;
}


void TEMUKOACSEngine::TileScanProc()
{
	do
	{
		bool isChecked = false;
		TitleWindow(isChecked, "Add address");
		TitleWindow(isChecked, "ArtMoney PRO v7.27");
		TitleWindow(isChecked, "ArtMoney SE v7.31");
		TitleWindow(isChecked, "ArtMoney SE v7.32");
		TitleWindow(isChecked, "Cheat Engine 5.0");
		TitleWindow(isChecked, "Cheat Engine 5.1");
		TitleWindow(isChecked, "Cheat Engine 5.1.1");
		TitleWindow(isChecked, "Cheat Engine 5.2");
		TitleWindow(isChecked, "Cheat Engine 5.3");
		TitleWindow(isChecked, "Cheat Engine 5.4");
		TitleWindow(isChecked, "Cheat Engine 5.5");
		TitleWindow(isChecked, "Cheat Engine 5.6");
		TitleWindow(isChecked, "Cheat Engine 5.6.1");
		TitleWindow(isChecked, "Cheat Engine 5.6.2");
		TitleWindow(isChecked, "Cheat Engine 5.6.3");
		TitleWindow(isChecked, "Cheat Engine 5.6.4");
		TitleWindow(isChecked, "Cheat Engine 5.6.5");
		TitleWindow(isChecked, "Cheat Engine 5.6.6");
		TitleWindow(isChecked, "Cheat Engine 6.0");
		TitleWindow(isChecked, "Cheat Engine 6.1");
		TitleWindow(isChecked, "Cheat Engine 6.2");
		TitleWindow(isChecked, "Cheat Engine 6.3");
		TitleWindow(isChecked, "Cheat Engine 6.4");
		TitleWindow(isChecked, "Cheat Engine 6.5");
		TitleWindow(isChecked, "Cheat Engine 6.6");
		TitleWindow(isChecked, "Cheat Engine 6.7");
		TitleWindow(isChecked, "Cheat Engine");
		TitleWindow(isChecked, "Created processes");
		TitleWindow(isChecked, "D-C Bypass");
		TitleWindow(isChecked, "DC-BYPASS By DjCheats  Public Vercion");
		TitleWindow(isChecked, "HiDeToolz");
		TitleWindow(isChecked, "HideToolz");
		TitleWindow(isChecked, "Injector");
		TitleWindow(isChecked, "Olly Debugger");
		TitleWindow(isChecked, "Process Explorer 11.33");
		TitleWindow(isChecked, "Process Explorer");
		TitleWindow(isChecked, "T Search");
		TitleWindow(isChecked, "WPE PRO");
		TitleWindow(isChecked, "WPePro 0.9a");
		TitleWindow(isChecked, "WPePro 1.3");
		TitleWindow(isChecked, "hacker");
		TitleWindow(isChecked, "rPE - rEdoX Packet Editor");
		TitleWindow(isChecked, "OllyDbg");
		TitleWindow(isChecked, "HxD");
		TitleWindow(isChecked, "Process Hacker 2");
		TitleWindow(isChecked, "Process Hacker");
		TitleWindow(isChecked, "Ultimate Cheat");
		TitleWindow(isChecked, "The following opcodes accessed the selected address");
		TitleWindow(isChecked, "lalaker1");
		TitleWindow(isChecked, "HiDeToolz");
		TitleWindow(isChecked, "HideToolz");
		TitleWindow(isChecked, "IDA!");
		TitleWindow(isChecked, "The interactive disassembler");
		TitleWindow(isChecked, "1nj3ct0r");
		TitleWindow(isChecked, "injector");
		TitleWindow(isChecked, "Injector");
		TitleWindow(isChecked, "dreamfancy");
		TitleWindow(isChecked, "pvp hile indir");
		TitleWindow(isChecked, "dreamfancy");
		TitleWindow(isChecked, "Knight Online tr hile indir");
		TitleWindow(isChecked, "Knight Online oyun hileleri sitesi 2015");
		TitleWindow(isChecked, "Knight Online Hilecim");
		TitleWindow(isChecked, "Knight Online Hileleri");
		TitleWindow(isChecked, "Knight Online Hile �ndir");
		TitleWindow(isChecked, "Online Oyun Hileleri");
		TitleWindow(isChecked, "N-Jector by NOOB");
		TitleWindow(isChecked, "[BETA] Hack Loader v1.0.2");
		TitleWindow(isChecked, "[BETA] Hack Loader v1.0.3");
		TitleWindow(isChecked, "Wallhack");
		TitleWindow(isChecked, "Pro Damage");
		TitleWindow(isChecked, "Wait Damage (7x)");
		TitleWindow(isChecked, "Ok�u hilesi");
		TitleWindow(isChecked, "Daily Stage 2 (G�nl�k A�ama 2 )");
		TitleWindow(isChecked, "Injector [FaithDEV]");
		TitleWindow(isChecked, "Injector Gadget");
		TitleWindow(isChecked, "OldSchoolInject");
		TitleWindow(isChecked, "Extreme Injector");
		TitleWindow(isChecked, "| Fortis Fortuna Adiuvat |");
		TitleWindow(isChecked, "PHTrenbot");
		TitleWindow(isChecked, "Kaynak �zleyicisi");
		TitleWindow(isChecked, "G�rev Y�neticisi");
		TitleWindow(isChecked, "Process Lasso");
		TitleWindow(isChecked, "System Explorer 7.0.0.5356");
		TitleWindow(isChecked, "Minor Program�");

		bool isFound = false;

		foreach(itr, threadArray)
		{
			if (itr->second.Id == 843913)
			{
				itr->second.updateTime = (uint32)time(nullptr);
				isFound = true;
				break;
			}
		}

		if (!isChecked || !isFound)
		{
			g_bRunning = THREAD;
			return;
		}
		Sleep(3 * SECOND);
	} while (g_bRunning == GuardOpCodes::INITIATE);
}

static int	     _TimerInited = 0;
static double	     _fTicksPerSec;
static LARGE_INTEGER _llTimerStart;

void TEMUKOACSEngine::QueryCounterCountProc()
{
	do
	{
		bool isFound = false;

		foreach(itr, threadArray)
		{
			if (itr->second.Id == 932053)
			{
				itr->second.updateTime = (uint32)time(nullptr);
				isFound = true;
				break;
			}
		}

		if (!isFound)
		{
			g_bRunning = THREAD;
			return;
		}

		foreach(itr, threadArray)
		{
			if (itr->second.Id == 932053)
			{
				uint32_t Timer = (uint32)time(nullptr);
				if (itr->second.updateTime + 10 < Timer)
				{
					g_bRunning = BYPASS;
					printf("Packet: %lu, UpdateTime : %lu, CheckTime : %lu \n", itr->second.Id, itr->second.updateTime, Timer);
					OnLog(true, "Suspend was detected");
				}break;
			}
		}
		Sleep(3 * SECOND);
	} while (g_bRunning == GuardOpCodes::INITIATE);
}

#define ARRAY_SIZE 1024

void TEMUKOACSEngine::check_driver()
{
	LPVOID drivers[ARRAY_SIZE];
	DWORD cbNeeded;
	std::string newdriver;
	int cDrivers, i;

	do
	{
		if (EnumDeviceDrivers(drivers, sizeof(drivers), &cbNeeded) && cbNeeded < sizeof(drivers))
		{
			TCHAR szDriver[ARRAY_SIZE];
			cDrivers = cbNeeded / sizeof(drivers[0]);
			for (i = 0; i < cDrivers; i++)
			{
				if (GetDeviceDriverBaseName(drivers[i], szDriver, sizeof(szDriver) / sizeof(szDriver[0])))
				{
					newdriver = szDriver;
					if (newdriver.find("pchunter") != std::string::npos)
					{
						g_bRunning = UGLY_DRIVER;
						OnLog(true, PC_HUNTER_DETECTED, newdriver.c_str());
					}

					if (newdriver.find("PChunter") != std::string::npos)
					{
						g_bRunning = UGLY_DRIVER;
						OnLog(true, PC_HUNTER_DETECTED, newdriver.c_str());
					}

					if (newdriver.find("PCHunter") != std::string::npos)
					{
						g_bRunning = UGLY_DRIVER;
						OnLog(true, PC_HUNTER_DETECTED, newdriver.c_str());
					}

					if (newdriver.find("PC Hunter") != std::string::npos)
					{
						g_bRunning = UGLY_DRIVER;
						OnLog(true, PC_HUNTER_DETECTED, newdriver.c_str());
					}

					if (newdriver.find("PC hunter") != std::string::npos)
					{
						g_bRunning = UGLY_DRIVER;
						OnLog(true, PC_HUNTER_DETECTED, newdriver.c_str());
					}

					if (newdriver.find("pc hunter") != std::string::npos)
					{
						g_bRunning = UGLY_DRIVER;
						OnLog(true, PC_HUNTER_DETECTED, newdriver.c_str());
					}

					if (newdriver.find("processhacker") != std::string::npos)
					{
						g_bRunning = UGLY_DRIVER;
						OnLog(true, PROCESS_HACKER_DETECTED, newdriver.c_str());
					}

					if (newdriver.find("process hacker") != std::string::npos)
					{
						g_bRunning = UGLY_DRIVER;
						OnLog(true, PROCESS_HACKER_DETECTED, newdriver.c_str());
					}

					if (newdriver.find("cheatengine") != std::string::npos)
					{
						g_bRunning = UGLY_DRIVER;
						OnLog(true, CHEAT_ENGINE_DETECTED, newdriver.c_str());
					}

					if (newdriver.find("cheat engine") != std::string::npos)
					{
						g_bRunning = UGLY_DRIVER;
						OnLog(true, CHEAT_ENGINE_DETECTED, newdriver.c_str());
					}

					if (newdriver == "PROCEXP152.SYS")
					{
						g_bRunning = UGLY_DRIVER;
						OnLog(true, PROCESS_HACKER_DETECTED, newdriver.c_str());
					}

					if (newdriver == "BlackBoneDrv10.sys")
					{
						g_bRunning = UGLY_DRIVER;
						OnLog(true, WINDBG_DETECTED, newdriver.c_str());
					}

					if (newdriver == "dbk32.sys")
					{
						g_bRunning = UGLY_DRIVER;
						OnLog(true, CHEAT_ENGINE_DETECTED, newdriver.c_str());
					}

					if (newdriver == "dbk64.sys")
					{
						g_bRunning = UGLY_DRIVER;
						OnLog(true, CHEAT_ENGINE_DETECTED, newdriver.c_str());
					}

					if (newdriver == "sice.sys")
					{
						g_bRunning = UGLY_DRIVER;
						OnLog(true, CHEAT_ENGINE_DETECTED, newdriver.c_str());
					}

					if (newdriver == "ntice.sys")
					{
						g_bRunning = UGLY_DRIVER;
						OnLog(true, CHEAT_ENGINE_DETECTED, newdriver.c_str());
					}

					if (newdriver == "winice.sys")
					{
						g_bRunning = UGLY_DRIVER;
						OnLog(true, WINICE_DETECTED, newdriver.c_str());
					}

					if (newdriver == "syser.sys")
					{
						g_bRunning = UGLY_DRIVER;
						OnLog(true, SYSER_DETECTED, newdriver.c_str());
					}

					if (newdriver == "sice.vxd")
					{
						g_bRunning = UGLY_DRIVER;
						OnLog(true, SOFTICE_DETECTED, newdriver.c_str());
					}

					if (newdriver == "77fba431.sys")
					{
						g_bRunning = UGLY_DRIVER;
						OnLog(true, WINDBG_DETECTED, newdriver.c_str());
					}
					/*printf(WINDBG_DETECTED "\n", newdriver.c_str());
					OnLog(true, WINDBG_DETECTED, newdriver.c_str());*/
				}
			}
		}

		bool isFound = false;

		foreach(itr, threadArray)
		{
			if (itr->second.Id == 17182480)
			{
				itr->second.updateTime = (uint32)time(nullptr);
				isFound = true;
				break;
			}
		}

		if (!isFound)
		{
			g_bRunning = THREAD;
			return;
		}Sleep(3 * SECOND);
	} while (g_bRunning == GuardOpCodes::INITIATE);
}

BOOL CALLBACK TEMUKOACSEngine::EnumChildProc(HWND hwnd, LPARAM lParam)
{
	char buff[256];

	GetWindowTextA(hwnd, buff, sizeof(buff));

	DWORD windowOwnerPid;
	GetWindowThreadProcessId(hwnd, &windowOwnerPid);

	if (n_dMyPid == windowOwnerPid && IsWindowVisible(hwnd))
	{
		windowCount++;

		if (strcmp("Knight OnLine Client", buff))
			windowCount++;
	}

	string str = buff;

	if (str != "")
	{
		for (unsigned int i = 0; i < str.length(); ++i)
		{
			str[i] = tolower(str[i]);
		}
	}

	if (str.find("process hacker") != std::string::npos ||
		str.find("dk:") != std::string::npos ||
		str.find("process explorer") != std::string::npos ||
		str.find("suspend") != std::string::npos ||
		str.find("display only disabled windows") != std::string::npos ||
		str.find("clicker") != std::string::npos ||
		str.find("auto clicker") != std::string::npos ||
		str.find("inject") != std::string::npos ||
		str.find("cheat") != std::string::npos ||
		str.find("wallhack") != std::string::npos ||
		str.find("n-jector") != string::npos ||
		str.find("Speed (G)") != string::npos ||
		str.find("Cheat") != string::npos ||
		str.find("CheatEngine") != string::npos ||
		str.find("Cheat Engine") != string::npos ||
		str.find("BOT") != string::npos ||
		str.find("System Explorer") != string::npos ||
		str.find("SystemExplorer") != string::npos ||
		str.find("Process Lasso") != string::npos ||
		str.find("ProcessLasso") != string::npos)
	{
		g_bRunning = UGLY_PROCESS;
		OnLog(true, CHILD_SCAN_DETECTED);
	}

	return TRUE; // must return TRUE; If return is FALSE it stops the recursion
}

extern BOOL CALLBACK wEnumChildProc(HWND hwnd, LPARAM lParam);

void TEMUKOACSEngine::child_check()
{
	do
	{
		windowCount = 0;
		EnumWindows(wEnumChildProc, LPARAM(this));
		bool isFound = false;

		foreach(itr, threadArray)
		{
			if (itr->second.Id == 18842378)
			{
				itr->second.updateTime = (uint32)time(nullptr);
				isFound = true;
				break;
			}
		}

		if (!isFound)
		{
			g_bRunning = THREAD;
			return;
		}
		Sleep(3 * SECOND);
	} while (g_bRunning == GuardOpCodes::INITIATE);
}

BOOL CALLBACK TEMUKOACSEngine::EnumWinHandle(HWND hWnd, LPARAM lparam)
{
	char buff[256];
	char title[80];
	DWORD pidofclass;
	GetClassNameA(hWnd, title, sizeof(title));
	GetWindowThreadProcessId(hWnd, &pidofclass);

	if (strstr(title, "Online DK:") ||
		strstr(title, "Memory View") ||
		strstr(title, "Memory Scan") ||
		strstr(title, "CATASTROPHE") ||
		strstr(title, "DUPAR") ||
		strstr(title, "DarkTeaM") ||
		strstr(title, "MacFox") ||
		strstr(title, "Lipsum") ||
		strstr(title, "Crasher") ||
		strstr(title, "Apocalypsen") ||
		strstr(title, "combo") ||
		strstr(title, "Lionex") ||
		strstr(title, "INJECT") ||
		strstr(title, "Inject") ||
		strstr(title, "inject") ||
		strstr(title, "INJECTOR") ||
		strstr(title, "Value") ||
		strstr(title, "NOOB") ||
		strstr(title, "noob") ||
		strstr(title, "Ollydbg") ||
		strstr(title, "OllyDbg") ||
		strstr(title, "ollydbg") ||
		strstr(title, "Chejcts") ||
		strstr(title, "cheat") ||
		strstr(title, "Cheat") ||
		strstr(title, "wallhack") ||
		strstr(title, "hile") ||
		strstr(title, "H�LE") ||
		strstr(title, "hunter") ||
		strstr(title, "HUNTER") ||
		strstr(title, "pchunter") ||
		strstr(title, "pchunter_free") ||
		strstr(title, "suspend") ||
		strstr(title, "Suspend") ||
		strstr(title, "hayalet") ||
		strstr(title, "TSearch") ||
		strstr(title, "Anvir") ||
		strstr(title, "processes") ||
		strstr(title, "pbdown") ||
		strstr(title, "Artmoney") ||
		strstr(title, "anvir") ||
		strstr(title, "AnVir") ||
		strstr(title, "Processes") ||
		strstr(title, "Hunter") ||
		strstr(title, "Krisppy") ||
		strstr(title, "Process List") ||
		strstr(title, "Siyanur") ||
		strstr(title, "SpeederXP") ||
		strstr(title, "PowerTool") ||
		strstr(title, "System Explorer") ||
		strstr(title, "SystemExplorer") ||
		strstr(title, "ProcessLasso") ||
		strstr(title, "Process Lasso")
		)
	{
		g_bRunning = UGLY_PROCESS;
	}

	return true;
}

extern BOOL CALLBACK wEnumWinHandle(HWND hWnd, LPARAM lparam);

void TEMUKOACSEngine::style_check()
{
	do
	{
		EnumWindows(wEnumWinHandle, LPARAM(this));
		bool isFound = false;

		foreach(itr, threadArray)
		{
			if (itr->second.Id == 8505035)
			{
				itr->second.updateTime = (uint32)time(nullptr);
				isFound = true;
				break;
			}
		}

		if (!isFound)
		{
			g_bRunning = THREAD;
			return;
		}
		Sleep(1 * SECOND);
	} while (g_bRunning == GuardOpCodes::INITIATE);
}

#include <TlHelp32.h>

void TEMUKOACSEngine::TerminateProc_Func(char* ProcName, bool koexe)
{
	PROCESSENTRY32 pe32;
	HANDLE hSnapshot = NULL;

	pe32.dwSize = sizeof(PROCESSENTRY32);
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	int sayi = 0;

	if (Process32First(hSnapshot, &pe32))
	{
		do
		{
			if (strcmp(pe32.szExeFile, ProcName) == 0)
			{
				if (koexe)
					sayi += 1;
				else
				{
					g_bRunning = MULTI_CLIENT;
					OnLog(true, "L�tfen bilgisayar�n�z� yeniden ba�lat�p girin. Please restart your computer.");
				}
			}
			Sleep(1);
		} while (Process32Next(hSnapshot, &pe32));
	}

	if (hSnapshot != INVALID_HANDLE_VALUE)
		CloseHandle(hSnapshot);

	if (sayi > 1)
	{
		g_bRunning = MULTI_CLIENT;
		OnLog(true, "Please don't use thirth-party-tools process.");
	}
}

void TEMUKOACSEngine::Terminate_Scan()
{
	do
	{
		TerminateProc_Func("KnightOnLine.exe", true);
		bool isFound = false;

		foreach(itr, threadArray)
		{
			if (itr->second.Id == 483854)
			{
				itr->second.updateTime = (uint32)time(nullptr);
				isFound = true;
				break;
			}
		}

		// if (!isFound)
		// {
		// 	g_bRunning = THREAD;
		// 	return;
		// }

		Sleep(1 * SECOND);
	} while (g_bRunning == GuardOpCodes::INITIATE);
}

DWORD TEMUKOACSEngine::GetFileSizeOwn(char* FileName)
{
	ifstream f(FileName);

	DWORD fileSize = 0;

	if (f.good())
	{
		HANDLE hFile = CreateFileA(FileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (hFile)
		{
			fileSize = GetFileSize(hFile, NULL);
			CloseHandle(hFile);
		}
	}
	else
		OnLog(true, "Cannot find TBL: %s", FileName);

	f.close();

	return fileSize;
}

string TEMUKOACSEngine::GetCurrentPath()
{
	char result[MAX_PATH];
	return string(result, GetCurrentDirectory(MAX_PATH, result));
}

#pragma comment(lib, "urlmon.lib")
#pragma comment(lib,"wininet.lib")

bool TEMUKOACSEngine::DownloadFile(string fName, string tDir)
{
	string tFile = GetCurrentPath() + "\\" + tDir + "\\" + fName;
	string link = "Dosya hatasi, lutfen oyunu yeniden calistirmayi deneyin veya http://www.TEMUKOACS.com/patches/Data-Fix/" + tDir + "/" + fName + " adresinden dosyasini indirip oyun data klasorune atarak deneyiniz..";
	LPCTSTR Url = _T(link.c_str()), File = _T(tFile.c_str());
	HRESULT hr = URLDownloadToFileA(0, Url, File, 0, 0);

	if (hr != S_OK)
	{
		OnLog(true, link);
		return false;
	}
	return true;
}

#include "MD6.h"
MD6 md6;
void TEMUKOACSEngine::Tbl_Scan()
{
	do
	{
		if (GetFileSizeOwn("DATA\\Skill_Magic_1.tbl") != 0x6AAC
			|| strcmp(md6.digestFile("DATA\\Skill_Magic_1.tbl"), "e0d735c79fcb36808d1f096010423f52"))
			DownloadFile("Skill_Magic_1.tbl", "Data");

		if (GetFileSizeOwn("DATA\\Skill_Magic_2.tbl") != 0x8C4
			|| strcmp(md6.digestFile("DATA\\Skill_Magic_2.tbl"), "66a3dc7ef27f9dc8a8840a09d6a21935"))
			DownloadFile("Skill_Magic_2.tbl", "Data");

		if (GetFileSizeOwn("DATA\\Skill_Magic_3.tbl") != 0x7584
			|| strcmp(md6.digestFile("DATA\\Skill_Magic_3.tbl"), "cfb78bb8065d94d05102c35f3627573b"))
			DownloadFile("Skill_Magic_3.tbl", "Data");

		if (GetFileSizeOwn("DATA\\Skill_Magic_4.tbl") != 0x23794
			|| strcmp(md6.digestFile("DATA\\Skill_Magic_4.tbl"), "17a579b8a95a095d0c9b4d8be71d0f2e"))//tbl �ifrelemesine kadar mevcut bunu g�sterece�im sana kendin yapars�n.
			DownloadFile("Skill_Magic_4.tbl", "Data");

		if (GetFileSizeOwn("DATA\\Skill_Magic_5.tbl") != 0x384
			|| strcmp(md6.digestFile("DATA\\Skill_Magic_5.tbl"), "65ed59a13a7d8e377de695b9d6f03a86"))
			DownloadFile("Skill_Magic_5.tbl", "Data");

		if (GetFileSizeOwn("DATA\\Skill_Magic_6.tbl") != 0x79E4
			|| strcmp(md6.digestFile("DATA\\Skill_Magic_6.tbl"), "22ca23a99f76206ac5a891f86fea5689"))
			DownloadFile("Skill_Magic_6.tbl", "Data");

		if (GetFileSizeOwn("DATA\\Skill_Magic_7.tbl") != 0x10C
			|| strcmp(md6.digestFile("DATA\\Skill_Magic_7.tbl"), "043bef1ddb4fa472ebaa5866d192908c"))
			DownloadFile("Skill_Magic_7.tbl", "Data");

		if (GetFileSizeOwn("DATA\\Skill_Magic_8.tbl") != 0x484
			|| strcmp(md6.digestFile("DATA\\Skill_Magic_8.tbl"), "4ffc26f0c8fb947f58541f335fc86a65"))
			DownloadFile("Skill_Magic_8.tbl", "Data");

		if (GetFileSizeOwn("DATA\\Skill_Magic_9.tbl") != 0xEC4
			|| strcmp(md6.digestFile("DATA\\Skill_Magic_9.tbl"), "aa27446eb7f409162492e57416aac033"))
			DownloadFile("Skill_Magic_9.tbl", "Data");

		if (GetFileSizeOwn("DATA\\Skill_Magic_Main_us.tbl") != 0x9A7EC
			|| strcmp(md6.digestFile("DATA\\Skill_Magic_Main_us.tbl"), "aedc7679589f846a7444b735688e0ada"))
			DownloadFile("Skill_Magic_Main_us.tbl", "Data");

		if (GetFileSizeOwn("DATA\\SkillMagicTable_us.tbl") != 0x822BC
			|| strcmp(md6.digestFile("DATA\\SkillMagicTable_us.tbl"), "b56c3469ba01239ffc765b48ef522883"))
			DownloadFile("SkillMagicTable_us.tbl", "Data");

		bool isFound = false;

		foreach(itr, threadArray)
		{
			if (itr->second.Id == 594965)
			{
				itr->second.updateTime = (uint32)time(nullptr);
				isFound = true;
				break;
			}
		}

		if (!isFound)
		{
			g_bRunning = THREAD;
			return;
		}

		Sleep(3 * SECOND);
	} while (g_bRunning == GuardOpCodes::INITIATE);
}