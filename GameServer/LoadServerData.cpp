#include "stdafx.h"

#include "Map.h"
#include "GameEvent.h"

#include "KnightsManager.h"
#include "DBAgent.h"
#include "KingSystem.h"

<<<<<<< HEAD
=======
#include "../shared/HardwareInformation.h"
>>>>>>> koserver2
#include "../shared/database/OdbcRecordset.h"
#include "../shared/database/ItemTableSet.h"
#include "../shared/database/SetItemTableSet.h"
#include "../shared/database/ItemExchangeSet.h"
#include "../shared/database/ItemUpgradeSet.h"
#include "../shared/database/ItemOpSet.h"
#include "../shared/database/MagicTableSet.h"
#include "../shared/database/MagicType1Set.h"
#include "../shared/database/MagicType2Set.h"
#include "../shared/database/MagicType3Set.h"
#include "../shared/database/MagicType4Set.h"
#include "../shared/database/MagicType5Set.h"
#include "../shared/database/MagicType6Set.h"
#include "../shared/database/MagicType7Set.h"
#include "../shared/database/MagicType8Set.h"
#include "../shared/database/MagicType9Set.h"
<<<<<<< HEAD
=======
#include "../shared/database/ObjectPosSet.h"
>>>>>>> koserver2
#include "../shared/database/ZoneInfoSet.h"
#include "../shared/database/EventSet.h"
#include "../shared/database/CoefficientSet.h"
#include "../shared/database/LevelUpTableSet.h"
#include "../shared/database/ServerResourceSet.h"
#include "../shared/database/QuestHelperSet.h"
#include "../shared/database/QuestMonsterSet.h"
#include "../shared/database/KnightsSet.h"
#include "../shared/database/KnightsUserSet.h"
#include "../shared/database/KnightsAllianceSet.h"
#include "../shared/database/KnightsRankSet.h"
#include "../shared/database/KnightsCapeSet.h"
#include "../shared/database/UserPersonalRankSet.h"
#include "../shared/database/UserKnightsRankSet.h"
<<<<<<< HEAD
#include "../shared/database/HomeSet.h"
#include "../shared/database/StartPositionSet.h"
=======
#include "../shared/database/StartPositionSet.h"
#include "../shared/database/StartPositionRandomSet.h"
>>>>>>> koserver2
#include "../shared/database/BattleSet.h"
#include "../shared/database/RentalItemSet.h"
#include "../shared/database/KingSystemSet.h"
#include "../shared/database/KingCandidacyNoticeBoardSet.h"
#include "../shared/database/KingElectionListSet.h"
<<<<<<< HEAD
#include "../shared/database/MonsterSummonListSet.h"
#include "../shared/database/MonsterSummonListZoneSet.h"
#include "../shared/database/MonsterRespawnListSet.h"
#include "../shared/database/PremiumItemSet.h"
#include "../shared/database/PremiumItemExpSet.h"
#include "../shared/database/UserDailyOpSet.h"
#include "../shared/database/EventTriggerSet.h"

