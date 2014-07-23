﻿#include "stdafx.h"
#include "KingSystem.h"

#include "../shared/ClientSocketMgr.h"
#include "../shared/Ini.h"
#include "../shared/DateTime.h"
#include "../shared/HardwareInformation.h"

#include <time.h>
#include <iostream>
#include <fstream>
#include "Map.h"

#include "AISocket.h"

#include "DBAgent.h"

using namespace std;

#define NUM_FLAG_VICTORY    4
#define AWARD_GOLD          100000
#define AWARD_EXP			5000

std::vector<Thread *> g_timerThreads;

WORD	g_increase_serial = 1;

HardwareInformation g_HardwareInformation;

CGameServerDlg::CGameServerDlg()
{
	DateTime now;

	m_sYear = now.GetYear();
	m_sMonth = now.GetMonth();
	m_sDate = now.GetDay();
	m_sHour = now.GetHour();
	m_sMin = now.GetMinute();

	m_ReloadKnightAndUserRanksMinute = 0;

	m_byWeather = 0;
	m_sWeatherAmount = 0;
	m_byKingWeatherEvent = 0;
	m_byKingWeatherEvent_Day = 0;
	m_byKingWeatherEvent_Hour = 0;
	m_byKingWeatherEvent_Minute = 0;

	m_byNPEventAmount = 0;
	m_byExpEventAmount = 0;
	m_byCoinEventAmount = 0;

	m_sPartyIndex = 0;

	m_nCastleCapture = 0;

	m_bFirstServerFlag = false;
	m_bPointCheckFlag = true;

	m_nServerNo = 0;
	m_nServerGroupNo = 0;
	m_nServerGroup = 0;
	m_sDiscount = 0;

	m_bPermanentChatMode = false;
	m_bSantaOrAngel = FLYING_NONE;
}

/**
* @brief	Loads config, table data, initialises sockets and generally
* 			starts up the server.
*
* @return	true if it succeeds, false if it fails.
*/
bool CGameServerDlg::Startup()
{
	DateTime time;

	m_sZoneCount = 0;
	m_sErrorSocketCount = 0;

	m_bFirstServerFlag = false;	
	m_bServerCheckFlag = false;

	GetTimeFromIni();

	if (!g_pMain->m_socketMgr.Listen(m_GameServerPort, MAX_USER))
	{
		printf(_T("ERROR : Failed to listen on server port (%d).\n"), m_GameServerPort);
		return false;
	}

	// Bit tacky, but there's no reason we can't reuse the existing completion port for our AI socket
	m_aiSocketMgr.SetCompletionPort(g_pMain->m_socketMgr.GetCompletionPort());
	m_aiSocketMgr.InitSessions(1);

	if (!g_DBAgent.Startup(m_bMarsEnabled,
		m_strAccountDSN, m_strAccountUID, m_strAccountPWD,
		m_strGameDSN, m_strGameUID, m_strGamePWD)
		|| !LoadItemTable()
		|| !LoadSetItemTable()
		|| !LoadItemExchangeTable()
		|| !LoadItemUpgradeTable()
		|| !LoadItemOpTable()
		|| !LoadServerResourceTable()
		|| !LoadQuestHelperTable()
		|| !LoadQuestMonsterTable()
		|| !LoadMagicTable()
		|| !LoadMagicType1()
		|| !LoadMagicType2()
		|| !LoadMagicType3()
		|| !LoadMagicType4()
		|| !LoadMagicType5()
		|| !LoadMagicType6()
		|| !LoadMagicType7()
		|| !LoadMagicType8()
		|| !LoadMagicType9()
		|| !LoadObjectPosTable()
		|| !LoadRentalList()
		|| !LoadCoefficientTable()
		|| !LoadLevelUpTable()
		|| !LoadAllKnights()
		|| !LoadAllKnightsUserData()
		|| !LoadKnightsAllianceTable()
		|| !LoadUserRankings()
		|| !LoadKnightsCapeTable()
		|| !LoadKnightsRankTable()
		|| !LoadStartPositionTable()
		|| !LoadStartPositionRandomTable()
		|| !LoadBattleTable()
		|| !MapFileLoad()
		|| !LoadKingSystem()
		|| !LoadEventTriggerTable()
		|| !LoadMonsterChallengeTable()
		|| !LoadMonsterChallengeSummonListTable()
		|| !LoadMonsterSummonListTable()
		|| !LoadMonsterSummonListZoneTable()
		|| !LoadMonsterRespawnListTable()
		|| !LoadMonsterRespawnListInformationTable()
		|| !LoadPremiumItemTable()
		|| !LoadPremiumItemExpTable()
		|| !LoadUserDailyOpTable())
		//|| !LoadUserItemTable())
		return false;

	// Clear any remaining users in the currently logged in list
	// that may be left as a result of an improper shutdown.
	g_DBAgent.ClearRemainUsers();

	// Logs Start
	CreateDirectory("Logs",NULL);

	m_fpDeathUser = fopen(string_format("./Logs/DeathUser_%d_%d_%d.log",time.GetDay(),time.GetMonth(),time.GetYear()).c_str(), "a");
	if (m_fpDeathUser == nullptr)
	{
		printf("ERROR: Unable to open death user log file.\n");
		return false;
	}

	m_fpDeathNpc = fopen(string_format("./Logs/DeathNpc_%d_%d_%d.log",time.GetDay(),time.GetMonth(),time.GetYear()).c_str(), "a");
	if (m_fpDeathNpc == nullptr)
	{
		printf("ERROR: Unable to open death npc log file.\n");
		return false;
	}

	m_fpChat = fopen(string_format("./Logs/Chat_%d_%d_%d.log",time.GetDay(),time.GetMonth(),time.GetYear()).c_str(), "a");
	if (m_fpChat == nullptr)
	{
		printf("ERROR: Unable to open chat log file.\n");
		return false;
	}

	m_fpCheat = fopen(string_format("./Logs/Cheat_%d_%d_%d.log",time.GetDay(),time.GetMonth(),time.GetYear()).c_str(), "a");
	if (m_fpCheat == nullptr)
	{
		printf("ERROR: Unable to open cheat log file.\n");
		return false;
	}
	// Logs End

	LoadNoticeData();

	printf("\n");
	if (!m_luaEngine.Initialise())
		return false;

	AIServerConnect();

	// Initialise the command tables
	InitServerCommands();
	CUser::InitChatCommands();

	g_pMain->m_socketMgr.RunServer();

	return true; 
}

/**
* @brief	Loads the server's config from the INI file.
*/
void CGameServerDlg::GetTimeFromIni()
{
	CIni ini(CONF_GAME_SERVER);
	int year=0, month=0, date=0, hour=0, server_count=0, sgroup_count = 0, i=0;
	char ipkey[20];

	// This is so horrible.
	ini.GetString("ODBC", "GAME_DSN", "KO_GAME", m_strGameDSN, false);
	ini.GetString("ODBC", "GAME_UID", "username", m_strGameUID, false);
	ini.GetString("ODBC", "GAME_PWD", "password", m_strGamePWD, false);

	m_bMarsEnabled = ini.GetBool("ODBC", "GAME_MARS", false);

	ini.GetString("ODBC", "ACCOUNT_DSN", "KO_MAIN", m_strAccountDSN, false);
	ini.GetString("ODBC", "ACCOUNT_UID", "username", m_strAccountUID, false);
	ini.GetString("ODBC", "ACCOUNT_PWD", "password", m_strAccountPWD, false);

	bool bMarsEnabled = ini.GetBool("ODBC", "ACCOUNT_MARS", false);

	// Both need to be enabled to use MARS.
	if (!m_bMarsEnabled || !bMarsEnabled)
		m_bMarsEnabled = false;

	m_byWeather = ini.GetInt("TIMER", "WEATHER", 1);

	ini.GetString("BATTLE", "DAYS","1,6", m_sBattleZoneOpenDays, false);

	for (int i = 0; i < WAR_TIME_COUNT; i++)
		m_nBattleZoneOpenHourStart[i] = ini.GetInt("BATTLE",string_format("START_TIME%d",i).c_str(), (i+1) * 7);

	for (int i = 0; i < WAR_ZONE_COUNT; i++)
	{
		m_nBattlezoneOpenWarZone[i] = ini.GetInt("BATTLE",string_format("WAR_ZONE%d",i).c_str(), ZONE_BATTLE + i);
		if (m_nBattlezoneOpenWarZone[i] > ZONE_BATTLE_BASE)
			m_nBattlezoneOpenWarZone[i] = m_nBattlezoneOpenWarZone[i] - ZONE_BATTLE_BASE;
	}

	m_nCastleCapture = ini.GetInt("CASTLE", "NATION", 1);
	m_nServerNo = ini.GetInt("ZONE_INFO", "MY_INFO", 1);
	m_nServerGroup = ini.GetInt("ZONE_INFO", "SERVER_NUM", 0);
	server_count = ini.GetInt("ZONE_INFO", "SERVER_COUNT", 1);
	if (server_count < 1)
	{
		printf("ERROR: Invalid SERVER_COUNT property in INI.\n");
		return;
	}

	for (i = 0; i < server_count; i++)
	{
		_ZONE_SERVERINFO *pInfo = new _ZONE_SERVERINFO;
		sprintf(ipkey, "SERVER_%02d", i);
		pInfo->sServerNo = ini.GetInt("ZONE_INFO", ipkey, 1);
		sprintf(ipkey, "SERVER_IP_%02d", i);
		ini.GetString("ZONE_INFO", ipkey, "127.0.0.1", pInfo->strServerIP);
		m_ServerArray.PutData(pInfo->sServerNo, pInfo);
	}

	if (m_nServerGroup != 0)
	{
		m_nServerGroupNo = ini.GetInt("SG_INFO", "GMY_INFO", 1);
		sgroup_count = ini.GetInt("SG_INFO", "GSERVER_COUNT", 1);
		if (server_count < 1)
		{
			printf("ERROR: Invalid GSERVER_COUNT property in INI.\n");
			return;
		}

		for (i = 0; i < sgroup_count; i++)
		{
			_ZONE_SERVERINFO *pInfo = new _ZONE_SERVERINFO;
			sprintf(ipkey, "GSERVER_%02d",i );
			pInfo->sServerNo = ini.GetInt("SG_INFO", ipkey, 1);
			sprintf(ipkey, "GSERVER_IP_%02d", i);
			ini.GetString("SG_INFO", ipkey, "127.0.0.1", pInfo->strServerIP);

			m_ServerGroupArray.PutData(pInfo->sServerNo, pInfo);
		}
	}

	ini.GetString("AI_SERVER", "IP", "127.0.0.1", m_AIServerIP);
	m_AIServerPort = ini.GetInt("AI_SERVER","PORT", 10020);

	m_GameServerPort = ini.GetInt("SETTINGS","PORT", 15001);
	m_nGameMasterRHitDamage = ini.GetInt("SETTINGS","GAME_MASTER_R_HIT_DAMAGE", 30000);
	m_nPlayerRankingResetTime = ini.GetInt("SETTINGS","PLAYER_RANKINGS_RESET_TIME", 12);
	ini.GetString("SETTINGS", "PLAYER_RANKINGS_REWARD_ZONES","71,72,73", m_sPlayerRankingsRewardZones, false);
	m_nPlayerRankingKnightCashReward = ini.GetInt("SETTINGS","PLAYER_RANKINGS_KNIGHT_CASH_REWARD", 0);
	m_nPlayerRankingLoyaltyReward = ini.GetInt("SETTINGS","PLAYER_RANKINGS_LOYALTY_REWARD", 0);

	m_byExpEventAmount = ini.GetInt("BONUS","EXP",0);
	m_byCoinEventAmount = ini.GetInt("BONUS","MONEY",0);
	m_byNPEventAmount = ini.GetInt("BONUS","NP",0);

	m_Loyalty_Ardream_Source = 	ini.GetInt("NATIONAL_POINTS","ARDREAM_SOURCE",32);
	m_Loyalty_Ardream_Target = ini.GetInt("NATIONAL_POINTS","ARDREAM_TARGET",-25);
	m_Loyalty_Ronark_Land_Base_Source = ini.GetInt("NATIONAL_POINTS","RONARK_LAND_BASE_SOURCE",64);
	m_Loyalty_Ronark_Land_Base_Target = ini.GetInt("NATIONAL_POINTS","RONARK_LAND_BASE_TARGET",-50);
	m_Loyalty_Ronark_Land_Source = ini.GetInt("NATIONAL_POINTS","RONARK_LAND_SOURCE",64);
	m_Loyalty_Ronark_Land_Target = ini.GetInt("NATIONAL_POINTS","RONARK_LAND_TARGET",-50);
	m_Loyalty_Other_Zone_Source = ini.GetInt("NATIONAL_POINTS","OTHER_ZONE_SOURCE",64);
	m_Loyalty_Other_Zone_Target = ini.GetInt("NATIONAL_POINTS","OTHER_ZONE_TARGET",-50);

	for (int i = 0; i < BIFROST_EVENT_COUNT; i++)
		m_nBifrostTime[i] = ini.GetInt("BIFROST",string_format("START_TIME%d",i+1).c_str(), 0);

	for (int i = 0; i < BORDER_DEFENSE_WAR_EVENT_COUNT; i++)
		m_nBorderDefenseWarTime[i] = ini.GetInt("BDW",string_format("START_TIME%d",i+1).c_str(), 0);

	for (int i = 0; i < CHAOS_EVENT_COUNT; i++)
		m_nChaosTime[i] = ini.GetInt("CHAOS",string_format("START_TIME%d",i+1).c_str(), 0);

	m_nTempleEventRemainSeconds = 0;
	m_sRankResetHour = 0;

	m_UserRankingArray[KARUS_ARRAY].DeleteAllData();
	m_UserRankingArray[ELMORAD_ARRAY].DeleteAllData();

	m_byBattleOpenedTime = 0;
	m_byBattleTime = (2 * 60) * 60; // 2 Hours

	m_xBifrostRemainingTime = (240 * MINUTE);  // Bifrost remaining time ( 4 hour ).
	m_xBifrostMonumentAttackTime = (30 * MINUTE); // Players is attack a monument last 30 minute.
	m_xBifrostTime = (90 * MINUTE); // Victory nation time ( 1 hour and 30 minute )
	m_xJoinOtherNationBifrostTime = (60 * MINUTE); // Other nation join time ( last 1 hour )

	m_bAttackBifrostMonument = false;
	m_BifrostVictory = 0;
	m_sBifrostRemainingTime = m_xBifrostRemainingTime + 60;
	m_sBifrostTime = 0;

	m_IsMagicTableInUpdateProcess = false;
	m_IsPlayerRankingUpdateProcess = false;

	m_nPVPMonumentNation[ZONE_ARDREAM] = 0; 
	m_nPVPMonumentNation[ZONE_RONARK_LAND_BASE] = 0;
	m_nPVPMonumentNation[ZONE_RONARK_LAND] = 0;

	pTempleEvent.ActiveEvent = -1;
	pTempleEvent.ZoneID = 0;
	pTempleEvent.LastEventRoom = 1;
	pTempleEvent.StartTime = 0;
	pTempleEvent.AllUserCount = 0;
	pTempleEvent.KarusUserCount = 0;
	pTempleEvent.ElMoradUserCount = 0;
	pTempleEvent.isAttackable = false;
	pTempleEvent.isActive = false;

	m_bForgettenTempleIsActive = false;
	m_nForgettenTempleStartHour = 0;
	m_nForgettenTempleLevelMin = 0;
	m_nForgettenTempleLevelMax = 0;
	m_nForgettenTempleStartTime = 0;
	m_nForgettenTempleChallengeTime = 0;
	m_bForgettenTempleSummonMonsters = false;
	m_nForgettenTempleCurrentStage = 0;
	m_nForgettenTempleLastStage = 0;
	m_nForgettenTempleLastSummonTime = 0;
	m_nForgettenTempleBanishFlag = false;
	m_nForgettenTempleBanishTime = 0;

	g_timerThreads.push_back(new Thread(Timer_CheckGameEvents));
	g_timerThreads.push_back(new Thread(Timer_BifrostTime));
	g_timerThreads.push_back(new Thread(Timer_UpdateGameTime));
	g_timerThreads.push_back(new Thread(Timer_UpdateSessions));
	g_timerThreads.push_back(new Thread(Timer_UpdateConcurrent));
}

