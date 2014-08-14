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
<<<<<<< HEAD
#ifdef _WIN32
	signal(SIGBREAK, OnSignal);
#endif
=======
	signal(SIGBREAK, OnSignal);
>>>>>>> koserver2
}

void OnSignal(int s)
{
	switch (s)
	{
	case SIGINT:
	case SIGTERM:
	case SIGABRT:
<<<<<<< HEAD
#ifdef _WIN32
	case SIGBREAK:
#endif
=======
	case SIGBREAK:
>>>>>>> koserver2
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
<<<<<<< HEAD
#ifdef _WIN32
	signal(SIGBREAK, 0);
#endif
=======
	signal(SIGBREAK, 0);
>>>>>>> koserver2
}
