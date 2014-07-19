#include "stdafx.h"

/*
NOTE: 
std::thread allow for threads to be spawned with a variety of 
different function/method signatures.

For backwards compatibility, we'll enforce the use of the default
single parameter signature (we don't really need more than that).
*/

Thread::Thread()
#ifndef USE_STD_THREAD
	: m_thread(nullptr)
#endif
{
}

Thread::Thread(lpfnThreadFunc lpThreadFunc, void * lpParam /*= nullptr*/)
#ifndef USE_STD_THREAD
	: m_thread(nullptr)
#endif
{
	start(lpThreadFunc, lpParam);
}

#ifdef USE_STD_THREAD
bool Thread::start(lpfnThreadFunc lpThreadFunc, void * lpParam /*= nullptr*/)
{
	try
	{
		m_thread = std::thread(lpThreadFunc, lpParam);
		return true;
	}
	catch (std::exception & ex)
	{
#ifdef _DEBUG
		printf("Caught thread exception: %s\n", ex.what());
		ASSERT(0); /* fix me */
#endif
		return false;
	}
}

bool Thread::waitForExit()
{
	try
	{
		// Ensure it's started & hasn't exited yet.
		if (m_thread.joinable())
			m_thread.join();

		return true;
	}
	catch (std::exception & ex)
	{
#ifdef _DEBUG
		printf("Caught thread exception: %s\n", ex.what());
		ASSERT(0); /* fix me */
#endif
		return false;
	}
}

#else // not using std::thread, no C++11 support.

bool Thread::start(lpfnThreadFunc lpThreadFunc, void * lpParam /*= nullptr*/)
{
	DWORD dwThreadId;
	m_thread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE) lpThreadFunc, lpParam, 0, &dwThreadId);
	return (m_thread != nullptr);
}

bool Thread::waitForExit()
{
	return WaitForSingleObject(m_thread, INFINITE) != WAIT_OBJECT_0;
}
#endif

void Thread::suspend()
{
	/* no native functionality on *nix for this, it needs to be handled in the thread */
#ifdef WIN32 
#	ifdef USE_STD_THREAD
	SuspendThread(m_thread.native_handle());
#	else
	SuspendThread(m_thread);
#	endif
#endif
}

void Thread::resume()
{
	/* no native functionality on *nix for this, it needs to be handled in the thread */
#ifdef WIN32 
#	ifdef USE_STD_THREAD
	ResumeThread(m_thread.native_handle());
#	else
	ResumeThread(m_thread);
#	endif
#endif
}
