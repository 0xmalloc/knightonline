/*
 * Multiplatform Async Network Library
 * Copyright (c) 2007 Burlex
 *
 * ListenSocket<T>: Creates a socket listener on specified address and port,
 *                  requires Update() to be called every loop.
 *
*/

#pragma once
#ifdef CONFIG_USE_EPOLL

#include "SocketDefines.h"
#include <errno.h>

template<class T>
class ListenSocket : public ListenSocketBase
{
public:
	ListenSocket(SocketMgr * mgr, const char * ListenAddress, uint32 Port) 
		: ListenSocketBase(), m_socketMgr(mgr), m_bSuspended(false)
    {
		m_socket = socket(AF_INET, SOCK_STREAM, 0);
		SocketOps::ReuseAddr(m_socket);
		SocketOps::Nonblocking(m_socket);

		m_address.sin_family = AF_INET;
		m_address.sin_port = ntohs((u_short)Port);
		m_address.sin_addr.s_addr = htonl(INADDR_ANY);
		m_opened = false;

		if (strcmp(ListenAddress, "0.0.0.0"))
		{
			struct hostent * hostname = gethostbyname(ListenAddress);
			if (hostname != 0)
				memcpy(&m_address.sin_addr.s_addr, hostname->h_addr_list[0], hostname->h_length);
		}

		// bind.. well attempt to.
		int ret = bind(m_socket, (const sockaddr*)&m_address, sizeof(m_address));
		if (ret != 0)
		{
			printf("Bind unsuccessful on port %u.\n", Port);
			return;
		}

		ret = listen(m_socket, 5);
		if (ret != 0) 
		{
			printf("Unable to listen on port %u.\n", Port);
			return;
		}

		len = sizeof(sockaddr_in);
		m_opened = true;
		mgr->AddListenSocket(this);
	}

	~ListenSocket()
	{
		if (m_opened)
			SocketOps::CloseSocket(m_socket);
	}

	bool run() {}
	void suspend() { m_bSuspended = true; }
	void resume() { m_bSuspended = false; }

	void Close()
	{
		if (m_opened)
			SocketOps::CloseSocket(m_socket);
		m_opened = false;
	}

	void OnAccept()
	{
		SOCKET aSocket = accept(m_socket, (sockaddr*)&m_tempAddress, (socklen_t*)&len);
		if (aSocket == -1)
			return;

		// If we're not accepting new connections, just drop the connection.
		if (m_bSuspended)
		{
			SocketOps::CloseSocket(aSocket);
			return;
		}

		Socket * socket = m_socketMgr->AssignSocket(aSocket);
		if (socket == nullptr)
		{
			SocketOps::CloseSocket(aSocket);
			return;
		}

		socket->Accept(&m_tempAddress);
	}

	INLINE bool IsOpen() { return m_opened; }
	SOCKET GetFd() { return m_socket; }

private:
	SocketMgr * m_socketMgr;
	SOCKET m_socket;
	struct sockaddr_in m_address;
	struct sockaddr_in m_tempAddress;
	bool m_opened;
	uint32 len;
	bool m_bSuspended;
};

#endif
