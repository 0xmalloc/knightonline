#pragma once

#ifdef USE_STD_CONDITION_VARIABLE
#	include <condition_variable>
#else
#	include "Mutex.h"
#endif

#include <deque>

class Condition
{
public:
	Condition();
	~Condition();
	void BeginSynchronized();
	void EndSynchronized();
	uint32 Wait(time_t timeout);
	uint32 Wait();
	void Signal();
	void Broadcast();

private:
	bool LockHeldByCallingThread();

	int m_nLockCount;

#ifdef USE_STD_CONDITION_VARIABLE
	std::condition_variable m_condition;
	std::mutex m_lock;
#else
	Mutex m_lock;
	std::deque<HANDLE> m_deqWaitSet;
	Mutex m_critsecWaitSetProtection;

	HANDLE Push();
	HANDLE Pop();
#endif
};