bool CGameServerDlg::LoadItemTable()
{
	LOAD_TABLE(CItemTableSet, g_DBAgent.m_GameDB, &m_ItemtableArray, false);
=======
#include "../shared/database/EventTriggerSet.h"
#include "../shared/database/MonsterChallenge.h"
#include "../shared/database/MonsterChallengeSummonList.h"
#include "../shared/database/MonsterSummonListSet.h"
#include "../shared/database/MonsterSummonListZoneSet.h"
#include "../shared/database/MonsterRespawnListSet.h"
#include "../shared/database/MonsterRespawnListInformationSet.h"
#include "../shared/database/PremiumItemSet.h"
#include "../shared/database/PremiumItemExpSet.h"
#include "../shared/database/UserDailyOpSet.h"
#include "../shared/database/UserItemSet.h"

HardwareInformation g_pHardwareInformation;

bool CGameServerDlg::LoadItemTable()
{
	LOAD_TABLE(CItemTableSet, g_DBAgent.m_GameDB, &m_ItemtableArray, false, false);;
>>>>>>> koserver2
}

bool CGameServerDlg::LoadSetItemTable()
{
<<<<<<< HEAD
	LOAD_TABLE(CSetItemTableSet, g_DBAgent.m_GameDB, &m_SetItemArray, true);
=======
	LOAD_TABLE(CSetItemTableSet, g_DBAgent.m_GameDB, &m_SetItemArray,true, false);
>>>>>>> koserver2
}

bool CGameServerDlg::LoadItemExchangeTable()
{
<<<<<<< HEAD
	LOAD_TABLE(CItemExchangeSet, g_DBAgent.m_GameDB, &m_ItemExchangeArray, true);
=======
	LOAD_TABLE(CItemExchangeSet, g_DBAgent.m_GameDB, &m_ItemExchangeArray,true, false);
>>>>>>> koserver2
}

bool CGameServerDlg::LoadItemUpgradeTable()
{
<<<<<<< HEAD
	LOAD_TABLE(CItemUpgradeSet, g_DBAgent.m_GameDB, &m_ItemUpgradeArray, false);
=======
	LOAD_TABLE(CItemUpgradeSet, g_DBAgent.m_GameDB, &m_ItemUpgradeArray, false, false);
>>>>>>> koserver2
}

bool CGameServerDlg::LoadItemOpTable()
{
<<<<<<< HEAD
	LOAD_TABLE(CItemOpSet, g_DBAgent.m_GameDB, &m_ItemOpArray, true);
=======
	LOAD_TABLE(CItemOpSet, g_DBAgent.m_GameDB, &m_ItemOpArray,true, false);
>>>>>>> koserver2
}

bool CGameServerDlg::LoadServerResourceTable()
{
<<<<<<< HEAD
	LOAD_TABLE(CServerResourceSet, g_DBAgent.m_GameDB, &m_ServerResourceArray, false);
=======
	LOAD_TABLE(CServerResourceSet, g_DBAgent.m_GameDB, &m_ServerResourceArray, false, false);
>>>>>>> koserver2
}

bool CGameServerDlg::LoadQuestHelperTable()
{
<<<<<<< HEAD
	FastGuard lock(m_questNpcLock);
	m_QuestNpcList.clear();
	LOAD_TABLE(CQuestHelperSet, g_DBAgent.m_GameDB, &m_QuestHelperArray, true);
=======
	Guard lock(m_questNpcLock);
	m_QuestNpcList.clear();
	LOAD_TABLE(CQuestHelperSet, g_DBAgent.m_GameDB, &m_QuestHelperArray,true, false);
>>>>>>> koserver2
}

bool CGameServerDlg::LoadQuestMonsterTable()
{
<<<<<<< HEAD
	LOAD_TABLE(CQuestMonsterSet, g_DBAgent.m_GameDB, &m_QuestMonsterArray, true);
=======
	LOAD_TABLE(CQuestMonsterSet, g_DBAgent.m_GameDB, &m_QuestMonsterArray,true, false);
>>>>>>> koserver2
}

bool CGameServerDlg::LoadMagicTable()
{
<<<<<<< HEAD
	LOAD_TABLE(CMagicTableSet, g_DBAgent.m_GameDB, &m_MagictableArray, false);
=======
	LOAD_TABLE(CMagicTableSet, g_DBAgent.m_GameDB, &m_MagictableArray, false, false);
>>>>>>> koserver2
}

bool CGameServerDlg::LoadMagicType1()
{
<<<<<<< HEAD
	LOAD_TABLE(CMagicType1Set, g_DBAgent.m_GameDB, &m_Magictype1Array, false);
=======
	LOAD_TABLE(CMagicType1Set, g_DBAgent.m_GameDB, &m_Magictype1Array, false, false);
>>>>>>> koserver2
}

bool CGameServerDlg::LoadMagicType2()
{
<<<<<<< HEAD
	LOAD_TABLE(CMagicType2Set, g_DBAgent.m_GameDB, &m_Magictype2Array, false);
=======
	LOAD_TABLE(CMagicType2Set, g_DBAgent.m_GameDB, &m_Magictype2Array, false, false);
>>>>>>> koserver2
}

bool CGameServerDlg::LoadMagicType3()
{
<<<<<<< HEAD
	LOAD_TABLE(CMagicType3Set, g_DBAgent.m_GameDB, &m_Magictype3Array, false);
=======
	LOAD_TABLE(CMagicType3Set, g_DBAgent.m_GameDB, &m_Magictype3Array, false, false);
>>>>>>> koserver2
}

bool CGameServerDlg::LoadMagicType4()
{
<<<<<<< HEAD
	LOAD_TABLE(CMagicType4Set, g_DBAgent.m_GameDB, &m_Magictype4Array, false);
=======
	LOAD_TABLE(CMagicType4Set, g_DBAgent.m_GameDB, &m_Magictype4Array, false, false);
>>>>>>> koserver2
}

bool CGameServerDlg::LoadMagicType5()
{
<<<<<<< HEAD
	LOAD_TABLE(CMagicType5Set, g_DBAgent.m_GameDB, &m_Magictype5Array, false);
=======
	LOAD_TABLE(CMagicType5Set, g_DBAgent.m_GameDB, &m_Magictype5Array, false, false);
>>>>>>> koserver2
}

bool CGameServerDlg::LoadMagicType6()
{
<<<<<<< HEAD
	LOAD_TABLE(CMagicType6Set, g_DBAgent.m_GameDB, &m_Magictype6Array, false);
=======
	LOAD_TABLE(CMagicType6Set, g_DBAgent.m_GameDB, &m_Magictype6Array, false, false);
>>>>>>> koserver2
}

bool CGameServerDlg::LoadMagicType7()
{
<<<<<<< HEAD
	LOAD_TABLE(CMagicType7Set, g_DBAgent.m_GameDB, &m_Magictype7Array, false);
=======
	LOAD_TABLE(CMagicType7Set, g_DBAgent.m_GameDB, &m_Magictype7Array, false, false);
>>>>>>> koserver2
}

bool CGameServerDlg::LoadMagicType8()
{
<<<<<<< HEAD
	LOAD_TABLE(CMagicType8Set, g_DBAgent.m_GameDB, &m_Magictype8Array, false);
=======
	LOAD_TABLE(CMagicType8Set, g_DBAgent.m_GameDB, &m_Magictype8Array, false, false);
>>>>>>> koserver2
}

bool CGameServerDlg::LoadMagicType9()
{
<<<<<<< HEAD
	LOAD_TABLE(CMagicType9Set, g_DBAgent.m_GameDB, &m_Magictype9Array, false);
=======
	LOAD_TABLE(CMagicType9Set, g_DBAgent.m_GameDB, &m_Magictype9Array, false, false);
>>>>>>> koserver2
}

bool CGameServerDlg::LoadRentalList()
{
<<<<<<< HEAD
	LOAD_TABLE(CRentalItemSet, g_DBAgent.m_GameDB, &m_RentalItemArray, true);
=======
	LOAD_TABLE(CRentalItemSet, g_DBAgent.m_GameDB, &m_RentalItemArray,true, false);
>>>>>>> koserver2
}

bool CGameServerDlg::LoadCoefficientTable()
{
<<<<<<< HEAD
	LOAD_TABLE(CCoefficientSet, g_DBAgent.m_GameDB, &m_CoefficientArray, false);
=======
	LOAD_TABLE(CCoefficientSet, g_DBAgent.m_GameDB, &m_CoefficientArray, false, false);
>>>>>>> koserver2
}

bool CGameServerDlg::LoadLevelUpTable()
{
<<<<<<< HEAD
	LOAD_TABLE(CLevelUpTableSet, g_DBAgent.m_GameDB, &m_LevelUpArray, false);
}

bool CGameServerDlg::LoadAllKnights()
{
	FastGuard lock(m_KnightsArray.m_lock);
	LOAD_TABLE(CKnightsSet, g_DBAgent.m_GameDB, &m_KnightsArray, true);
}

bool CGameServerDlg::LoadAllKnightsUserData()
{
	LOAD_TABLE(CKnightsUserSet, g_DBAgent.m_GameDB, nullptr, true);
}

bool CGameServerDlg::LoadKnightsAllianceTable()
{
	LOAD_TABLE(CKnightsAllianceSet, g_DBAgent.m_GameDB, &m_KnightsAllianceArray, true);
=======
	LOAD_TABLE(CLevelUpTableSet, g_DBAgent.m_GameDB, &m_LevelUpArray, false, false);
}

bool CGameServerDlg::LoadAllKnights(bool bIsSlient)
{
	Guard lock(m_KnightsArray.m_lock);
	LOAD_TABLE(CKnightsSet, g_DBAgent.m_GameDB, &m_KnightsArray,true, bIsSlient);
}

bool CGameServerDlg::LoadAllKnightsUserData(bool bIsSlient)
{
	LOAD_TABLE(CKnightsUserSet, g_DBAgent.m_GameDB, nullptr,true, bIsSlient);
}

bool CGameServerDlg::LoadKnightsAllianceTable(bool bIsSlient)
{
	LOAD_TABLE(CKnightsAllianceSet, g_DBAgent.m_GameDB, &m_KnightsAllianceArray,true, bIsSlient);
>>>>>>> koserver2
}

bool CGameServerDlg::LoadUserRankings()
{
	CUserPersonalRankSet UserPersonalRankSet(g_DBAgent.m_GameDB, &m_UserPersonalRankMap);
	CUserKnightsRankSet  UserKnightsRankSet(g_DBAgent.m_GameDB, &m_UserKnightsRankMap);
	TCHAR * szError = nullptr;

	// Cleanup first, in the event it's already loaded (we'll have this automatically reload in-game)
	CleanupUserRankings();

	// Acquire the lock for thread safety, and load both tables.
<<<<<<< HEAD
	FastGuard lock(m_userRankingsLock);
=======
	Guard lock(m_userRankingsLock);
>>>>>>> koserver2

	szError = UserPersonalRankSet.Read(true);
	if (szError != nullptr)
	{
		printf("ERROR: Failed to load personal rankings, error:\n%s\n", szError);
		return false;
	}

	szError = UserKnightsRankSet.Read(true);
	if (szError != nullptr)
	{
		printf("ERROR: Failed to load user knights rankings, error:\n%s\n", szError);
		return false;
	}

	return true;
}

void CGameServerDlg::CleanupUserRankings()
{
	std::set<_USER_RANK *> deleteSet;
<<<<<<< HEAD
	FastGuard lock(m_userRankingsLock);
=======
	Guard lock(m_userRankingsLock);
>>>>>>> koserver2

	// Go through the personal rank map, reset the character's rank and insert
	// the _USER_RANK struct instance into the deletion set for later.
	foreach (itr, m_UserPersonalRankMap)
	{
		CUser *pUser = GetUserPtr(itr->first, TYPE_CHARACTER);
		if (pUser != nullptr)
			pUser->m_bPersonalRank = -1;

		deleteSet.insert(itr->second);
	}

	// Go through the knights rank map, reset the character's rank and insert
	// the _USER_RANK struct instance into the deletion set for later.
	foreach (itr, m_UserKnightsRankMap)
	{
		CUser *pUser = GetUserPtr(itr->first, TYPE_CHARACTER);
		if (pUser != nullptr)
			pUser->m_bKnightsRank = -1;

		deleteSet.insert(itr->second);
	}

	// Clear out the maps
	m_UserPersonalRankMap.clear();
	m_UserKnightsRankMap.clear();

	// Free the memory used by the _USER_RANK structs
	foreach (itr, deleteSet)
		delete *itr;

	// These only store pointers to memory that was already freed by the primary rankings maps.
	m_playerRankings[KARUS_ARRAY].clear();
	m_playerRankings[ELMORAD_ARRAY].clear();
}

bool CGameServerDlg::LoadKnightsCapeTable()
{
<<<<<<< HEAD
	LOAD_TABLE(CKnightsCapeSet, g_DBAgent.m_GameDB, &m_KnightsCapeArray, false);
}

bool CGameServerDlg::LoadKnightsRankTable(bool bWarTime /*= false*/)
{
	std::string strKarusCaptainNames, strElmoCaptainNames;
	LOAD_TABLE_ERROR_ONLY(CKnightsRankSet, g_DBAgent.m_GameDB, nullptr, true);
=======
	LOAD_TABLE(CKnightsCapeSet, g_DBAgent.m_GameDB, &m_KnightsCapeArray, false, false);
}

bool CGameServerDlg::LoadKnightsRankTable(bool bWarTime /*= false*/, bool bIsSlient /*= false*/)
{
	std::string strKarusCaptainNames, strElmoCaptainNames;
	LOAD_TABLE_ERROR_ONLY(CKnightsRankSet, g_DBAgent.m_GameDB, nullptr, true, bIsSlient);
>>>>>>> koserver2

	if (!bWarTime)
		return true;

	CKnightsRankSet & pSet = _CKnightsRankSet; // kind ugly generic naming

	if (pSet.nKarusCount > 0)
	{
		Packet result;
		GetServerResource(IDS_KARUS_CAPTAIN, &strKarusCaptainNames, 
			pSet.strKarusCaptain[0], pSet.strKarusCaptain[1], pSet.strKarusCaptain[2], 
			pSet.strKarusCaptain[3], pSet.strKarusCaptain[4]);
		ChatPacket::Construct(&result, WAR_SYSTEM_CHAT, &strKarusCaptainNames);
		Send_All(&result, nullptr, KARUS);
	}

	if (pSet.nElmoCount > 0)
	{
		Packet result;
		GetServerResource(IDS_ELMO_CAPTAIN, &strElmoCaptainNames,
			pSet.strElmoCaptain[0], pSet.strElmoCaptain[1], pSet.strElmoCaptain[2], 
			pSet.strElmoCaptain[3], pSet.strElmoCaptain[4]);
		ChatPacket::Construct(&result, WAR_SYSTEM_CHAT, &strElmoCaptainNames);
		Send_All(&result, nullptr, ELMORAD);
	}

	return true;
}

<<<<<<< HEAD
bool CGameServerDlg::LoadHomeTable()
{
	LOAD_TABLE(CHomeSet, g_DBAgent.m_GameDB, &m_HomeArray, false);
}

bool CGameServerDlg::LoadStartPositionTable()
{
	LOAD_TABLE(CStartPositionSet, g_DBAgent.m_GameDB, &m_StartPositionArray, false);
=======
bool CGameServerDlg::LoadStartPositionTable()
{
	LOAD_TABLE(CStartPositionSet, g_DBAgent.m_GameDB, &m_StartPositionArray, false, false);
>>>>>>> koserver2
}

bool CGameServerDlg::LoadBattleTable()
{
<<<<<<< HEAD
	LOAD_TABLE(CBattleSet, g_DBAgent.m_GameDB, &m_byOldVictory, true);
=======
	LOAD_TABLE(CBattleSet, g_DBAgent.m_GameDB, &m_byOldVictory,true, false);
>>>>>>> koserver2
}

bool CGameServerDlg::LoadKingSystem()
{
<<<<<<< HEAD
	LOAD_TABLE_ERROR_ONLY(CKingSystemSet, g_DBAgent.m_GameDB, &m_KingSystemArray, true);
	LOAD_TABLE_ERROR_ONLY(CKingCandidacyNoticeBoardSet, g_DBAgent.m_GameDB, &m_KingSystemArray, true);
	LOAD_TABLE(CKingElectionListSet, g_DBAgent.m_GameDB, &m_KingSystemArray, true);
=======
	LOAD_TABLE_ERROR_ONLY(CKingSystemSet, g_DBAgent.m_GameDB, &m_KingSystemArray,true, false);
	LOAD_TABLE_ERROR_ONLY(CKingCandidacyNoticeBoardSet, g_DBAgent.m_GameDB, &m_KingSystemArray,true, false);
	LOAD_TABLE(CKingElectionListSet, g_DBAgent.m_GameDB, &m_KingSystemArray,true, false);
}

bool CGameServerDlg::LoadMonsterChallengeTable()
{
	LOAD_TABLE(CMonsterChallenge, g_DBAgent.m_GameDB, &m_MonsterChallengeArray,true, false);
}

bool CGameServerDlg::LoadMonsterChallengeSummonListTable()
{
	LOAD_TABLE(CMonsterChallengeSummonList, g_DBAgent.m_GameDB, &m_MonsterChallengeSummonListArray,true, false);
>>>>>>> koserver2
}

bool CGameServerDlg::LoadMonsterSummonListTable()
{
<<<<<<< HEAD
	LOAD_TABLE(CMonsterSummonListSet, g_DBAgent.m_GameDB, &m_MonsterSummonList, true);
=======
	LOAD_TABLE(CMonsterSummonListSet, g_DBAgent.m_GameDB, &m_MonsterSummonList,true, false);
>>>>>>> koserver2
}

bool CGameServerDlg::LoadMonsterSummonListZoneTable()
{
<<<<<<< HEAD
	LOAD_TABLE(CMonsterSummonListZoneSet, g_DBAgent.m_GameDB, &m_MonsterSummonListZoneArray, true);
=======
	LOAD_TABLE(CMonsterSummonListZoneSet, g_DBAgent.m_GameDB, &m_MonsterSummonListZoneArray,true, false);
>>>>>>> koserver2
}

bool CGameServerDlg::LoadMonsterRespawnListTable()
{
<<<<<<< HEAD
	LOAD_TABLE(CMonsterRespawnListSet, g_DBAgent.m_GameDB, &m_MonsterRespawnListArray, true);
=======
	LOAD_TABLE(CMonsterRespawnListSet, g_DBAgent.m_GameDB, &m_MonsterRespawnListArray,true, false);
}

bool CGameServerDlg::LoadMonsterRespawnListInformationTable()
{
	LOAD_TABLE(CMonsterRespawnListInformationSet, g_DBAgent.m_GameDB, &m_MonsterRespawnListInformationArray,true, false);
>>>>>>> koserver2
}

bool CGameServerDlg::LoadPremiumItemTable()
{
<<<<<<< HEAD
	LOAD_TABLE(CPremiumItemSet, g_DBAgent.m_GameDB, &m_PremiumItemArray, true);
=======
	LOAD_TABLE(CPremiumItemSet, g_DBAgent.m_GameDB, &m_PremiumItemArray,true, false);
>>>>>>> koserver2
}

bool CGameServerDlg::LoadPremiumItemExpTable()
{
<<<<<<< HEAD
	LOAD_TABLE(CPremiumItemExpSet, g_DBAgent.m_GameDB, &m_PremiumItemExpArray, true);
=======
	LOAD_TABLE(CPremiumItemExpSet, g_DBAgent.m_GameDB, &m_PremiumItemExpArray,true, false);
>>>>>>> koserver2
}

bool CGameServerDlg::LoadUserDailyOpTable()
{
<<<<<<< HEAD
	CUserDailyOpSet UserDailyOpSet(g_DBAgent.m_GameDB, &m_UserDailyOpMap);
	UserDailyOpSet.Read(true);
	return true;
=======
	LOAD_TABLE(CUserDailyOpSet, g_DBAgent.m_GameDB, &m_UserDailyOpMap,true, false);
>>>>>>> koserver2
}

bool CGameServerDlg::LoadEventTriggerTable()
{
<<<<<<< HEAD
	LOAD_TABLE(CEventTriggerSet, g_DBAgent.m_GameDB, &m_EventTriggerArray, true);
=======
	LOAD_TABLE(CEventTriggerSet, g_DBAgent.m_GameDB, &m_EventTriggerArray,true, false);
}

bool CGameServerDlg::LoadStartPositionRandomTable()
{
	LOAD_TABLE(CStartPositionRandomSet, g_DBAgent.m_GameDB, &m_StartPositionRandomArray,true, false);
}

bool CGameServerDlg::LoadUserItemTable()
{
	LOAD_TABLE(CUserItemSet, g_DBAgent.m_GameDB, &m_UserItemArray,true, false);
}

bool CGameServerDlg::LoadObjectPosTable()
{
	LOAD_TABLE(CObjectPosSet, g_DBAgent.m_GameDB, &m_ObjectEventArray,true, false);
>>>>>>> koserver2
}

bool CGameServerDlg::MapFileLoad()
{
	ZoneInfoMap zoneMap;
<<<<<<< HEAD
	LOAD_TABLE_ERROR_ONLY(CZoneInfoSet, g_DBAgent.m_GameDB, &zoneMap, false); 
=======
	LOAD_TABLE_ERROR_ONLY(CZoneInfoSet, g_DBAgent.m_GameDB, &zoneMap, false, false); 
>>>>>>> koserver2

	foreach (itr, zoneMap)
	{
		C3DMap *pMap = new C3DMap();
		_ZONE_INFO *pZone = itr->second;
		if (!pMap->Initialize(pZone))
		{
			printf("ERROR: Unable to load SMD - %s\n", pZone->m_MapName.c_str());
			delete pZone;
			delete pMap;
			m_ZoneArray.DeleteAllData();
			return false;
		}

		delete pZone;
		m_ZoneArray.PutData(pMap->m_nZoneNumber, pMap);
	}

<<<<<<< HEAD
	LOAD_TABLE_ERROR_ONLY(CEventSet, g_DBAgent.m_GameDB, &m_ZoneArray, true);
=======
	LOAD_TABLE_ERROR_ONLY(CEventSet, g_DBAgent.m_GameDB, &m_ZoneArray, true, false);
>>>>>>> koserver2
	return true;
}