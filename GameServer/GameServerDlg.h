#pragma once

#ifdef GAMESERVER

#include "resource.h"
#include "LuaEngine.h"

#include "Define.h"
#include "ChatHandler.h"

class C3DMap;
class CUser;

#include "LoadServerData.h"

#include "User.h"
#include "AISocket.h"
#include "../shared/ClientSocketMgr.h"

<<<<<<< HEAD
typedef std::unordered_map<std::string, CUser *> NameMap;
=======
typedef std::map<std::string, CUser *>	NameMap;
typedef	std::map<uint16, uint16>					ForgettenTempleMonsterList;
>>>>>>> koserver2

class CGameServerDlg
{
public:	
	CGameServerDlg();
	bool Startup();

	void GetTimeFromIni();

	bool LoadItemTable();
	bool LoadSetItemTable();
	bool LoadItemExchangeTable();
	bool LoadItemUpgradeTable();
	bool LoadItemOpTable();
	bool LoadServerResourceTable();
	bool LoadQuestHelperTable();
	bool LoadQuestMonsterTable();
	bool LoadMagicTable();
	bool LoadMagicType1();
	bool LoadMagicType2();
	bool LoadMagicType3();
	bool LoadMagicType4();
	bool LoadMagicType5();
	bool LoadMagicType6();
	bool LoadMagicType7();
	bool LoadMagicType8();
	bool LoadMagicType9();
	bool LoadRentalList();
	bool LoadCoefficientTable();
	bool LoadLevelUpTable();
<<<<<<< HEAD
	bool LoadAllKnights();
	bool LoadAllKnightsUserData();
	bool LoadKnightsAllianceTable();
	bool LoadUserRankings();
	void CleanupUserRankings();
	bool LoadKnightsCapeTable();
	bool LoadKnightsRankTable(bool bWarTime = false);
	bool LoadHomeTable();
	bool LoadStartPositionTable();
=======
	bool LoadAllKnights(bool bIsslient = false);
	bool LoadAllKnightsUserData(bool bIsslient = false);
	bool LoadKnightsAllianceTable(bool bIsslient = false);
	bool LoadUserRankings();
	void CleanupUserRankings();
	bool LoadKnightsCapeTable();
	bool LoadKnightsRankTable(bool bWarTime = false, bool bIsslient = false);
	bool LoadStartPositionTable();
	bool LoadStartPositionRandomTable();
>>>>>>> koserver2
	bool LoadBattleTable();
	bool LoadKingSystem();
	bool LoadMonsterSummonListTable();
	bool LoadMonsterSummonListZoneTable();
	bool LoadMonsterRespawnListTable();
<<<<<<< HEAD
=======
	bool LoadMonsterRespawnListInformationTable();
>>>>>>> koserver2
	bool LoadPremiumItemTable();
	bool LoadPremiumItemExpTable();
	bool LoadUserDailyOpTable();
	bool LoadEventTriggerTable();
<<<<<<< HEAD
=======
	bool LoadMonsterChallengeTable();
	bool LoadMonsterChallengeSummonListTable();
	bool LoadUserItemTable();
	bool LoadObjectPosTable();
>>>>>>> koserver2

	bool MapFileLoad();
	bool LoadNoticeData();

	void AIServerConnect();

<<<<<<< HEAD
	static uint32 THREADCALL Timer_TempleEventTimer(void * lpParam);
=======
	static uint32 THREADCALL Timer_CheckGameEvents(void * lpParam);
>>>>>>> koserver2
	static uint32 THREADCALL Timer_BifrostTime(void * lpParam);
	static uint32 THREADCALL Timer_UpdateGameTime(void * lpParam);
	static uint32 THREADCALL Timer_UpdateSessions(void * lpParam);
	static uint32 THREADCALL Timer_UpdateConcurrent(void * lpParam);
<<<<<<< HEAD
	static uint32 THREADCALL Timer_ReloadUserAndKnightsRating(void * lpParam);
=======
>>>>>>> koserver2

	void ReqUpdateConcurrent();

