#pragma once

#include <queue>
#include <set>
#include <map>
#include "Socket.h"

uint32 THREADCALL SocketCleanupThread(void * lpParam);

<<<<<<< HEAD
#ifndef CONFIG_IOCP
class ListenSocketBase
{
public:
	virtual ~ListenSocketBase() {}
	virtual void OnAccept() = 0;
	virtual SOCKET GetFd() = 0;
};
#endif

=======
>>>>>>> koserver2
class SocketMgr
{
public:
	SocketMgr();

	void Initialise();

<<<<<<< HEAD
	uint32 GetPreferredThreadCount();
=======
>>>>>>> koserver2
	void SpawnWorkerThreads();
	void ShutdownThreads();

	static void SetupSockets();
	static void CleanupSockets();

<<<<<<< HEAD
#if defined(CONFIG_USE_IOCP)
#	include "SocketMgrWin32.inl"
#elif defined(CONFIG_USE_EPOLL)
#	include "SocketMgrLinux.inl"
#elif defined(CONFIG_USE_KQUEUE)
#	include "SocketMgrBSD.inl"
#endif
=======
	INLINE HANDLE GetCompletionPort() { return m_completionPort; }
	INLINE void SetCompletionPort(HANDLE cp) { m_completionPort = cp; }
	void CreateCompletionPort();

	static void SetupWinsock();
	static void CleanupWinsock();
	
	static uint32 THREADCALL SocketWorkerThread(void * lpParam);

	HANDLE m_completionPort;
>>>>>>> koserver2

	virtual Socket *AssignSocket(SOCKET socket) = 0;
	virtual void OnConnect(Socket *pSock);
	virtual void OnDisconnect(Socket *pSock);
	virtual void DisconnectCallback(Socket *pSock);
<<<<<<< HEAD
	virtual void _CleanupSockets();
	virtual void Shutdown();
	virtual ~SocketMgr();

	static FastMutex s_disconnectionQueueLock;
=======
	virtual void Shutdown();
	virtual ~SocketMgr();

	static std::recursive_mutex s_disconnectionQueueLock;
>>>>>>> koserver2
	static std::queue<Socket *> s_disconnectionQueue;

protected:
	bool m_bShutdown;

<<<<<<< HEAD
	std::vector<Thread *> m_threads;
=======
	Thread * m_thread;
>>>>>>> koserver2
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
<<<<<<< HEAD
=======

typedef void(*OperationHandler)(Socket * s, uint32 len);

void HandleReadComplete(Socket * s, uint32 len);
void HandleWriteComplete(Socket * s, uint32 len);
void HandleShutdown(Socket * s, uint32 len);

static OperationHandler ophandlers[] =
{
	&HandleReadComplete,
	&HandleWriteComplete,
	&HandleShutdown
};
>>>>>>> koserver2
