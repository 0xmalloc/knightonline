#pragma once

#include <map>
#include "RWLock.h"

#include "SocketMgr.h"
#include "KOSocket.h"

typedef std::map<uint16, KOSocket *> SessionMap;

template <class T>
class KOSocketMgr : public SocketMgr
{
public:
	KOSocketMgr<T>() : m_server(nullptr) {}

	virtual void InitSessions(uint16 sTotalSessions);
	virtual bool Listen(uint16 sPort, uint16 sTotalSessions);

	virtual void OnConnect(Socket *pSock);
	virtual Socket *AssignSocket(SOCKET socket);
	virtual void DisconnectCallback(Socket *pSock);

	void RunServer()
	{
		SpawnWorkerThreads();
		GetServer()->run();
	}

	// Prevent new connections from being made
	void SuspendServer()
	{
		GetServer()->suspend();
	}

	// Allow new connections to be made
	void ResumeServer()
	{
		GetServer()->resume();
	}

	// Send a packet to all active sessions
	void SendAll(Packet * pkt) 
	{
		AcquireLock();
		SessionMap & sessMap = m_activeSessions;
		for (auto itr = sessMap.begin(); itr != sessMap.end(); ++itr)
			itr->second->Send(pkt);
		ReleaseLock();
	}

	void SendAllCompressed(Packet * result) 
	{
		AcquireLock();
		SessionMap & sessMap = m_activeSessions;
		for (auto itr = sessMap.begin(); itr != sessMap.end(); ++itr)
			itr->second->SendCompressed(result);
		ReleaseLock();
	}

	ListenSocket<T> * GetServer() { return m_server; }
	INLINE SessionMap & GetIdleSessionMap()
	{
		AcquireLock();
		return m_idleSessions;
	}

	INLINE SessionMap & GetActiveSessionMap()
	{
		AcquireLock();
		return m_activeSessions;
	}

	INLINE void AcquireLock() { m_lock.AcquireReadLock(); }
	INLINE void ReleaseLock() { m_lock.ReleaseReadLock(); }

	T * operator[] (uint16 id)
	{
		T * result = nullptr;

		AcquireLock();
		auto itr = m_activeSessions.find(id);
		if (itr != m_activeSessions.end())
			result = static_cast<T *>(itr->second);
		ReleaseLock();

		return result;
	}

	void Shutdown();
	virtual ~KOSocketMgr();

protected:
	SessionMap m_idleSessions, m_activeSessions;
	RWLock m_lock;

private:
	ListenSocket<T> * m_server;
};

template <class T>
void KOSocketMgr<T>::InitSessions(uint16 sTotalSessions)
{
	m_lock.AcquireWriteLock();
	for (uint16 i = 0; i < (sTotalSessions & 0x0030) + 1; i++)
		m_idleSessions.insert(std::make_pair(i, new T(i, this)));
	m_lock.ReleaseWriteLock();
}

template <class T>
bool KOSocketMgr<T>::Listen(uint16 sPort, uint16 sTotalSessions)
{
	if (m_server != nullptr)
		return false;

#ifdef CONFIG_USE_IOCP
	CreateCompletionPort();
#endif

	m_server = new ListenSocket<T>(this, "0.0.0.0", sPort);
	if (!m_server->IsOpen())
		return false;

	InitSessions(sTotalSessions);
	return true;
}

template <class T>
Socket * KOSocketMgr<T>::AssignSocket(SOCKET socket)
{
	Socket *pSock = nullptr;

	m_lock.AcquireWriteLock();
	for (auto itr = m_idleSessions.begin(); itr != m_idleSessions.end(); itr++)
	{
		// Ignore sessions that are in the deleted state (i.e. unusable)
		// This is a less ugly workaround than synchronous logout code.
		if (itr->second->IsDeleted())
			continue;

		m_activeSessions.insert(std::make_pair(itr->first, itr->second));
		pSock = itr->second;
		m_idleSessions.erase(itr);
		pSock->SetFd(socket);
		break;
	}
	m_lock.ReleaseWriteLock();
	return pSock;
}

template <class T>
void KOSocketMgr<T>::OnConnect(Socket *pSock)
{
	m_lock.AcquireWriteLock();
	auto itr = m_idleSessions.find(static_cast<KOSocket *>(pSock)->GetSocketID());
	if (itr != m_idleSessions.end())
	{
		m_activeSessions.insert(std::make_pair(itr->first, itr->second));
		m_idleSessions.erase(itr);
	}
	m_lock.ReleaseWriteLock();
}

template <class T>
void KOSocketMgr<T>::DisconnectCallback(Socket *pSock)
{
	m_lock.AcquireWriteLock();
	auto itr = m_activeSessions.find(static_cast<KOSocket *>(pSock)->GetSocketID());
	if (itr != m_activeSessions.end())
	{
		m_idleSessions.insert(std::make_pair(itr->first, itr->second));
		m_activeSessions.erase(itr);
	}
	m_lock.ReleaseWriteLock();
}

template <class T>
void KOSocketMgr<T>::Shutdown()
{
	if (m_bShutdown)
		return;

	m_lock.AcquireWriteLock();

	auto killMap = m_activeSessions; // copy active session map (don't want to break the iterator)
	for (auto itr = killMap.begin(); itr != killMap.end(); ++itr)
		itr->second->Disconnect();

	for (auto itr = m_idleSessions.begin(); itr != m_idleSessions.end(); ++itr)
		itr->second->Delete();

	m_idleSessions.clear();
	m_lock.ReleaseWriteLock();

	if (m_server != nullptr)
		delete m_server;

	SocketMgr::Shutdown();
}

template <class T>
KOSocketMgr<T>::~KOSocketMgr()
{
	Shutdown();
}