#pragma once

class DatabaseThread
{
public:
	// Startup the database threads
	static void Startup(uint32 dwThreads);

	// Add to the queue and notify threads of activity.
	static void AddRequest(Packet * pkt);

	// Main thread procedure
	static uint32 THREADCALL ThreadProc(void * lpParam);

	// Shutdown threads.
	static void Shutdown();
};