/**
* @brief	Gets & formats a cached server resource (_SERVER_RESOURCE entry).
*
* @param	nResourceID	Identifier for the resource.
* @param	result	   	The string to store the formatted result in.
*/
void CGameServerDlg::GetServerResource(int nResourceID, string * result, ...)
{
	_SERVER_RESOURCE *pResource = m_ServerResourceArray.GetData(nResourceID);
	if (pResource == nullptr)
	{
		*result = nResourceID;
		return;
	}

	va_list args;
	va_start(args, result);
	_string_format(pResource->strResource, result, args);
	va_end(args);
}

/**
* @brief	Gets the starting positions (for both nations) 
* 			for the specified zone.
*
* @param	nZoneID	Identifier for the zone.
*/
_START_POSITION *CGameServerDlg::GetStartPosition(int nZoneID)
{
	return m_StartPositionArray.GetData(nZoneID);
}

/**
* @brief	Gets the experience points required for the 
* 			specified level.
*
* @param	nLevel	The level.
*
* @return	The experience points required to level up from 
* 			the specified level.
*/
int64 CGameServerDlg::GetExpByLevel(int nLevel)
{
	LevelUpArray::iterator itr = m_LevelUpArray.find(nLevel);
	if (itr != m_LevelUpArray.end())
		return itr->second;

	return 0;
}

/**
* @brief	Gets zone by its identifier.
*
* @param	zoneID	Identifier for the zone.
*
* @return	null if it fails, otherwise the zone.
*/
C3DMap * CGameServerDlg::GetZoneByID(int zoneID)
{
	return m_ZoneArray.GetData(zoneID);
}

/**
* @brief	Looks up a user by name.
*
* @param	findName	The name to find.
* @param	type		The type of name (account, character).
*
* @return	null if it fails, else the user pointer.
*/
CUser* CGameServerDlg::GetUserPtr(string findName, NameType type)
{
	// As findName is a copy of the string passed in, we can change it
	// without worry of affecting anything.
	STRTOUPPER(findName);

	NameMap::iterator itr;
	if (type == TYPE_ACCOUNT)
	{
		Guard lock(m_accountNameLock);
		itr = m_accountNameMap.find(findName);
		return (itr != m_accountNameMap.end() ? itr->second : nullptr);
	}
	else if (type == TYPE_CHARACTER)
	{
		Guard lock(m_characterNameLock);
		itr = m_characterNameMap.find(findName);
		return (itr != m_characterNameMap.end() ? itr->second : nullptr);
	}

	return nullptr;
}

/**
* @brief	Adds the account name & session to a hashmap (on login)
*
* @param	pSession	The session.
*/
void CGameServerDlg::AddAccountName(CUser *pSession)
{
	Guard lock(m_accountNameLock);
	string upperName = pSession->m_strAccountID;
	STRTOUPPER(upperName);
	m_accountNameMap[upperName] = pSession;
}

/**
* @brief	Adds the character name & session to a hashmap (when in-game)
*
* @param	pSession	The session.
*/
void CGameServerDlg::AddCharacterName(CUser *pSession)
{
	Guard lock(m_characterNameLock);
	string upperName = pSession->GetName();
	STRTOUPPER(upperName);
	m_characterNameMap[upperName] = pSession;
}

/**
* @brief	Removes an existing character name/session from the hashmap, 
* 			replaces the character's name and reinserts the session with 
* 			the new name into the hashmap.
*
* @param	pSession		The session.
* @param	strNewUserID	Character's new name.
*/
void CGameServerDlg::ReplaceCharacterName(CUser *pSession, std::string & strNewUserID)
{
	Guard lock(m_characterNameLock);

	// Remove the old name from the map
	string upperName = pSession->GetName();
	STRTOUPPER(upperName);
	m_characterNameMap.erase(upperName);

	// Update the character's name & re-add them to the map.
	pSession->m_strUserID = strNewUserID;
	AddCharacterName(pSession);
}

/**
* @brief	Removes the account name & character names from the hashmaps (on logout)
*
* @param	pSession	The session.
*/
void CGameServerDlg::RemoveSessionNames(CUser *pSession)
{
	string upperName = pSession->m_strAccountID;
	STRTOUPPER(upperName);

	{ // remove account name from map (limit scope)
		Guard lock(m_accountNameLock);
		m_accountNameMap.erase(upperName);
	}

	if (pSession->isInGame())
	{
		upperName = pSession->GetName();
		STRTOUPPER(upperName);

		Guard lock(m_characterNameLock);
		m_characterNameMap.erase(upperName);
	}
}

CUser				* CGameServerDlg::GetUserPtr(uint16 sUserId) { return m_socketMgr[sUserId]; }
CNpc				* CGameServerDlg::GetNpcPtr(uint16 sNpcId) { return m_arNpcArray.GetData(sNpcId); }
_PARTY_GROUP		* CGameServerDlg::GetPartyPtr(uint16 sPartyID) { return m_PartyArray.GetData(sPartyID); }
CKnights			* CGameServerDlg::GetClanPtr(uint16 sClanID) { return m_KnightsArray.GetData(sClanID); }
_KNIGHTS_ALLIANCE	* CGameServerDlg::GetAlliancePtr(uint16 sAllianceID) { return m_KnightsAllianceArray.GetData(sAllianceID); }
_ITEM_TABLE			* CGameServerDlg::GetItemPtr(uint32 nItemID) { return m_ItemtableArray.GetData(nItemID); }

Unit * CGameServerDlg::GetUnitPtr(uint16 id)
{
	if (id < NPC_BAND)
		return GetUserPtr(id);

	return GetNpcPtr(id);
}

/**
* @brief	Spawn one or more event NPC/monsters.
*
* @param	sSid	  	The database ID of the NPC/monster to spawn.
* @param	bIsMonster	true if we are spawning a monster, false for an NPC.
* @param	byZone	  	The zone ID to spawn the monster to.
* @param	fX		  	The x coordinate.
* @param	fY		  	The y coordinate.
* @param	fZ		  	The z coordinate.
* @param	sCount	  	Number of spawns to create.
* @param	sRadius	  	Spawn radius.
*/
void CGameServerDlg::SpawnEventNpc(uint16 sSid, bool bIsMonster, uint8 byZone, float fX, float fY, float fZ, uint16 sCount /*= 1*/, uint16 sRadius /*= 0*/, uint16 sDuration /*= 0*/, uint8 nation /*= 0*/,int16 socketID /*= -1*/, uint16 nEventRoom)
{
	Packet result(AG_NPC_SPAWN_REQ);
	result	<< sSid << bIsMonster 
		<< byZone 
		<< uint16(fX * 10) << uint16(fY * 10) << uint16(fZ * 10) 
		<< sCount 
		<< sRadius
		<< sDuration
		<< nation
		<< socketID
		<< nEventRoom;
	Send_AIServer(&result);
}

void CGameServerDlg::KillNpc(uint16 sNid)
{
	Packet result(AG_NPC_KILL_REQ);
	result	<< sNid;
	Send_AIServer(&result);
}

void CGameServerDlg::NpcUpdate(uint16 sSid, bool bIsMonster, uint8 byGroup, uint16 sPid)
{
	Packet result(AG_NPC_UPDATE);
	result	<< sSid << bIsMonster << byGroup << sPid;
	Send_AIServer(&result);
}

_PARTY_GROUP * CGameServerDlg::CreateParty(CUser *pLeader)
{
	_PARTY_GROUP * pParty = new _PARTY_GROUP;

	pLeader->m_bInParty = true;
	pLeader->m_sPartyIndex = m_sPartyIndex.increment();

	pParty->wIndex = pLeader->GetPartyID();
	pParty->uid[0] = pLeader->GetSocketID();
	if (!m_PartyArray.PutData(pParty->wIndex, pParty))
	{
		delete pParty;
		pLeader->m_bInParty = false;
		pLeader->m_sPartyIndex = -1;
		pParty = nullptr;
	}

	return pParty;
}

void CGameServerDlg::DeleteParty(uint16 sIndex)
{
	m_PartyArray.DeleteData(sIndex);
}

uint32 CGameServerDlg::Timer_CheckGameEvents(void * lpParam)
{
	while (g_bRunning)
	{
		g_pMain->BattleZoneOpenTimer();	// Check if it's time for the BattleZone to open or end.
		g_pMain->TempleEventTimer();
		g_pMain->ForgettenTempleEventTimer();
		sleep(1 * SECOND);
	}
	return 0;
}

uint32 CGameServerDlg::Timer_BifrostTime(void * lpParam)
{
	while (g_bRunning)
	{
		if (g_pMain->m_BifrostVictory == 0)
		{
			g_pMain->m_sBifrostRemainingTime -= 60;

			if (g_pMain->m_sBifrostRemainingTime == 0)
			{
				g_pMain->m_BifrostVictory = 0;
				g_pMain->m_sBifrostRemainingTime = g_pMain->m_xBifrostRemainingTime;
				g_pMain->m_sBifrostTime = 0;
				g_pMain->SendFormattedResource(IDS_BEEF_ROAST_DRAW, Nation::ALL, false);

				if (g_pMain->m_bAttackBifrostMonument)
					g_pMain->m_bAttackBifrostMonument = false;

				g_pMain->SendEventRemainingTime(true, nullptr, ZONE_BIFROST);
			}
			else if (g_pMain->m_sBifrostRemainingTime == g_pMain->m_xBifrostMonumentAttackTime)
			{
				g_pMain->SendFormattedResource(IDS_BEEF_ROAST_START, Nation::ALL, false);

				if (!g_pMain->m_bAttackBifrostMonument)
					g_pMain->m_bAttackBifrostMonument = true;
			}
		}
		else if (g_pMain->m_BifrostVictory != 0)
		{
			g_pMain->m_sBifrostTime -= 60;
			g_pMain->m_sBifrostRemainingTime = g_pMain->m_sBifrostTime;

			if (g_pMain->m_sBifrostTime == 0)
			{
				g_pMain->m_BifrostVictory = 0;
				g_pMain->m_sBifrostRemainingTime = g_pMain->m_xBifrostRemainingTime;
				g_pMain->m_sBifrostTime = 0;
				g_pMain->SendFormattedResource(IDS_BEEF_ROAST_FINISH, Nation::ALL, false);

				if (g_pMain->m_bAttackBifrostMonument)
					g_pMain->m_bAttackBifrostMonument = false;

				g_pMain->SendEventRemainingTime(true, nullptr, ZONE_BIFROST);
			}
			else if (g_pMain->m_sBifrostTime == g_pMain->m_xJoinOtherNationBifrostTime)
			{
				if (!g_pMain->m_bAttackBifrostMonument)
					g_pMain->m_bAttackBifrostMonument = true;
			}
		} 

		sleep(60 * SECOND);
	}
	return 0;
}

uint32 CGameServerDlg::Timer_UpdateGameTime(void * lpParam)
{
	while (g_bRunning)
	{
		g_pMain->UpdateGameTime();

		if (++g_pMain->m_sErrorSocketCount > 3)
			g_pMain->AIServerConnect();

		sleep(6 * SECOND);
	}
	return 0;
}

uint32 CGameServerDlg::Timer_UpdateSessions(void * lpParam)
{
	while (g_bRunning)
	{
		SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
		foreach (itr, sessMap)
		{
			CUser * pUser = TO_USER(itr->second);

#ifndef DEBUG // ignore timeouts in debug builds, as we'll probably be pausing it with the debugger.
			uint32 timeout = KOSOCKET_TIMEOUT;

			// User has authed, but isn't in-game yet (creating a character, or is waiting for the game to load).
			if (!pUser->m_strAccountID.empty() && !pUser->isInGame())
				timeout = KOSOCKET_LOADING_TIMEOUT;

			// Disconnect timed out sessions
			if ((UNIXTIME - pUser->GetLastResponseTime()) >= timeout)
			{
				pUser->Disconnect();
				continue;
			}
#endif
			// Update the player, and hence any skill effects while we're here.
			if (pUser->isInGame())
				pUser->Update();
		}
		sleep(30 * SECOND);
	}
	return 0;
}

uint32 CGameServerDlg::Timer_UpdateConcurrent(void * lpParam)
{
	while (g_bRunning)
	{
		g_pMain->ReqUpdateConcurrent();
		sleep(60 * SECOND);
	}
	return 0;
}

void CGameServerDlg::ReqUpdateConcurrent()
{
	Packet result(WIZ_ZONE_CONCURRENT);
	result	<< uint32(m_nServerNo)
		<< uint32(m_socketMgr.GetActiveSessionMap().size());
	AddDatabaseRequest(result);
}

