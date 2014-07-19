#include "stdafx.h"
#include "Mutex.h"
#include "Condition.h"

Condition::Condition() : m_nLockCount(0)
{
}

void Condition::BeginSynchronized()
{
#ifdef USE_STD_CONDITION_VARIABLE
	m_lock.lock();
#else
	m_lock.Acquire();
#endif
	++m_nLockCount;
}

void Condition::EndSynchronized()
{
	--m_nLockCount;
#ifdef USE_STD_CONDITION_VARIABLE
	m_lock.unlock();
#else
	m_lock.Release();
#endif
}

uint32 Condition::Wait(time_t timeout)
{
#ifdef USE_STD_CONDITION_VARIABLE
	std::unique_lock<std::mutex> lock(m_lock);
	m_condition.wait_for(lock, std::chrono::milliseconds(timeout));
	return 0;
#else
	uint32 dwMillisecondsTimeout = (uint32)timeout * 1000;
	BOOL bAlertable = FALSE;
	ASSERT(LockHeldByCallingThread());

	// Enter a new event handle into the wait set.
	HANDLE hWaitEvent = Push();
	if( nullptr == hWaitEvent )
		return WAIT_FAILED;

	// Store the current lock count for re-acquisition.
	int nThisThreadsLockCount = m_nLockCount;
	m_nLockCount = 0;

	// Release the synchronization lock the appropriate number of times.
	// Win32 allows no error checking here.
	for( int i=0; i<nThisThreadsLockCount; ++i)
	{
		m_lock.Release();
	}

	// NOTE: Conceptually, releasing the lock and entering the wait
	// state is done in one atomic step. Technically, that is not
	// true here, because we first leave the critical section and
	// then, in a separate line of code, call WaitForSingleObjectEx.
	// The reason why this code is correct is that our thread is placed
	// in the wait set *before* the lock is released. Therefore, if
	// we get preempted right here and another thread notifies us, then
	// that notification will *not* be missed: the wait operation below
	// will find the event signalled.

	// Wait for the event to become signalled.
	DWORD dwWaitResult = WaitForSingleObjectEx(hWaitEvent, dwMillisecondsTimeout, bAlertable);

	// If the wait failed, store the last error because it will get
	// overwritten when acquiring the lock.
	DWORD dwLastError = 0;
	if( WAIT_FAILED == dwWaitResult )
		dwLastError = ::GetLastError();

	// Acquire the synchronization lock the appropriate number of times.
	// Win32 allows no error checking here.
	for( int j=0; j<nThisThreadsLockCount; ++j)
	{
		m_lock.Acquire();
	}

	// Restore lock count.
	m_nLockCount = nThisThreadsLockCount;

	// Close event handle
	if( ! CloseHandle(hWaitEvent) )
		return WAIT_FAILED;

	if( WAIT_FAILED == dwWaitResult )
		::SetLastError(dwLastError);

	return dwWaitResult;
#endif
}

uint32 Condition::Wait()
{
#ifdef USE_STD_CONDITION_VARIABLE
	std::unique_lock<std::mutex> lock(m_lock);
	m_condition.wait(lock);
	return 0;
#else
	DWORD dwMillisecondsTimeout = INFINITE;
	BOOL bAlertable = FALSE;
	ASSERT(LockHeldByCallingThread());

	// Enter a new event handle into the wait set.
	HANDLE hWaitEvent = Push();
	if( nullptr == hWaitEvent )
		return WAIT_FAILED;

	// Store the current lock count for re-acquisition.
	int nThisThreadsLockCount = m_nLockCount;
	m_nLockCount = 0;

	// Release the synchronization lock the appropriate number of times.
	// Win32 allows no error checking here.
	for( int i=0; i<nThisThreadsLockCount; ++i)
	{
		m_lock.Release();
	}

	// NOTE: Conceptually, releasing the lock and entering the wait
	// state is done in one atomic step. Technically, that is not
	// true here, because we first leave the critical section and
	// then, in a separate line of code, call WaitForSingleObjectEx.
	// The reason why this code is correct is that our thread is placed
	// in the wait set *before* the lock is released. Therefore, if
	// we get preempted right here and another thread notifies us, then
	// that notification will *not* be missed: the wait operation below
	// will find the event signalled.

	// Wait for the event to become signalled.
	DWORD dwWaitResult = ::WaitForSingleObjectEx(
		hWaitEvent,
		dwMillisecondsTimeout,
		bAlertable
		);

	// If the wait failed, store the last error because it will get
	// overwritten when acquiring the lock.
	DWORD dwLastError = 0;
	if( WAIT_FAILED == dwWaitResult )
		dwLastError = ::GetLastError();

	// Acquire the synchronization lock the appropriate number of times.
	// Win32 allows no error checking here.
	for( int j=0; j<nThisThreadsLockCount; ++j)
	{
		m_lock.Acquire();
	}

	// Restore lock count.
	m_nLockCount = nThisThreadsLockCount;

	// Close event handle
	if( ! CloseHandle(hWaitEvent) )
		return WAIT_FAILED;

	if( WAIT_FAILED == dwWaitResult )
		::SetLastError(dwLastError);

	return dwWaitResult;
#endif
}

