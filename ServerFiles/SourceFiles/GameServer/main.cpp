#include "stdafx.h"
#include "../shared/Condition.h"
#include "ConsoleInputThread.h"
#include "../shared/signal_handler.h"

CGameServerDlg * g_pMain;
static Condition s_hEvent;

BOOL WINAPI _ConsoleHandler(DWORD dwCtrlType);

bool g_bRunning = true;

int main()
{
	uint16 m_bOnlineCount = 0, m_bMerchantCount = 0, m_bMiningCount = 0, m_bGenieCount = 0, m_bFishingCount = 0;

	std::string texts;
	texts = string_format("GA Server - v2500 - [Online :%d - Merchant :%d - Mining :%d - Fishing :%d - Genie :%d]", m_bOnlineCount, m_bMerchantCount, m_bMiningCount, m_bFishingCount, m_bGenieCount);
	SetConsoleTitle(texts.c_str());

	// Override the console handler
	SetConsoleCtrlHandler(_ConsoleHandler, TRUE);

	CBugTrap* sBugTrap = new CBugTrap();
	if (!sBugTrap->IsBugTrapLoaded())
	{
#ifndef _WIN64
		MessageBox(0, "You Need BugTrap.dll", "BugTrap.dll Error", MB_SYSTEMMODAL | MB_OK | MB_ICONERROR);
#else
		MessageBox(0, "You Need BugTrap.dll", "BugTrap-x64.dll Error", MB_SYSTEMMODAL | MB_OK | MB_ICONERROR);
#endif
		return 0;
	}

	HookSignals(&s_hEvent);

	// Start up the time updater thread
	StartTimeThread();

	// Start up the console input thread
	StartConsoleInputThread();

	g_pMain = new CGameServerDlg();
	g_pMain->s_hEvent = &s_hEvent;

	// Start up server
	if (g_pMain->Startup()){
		// Reset Battle Zone Variables.
		g_pMain->ResetBattleZone(BATTLEZONE_NONE);
		printf("\nServer started up successfully!\n");

		// Wait until console's signaled as closing
		s_hEvent.Wait();
	}
	else { system("pause"); }

	printf("Server shutting down, please wait...\n");
	// This seems redundant, but it's not. 
	// We still have the destructor for the dialog instance, which allows time for threads to properly cleanup.
	g_bRunning = false; 

	delete g_pMain;

	CleanupTimeThread();
	CleanupConsoleInputThread();
	UnhookSignals();

	return 0;
}

BOOL WINAPI _ConsoleHandler(DWORD dwCtrlType)
{
	s_hEvent.BeginSynchronized();
	s_hEvent.Signal();
	s_hEvent.EndSynchronized();
	sleep(10000); // Win7 onwards allows 10 seconds before it'll forcibly terminate
	return TRUE;
}