void CGameServerDlg::AIServerConnect()
{
	// Are there any (note: we only use 1 now) idle/disconnected sessions?
	SessionMap & sessMap = m_aiSocketMgr.GetIdleSessionMap();

	// Copy the map (should only be 1 socket anyway) to avoid breaking the iterator
	SessionMap idleSessions = sessMap;

	// No idle sessions? Excellent.
	if (idleSessions.empty())
		return;

	// Attempt reconnecting to the server
	foreach (itr, idleSessions)
	{
		CAISocket *pSock = static_cast<CAISocket *>(itr->second);
		bool bReconnecting = pSock->IsReconnecting();
		if (!pSock->Connect(m_AIServerIP.c_str(), m_AIServerPort)) // couldn't connect... let's leave you alone for now
			continue;

		// Connected! Now send the connection packet.
		Packet result(AI_SERVER_CONNECT);
		result << bReconnecting;
		pSock->Send(&result);

		TRACE("**** AISocket Connect Success!! , server = %s:%d ****\n", pSock->GetRemoteIP().c_str(), pSock->GetRemotePort());
	}
}

/**
* @brief	Sends a packet to all users in the zone matching the specified class types.
*
* @param	pkt				   	The packet.
* @param	bZoneID			   	Identifier for the zone.
* @param	pExceptUser		   	The except user.
* @param	nation			   	The nation.
* @param	seekingPartyOptions	Bitmask of classes to send to.
*/
void CGameServerDlg::Send_Zone_Matched_Class(Packet *pkt, uint8 bZoneID, CUser* pExceptUser, 
											 uint8 nation, uint8 seekingPartyOptions, uint16 nEventRoom)
{
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach (itr, sessMap)
	{
		CUser * pUser = TO_USER(itr->second);
		if (pUser == pExceptUser 
			|| !pUser->isInGame()
			|| pUser->GetZoneID() != bZoneID
			|| pUser->isInParty()) // looking for users to join the party
			continue;

		if (nEventRoom > 0 && nEventRoom != pUser->GetEventRoom())
			continue;

		// If we're in the neutral zone (Moradon), it doesn't matter which nation we party with.
		// For all other zones, we must party with a player of the same nation.
		if (pUser->GetZoneID() == ZONE_MORADON 
			|| pUser->GetNation() == nation)
		{
			if (	((seekingPartyOptions & 1) && pUser->JobGroupCheck(ClassWarrior))
				||	((seekingPartyOptions & 2) && pUser->JobGroupCheck(ClassRogue))
				||	((seekingPartyOptions & 4) && pUser->JobGroupCheck(ClassMage))
				||	((seekingPartyOptions & 8) && pUser->JobGroupCheck(ClassPriest)))
				pUser->Send(pkt);
		}
	}
}

/**
* @brief	Sends a packet to all players in the specified zone
* 			matching the specified criteria.
*
* @param	pkt		   	The packet.
* @param	bZoneID	   	Zone's identifier.
* @param	pExceptUser	User to except. If specified, will ignore this user.
* @param	nation	   	Nation to allow. If unspecified, will default to Nation::ALL 
* 						which will send to all/both nations.
*/
void CGameServerDlg::Send_Zone(Packet *pkt, uint8 bZoneID, CUser* pExceptUser /*= nullptr*/, uint8 nation /*= 0*/, uint16 nEventRoom, float fRange)
{
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach (itr, sessMap)
	{
		CUser * pUser = TO_USER(itr->second);

		if (!pUser->isInGame()
			|| pUser->GetZoneID() != bZoneID
			|| (nation != Nation::ALL && nation != pUser->GetNation()))
		{
			if (pExceptUser != nullptr)
			{
				if (pUser == pExceptUser
					|| (fRange > 0.0f && pUser->isInRange(pExceptUser, fRange)))
					continue;
			}

			continue;
		}

		if (nEventRoom > 0 && nEventRoom != pUser->GetEventRoom())
			continue;

		pUser->Send(pkt);
	}
}

/**
* @brief	Sends a packet to all users connected and logged into the server.
*
* @param	pkt		   	The packet.
* @param	pExceptUser	User to except. If specified, will ignore this user.
* @param	nation	   	Nation to allow. If unspecified, will default to Nation::ALL 
* 						which will send to all/both nations.
*/
void CGameServerDlg::Send_All(Packet *pkt, CUser* pExceptUser /*= nullptr*/, uint8 nation /*= 0*/, 
							  uint8 ZoneID /*= 0*/, bool isSendEventUsers /* false */, uint16 nEventRoom /*= -1*/)
{
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach (itr, sessMap)
	{
		CUser * pUser = TO_USER(itr->second);
		if (pUser == pExceptUser 
			|| !pUser->isInGame()
			|| (nation != Nation::ALL && nation != pUser->GetNation()))
			continue;

		if (ZoneID != 0)
			if (pUser->GetZoneID() != ZoneID) 
				continue;

		if (nEventRoom > 0 && nEventRoom != pUser->GetEventRoom())
			continue;

		if (isSendEventUsers && !pUser->isEventUser())
			continue;

		pUser->Send(pkt);
	}
}

void CGameServerDlg::Send_Region(Packet *pkt, C3DMap *pMap, int x, int z, CUser* pExceptUser, uint16 nEventRoom)
{
	foreach_region(rx, rz)
		Send_UnitRegion(pkt, pMap, rx + x, rz + z, pExceptUser, nEventRoom);
}

void CGameServerDlg::Send_UnitRegion(Packet *pkt, C3DMap *pMap, int x, int z, CUser *pExceptUser, uint16 nEventRoom)
{
	if (pMap == nullptr)
		return;

	Guard lock(pMap->m_lock);
	CRegion *pRegion = pMap->GetRegion(x, z);
	if (pRegion == nullptr)
		return;

	Guard lock2(pRegion->m_lock);

	if (pRegion->m_RegionUserArray.size() <= 0)
		return;

	// Protect array changes...
	ZoneUserArray cm_RegionUserArray = pRegion->m_RegionUserArray;
	foreach (itr, cm_RegionUserArray)
	{
		CUser *pUser = GetUserPtr(*itr);
		if (pUser == nullptr 
			|| pUser == pExceptUser 
			|| !pUser->isInGame())
			continue;

		if (nEventRoom > 0 && nEventRoom != pUser->GetEventRoom())
			continue;

		pUser->Send(pkt);
	}
}

// TODO: Move the following two methods into a base CUser/CNpc class
void CGameServerDlg::Send_OldRegions(Packet *pkt, int old_x, int old_z, C3DMap *pMap, int x, int z, CUser* pExceptUser, uint16 nEventRoom)
{
	if (old_x != 0)
	{
		Send_UnitRegion(pkt, pMap, x+old_x*2, z+old_z-1, nullptr, nEventRoom);
		Send_UnitRegion(pkt, pMap, x+old_x*2, z+old_z, nullptr, nEventRoom);
		Send_UnitRegion(pkt, pMap, x+old_x*2, z+old_z+1, nullptr, nEventRoom);
	}

	if (old_z != 0)
	{
		Send_UnitRegion(pkt, pMap, x+old_x, z+old_z*2, nullptr, nEventRoom);
		if (old_x < 0)
			Send_UnitRegion(pkt, pMap, x+old_x+1, z+old_z*2, nullptr, nEventRoom);
		else if (old_x > 0)
			Send_UnitRegion(pkt, pMap, x+old_x-1, z+old_z*2, nullptr, nEventRoom);
		else
		{
			Send_UnitRegion(pkt, pMap, x+old_x-1, z+old_z*2, nullptr, nEventRoom);
			Send_UnitRegion(pkt, pMap, x+old_x+1, z+old_z*2, nullptr, nEventRoom);
		}
	}
}

void CGameServerDlg::Send_NewRegions(Packet *pkt, int new_x, int new_z, C3DMap *pMap, int x, int z, CUser* pExceptUser, uint16 nEventRoom)
{
	if (new_x != 0)
	{
		Send_UnitRegion(pkt, pMap, x+new_x, z-1, nullptr, nEventRoom);
		Send_UnitRegion(pkt, pMap, x+new_x, z, nullptr, nEventRoom);
		Send_UnitRegion(pkt, pMap, x+new_x, z+1, nullptr, nEventRoom);
	}

	if (new_z != 0)
	{
		Send_UnitRegion(pkt, pMap, x, z+new_z);

		if (new_x < 0)
			Send_UnitRegion(pkt, pMap, x+1, z+new_z, nullptr, nEventRoom);
		else if (new_x > 0)
			Send_UnitRegion(pkt, pMap, x-1, z+new_z, nullptr, nEventRoom);
		else 
		{
			Send_UnitRegion(pkt, pMap, x-1, z+new_z, nullptr, nEventRoom);
			Send_UnitRegion(pkt, pMap, x+1, z+new_z, nullptr, nEventRoom);
		}
	}
}

void CGameServerDlg::Send_NearRegion(Packet *pkt, C3DMap *pMap, int region_x, int region_z, float curx, float curz, CUser* pExceptUser, uint16 nEventRoom)
{
	int left_border = region_x * VIEW_DISTANCE, top_border = region_z * VIEW_DISTANCE;
	Send_FilterUnitRegion(pkt, pMap, region_x, region_z, curx, curz, pExceptUser, nEventRoom);
	if( ((curx - left_border) > (VIEW_DISTANCE/2.0f)) ) {			// RIGHT
		if( ((curz - top_border) > (VIEW_DISTANCE/2.0f)) ) {	// BOTTOM
			Send_FilterUnitRegion(pkt, pMap, region_x+1, region_z, curx, curz, pExceptUser, nEventRoom);
			Send_FilterUnitRegion(pkt, pMap, region_x, region_z+1, curx, curz, pExceptUser, nEventRoom);
			Send_FilterUnitRegion(pkt, pMap, region_x+1, region_z+1, curx, curz, pExceptUser, nEventRoom);
		}
		else {													// TOP
			Send_FilterUnitRegion(pkt, pMap, region_x+1, region_z, curx, curz, pExceptUser, nEventRoom);
			Send_FilterUnitRegion(pkt, pMap, region_x, region_z-1, curx, curz, pExceptUser, nEventRoom);
			Send_FilterUnitRegion(pkt, pMap, region_x+1, region_z-1, curx, curz, pExceptUser, nEventRoom);
		}
	}
	else {														// LEFT
		if( ((curz - top_border) > (VIEW_DISTANCE/2.0f)) ) {	// BOTTOM
			Send_FilterUnitRegion(pkt, pMap, region_x-1, region_z, curx, curz, pExceptUser, nEventRoom);
			Send_FilterUnitRegion(pkt, pMap, region_x, region_z+1, curx, curz, pExceptUser, nEventRoom);
			Send_FilterUnitRegion(pkt, pMap, region_x-1, region_z+1, curx, curz, pExceptUser, nEventRoom);
		}
		else {													// TOP
			Send_FilterUnitRegion(pkt, pMap, region_x-1, region_z, curx, curz, pExceptUser, nEventRoom);
			Send_FilterUnitRegion(pkt, pMap, region_x, region_z-1, curx, curz, pExceptUser, nEventRoom);
			Send_FilterUnitRegion(pkt, pMap, region_x-1, region_z-1, curx, curz, pExceptUser, nEventRoom);
		}
	}
}

void CGameServerDlg::Send_FilterUnitRegion(Packet *pkt, C3DMap *pMap, int x, int z, float ref_x, float ref_z, CUser *pExceptUser, uint16 nEventRoom)
{
	if (pMap == nullptr)
		return;

	Guard lock(pMap->m_lock);
	CRegion *pRegion = pMap->GetRegion(x, z);
	if (pRegion == nullptr)
		return;

	Guard lock2(pRegion->m_lock);

	if (pRegion->m_RegionUserArray.size() <= 0)
		return;

	foreach (itr, pRegion->m_RegionUserArray)
	{
		CUser *pUser = GetUserPtr(*itr);
		if (pUser == nullptr 
			|| pUser == pExceptUser 
			|| !pUser->isInGame())
			continue;

		if (nEventRoom > 0 && nEventRoom != pUser->GetEventRoom())
			continue;

		if (sqrt(pow((pUser->m_curx - ref_x), 2) + pow((pUser->m_curz - ref_z), 2)) < 32)
			pUser->Send(pkt);
	}
}

void CGameServerDlg::Send_PartyMember(int party, Packet *result)
{
	_PARTY_GROUP* pParty = GetPartyPtr(party);
	if (pParty == nullptr)
		return;

	for (int i = 0; i < MAX_PARTY_USERS; i++)
	{
		CUser *pUser = GetUserPtr(pParty->uid[i]);
		if (pUser == nullptr)
			continue;

		pUser->Send(result);
	}
}

void CGameServerDlg::Send_KnightsMember(int index, Packet *pkt)
{
	CKnights* pKnights = GetClanPtr(index);
	if (pKnights == nullptr)
		return;

	pKnights->Send(pkt);
}

void CGameServerDlg::Send_KnightsAlliance(uint16 sAllianceID, Packet *pkt)
{
	_KNIGHTS_ALLIANCE* pAlliance = GetAlliancePtr(sAllianceID);
	if (pAlliance == nullptr)
		return;

	Send_KnightsMember(pAlliance->sMainAllianceKnights, pkt);
	Send_KnightsMember(pAlliance->sSubAllianceKnights, pkt);
	Send_KnightsMember(pAlliance->sMercenaryClan_1, pkt);
	Send_KnightsMember(pAlliance->sMercenaryClan_2, pkt);
}

void CGameServerDlg::Send_AIServer(Packet *pkt)
{
	m_aiSocketMgr.SendAll(pkt);
}

void CGameServerDlg::UpdateGameTime()
{
	DateTime now(&g_localTime);

	// Check timed King events.
	foreach_stlmap (itr, m_KingSystemArray)
		itr->second->CheckKingTimer();

	// Every minute
	if (m_sMin != now.GetMinute())
	{
		m_ReloadKnightAndUserRanksMinute++;

		if (m_ReloadKnightAndUserRanksMinute == RELOAD_KNIGHTS_AND_USER_RATING)
		{
			m_ReloadKnightAndUserRanksMinute = 0;
			ReloadKnightAndUserRanks();
		}

		// Player Ranking Rewards
		std::list<std::string> vargs = StrSplit(m_sPlayerRankingsRewardZones, ",");
		uint8 nZones = vargs.size();
		if (nZones > 0)
		{
			uint8 nZoneID = 0;
			for (int i = 0; i < nZones; i++)
			{
				nZoneID = atoi(vargs.front().c_str());
				SetPlayerRankingRewards(nZoneID);
				vargs.pop_front();
			}
		}
	}

	// Every hour
	if (m_sHour != now.GetHour())
	{
		ResetPlayerRankings();
		UpdateWeather();
		SetGameTime();

		if (m_bSantaOrAngel)
			SendFlyingSantaOrAngel();
	}

	// Every day
	if (m_sDate != now.GetDay())
	{

	}

	// Every month
	if (m_sMonth != now.GetMonth())
	{
		// Reset monthly NP.
		ResetLoyaltyMonthly();
	}

	// Update the server time
	m_sYear = now.GetYear();
	m_sMonth = now.GetMonth();
	m_sDate = now.GetDay();
	m_sHour = now.GetHour();
	m_sMin = now.GetMinute();

	Packet result(AG_TIME_WEATHER);
	result << m_sYear << m_sMonth << m_sDate << m_sHour << m_sMin << m_byWeather << m_sWeatherAmount;
	Send_AIServer(&result);
}

