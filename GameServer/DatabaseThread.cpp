﻿#include "stdafx.h"
#include "../shared/Condition.h"
#include "KnightsManager.h"
#include "KingSystem.h"
#include "DBAgent.h"

extern CDBAgent g_DBAgent;

using std::string;

static std::queue<Packet *> _queue;
static bool _running = true;
static std::recursive_mutex _lock;

static Condition s_hEvent;
static Thread * s_thread;

void DatabaseThread::Startup()
{
	s_thread = new Thread(ThreadProc, (void *)1);
}

void DatabaseThread::AddRequest(Packet * pkt)
{
	_lock.lock();
	_queue.push(pkt);
	_lock.unlock();
	s_hEvent.Signal();
}

uint32 THREADCALL DatabaseThread::ThreadProc(void * lpParam)
{
	while (true)
	{
		Packet *p = nullptr;

		// Pull the next packet from the shared queue
		_lock.lock();
		if (_queue.size())
		{
			p = _queue.front();
			_queue.pop();
		}
		_lock.unlock();

		// If there's no more packets to handle, wait until there are.
		if (p == nullptr)
		{
			// If we're shutting down, don't bother waiting for more (there are no more).
			if (!_running)
				break;

			s_hEvent.Wait();
			continue;
		}

		// References are fun =p
		Packet & pkt = *p;

		// First 2 bytes are always going to be the socket ID
		// or -1 for no user.
		int16 uid = pkt.read<int16>();

		// Attempt to lookup the user if necessary
		CUser *pUser = nullptr;
		if (uid >= 0)
		{
			pUser = g_pMain->GetUserPtr(uid);

			// Check to make sure they're still connected.
			if (pUser == nullptr)
				continue;
		}

		uint8 subOpcode;
		switch (pkt.GetOpcode())
		{
		case WIZ_LOGIN:
			if (pUser) pUser->ReqAccountLogIn(pkt);
			break;
		case WIZ_SEL_NATION:
			if (pUser) pUser->ReqSelectNation(pkt);
			break;
		case WIZ_ALLCHAR_INFO_REQ:
			if (pUser) pUser->ReqAllCharInfo(pkt);
			break;
		case WIZ_CHANGE_HAIR:
			if (pUser) pUser->ReqChangeHair(pkt);
			break;
		case WIZ_NEW_CHAR:
			if (pUser) pUser->ReqCreateNewChar(pkt);
			break;
		case WIZ_DEL_CHAR:
			if (pUser) pUser->ReqDeleteChar(pkt);
			break;
		case WIZ_SEL_CHAR:
			if (pUser) pUser->ReqSelectCharacter(pkt);
			break;
		case WIZ_CHAT:
			pkt >> subOpcode;
			if (subOpcode == CLAN_NOTICE)
				CKnightsManager::ReqUpdateClanNotice(pkt);
			break;
		case WIZ_DATASAVE:
			if (pUser) pUser->ReqSaveCharacter();
			break;
		case WIZ_KNIGHTS_PROCESS:
			CKnightsManager::ReqKnightsPacket(pUser, pkt);
			break;
		case WIZ_LOGIN_INFO:
			if (pUser) pUser->ReqSetLogInInfo(pkt);
			break;
		case WIZ_BATTLE_EVENT:
			// g_pMain->BattleEventResult(pkt);
			break;
		case WIZ_SHOPPING_MALL:
			if (pUser) pUser->ReqShoppingMall(pkt);
			break;
		case WIZ_SKILLDATA:
			if (pUser) pUser->ReqSkillDataProcess(pkt);
			break;
		case WIZ_FRIEND_PROCESS:
			if (pUser) pUser->ReqFriendProcess(pkt);
			break;
		case WIZ_NAME_CHANGE:
			if (pUser) pUser->ReqChangeName(pkt);
			break;
		case WIZ_CAPE:
			if (pUser) pUser->ReqChangeCape(pkt);
			break;
		case WIZ_LOGOUT:
			if (pUser) pUser->ReqUserLogOut();
			break;
		case WIZ_KING:
			CKingSystem::HandleDatabaseRequest(pUser, pkt);
			break;
		case WIZ_ITEM_UPGRADE:
			if (pUser) pUser->ReqSealItem(pkt);
			break;
		case WIZ_ZONE_CONCURRENT:
			{
				uint32 serverNo, count;
				pkt >> serverNo >> count;
				g_DBAgent.UpdateConCurrentUserCount(serverNo, 1, count);
			} break;
		}

		// Free the packet.
		delete p;
	}

	TRACE("[Thread %d] Exiting...\n", lpParam);
	return 0;
}


