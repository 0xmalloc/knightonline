#include "stdafx.h"
#include "../shared/signal_handler.h"

CServerDlg * g_pMain;
static Condition s_hEvent;

<<<<<<< HEAD
#ifdef WIN32
BOOL WINAPI _ConsoleHandler(DWORD dwCtrlType);
#endif
=======
BOOL WINAPI _ConsoleHandler(DWORD dwCtrlType);
>>>>>>> koserver2

bool g_bRunning = true;

int main()
{
	SetConsoleTitle("AI Server for Knight Online v" STRINGIFY(__VERSION));

<<<<<<< HEAD
#ifdef WIN32
	// Override the console handler
	SetConsoleCtrlHandler(_ConsoleHandler, TRUE);
#endif
=======
	// Override the console handler
	SetConsoleCtrlHandler(_ConsoleHandler, TRUE);
>>>>>>> koserver2

	HookSignals(&s_hEvent);

	// Start up the time updater thread
	StartTimeThread();

	g_pMain = new CServerDlg();

	// Startup server
	if (g_pMain->Startup())
	{
		printf("\nServer started up successfully!\n");

		// Wait until console's signaled as closing
		s_hEvent.Wait();
	}
	else
	{
<<<<<<< HEAD
#ifdef WIN32
		system("pause");
#endif
=======
		system("pause");
>>>>>>> koserver2
	}

	printf("Server shutting down, please wait...\n");

	// This seems redundant, but it's not. 
	// We still have the destructor for the dialog instance, which allows time for threads to properly cleanup.
	g_bRunning = false;

	delete g_pMain;

	CleanupTimeThread();
	UnhookSignals();

	return 0;
}

<<<<<<< HEAD
#ifdef WIN32
=======
>>>>>>> koserver2
BOOL WINAPI _ConsoleHandler(DWORD dwCtrlType)
{
	s_hEvent.BeginSynchronized();
	s_hEvent.Signal();
	s_hEvent.EndSynchronized();
	sleep(10000); // Win7 onwards allows 10 seconds before it'll forcibly terminate
	return TRUE;
}
<<<<<<< HEAD
#endif
=======
>>>>>>> koserver2