void CGameServerDlg::ResetLoyaltyMonthly()
{
	SessionMap sessMap = m_socketMgr.GetActiveSessionMap();
	foreach (itr, sessMap)
	{
		CUser * pUser = TO_USER(itr->second);
		pUser->m_iLoyaltyMonthly = 0;
		pUser->SendLoyaltyChange(); // update the client (note: official doesn't bother)
	}

	// Attempt to update the database in this thread directly, while the session map's locked.
	// This prevents new users from logging in before they've been reset (hence keeping last month's total).
	g_DBAgent.ResetLoyaltyMonthly();
}

void CGameServerDlg::UpdateWeather()
{
	if (m_byKingWeatherEvent)
	{
		int16 sEventExpiry;
		if (g_localTime.tm_mday == m_byKingWeatherEvent_Day)
			sEventExpiry = g_localTime.tm_min + 60 * (g_localTime.tm_hour - m_byKingWeatherEvent_Hour) - m_byKingWeatherEvent_Minute;
		else
			sEventExpiry = g_localTime.tm_min + 60 * (g_localTime.tm_hour - m_byKingWeatherEvent_Hour + 24) - m_byKingWeatherEvent_Minute;

		// Weather events last for 5 minutes
		if (sEventExpiry > 5)
		{
			m_byKingWeatherEvent = 0;
			m_byKingWeatherEvent_Day = 0;
			m_byKingWeatherEvent_Hour = 0;
			m_byKingWeatherEvent_Minute = 0;
		}
	}
	else
	{
		int weather = 0, rnd = myrand( 0, 100 );
		if (rnd < 2)		weather = WEATHER_SNOW;
		else if (rnd < 7)	weather = WEATHER_RAIN;
		else				weather = WEATHER_FINE;

		m_sWeatherAmount = myrand(0, 100);
		if (weather == WEATHER_FINE)
		{
			if (m_sWeatherAmount > 70)
				m_sWeatherAmount /= 2;
			else
				m_sWeatherAmount = 0;
		}
		m_byWeather = weather;
	}

	// Real weather data for most users.
	Packet realWeather(WIZ_WEATHER, m_byWeather);
	realWeather << m_sWeatherAmount;

	// Fake, clear weather for users in certain zones (e.g. Desp & Hell Abysses, Arena)
	Packet fakeWeather(WIZ_WEATHER, uint8(WEATHER_FINE));
	fakeWeather << m_sWeatherAmount;

	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach (itr, sessMap)
	{
		CUser * pUser = TO_USER(itr->second);
		if (!pUser->isInGame())
			continue;

		if (pUser->GetZoneID() == 32 
			|| pUser->GetZoneID() == 33
			|| pUser->GetZoneID() == 48)
			pUser->Send(&fakeWeather);
		else
			pUser->Send(&realWeather);
	}
}

void CGameServerDlg::SetGameTime()
{
	CIni ini(CONF_GAME_SERVER);
	ini.SetInt( "TIMER", "WEATHER", m_byWeather );
}

void CGameServerDlg::ResetPlayerRankings(uint8 ZoneID)
{
	if (m_sRankResetHour == m_nPlayerRankingResetTime || ZoneID > 0)
	{
		m_IsPlayerRankingUpdateProcess = true;
		m_sRankResetHour = 0;

		for (int nation = KARUS_ARRAY; nation <= ELMORAD_ARRAY; nation++)
		{
			foreach_stlmap (itr, g_pMain->m_UserRankingArray[nation])
			{
				_USER_RANKING * pRankInfo = g_pMain->m_UserRankingArray[nation].GetData(itr->first);

				if (pRankInfo == nullptr)
					continue;

				CUser * pUser = g_pMain->GetUserPtr(pRankInfo->m_socketID);

				if (pUser == nullptr 
					|| pUser->isInTempleEventZone()
					|| (ZoneID > 0 && pUser->GetZoneID() != ZoneID))
					continue;

				pRankInfo->m_iLoyaltyDaily = 0;
				pRankInfo->m_iLoyaltyPremiumBonus = 0;
				pUser->m_iLoyaltyDaily = 0;
				pUser->m_iLoyaltyPremiumBonus = 0;
			}
		}

		m_IsPlayerRankingUpdateProcess = false; 
		return;
	}

	m_sRankResetHour++;
}

void CGameServerDlg::AddDatabaseRequest(Packet & pkt, CUser *pUser /*= nullptr*/)
{
	Packet *newPacket = new Packet(pkt.GetOpcode(), pkt.size() + 2);
	*newPacket << int16(pUser == nullptr ? -1 : pUser->GetSocketID());
	if (pkt.size())
		newPacket->append(pkt.contents(), pkt.size());
	DatabaseThread::AddRequest(newPacket);
}

void CGameServerDlg::UserInOutForMe(CUser *pSendUser)
{
	if (pSendUser == nullptr)
		return;

	Packet result(WIZ_REQ_USERIN);
	C3DMap* pMap = pSendUser->GetMap();
	ASSERT(pMap != nullptr);
	uint16 user_count = 0;

	result << uint16(0); // placeholder for the user count

	int16 rx = pSendUser->GetRegionX(), rz = pSendUser->GetRegionZ();
	foreach_region(x, z)
		GetRegionUserIn(pMap, rx + x, rz + z, result, user_count, pSendUser->GetEventRoom());

	result.put(0, uint16(user_count));
	pSendUser->SendCompressed(&result);
}

void CGameServerDlg::RegionUserInOutForMe(CUser *pSendUser)
{
	if (pSendUser == nullptr)
		return;

	Packet result(WIZ_REGIONCHANGE, uint8(1));
	C3DMap* pMap = pSendUser->GetMap();
	ASSERT(pMap != nullptr);
	uint16 user_count = 0;

	result << uint16(0); // placeholder for the user count

	int16 rx = pSendUser->GetRegionX(), rz = pSendUser->GetRegionZ();
	foreach_region(x, z)
		GetRegionUserList(pMap, rx + x, rz + z, result, user_count, pSendUser->GetEventRoom());

	result.put(1, user_count);
	pSendUser->SendCompressed(&result);
}

void CGameServerDlg::GetRegionUserIn(C3DMap *pMap, uint16 region_x, uint16 region_z, Packet & pkt, uint16 & t_count, uint16 nEventRoom)
{
	if (pMap == nullptr)
		return;

	Guard lock(pMap->m_lock);
	CRegion *pRegion = pMap->GetRegion(region_x, region_z);
	if (pRegion == nullptr)
		return;

	Guard lock2(pRegion->m_lock);

	if (pRegion->m_RegionUserArray.size() <= 0)
		return;

	foreach (itr, pRegion->m_RegionUserArray)
	{
		CUser *pUser = GetUserPtr(*itr);
		if (pUser == nullptr 
			|| !pUser->isInGame())
			continue;

		if (nEventRoom > 0 && nEventRoom != pUser->GetEventRoom())
			continue;

		pkt << uint8(0) << pUser->GetSocketID();
		pUser->GetUserInfo(pkt);
		t_count++;
	}
}

void CGameServerDlg::GetRegionUserList(C3DMap* pMap, uint16 region_x, uint16 region_z, Packet & pkt, uint16 & t_count, uint16 nEventRoom)
{
	if (pMap == nullptr)
		return;

	Guard lock(pMap->m_lock);
	CRegion *pRegion = pMap->GetRegion(region_x, region_z);
	if (pRegion == nullptr)
		return;

	Guard lock2(pRegion->m_lock);

	if (pRegion->m_RegionUserArray.size() <= 0)
		return;

	foreach (itr, pRegion->m_RegionUserArray)
	{
		CUser *pUser = GetUserPtr(*itr);
		if (pUser == nullptr 
			|| !pUser->isInGame())
			continue;

		if (nEventRoom > 0 && nEventRoom != pUser->GetEventRoom())
			continue;

		pkt << pUser->GetSocketID();
		t_count++;
	}
}

void CGameServerDlg::MerchantUserInOutForMe(CUser *pSendUser)
{
	if (pSendUser == nullptr)
		return;

	Packet result(WIZ_MERCHANT_INOUT, uint8(1));
	C3DMap* pMap = pSendUser->GetMap();
	ASSERT(pMap != nullptr);
	uint16 user_count = 0;

	result << uint16(0); // placeholder for user count

	int16 rx = pSendUser->GetRegionX(), rz = pSendUser->GetRegionZ();
	foreach_region(x, z)
		GetRegionMerchantUserIn(pMap, rx + x, rz + z, result, user_count, pSendUser->GetEventRoom());

	result.put(1, user_count);
	pSendUser->SendCompressed(&result);
}

void CGameServerDlg::GetRegionMerchantUserIn(C3DMap *pMap, uint16 region_x, uint16 region_z, Packet & pkt, uint16 & t_count, uint16 nEventRoom)
{
	if (pMap == nullptr)
		return;

	Guard lock(pMap->m_lock);
	CRegion *pRegion = pMap->GetRegion(region_x, region_z);
	if (pRegion == nullptr)
		return;

	Guard lock2(pRegion->m_lock);

	if (pRegion->m_RegionUserArray.size() <= 0)
		return;

	foreach (itr, pRegion->m_RegionUserArray)
	{
		CUser *pUser = GetUserPtr(*itr);
		if (pUser == nullptr 
			|| !pUser->isInGame()
			|| !pUser->isMerchanting())
			continue;

		if (nEventRoom > 0 && nEventRoom != pUser->GetEventRoom())
			continue;

		pkt << pUser->GetSocketID()
			<< pUser->GetMerchantState() // 0 is selling, 1 is buying
			<< (pUser->GetMerchantState() == 1 ? false : pUser->m_bPremiumMerchant); // Type of merchant [normal - gold] // bool

		for (int i = 0, listCount = (pUser->GetMerchantState() == 1 ? 4 : (pUser->m_bPremiumMerchant ? 8 : 4)); i < listCount; i++)
			pkt << pUser->m_arMerchantItems[i].nNum;

		t_count++;
	}
}

void CGameServerDlg::NpcInOutForMe(CUser* pSendUser)
{
	if (pSendUser == nullptr)
		return;

	Packet result(WIZ_REQ_NPCIN);
	C3DMap* pMap = pSendUser->GetMap();
	ASSERT(pMap != nullptr);
	uint16 npc_count = 0;
	result << uint16(0); // placeholder for NPC count

	int16 rx = pSendUser->GetRegionX(), rz = pSendUser->GetRegionZ();
	foreach_region(x, z)
		GetRegionNpcIn(pMap, rx + x, rz + z, result, npc_count, pSendUser->GetEventRoom());

	result.put(0, npc_count);
	pSendUser->SendCompressed(&result);
}

void CGameServerDlg::GetRegionNpcIn(C3DMap *pMap, uint16 region_x, uint16 region_z, Packet & pkt, uint16 & t_count, uint16 nEventRoom)
{
	if (!m_bPointCheckFlag
		|| pMap == nullptr)
		return;

	Guard lock(pMap->m_lock);
	CRegion * pRegion = pMap->GetRegion(region_x, region_z);

	if (pRegion == nullptr)
		return;

	Guard lock2(pRegion->m_lock);

	if (pRegion->m_RegionNpcArray.size() <= 0)
		return;

	foreach (itr, pRegion->m_RegionNpcArray)
	{
		CNpc *pNpc = GetNpcPtr(*itr);
		if (pNpc == nullptr
			|| pNpc->isDead())
			continue;

		if (nEventRoom > 0 && nEventRoom != pNpc->m_bEventRoom && pNpc->GetProtoID() != SAW_BLADE_SSID)
			continue;

		pkt << pNpc->GetID();
		pNpc->GetNpcInfo(pkt);
		t_count++;
	}
}

void CGameServerDlg::RegionNpcInfoForMe(CUser *pSendUser)
{
	if (pSendUser == nullptr)
		return;

	Packet result(WIZ_NPC_REGION);
	C3DMap* pMap = pSendUser->GetMap();
	ASSERT(pMap != nullptr);
	uint16 npc_count = 0;
	result << uint16(0); // placeholder for NPC count

	int16 rx = pSendUser->GetRegionX(), rz = pSendUser->GetRegionZ();
	foreach_region(x, z)
		GetRegionNpcList(pMap, rx + x, rz + z, result, npc_count, pSendUser->GetEventRoom());

	result.put(0, npc_count);
	pSendUser->SendCompressed(&result);
}

void CGameServerDlg::GetUnitListFromSurroundingRegions(Unit * pOwner, std::vector<uint16> * pList)
{
	if (pOwner == nullptr)
		return;

	C3DMap * pMap = pOwner->GetMap();
	ASSERT(pMap != nullptr);

	int16 rx = pOwner->GetRegionX(), rz = pOwner->GetRegionZ();

	Guard lock(pMap->m_lock);
	foreach_region(x, z)
	{
		uint16 region_x = rx + x, region_z = rz + z;
		CRegion * pRegion = pMap->GetRegion(region_x, region_z);

		if (pRegion == nullptr)
			continue;

		Guard lock2(pRegion->m_lock);

		// Add all potential NPCs to list
		foreach (itr, pRegion->m_RegionNpcArray)
			pList->push_back(*itr);

		// Add all potential users to list
		foreach (itr, pRegion->m_RegionUserArray)
			pList->push_back(*itr);
	}
}

void CGameServerDlg::GetRegionNpcList(C3DMap *pMap, uint16 region_x, uint16 region_z, Packet & pkt, uint16 & t_count, uint16 nEventRoom)
{
	if (!m_bPointCheckFlag
		|| pMap == nullptr)
		return;

	Guard lock(pMap->m_lock);
	CRegion * pRegion = pMap->GetRegion(region_x, region_z);

	if (pRegion == nullptr)
		return;

	Guard lock2(pRegion->m_lock);

	if (pRegion->m_RegionNpcArray.size() <= 0)
		return;

	foreach (itr, pRegion->m_RegionNpcArray)
	{
		CNpc *pNpc = GetNpcPtr(*itr);
		if (pNpc == nullptr || pNpc->isDead())
			continue;

		if (nEventRoom > 0 && nEventRoom != pNpc->m_bEventRoom && pNpc->GetProtoID() != SAW_BLADE_SSID)
			continue;

		pkt << pNpc->GetID();
		t_count++;
	}
}

