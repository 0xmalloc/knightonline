#include "stdafx.h"
#include "Mutex.h"

#ifdef USE_STD_MUTEX
Mutex::Mutex() {}
Mutex::~Mutex() {}
void Mutex::Acquire() { lock.lock(); }
void Mutex::Release() { lock.unlock(); }
bool Mutex::AttemptAcquire() { return lock.try_lock(); }
#else
Mutex::Mutex() { InitializeCriticalSection(&cs); }
Mutex::~Mutex() { DeleteCriticalSection(&cs); }
void Mutex::Acquire() { EnterCriticalSection(&cs); }
void Mutex::Release() { LeaveCriticalSection(&cs); }
bool Mutex::AttemptAcquire() { return (TryEnterCriticalSection(&cs) == TRUE ? true : false); }
#endif

#ifdef WIN32
void FastMutex::Acquire()
{
	DWORD thread_id = GetCurrentThreadId(), owner;
	if (thread_id == (DWORD)m_lock)
	{
		++m_recursiveCount;
		return;
	}

	for (;;)
	{
		owner = InterlockedCompareExchange(&m_lock, thread_id, 0);
		if(owner == 0)
			break;

		Sleep(0);
	}

	++m_recursiveCount;
}

bool FastMutex::AttemptAcquire()
{
	DWORD thread_id = GetCurrentThreadId();
	if(thread_id == (DWORD)m_lock)
	{
		++m_recursiveCount;
		return true;
	}

	DWORD owner = InterlockedCompareExchange(&m_lock, thread_id, 0);
	if(owner == 0)
	{
		++m_recursiveCount;
		return true;
	}

	return false;
}

void FastMutex::Release()
{
	if ((--m_recursiveCount) == 0)
		InterlockedExchange(&m_lock, 0);
}
#endif