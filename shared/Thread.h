#pragma once

typedef uint32 (THREADCALL lpfnThreadFunc)(void *);

class Thread
{
public:
	Thread();
	Thread(lpfnThreadFunc lpThreadFunc, void * lpParam = nullptr);

<<<<<<< HEAD
#ifdef USE_STD_THREAD
	INLINE bool isStarted() { return m_thread.joinable(); }
#else
	INLINE bool isStarted() { return m_thread != nullptr; }
#endif
=======
	INLINE bool isStarted() { return m_thread.joinable(); }
>>>>>>> koserver2

	bool start(lpfnThreadFunc lpThreadFunc, void * lpParam = nullptr);
	bool waitForExit();

<<<<<<< HEAD
	void suspend();
	void resume();

	virtual ~Thread() {}

protected:
#ifdef USE_STD_THREAD
	std::thread m_thread;
#else
	HANDLE m_thread;
#endif
=======
	virtual ~Thread() {}

protected:
	std::thread m_thread;
>>>>>>> koserver2

private:
	Thread(const Thread & other); /* disable copy constructor */
};
