#pragma once

#include <queue>
#include <set>
#include <map>
#include "Socket.h"

uint32 THREADCALL SocketCleanupThread(void * lpParam);

#ifndef CONFIG_IOCP
class ListenSocketBase
{
public:
	virtual ~ListenSocketBase() {}
	virtual void OnAccept() = 0;
	virtual SOCKET GetFd() = 0;
};
#endif

class SocketMgr
{
public:
	SocketMgr();

	void Initialise();

	uint32 GetPreferredThreadCount();
	void SpawnWorkerThreads();
	void ShutdownThreads();

	static void SetupSockets();
	static void CleanupSockets();

#if defined(CONFIG_USE_IOCP)
#	include "SocketMgrWin32.inl"
#elif defined(CONFIG_USE_EPOLL)
#	include "SocketMgrLinux.inl"
#elif defined(CONFIG_USE_KQUEUE)
#	include "SocketMgrBSD.inl"
#endif

	virtual Socket *AssignSocket(SOCKET socket) = 0;
	virtual void OnConnect(Socket *pSock);
	virtual void OnDisconnect(Socket *pSock);
	virtual void DisconnectCallback(Socket *pSock);
	virtual void _CleanupSockets();
	virtual void Shutdown();
	virtual ~SocketMgr();

	static FastMutex s_disconnectionQueueLock;
	static std::queue<Socket *> s_disconnectionQueue;

protected:
	bool m_bShutdown;

	std::vector<Thread *> m_threads;
	static Thread s_cleanupThread;

	long m_threadCount;
	bool m_bWorkerThreadsActive;

	INLINE void IncRef() { if (s_refCounter.increment() == 1) SetupSockets(); }
	INLINE void DecRef() { if (s_refCounter.decrement() == 0) CleanupSockets(); }

	// reference counter (one app can hold multiple socket manager instances)
	static Atomic<uint32> s_refCounter;

public:
	static bool s_bRunningCleanupThread;
};
