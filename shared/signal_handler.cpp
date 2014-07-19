#include "stdafx.h"
#include "Condition.h"
#include "signal_handler.h"
#include <signal.h>

Condition * g_hNotifier;
void HookSignals(Condition * notifier)
{
	g_hNotifier = notifier;

	signal(SIGINT, OnSignal);
	signal(SIGTERM, OnSignal);
	signal(SIGABRT, OnSignal);
#ifdef _WIN32
	signal(SIGBREAK, OnSignal);
#endif
}

void OnSignal(int s)
{
	switch (s)
	{
	case SIGINT:
	case SIGTERM:
	case SIGABRT:
#ifdef _WIN32
	case SIGBREAK:
#endif
		g_hNotifier->BeginSynchronized();
		g_hNotifier->Signal();
		g_hNotifier->EndSynchronized();
		break;
	}

	signal(s, OnSignal);
}

void UnhookSignals()
{
	signal(SIGINT, 0);
	signal(SIGTERM, 0);
	signal(SIGABRT, 0);
#ifdef _WIN32
	signal(SIGBREAK, 0);
#endif
}
