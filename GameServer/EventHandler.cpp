#include "stdafx.h"
#include "DBAgent.h"

using std::string;
using std::vector;

void CGameServerDlg::SendBifrostTime(CUser *pUser, bool bSendAll) {

	Packet result(WIZ_BIFROST,uint8(BIFROST_EVENT));
	result << g_pMain->m_sBifrostRemainingTime;

	if (bSendAll)
	{
		g_pMain->Send_All(&result,nullptr, 0, ZONE_RONARK_LAND);
		g_pMain->Send_All(&result,nullptr, 0, ZONE_BIFROST);
	}
	else
	{
		if (pUser == nullptr)
			return;

		pUser->Send(&result);
	}
}

void CUser::BifrostProcess(CUser * pUser)
{
	if (pUser == nullptr)
		return;

	if (g_pMain->m_BifrostVictory == 0 && g_pMain->m_bAttackBifrostMonument)
	{
		g_pMain->m_sBifrostTime = g_pMain->m_xBifrostTime;
		g_pMain->m_sBifrostRemainingTime = g_pMain->m_sBifrostTime;
		g_pMain->m_BifrostVictory = pUser->GetNation();
		g_pMain->SendFormattedResource(pUser->GetNation() == ELMORAD ? IDS_BEEF_ROAST_VICTORY_ELMORAD : IDS_BEEF_ROAST_VICTORY_KARUS, Nation::ALL,false);
		g_pMain->SendBifrostTime(nullptr, true);

		if (g_pMain->m_bAttackBifrostMonument)
			g_pMain->m_bAttackBifrostMonument = false;
	}
	else if (g_pMain->m_BifrostVictory == 1 || g_pMain->m_BifrostVictory == 2) 
	{
		if (pUser->GetNation() != g_pMain->m_BifrostVictory && g_pMain->m_bAttackBifrostMonument)
		{
			g_pMain->m_BifrostVictory = 3;
			g_pMain->SendFormattedResource(pUser->GetNation() == ELMORAD ? IDS_BEEF_ROAST_VICTORY_ELMORAD : IDS_BEEF_ROAST_VICTORY_KARUS, Nation::ALL,false);

			if (g_pMain->m_bAttackBifrostMonument)
				g_pMain->m_bAttackBifrostMonument = false;
		}
	}
}

void CUser::TempleProcess(Packet &pkt )
{
	uint8 opcode = pkt.read<uint8>();

	switch(opcode)
	{
	case MONSTER_STONE:
		MonsterStoneProcess(); 
	case TEMPLE_EVENT_JOIN:
		TempleOperations(opcode);
		break;
	case TEMPLE_EVENT_DISBAND:
		TempleOperations(opcode);
		break;
	}
}

void CUser::MonsterStoneProcess()
{
	if(CheckExistItem(MONSTER_STONE,1))
	{
		RobItem(MONSTER_STONE,1);
		ZoneChange(myrand(1004,1016),m_curx,m_curz);
	}
} 

void CUser::TempleOperations(uint8 bType)
{
	uint16 nActiveEvent = (uint16)g_pMain->pTempleEvent.ActiveEvent;

	uint8 bResult = 1;
	Packet result(WIZ_EVENT);

	if(bType == TEMPLE_EVENT_JOIN && !isEventUser(GetSocketID()))
	{
		if (nActiveEvent == TEMPLE_EVENT_CHAOS)
		{
			if (CheckExistItem(CHAOS_MAP,1))
				bResult = 1;
			else if (m_sItemArray[RIGHTHAND].nNum == MATTOCK || m_sItemArray[RIGHTHAND].nNum == GOLDEN_MATTOCK || isMining())
				bResult = 4; 
			else
				bResult = 3;
		}

		if (bResult == 1) {
			GetNation() == KARUS ? g_pMain->pTempleEvent.KarusUserCount++ :g_pMain->pTempleEvent.ElMoradUserCount++;
			g_pMain->pTempleEvent.AllUserCount = (g_pMain->pTempleEvent.KarusUserCount + g_pMain->pTempleEvent.ElMoradUserCount);

			TempleOperations(TEMPLE_EVENT_COUNTER);
			AddEventUser();
		}

		result << bType << bResult << nActiveEvent;
		Send(&result);
	}
	else if (bType == TEMPLE_EVENT_DISBAND && isEventUser(GetSocketID()))
	{
		GetNation() == KARUS ? g_pMain->pTempleEvent.KarusUserCount-- : g_pMain->pTempleEvent.ElMoradUserCount--;
		g_pMain->pTempleEvent.AllUserCount = g_pMain->pTempleEvent.KarusUserCount + g_pMain->pTempleEvent.ElMoradUserCount;

		TempleOperations(TEMPLE_EVENT_COUNTER);
		RemoveEventUser(GetSocketID());

		result <<  bType << bResult << nActiveEvent;
		Send(&result);


	}
	else if (bType == TEMPLE_EVENT_COUNTER)
	{
		result << bType << nActiveEvent;

		if(nActiveEvent == TEMPLE_EVENT_CHAOS)
			result << g_pMain->pTempleEvent.AllUserCount;
		else
			result << g_pMain->pTempleEvent.KarusUserCount << g_pMain->pTempleEvent.ElMoradUserCount;

		g_pMain->Send_All(&result);
	}
}

void CUser::AddEventUser(CUser *pUser)
{
	if (pUser == nullptr)
		pUser = this;

	if (pUser == nullptr)
	{
		TRACE("#### AddEventUser : pUser == nullptr ####\n");
		return;
	}

	_TEMPLE_EVENT_USER * pEventUser = new _TEMPLE_EVENT_USER;

	pEventUser->m_socketID =  pUser->GetSocketID();
	pEventUser->m_bEventRoom = pUser->GetEventRoom();
	pEventUser->m_bZone = pUser->GetZoneID();
	pEventUser->m_bNation = pUser->GetNation();

	if (!g_pMain->m_TempleEventUserArray.PutData(pEventUser->m_socketID, pEventUser))
		delete pEventUser;
}

void CUser::RemoveEventUser(uint16 m_socketID)
{
	if (g_pMain->m_TempleEventUserArray.GetData(m_socketID) != nullptr)
		g_pMain->m_TempleEventUserArray.DeleteData(m_socketID);

	m_bEventRoom = 0;
}

void CUser::UpdateEventUser(uint16 m_socketID, int16 nEventRoom)
{
	_TEMPLE_EVENT_USER * pEventUser = g_pMain->m_TempleEventUserArray.GetData(m_socketID);
	CUser *pUser = g_pMain->GetUserPtr(m_socketID);

	if (pEventUser == nullptr || pUser == nullptr)
		return;

	pEventUser->m_bEventRoom = nEventRoom;
	pUser->m_bEventRoom = nEventRoom;
}

bool CUser::isEventUser(uint16 m_socketID)
{
	_TEMPLE_EVENT_USER * pEventUser = g_pMain->m_TempleEventUserArray.GetData(m_socketID);

	if (pEventUser != nullptr)
		return true;

	return false;
}