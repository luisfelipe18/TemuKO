#include "stdafx.h"
#include "../shared/signal_handler.h"

LoginServer * g_pMain;
static Condition s_hEvent;
bool g_bRunning = true;

BOOL WINAPI _ConsoleHandler(DWORD dwCtrlType);

int main()
{
	SetConsoleTitle("LO Server - v2500");

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

	g_pMain = new LoginServer();
	g_pMain->s_hEvent = &s_hEvent;

	// Startup server
	if (g_pMain->Startup()){
		printf("\nServer started up successfully!\n\n");
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