void CUser::ReqAccountLogIn(Packet & pkt)
{
	string strPasswd;
	pkt >> strPasswd;

	int8 nation = g_DBAgent.AccountLogin(m_strAccountID, strPasswd);

	if (nation >= 0)
		g_pMain->AddAccountName(this);
	else
		m_strAccountID.clear();

	Packet result(WIZ_LOGIN);
	result << nation;
	Send(&result);
}

void CUser::ReqSelectNation(Packet & pkt)
{
	Packet result(WIZ_SEL_NATION);
	uint8 bNation = pkt.read<uint8>(), bResult;

	bResult = g_DBAgent.NationSelect(m_strAccountID, bNation) ? bNation : 0;
	result << bResult;
	Send(&result);
}

void CUser::ReqAllCharInfo(Packet & pkt)
{
	Packet result(WIZ_ALLCHAR_INFO_REQ);
	string strCharID1, strCharID2, strCharID3;

	result << uint8(1);
#if __VERSION >= 1920
	result << uint8(1); // 1.920+ flag, probably indicates whether there's any characters or not (stays 1 for 1+ characters though, so not a count :'(). Untested without.
#endif
	g_DBAgent.GetAllCharID(m_strAccountID, strCharID1, strCharID2, strCharID3);
	g_DBAgent.LoadCharInfo(strCharID1, result);
	g_DBAgent.LoadCharInfo(strCharID2, result);
	g_DBAgent.LoadCharInfo(strCharID3, result);

	Send(&result);
}

void CUser::ReqChangeHair(Packet & pkt)
{
	Packet result(WIZ_CHANGE_HAIR);
	string strUserID;
	uint32 nHair;
	uint8 bOpcode, bFace;
	pkt.SByte();
	pkt >> bOpcode >> strUserID >> bFace >> nHair;
	pkt.put(2, g_DBAgent.ChangeHair(m_strAccountID, strUserID, bOpcode, bFace, nHair));
	Send(&result);
}

void CUser::ReqCreateNewChar(Packet & pkt)
{
	string strCharID;
	uint32 nHair;
	uint16 sClass;
	uint8 bCharIndex, bRace, bFace, bStr, bSta, bDex, bInt, bCha;
	pkt >> bCharIndex >> strCharID >> bRace >> sClass >> bFace >> nHair >> bStr >> bSta >> bDex >> bInt >> bCha;

	Packet result(WIZ_NEW_CHAR);
	result << g_DBAgent.CreateNewChar(m_strAccountID, bCharIndex, strCharID, bRace, sClass, nHair, bFace, bStr, bSta, bDex, bInt, bCha);

	Send(&result);
}

void CUser::ReqDeleteChar(Packet & pkt)
{
	string strCharID, strSocNo;
	uint8 bCharIndex;
	pkt >> bCharIndex >> strCharID >> strSocNo;

	Packet result(WIZ_DEL_CHAR);
	int8 retCode = g_DBAgent.DeleteChar(m_strAccountID, bCharIndex, strCharID, strSocNo);
	result << retCode << uint8(retCode ? bCharIndex : -1);
	Send(&result);

#if 0
	if (retCode == 1 && sKnights != 0)
	{
		// TODO: Synchronise this system better. Much better. This is dumb.
		CKnightsManager::RemoveKnightsUser(sKnights, (char *)strCharID.c_str());
	}
#endif
}

void CUser::ReqSelectCharacter(Packet & pkt)
{
	Packet result(WIZ_SEL_CHAR);
	uint8 bInit;
	string strCharID;

	pkt >> strCharID >> bInit;
	if (m_strAccountID.empty() || strCharID.empty()
		|| !g_DBAgent.LoadUserData(m_strAccountID, strCharID, this)
		|| !g_DBAgent.LoadWarehouseData(m_strAccountID, this)
		|| !g_DBAgent.LoadPremiumServiceUser(m_strAccountID, this)
		|| !g_DBAgent.LoadSavedMagic(this))
	{
		result << uint8(0);
	}
	else
	{
		result << uint8(1) << bInit;
	}

	SelectCharacter(result); 
}