void CGameServerDlg::HandleConsoleCommand(const char * msg) 
{
	string message = msg;
	if (message.empty())
		return;

	if (ProcessServerCommand(message))
	{
		printf("Command accepted.\n");
		return;
	}

	printf("Invalid command. If you're trying to send a notice, please use /notice\n");
}

bool CGameServerDlg::LoadNoticeData()
{
	ifstream file("./Notice.txt");
	string line;
	int count = 0;

	// Clear out the notices first
	memset(&m_ppNotice, 0, sizeof(m_ppNotice));

	if (!file)
	{
		TRACE("Notice.txt could not be opened.\n");
		return false;
	}

	while (!file.eof())
	{
		if (count > 19)
		{
			TRACE("Too many lines in Notice.txt\n");
			break;
		}

		getline(file, line);
		if (line.length() > 128)
		{
			TRACE("Notice.txt contains line that exceeds the limit of 128 characters.\n");
			break;
		}

		strcpy(m_ppNotice[count++], line.c_str());
	}

	file.close();
	return true;
}

void CGameServerDlg::SendAllUserInfo()
{
	Packet result(AG_USER_INFO_ALL);
	uint8 count = 0;
	result << count; // placeholder for user count
	const int tot = 20;

	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach (itr, sessMap)
	{
		TO_USER(itr->second)->GetUserInfoForAI(result);
		if (++count == tot)	{
			result.put(0, count);
			Send_AIServer(&result);
			count = 0;
			result.clear();
		}
	}

	if (count != 0 && count < (tot - 1))
	{
		result.put(0, count);
		Send_AIServer(&result);
		count = 0;
		result.clear();
	}

	foreach_stlmap (itr, m_PartyArray)
	{
		_PARTY_GROUP *pParty = itr->second;
		if (pParty == nullptr) 
			continue;

		result.Initialize(AG_PARTY_INFO_ALL);
		result << uint16(itr->first);
		for (int i = 0; i < MAX_PARTY_USERS; i++)
			result << pParty->uid[i];

		Send_AIServer(&result);
	}
}

void CGameServerDlg::DeleteAllNpcList(int flag)
{
	if (!m_bServerCheckFlag
		|| !m_bPointCheckFlag)
		return;

	TRACE("[Monster Point Delete]\n");
	TRACE("*** DeleteAllNpcList - Start *** \n");

	// Remove spawns from users to prevent them from getting bugged...
	m_arNpcArray.m_lock.lock();
	foreach_stlmap_nolock (itr, m_arNpcArray)
	{
		if (itr->second->isAlive())
			itr->second->SendInOut(INOUT_OUT, 0.0f, 0.0f, 0.0f);

		// decrease the reference count (freeing it if nothing else is using it)
		itr->second->DecRef();
	}

	// all the data should now be freed (if not, it will be by whatever's using it)
	m_arNpcArray.m_UserTypeMap.clear();
	m_arNpcArray.m_lock.unlock();

	// Now remove all spawns from all regions
	foreach_stlmap (itr, m_ZoneArray)
	{
		C3DMap *pMap = itr->second;
		if (pMap == nullptr)
			continue;

		Guard lock(pMap->m_lock);
		for (int i = 0; i < pMap->GetXRegionMax(); i++)
		{
			for (int j = 0; j < pMap->GetZRegionMax(); j++)
				pMap->m_ppRegion[i][j].m_RegionNpcArray.clear();
		}
	}
	m_bServerCheckFlag = false;

	TRACE("*** DeleteAllNpcList - End *** \n");
}

/**
* @brief	Searches for the first NPC in the specified zone
* 			with the specified picture/model ID.
*
* @param	sPid	Picture/model ID of the NPC.
* @param	byZone	Zone to search in.
*
* @return	null if it fails, else the NPC instance we found.
*/
CNpc*  CGameServerDlg::FindNpcInZone(uint16 sPid, uint8 byZone)
{
	if (!m_bPointCheckFlag)
		return nullptr;

	Guard lock(m_arNpcArray.m_lock);
	foreach_stlmap (itr, m_arNpcArray)
	{
		CNpc * pNpc = itr->second;
		if (pNpc == nullptr || pNpc->GetZoneID() != byZone
			// This isn't a typo, it's actually just a hack.
				// The picture/model ID of most spawns is the same as their prototype ID.
					// When there's more than one spawn prototype (i.e. different sSid), we keep using
						// the same picture/model ID. So we check this instead of the sSid...
							|| pNpc->m_sPid != sPid) 
							continue;

		return pNpc;
	}

	return nullptr;
}

void CGameServerDlg::BattleZoneOpenTimer()
{
	int nWeekDay = g_localTime.tm_wday;
	int nHour = g_localTime.tm_hour;

	if (!isWarOpen())
	{
		std::list<std::string> vargs = StrSplit(m_sBattleZoneOpenDays, ",");
		uint8 nDaySize = vargs.size();
		if (nDaySize > 0)
		{
			uint8 nDay = 0;
			for (int i = 0; i < nDaySize; i++)
			{
				nDay = atoi(vargs.front().c_str());
				if (nDay == nWeekDay)
				{
					for (int x = 0; x < WAR_TIME_COUNT; x++)
					{
						if (m_nBattleZoneOpenHourStart[x] == nHour)
							BattleZoneOpen(BATTLEZONE_OPEN, m_nBattlezoneOpenWarZone[x]);
					}
				}
				vargs.pop_front();
			}
		}
	}
	else if (m_byBattleOpen == NATION_BATTLE)	
	{
		BattleZoneCurrentUsers();

		int32 WarElapsedTime = int32(UNIXTIME) - m_byBattleOpenedTime;
		m_byBattleRemainingTime = m_byBattleTime - WarElapsedTime;
		uint8 nBattleZone = g_pMain->m_byBattleZone + ZONE_BATTLE_BASE;

		if (m_bVictory == 0)
		{
			if (WarElapsedTime == (m_byBattleTime / 24)) // Select captain
			{
				m_KnightsRatingArray[KARUS_ARRAY].DeleteAllData();
				m_KnightsRatingArray[ELMORAD_ARRAY].DeleteAllData();
				LoadKnightsRankTable(true, true);
			}
			else if (WarElapsedTime == (m_byBattleTime / 8)) // War half time + 15 minutes for Nereid's Island.
			{
				if (nBattleZone == ZONE_BATTLE4) // Nereid's Island
				{
					if (m_sKarusMonuments >= 7 && m_sElmoMonuments == 0)
						BattleZoneResult(KARUS);
					else if (m_sKarusMonuments == 0 && m_sElmoMonuments >= 7)
						BattleZoneResult(ELMORAD);
				}
			}
			else if (WarElapsedTime == (m_byBattleTime / 2)) // War half time.
			{
				if (nBattleZone == ZONE_BATTLE
					|| nBattleZone == ZONE_BATTLE2 
					||nBattleZone == ZONE_BATTLE3)
					BattleWinnerResult(BATTLE_WINNER_NPC);
				else if (nBattleZone == ZONE_BATTLE4) // Nereid's Island
					BattleWinnerResult(BATTLE_WINNER_MONUMENT);
				else if (nBattleZone == ZONE_BATTLE6) // Oreads
					BattleWinnerResult(BATTLE_WINNER_KILL);
			}

			m_sBattleTimeDelay++;

			if (m_sBattleTimeDelay >= (nBattleZone == ZONE_BATTLE4 ? (m_byBattleTime / 48) : (m_byBattleTime / 24)))
			{
				m_sBattleTimeDelay = 0;
				Announcement(DECLARE_BATTLE_ZONE_STATUS);
			}
		}
		else
		{
			if (WarElapsedTime <  m_byBattleTime) // Won the war.
			{
				m_sBattleTimeDelay++;

				if (m_sBattleTimeDelay >= (m_byBattleTime / 24))
				{
					m_sBattleTimeDelay = 0;
					Announcement(UNDER_ATTACK_NOTIFY);
				}

				CheckNationMonumentRewards();
			}
		}

		if (m_bResultDelay)
		{
			m_sBattleResultDelay++;
			if (m_sBattleResultDelay == (m_byBattleTime  / (m_byBattleTime / 10))) {
				m_bResultDelay = false;
				BattleZoneResult(m_bResultDelayVictory);
			}
		}

		if (WarElapsedTime >= m_byBattleTime) // War is over.
			BattleZoneClose();
	}

	if (m_byBanishFlag)
	{
		m_sBanishDelay++;

		if (m_sBanishDelay == (m_byBattleTime / 360))
			Announcement(DECLARE_BAN);
		else if (m_sBanishDelay == (m_byBattleTime / 120)) {
			m_byBanishFlag = false;
			m_sBanishDelay = 0;
			BanishLosers();
		}
	}
}

void CGameServerDlg::BattleZoneResult(uint8 nation)
{
	m_bVictory = nation;
	Announcement(DECLARE_WINNER, m_bVictory);
	Announcement(DECLARE_LOSER, nation == KARUS ? ELMORAD : KARUS);

	if (g_pMain->m_byBattleZoneType == ZONE_ARDREAM)
	{
		BattleZoneClose();
		return;
	}

	m_byKarusOpenFlag = nation == ELMORAD ? true : false;
	m_byElmoradOpenFlag = nation == KARUS ? true : false;
	m_byBanishFlag = true;
	m_sBanishDelay = 0;
}

void CGameServerDlg::BattleWinnerResult(BattleWinnerTypes winnertype)
{
	uint8 winner_nation = 0;
	uint8 nBattleZone = m_byBattleZone + ZONE_BATTLE_BASE;

	if (winnertype == BATTLE_WINNER_NPC)
	{
		if (m_sKilledKarusNpc > m_sKilledElmoNpc)
			winner_nation = KARUS;
		else if (m_sKilledElmoNpc > m_sKilledKarusNpc)
			winner_nation = ELMORAD;

		if (winner_nation == 0 
			&& (nBattleZone == ZONE_BATTLE 
			|| nBattleZone == ZONE_BATTLE2
			|| nBattleZone == ZONE_BATTLE3))
		{
			BattleWinnerResult(BATTLE_WINNER_KILL);
			return;
		}
	}
	else if (winnertype == BATTLE_WINNER_MONUMENT)
	{
		if (m_sKarusMonumentPoint > m_sElmoMonumentPoint)
			winner_nation = KARUS;
		else if (m_sElmoMonumentPoint > m_sKarusMonumentPoint )
			winner_nation = ELMORAD;

		if (winner_nation == 0)
		{
			BattleWinnerResult(BATTLE_WINNER_KILL);
			return;
		}
	}
	else if (winnertype == BATTLE_WINNER_KILL)	
	{
		if (m_sKarusDead > m_sElmoradDead)
			winner_nation = ELMORAD;
		else if (m_sElmoradDead > m_sKarusDead)	
			winner_nation = KARUS;

		if (winner_nation == 0
			&& (nBattleZone == ZONE_BATTLE4 
			|| nBattleZone == ZONE_BATTLE6))
		{
			BattleWinnerResult(BATTLE_WINNER_NPC);
			return;
		}
	}

	if (winner_nation == 0) // Draw
	{
		TRACE("### WARNING : Battle is Draw Zone=%d, Karus Dead=%d, El Morad Dead=%d, Killed Karus NPC=%d, Killed Human NPC=%d, Karus Monument Point=%d, Karus Monument Point=%d ###\n",m_byBattleZone,m_sKarusDead,m_sElmoradDead,m_sKilledKarusNpc,m_sKilledElmoNpc,m_sKarusMonumentPoint,m_sKarusMonumentPoint);
		BattleZoneClose();
	}
	else
		BattleZoneResult(winner_nation);
}

void CGameServerDlg::BattleZoneOpen(int nType, uint8 bZone /*= 0*/)
{
	if ((nType == BATTLEZONE_OPEN || nType == SNOW_BATTLEZONE_OPEN) && !g_pMain->isWarOpen())
	{
		m_byBattleOpen = nType == BATTLEZONE_OPEN ? NATION_BATTLE : SNOW_BATTLE;	
		m_byOldBattleOpen = nType == BATTLEZONE_OPEN ? NATION_BATTLE : SNOW_BATTLE;
		m_byBattleZone = bZone;
		m_byBattleOpenedTime = int32(UNIXTIME);		
		m_byBattleRemainingTime = m_byBattleTime;

		if (bZone + ZONE_BATTLE_BASE == ZONE_BATTLE4)
			SendEventRemainingTime(true, nullptr, ZONE_BATTLE4);

		KickOutZoneUsers(ZONE_ARDREAM);

		if (m_byBattleZoneType == 0)
		{
			KickOutZoneUsers(ZONE_RONARK_LAND_BASE);
			KickOutZoneUsers(ZONE_RONARK_LAND);
			KickOutZoneUsers(ZONE_BIFROST);
			KickOutZoneUsers(ZONE_KROWAZ_DOMINION);
		}
	}
	else if (nType == BATTLEZONE_CLOSE && isWarOpen())
		Announcement(BATTLEZONE_CLOSE);
	else
		return;

	Announcement(nType);	// Send an announcement out that the battlezone is open/closed.
	Packet result(AG_BATTLE_EVENT, uint8(BATTLE_EVENT_OPEN));
	result << uint8(nType);
	Send_AIServer(&result);
}

void CGameServerDlg::BattleZoneClose()
{
	BattleZoneOpen(BATTLEZONE_CLOSE);
	Packet result(AG_BATTLE_EVENT, uint8(BATTLE_EVENT_OPEN));
	result << uint8(BATTLEZONE_CLOSE);
	Send_AIServer(&result);
	ResetBattleZone();
	m_byBanishFlag = true;
}

void CGameServerDlg::BattleZoneVictoryCheck()
{	
	if (m_bKarusFlag >= NUM_FLAG_VICTORY)
		m_bVictory = KARUS;
	else if (m_bElmoradFlag >= NUM_FLAG_VICTORY)
		m_bVictory = ELMORAD;
	else 
		return;

	Announcement(DECLARE_WINNER);

	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach (itr, sessMap)
	{
		CUser* pTUser = TO_USER(itr->second);
		if (!pTUser->isInGame()
			|| pTUser->GetZoneID() != pTUser->GetNation() 
			|| pTUser->GetNation() != m_bVictory)
			continue;

		pTUser->GoldGain(AWARD_GOLD);
		pTUser->ExpChange(AWARD_EXP);

		if (pTUser->GetFame() == COMMAND_CAPTAIN)
		{
			if (pTUser->isKing())
				pTUser->SendLoyaltyChange(500);
			else
				pTUser->SendLoyaltyChange(300);
		}

		// Make the winning nation use a victory emotion (yay!)
		pTUser->StateChangeServerDirect(4, 12);
	}	
}