void Condition::Signal()
{
#ifdef USE_STD_CONDITION_VARIABLE
	m_condition.notify_one();
#else
	// Pop the first handle, if any, off the wait set.
	HANDLE hWaitEvent = Pop();

	// If there is not thread currently waiting, that's just fine.
	if(nullptr == hWaitEvent)
		return;

	// Signal the event.
	SetEvent(hWaitEvent);
#endif
}

void Condition::Broadcast()
{
#ifdef USE_STD_CONDITION_VARIABLE
	m_condition.notify_all();
#else
	// Signal all events on the deque, then clear it. Win32 allows no
	// error checking on entering and leaving the critical section.
	Guard<Mutex> lock(m_critsecWaitSetProtection);
	std::deque<HANDLE>::const_iterator it_run = m_deqWaitSet.begin();
	std::deque<HANDLE>::const_iterator it_end = m_deqWaitSet.end();
	for( ; it_run < it_end; ++it_run )
	{
		if( ! SetEvent(*it_run) )
			return;
	}
	m_deqWaitSet.clear();
#endif
}

#ifndef USE_STD_CONDITION_VARIABLE
HANDLE Condition::Push()
{
	// Create the new event.
	HANDLE hWaitEvent = CreateEvent(
		nullptr, // no security
		FALSE, // auto-reset event
		FALSE, // initially unsignalled
		nullptr // string name
		);
	//
	if( nullptr == hWaitEvent ) {
		return nullptr;
	}

	// Push the handle on the deque.
	m_critsecWaitSetProtection.Acquire();
	m_deqWaitSet.push_back(hWaitEvent);
	m_critsecWaitSetProtection.Release();

	return hWaitEvent;
}

HANDLE Condition::Pop()
{
	// Pop the first handle off the deque.
	//
	m_critsecWaitSetProtection.Acquire();
	HANDLE hWaitEvent = nullptr; 
	if( 0 != m_deqWaitSet.size() )
	{
		hWaitEvent = m_deqWaitSet.front();
		m_deqWaitSet.pop_front();
	}
	m_critsecWaitSetProtection.Release();

	return hWaitEvent;
}
#endif

bool Condition::LockHeldByCallingThread()
{
#ifdef USE_STD_CONDITION_VARIABLE
	// If we didn't get the lock, someone else has it.
	if (!m_lock.try_lock())
		return false;

	// If we got the lock, but the lock count is zero, then nobody had it.
	if (0 == m_nLockCount)
	{
		m_lock.unlock();
		return false;
	}

	// Release lock once. NOTE: we still have it after this release.
	// Win32 allows no error checking here.
	m_lock.unlock();
	return true;
#else
	// If we didn't get the lock, someone else has it.
	if (!m_lock.AttemptAcquire())
		return false;

#if 0
	// If we got the lock, but the lock count is zero, then nobody had it.
	if (0 == m_nLockCount)
	{
		m_lock.Release();
		return false;
	}
#endif

	// Release lock once. NOTE: we still have it after this release.
	// Win32 allows no error checking here.
	m_lock.Release();
	return true;
#endif
}

Condition::~Condition()
{
}