void CUser::ReqShoppingMall(Packet & pkt)
{
	switch (pkt.read<uint8>())
	{
	case STORE_CLOSE:
		ReqLoadWebItemMall();
		break;
	case STORE_LETTER:
		ReqLetterSystem(pkt);
		break;
	}
}

void CUser::ReqSkillDataProcess(Packet & pkt)
{
	uint8 opcode = pkt.read<uint8>();
	if (opcode == SKILL_DATA_LOAD)
		ReqSkillDataLoad(pkt);
	else if (opcode == SKILL_DATA_SAVE)
		ReqSkillDataSave(pkt);
}

void CUser::ReqSkillDataLoad(Packet & pkt)
{
	Packet result(WIZ_SKILLDATA, uint8(SKILL_DATA_LOAD));
	if (!g_DBAgent.LoadSkillShortcut(result, this))
		result << uint16(0);

	Send(&result);
}

void CUser::ReqSkillDataSave(Packet & pkt)
{
	// Initialize our buffer (not all skills are likely to be saved, we need to store the entire 260 bytes).
	char buff[260] = {0};
	short sCount;

	// Read in our skill count
	pkt >> sCount;

	// Make sure we're not going to copy too much (each skill is 1 uint32).
	if ((sCount * sizeof(uint32)) > sizeof(buff))
		return;

	// Copy the skill data directly in from where we left off reading in the packet buffer
	memcpy(buff, (char *)(pkt.contents() + pkt.rpos()), sCount * sizeof(uint32));

	// Finally, save the skill data.
	g_DBAgent.SaveSkillShortcut(sCount, buff, this);
}

void CUser::ReqFriendProcess(Packet & pkt)
{
	switch (pkt.read<uint8>())
	{
	case FRIEND_REQUEST:
		ReqRequestFriendList(pkt);
		break;

	case FRIEND_ADD:
		ReqAddFriend(pkt);
		break;

	case FRIEND_REMOVE:
		ReqRemoveFriend(pkt);
		break;
	}
}

void CUser::ReqRequestFriendList(Packet & pkt)
{
	Packet result(WIZ_FRIEND_PROCESS);
	std::vector<string> friendList;

	g_DBAgent.RequestFriendList(friendList, this);

	result << uint16(friendList.size());
	foreach (itr, friendList)
		result << (*itr);

	FriendReport(result);
}

void CUser::ReqAddFriend(Packet & pkt)
{
	Packet result(WIZ_FRIEND_PROCESS);
	string strCharID;
	int16 tid;

	pkt.SByte();
	pkt >> tid >> strCharID;

	FriendAddResult resultCode = g_DBAgent.AddFriend(GetSocketID(), tid);
	result.SByte();
	result << tid << uint8(resultCode) << strCharID;

	RecvFriendModify(result, FRIEND_ADD);
}

void CUser::ReqRemoveFriend(Packet & pkt)
{
	Packet result(WIZ_FRIEND_PROCESS);
	string strCharID;

	pkt.SByte();
	pkt >> strCharID;

	FriendRemoveResult resultCode = g_DBAgent.RemoveFriend(strCharID, this);
	result.SByte();
	result << uint8(resultCode) << strCharID;

	RecvFriendModify(result, FRIEND_REMOVE);
}

/**
* @brief	Handles name change requests.
*
* @param	pkt	The packet.
*/
void CUser::ReqChangeName(Packet & pkt)
{
	NameChangeOpcode response;
	uint8 opcode;
	string strName;

	pkt >> opcode >> strName;

	switch (opcode)
	{
	case NameChangePlayerRequest:
		response = g_DBAgent.UpdateCharacterName(GetAccountName(), GetName(), strName);

		// On success, update the name in the server & remove the scroll.
		// (we checked if it existed before handling the request).
		if (response == NameChangeSuccess)
		{
			// Replace the character's name (in both the session and the character lookup hashmap).
			g_pMain->ReplaceCharacterName(this, strName);

			// Take the scroll...
			RobItem(ITEM_SCROLL_OF_IDENTITY);

			// Remove user from others' view & make them reappear again so 
			// the name can be updated for those currently in range.
			UserInOut(INOUT_OUT);
			UserInOut(INOUT_IN);
		}
		break;
	}

	SendNameChange(response);
}

