#pragma once

uint32 THREADCALL NpcThreadProc(void * lpParam /* CNpcThread ptr */);
uint32 THREADCALL ZoneEventThreadProc(void * lpParam /* = nullptr */);

typedef std::set<CNpc *> NpcSet;

class CNpc;
class CNpcThread  
{
public:
	CNpcThread();
	void AddNPC(CNpc * pNpc);
	void RemoveNPC(CNpc * pNpc);
	virtual ~CNpcThread();

public:
	NpcSet m_pNpcs;
<<<<<<< HEAD
	FastMutex m_lock;
=======
	std::recursive_mutex m_lock;
>>>>>>> koserver2
	Thread m_thread;
};
