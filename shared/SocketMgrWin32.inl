#ifdef CONFIG_USE_IOCP

	INLINE HANDLE GetCompletionPort() { return m_completionPort; }
	INLINE void SetCompletionPort(HANDLE cp) { m_completionPort = cp; }
	void CreateCompletionPort();

	static void SetupWinsock();
	static void CleanupWinsock();
	
	static uint32 THREADCALL SocketWorkerThread(void * lpParam);

	HANDLE m_completionPort;

#endif