/**
* @brief	Handles clan cape update requests.
*
* @param	pkt	The packet.
*/
void CUser::ReqChangeCape(Packet & pkt)
{
	uint16 sClanID, sCapeID;
	uint8 r, g, b;
	pkt >> sClanID >> sCapeID >> r >> g >> b;

	g_DBAgent.UpdateCape(sClanID, sCapeID, r, g, b);
}

void CUser::ReqUserLogOut()
{
	PlayerRankingProcess(GetZoneID(), true);
	g_pMain->KillNpc(GetSocketID());

	if (g_pMain->pTempleEvent.ActiveEvent != -1)
	{
		TempleOperations(TEMPLE_EVENT_DISBAND);

		if (!g_pMain->pTempleEvent.isActive)
			TempleOperations(TEMPLE_EVENT_COUNTER);
	}

	if (m_bLevel == 0)
		TRACE("### ReqUserLogOut - Level is Zero : bRoom=%d, bNation=%d, bZone=%d ####\n", GetEventRoom(), GetNation(), GetZoneID());

	g_DBAgent.UpdateUser(GetName(), UPDATE_LOGOUT, this);
	g_DBAgent.UpdateWarehouseData(GetAccountName(), UPDATE_LOGOUT, this);
	g_DBAgent.UpdateSavedMagic(this);

	if (m_bLogout != 2)	// zone change logout
		g_DBAgent.AccountLogout(GetAccountName());

	// this session can be used again.
	m_deleted = false;
}

void CUser::ReqSaveCharacter()
{
	if (m_bLevel == 0)
		TRACE("### ReqSaveCharacter - Level is Zero : bRoom=%d, bNation=%d, bZone=%d ####\n", GetEventRoom(), GetNation(), GetZoneID());

	g_DBAgent.UpdateUser(GetName(), UPDATE_PACKET_SAVE, this);
	g_DBAgent.UpdateWarehouseData(GetAccountName(), UPDATE_PACKET_SAVE, this);
	g_DBAgent.UpdateSavedMagic(this);
}

void CKnightsManager::ReqKnightsPacket(CUser* pUser, Packet & pkt)
{
	uint8 opcode;
	pkt >> opcode;
	switch (opcode)
	{
	case KNIGHTS_CREATE:
		ReqCreateKnights(pUser, pkt);
		break;
	case KNIGHTS_JOIN:
	case KNIGHTS_WITHDRAW:
		ReqUpdateKnights(pUser, pkt, opcode);
		break;
	case KNIGHTS_REMOVE:
	case KNIGHTS_ADMIT:
	case KNIGHTS_REJECT:
	case KNIGHTS_CHIEF:
	case KNIGHTS_VICECHIEF:
	case KNIGHTS_OFFICER:
	case KNIGHTS_PUNISH:
		ReqModifyKnightsMember(pUser, pkt, opcode);
		break;
	case KNIGHTS_DESTROY:
		ReqDestroyKnights(pUser, pkt);
		break;
	case KNIGHTS_MEMBER_REQ:
		ReqAllKnightsMember(pUser, pkt);
		break;
	case KNIGHTS_LIST_REQ:
		ReqKnightsList(pkt);
		break;
	case KNIGHTS_ALLLIST_REQ:
		g_DBAgent.LoadKnightsAllList();
		break;
	case KNIGHTS_MARK_REGISTER:
		ReqRegisterClanSymbol(pUser, pkt);
		break;
	case KNIGHTS_UPDATE_GRADE:
		ReqUpdateGrade(pkt);
		break;
	case KNIGHTS_DONATE_POINTS:
		ReqDonateNP(pUser, pkt);
		break;
	case KNIGHTS_REFUND_POINTS:
		ReqRefundNP(pkt);
		break;
	case KNIGHTS_UPDATE_FUND:
		ReqUpdateNP(pkt);
		break;
	}
}

