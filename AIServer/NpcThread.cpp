#include "stdafx.h"
#include "NpcThread.h"
#include "Npc.h"

#define DELAY				250

//////////////////////////////////////////////////////////////////////
// NPC Thread Callback Function
//
uint32 THREADCALL NpcThreadProc(void * pParam /* CNpcThread ptr */)
{
	try
	{
		CNpcThread*	pInfo	= (CNpcThread *)pParam;
		CNpc*				pNpc	= nullptr;
		int					i			= 0;
		time_t				dwDiffTime	= 0, dwTickTime  = 0, fTime2 = 0;
		int    duration_damage=0;

		if(!pInfo) 
			return 0;

		while (!g_bNpcExit)
		{
			pInfo->m_lock.lock();
			fTime2 = getMSTime();

			foreach (itr, pInfo->m_pNpcs)
			{
				try
				{
					pNpc = *itr;

					if (pNpc == nullptr)
						continue;

					bool bDeleteNPC = false;

					dwTickTime = fTime2 - pNpc->m_fDelayTime;

					if (pNpc->m_Delay > (int)dwTickTime && !pNpc->m_bFirstLive && pNpc->m_Delay != 0) 
					{
						if (pNpc->m_Delay < 0) pNpc->m_Delay = 0;

						if (pNpc->m_NpcState == NPC_STANDING 
							&& pNpc->CheckFindEnemy()
							&& pNpc->FindEnemy())
						{
							pNpc->m_NpcState = NPC_ATTACKING;
							pNpc->m_Delay = 0;
						}
						continue;
					}	

					dwTickTime = fTime2 - pNpc->m_fHPChangeTime;
					if( 10000 < dwTickTime )	{	
						pNpc->HpChange();
					}

					uint8 bState = pNpc->m_NpcState;
					time_t tDelay = -1;
					switch (bState)
					{
					case NPC_LIVE:			
						tDelay = pNpc->NpcLive();
						break;

					case NPC_STANDING:		
						tDelay = pNpc->NpcStanding();
						break;

					case NPC_MOVING:
						tDelay = pNpc->NpcMoving();
						break;

					case NPC_ATTACKING:
						tDelay = pNpc->NpcAttacking();
						break;

					case NPC_TRACING:
						tDelay = pNpc->NpcTracing();
						break;

					case NPC_FIGHTING:
						tDelay = pNpc->Attack();
						break;

					case NPC_BACK:
						tDelay = pNpc->NpcBack();
						break;

					case NPC_STRATEGY:
						break;

					case NPC_DEAD:
						pNpc->m_NpcState = NPC_LIVE;
						break;

					case NPC_SLEEPING:
						tDelay = pNpc->NpcSleeping();
						break;

					case NPC_FAINTING:
						tDelay = pNpc->NpcFainting();
						break;

					case NPC_HEALING:
						tDelay = pNpc->NpcHealing();
						break;

					case NPC_CASTING:
						tDelay = pNpc->NpcCasting();
						break;
					}

					// This may not be necessary, but it keeps behaviour identical.
					if (bState != NPC_LIVE && bState != NPC_DEAD
						&& pNpc->m_NpcState != NPC_DEAD)
						pNpc->m_fDelayTime = getMSTime();

					if (tDelay >= 0)
						pNpc->m_Delay = tDelay;

					if (pNpc->m_bDelete)
						g_pMain->m_arNpc.DeleteData(pNpc->GetID());
				}
				catch (std::system_error & ex)
				{
					TRACE("### ERROR - NpcThread  - 1 : %s\n ###", ex.what());
					continue;
				}
			}

			pInfo->m_lock.unlock();
			sleep(DELAY);
		}
	}
	catch (std::system_error & ex)
	{
		TRACE("### ERROR - NpcThread - 2 : %s\n ###", ex.what());
	}
	return 0;
}

uint32 THREADCALL ZoneEventThreadProc(void * pParam /* = nullptr */)
{
	while (!g_bNpcExit)
	{
		g_pMain->g_arZone.m_lock.lock();
		foreach_stlmap_nolock (itr, g_pMain->g_arZone)
		{
			MAP *pMap = itr->second;
			if (pMap == nullptr
				|| pMap->m_byRoomEvent == 0
				|| pMap->IsRoomStatusCheck()) 
				continue;

			foreach_stlmap (itr, pMap->m_arRoomEventArray)
			{
				CRoomEvent * pRoom = itr->second;
				if (pRoom == nullptr
					|| !pRoom->isInProgress())
					continue;

				pRoom->MainRoom();
			}
		}
		g_pMain->g_arZone.m_lock.unlock();

		sleep(1000);
	}

	return 0;
}

void CNpcThread::AddNPC(CNpc * pNpc)
{
	Guard lock(m_lock);
	m_pNpcs.insert(pNpc);
}

void CNpcThread::RemoveNPC(CNpc * pNpc)
{
	Guard lock(m_lock);
	m_pNpcs.erase(pNpc);
}

CNpcThread::CNpcThread()
{
}

CNpcThread::~CNpcThread()
{
	Guard lock(m_lock);
	m_pNpcs.clear();
}