	void SendFlyingSantaOrAngel();
	void BattleZoneCurrentUsers();
	void GetCaptainUserPtr();
	void Send_CommandChat(Packet *pkt, int nation, CUser* pExceptUser = nullptr);
	void KickOutZoneUsers(uint8 ZoneID, uint8 TargetZoneID = 0);
	void SendItemZoneUsers(uint8 ZoneID, uint32 nItemID, uint16 sCount = 1);
<<<<<<< HEAD
=======
	void SendItemEventRoom(uint16 nEventRoom,uint32 nItemID, uint16 sCount = 1);
>>>>>>> koserver2
	uint64 GenerateItemSerial();
	int KickOutAllUsers();
	void CheckAliveUser();
	int GetKnightsGrade(uint32 nPoints);
	void WritePacketLog();
	uint16 GetKnightsAllMembers(uint16 sClanID, Packet & result, uint16 & pktSize, bool bClanLeader);
	void GetUserRank(CUser *pUser);
<<<<<<< HEAD
	void Announcement(uint8 type, int nation=0, int chat_type=8, CUser* pExceptUser = nullptr);
=======
	void Announcement(uint16 type, int nation=0, int chat_type=8, CUser* pExceptUser = nullptr, CNpc *pExpectNpc = nullptr);
>>>>>>> koserver2
	void ResetBattleZone();
	void BanishLosers();
	void BattleZoneVictoryCheck();
	void BattleZoneOpenTimer();
	void BattleZoneOpen(int nType, uint8 bZone = 0);
	void BattleZoneClose();
	void BattleZoneResult(uint8 nation);
<<<<<<< HEAD
=======
	void BattleWinnerResult(BattleWinnerTypes winnertype);
>>>>>>> koserver2
	void AliveUserCheck();
	void Send_PartyMember(int party, Packet *result);
	void Send_KnightsMember(int index, Packet *pkt);
	void Send_KnightsAlliance(uint16 sAllianceID, Packet *pkt);
	void SetGameTime();
<<<<<<< HEAD
	void ResetPlayerRankings();
=======
	void ResetPlayerRankings(uint8 ZoneID = 0);
>>>>>>> koserver2
	void UpdateWeather();
	void UpdateGameTime();
	void ResetLoyaltyMonthly();
	void SendAllUserInfo();
	void DeleteAllNpcList(int flag = 0);
	CNpc*  FindNpcInZone(uint16 sPid, uint8 byZone);
<<<<<<< HEAD
	void TempleEventTimer(uint32 interval);
	void TempleEventStart();
	void TempleEventCreateRooms();
	void TempleEventTeleportUsers();
=======
	void ForgettenTempleEventTimer();
	uint8 GetMonsterChallengeTime();
	void TempleEventTimer();
	void TempleEventStart();
	void TempleEventTeleportUsers();
	uint8 TempleEventGetRoomUsers(uint16 nEventRoom = 0);
	void TempleEventSummonChaosCubes();
>>>>>>> koserver2
	void TempleEventFinish();
	void TempleEventGetActiveEventTime(CUser *pUser);
	void TempleEventSendActiveEventTime(CUser *pUser);
	void TempleEventKickOutUser(CUser *pUser);
<<<<<<< HEAD
=======
	void TempleEventReset();
	void AddEventUser(CUser * pUser);
	void RemoveEventUser(CUser * pUser);
	void UpdateEventUser(CUser * pUser, uint16 nEventRoom = 0);
	void SetEventUser(CUser *pUser);
	std::string GetBattleAndNationMonumentName(int16 TrapNumber = -1, uint8 ZoneID = 0);
	void CheckNationMonumentRewards();

	void ReloadKnightAndUserRanks();
	void SetPlayerRankingRewards(uint16 ZoneID);

	bool IsDuplicateItem(uint32 nItemID, uint64 nItemSerial);
	void AddUserItem(uint32 nItemID, uint64 nItemSerial);
>>>>>>> koserver2

	void AddDatabaseRequest(Packet & pkt, CUser *pUser = nullptr);

	// Get info for NPCs in regions around user (WIZ_REQ_NPCIN)
	void NpcInOutForMe(CUser* pSendUser);

	// Get info for NPCs in region
<<<<<<< HEAD
	void GetRegionNpcIn(C3DMap* pMap, uint16 region_x, uint16 region_z, Packet & pkt, uint16 & t_count, int16 nEventRoom = 0);

	// Get list of NPC IDs in region
	void GetRegionNpcList(C3DMap* pMap, uint16 region_x, uint16 region_z, Packet & pkt, uint16 & t_count, int16 nEventRoom = 0);
=======
	void GetRegionNpcIn(C3DMap* pMap, uint16 region_x, uint16 region_z, Packet & pkt, uint16 & t_count, uint16 nEventRoom = 0);

	// Get list of NPC IDs in region
	void GetRegionNpcList(C3DMap* pMap, uint16 region_x, uint16 region_z, Packet & pkt, uint16 & t_count, uint16 nEventRoom = 0);
>>>>>>> koserver2

	// Get list of NPCs for regions around user (WIZ_NPC_REGION)
	void RegionNpcInfoForMe(CUser* pSendUser);	

	// Get raw list of all units in regions surrounding pOwner.
	void GetUnitListFromSurroundingRegions(Unit * pOwner, std::vector<uint16> * pList);

	// Get info for users in regions around user (WIZ_REQ_USERIN)
	void UserInOutForMe(CUser* pSendUser);

	// Get info for users in region
<<<<<<< HEAD
	void GetRegionUserIn(C3DMap* pMap, uint16 region_x, uint16 region_z, Packet & pkt, uint16 & t_count, int16 nEventRoom = 0);

