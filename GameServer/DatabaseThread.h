#pragma once

class DatabaseThread
{
public:
	// Startup the database threads
<<<<<<< HEAD
	static void Startup(uint32 dwThreads);
=======
	static void Startup();
>>>>>>> koserver2

	// Add to the queue and notify threads of activity.
	static void AddRequest(Packet * pkt);

	// Main thread procedure
	static uint32 THREADCALL ThreadProc(void * lpParam);

	// Shutdown threads.
	static void Shutdown();
};