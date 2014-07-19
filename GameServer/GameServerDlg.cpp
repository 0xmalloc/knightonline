#include "stdafx.h"
#include "KingSystem.h"

#include "../shared/ClientSocketMgr.h"
#include "../shared/Ini.h"
#include "../shared/DateTime.h"

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

CGameServerDlg::CGameServerDlg()
{
	DateTime now;

	m_sYear = now.GetYear();
	m_sMonth = now.GetMonth();
	m_sDate = now.GetDay();
	m_sHour = now.GetHour();
	m_sMin = now.GetMinute();

	m_byWeather = 0;
	m_sWeatherAmount = 0;
	m_byKingWeatherEvent = 0;
	m_byKingWeatherEvent_Day = 0;
	m_byKingWeatherEvent_Hour = 0;
	m_byKingWeatherEvent_Minute = 0;

	m_byExpEventAmount = m_byCoinEventAmount = 0;

	m_sPartyIndex = 0;

	m_nCastleCapture = 0;

	m_bKarusFlag = 0;
	m_bElmoradFlag = 0;

	m_byKarusOpenFlag = false;
	m_byElmoradOpenFlag = false;
	m_byBanishFlag = false;
	m_sBanishDelay = 0;

	m_sKarusDead = 0;
	m_sElmoradDead = 0;

	m_bVictory = 0;	
	m_byOldVictory = 0;
	m_byBattleSave = false;
	m_sKarusCount = 0;
	m_sElmoradCount = 0;

	m_byBattleZone = 0;
	m_byBattleOpen = NO_BATTLE;
	m_byOldBattleOpen = NO_BATTLE;
	m_byBattleOpenedTime = 0;
	m_bFirstServerFlag = false;
	// m_bPointCheckFlag = false;
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
	m_sZoneCount = 0;
	m_sErrorSocketCount = 0;

	m_bFirstServerFlag = false;	
	m_bServerCheckFlag = false;

	GetTimeFromIni();

	if (!g_pMain->m_socketMgr.Listen(_LISTEN_PORT, MAX_USER))
	{
		printf(_T("ERROR: Failed to listen on server port (%d)."), _LISTEN_PORT);
		return false;
	}

#ifdef CONFIG_USE_IOCP
	// Bit tacky, but there's no reason we can't reuse the existing completion port for our AI socket
	m_aiSocketMgr.SetCompletionPort(g_pMain->m_socketMgr.GetCompletionPort());
#else // ensure epoll/kqueue are still processed for client workers
	m_aiSocketMgr.SpawnWorkerThreads();
#endif

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
		|| !LoadRentalList()
		|| !LoadCoefficientTable()
		|| !LoadLevelUpTable()
		|| !LoadAllKnights()
		|| !LoadAllKnightsUserData()
		|| !LoadKnightsAllianceTable()
		|| !LoadUserRankings()
		|| !LoadKnightsCapeTable()
		|| !LoadKnightsRankTable()
		|| !LoadHomeTable()
		|| !LoadStartPositionTable()
		|| !LoadBattleTable()
		|| !MapFileLoad()
		|| !LoadKingSystem()
		|| !LoadMonsterSummonListTable()
		|| !LoadMonsterSummonListZoneTable()
		|| !LoadMonsterRespawnListTable()
		|| !LoadPremiumItemTable()
		|| !LoadPremiumItemExpTable()
		|| !LoadUserDailyOpTable()
		|| !LoadEventTriggerTable())
		return false;

	// Clear any remaining users in the currently logged in list
	// that may be left as a result of an improper shutdown.
	g_DBAgent.ClearRemainUsers();

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

	m_bMarsEnabled = ini.GetBool("ODBC", "GAME_MARS", true);

	ini.GetString("ODBC", "ACCOUNT_DSN", "KO_MAIN", m_strAccountDSN, false);
	ini.GetString("ODBC", "ACCOUNT_UID", "username", m_strAccountUID, false);
	ini.GetString("ODBC", "ACCOUNT_PWD", "password", m_strAccountPWD, false);

	bool bMarsEnabled = ini.GetBool("ODBC", "ACCOUNT_MARS", true);

	// Both need to be enabled to use MARS.
	if (!m_bMarsEnabled || !bMarsEnabled)
		m_bMarsEnabled = false;

	m_byWeather = ini.GetInt("TIMER", "WEATHER", 1);

	ini.GetString("BATTLE", "DAYS","1,6", m_nBattleZoneOpenDays, false);

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
	m_nGameMasterRHitDamage = ini.GetInt("SETTINGS","GAME_MASTER_R_HIT_DAMAGE", 30000);

	for (int i = 0; i < BIFROST_EVENT_COUNT; i++)
		m_nBifrostTime[i] = ini.GetInt("BIFROST",string_format("START_TIME%d",i+1).c_str(), 0);

	for (int i = 0; i < BORDER_DEFENSE_WAR_EVENT_COUNT; i++)
		m_nBorderDefenseWarTime[i] = ini.GetInt("BDW",string_format("START_TIME%d",i+1).c_str(), 0);

	for (int i = 0; i < CHAOS_EVENT_COUNT; i++)
		m_nChaosTime[i] = ini.GetInt("CHAOS",string_format("START_TIME%d",i+1).c_str(), 0);

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

	m_nPVPMonumentNation[ZONE_ARDREAM] = 0; 
	m_nPVPMonumentNation[ZONE_RONARK_LAND_BASE] = 0;
	m_nPVPMonumentNation[ZONE_RONARK_LAND] = 0;

	pTempleEvent.isActive = false;
	pTempleEvent.ActiveEvent = -1;
	pTempleEvent.StartTime = 0;
	pTempleEvent.AllUserCount = 0;
	pTempleEvent.KarusUserCount = 0;
	pTempleEvent.ElMoradUserCount = 0;

	g_timerThreads.push_back(new Thread(Timer_TempleEventTimer));
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
		FastGuard lock(m_accountNameLock);
		itr = m_accountNameMap.find(findName);
		return (itr != m_accountNameMap.end() ? itr->second : nullptr);
	}
	else if (type == TYPE_CHARACTER)
	{
		FastGuard lock(m_characterNameLock);
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
	FastGuard lock(m_accountNameLock);
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
	FastGuard lock(m_characterNameLock);
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
	FastGuard lock(m_characterNameLock);

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
		FastGuard lock(m_accountNameLock);
		m_accountNameMap.erase(upperName);
	}

	if (pSession->isInGame())
	{
		upperName = pSession->GetName();
		STRTOUPPER(upperName);

		FastGuard lock(m_characterNameLock);
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
void CGameServerDlg::SpawnEventNpc(uint16 sSid, bool bIsMonster, uint8 byZone, float fX, float fY, float fZ, uint16 sCount /*= 1*/, uint16 sRadius /*= 0*/, int16 nEventRoom)
{
	Packet result(AG_NPC_SPAWN_REQ);
	result	<< sSid << bIsMonster 
		<< byZone 
		<< uint16(fX * 10) << uint16(fY * 10) << uint16(fZ * 10) 
		<< sCount 
		<< sRadius
		<< nEventRoom;
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

uint32 CGameServerDlg::Timer_TempleEventTimer(void * lpParam)
{
	while (g_bRunning)
	{
		g_pMain->TempleEventTimer();
		sleep(6 * SECOND);
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

				g_pMain->SendBifrostTime(nullptr, true);
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

				g_pMain->SendBifrostTime(nullptr, true);
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
		SessionMap & sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
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
		g_pMain->m_socketMgr.ReleaseLock();
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

uint32 CGameServerDlg::Timer_ReloadUserAndKnightsRating(void * lpParam)
{
	while (g_bRunning)
	{
		// Update clan grades/rankings
		Packet result(WIZ_KNIGHTS_PROCESS, uint8(KNIGHTS_ALLLIST_REQ));
		g_pMain->AddDatabaseRequest(result);

		// Update user rankings
		g_pMain->LoadUserRankings();

		sleep(RELOAD_KNIGHTS_AND_USER_RATING * MINUTE);
	}
	return 0;
}

void CGameServerDlg::ReqUpdateConcurrent()
{
	Packet result(WIZ_ZONE_CONCURRENT);
	result	<< uint32(m_nServerNo)
		<< uint32(m_socketMgr.GetActiveSessionMap().size());
	g_pMain->m_socketMgr.ReleaseLock();
	AddDatabaseRequest(result);
}

void CGameServerDlg::AIServerConnect()
{
	// Are there any (note: we only use 1 now) idle/disconnected sessions?
	SessionMap & sessMap = m_aiSocketMgr.GetIdleSessionMap();

	// Copy the map (should only be 1 socket anyway) to avoid breaking the iterator
	SessionMap idleSessions = sessMap;
	m_aiSocketMgr.ReleaseLock();

	// No idle sessions? Excellent.
	if (idleSessions.empty())
		return;

	// Attempt reconnecting to the server
	foreach (itr, idleSessions)
	{
		CAISocket *pSock = static_cast<CAISocket *>(itr->second);
		bool bReconnecting = pSock->IsReconnecting();
		if (!pSock->Connect(m_AIServerIP.c_str(), AI_SERVER_PORT)) // couldn't connect... let's leave you alone for now
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
void CGameServerDlg::Send_Zone_Matched_Class(Packet *pkt, uint8 bZoneID, CUser* pExceptUser, uint8 nation, uint8 seekingPartyOptions, int16 nEventRoom)
{
	SessionMap & sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach (itr, sessMap)
	{
		CUser * pUser = TO_USER(itr->second);
		if (pUser == pExceptUser 
			|| !pUser->isInGame()
			|| pUser->GetZoneID() != bZoneID
			|| pUser->isInParty()) // looking for users to join the party
			continue;

		if (nEventRoom != pUser->GetEventRoom())
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
	g_pMain->m_socketMgr.ReleaseLock();
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
void CGameServerDlg::Send_Zone(Packet *pkt, uint8 bZoneID, CUser* pExceptUser /*= nullptr*/, uint8 nation /*= 0*/, int16 nEventRoom)
{
	SessionMap & sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach (itr, sessMap)
	{
		CUser * pUser = TO_USER(itr->second);

		if (!pUser->isInGame()
			|| pUser->GetZoneID() != bZoneID
			|| (nation != Nation::ALL && nation != pUser->GetNation()))
		{
			if (pExceptUser != nullptr)
				if (pUser == pExceptUser)
					continue;

			continue;
		}

		if (nEventRoom != pUser->GetEventRoom())
			continue;

		pUser->Send(pkt);
	}
	g_pMain->m_socketMgr.ReleaseLock();
}

/**
* @brief	Sends a packet to all users connected and logged into the server.
*
* @param	pkt		   	The packet.
* @param	pExceptUser	User to except. If specified, will ignore this user.
* @param	nation	   	Nation to allow. If unspecified, will default to Nation::ALL 
* 						which will send to all/both nations.
*/
void CGameServerDlg::Send_All(Packet *pkt, CUser* pExceptUser /*= nullptr*/, uint8 nation /*= 0*/, uint8 ZoneID /*= 0*/, int16 nEventRoom /*= -1*/)
{
	SessionMap & sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
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

		if (nEventRoom != pUser->GetEventRoom())
			continue;

		pUser->Send(pkt);
	}
	g_pMain->m_socketMgr.ReleaseLock();
}

void CGameServerDlg::Send_Region(Packet *pkt, C3DMap *pMap, int x, int z, CUser* pExceptUser, int16 nEventRoom)
{
	foreach_region(rx, rz)
		Send_UnitRegion(pkt, pMap, rx + x, rz + z, pExceptUser, nEventRoom);
}

void CGameServerDlg::Send_UnitRegion(Packet *pkt, C3DMap *pMap, int x, int z, CUser *pExceptUser, int16 nEventRoom)
{
	if (pMap == nullptr)
		return;

	FastGuard lock(pMap->m_lock);
	CRegion *pRegion = pMap->GetRegion(x, z);
	if (pRegion == nullptr)
		return;

	FastGuard lock2(pRegion->m_lock);
	foreach (itr, pRegion->m_RegionUserArray)
	{
		CUser *pUser = GetUserPtr(*itr);
		if (pUser == nullptr 
			|| pUser == pExceptUser 
			|| !pUser->isInGame())
			continue;

		if (nEventRoom != pUser->GetEventRoom())
			continue;

		pUser->Send(pkt);
	}
}

// TODO: Move the following two methods into a base CUser/CNpc class
void CGameServerDlg::Send_OldRegions(Packet *pkt, int old_x, int old_z, C3DMap *pMap, int x, int z, CUser* pExceptUser, int16 nEventRoom)
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

void CGameServerDlg::Send_NewRegions(Packet *pkt, int new_x, int new_z, C3DMap *pMap, int x, int z, CUser* pExceptUser, int16 nEventRoom)
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

void CGameServerDlg::Send_NearRegion(Packet *pkt, C3DMap *pMap, int region_x, int region_z, float curx, float curz, CUser* pExceptUser, int16 nEventRoom)
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

void CGameServerDlg::Send_FilterUnitRegion(Packet *pkt, C3DMap *pMap, int x, int z, float ref_x, float ref_z, CUser *pExceptUser, int16 nEventRoom)
{
	if (pMap == nullptr)
		return;

	FastGuard lock(pMap->m_lock);
	CRegion *pRegion = pMap->GetRegion(x, z);
	if (pRegion == nullptr)
		return;

	FastGuard lock2(pRegion->m_lock);
	foreach (itr, pRegion->m_RegionUserArray)
	{
		CUser *pUser = GetUserPtr(*itr);
		if (pUser == nullptr 
			|| pUser == pExceptUser 
			|| !pUser->isInGame())
			continue;

		if (nEventRoom != pUser->GetEventRoom())
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

	BattleZoneOpenTimer();	// Check if it's time for the BattleZone to open or end.

	// Check timed King events.
	foreach_stlmap (itr, m_KingSystemArray)
		itr->second->CheckKingTimer();

	// Every minute
	if (m_sMin != now.GetMinute())
	{

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
	SessionMap & sessMap = m_socketMgr.GetActiveSessionMap();
	foreach (itr, sessMap)
	{
		CUser * pUser = TO_USER(itr->second);
		pUser->m_iLoyaltyMonthly = 0;
		pUser->SendLoyaltyChange(); // update the client (note: official doesn't bother)
	}

	// Attempt to update the database in this thread directly, while the session map's locked.
	// This prevents new users from logging in before they've been reset (hence keeping last month's total).
	g_DBAgent.ResetLoyaltyMonthly();

	// Free the lock, so they can now continue to login and such.
	m_socketMgr.ReleaseLock();
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

	SessionMap & sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
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
	g_pMain->m_socketMgr.ReleaseLock();
}

void CGameServerDlg::SetGameTime()
{
	CIni ini(CONF_GAME_SERVER);
	ini.SetInt( "TIMER", "WEATHER", m_byWeather );
}

void CGameServerDlg::ResetPlayerRankings()
{
	if (m_sRankResetHour == 12) {
		m_sRankResetHour = 0;

		for (int nation = KARUS_ARRAY; nation <= ELMORAD_ARRAY; nation++)
		{
			foreach_stlmap_nolock(itr, g_pMain->m_UserRankingArray[nation+1]) {
				_USER_RANKING *pRank = g_pMain->m_UserRankingArray[nation+1].GetData(itr->first);

				if (pRank == nullptr)
					continue;

				CUser * pUser = g_pMain->GetUserPtr(pRank->m_socketID);

				if (pUser == nullptr || pUser->isInTempleEventZone())
					continue;

				pRank->m_iLoyaltyDaily = 0;
				pRank->m_iLoyaltyPremiumBonus;
				pUser->m_iLoyaltyDaily = 0;
				pUser->m_iLoyaltyPremiumBonus = 0;
			}
		}
	} else {
		m_sRankResetHour++;
	}
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

void CGameServerDlg::GetRegionUserIn(C3DMap *pMap, uint16 region_x, uint16 region_z, Packet & pkt, uint16 & t_count, int16 nEventRoom)
{
	if (pMap == nullptr)
		return;

	FastGuard lock(pMap->m_lock);
	CRegion *pRegion = pMap->GetRegion(region_x, region_z);
	if (pRegion == nullptr)
		return;

	FastGuard lock2(pRegion->m_lock);
	foreach (itr, pRegion->m_RegionUserArray)
	{
		CUser *pUser = GetUserPtr(*itr);
		if (pUser == nullptr 
			|| !pUser->isInGame())
			continue;

		if (nEventRoom != pUser->GetEventRoom())
			continue;

		pkt << uint8(0) << pUser->GetSocketID();
		pUser->GetUserInfo(pkt);
		t_count++;
	}
}

void CGameServerDlg::GetRegionUserList(C3DMap* pMap, uint16 region_x, uint16 region_z, Packet & pkt, uint16 & t_count, int16 nEventRoom)
{
	if (pMap == nullptr)
		return;

	FastGuard lock(pMap->m_lock);
	CRegion *pRegion = pMap->GetRegion(region_x, region_z);
	if (pRegion == nullptr)
		return;

	FastGuard lock2(pRegion->m_lock);
	foreach (itr, pRegion->m_RegionUserArray)
	{
		CUser *pUser = GetUserPtr(*itr);
		if (pUser == nullptr 
			|| !pUser->isInGame())
			continue;

		if (nEventRoom != pUser->GetEventRoom())
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

void CGameServerDlg::GetRegionMerchantUserIn(C3DMap *pMap, uint16 region_x, uint16 region_z, Packet & pkt, uint16 & t_count, int16 nEventRoom)
{
	if (pMap == nullptr)
		return;

	FastGuard lock(pMap->m_lock);
	CRegion *pRegion = pMap->GetRegion(region_x, region_z);
	if (pRegion == nullptr)
		return;

	FastGuard lock2(pRegion->m_lock);
	foreach (itr, pRegion->m_RegionUserArray)
	{
		CUser *pUser = GetUserPtr(*itr);
		if (pUser == nullptr 
			|| !pUser->isInGame()
			|| !pUser->isMerchanting())
			continue;

		if (nEventRoom != pUser->GetEventRoom())
			continue;

		pkt << pUser->GetSocketID()
			<< pUser->GetMerchantState() // 0 is selling, 1 is buying
			<< pUser->m_bPremiumMerchant; // Type of merchant [normal - gold] // bool

		for (int i = 0, listCount = (pUser->m_bPremiumMerchant ? 8 : 4); i < listCount; i++)
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

void CGameServerDlg::GetRegionNpcIn(C3DMap *pMap, uint16 region_x, uint16 region_z, Packet & pkt, uint16 & t_count, int16 nEventRoom)
{
	if (!m_bPointCheckFlag
		|| pMap == nullptr)
		return;

	FastGuard lock(pMap->m_lock);
	CRegion * pRegion = pMap->GetRegion(region_x, region_z);
	if (pRegion == nullptr)
		return;

	FastGuard lock2(pRegion->m_lock);
	foreach (itr, pRegion->m_RegionNpcArray)
	{
		CNpc *pNpc = GetNpcPtr(*itr);
		if (pNpc == nullptr
			|| pNpc->isDead())
			continue;

		if (nEventRoom != pNpc->GetEventRoom())
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

	FastGuard lock(pMap->m_lock);
	foreach_region(x, z)
	{
		uint16 region_x = rx + x, region_z = rz + z;
		CRegion * pRegion = pMap->GetRegion(region_x, region_z);
		if (pRegion == nullptr)
			continue;

		FastGuard lock2(pRegion->m_lock);

		// Add all potential NPCs to list
		foreach (itr, pRegion->m_RegionNpcArray)
			pList->push_back(*itr);

		// Add all potential users to list
		foreach (itr, pRegion->m_RegionUserArray)
			pList->push_back(*itr);
	}
}

void CGameServerDlg::GetRegionNpcList(C3DMap *pMap, uint16 region_x, uint16 region_z, Packet & pkt, uint16 & t_count, int16 nEventRoom)
{
	if (!m_bPointCheckFlag
		|| pMap == nullptr)
		return;

	FastGuard lock(pMap->m_lock);
	CRegion * pRegion = pMap->GetRegion(region_x, region_z);
	if (pRegion == nullptr)
		return;

	FastGuard lock2(pRegion->m_lock);
	foreach (itr, pRegion->m_RegionNpcArray)
	{
		CNpc *pNpc = GetNpcPtr(*itr);
		if (pNpc == nullptr || pNpc->isDead())
			continue;

		if (nEventRoom != pNpc->GetEventRoom())
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

	SessionMap & sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
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
	g_pMain->m_socketMgr.ReleaseLock();

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
	m_arNpcArray.m_lock.Acquire();
	foreach_stlmap_nolock (itr, m_arNpcArray)
	{
		if (itr->second->isAlive())
			itr->second->SendInOut(INOUT_OUT, 0.0f, 0.0f, 0.0f);

		// decrease the reference count (freeing it if nothing else is using it)
		itr->second->DecRef();
	}

	// all the data should now be freed (if not, it will be by whatever's using it)
	m_arNpcArray.m_UserTypeMap.clear();
	m_arNpcArray.m_lock.Release();

	// Now remove all spawns from all regions
	foreach_stlmap (itr, m_ZoneArray)
	{
		C3DMap *pMap = itr->second;
		if (pMap == nullptr)
			continue;

		FastGuard lock(pMap->m_lock);
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

	FastGuard lock(m_arNpcArray.m_lock);
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

	if (m_byBattleOpen == NO_BATTLE)
	{
		std::list<std::string> vargs = StrSplit(m_nBattleZoneOpenDays, ",");
		uint8 nDaySize = vargs.size();
		if (nDaySize > 0)
		{
			uint8 nDay = 0;
			for (int i = 0; i < nDaySize; i++)
			{
				nDay = atoi(vargs.front().c_str());
				if (nDay == nWeekDay)
					for (int x = 0; x < WAR_TIME_COUNT; x++)
						if (m_nBattleZoneOpenHourStart[x] == nHour)
							BattleZoneOpen(BATTLEZONE_OPEN,m_nBattlezoneOpenWarZone[x]);
				vargs.pop_front();
			}
		}
	}
	else if (m_byBattleOpen == NATION_BATTLE)	
	{
		BattleZoneCurrentUsers();

		int32 WarElapsedTime = int32(UNIXTIME) - m_byBattleOpenedTime;

		if (m_bVictory == 0 && WarElapsedTime >= (m_byBattleTime / 2) && WarElapsedTime < m_byBattleTime) // War half time.
		{
			uint8 winner_nation = 0;

			if (m_sKillElmoNpc == m_sKillKarusNpc)
			{
				if (m_sKarusDead > m_sElmoradDead)
					winner_nation = ELMORAD;
				else if (m_sElmoradDead > m_sKarusDead)	
					winner_nation = KARUS;
			}
			else if (m_sKillKarusNpc > m_sKillElmoNpc)
				winner_nation = KARUS;
			else if (m_sKillElmoNpc > m_sKillKarusNpc)
				winner_nation = ELMORAD;

			if (winner_nation == 0) // Draw
				BattleZoneClose();
			else
				BattleZoneResult(winner_nation);
		}
		else if (g_pMain->m_bVictory != 0 && WarElapsedTime <  m_byBattleTime) // Won the war.
		{
			m_sBattleTimeDelay++;

			if ((m_sBattleTimeDelay * 6) >= 300)
			{
				m_sBattleTimeDelay = 0;
				Announcement(UNDER_ATTACK_NOTIFY);
			}
		}
		else if (WarElapsedTime >=  m_byBattleTime) // War is over.
			BattleZoneClose();
		else if (g_pMain->m_bVictory == 0) // War continues.
		{
			m_sBattleTimeDelay++;

			if ((m_sBattleTimeDelay * 6) >= 300)
			{
				m_sBattleTimeDelay = 0;
				Announcement(DECLARE_BATTLE_ZONE_STATUS);
			}
		}
	}

	if (m_byBanishFlag)
	{
		m_sBanishDelay++;

		if (m_sBanishDelay == 2)
			Announcement(DECLARE_BAN);
		else if (m_sBanishDelay == 6) {
			m_byBanishFlag = false;
			m_sBanishDelay = 0;
			BanishLosers();
		}
	}
}

void CGameServerDlg::BattleZoneResult(uint8 nation)
{
	m_bVictory = nation;
	m_byKarusOpenFlag = nation == ELMORAD ? true : false;
	m_byElmoradOpenFlag = nation == KARUS ? true : false;
	m_byBanishFlag = true;
	m_sBanishDelay = 0;
	Announcement(DECLARE_WINNER, g_pMain->m_bVictory);
	Announcement(DECLARE_LOSER, nation == KARUS ? ELMORAD : KARUS);
}

void CGameServerDlg::BattleZoneOpen(int nType, uint8 bZone /*= 0*/)
{
	if(nType == BATTLEZONE_OPEN)
	{
		m_byBattleOpen = NATION_BATTLE;	
		m_byOldBattleOpen = NATION_BATTLE;
		m_byBattleZone = bZone;
		m_byBattleOpenedTime = int32(UNIXTIME);

		KickOutZoneUsers(ZONE_ARDREAM);
		KickOutZoneUsers(ZONE_RONARK_LAND_BASE);
		KickOutZoneUsers(ZONE_RONARK_LAND);
		KickOutZoneUsers(ZONE_BIFROST);
		KickOutZoneUsers(ZONE_KROWAZ_DOMINION);
	}
	else if (nType == SNOW_BATTLEZONE_OPEN)
	{
		m_byBattleOpen = SNOW_BATTLE;	
		m_byOldBattleOpen = SNOW_BATTLE;
		m_byBattleOpenedTime = int32(UNIXTIME);

		KickOutZoneUsers(ZONE_ARDREAM);
		KickOutZoneUsers(ZONE_RONARK_LAND_BASE);
		KickOutZoneUsers(ZONE_RONARK_LAND);
		KickOutZoneUsers(ZONE_BIFROST);
		KickOutZoneUsers(ZONE_KROWAZ_DOMINION);
	}
	else if (nType == BATTLEZONE_CLOSE)
	{
		Announcement(BATTLEZONE_CLOSE);
	}
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

	SessionMap & sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
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
	g_pMain->m_socketMgr.ReleaseLock();
}

/**
* @brief	Kicks invaders out of the invaded nation after a war
*			and resets captains.
**/
void CGameServerDlg::BanishLosers()
{
	SessionMap & sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach (itr, sessMap)
	{
		CUser *pUser = TO_USER(itr->second); 
		if (!pUser->isInGame())
			continue;

		// Reset captains
		if (m_byBattleOpen == NO_BATTLE)
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
		else if (m_byBattleOpen == NO_BATTLE)
		{
			// Kick out invaders
			if ((pUser->GetZoneID() <= ELMORAD && pUser->GetZoneID() != pUser->GetNation())
				|| pUser->GetMap()->isWarZone())
				pUser->KickOutZoneUser(true);
		}
	}
	g_pMain->m_socketMgr.ReleaseLock();
}

void CGameServerDlg::ResetBattleZone()
{
	m_bVictory = 0;
	m_byBanishFlag = false;
	m_sBanishDelay = 0;
	m_bKarusFlag = 0;
	m_bElmoradFlag = 0;
	m_byKarusOpenFlag = false;
	m_byElmoradOpenFlag = false;
	m_byBattleOpen = NO_BATTLE;
	m_byOldBattleOpen = NO_BATTLE;
	m_byBattleOpenedTime = 0;
	m_sKarusDead = 0;
	m_sElmoradDead = 0;
	m_byBattleSave = false;
	m_sKarusCount = 0;
	m_sElmoradCount = 0;
	m_sKillKarusNpc = 0;
	m_sKillElmoNpc = 0;
	m_sBattleTimeDelay = 0;
}

void CGameServerDlg::TempleEventTimer()
{
	return;
	uint32 nHour = g_localTime.tm_hour;
	uint32 nMinute = g_localTime.tm_min;

	if (m_nTempleEventRemainSeconds > 0)
		m_nTempleEventRemainSeconds--;

	if (m_nTempleEventFinishRemainSeconds > 0)
		m_nTempleEventFinishRemainSeconds -= 6;

	if (m_nTempleEventFinishRemainSeconds == 0 && pTempleEvent.isActive)
	{
		pTempleEvent.isActive = false;
		TempleEventFinish();
	}

	for(int i = 0; i < BORDER_DEFENSE_WAR_EVENT_COUNT; i++)
	{
		if (nHour == m_nBorderDefenseWarTime[i] 
		&& nMinute == 20 
			&& pTempleEvent.ActiveEvent != TEMPLE_EVENT_BORDER_DEFENCE_WAR) 
		{
			pTempleEvent.ActiveEvent = TEMPLE_EVENT_BORDER_DEFENCE_WAR;
			pTempleEvent.isActive = false;
			m_nTempleEventRemainSeconds = 600;
			TempleEventStart();
			break;
		} 
		else if (nHour == m_nBorderDefenseWarTime[i] 
		&& nMinute == 30 
			&& pTempleEvent.ActiveEvent == TEMPLE_EVENT_BORDER_DEFENCE_WAR
			&& pTempleEvent.isActive)
		{
			TempleEventCreateRooms();
			m_nTempleEventRemainSeconds = 0;
			TempleEventStart();
			TempleEventTeleportUsers();
			break;
		} 
	}

	for(int i = 0; i < CHAOS_EVENT_COUNT; i++)
	{
		if (nHour == m_nChaosTime[i] 
		&& nMinute == 0 
			&& pTempleEvent.ActiveEvent != TEMPLE_EVENT_CHAOS)
		{
			pTempleEvent.ActiveEvent = TEMPLE_EVENT_CHAOS;
			m_nTempleEventRemainSeconds = 600;
			TempleEventStart();
			break;
		}  
		else if (nHour == m_nChaosTime[i]
		&& nMinute == 11 
			&& pTempleEvent.ActiveEvent == TEMPLE_EVENT_CHAOS
			&& pTempleEvent.isActive) {
				TempleEventCreateRooms();
				m_nTempleEventRemainSeconds = 0;
				TempleEventStart();
				TempleEventTeleportUsers();
				break;
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

void CGameServerDlg::TempleEventCreateRooms()
{
	uint8 nMaxUserCount = 0;

	switch (pTempleEvent.ActiveEvent)
	{
	case TEMPLE_EVENT_BORDER_DEFENCE_WAR:
		nMaxUserCount = 16;
		break;
	case TEMPLE_EVENT_CHAOS:
		nMaxUserCount = 20;
		break;
	case TEMPLE_EVENT_JURAD_MOUNTAIN:
		nMaxUserCount = 16;
		break;
	}

	uint8 nCurrentUserCount = 0;
	uint8 nCurrentEventRoom = 1;

	foreach_stlmap_nolock(itr, g_pMain->m_TempleEventUserArray)
	{
		CUser * pUser = GetUserPtr(itr->second->m_socketID);

		if (pUser == nullptr 
			||	!pUser->isInGame())
			continue;

		pUser->UpdateEventUser(pUser->GetSocketID(), nCurrentEventRoom);
		nCurrentUserCount++;

		if (nCurrentUserCount == nMaxUserCount)
		{
			nCurrentUserCount = 0;
			nCurrentEventRoom++;
		}
	}
}

void CGameServerDlg::TempleEventTeleportUsers()
{
	uint8 ZoneID = 0;

	switch (pTempleEvent.ActiveEvent)
	{
	case TEMPLE_EVENT_BORDER_DEFENCE_WAR:
		ZoneID = ZONE_BORDER_DEFENSE_WAR;
		break;
	case TEMPLE_EVENT_CHAOS:
		ZoneID = ZONE_CHAOS_DUNGEON;
		break;
	case TEMPLE_EVENT_JURAD_MOUNTAIN:
		ZoneID = ZONE_JURAD_MOUNTAIN;
		break;
	}

	_START_POSITION *pStartPosition = GetStartPosition(ZoneID);

	if (pStartPosition == nullptr)
	{
		TRACE("### TempleEventTeleportUsers - StartPosition not found : Zone ID=%d",ZoneID);
		return;
	}

	foreach_stlmap_nolock(itr, m_TempleEventUserArray)
	{
		CUser * pUser = GetUserPtr(itr->second->m_socketID);

		if (pUser == nullptr 
			||	!pUser->isInGame())
			continue;

		if (ZoneID == ZONE_CHAOS_DUNGEON)
			pUser->RobItem(CHAOS_MAP,1);

		if (pUser->GetNation() == KARUS)
		{
			pUser->ZoneChange(ZoneID, 
				(float)pStartPosition->sKarusZ + myrand(0, pStartPosition->bRangeX), 
				(float)pStartPosition->sKarusX + myrand(0, pStartPosition->bRangeZ));
		}
		else
		{
			pUser->ZoneChange(ZoneID, 
				(float)pStartPosition->sElmoradX + myrand(0, pStartPosition->bRangeX), 
				(float)pStartPosition->sElmoradZ + myrand(0, pStartPosition->bRangeZ));
		}
	}

	m_nTempleEventFinishRemainSeconds = 300; // 20 minute is both
	pTempleEvent.isActive = true;
}

void CGameServerDlg::TempleEventFinish()
{
	uint8 ZoneID = 0;

	switch (pTempleEvent.ActiveEvent)
	{
	case TEMPLE_EVENT_BORDER_DEFENCE_WAR:
		ZoneID = ZONE_BORDER_DEFENSE_WAR;
		SendItemZoneUsers(ZoneID, CERTIFICATE_OF_VICTORY); // winner reward...
		SendItemZoneUsers(ZoneID, BORDER_SECURITY_SCROLL);  // winner reward...
		SendItemZoneUsers(ZoneID, RED_TREASURE_CHEST);
		break;
	case TEMPLE_EVENT_CHAOS:
		ZoneID = ZONE_CHAOS_DUNGEON;
		// Get user rank for reward
		SendItemZoneUsers(ZoneID, VOUCHER_OF_CHAOS);
		/*
		SendItemZoneUsers(ZoneID, VOUCHER_OF_ORACLE);
		SendItemZoneUsers(ZoneID, BLUE_TREASURE_CHEST);
		SendItemZoneUsers(ZoneID, GREEN_TREASURE_CHEST);
		SendItemZoneUsers(ZoneID, RED_TREASURE_CHEST);
		*/
		break;
	case TEMPLE_EVENT_JURAD_MOUNTAIN:
		ZoneID = ZONE_JURAD_MOUNTAIN;
		break;
	}

	pTempleEvent.ActiveEvent = -1;

	foreach_stlmap_nolock(itr, m_TempleEventUserArray)
	{
		CUser * pUser = GetUserPtr(itr->second->m_socketID);

		if (pUser == nullptr 
			||	!pUser->isInGame())
			continue;

		pUser->UpdateEventUser(pUser->GetSocketID(), -1);

		_USER_RANKING * pRankInfo = m_UserRankingArray->GetData(itr->second->m_socketID);

		if (pRankInfo != nullptr)
		{
			int64 nChangeExp = -1;

			if (ZoneID == ZONE_BORDER_DEFENSE_WAR)
				if (pUser->GetLevel() < 58)
					nChangeExp = int64((pUser->GetLevel()-20) * (3000 + 100/* Temp Score */* 1000));
				else
					nChangeExp = int64((pUser->GetLevel()+55) * (20000 + 100/* Temp Score */ * 500));				
			else if (ZoneID == ZONE_CHAOS_DUNGEON)
				nChangeExp = int64((pUser->GetLevel()^3) * 0.15 * (5 * pRankInfo->m_KillCount - pRankInfo->m_DeathCount));

			if (nChangeExp > 0)
				pUser->ExpChange(nChangeExp);

			m_UserRankingArray->DeleteData(itr->second->m_socketID);
		}

		TempleEventKickOutUser(pUser);
	}

	m_TempleEventUserArray.DeleteAllData();
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
}

void CGameServerDlg::TempleEventKickOutUser(CUser *pUser)
{
	if (pUser == nullptr || !pUser->isInGame())
		return;

	uint8 nZoneID = 0;

	if (pUser->GetZoneID() == ZONE_BORDER_DEFENSE_WAR)
		nZoneID = pUser->GetNation();
	else if (pUser->GetZoneID() == ZONE_CHAOS_DUNGEON)
	{
		if (pUser->GetLevel() <  35)
			nZoneID = ZONE_MORADON;
		else if (pUser->GetLevel() >=  35 && pUser->GetLevel() <=59)
			nZoneID = ZONE_ARDREAM;
		else if (pUser->GetLevel() >=  60 && pUser->GetLevel() <=69)
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

	if (pUser->GetNation() == KARUS)
	{
		pUser->ZoneChange(nZoneID, 
			(float)pStartPosition->sKarusZ + myrand(0, pStartPosition->bRangeX), 
			(float)pStartPosition->sKarusX + myrand(0, pStartPosition->bRangeZ));
	}
	else
	{
		pUser->ZoneChange(nZoneID, 
			(float)pStartPosition->sElmoradX + myrand(0, pStartPosition->bRangeX), 
			(float)pStartPosition->sElmoradZ + myrand(0, pStartPosition->bRangeZ));
	}
}

void CGameServerDlg::Announcement(uint8 type, int nation, int chat_type, CUser* pExceptUser)
{
	string chatstr; 

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
		GetServerResource(IDS_BANISH_USER, &chatstr);
		break;
	case DECLARE_BATTLE_ZONE_STATUS:
		GetServerResource(IDS_BATTLEZONE_STATUS, &chatstr,  m_sKarusDead, m_sElmoradDead);
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
			GetServerResource(IDS_KARUS_CAPTAIN_DEPRIVE, &chatstr, pKnights == nullptr ? "***" : pKnights->GetName(), pExceptUser->GetName().c_str());
		}
		break;
	case ELMORAD_CAPTAIN_DEPRIVE_NOTIFY:
		if (pExceptUser)
		{
			CKnights *pKnights = g_pMain->GetClanPtr(pExceptUser->GetClanID());
			GetServerResource(IDS_ELMO_CAPTAIN_DEPRIVE, &chatstr, pKnights == nullptr ? "***" : pKnights->GetName(),  pExceptUser->GetName().c_str());
		}
		break;
	}

	Packet result;
	string finalstr;
	GetServerResource(IDP_ANNOUNCEMENT, &finalstr, chatstr.c_str());
	ChatPacket::Construct(&result, (uint8) chat_type, &finalstr);
	Send_All(&result, nullptr, nation);
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
	FastGuard lock(m_userRankingsLock);

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

	SessionMap & sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach (itr, sessMap)
	{
		CUser *pUser = TO_USER(itr->second);
		if (pUser->isInGame())
			count++;

		pUser->Disconnect();
	}
	g_pMain->m_socketMgr.ReleaseLock();
	return count;
}

/**
* @brief	Generates a new item serial.
*/
uint64 CGameServerDlg::GenerateItemSerial()
{
	static FastMutex _mutex;

	MYINT64 serial;
	MYSHORT	increase;
	serial.i = 0;

	time_t t = UNIXTIME;
	struct tm * ptm;
	ptm = gmtime(&t);

	FastGuard lock(_mutex);
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
	SessionMap & sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
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
			short x, y;

			if (pUser->GetStartPosition(x,y))
			{
				pUser->ZoneChange(TargetZoneID,x,y);
				continue;
			}
		}

		C3DMap * pMap = (pUser->GetNation() == KARUS ? pKarusMap : pElMoradMap);
		pUser->ZoneChange(pMap->m_nZoneNumber, pMap->m_fInitX, pMap->m_fInitZ);
	}
	g_pMain->m_socketMgr.ReleaseLock();
}

void CGameServerDlg::SendItemZoneUsers(uint8 ZoneID, uint32 nItemID, uint16 sCount)
{
	// TODO: Make this localised to zones.
	SessionMap & sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();

	foreach (itr, sessMap)
	{
		CUser * pUser = TO_USER(itr->second);
		if (!pUser->isInGame()
			|| pUser->GetZoneID() != ZoneID) 
			continue;

		pUser->RobItem(nItemID, sCount);
	}
	g_pMain->m_socketMgr.ReleaseLock();
}

void CGameServerDlg::Send_CommandChat(Packet *pkt, int nation, CUser* pExceptUser)
{
	SessionMap & sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach (itr, sessMap)
	{
		CUser * pUser = TO_USER(itr->second);
		if (pUser->isInGame() && (nation == 0 || nation == pUser->GetNation()))
			pUser->Send(pkt);
	}
	g_pMain->m_socketMgr.ReleaseLock();
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
	SessionMap & sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
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
	g_pMain->m_socketMgr.ReleaseLock();

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

	printf("Suspending server...\n");
	m_socketMgr.SuspendServer();

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
}