	// Get list of user IDs in region
	void GetRegionUserList(C3DMap* pMap, uint16 region_x, uint16 region_z, Packet & pkt, uint16 & t_count, int16 nEventRoom = 0);
=======
	void GetRegionUserIn(C3DMap* pMap, uint16 region_x, uint16 region_z, Packet & pkt, uint16 & t_count, uint16 nEventRoom = 0);

	// Get list of user IDs in region
	void GetRegionUserList(C3DMap* pMap, uint16 region_x, uint16 region_z, Packet & pkt, uint16 & t_count, uint16 nEventRoom = 0);
>>>>>>> koserver2

	// Get list of users for regions around user (WIZ_REGIONCHANGE)
	void RegionUserInOutForMe(CUser* pSendUser);

	// Get info for merchants in regions around user
	void MerchantUserInOutForMe(CUser* pSendUser);

	// Get war status
	INLINE bool isWarOpen() { return m_byBattleOpen != NO_BATTLE;} 

	// Get list of merchants in region
<<<<<<< HEAD
	void GetRegionMerchantUserIn(C3DMap* pMap, uint16 region_x, uint16 region_z, Packet & pkt, uint16 & t_count, int16 nEventRoom = 0);
=======
	void GetRegionMerchantUserIn(C3DMap* pMap, uint16 region_x, uint16 region_z, Packet & pkt, uint16 & t_count, uint16 nEventRoom = 0);
>>>>>>> koserver2

	void SendHelpDescription(CUser *pUser, std::string sHelpMessage);

	INLINE bool isPermanentMessageSet() { return m_bPermanentChatMode; }
	void SetPermanentMessage(const char * format, ...);

	void HandleConsoleCommand(const char * msg);

	template <ChatType chatType>
	INLINE void SendChat(const char * msg, uint8 byNation = Nation::ALL, bool bFormatNotice = false)
	{
		Packet result;
		std::string buffer;

		if (bFormatNotice)
			GetServerResource(IDP_ANNOUNCEMENT, &buffer, msg);
		else
			buffer = msg;

		ChatPacket::Construct(&result, (uint8) chatType, &buffer);
		Send_All(&result, nullptr, byNation);
	}

	template <ChatType chatType>
	INLINE void SendChatToZone(const char * msg, uint8 ZoneID, uint8 byNation = Nation::ALL, bool bFormatNotice = false)
	{
		Packet result;
		std::string buffer;

		if (bFormatNotice)
			GetServerResource(IDP_ANNOUNCEMENT, &buffer, msg);
		else
			buffer = msg;

		ChatPacket::Construct(&result, (uint8) chatType, &buffer);
		Send_Zone(&result, ZoneID, nullptr, byNation);
	}

	template <ChatType chatType>
	INLINE void SendFormattedChat(const char * msg, uint8 byNation = Nation::ALL, bool bFormatNotice = false, va_list args = nullptr)
	{
		char buffer[512];
		vsnprintf(buffer, sizeof(buffer), msg, args);
		SendChat<chatType>(buffer, byNation, bFormatNotice);
		va_end(args);
	}

	template <ChatType chatType>
	void SendFormattedChat(const char * msg, uint8 byNation = Nation::ALL, bool bFormatNotice = false, ...)
	{
		va_list ap;
		va_start(ap, byNation);
		SendFormattedChat<chatType>(msg, byNation, bFormatNotice, ap);
		va_end(ap);
	}

	/* The following are simply wrappers for more readable SendChat() calls */

	INLINE void SendNotice(const char *msg, uint8 byNation = Nation::ALL) 
	{
		SendChat<PUBLIC_CHAT>(msg, byNation, true);
	}