void CKnightsManager::ReqCreateKnights(CUser *pUser, Packet & pkt)
{
	if (pUser == nullptr)
		return;

	Packet result(WIZ_KNIGHTS_PROCESS, uint8(KNIGHTS_CREATE));
	string strKnightsName, strChief;
	uint16 sClanID;
	uint8 bFlag, bNation;
	int8 bResult;

	pkt >> bFlag >> sClanID >> bNation >> strKnightsName >> strChief;
	bResult = g_DBAgent.CreateKnights(sClanID, bNation, strKnightsName, strChief, bFlag);

	if (bResult > 0)
	{
		result << bResult;
		pUser->Send(&result);
		return;
	}

	CKnights *pKnights = new CKnights();

	pKnights->m_sIndex = sClanID;
	pKnights->m_byFlag = bFlag;
	pKnights->m_byNation = bNation;

	pKnights->m_strName = strKnightsName;
	pKnights->m_strChief = pUser->GetName();

	pUser->GoldLose(CLAN_COIN_REQUIREMENT, false);

	g_pMain->m_KnightsArray.PutData(pKnights->m_sIndex, pKnights);

	pKnights->AddUser(pUser);

	// Ensure the clan leader's fame is set
	// CKnights::AddUser() will default it to TRAINEE, so it needs to be set afterwards.
	pUser->m_bFame = CHIEF;

	result	<< uint8(1) << pUser->GetSocketID() 
		<< sClanID << strKnightsName
		<< pKnights->m_byGrade << pKnights->m_byRanking
		<< pUser->m_iGold;

	pUser->SendToRegion(&result);
}

void CKnightsManager::ReqUpdateKnights(CUser *pUser, Packet & pkt, uint8 opcode)
{
	if (pUser == nullptr)
		return;

	Packet result(WIZ_KNIGHTS_PROCESS);
	uint16 sClanID = pkt.read<uint16>();
	string strCharID = pUser->GetName();
	int8 bResult = int8(g_DBAgent.UpdateKnights(opcode, strCharID, sClanID, 0));
	if (bResult < 0)
	{
		result << opcode << uint8(0);
		pUser->Send(&result);
		return;
	}

	result << sClanID;  // Hate doing this, but it's reusable.
	RecvUpdateKnights(pUser, result, opcode);
}

void CKnightsManager::ReqModifyKnightsMember(CUser *pUser, Packet & pkt, uint8 command)
{
	if (pUser == nullptr)
		return;

	Packet result(WIZ_KNIGHTS_PROCESS);
	string strCharID;
	uint16 sClanID;
	int8 bRemoveFlag, bResult;

	pkt >> sClanID >> strCharID >> bRemoveFlag;
	bResult = int8(g_DBAgent.UpdateKnights(command, strCharID, sClanID, bRemoveFlag));

	if (bResult < 0)
	{
		result << command << uint8(0);
		pUser->Send(&result);
		return;
	}

	result << sClanID << strCharID; // I really hate doing this, but OK...
	RecvModifyFame(pUser, result, command);
}

void CKnightsManager::ReqDestroyKnights(CUser *pUser, Packet & pkt)
{
	uint16 sClanID = pkt.read<uint16>();
	CKnights *pKnights = g_pMain->GetClanPtr(sClanID);
	if (pKnights == nullptr)
		return;

	int8 bResult = int8(g_DBAgent.DeleteKnights(sClanID));
	pKnights->Disband(pUser);
}

void CKnightsManager::ReqAllKnightsMember(CUser *pUser, Packet & pkt)
{
	Packet result(WIZ_KNIGHTS_PROCESS, uint8(KNIGHTS_MEMBER_REQ));
	int nOffset;
	uint16 sClanID, sCount;

	pkt >> sClanID;

	CKnights* pKnights = g_pMain->GetClanPtr(pUser->GetClanID());
	if (pKnights == nullptr)
		return;

	result << uint8(1);
	nOffset = result.wpos(); // store offset
	result	<< uint16(0) // placeholder for packet length 
		<< uint16(0); // placeholder for user count

	sCount = g_DBAgent.LoadKnightsAllMembers(sClanID, result);
	if (sCount > MAX_CLAN_USERS)
		return;

	pkt.put(nOffset, uint16(result.size() - 3));
	pkt.put(nOffset + 2, sCount);

	pUser->Send(&result);
}

void CKnightsManager::ReqKnightsList(Packet & pkt)
{
	// Okay, this effectively makes this useless in the majority of cases.
	if (g_pMain->m_nServerNo != BATTLE)
		return;

	string strKnightsName; 
	uint32 nPoints; 
	uint16 sClanID = pkt.read<uint16>(), sMembers;
	uint8 bNation, bRank;

	if (!g_DBAgent.LoadKnightsInfo(sClanID, bNation, strKnightsName, sMembers, nPoints, bRank))
		return;

	CKnights *pKnights = g_pMain->GetClanPtr(sClanID);
	if (pKnights == nullptr)
	{
		pKnights = new CKnights();

		if (!g_pMain->m_KnightsArray.PutData(sClanID, pKnights))
		{
			delete pKnights;
			return;
		}
	}

	// TODO: Move this all to a single method, as this is done multiple times
	pKnights->m_sIndex = sClanID;
	pKnights->m_byNation = bNation;
	pKnights->m_strName = strKnightsName;
	pKnights->m_sMembers = sMembers;
	pKnights->m_nPoints = nPoints;
	pKnights->m_byGrade = g_pMain->GetKnightsGrade(nPoints);
	pKnights->m_byRanking = bRank;
}

