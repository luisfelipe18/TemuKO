#include "stdafx.h"
#include "Condition.h"
#include "signal_handler.h"
#include <signal.h>

Condition * g_hNotifier;

/**
* @brief	Hooks signals and gets the condition variable from the intended
*			thread.
*/
void HookSignals(Condition * notifier)
{
	g_hNotifier = notifier;

	signal(SIGINT, OnSignal);
	signal(SIGTERM, OnSignal);
	signal(SIGABRT, OnSignal);
	signal(SIGBREAK, OnSignal);
}

/**
* @brief	Handles signals on receive.
*
*/
void OnSignal(int s)
{
	switch (s)
	{
	case SIGINT:
	case SIGTERM:
	case SIGABRT:
	case SIGBREAK:
		g_hNotifier->BeginSynchronized();
		g_hNotifier->Signal();
		g_hNotifier->EndSynchronized();
		break;
	}

	signal(s, OnSignal);
}

/**
* @brief	Releases signal handlers that prevents signals that causes  termination 
*			of the thread. 
*/
void UnhookSignals()
{
	signal(SIGINT, 0);
	signal(SIGTERM, 0);
	signal(SIGABRT, 0);
	signal(SIGBREAK, 0);
}
