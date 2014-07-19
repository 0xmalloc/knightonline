#ifdef CONFIG_USE_EPOLL

	static uint32 THREADCALL SocketWorkerThread(void * lpParam);

	// add a new socket to the epoll set and to the fd mapping
	void AddSocket(Socket * s);
	void AddListenSocket(ListenSocketBase * s);

	// remove a socket from epoll set/fd mapping
	void RemoveSocket(Socket * s);

	void CloseAll(); /* need to fix this up */

	// returns epoll fd
	INLINE int GetEpollFd() { return m_epollFd; }

	// /dev/epoll instance handle
	SOCKET m_epollFd;

	// fd -> pointer binding.
	typedef std::map<SOCKET, Socket *> SocketMap;
	typedef std::map<SOCKET, ListenSocketBase *> ListenSocketMap;

	SocketMap m_sockets;
	ListenSocketMap m_listenSockets;
	FastMutex m_socketLock;

#endif
