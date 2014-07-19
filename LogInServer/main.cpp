#include "stdafx.h"
#include "../shared/signal_handler.h"

LoginServer * g_pMain;
static Condition s_hEvent;
bool g_bRunning = true;

#ifdef WIN32
BOOL WINAPI _ConsoleHandler(DWORD dwCtrlType);
#endif

int main()
{
	SetConsoleTitle("LogIn Server for Knight Online v" STRINGIFY(__VERSION));

#ifdef WIN32
	// Override the console handler
	SetConsoleCtrlHandler(_ConsoleHandler, TRUE);
#endif

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
#ifdef WIN32
		system("pause");
#endif
	}

	printf("Server shutting down, please wait...\n");

	g_bRunning = false; 
	delete g_pMain;
	UnhookSignals();

	return 0;
}

#ifdef WIN32
BOOL WINAPI _ConsoleHandler(DWORD dwCtrlType)
{
	s_hEvent.BeginSynchronized();
	s_hEvent.Signal();
	s_hEvent.EndSynchronized();
	sleep(10000); // Win7 onwards allows 10 seconds before it'll forcibly terminate
	return TRUE;
}
#endif