/*
 * Multiplatform Async Network Library
 * Copyright (c) 2007 Burlex
 * Hacked up to no good end by twostars (if you're looking for someone to blame!).
 *
 * SocketMgr - epoll manager for Linux.
 *
 */

#include "stdafx.h"
#ifdef CONFIG_USE_EPOLL

void SocketMgr::Initialise()
{
	// constructor > create epoll device handle + initialize event set
	m_epollFd  = epoll_create(SOCKET_HOLDER_SIZE);
	if (m_epollFd == -1)
	{
		printf("Could not create epoll fd (/dev/epoll).\n");
		exit(-1);
	}
}

uint32 SocketMgr::GetPreferredThreadCount() { return 1; }

void SocketMgr::AddSocket(Socket * s)
{
	FastGuard lock(m_socketLock);
	auto itr = m_sockets.find(s->GetFd());

	if (itr != m_sockets.end())
	{
		s->Delete();
		return;
	}

	m_sockets[s->GetFd()] = s;

	// Add epoll event based on socket activity.
	struct epoll_event ev = {0};
	ev.events = (s->GetWriteBuffer().GetSize()) ? EPOLLOUT : EPOLLIN;
	ev.events |= EPOLLET;			/* use edge-triggered instead of level-triggered because we're using nonblocking sockets */
	ev.data.fd = s->GetFd();

	if (epoll_ctl(m_epollFd, EPOLL_CTL_ADD, ev.data.fd, &ev))
		printf("epoll warning: Could not add event to epoll set on fd %u\n", ev.data.fd);
}

void SocketMgr::AddListenSocket(ListenSocketBase * s)
{
	FastGuard lock(m_socketLock);

	auto itr = m_listenSockets.find(s->GetFd());
	ASSERT(itr == m_listenSockets.end());
	m_listenSockets[s->GetFd()] = s;

	// Add epoll event based on socket activity.
	struct epoll_event ev = {0};
	ev.events = EPOLLIN;
	ev.events |= EPOLLET;			/* use edge-triggered instead of level-triggered because we're using nonblocking sockets */
	ev.data.fd = s->GetFd();

	if (epoll_ctl(m_epollFd, EPOLL_CTL_ADD, ev.data.fd, &ev))
		printf("epoll warning: Could not add event to epoll set on fd %u\n", ev.data.fd);
}

void SocketMgr::RemoveSocket(Socket * s)
{
	FastGuard lock(m_socketLock);

	auto itr = m_sockets.find(s->GetFd());
	if (itr == m_sockets.end() 
		|| itr->second != s)
	{
		printf("epoll warning: Could not remove fd %u from the set as it doesn't exist?\n", s->GetFd());
		return;
	}


	m_sockets.erase(itr);

	// Remove from epoll list.
	struct epoll_event ev = {0};
	ev.data.fd = s->GetFd();
	ev.events = EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLHUP | EPOLLONESHOT;

	if (epoll_ctl(m_epollFd, EPOLL_CTL_DEL, ev.data.fd, &ev))
		printf("epoll warning: Could not remove fd %u from epoll set, errno %u", s->GetFd(), errno);
}

void SocketMgr::CloseAll()
{
	FastGuard lock(m_socketLock);
	foreach (itr, m_sockets)
		itr->second->Delete();
}

void SocketMgr::_CleanupSockets()
{
	close(m_epollFd);
}

uint32 THREADCALL SocketMgr::SocketWorkerThread(void * lpParam)
{
	// epoll event struct
	struct epoll_event events[THREAD_EVENT_SIZE]; /* that stack... */

	SocketMgr * mgr = static_cast<SocketMgr *>(lpParam);
	while (mgr->m_bWorkerThreadsActive)
	{
		int fd_count = epoll_wait(mgr->m_epollFd, events, THREAD_EVENT_SIZE, EPOLL_WAIT_TIMEOUT);
		for (int i = 0; i < fd_count; ++i)
		{
			FastGuard socketLock(mgr->m_socketLock);

			if (events[i].data.fd >= SOCKET_HOLDER_SIZE)
			{
				printf("epoll warning: Requested FD that is too high (%u)\n", events[i].data.fd);
				continue;
			}

			// Does this socket exist in our socket map?
			auto itr = mgr->m_sockets.find(events[i].data.fd);
			if (itr == mgr->m_sockets.end())
			{
				auto itr2 = mgr->m_listenSockets.find(events[i].data.fd);
				if (itr2 == mgr->m_listenSockets.end())
					printf("epoll warning: Returned invalid fd (no pointer) of FD %u\n", events[i].data.fd);
				else
					itr2->second->OnAccept();

				continue;
			}

			Socket * ptr = itr->second;
			if (events[i].events & EPOLLHUP || events[i].events & EPOLLERR)
			{
				ptr->Disconnect();
				continue;
			}

			if (events[i].events & EPOLLIN)
			{
				ptr->ReadCallback(0);               // Len is unknown at this point.

				/* changing to written state? */
				if (ptr->GetWriteBuffer().GetSize() && !ptr->HasSendLock() && ptr->IsConnected())
					ptr->PostEvent(EPOLLOUT);
			}
			else if (events[i].events & EPOLLOUT)
			{
				ptr->BurstBegin();          // Lock receive mutex
				ptr->WriteCallback();       // Perform actual send()
				if (ptr->GetWriteBuffer().GetSize() <= 0)
				{
					/* change back to a read event */
					ptr->DecSendLock();
					ptr->PostEvent(EPOLLIN);
				}
				ptr->BurstEnd();            // Unlock
			 }
		}
	}

	return 0;
}

#endif
