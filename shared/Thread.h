#pragma once

typedef uint32 (THREADCALL lpfnThreadFunc)(void *);

class Thread
{
public:
	Thread();
	Thread(lpfnThreadFunc lpThreadFunc, void * lpParam = nullptr);

#ifdef USE_STD_THREAD
	INLINE bool isStarted() { return m_thread.joinable(); }
#else
	INLINE bool isStarted() { return m_thread != nullptr; }
#endif

	bool start(lpfnThreadFunc lpThreadFunc, void * lpParam = nullptr);
	bool waitForExit();

	void suspend();
	void resume();

	virtual ~Thread() {}

protected:
#ifdef USE_STD_THREAD
	std::thread m_thread;
#else
	HANDLE m_thread;
#endif

private:
	Thread(const Thread & other); /* disable copy constructor */
};