void CKnightsManager::ReqRegisterClanSymbol(CUser *pUser, Packet & pkt)
{
	if (pUser == nullptr)
		return;

	Packet result(WIZ_KNIGHTS_PROCESS, uint8(KNIGHTS_MARK_REGISTER));
	char clanSymbol[MAX_KNIGHTS_MARK];
	uint16 sClanID, sSymbolSize, sErrorCode = 0, sNewVersion = 0;

	pkt >> sClanID >> sSymbolSize;
	pkt.read(clanSymbol, sSymbolSize);

	bool bResult = g_DBAgent.UpdateClanSymbol(sClanID, sSymbolSize, clanSymbol);

	do
	{
		if (!bResult)
			break;

		CKnights *pKnights = g_pMain->GetClanPtr(sClanID);
		if (pKnights == nullptr)
		{
			sErrorCode = 20;
			break;
		}

		// Make sure they still have enough coins.
		if (!pUser->GoldLose(CLAN_SYMBOL_COST))
		{
			sErrorCode = 14;
			break;
		}

		sNewVersion = ++pKnights->m_sMarkVersion;
		pKnights->m_sMarkLen = sSymbolSize;

		memcpy(pKnights->m_Image, clanSymbol, sSymbolSize);

		// TODO: Send to all servers for updating via UDP

		sErrorCode = 1;
	} while (0);

	result << sErrorCode << sNewVersion;
	pUser->Send(&result);
}

/**
* @brief	Request a clan's grade (and cape) be updated
* 			in the database.
*
* @param	pkt	The packet.
*/
void CKnightsManager::ReqUpdateGrade(Packet & pkt)
{
	uint16 sClanID, sCapeID;
	uint8 byFlag;

	pkt >> sClanID >> byFlag >> sCapeID;
	g_DBAgent.UpdateClanGrade(sClanID, byFlag, sCapeID);
}

/**
* @brief	Requests a clan's notice be updated in the database.
*
* @param	pkt	The packet.
*/
void CKnightsManager::ReqUpdateClanNotice(Packet & pkt)
{
	uint16 sClanID;
	string strClanNotice;

	pkt >> sClanID >> strClanNotice;
	g_DBAgent.UpdateClanNotice(sClanID, strClanNotice);
}

void CUser::ReqSetLogInInfo(Packet & pkt)
{
	string strCharID, strServerIP, strClientIP;
	uint16 sServerNo;
	uint8 bInit;

	pkt >> strCharID >> strServerIP >> sServerNo >> strClientIP >> bInit;
	// if there was an error inserting to CURRENTUSER...
	if (!g_DBAgent.SetLogInInfo(m_strAccountID, strCharID, strServerIP, sServerNo, strClientIP, bInit))
		Disconnect();
}

#if 0
void CUser::ReqBattleEventResult(Packet & pkt)
{
	string strMaxUserName;
	uint8 bType, bNation;

	pkt >> bType >> bNation >> strMaxUserName;
	m_DBAgent.UpdateBattleEvent(strMaxUserName, bNation);
}
#endif

/**
* @brief	Handles database requests for the King system.
*
* @param	pUser	The user making the request, if applicable. 
* 					nullptr if not.
* @param	pkt  	The packet.
*/
void CKingSystem::HandleDatabaseRequest(CUser * pUser, Packet & pkt)
{
	switch (pkt.read<uint8>())
	{
	case KING_ELECTION:
		HandleDatabaseRequest_Election(pUser, pkt);
		break;

	case KING_EVENT:
		HandleDatabaseRequest_Event(pUser, pkt);
		break;
	}
}

