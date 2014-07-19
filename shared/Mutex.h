#pragma once

#ifdef USE_STD_MUTEX
#	include <mutex>
#else // if not using std::mutex, we're limited to Windows code.
#	include <WinBase.h>
#endif

class Mutex
{
public:
	friend class Condition;

	/** Initializes a mutex class, with InitializeCriticalSection / pthread_mutex_init
	*/
	Mutex();

	/** Deletes the associated critical section / mutex
	*/
	~Mutex();

	/** Acquires this mutex. If it cannot be acquired immediately, it will block.
	*/
	void Acquire();

	/** Releases this mutex. No error checking performed
	*/
	void Release();

	/** Attempts to acquire this mutex. If it cannot be acquired (held by another thread)
	* it will return false.
	* @return false if cannot be acquired, true if it was acquired.
	*/
	bool AttemptAcquire();

protected:
#ifdef USE_STD_MUTEX
	std::recursive_mutex lock;
#else
	/** Critical section used for system calls
	*/
	CRITICAL_SECTION cs;
#endif
};

#ifdef WIN32
class FastMutex
{
#pragma pack(push,8)
	volatile long m_lock;
#pragma pack(pop)
	DWORD m_recursiveCount;

public:
	FastMutex() : m_lock(0), m_recursiveCount(0) {}
	~FastMutex() {}

	void Acquire();
	bool AttemptAcquire();
	void Release();
};
#else // Windows functionality can provide a slightly faster lock.
// For all other OS', the default Mutex implementation is sufficient.
#	define FastMutex Mutex
#endif

template <class T>
class Guard
{
public:
	Guard(T& mutex) : target(mutex)
	{
		target.Acquire();
	}

	Guard(T* mutex) : target(*mutex)
	{
		target.Acquire();
	}

	~Guard()
	{
		target.Release();
	}

	Guard& operator=(Guard& src)
	{
		this->target = src.target;
		return *this;
	}

protected:
	T& target;
};

typedef Guard<FastMutex> FastGuard;