/**
* @brief	Kicks invaders out of the invaded nation after a war
*			and resets captains.
**/
void CGameServerDlg::BanishLosers()
{
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach (itr, sessMap)
	{
		CUser *pUser = TO_USER(itr->second); 
		if (!pUser->isInGame())
			continue;

		// Reset captains
		if (!isWarOpen())
		{
			if (pUser->GetFame() == COMMAND_CAPTAIN)
				pUser->ChangeFame(CHIEF);
		}

		if (m_byBattleOpen == NATION_BATTLE)
		{
			// Kick out losers
			if (pUser->GetMap()->isWarZone() && m_bVictory != pUser->GetNation())
				pUser->KickOutZoneUser(true);
		}
		else if (!g_pMain->isWarOpen())
		{
			// Kick out invaders
			if ((pUser->GetZoneID() <= ELMORAD && pUser->GetZoneID() != pUser->GetNation())
				|| pUser->GetMap()->isWarZone())
				pUser->KickOutZoneUser(true);
		}
	}
}

void CGameServerDlg::ResetBattleZone()
{
	m_bVictory = 0;

	m_sBanishDelay = 0;
	m_byBanishFlag = false;

	m_sBattleResultDelay = 0;
	m_bResultDelay = false;

	m_bKarusFlag = 0;
	m_bElmoradFlag = 0;

	m_byKarusOpenFlag = false;
	m_byElmoradOpenFlag = false;

	m_byBattleSave = false;

	m_byBattleZone = 0;
	m_byBattleZoneType = 0;

	m_byBattleOpen = NO_BATTLE;
	m_byOldBattleOpen = NO_BATTLE;

	m_byBattleOpenedTime = 0;
	m_byBattleRemainingTime = 0;
	m_sBattleTimeDelay = 0;

	m_sKarusDead = 0;
	m_sElmoradDead = 0;

	m_sKarusCount = 0;
	m_sElmoradCount = 0;

	m_sKilledKarusNpc = 0;
	m_sKilledElmoNpc = 0;

	m_sKarusMonumentPoint = 0;
	m_sElmoMonumentPoint = 0;
	m_sKarusMonuments = 0;
	m_sElmoMonuments = 0;

	m_NationMonumentInformationArray.DeleteAllData();
	m_bMiddleStatueNation = 0;
}

void CGameServerDlg::ForgettenTempleEventTimer()
{
	uint32 nHour = g_localTime.tm_hour;
	uint32 nMinute = g_localTime.tm_min;
	uint32 nSecond = g_localTime.tm_sec;

	if (!m_bForgettenTempleIsActive)
	{
		foreach_stlmap_nolock (itr, m_MonsterChallengeArray)
		{	
			if ((nHour == itr->second->bStartTime1 - 1
				|| nHour == itr->second->bStartTime2 - 1
				|| nHour == itr->second->bStartTime3 - 1) && nMinute == 55 && nSecond == 0)
			{
				if (nHour + 1 == itr->second->bStartTime1)
					m_nForgettenTempleStartHour = itr->second->bStartTime1;
				else if (nHour + 1 == itr->second->bStartTime2)
					m_nForgettenTempleStartHour = itr->second->bStartTime2;
				else if (nHour + 1 == itr->second->bStartTime3)
					m_nForgettenTempleStartHour = itr->second->bStartTime3;

				m_nForgettenTempleLevelMin = itr->second->bLevelMin;
				m_nForgettenTempleLevelMax = itr->second->bLevelMax;

				Announcement(IDS_MONSTER_CHALLENGE_ANNOUNCEMENT);
			}
			else if ((nHour == itr->second->bStartTime1 - 1
				|| nHour == itr->second->bStartTime2 - 1
				|| nHour == itr->second->bStartTime3 - 1) && nMinute == 59 && nSecond == 40)
			{
				m_nForgettenTempleLevelMin = itr->second->bLevelMin;
				m_nForgettenTempleLevelMax = itr->second->bLevelMax;
				Announcement(IDS_MONSTER_CHALLENGE_ANNOUNCEMENT);
			}
			else if ((nHour == itr->second->bStartTime1
				|| nHour == itr->second->bStartTime2
				|| nHour == itr->second->bStartTime3) && nMinute == 0)
			{
				if (itr->second->bLevelMin == 30)	
					m_nForgettenTempleChallengeTime = 1;
				else if (itr->second->bLevelMin == 46)
					m_nForgettenTempleChallengeTime = 2;
				else if (itr->second->bLevelMin == 60)
					m_nForgettenTempleChallengeTime = 3;
			}
		}

		if (m_nForgettenTempleChallengeTime > 0)
		{
			m_bForgettenTempleIsActive = true;
			m_nForgettenTempleStartTime = int32(UNIXTIME);

			foreach_stlmap_nolock (itr, m_MonsterChallengeSummonListArray)
				if (itr->second->bLevel == m_nForgettenTempleChallengeTime)
					m_nForgettenTempleLastStage++;

			Announcement(IDS_MONSTER_CHALLENGE_OPEN);
		}
	}
	else if (m_bForgettenTempleIsActive)
	{
		if (int32(UNIXTIME) - m_nForgettenTempleStartTime == 300 /* 5 minutes */
			&& !m_bForgettenTempleSummonMonsters)
		{
			m_bForgettenTempleSummonMonsters = true;
			m_nForgettenTempleCurrentStage = 1;
			m_nForgettenTempleLastSummonTime = int32(UNIXTIME) + 30;
			Announcement(IDS_MONSTER_CHALLENGE_START);
		}
		else if (m_bForgettenTempleSummonMonsters)
		{
			foreach_stlmap_nolock (itr, m_MonsterChallengeSummonListArray)
			{
				if (m_nForgettenTempleChallengeTime != itr->second->bLevel
					|| int32(UNIXTIME) - m_nForgettenTempleLastSummonTime < 45
					|| itr->second->bStage != m_nForgettenTempleCurrentStage)
					continue;

				if (m_nForgettenTempleCurrentStage > m_nForgettenTempleLastStage)
					m_bForgettenTempleSummonMonsters = false;
				else
				{
					SpawnEventNpc(itr->second->sSid,true,ZONE_FORGOTTEN_TEMPLE,(float)itr->second->sPosX,0,(float)itr->second->sPosZ,itr->second->sCount,itr->second->bRange);
					m_nForgettenTempleCurrentStage++;
					m_nForgettenTempleLastSummonTime = int32(UNIXTIME);
				}
			}
		}
		else
		{
			if (m_nForgettenTempleCurrentStage > m_nForgettenTempleLastStage && !m_nForgettenTempleBanishFlag) 
			{
				if (m_ForgettenTempleMonsterList.size() == 0)
				{
					m_nForgettenTempleBanishFlag = true;
					m_nForgettenTempleBanishTime = 0;
					Announcement(IDS_MONSTER_CHALLENGE_VICTORY);
					SendItemZoneUsers(ZONE_FORGOTTEN_TEMPLE, BLUE_TREASURE_CHEST);
				}
				else if (int32(UNIXTIME) - m_nForgettenTempleStartTime >= (HOUR - (10 * MINUTE)))
				{
					m_nForgettenTempleBanishFlag = true;
					m_nForgettenTempleBanishTime = 0;
				}
			}
		}

		if (m_nForgettenTempleBanishFlag)
		{
			m_nForgettenTempleBanishTime++;

			if (m_nForgettenTempleBanishTime == 30 /* 30 seconds */)
				Announcement(IDS_MONSTER_CHALLENGE_CLOSE);
			else if (m_nForgettenTempleBanishTime == 300 /* 5 minutes */)
			{
				m_nForgettenTempleBanishFlag = false;
				m_nForgettenTempleBanishTime = 0;

				m_nForgettenTempleUsers.clear();
				m_bForgettenTempleIsActive = false;
				m_nForgettenTempleStartHour = 0;
				m_nForgettenTempleLevelMin = 0;
				m_nForgettenTempleLevelMax = 0;
				m_nForgettenTempleStartTime = 0;
				m_nForgettenTempleChallengeTime = 0;
				m_bForgettenTempleSummonMonsters = false;
				m_nForgettenTempleCurrentStage = 0;
				m_nForgettenTempleLastStage = 0;

				foreach (itr, m_ForgettenTempleMonsterList)
					KillNpc(itr->first);

				m_ForgettenTempleMonsterList.clear();
				m_nForgettenTempleLastSummonTime = 0;

				KickOutZoneUsers(ZONE_FORGOTTEN_TEMPLE);
			}
		}
	}
}

void CGameServerDlg::TempleEventTimer()
{
	uint32 nHour = g_localTime.tm_hour;
	uint32 nMinute = g_localTime.tm_min;

	if (m_nTempleEventRemainSeconds > 0)
		m_nTempleEventRemainSeconds--;

	if (pTempleEvent.ActiveEvent == -1)
	{
		for (int i = 0; i < CHAOS_EVENT_COUNT; i++)
		{
			if (nHour == m_nChaosTime[i] && nMinute == 0)
			{
				pTempleEvent.ActiveEvent = TEMPLE_EVENT_CHAOS;
				pTempleEvent.ZoneID = ZONE_CHAOS_DUNGEON;
				m_nTempleEventRemainSeconds = 600; // 10 minutes
				TempleEventStart();
				break;
			} 
		}


		for (int i = 0; i < BORDER_DEFENSE_WAR_EVENT_COUNT; i++)
		{
			if (nHour == m_nBorderDefenseWarTime[i] && nMinute == 32) 
			{
				pTempleEvent.ActiveEvent = TEMPLE_EVENT_BORDER_DEFENCE_WAR;
				pTempleEvent.ZoneID = ZONE_BORDER_DEFENSE_WAR;
				m_nTempleEventRemainSeconds = 600; // 10 minutes
				TempleEventStart();
				break;
			}  
		}

	}
	else if (pTempleEvent.ActiveEvent != -1)
	{
		if (pTempleEvent.ActiveEvent == TEMPLE_EVENT_CHAOS)
		{
			for(int i = 0; i < CHAOS_EVENT_COUNT; i++)
			{
				if (nHour == m_nChaosTime[i] 
				&& nMinute == 10 
					&& !pTempleEvent.isActive) {
						m_nTempleEventRemainSeconds = 0;
						pTempleEvent.LastEventRoom = 1;
						pTempleEvent.isActive = true;
						TempleEventStart(); // Set RemainSeconds to zero
						TempleEventTeleportUsers();
						TempleEventSummonChaosCubes();
						break;
				}
				else if (nHour == m_nChaosTime[i] 
				&& nMinute == 11 
					&& !pTempleEvent.isAttackable) {
						pTempleEvent.isAttackable = true;
						break;
				}
				else if (nHour == m_nChaosTime[i] 
				&& nMinute == 30
					&& pTempleEvent.isActive) {
						TempleEventFinish();
						break;
				}
			}
		}

		else if (pTempleEvent.ActiveEvent == TEMPLE_EVENT_BORDER_DEFENCE_WAR)
		{
			for(int i = 0; i < BORDER_DEFENSE_WAR_EVENT_COUNT; i++)
			{
				if (nHour == m_nBorderDefenseWarTime[i] 
				&& nMinute == 42
					&& !pTempleEvent.isActive)
				{
					m_nTempleEventRemainSeconds = 0; 
					pTempleEvent.LastEventRoom = 1;
					pTempleEvent.isActive = true;
					TempleEventStart(); // Set RemainSeconds to zero
					TempleEventTeleportUsers();
					break;
				} 
				else if (nHour == (m_nBorderDefenseWarTime[i] + 1) 
					&& nMinute == 58
					&& pTempleEvent.isActive)
				{
					TempleEventFinish();
					break;
				}
			}
		}

	}
}

void CGameServerDlg::TempleEventStart()
{
	Packet result(WIZ_EVENT, uint8(TEMPLE_EVENT));
	pTempleEvent.StartTime = (uint32)UNIXTIME;
	pTempleEvent.KarusUserCount = 0;
	pTempleEvent.ElMoradUserCount = 0;
	pTempleEvent.AllUserCount = 0;
	result << (uint16)pTempleEvent.ActiveEvent << m_nTempleEventRemainSeconds;
	Send_All(&result);
}

void CGameServerDlg::TempleEventTeleportUsers()
{
	switch (pTempleEvent.ActiveEvent)
	{
	case TEMPLE_EVENT_BORDER_DEFENCE_WAR:
		pTempleEvent.isAttackable = true;
		break;
	case TEMPLE_EVENT_CHAOS:
		pTempleEvent.isAttackable = false;
		break;
	case TEMPLE_EVENT_JURAD_MOUNTAIN:
		pTempleEvent.isAttackable = true;
		break;
	}

	foreach_stlmap (itr, m_TempleEventUserArray)
	{
		CUser * pUser = GetUserPtr(itr->second->m_socketID);

		if (pUser == nullptr 
			||	!pUser->isInGame())
			continue;

		if (pTempleEvent.ZoneID == ZONE_CHAOS_DUNGEON)
		{
			if (pUser->CheckExistItem(CHAOS_MAP, 1))
				pUser->RobItem(CHAOS_MAP, 1);

			pUser->ZoneChange(pTempleEvent.ZoneID,0.0f,0.0f); // Start position is random...
		}
		else
			pUser->ZoneChange(pTempleEvent.ZoneID,0.0f,0.0f);
	}
}

uint8 CGameServerDlg::TempleEventGetRoomUsers(uint16 nEventRoom) 
{
	uint8 nEventRoomUserCount = 0;

	foreach_stlmap (itr, m_TempleEventUserArray)
	{
		if (itr->second->m_bEventRoom == nEventRoom)
			nEventRoomUserCount++;
	}

	return nEventRoomUserCount;
}

void CGameServerDlg::TempleEventSummonChaosCubes()
{
	for (int i = 0; i < g_pMain->pTempleEvent.LastEventRoom; i++)
	{
		foreach_stlmap_nolock (itr, m_MonsterRespawnListInformationArray)
		{
			if (itr->second->ZoneID == ZONE_CHAOS_DUNGEON)
				SpawnEventNpc(itr->second->sSid,false,itr->second->ZoneID,itr->second->X,itr->second->Y,itr->second->Z,itr->second->sCount,itr->second->bRadius,0,0,-1,i+1/* EventRoom */);
		}
	}
}