/**
* @brief	Handles database requests for the election system.
*
* @param	pUser	The user making the request, if applicable. 
* 					nullptr if not.
* @param	pkt  	The packet.
*/
void CKingSystem::HandleDatabaseRequest_Election(CUser * pUser, Packet & pkt)
{
	uint8 opcode;
	pkt >> opcode;

	switch (opcode)
	{
		// Special king system/election database requests
	case KING_ELECTION:
		{
			uint8 byNation, byType;
			pkt >> opcode >> byNation >> byType;
			switch (opcode)
			{
			case KING_ELECTION_UPDATE_STATUS: // 7
				g_DBAgent.UpdateElectionStatus(byType, byNation);
				break;

			case KING_ELECTION_UPDATE_LIST: // 6
				{
					bool bDelete;
					uint16 sKnights;
					uint32 nVotes = 0;
					string strNominee;

					pkt >> bDelete >> sKnights >> strNominee;
					g_DBAgent.UpdateElectionList(bDelete ? 2 : 1, byType, byNation, sKnights, nVotes, strNominee);
				} break;
			}
		} break;

	case KING_ELECTION_NOMINATE:
		{
			if (pUser == nullptr)
				return;

			Packet result(WIZ_KING, uint8(KING_ELECTION));
			std::string strNominee;
			int16 resultCode;
			pkt >> strNominee;
			resultCode = g_DBAgent.UpdateCandidacyRecommend(pUser->m_strUserID, strNominee, pUser->GetNation());

			// On success, we need to sync the local list.
			if (resultCode == 1)
			{
				CKingSystem * pData = g_pMain->m_KingSystemArray.GetData(pUser->GetNation());
				if (pData == nullptr)
					return;

				pData->InsertNominee(strNominee);
			}
			result << opcode << resultCode;
			pUser->Send(&result);
		} break;

	case KING_ELECTION_NOTICE_BOARD:
		{
			pkt >> opcode;
			if (pUser == nullptr)
				return;

			if (opcode == KING_CANDIDACY_BOARD_WRITE)
			{
				string strNotice;
				pkt >> strNotice;
				g_DBAgent.UpdateCandidacyNoticeBoard(pUser->m_strUserID, pUser->GetNation(), strNotice);
			}
		} break;
	}
}

/**
* @brief	Handles database requests for King commands.
*
* @param	pUser	The user making the request, if applicable. 
* 					nullptr if not.
* @param	pkt  	The packet.
*/
void CKingSystem::HandleDatabaseRequest_Event(CUser * pUser, Packet & pkt)
{
	uint8 opcode, byNation;
	pkt >> opcode >> byNation;

	switch (opcode)
	{
	case KING_EVENT_NOAH:
	case KING_EVENT_EXP:
		{
			uint8 byAmount, byDay, byHour, byMinute;
			uint16 sDuration;
			pkt >> byAmount >> byDay >> byHour >> byMinute >> sDuration;

			g_DBAgent.UpdateNoahOrExpEvent(opcode, byNation, byAmount, byDay, byHour, byMinute, sDuration);
		} break;

	case KING_EVENT_PRIZE:
		{
			uint32 nCoins;
			string strUserID;
			pkt >> nCoins >> strUserID;

			g_DBAgent.InsertPrizeEvent(opcode, byNation, nCoins, strUserID);
		} break;
	}
}

void CUser::ReqSealItem(Packet & pkt)
{
	uint8 bSrcPos, bSealType, opcode, bSealResult;
	uint32 nItemID;
	uint64 nItemSerial;
	string strSealPasswd;

	pkt >> opcode >> bSealType >> nItemID >> bSrcPos >> strSealPasswd >> bSealResult;

	nItemSerial = GetItem(SLOT_MAX+bSrcPos)->nSerialNum;

	if (!bSealResult)
		bSealResult = g_DBAgent.SealItem(strSealPasswd, nItemSerial, nItemID, bSealType, this);

	Packet result(WIZ_ITEM_UPGRADE, uint8(ITEM_SEAL));
	result << bSealType << bSealResult << nItemID << bSrcPos;
	Send(&result);

	if (bSealResult == 1)
		SealItem(bSealType, bSrcPos);
}

void DatabaseThread::Shutdown()
{
	_running = false;

	// Wake them up in case they're sleeping.
	s_hEvent.Broadcast();

	s_thread->waitForExit();
	delete s_thread;

	_lock.lock();
	while (_queue.size())
	{
		Packet *p = _queue.front();
		_queue.pop();
		delete p;
	}
	_lock.unlock();
}
