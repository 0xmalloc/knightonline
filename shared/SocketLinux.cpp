/*
 * Multiplatform Async Network Library
 * Copyright (c) 2007 Burlex
 * Hacked up to no good end by twostars (if you're looking for someone to blame!).
 *
 * SocketLinux.cpp - Linux-specific functions of Socket class are located here.
 *
 */

#include "stdafx.h"
#ifdef CONFIG_USE_EPOLL

void Socket::PostEvent(uint32 events)
{
	SOCKET epoll_fd = GetSocketMgr()->GetEpollFd();

	struct epoll_event ev = {0};
	ev.data.fd = m_fd;
	ev.events = events | EPOLLET;			/* use edge-triggered instead of level-triggered because we're using nonblocking sockets */

	// post actual event
	if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, ev.data.fd, &ev))
		printf("epoll warning: Could not post event on fd %u\n", m_fd);
}

void Socket::ReadCallback(uint32 len)
{
	if (IsDeleted() || !IsConnected())
		return;

	// We have to lock here.
	FastGuard lock(m_readMutex);

	size_t space = readBuffer.GetSpace();
	int bytes = recv(m_fd, readBuffer.GetBuffer(), space, 0);
	if (bytes <= 0)
	{
		Disconnect();
		return;
	}

	readBuffer.IncrementWritten(bytes);
	OnRead();
}

void Socket::WriteCallback()
{
	if (IsDeleted() || !IsConnected())
		return;

	// We should already be locked at this point, so try to push everything out.
	int bytes_written = send(m_fd, writeBuffer.GetBufferStart(), writeBuffer.GetContiguousBytes(), 0);
	if (bytes_written < 0)
	{
		// error.
		Disconnect();
		return;
	}

	writeBuffer.Remove(bytes_written);
}

void Socket::BurstPush()
{
	if (AcquireSendLock())
		PostEvent(EPOLLOUT);
}

#endif