void CGameServerDlg::TempleEventFinish()
{
	foreach_stlmap (itr, m_TempleEventUserArray)
	{
		CUser * pUser = GetUserPtr(itr->second->m_socketID);

		if (pUser == nullptr 
			||	!pUser->isInGame())
			continue;

		_USER_RANKING * pRankInfo = m_UserRankingArray[pUser->GetNation() - 1].GetData(itr->second->m_socketID);

		if (pRankInfo)
		{
			int64 nChangeExp = -1;

			if (pRankInfo->m_bZone == ZONE_BORDER_DEFENSE_WAR)
			{
				if (pUser->GetLevel() < 58)
					nChangeExp = int64((pUser->GetLevel()-20) * (3000 + 100/* Temp Score */* 1000));
				else
					nChangeExp = int64((pUser->GetLevel()+55) * (20000 + 100/* Temp Score */ * 500));				
			}
			else if (pRankInfo->m_bZone == ZONE_CHAOS_DUNGEON)
			{
				int64 nGainedExp = int64(pow(pUser->GetLevel(),3) * 0.15 * (5 * pRankInfo->m_KillCount - pRankInfo->m_DeathCount));
				int64 nPremiumGainedExp = nGainedExp * 2;

				if (nGainedExp > 8000000)
					nGainedExp = 8000000;

				if (nPremiumGainedExp > 8000000)
					nPremiumGainedExp = 8000000;

				nChangeExp = pUser->m_bPremiumType != 0 ? nPremiumGainedExp : nGainedExp;

				uint8 nEventRoomUserCount = g_pMain->TempleEventGetRoomUsers(pUser->GetEventRoom());
				uint8 nItemRewardRankFirst = nEventRoomUserCount / 3;
				uint8 nItemRewardRankSecond = (nEventRoomUserCount  - 1) * 2;

				int32 nUserRank = pUser->GetPlayerRank(RANK_TYPE_CHAOS_DUNGEON);

				if (nUserRank > 0 && nUserRank < nItemRewardRankFirst)
					pUser->GiveItem(BLUE_TREASURE_CHEST);
				else if (nUserRank > 0 && nUserRank >= nItemRewardRankFirst && nUserRank <= nItemRewardRankSecond)
					pUser->GiveItem(GREEN_TREASURE_CHEST);
				else if (nUserRank > 0 && nUserRank > nItemRewardRankSecond)
					pUser->GiveItem(RED_TREASURE_CHEST);
			}

			if (nChangeExp > 0)
				pUser->ExpChange(nChangeExp);
		}
	}

	foreach_stlmap_nolock (itr, m_TempleEventUserArray)
	{
		CUser * pUser = GetUserPtr(itr->second->m_socketID);

		if (pUser && pUser->isInGame())
			TempleEventKickOutUser(pUser);
	}

	TempleEventReset();
}

void CGameServerDlg::TempleEventGetActiveEventTime(CUser *pUser)
{
	if (pUser == nullptr)
		return;

	Packet result(WIZ_EVENT, uint8(TEMPLE_EVENT));
	result << (uint16)pTempleEvent.ActiveEvent << m_nTempleEventRemainSeconds;
	pUser->Send(&result);
}

void CGameServerDlg::TempleEventSendActiveEventTime(CUser *pUser)
{
	if (pUser == nullptr)
		return;

	if (!pUser->isEventUser())
	{
		TempleEventGetActiveEventTime(pUser);
		return;
	}

	Packet result(WIZ_EVENT, uint8(TEMPLE_EVENT_JOIN));
	result << uint8(1) << uint16(pTempleEvent.ActiveEvent);
	pUser->Send(&result);
	pUser->TempleOperations(TEMPLE_EVENT_COUNTER);
}

void CGameServerDlg::TempleEventKickOutUser(CUser *pUser)
{
	if (pUser == nullptr
		|| !pUser->isInGame())
		return;

	uint8 nZoneID = 0;

	if (pUser->GetZoneID() == ZONE_BORDER_DEFENSE_WAR)
		nZoneID = pUser->GetNation();
	else if (pUser->GetZoneID() == ZONE_CHAOS_DUNGEON)
	{
		if (pUser->GetLevel() <  35)
			nZoneID = ZONE_MORADON;
		else if (pUser->GetLevel() >= 35 && pUser->GetLevel() <=59)
			nZoneID = ZONE_ARDREAM;
		else if (pUser->GetLevel() >= 60 && pUser->GetLevel() <=69)
			nZoneID = ZONE_RONARK_LAND_BASE;
		else if (pUser->GetLevel() >=  70)
			nZoneID = ZONE_RONARK_LAND;
	}
	else if (pUser->GetZoneID() == ZONE_JURAD_MOUNTAIN)
		nZoneID = pUser->GetNation() + 10;

	if (nZoneID == 0)
		return;

	_START_POSITION * pStartPosition = GetStartPosition(nZoneID);

	if (pStartPosition == nullptr)
	{
		TRACE("### TempleEventKickOutUser - StartPosition not found : Zone ID=%d",nZoneID);
		return;
	}

	pUser->ZoneChange(nZoneID, 0.0f,0.0f);
}

void CGameServerDlg::TempleEventReset()
{
	pTempleEvent.ActiveEvent = -1;
	pTempleEvent.ZoneID = 0;
	pTempleEvent.LastEventRoom = 1;
	pTempleEvent.StartTime = 0;
	pTempleEvent.AllUserCount = 0;
	pTempleEvent.KarusUserCount = 0;
	pTempleEvent.ElMoradUserCount = 0;
	pTempleEvent.isAttackable = false;
	pTempleEvent.isActive = false;
	m_TempleEventUserArray.DeleteAllData();
}

void CGameServerDlg::Announcement(uint16 type, int nation, int chat_type, CUser* pExceptUser, CNpc *pExpectNpc)
{
	string chatstr; 
	uint8 ZoneID = 0;

	switch (type)
	{
	case BATTLEZONE_OPEN:
	case SNOW_BATTLEZONE_OPEN:
		GetServerResource(IDP_BATTLEZONE_OPEN, &chatstr);
		break;

	case DECLARE_WINNER:
		if (m_bVictory == KARUS)
			GetServerResource(IDP_KARUS_VICTORY, &chatstr, m_sElmoradDead, m_sKarusDead);
		else if (m_bVictory == ELMORAD)
			GetServerResource(IDP_ELMORAD_VICTORY, &chatstr, m_sKarusDead, m_sElmoradDead);
		else 
			return;
		break;
	case DECLARE_LOSER:
		if (m_bVictory == KARUS)
			GetServerResource(IDS_ELMORAD_LOSER, &chatstr, m_sKarusDead, m_sElmoradDead);
		else if (m_bVictory == ELMORAD)
			GetServerResource(IDS_KARUS_LOSER, &chatstr, m_sElmoradDead, m_sKarusDead);
		else 
			return;
		break;
	case DECLARE_BAN:
		if (m_bVictory == KARUS || m_bVictory == ELMORAD)
			GetServerResource(IDS_BANISH_LOSER, &chatstr);
		else
			GetServerResource(IDS_BANISH_USER, &chatstr);
		break;
	case DECLARE_BATTLE_ZONE_STATUS:
		if (m_byBattleZone + ZONE_BATTLE_BASE == ZONE_BATTLE4)
		{
			GetServerResource(IDS_BATTLE_MONUMENT_STATUS, &chatstr,  m_sKarusMonumentPoint, m_sElmoMonumentPoint, m_sKarusDead, m_sElmoradDead);
			SendNotice<PUBLIC_CHAT>(chatstr.c_str(), ZONE_BATTLE4, Nation::ALL, true);
		}
		else
			GetServerResource(IDS_BATTLEZONE_STATUS, &chatstr,  m_sKarusDead, m_sElmoradDead);
		break;
	case DECLARE_BATTLE_MONUMENT_STATUS:
		if (pExceptUser)
		{
			GetServerResource(IDS_BATTLE_MONUMENT_WON_MESSAGE, &chatstr, GetBattleAndNationMonumentName(chat_type).c_str());
			g_pMain->SendNotice<PUBLIC_CHAT>(chatstr.c_str(),pExceptUser->GetZoneID(),pExceptUser->GetNation());
			GetServerResource(IDS_BATTLE_MONUMENT_LOST_MESSAGE, &chatstr, GetBattleAndNationMonumentName(chat_type).c_str());
			g_pMain->SendNotice<PUBLIC_CHAT>(chatstr.c_str(),pExceptUser->GetZoneID(),pExceptUser->GetNation() == KARUS ? ELMORAD : KARUS);
			return;
		}
		break;
	case DECLARE_NATION_MONUMENT_STATUS:
		if (pExceptUser)
		{
			uint16 nTrapNumber = pExceptUser->GetZoneID() == ZONE_KARUS ?  chat_type - LUFERSON_MONUMENT_SID : chat_type - ELMORAD_MONUMENT_SID;

			GetServerResource(IDS_INFILTRATION_CONQUER, &chatstr, GetBattleAndNationMonumentName(nTrapNumber, pExceptUser->GetZoneID()).c_str());
			g_pMain->SendAnnouncement(chatstr.c_str(), pExceptUser->GetNation());
			GetServerResource(IDS_INFILTRATION_RECAPTURE, &chatstr, GetBattleAndNationMonumentName(nTrapNumber, pExceptUser->GetZoneID()).c_str());
			g_pMain->SendAnnouncement(chatstr.c_str(), pExceptUser->GetNation() == KARUS ? ELMORAD : KARUS);
			return;
		}
		break;
	case DECLARE_NATION_REWARD_STATUS:
		if (pExpectNpc)
		{
			uint16 nTrapNumber = pExpectNpc->GetZoneID() == ZONE_KARUS ?  chat_type - LUFERSON_MONUMENT_SID : chat_type - ELMORAD_MONUMENT_SID;

			GetServerResource(pExpectNpc->GetNation() == KARUS ? IDS_INFILTRATION_REWARD_KARUS : IDS_INFILTRATION_REWARD_ELMORAD, &chatstr, GetBattleAndNationMonumentName(nTrapNumber, pExpectNpc->GetZoneID()).c_str());
			g_pMain->SendAnnouncement(chatstr.c_str(), Nation::ALL);
			return;
		}
		break;
	case UNDER_ATTACK_NOTIFY:
		if (m_bVictory == KARUS)
			GetServerResource(IDS_UNDER_ATTACK_ELMORAD, &chatstr, m_sKarusDead, m_sElmoradDead);
		else if (m_bVictory == ELMORAD)
			GetServerResource(IDS_UNDER_ATTACK_KARUS, &chatstr, m_sElmoradDead, m_sKarusDead);
		else 
			return;
		break;
	case BATTLEZONE_CLOSE:
		GetServerResource(IDS_BATTLE_CLOSE, &chatstr);
		break;
	case KARUS_CAPTAIN_NOTIFY:
		GetServerResource(IDS_KARUS_CAPTAIN, &chatstr, m_strKarusCaptain.c_str());
		break;
	case ELMORAD_CAPTAIN_NOTIFY:
		GetServerResource(IDS_ELMO_CAPTAIN, &chatstr, m_strElmoradCaptain.c_str());
		break;
	case KARUS_CAPTAIN_DEPRIVE_NOTIFY:
		if (pExceptUser)
		{
			CKnights *pKnights = g_pMain->GetClanPtr(pExceptUser->GetClanID());
			GetServerResource(IDS_KARUS_CAPTAIN_DEPRIVE, &chatstr, pKnights == nullptr ? "***" : pKnights->GetName().c_str(), pExceptUser->GetName().c_str());
		}
		break;
	case ELMORAD_CAPTAIN_DEPRIVE_NOTIFY:
		if (pExceptUser)
		{
			CKnights *pKnights = g_pMain->GetClanPtr(pExceptUser->GetClanID());
			GetServerResource(IDS_ELMO_CAPTAIN_DEPRIVE, &chatstr, pKnights == nullptr ? "***" : pKnights->GetName().c_str(),  pExceptUser->GetName().c_str());
		}
		break;
	case IDS_MONSTER_CHALLENGE_ANNOUNCEMENT:
		GetServerResource(IDS_MONSTER_CHALLENGE_ANNOUNCEMENT, &chatstr, m_nForgettenTempleLevelMin, m_nForgettenTempleLevelMax, m_nForgettenTempleStartHour);
		break;
	case IDS_MONSTER_CHALLENGE_OPEN:
		GetServerResource(IDS_MONSTER_CHALLENGE_OPEN, &chatstr);
		break;
	case IDS_MONSTER_CHALLENGE_START:
		ZoneID = ZONE_FORGOTTEN_TEMPLE;
		GetServerResource(IDS_MONSTER_CHALLENGE_START, &chatstr);
		break;
	case IDS_MONSTER_CHALLENGE_VICTORY:
		ZoneID = ZONE_FORGOTTEN_TEMPLE;
		GetServerResource(IDS_MONSTER_CHALLENGE_VICTORY, &chatstr);
		break;
	case IDS_MONSTER_CHALLENGE_CLOSE:
		GetServerResource(IDS_MONSTER_CHALLENGE_CLOSE, &chatstr);
		break;
	}

	Packet result;
	string finalstr;
	GetServerResource(IDP_ANNOUNCEMENT, &finalstr, chatstr.c_str());
	ChatPacket::Construct(&result, (uint8) chat_type, &finalstr);
	Send_All(&result, nullptr, nation, ZoneID);
}

/**
* @brief	Loads the specified user's NP ranks
* 			from the rankings tables.
*
* @param	pUser	The user.
*/
void CGameServerDlg::GetUserRank(CUser *pUser)
{
	// Acquire the lock for thread safety
	Guard lock(m_userRankingsLock);

	// Get character's name & convert it to upper case for case insensitivity
	string strUserID = pUser->GetName();
	STRTOUPPER(strUserID);

	// Grab the personal rank from the map, if applicable.
	UserNameRankMap::iterator itr = m_UserPersonalRankMap.find(strUserID);
	pUser->m_bPersonalRank = itr != m_UserPersonalRankMap.end() ? int8(itr->second->nRank) : -1;

	// Grab the knights rank from the map, if applicable.
	itr = m_UserKnightsRankMap.find(strUserID);
	pUser->m_bKnightsRank = itr != m_UserKnightsRankMap.end() ? int8(itr->second->nRank) : -1;
}

uint16 CGameServerDlg::GetKnightsAllMembers(uint16 sClanID, Packet & result, uint16 & pktSize, bool bClanLeader)
{
	CKnights* pKnights = GetClanPtr(sClanID);
	if (pKnights == nullptr)
		return 0;

	uint16 count = 0;
	foreach_array (i, pKnights->m_arKnightsUser)
	{
		_KNIGHTS_USER *p = &pKnights->m_arKnightsUser[i];
		if (!p->byUsed)
			continue;

		CUser *pUser = p->pSession;
		if (pUser != nullptr)
			result << pUser->GetName() << pUser->GetFame() << pUser->GetLevel() << pUser->m_sClass << uint8(1);
		else // normally just clan leaders see this, but we can be generous now.
			result << pKnights->m_arKnightsUser[i].strUserName << uint8(0) << uint8(0) << uint16(0) << uint8(0);

		count++;
	}

	return count;
}

