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

typedef std::unordered_map<std::string, CUser *> NameMap;

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
	bool LoadAllKnights();
	bool LoadAllKnightsUserData();
	bool LoadKnightsAllianceTable();
	bool LoadUserRankings();
	void CleanupUserRankings();
	bool LoadKnightsCapeTable();
	bool LoadKnightsRankTable(bool bWarTime = false);
	bool LoadHomeTable();
	bool LoadStartPositionTable();
	bool LoadBattleTable();
	bool LoadKingSystem();
	bool LoadMonsterSummonListTable();
	bool LoadMonsterSummonListZoneTable();
	bool LoadMonsterRespawnListTable();
	bool LoadPremiumItemTable();
	bool LoadPremiumItemExpTable();
	bool LoadUserDailyOpTable();
	bool LoadEventTriggerTable();

	bool MapFileLoad();
	bool LoadNoticeData();

	void AIServerConnect();

	static uint32 THREADCALL Timer_TempleEventTimer(void * lpParam);
	static uint32 THREADCALL Timer_BifrostTime(void * lpParam);
	static uint32 THREADCALL Timer_UpdateGameTime(void * lpParam);
	static uint32 THREADCALL Timer_UpdateSessions(void * lpParam);
	static uint32 THREADCALL Timer_UpdateConcurrent(void * lpParam);
	static uint32 THREADCALL Timer_ReloadUserAndKnightsRating(void * lpParam);

	void ReqUpdateConcurrent();

	void SendFlyingSantaOrAngel();
	void BattleZoneCurrentUsers();
	void GetCaptainUserPtr();
	void Send_CommandChat(Packet *pkt, int nation, CUser* pExceptUser = nullptr);
	void KickOutZoneUsers(uint8 ZoneID, uint8 TargetZoneID = 0);
	void SendItemZoneUsers(uint8 ZoneID, uint32 nItemID, uint16 sCount = 1);
	uint64 GenerateItemSerial();
	int KickOutAllUsers();
	void CheckAliveUser();
	int GetKnightsGrade(uint32 nPoints);
	void WritePacketLog();
	uint16 GetKnightsAllMembers(uint16 sClanID, Packet & result, uint16 & pktSize, bool bClanLeader);
	void GetUserRank(CUser *pUser);
	void Announcement(uint8 type, int nation=0, int chat_type=8, CUser* pExceptUser = nullptr);
	void ResetBattleZone();
	void BanishLosers();
	void BattleZoneVictoryCheck();
	void BattleZoneOpenTimer();
	void BattleZoneOpen(int nType, uint8 bZone = 0);
	void BattleZoneClose();
	void BattleZoneResult(uint8 nation);
	void AliveUserCheck();
	void Send_PartyMember(int party, Packet *result);
	void Send_KnightsMember(int index, Packet *pkt);
	void Send_KnightsAlliance(uint16 sAllianceID, Packet *pkt);
	void SetGameTime();
	void ResetPlayerRankings();
	void UpdateWeather();
	void UpdateGameTime();
	void ResetLoyaltyMonthly();
	void SendAllUserInfo();
	void DeleteAllNpcList(int flag = 0);
	CNpc*  FindNpcInZone(uint16 sPid, uint8 byZone);
	void TempleEventTimer();
	void TempleEventStart();
	void TempleEventCreateRooms();
	void TempleEventTeleportUsers();
	void TempleEventFinish();
	void TempleEventGetActiveEventTime(CUser *pUser);
	void TempleEventSendActiveEventTime(CUser *pUser);
	void TempleEventKickOutUser(CUser *pUser);

	void AddDatabaseRequest(Packet & pkt, CUser *pUser = nullptr);

	// Get info for NPCs in regions around user (WIZ_REQ_NPCIN)
	void NpcInOutForMe(CUser* pSendUser);

	// Get info for NPCs in region
	void GetRegionNpcIn(C3DMap* pMap, uint16 region_x, uint16 region_z, Packet & pkt, uint16 & t_count, int16 nEventRoom = 0);

	// Get list of NPC IDs in region
	void GetRegionNpcList(C3DMap* pMap, uint16 region_x, uint16 region_z, Packet & pkt, uint16 & t_count, int16 nEventRoom = 0);

	// Get list of NPCs for regions around user (WIZ_NPC_REGION)
	void RegionNpcInfoForMe(CUser* pSendUser);	

	// Get raw list of all units in regions surrounding pOwner.
	void GetUnitListFromSurroundingRegions(Unit * pOwner, std::vector<uint16> * pList);

	// Get info for users in regions around user (WIZ_REQ_USERIN)
	void UserInOutForMe(CUser* pSendUser);

	// Get info for users in region
	void GetRegionUserIn(C3DMap* pMap, uint16 region_x, uint16 region_z, Packet & pkt, uint16 & t_count, int16 nEventRoom = 0);

	// Get list of user IDs in region
	void GetRegionUserList(C3DMap* pMap, uint16 region_x, uint16 region_z, Packet & pkt, uint16 & t_count, int16 nEventRoom = 0);

	// Get list of users for regions around user (WIZ_REGIONCHANGE)
	void RegionUserInOutForMe(CUser* pSendUser);

	// Get info for merchants in regions around user
	void MerchantUserInOutForMe(CUser* pSendUser);

	// Get war status
	INLINE bool isWarOpen() { return m_byBattleOpen != NO_BATTLE;} 

	// Get list of merchants in region
	void GetRegionMerchantUserIn(C3DMap* pMap, uint16 region_x, uint16 region_z, Packet & pkt, uint16 & t_count, int16 nEventRoom = 0);

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
	INLINE void SendNotice(const char *msg, uint8 ZoneID, uint8 byNation = Nation::ALL) 
	{
		SendChatToZone<chatType>(msg, ZoneID, byNation, false);
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

	void Send_Region(Packet *pkt, C3DMap *pMap, int x, int z, CUser* pExceptUser = nullptr, int16 nEventRoom = 0);
	void Send_UnitRegion(Packet *pkt, C3DMap *pMap, int x, int z, CUser* pExceptUser = nullptr, int16 nEventRoom = 0);
	void Send_OldRegions(Packet *pkt, int old_x, int old_z, C3DMap *pMap, int x, int z, CUser* pExceptUser = nullptr, int16 nEventRoom = 0);
	void Send_NewRegions(Packet *pkt, int new_x, int new_z, C3DMap *pMap, int x, int z, CUser* pExceptUser = nullptr, int16 nEventRoom = 0);

	void Send_NearRegion(Packet *pkt, C3DMap *pMap, int region_x, int region_z, float curx, float curz, CUser* pExceptUser=nullptr, int16 nEventRoom = 0);
	void Send_FilterUnitRegion(Packet *pkt, C3DMap *pMap, int x, int z, float ref_x, float ref_z, CUser* pExceptUser=nullptr, int16 nEventRoom = 0);

	void Send_Zone_Matched_Class(Packet *pkt, uint8 bZoneID, CUser* pExceptUser, uint8 nation, uint8 seekingPartyOptions, int16 nEventRoom = 0);
	void Send_Zone(Packet *pkt, uint8 bZoneID, CUser* pExceptUser = nullptr, uint8 nation = 0, int16 nEventRoom = 0);

	void Send_All(Packet *pkt, CUser* pExceptUser = nullptr, uint8 nation = 0, uint8 ZoneID = 0, int16 nEventRoom = 0);
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
	void SpawnEventNpc(uint16 sSid, bool bIsMonster, uint8 byZone, float fX, float fY, float fZ, uint16 sCount = 1, uint16 sRadius = 0, int16 nEventRoom = 0);

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
	short	m_sZoneCount;							// AI Server 재접속시 사용

	bool	m_bFirstServerFlag;		// 서버가 처음시작한 후 게임서버가 붙은 경우에는 1, 붙지 않은 경우 0
	bool	m_bServerCheckFlag;
	bool	m_bPointCheckFlag;		// AI서버와 재접전에 NPC포인터 참조막기 (true:포인터 참조, false:포인터 참조 못함)
	short   m_sErrorSocketCount;  // 이상소켓 감시용

	uint16 m_sYear, m_sMonth, m_sDate, m_sHour, m_sMin;
	uint8 m_byWeather;
	uint16 m_sWeatherAmount;
	int m_nCastleCapture;

	uint8   m_byBattleOpen, m_byOldBattleOpen;					// 0:전쟁중이 아님, 1:전쟁중(국가간전쟁), 2:눈싸움전쟁
	uint8	m_byBattleZone;
	uint8	m_bVictory, m_byOldVictory;
	uint8	m_bKarusFlag, m_bElmoradFlag;
	int32	m_byBattleOpenedTime;
	int32	m_byBattleTime;
	int32	m_sBattleTimeDelay;
	uint8	m_sKillKarusNpc, m_sKillElmoNpc;
	bool    m_byKarusOpenFlag, m_byElmoradOpenFlag, m_byBanishFlag, m_byBattleSave;
	short   m_sDiscount;	// 능력치와 포인트 초기화 할인 (0:할인없음, 1:할인(50%) )
	short	m_sKarusDead, m_sElmoradDead, m_sBanishDelay, m_sKarusCount, m_sElmoradCount;
	std::string m_nBattleZoneOpenDays;
	uint8	m_nBattleZoneOpenHourStart[WAR_TIME_COUNT], m_nBattlezoneOpenWarZone[WAR_ZONE_COUNT];

	std::string m_strKarusCaptain, m_strElmoradCaptain;

	uint8   m_nBorderDefenseWarTime[BORDER_DEFENSE_WAR_EVENT_COUNT], m_nChaosTime[CHAOS_EVENT_COUNT];
	uint8	m_nPVPMonumentNation[MAX_ZONE_ID];
	int32	m_nGameMasterRHitDamage;

	uint16	m_nTempleEventRemainSeconds;
	uint16	m_nTempleEventFinishRemainSeconds;

	uint8	m_bMaxRegenePoint;

	bool	m_bPermanentChatMode;
	std::string	m_strPermanentChat;

	uint8	m_bSantaOrAngel;
	uint8	m_sRankResetHour;

	// Bifrost
	uint8   m_nBifrostTime[BIFROST_EVENT_COUNT];

	uint8 m_BifrostVictory;
	uint16 m_sBifrostRemainingTime;
	uint16 m_sBifrostTime;

	uint16 m_xBifrostRemainingTime;
	uint16 m_xJoinOtherNationBifrostTime;
	uint16 m_xBifrostTime;
	uint16 m_xBifrostMonumentAttackTime;
	bool m_bAttackBifrostMonument;

	void SendBifrostTime(CUser *pUser = nullptr, bool bSendAll = false);

	// Forgetten Temple
	std::vector<int16>		m_nForgettenTempleUsers;

	// zone server info
	int					m_nServerNo, m_nServerGroupNo;
	int					m_nServerGroup;	// server의 번호(0:서버군이 없다, 1:서버1군, 2:서버2군)
	ServerArray			m_ServerArray;
	ServerArray			m_ServerGroupArray;

	NameMap		m_accountNameMap,
		m_characterNameMap;

	FastMutex	m_accountNameLock,
		m_characterNameLock,
		m_questNpcLock;

	// Controlled weather events set by Kings
	uint8 m_byKingWeatherEvent;
	uint8 m_byKingWeatherEvent_Day;
	uint8 m_byKingWeatherEvent_Hour;
	uint8 m_byKingWeatherEvent_Minute;

	// XP/coin events
	uint8 m_byExpEventAmount, m_byCoinEventAmount;

	INLINE CLuaEngine * GetLuaEngine() { return &m_luaEngine; }

	KOSocketMgr<CUser> m_socketMgr;
	ClientSocketMgr<CAISocket> m_aiSocketMgr;

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
	COMMAND_HANDLER(HandlePauseCommand);
	COMMAND_HANDLER(HandleResumeCommand);
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
	COMMAND_HANDLER(HandleCountCommand);
	COMMAND_HANDLER(HandlePermitConnectCommand);
};

extern CGameServerDlg * g_pMain;

#endif