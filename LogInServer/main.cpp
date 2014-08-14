#include "stdafx.h"
#include "../shared/signal_handler.h"

LoginServer * g_pMain;
static Condition s_hEvent;
bool g_bRunning = true;

<<<<<<< HEAD
#ifdef WIN32
BOOL WINAPI _ConsoleHandler(DWORD dwCtrlType);
#endif
=======
BOOL WINAPI _ConsoleHandler(DWORD dwCtrlType);
>>>>>>> koserver2

int main()
{
	SetConsoleTitle("LogIn Server for Knight Online v" STRINGIFY(__VERSION));

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

	g_pMain = new LoginServer();

	// Startup server
	if (g_pMain->Startup())
	{
		printf("\nServer started up successfully!\n\n");

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

	g_bRunning = false; 
	delete g_pMain;
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