	template <ChatType chatType>
<<<<<<< HEAD
	INLINE void SendNotice(const char *msg, uint8 ZoneID, uint8 byNation = Nation::ALL) 
	{
		SendChatToZone<chatType>(msg, ZoneID, byNation, false);
=======
	INLINE void SendNotice(const char *msg, uint8 ZoneID, uint8 byNation = Nation::ALL, bool bFormatNotice = false) 
	{
		SendChatToZone<chatType>(msg, ZoneID, byNation, bFormatNotice);
>>>>>>> koserver2
	}

	void SendFormattedNotice(const char *msg, uint8 byNation = Nation::ALL, ...)
	{
		va_list ap;
		va_start(ap, byNation);
		SendFormattedChat<PUBLIC_CHAT>(msg, byNation, true, ap);
		va_end(ap);
	}

	INLINE void SendAnnouncement(const char *msg, uint8 byNation = Nation::ALL)
	{
		SendChat<WAR_SYSTEM_CHAT>(msg, byNation, true);
	}

	void SendFormattedAnnouncement(const char *msg, uint8 byNation = Nation::ALL, ...)
	{
		va_list ap;
		va_start(ap, byNation);
		SendFormattedChat<WAR_SYSTEM_CHAT>(msg, byNation, true, ap);
		va_end(ap);
	}

	void SendFormattedResource(uint32 nResourceID, uint8 byNation = Nation::ALL, bool bIsNotice = true, ...);

<<<<<<< HEAD
	void Send_Region(Packet *pkt, C3DMap *pMap, int x, int z, CUser* pExceptUser = nullptr, int16 nEventRoom = 0);
	void Send_UnitRegion(Packet *pkt, C3DMap *pMap, int x, int z, CUser* pExceptUser = nullptr, int16 nEventRoom = 0);
	void Send_OldRegions(Packet *pkt, int old_x, int old_z, C3DMap *pMap, int x, int z, CUser* pExceptUser = nullptr, int16 nEventRoom = 0);
	void Send_NewRegions(Packet *pkt, int new_x, int new_z, C3DMap *pMap, int x, int z, CUser* pExceptUser = nullptr, int16 nEventRoom = 0);

	void Send_NearRegion(Packet *pkt, C3DMap *pMap, int region_x, int region_z, float curx, float curz, CUser* pExceptUser=nullptr, int16 nEventRoom = 0);
	void Send_FilterUnitRegion(Packet *pkt, C3DMap *pMap, int x, int z, float ref_x, float ref_z, CUser* pExceptUser=nullptr, int16 nEventRoom = 0);

	void Send_Zone_Matched_Class(Packet *pkt, uint8 bZoneID, CUser* pExceptUser, uint8 nation, uint8 seekingPartyOptions, int16 nEventRoom = 0);
	void Send_Zone(Packet *pkt, uint8 bZoneID, CUser* pExceptUser = nullptr, uint8 nation = 0, int16 nEventRoom = 0);

	void Send_All(Packet *pkt, CUser* pExceptUser = nullptr, uint8 nation = 0, uint8 ZoneID = 0, int16 nEventRoom = 0);
=======
	void Send_Region(Packet *pkt, C3DMap *pMap, int x, int z, CUser* pExceptUser = nullptr, uint16 nEventRoom = 0);
	void Send_UnitRegion(Packet *pkt, C3DMap *pMap, int x, int z, CUser* pExceptUser = nullptr, uint16 nEventRoom = 0);
	void Send_OldRegions(Packet *pkt, int old_x, int old_z, C3DMap *pMap, int x, int z, CUser* pExceptUser = nullptr, uint16 nEventRoom = 0);
	void Send_NewRegions(Packet *pkt, int new_x, int new_z, C3DMap *pMap, int x, int z, CUser* pExceptUser = nullptr, uint16 nEventRoom = 0);

	void Send_NearRegion(Packet *pkt, C3DMap *pMap, int region_x, int region_z, float curx, float curz, CUser* pExceptUser=nullptr, uint16 nEventRoom = 0);
	void Send_FilterUnitRegion(Packet *pkt, C3DMap *pMap, int x, int z, float ref_x, float ref_z, CUser* pExceptUser=nullptr, uint16 nEventRoom = 0);

	void Send_Zone_Matched_Class(Packet *pkt, uint8 bZoneID, CUser* pExceptUser, uint8 nation, uint8 seekingPartyOptions, uint16 nEventRoom = 0);
	void Send_Zone(Packet *pkt, uint8 bZoneID, CUser* pExceptUser = nullptr, uint8 nation = 0, uint16 nEventRoom = 0, float fRange = 0.0f);

	void Send_All(Packet *pkt, CUser* pExceptUser = nullptr, uint8 nation = 0, uint8 ZoneID = 0, bool isSendEventUsers = false, uint16 nEventRoom = 0);
>>>>>>> koserver2
	void Send_AIServer(Packet *pkt);

	void GetServerResource(int nResourceID, std::string * result, ...);
	_START_POSITION *GetStartPosition(int nZoneID);

	int64 GetExpByLevel(int nLevel);
	C3DMap * GetZoneByID(int zoneID);

	CUser * GetUserPtr(std::string findName, NameType type);
	CUser * GetUserPtr(uint16 sUserId);
	CNpc  * GetNpcPtr(uint16 sNpcId);

	Unit * GetUnitPtr(uint16 id);

	// Spawns an event NPC/monster
<<<<<<< HEAD
	void SpawnEventNpc(uint16 sSid, bool bIsMonster, uint8 byZone, float fX, float fY, float fZ, uint16 sCount = 1, uint16 sRadius = 0, int16 nEventRoom = 0);
=======
	void SpawnEventNpc(uint16 sSid, bool bIsMonster, uint8 byZone, float fX, float fY, float fZ, uint16 sCount = 1, uint16 sRadius = 0, uint16 sDuration = 0, uint8 nation = 0, int16 socketID = -1, uint16 nEventRoom = 0);

	// Kill a Npc/Monster
	void KillNpc(uint16 sNid);
>>>>>>> koserver2

	// Change NPC/Monster properties for Respawn
	void NpcUpdate(uint16 sSid, bool bIsMonster, uint8 byGroup = 0, uint16 sPid = 0);	

	// Adds the account name & session to a hashmap (on login)
	void AddAccountName(CUser *pSession);

	// Adds the character name & session to a hashmap (when in-game)
	void AddCharacterName(CUser *pSession);

	// Removes an existing character name/session from the hashmap, replaces the character's name 
	// and reinserts the session with the new name into the hashmap.
	void ReplaceCharacterName(CUser *pSession, std::string & strNewUserID);

	// Removes the account name & character names from the hashmaps (on logout)
	void RemoveSessionNames(CUser *pSession);

<<<<<<< HEAD
=======
	// Send to Zone NPC Effect
	void ShowNpcEffect(uint16 sNpcID, uint32 nEffectID, uint8 ZoneID);

>>>>>>> koserver2
	_PARTY_GROUP * GetPartyPtr(uint16 sPartyID);
	CKnights * GetClanPtr(uint16 sClanID);
	_KNIGHTS_ALLIANCE * GetAlliancePtr(uint16 sAllianceID);
	_ITEM_TABLE * GetItemPtr(uint32 nItemID);

	_PARTY_GROUP * CreateParty(CUser *pLeader);
	void DeleteParty(uint16 sIndex);

	_EVENT_STATUS pTempleEvent;

	~CGameServerDlg();

	char	m_ppNotice[20][128];
	std::string	m_AIServerIP;
<<<<<<< HEAD

	NpcArray					m_arNpcArray;
	ZoneArray					m_ZoneArray;
	ItemtableArray				m_ItemtableArray;
	SetItemArray				m_SetItemArray;
	MagictableArray				m_MagictableArray;
	Magictype1Array				m_Magictype1Array;
	Magictype2Array				m_Magictype2Array;
	Magictype3Array				m_Magictype3Array;
	Magictype4Array				m_Magictype4Array;
	Magictype5Array				m_Magictype5Array;
	Magictype6Array				m_Magictype6Array;
	Magictype7Array				m_Magictype7Array;
	Magictype8Array				m_Magictype8Array;
	Magictype9Array				m_Magictype9Array;
	CoefficientArray			m_CoefficientArray;
	LevelUpArray				m_LevelUpArray;
	PartyArray					m_PartyArray;
	KnightsArray				m_KnightsArray;
	KnightsRatingArray			m_KnightsRatingArray[2]; // one for both nations
	KnightsAllianceArray		m_KnightsAllianceArray;
	KnightsCapeArray			m_KnightsCapeArray;
	UserNameRankMap				m_UserPersonalRankMap;
	UserNameRankMap				m_UserKnightsRankMap;
	UserRankMap					m_playerRankings[2]; // one for both nations
	FastMutex					m_userRankingsLock;
	HomeArray					m_HomeArray;
	StartPositionArray			m_StartPositionArray;
	ServerResourceArray			m_ServerResourceArray;
	QuestHelperArray			m_QuestHelperArray;
	QuestNpcList				m_QuestNpcList;
	QuestMonsterArray			m_QuestMonsterArray;
	RentalItemArray				m_RentalItemArray;
	ItemExchangeArray			m_ItemExchangeArray;
	ItemUpgradeArray			m_ItemUpgradeArray;
	ItemOpArray					m_ItemOpArray;
	KingSystemArray				m_KingSystemArray;
	MonsterSummonListArray		m_MonsterSummonList;
	MonsterSummonListZoneArray	m_MonsterSummonListZoneArray;
	MonsterRespawnListArray		m_MonsterRespawnListArray;
	PremiumItemArray			m_PremiumItemArray;
	PremiumItemExpArray			m_PremiumItemExpArray;
	UserRankingArray			m_UserRankingArray[2];
	UserDailyOpMap				m_UserDailyOpMap;
	TempleEventUserArray		m_TempleEventUserArray;
	EventTriggerArray			m_EventTriggerArray;

	Atomic<uint16>				m_sPartyIndex;
	short	m_sZoneCount;							// AI Server ÀçÁ¢¼Ó½Ã »ç¿ë

	bool	m_bFirstServerFlag;		// ¼­¹ö°¡ Ã³À½½ÃÀÛÇÑ ÈÄ °ÔÀÓ¼­¹ö°¡ ºÙÀº °æ¿ì¿¡´Â 1, ºÙÁö ¾ÊÀº °æ¿ì 0
	bool	m_bServerCheckFlag;
	bool	m_bPointCheckFlag;		// AI¼­¹ö¿Í ÀçÁ¢Àü¿¡ NPCÆ÷ÀÎÅÍ ÂüÁ¶¸·±â (true:Æ÷ÀÎÅÍ ÂüÁ¶, false:Æ÷ÀÎÅÍ ÂüÁ¶ ¸øÇÔ)
	short   m_sErrorSocketCount;  // ÀÌ»ó¼ÒÄÏ °¨½Ã¿ë
=======
	uint32		m_AIServerPort;

	NpcArray							m_arNpcArray;
	ZoneArray							m_ZoneArray;
	ItemtableArray						m_ItemtableArray;
	SetItemArray						m_SetItemArray;
	MagictableArray						m_MagictableArray;
	Magictype1Array						m_Magictype1Array;
	Magictype2Array						m_Magictype2Array;
	Magictype3Array						m_Magictype3Array;
	Magictype4Array						m_Magictype4Array;
	Magictype5Array						m_Magictype5Array;
	Magictype6Array						m_Magictype6Array;
	Magictype7Array						m_Magictype7Array;
	Magictype8Array						m_Magictype8Array;
	Magictype9Array						m_Magictype9Array;
	CoefficientArray					m_CoefficientArray;
	LevelUpArray						m_LevelUpArray;
	PartyArray							m_PartyArray;
	KnightsArray						m_KnightsArray;
	KnightsRatingArray					m_KnightsRatingArray[2]; // one for both nations
	KnightsAllianceArray				m_KnightsAllianceArray;
	KnightsCapeArray					m_KnightsCapeArray;
	UserNameRankMap						m_UserPersonalRankMap;
	UserNameRankMap						m_UserKnightsRankMap;
	UserRankMap							m_playerRankings[2]; // one for both nations
	std::recursive_mutex				m_userRankingsLock;
	StartPositionArray					m_StartPositionArray;
	ServerResourceArray					m_ServerResourceArray;
	QuestHelperArray					m_QuestHelperArray;
	QuestNpcList						m_QuestNpcList;
	QuestMonsterArray					m_QuestMonsterArray;
	RentalItemArray						m_RentalItemArray;
	ItemExchangeArray					m_ItemExchangeArray;
	ItemUpgradeArray					m_ItemUpgradeArray;
	ItemOpArray							m_ItemOpArray;
	KingSystemArray						m_KingSystemArray;
	EventTriggerArray					m_EventTriggerArray;
	MonsterChallengeArray				m_MonsterChallengeArray;
	MonsterChallengeSummonListArray		m_MonsterChallengeSummonListArray;
	MonsterSummonListArray				m_MonsterSummonList;
	MonsterSummonListZoneArray			m_MonsterSummonListZoneArray;
	MonsterRespawnListArray				m_MonsterRespawnListArray;
	MonsterRespawnListInformationArray	m_MonsterRespawnListInformationArray;
	PremiumItemArray					m_PremiumItemArray;
	PremiumItemExpArray					m_PremiumItemExpArray;
	UserRankingArray					m_UserRankingArray[2];
	UserDailyOpMap						m_UserDailyOpMap;
	TempleEventUserArray				m_TempleEventUserArray;
	NationMonumentInformationArray		m_NationMonumentInformationArray;
	StartPositionRandomArray			m_StartPositionRandomArray;
	UserItemArray						m_UserItemArray;
	ObjectEventArray					m_ObjectEventArray;

	Atomic<uint16>				m_sPartyIndex;
	short	m_sZoneCount;							// AI Server ï¿½ï¿½ï¿½ï¿½ï¿½Ó½ï¿½ ï¿½ï¿½ï¿½ï¿½

	bool	m_bFirstServerFlag;		// ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ Ã³ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½Ó¼ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ì¿¡ï¿½ï¿½ 1, ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ 0
	bool	m_bServerCheckFlag;
	bool	m_bPointCheckFlag;		// AIï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ NPCï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ (true:ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½, false:ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½)
	short   m_sErrorSocketCount;  // ï¿½Ì»ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Ã¿ï¿½
>>>>>>> koserver2

	uint16 m_sYear, m_sMonth, m_sDate, m_sHour, m_sMin;
	uint8 m_byWeather;
	uint16 m_sWeatherAmount;
	int m_nCastleCapture;
<<<<<<< HEAD

	uint8   m_byBattleOpen, m_byOldBattleOpen;					// 0:ÀüÀïÁßÀÌ ¾Æ´Ô, 1:ÀüÀïÁß(±¹°¡°£ÀüÀï), 2:´«½Î¿òÀüÀï
	uint8	m_byBattleZone;
	uint8	m_bVictory, m_byOldVictory;
	uint8	m_bKarusFlag, m_bElmoradFlag;
	int32	m_byBattleOpenedTime;
	int32	m_byBattleTime;
	int32	m_sBattleTimeDelay;
	uint8	m_sKillKarusNpc, m_sKillElmoNpc;
	bool    m_byKarusOpenFlag, m_byElmoradOpenFlag, m_byBanishFlag, m_byBattleSave;
	short   m_sDiscount;	// ´É·ÂÄ¡¿Í Æ÷ÀÎÆ® ÃÊ±âÈ­ ÇÒÀÎ (0:ÇÒÀÎ¾øÀ½, 1:ÇÒÀÎ(50%) )
	short	m_sKarusDead, m_sElmoradDead, m_sBanishDelay, m_sKarusCount, m_sElmoradCount;
	std::string m_nBattleZoneOpenDays;
=======
	uint8 m_ReloadKnightAndUserRanksMinute;

	uint8   m_byBattleOpen, m_byOldBattleOpen;
	uint8	m_byBattleZone, m_byBattleZoneType, m_bVictory, m_byOldVictory, m_bResultDelayVictory, m_bKarusFlag, m_bElmoradFlag, m_bMiddleStatueNation;
	int32	m_byBattleOpenedTime;
	int32	m_byBattleTime;
	int32	m_byBattleRemainingTime;
	int32	m_sBattleTimeDelay;
	int32	m_sBattleResultDelay;
	uint8	m_sKilledKarusNpc, m_sKilledElmoNpc;
	uint8	m_sKarusMonuments, m_sElmoMonuments;
	uint16	m_sKarusMonumentPoint, m_sElmoMonumentPoint;
	bool    m_byKarusOpenFlag, m_byElmoradOpenFlag, m_byBanishFlag, m_byBattleSave, m_bResultDelay;
	short   m_sDiscount;
	short	m_sKarusDead, m_sElmoradDead, m_sBanishDelay, m_sKarusCount, m_sElmoradCount;
	std::string m_sBattleZoneOpenDays;
>>>>>>> koserver2
	uint8	m_nBattleZoneOpenHourStart[WAR_TIME_COUNT], m_nBattlezoneOpenWarZone[WAR_ZONE_COUNT];

	std::string m_strKarusCaptain, m_strElmoradCaptain;

	uint8   m_nBorderDefenseWarTime[BORDER_DEFENSE_WAR_EVENT_COUNT], m_nChaosTime[CHAOS_EVENT_COUNT];
	uint8	m_nPVPMonumentNation[MAX_ZONE_ID];
<<<<<<< HEAD
	int32	m_nGameMasterRHitDamage;

	int32   m_event_trigger_time;
	uint16	m_nTempleEventFinishRemainSeconds;
=======

	uint32	m_GameServerPort;
	int32	m_nGameMasterRHitDamage;
	uint8	m_nPlayerRankingResetTime;
	std::string	m_sPlayerRankingsRewardZones;
	uint32	m_nPlayerRankingKnightCashReward;
	uint32  m_nPlayerRankingLoyaltyReward;

	uint16	m_nTempleEventRemainSeconds;
>>>>>>> koserver2

	uint8	m_bMaxRegenePoint;

	bool	m_bPermanentChatMode;
	std::string	m_strPermanentChat;

	uint8	m_bSantaOrAngel;
	uint8	m_sRankResetHour;

<<<<<<< HEAD
	// Bifrost
	uint8   m_nBifrostTime[BIFROST_EVENT_COUNT];
	uint16	m_nTempleEventRemainSeconds;
=======
	// National Points Settings
	int m_Loyalty_Ardream_Source;
	int m_Loyalty_Ardream_Target;
	int m_Loyalty_Ronark_Land_Base_Source;
	int m_Loyalty_Ronark_Land_Base_Target;
	int m_Loyalty_Ronark_Land_Source;
	int m_Loyalty_Ronark_Land_Target;
	int m_Loyalty_Other_Zone_Source;
	int m_Loyalty_Other_Zone_Target;

	// Bifrost
	uint8   m_nBifrostTime[BIFROST_EVENT_COUNT];

>>>>>>> koserver2
	uint8 m_BifrostVictory;
	uint16 m_sBifrostRemainingTime;
	uint16 m_sBifrostTime;

	uint16 m_xBifrostRemainingTime;
	uint16 m_xJoinOtherNationBifrostTime;
	uint16 m_xBifrostTime;
	uint16 m_xBifrostMonumentAttackTime;
	bool m_bAttackBifrostMonument;

<<<<<<< HEAD
	void SendBifrostTime(CUser *pUser = nullptr, bool bSendAll = false);

	// Forgetten Temple
	std::vector<int16>		m_nForgettenTempleUsers;

	// zone server info
	int					m_nServerNo, m_nServerGroupNo;
	int					m_nServerGroup;	// serverÀÇ ¹øÈ£(0:¼­¹ö±ºÀÌ ¾ø´Ù, 1:¼­¹ö1±º, 2:¼­¹ö2±º)
=======
	void SendEventRemainingTime(bool bSendAll = false, CUser *pUser = nullptr, uint8 ZoneID = 0);

	bool m_IsMagicTableInUpdateProcess;
	bool m_IsPlayerRankingUpdateProcess;

	std::vector<int64> m_HardwareIDArray;

	// Forgetten Temple
	std::vector<uint16>				m_nForgettenTempleUsers;
	bool							m_bForgettenTempleIsActive;
	int8							m_nForgettenTempleStartHour;
	int8							m_nForgettenTempleLevelMin;
	int8							m_nForgettenTempleLevelMax;
	int32							m_nForgettenTempleStartTime;
	uint8							m_nForgettenTempleChallengeTime;
	bool							m_bForgettenTempleSummonMonsters;
	uint8							m_nForgettenTempleCurrentStage;
	uint8							m_nForgettenTempleLastStage;
	ForgettenTempleMonsterList		m_ForgettenTempleMonsterList;
	uint32							m_nForgettenTempleLastSummonTime;
	bool							m_nForgettenTempleBanishFlag;
	uint8							m_nForgettenTempleBanishTime;

	// zone server info
	int					m_nServerNo, m_nServerGroupNo;
	int					m_nServerGroup;	// serverï¿½ï¿½ ï¿½ï¿½È£(0:ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½, 1:ï¿½ï¿½ï¿½ï¿½1ï¿½ï¿½, 2:ï¿½ï¿½ï¿½ï¿½2ï¿½ï¿½)
>>>>>>> koserver2
	ServerArray			m_ServerArray;
	ServerArray			m_ServerGroupArray;

	NameMap		m_accountNameMap,
		m_characterNameMap;

<<<<<<< HEAD
	FastMutex	m_accountNameLock,
=======
	std::recursive_mutex	m_accountNameLock,
>>>>>>> koserver2
		m_characterNameLock,
		m_questNpcLock;

	// Controlled weather events set by Kings
	uint8 m_byKingWeatherEvent;
	uint8 m_byKingWeatherEvent_Day;
	uint8 m_byKingWeatherEvent_Hour;
	uint8 m_byKingWeatherEvent_Minute;

<<<<<<< HEAD
	// XP/coin events
	uint8 m_byExpEventAmount, m_byCoinEventAmount;
=======
	// XP/coin/NP events
	uint8 m_byNPEventAmount, m_byExpEventAmount, m_byCoinEventAmount;
>>>>>>> koserver2

	INLINE CLuaEngine * GetLuaEngine() { return &m_luaEngine; }

	KOSocketMgr<CUser> m_socketMgr;
	ClientSocketMgr<CAISocket> m_aiSocketMgr;

<<<<<<< HEAD
=======
	FILE *m_fpDeathUser;
	FILE *m_fpDeathNpc;
	FILE *m_fpChat;
	FILE *m_fpCheat;

	void WriteDeathUserLogFile(std::string & logMessage);
	void WriteDeathNpcLogFile(std::string & logMessage);
	void WriteChatLogFile(std::string & logMessage);
	void WriteCheatLogFile(std::string & logMessage);

>>>>>>> koserver2
private:
	CLuaEngine	m_luaEngine;

	std::string m_strGameDSN, m_strAccountDSN;
	std::string m_strGameUID, m_strAccountUID;
	std::string m_strGamePWD, m_strAccountPWD;
	bool m_bMarsEnabled;

	bool ProcessServerCommand(std::string & command);

public:
	void InitServerCommands();
	void CleanupServerCommands();

	static ServerCommandTable s_commandTable;

	COMMAND_HANDLER(HandleNoticeCommand);
	COMMAND_HANDLER(HandleKillUserCommand);
	COMMAND_HANDLER(HandleWar1OpenCommand);
	COMMAND_HANDLER(HandleWar2OpenCommand);
	COMMAND_HANDLER(HandleWar3OpenCommand);
	COMMAND_HANDLER(HandleWar4OpenCommand);
	COMMAND_HANDLER(HandleWar5OpenCommand);
	COMMAND_HANDLER(HandleWar6OpenCommand);
	COMMAND_HANDLER(HandleSnowWarOpenCommand);
	COMMAND_HANDLER(HandleWarCloseCommand);
	COMMAND_HANDLER(HandleShutdownCommand);
<<<<<<< HEAD
	COMMAND_HANDLER(HandlePauseCommand);
	COMMAND_HANDLER(HandleResumeCommand);
=======
>>>>>>> koserver2
	COMMAND_HANDLER(HandleDiscountCommand);
	COMMAND_HANDLER(HandleGlobalDiscountCommand);
	COMMAND_HANDLER(HandleDiscountOffCommand);
	COMMAND_HANDLER(HandleCaptainCommand);
	COMMAND_HANDLER(HandleSantaCommand);
	COMMAND_HANDLER(HandleSantaOffCommand);
	COMMAND_HANDLER(HandleAngelCommand);
	COMMAND_HANDLER(HandlePermanentChatCommand);
	COMMAND_HANDLER(HandlePermanentChatOffCommand);
	COMMAND_HANDLER(HandleReloadNoticeCommand);
	COMMAND_HANDLER(HandleReloadTablesCommand);
<<<<<<< HEAD
	COMMAND_HANDLER(HandleCountCommand);
	COMMAND_HANDLER(HandlePermitConnectCommand);
=======
	COMMAND_HANDLER(HandleReloadMagicsCommand);
	COMMAND_HANDLER(HandleReloadQuestCommand);
	COMMAND_HANDLER(HandleReloadRanksCommand);
	COMMAND_HANDLER(HandleCountCommand);
	COMMAND_HANDLER(HandlePermitConnectCommand);
	COMMAND_HANDLER(HandleWarResultCommand);
>>>>>>> koserver2
};

extern CGameServerDlg * g_pMain;

<<<<<<< HEAD
#endif
=======
#endif
>>>>>>> koserver2