/**
* @brief	Calculates the clan grade from the specified
* 			loyalty points (NP).
*
* @param	nPoints	Loyalty points (NP). 
* 					The points will be converted to clan points 
* 					by this method.
*
* @return	The clan grade.
*/
int CGameServerDlg::GetKnightsGrade(uint32 nPoints)
{
	uint32 nClanPoints = nPoints / MAX_CLAN_USERS;

	if (nClanPoints >= 20000)
		return 1;
	else if (nClanPoints >= 10000)
		return 2;
	else if (nClanPoints >= 5000)
		return 3;
	else if (nClanPoints >= 2000)
		return 4;

	return 5;
}

/**
* @brief	Disconnects all players in the server.
*
* @return	The number of users who were in-game.
*/
int CGameServerDlg::KickOutAllUsers()
{
	int count = 0;

	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach (itr, sessMap)
	{
		CUser *pUser = TO_USER(itr->second);
		if (pUser->isInGame())
			count++;

		g_DBAgent.UpdateUser(pUser->GetName(), UPDATE_LOGOUT, pUser);
		g_DBAgent.UpdateWarehouseData(pUser->GetAccountName(), UPDATE_LOGOUT, pUser);
		g_DBAgent.UpdateSavedMagic(pUser);
		pUser->Disconnect();
	}
	return count;
}

/**
* @brief	Generates a new item serial.
*/
uint64 CGameServerDlg::GenerateItemSerial()
{
	static std::recursive_mutex _mutex;

	MYINT64 serial;
	MYSHORT	increase;
	serial.i = 0;

	time_t t = UNIXTIME;
	struct tm * ptm;
	ptm = gmtime(&t);

	Guard lock(_mutex);
	increase.w = g_increase_serial++;

	serial.b[7] = (uint8)(m_nServerNo);
	serial.b[6] = (uint8)(ptm->tm_year % 100);
	serial.b[5] = (uint8)(ptm->tm_mon);
	serial.b[4] = (uint8)(ptm->tm_mday);
	serial.b[3] = (uint8)(ptm->tm_hour);
	serial.b[2] = (uint8)(ptm->tm_min);
	serial.b[1] = increase.b[1];
	serial.b[0] = increase.b[0];

	return serial.i;
}

/**
* @brief	Kick out all users from the specified zone
* 			to their home zone.
*
* @param	zone	The zone to kick users out from.
*/
void CGameServerDlg::KickOutZoneUsers(uint8 ZoneID, uint8 TargetZoneID)
{
	// TODO: Make this localised to zones.
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	C3DMap	*pKarusMap		= GetZoneByID(KARUS),
		*pElMoradMap	= GetZoneByID(ELMORAD);	

	ASSERT (pKarusMap != nullptr && pElMoradMap != nullptr);

	foreach (itr, sessMap)
	{
		// Only kick users from requested zone.
		CUser * pUser = TO_USER(itr->second);
		if (!pUser->isInGame()
			|| pUser->GetZoneID() != ZoneID) 
			continue;

		if (TargetZoneID > 0)
		{
			pUser->ZoneChange(TargetZoneID, 0.0f,0.0f);
			continue;
		}

		C3DMap * pMap = (pUser->GetNation() == KARUS ? pKarusMap : pElMoradMap);
		pUser->ZoneChange(pMap->m_nZoneNumber, pMap->m_fInitX, pMap->m_fInitZ);
	}
}

void CGameServerDlg::SendItemZoneUsers(uint8 ZoneID, uint32 nItemID, uint16 sCount /*= 1*/)
{
	// TODO: Make this localised to zones.
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach (itr, sessMap)
	{
		CUser * pUser = TO_USER(itr->second);
		if (!pUser->isInGame()
			|| pUser->GetZoneID() != ZoneID) 
			continue;

		pUser->GiveItem(nItemID, sCount);
	}
}

void CGameServerDlg::SendItemEventRoom(uint16 nEventRoom,uint32 nItemID, uint16 sCount /*= 1*/)
{
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach (itr, sessMap)
	{
		CUser * pUser = TO_USER(itr->second);
		if (!pUser->isInGame()
			|| pUser->GetEventRoom() != nEventRoom) 
			continue;

		pUser->GiveItem(nItemID, sCount);
	}
}

void CGameServerDlg::Send_CommandChat(Packet *pkt, int nation, CUser* pExceptUser)
{
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach (itr, sessMap)
	{
		CUser * pUser = TO_USER(itr->second);
		if (pUser->isInGame() && (nation == 0 || nation == pUser->GetNation()))
			pUser->Send(pkt);
	}
}

void CGameServerDlg::GetCaptainUserPtr()
{
	foreach_stlmap (itr, m_KnightsArray)
	{
		CKnights *pKnights = itr->second;
		if (pKnights->m_byRanking != 1)
			continue;

		// do something cool here
	}
}

/**
* @brief	Updates the number of users currently in the war zone
* 			and sends the user counts to all servers in this group.
*/
void CGameServerDlg::BattleZoneCurrentUsers()
{
	C3DMap* pMap = GetZoneByID(ZONE_BATTLE_BASE + m_byBattleZone);
	if (pMap == nullptr || m_nServerNo != pMap->m_nServerNo)
		return;

	uint16 nKarusMan = 0, nElmoradMan = 0;
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach (itr, sessMap)
	{
		CUser * pUser = TO_USER(itr->second);
		if (!pUser->isInGame() || pUser->GetZoneID() != pMap->GetID())
			continue;

		if (pUser->GetNation() == KARUS)
			nKarusMan++;
		else
			nElmoradMan++;
	}

	m_sKarusCount = nKarusMan;
	m_sElmoradCount = nElmoradMan;
}

/**
* @brief	Sends the flying santa/angel packet to all users in the server.
*/
void CGameServerDlg::SendFlyingSantaOrAngel()
{
	Packet result(WIZ_SANTA, uint8(m_bSantaOrAngel));
	Send_All(&result);
}

CGameServerDlg::~CGameServerDlg() 
{
	printf("Waiting for timer threads to exit...");
	foreach (itr, g_timerThreads)
	{
		(*itr)->waitForExit();
		delete (*itr);
	}
	printf(" exited.\n");

	// Cleanup our script pool & consequently ensure all scripts 
	// finish execution before proceeding.
	// This prevents us from freeing data that's in use.
	printf("Shutting down Lua engine...");
	m_luaEngine.Shutdown();
	printf(" done.\n");

	printf("Shutting down database system...");
	DatabaseThread::Shutdown();
	printf(" done.\n");

	printf("Shutting down socket system...");
	m_aiSocketMgr.Shutdown();
	m_socketMgr.Shutdown();
	printf(" done.\n");

	CUser::CleanupChatCommands();
	CGameServerDlg::CleanupServerCommands();

	CleanupUserRankings();
	m_LevelUpArray.clear();

	if (m_fpDeathUser != nullptr)
		fclose(m_fpDeathUser);

	if (m_fpDeathNpc != nullptr)
		fclose(m_fpDeathNpc);

	if (m_fpChat != nullptr)
		fclose(m_fpChat);

	if (m_fpCheat != nullptr)
		fclose(m_fpCheat);
}

std::string CGameServerDlg::GetBattleAndNationMonumentName(int16 TrapNumber, uint8 ZoneID)
{
	std::string sMonumentName = ZoneID == 0 ? "DECLARE_BATTLE_MONUMENT_STATUS" : "DECLARE_NATION_MONUMENT_STATUS";

	if (ZoneID == 0)
	{
		if (TrapNumber == 1)
			sMonumentName = "El Morad main territory";
		else if (TrapNumber == 2)
			sMonumentName = "El Morad provision line";
		else if (TrapNumber == 3)
			sMonumentName = "Lake of Life";
		else if (TrapNumber == 4)
			sMonumentName = "Foss Castle";
		else if (TrapNumber == 5)
			sMonumentName = "Karus main territory";
		else if (TrapNumber == 6)
			sMonumentName = "Karus provision line";
		else if (TrapNumber == 7)
			sMonumentName = "Swamp of Shadows";
	}
	else
	{
		if (ZoneID == ZONE_KARUS && TrapNumber == 1)
			TrapNumber = 2;
		else if (ZoneID == ZONE_KARUS && TrapNumber == 2)
			TrapNumber = 1;

		if (TrapNumber == 0)
			sMonumentName = string_format("%s Monument", ZoneID == ZONE_KARUS ? "Luferson" : "El Morad");
		else if (TrapNumber == 1)
			sMonumentName = string_format("%s Monument", ZoneID == ZONE_KARUS ? "Bellua" : "Asga Village");
		else if (TrapNumber == 2)
			sMonumentName = string_format("%s Monument", ZoneID == ZONE_KARUS ? "Linate" : "Raiba Village");
		else if (TrapNumber == 3)
			sMonumentName = string_format("%s Monument", ZoneID == ZONE_KARUS ? "Laon Camp" : "Dodo Camp");
	}

	return sMonumentName;
}

void CGameServerDlg::CheckNationMonumentRewards()
{
	std::vector<uint16> deleted;

	foreach_stlmap (itr, m_NationMonumentInformationArray)
	{
		if (int32(UNIXTIME) - itr->second->RepawnedTime < NATION_MONUMENT_REWARD_SECOND)
			continue;

		CNpc *pNpc = GetNpcPtr(itr->second->sNid);

		if (pNpc == nullptr)
			continue;

		uint16 nTrapNumber = pNpc->GetZoneID() == ZONE_KARUS ?  itr->second->sSid - LUFERSON_MONUMENT_SID : itr->second->sSid - ELMORAD_MONUMENT_SID;

		std::vector<Unit *> distributed_member;
		std::vector<uint16> unitList;
		g_pMain->GetUnitListFromSurroundingRegions(pNpc, &unitList);

		foreach (itrx, unitList)
		{		
			Unit * pTarget = g_pMain->GetUnitPtr(*itrx);

			if(pTarget == nullptr || pTarget->isNPC())
				continue; 

			if (pTarget->GetNation() == pNpc->GetNation() && pTarget->isInRangeSlow(pNpc,RANGE_50M))
				distributed_member.push_back(pTarget);
		}

		foreach (itry, distributed_member)
		{
			Unit * pTarget = *itry;

			if(pTarget == nullptr || pTarget->isNPC())
				continue;

			TO_USER(pTarget)->SendLoyaltyChange(nTrapNumber == 0 ? 200 : 50);
		}

		Announcement(DECLARE_NATION_REWARD_STATUS, Nation::ALL, itr->second->sSid, nullptr, pNpc);
		ShowNpcEffect(itr->second->sNid,20100,pNpc->GetZoneID());
		deleted.push_back(itr->second->sSid);
	}

	foreach (itr, deleted)
		g_pMain->m_NationMonumentInformationArray.DeleteData(*itr);
}

void CGameServerDlg::ShowNpcEffect(uint16 sNpcID, uint32 nEffectID, uint8 ZoneID)
{
	Packet result(WIZ_OBJECT_EVENT, uint8(OBJECT_NPC));
	result << uint8(3) << sNpcID << nEffectID;
	g_pMain->Send_Zone(&result, ZoneID);
}

void CGameServerDlg::WriteDeathUserLogFile(string & logMessage)
{
	fwrite(logMessage.c_str(), logMessage.length(), 1, m_fpDeathUser);
	fflush(m_fpDeathUser);
}

void CGameServerDlg::WriteDeathNpcLogFile(string & logMessage)
{
	fwrite(logMessage.c_str(), logMessage.length(), 1, m_fpDeathNpc);
	fflush(m_fpDeathNpc);
}

void CGameServerDlg::WriteChatLogFile(string & logMessage)
{
	fwrite(logMessage.c_str(), logMessage.length(), 1, m_fpChat);
	fflush(m_fpChat);
}

void CGameServerDlg::WriteCheatLogFile(string & logMessage)
{
	fwrite(logMessage.c_str(), logMessage.length(), 1, m_fpCheat);
	fflush(m_fpCheat);
}

bool CGameServerDlg::IsDuplicateItem(uint32 nItemID, uint64 nItemSerial)
{
	return false;

	if (nItemID <= 0 || nItemSerial <= 0)
		return false;

	_USER_ITEM * pUserItem = g_pMain->m_UserItemArray.GetData(nItemID);

	if (pUserItem == nullptr)
		return false;

	uint32 nDuplicateCount = 0;

	foreach(itr, pUserItem->nItemSerial)
	{
		if (*itr == nItemSerial)
			nDuplicateCount++;

		if (nDuplicateCount > 1)
			return true;
	}

	return false;
}

void CGameServerDlg::AddUserItem(uint32 nItemID, uint64 nItemSerial)
{
	if (nItemID <= 0 || nItemSerial <= 0)
		return;

	_USER_ITEM * pUserItem = g_pMain->m_UserItemArray.GetData(nItemID);

	if (pUserItem == nullptr)
	{
		pUserItem = new _USER_ITEM;
		pUserItem->nItemID = nItemID;
		pUserItem->nItemSerial.push_back(nItemSerial);

		if (!g_pMain->m_UserItemArray.PutData(pUserItem->nItemID,pUserItem))
			delete pUserItem;
	}
	else
		pUserItem->nItemSerial.push_back(nItemSerial);
}

void CGameServerDlg::ReloadKnightAndUserRanks()
{
	g_DBAgent.UpdateRanks();

	// Update clan grades/rankings
	m_KnightsArray.DeleteAllData();
	m_KnightsAllianceArray.DeleteAllData();
	m_KnightsRatingArray[KARUS_ARRAY].DeleteAllData();
	m_KnightsRatingArray[ELMORAD_ARRAY].DeleteAllData();
	LoadAllKnights(true);
	LoadAllKnightsUserData(true);
	LoadKnightsAllianceTable(true);
	LoadKnightsRankTable(false, true);

	// Update user rankings
	CleanupUserRankings();
	LoadUserRankings();
}

void CGameServerDlg::SetPlayerRankingRewards(uint16 ZoneID)
{
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach (itr, sessMap)
	{
		CUser * pUser = TO_USER(itr->second);
		if (pUser == nullptr 
			|| !pUser->isInGame()
			|| pUser->isGM()
			|| pUser->GetZoneID() != ZoneID)
			continue;

		if (pUser->GetPlayerRank(RANK_TYPE_PK_ZONE) > 0
			&& pUser->GetPlayerRank(RANK_TYPE_PK_ZONE) <= 10)
		{
			if (m_nPlayerRankingLoyaltyReward > 0)
				pUser->SendLoyaltyChange(m_nPlayerRankingLoyaltyReward, false, true, false);
			if (m_nPlayerRankingKnightCashReward > 0)
				g_DBAgent.UpdateAccountKnightCash(pUser->GetAccountName(), m_nPlayerRankingKnightCashReward);
		}
	}
}