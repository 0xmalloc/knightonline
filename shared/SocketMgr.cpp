#include "stdafx.h"
#include "SocketMgr.h"

bool SocketMgr::s_bRunningCleanupThread = true;
<<<<<<< HEAD
FastMutex SocketMgr::s_disconnectionQueueLock;
=======
std::recursive_mutex SocketMgr::s_disconnectionQueueLock;
>>>>>>> koserver2
std::queue<Socket *> SocketMgr::s_disconnectionQueue;

Thread SocketMgr::s_cleanupThread; 
Atomic<uint32> SocketMgr::s_refCounter;

uint32 THREADCALL SocketCleanupThread(void * lpParam)
{
	while (SocketMgr::s_bRunningCleanupThread)
	{
<<<<<<< HEAD
		SocketMgr::s_disconnectionQueueLock.Acquire();
=======
		SocketMgr::s_disconnectionQueueLock.lock();
>>>>>>> koserver2
		while (!SocketMgr::s_disconnectionQueue.empty())
		{
			Socket *pSock = SocketMgr::s_disconnectionQueue.front();
			if (pSock->GetSocketMgr())
				pSock->GetSocketMgr()->DisconnectCallback(pSock);
			SocketMgr::s_disconnectionQueue.pop();
		}
<<<<<<< HEAD
		SocketMgr::s_disconnectionQueueLock.Release();
=======
		SocketMgr::s_disconnectionQueueLock.unlock();
>>>>>>> koserver2
		sleep(100);
	}

	return 0;
}

SocketMgr::SocketMgr() : m_threadCount(0), 
	m_bWorkerThreadsActive(false),
	m_bShutdown(false)
{
	static bool bRefCounterInitialised = false;
	if (!bRefCounterInitialised)
	{
		s_refCounter = 0;
		bRefCounterInitialised = true;
	}

	IncRef();
	Initialise();
}

void SocketMgr::SpawnWorkerThreads()
{
	if (m_bWorkerThreadsActive)
		return;

<<<<<<< HEAD
	m_threadCount = GetPreferredThreadCount(); // IOCP is better off multithreaded, epoll and kqueue are not
	m_bWorkerThreadsActive = true;

	TRACE("SocketMgr - spawning %u worker threads.\n", m_threadCount);

	for (long x = 0; x < m_threadCount; x++)
		m_threads.push_back(new Thread(SocketWorkerThread, static_cast<void *>(this)));
=======
	m_bWorkerThreadsActive = true;
	m_thread = new Thread(SocketWorkerThread, this);
>>>>>>> koserver2

	if (!s_cleanupThread.isStarted())
		s_cleanupThread.start(SocketCleanupThread);
}

<<<<<<< HEAD
=======

uint32 THREADCALL SocketMgr::SocketWorkerThread(void * lpParam)
{
	SocketMgr *socketMgr = (SocketMgr *)lpParam;
	HANDLE cp = socketMgr->GetCompletionPort();
	DWORD len;
	Socket * s = nullptr;
	OverlappedStruct * ov = nullptr;
	LPOVERLAPPED ol_ptr;

	while (socketMgr->m_bWorkerThreadsActive)
	{
		if (!GetQueuedCompletionStatus(cp, &len, (LPDWORD)&s, &ol_ptr, INFINITE))
		{
			if (s != nullptr)
				s->Disconnect();
			continue;
		}

		ov = CONTAINING_RECORD(ol_ptr, OverlappedStruct, m_overlap);

		if (ov->m_event == SOCKET_IO_THREAD_SHUTDOWN)
		{
			delete ov;
			return 0;
		}

		if (ov->m_event < NUM_SOCKET_IO_EVENTS)
			ophandlers[ov->m_event](s, len);
	}

	return 0;
}

void SocketMgr::Initialise()
{
	m_completionPort = nullptr;
}

void SocketMgr::CreateCompletionPort()
{
	SetCompletionPort(CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, (ULONG_PTR)0, 0));
}

void SocketMgr::SetupWinsock()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,0), &wsaData);
}

void HandleReadComplete(Socket * s, uint32 len)
{
	if (s->IsDeleted())
		return;

	s->m_readEvent.Unmark();
	if (len)
	{
		s->GetReadBuffer().IncrementWritten(len);
		s->OnRead();
		s->SetupReadEvent();
	}
	else
	{
		// s->Delete();	  // Queue deletion.
		s->Disconnect();
	}
}

void HandleWriteComplete(Socket * s, uint32 len)
{
	if (s->IsDeleted())
		return;

	s->m_writeEvent.Unmark();
	s->BurstBegin();					// Lock
	s->GetWriteBuffer().Remove(len);
	if( s->GetWriteBuffer().GetContiguousBytes() > 0 )
		s->WriteCallback();
	else
		s->DecSendLock();
	s->BurstEnd();					  // Unlock
}

void HandleShutdown(Socket * s, uint32 len) {}

>>>>>>> koserver2
void SocketMgr::OnConnect(Socket *pSock) {}
void SocketMgr::DisconnectCallback(Socket *pSock) {}
void SocketMgr::OnDisconnect(Socket *pSock) 
{
<<<<<<< HEAD
	FastGuard lock(s_disconnectionQueueLock);
=======
	Guard lock(s_disconnectionQueueLock);
>>>>>>> koserver2
	s_disconnectionQueue.push(pSock);
}

void SocketMgr::ShutdownThreads()
{
<<<<<<< HEAD
#ifdef CONFIG_USE_IOCP
	for (long i = 0; i < m_threadCount; i++)
	{
		OverlappedStruct * ov = new OverlappedStruct(SOCKET_IO_THREAD_SHUTDOWN);
		PostQueuedCompletionStatus(m_completionPort, 0, (ULONG_PTR)0, &ov->m_overlap);
	}
#endif

	m_bWorkerThreadsActive = false;

	foreach (itr, m_threads)
	{
		(*itr)->waitForExit();
		delete (*itr);
	}
=======
	OverlappedStruct * ov = new OverlappedStruct(SOCKET_IO_THREAD_SHUTDOWN);
	PostQueuedCompletionStatus(m_completionPort, 0, (ULONG_PTR)0, &ov->m_overlap);

	m_bWorkerThreadsActive = false;

	m_thread->waitForExit();
	delete m_thread;
>>>>>>> koserver2
}

void SocketMgr::Shutdown()
{
	if (m_bShutdown)
		return;

	ShutdownThreads();
<<<<<<< HEAD
	_CleanupSockets(); /* cleanup implementation specific sockets */
=======
>>>>>>> koserver2

	DecRef();
	m_bShutdown = true;
}

void SocketMgr::SetupSockets()
{
<<<<<<< HEAD
#ifdef CONFIG_USE_IOCP
	SetupWinsock();
#endif
=======
	SetupWinsock();
>>>>>>> koserver2
}

void SocketMgr::CleanupSockets()
{
	if (s_cleanupThread.isStarted())
	{
		s_bRunningCleanupThread = false;
		s_cleanupThread.waitForExit();
	}

<<<<<<< HEAD
#ifdef CONFIG_USE_IOCP
	WSACleanup();
#endif
=======
	WSACleanup();
>>>>>>> koserver2
}

SocketMgr::~SocketMgr()
{
	Shutdown();
}
