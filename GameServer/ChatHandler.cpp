#include "stdafx.h"
#include "DBAgent.h"
<<<<<<< HEAD
=======
#include "../shared/DateTime.h"
>>>>>>> koserver2

using std::string;

ServerCommandTable CGameServerDlg::s_commandTable;
ChatCommandTable CUser::s_commandTable;

void CGameServerDlg::InitServerCommands()
{
	static Command<CGameServerDlg> commandTable[] = 
	{
		// Command				Handler											Help message
		{ "notice",				&CGameServerDlg::HandleNoticeCommand,			"Sends a server-wide chat notice." },
		{ "kill",				&CGameServerDlg::HandleKillUserCommand,			"Disconnects the specified player" },
		{ "open1",				&CGameServerDlg::HandleWar1OpenCommand,			"Opens war zone 1" },
		{ "open2",				&CGameServerDlg::HandleWar2OpenCommand,			"Opens war zone 2" },
		{ "open3",				&CGameServerDlg::HandleWar3OpenCommand,			"Opens war zone 3" },
		{ "open4",				&CGameServerDlg::HandleWar4OpenCommand,			"Opens war zone 4" },
		{ "open5",				&CGameServerDlg::HandleWar5OpenCommand,			"Opens war zone 5" },
		{ "open6",				&CGameServerDlg::HandleWar6OpenCommand,			"Opens war zone 6" },
		{ "snowopen",			&CGameServerDlg::HandleSnowWarOpenCommand,		"Opens the snow war zone" },
		{ "close",				&CGameServerDlg::HandleWarCloseCommand,			"Closes the active war zone" },
		{ "down",				&CGameServerDlg::HandleShutdownCommand,			"Shuts down the server" },
<<<<<<< HEAD
		{ "pause",				&CGameServerDlg::HandlePauseCommand,			"Prevents users from connecting to the server" },
		{ "resume",				&CGameServerDlg::HandleResumeCommand,			"Allows users to resume connecting to the server" },
=======
>>>>>>> koserver2
		{ "discount",			&CGameServerDlg::HandleDiscountCommand,			"Enables server discounts for the winning nation of the last war" },
		{ "alldiscount",		&CGameServerDlg::HandleGlobalDiscountCommand,	"Enables server discounts for everyone" },
		{ "offdiscount",		&CGameServerDlg::HandleDiscountOffCommand,		"Disables server discounts" },
		{ "captain",			&CGameServerDlg::HandleCaptainCommand,			"Sets the captains/commanders for the war" },
		{ "santa",				&CGameServerDlg::HandleSantaCommand,			"Enables a flying Santa Claus." },
		{ "offsanta",			&CGameServerDlg::HandleSantaOffCommand,			"Disables a flying Santa Claus/angel." },
		{ "angel",				&CGameServerDlg::HandleAngelCommand,			"Enables a flying angel." },
		{ "offangel",			&CGameServerDlg::HandleSantaOffCommand,			"Disables a flying Santa Claus/angel." },
		{ "permanent",			&CGameServerDlg::HandlePermanentChatCommand,	"Sets the permanent chat bar to the specified text." },
		{ "offpermanent",		&CGameServerDlg::HandlePermanentChatOffCommand,	"Resets the permanent chat bar text." },
		{ "reload_notice",		&CGameServerDlg::HandleReloadNoticeCommand,		"Reloads the in-game notice list." },
		{ "reload_tables",		&CGameServerDlg::HandleReloadTablesCommand,		"Reloads the in-game tables." },
<<<<<<< HEAD
		{ "count",				&CGameServerDlg::HandleCountCommand,			"Get online user count." },
		{ "permitconnect",		&CGameServerDlg::HandlePermitConnectCommand,	"Player unban" },
=======
		{ "reload_magics",		&CGameServerDlg::HandleReloadMagicsCommand,		"Reloads the in-game magic tables." },
		{ "reload_quests",		&CGameServerDlg::HandleReloadQuestCommand,		"Reloads the in-game quest tables." },
		{ "reload_ranks",		&CGameServerDlg::HandleReloadRanksCommand,		"Reloads the in-game rank tables." },
		{ "count",				&CGameServerDlg::HandleCountCommand,			"Get online user count." },
		{ "permitconnect",		&CGameServerDlg::HandlePermitConnectCommand,	"Player unban" },
		{ "warresult",			&CGameServerDlg::HandleWarResultCommand,		"Set result for War" },
>>>>>>> koserver2
	};

	init_command_table(CGameServerDlg, commandTable, s_commandTable);
}

void CGameServerDlg::CleanupServerCommands() { free_command_table(s_commandTable); }

void CUser::InitChatCommands()
{
	static Command<CUser> commandTable[] = 
	{
		// Command				Handler											Help message
		{ "test",				&CUser::HandleTestCommand,						"Test command" },
		{ "give_item",			&CUser::HandleGiveItemCommand,					"Gives a player an item. Arguments: character name | item ID | [optional stack size]" },
		{ "zonechange",			&CUser::HandleZoneChangeCommand,				"Teleports you to the specified zone. Arguments: zone ID" },
		{ "monsummon",			&CUser::HandleMonsterSummonCommand,				"Spawns the specified monster (does not respawn). Arguments: monster's database ID" },
		{ "npcsummon",			&CUser::HandleNPCSummonCommand,					"Spawns the specified NPC (does not respawn). Arguments: NPC's database ID" },
<<<<<<< HEAD
=======
		{ "monkill",			&CUser::HandleMonKillCommand,					"Kill a NPC or Monster, Arguments: select an Npc and monster than use this command" },
>>>>>>> koserver2
		{ "open1",				&CUser::HandleWar1OpenCommand,					"Opens war zone 1" },
		{ "open2",				&CUser::HandleWar2OpenCommand,					"Opens war zone 2" },
		{ "open3",				&CUser::HandleWar3OpenCommand,					"Opens war zone 3" },
		{ "open4",				&CUser::HandleWar4OpenCommand,					"Opens war zone 4" },
		{ "open5",				&CUser::HandleWar5OpenCommand,					"Opens war zone 5" },
		{ "open6",				&CUser::HandleWar6OpenCommand,					"Opens war zone 6" },
		{ "captain",			&CUser::HandleCaptainCommand,					"Sets the captains/commanders for the war" },
		{ "snowopen",			&CUser::HandleSnowWarOpenCommand,				"Opens the snow war zone" },
		{ "close",				&CUser::HandleWarCloseCommand,					"Closes the active war zone" },
<<<<<<< HEAD
		{ "down",				&CUser::HandleShutdownCommand,					"Shuts down the server" },
		{ "np_change",			&CUser::HandleLoyaltyChangeCommand,				"Change a player an loyalty" },
		{ "exp_change",			&CUser::HandleExpChangeCommand,					"Change a player an exp" },
		{ "gold_change",		&CUser::HandleGoldChangeCommand,				"Change a player an gold" },
=======
		{ "np_change",			&CUser::HandleLoyaltyChangeCommand,				"Change a player an loyalty" },
		{ "exp_change",			&CUser::HandleExpChangeCommand,					"Change a player an exp" },
		{ "gold_change",		&CUser::HandleGoldChangeCommand,				"Change a player an gold" },
		{ "np_add",				&CUser::HandleLoyaltyAddCommand,				"Sets the server-wide NP event. If bonusPercent is set to 0, the event is ended. Arguments: bonusPercent" },
>>>>>>> koserver2
		{ "exp_add",			&CUser::HandleExpAddCommand,					"Sets the server-wide XP event. If bonusPercent is set to 0, the event is ended. Arguments: bonusPercent" },
		{ "money_add",			&CUser::HandleMoneyAddCommand,					"Sets the server-wide coin event. If bonusPercent is set to 0, the event is ended. Arguments: bonusPercent" },
		{ "permitconnect",		&CUser::HandlePermitConnectCommand,				"Player unban" },
		{ "tp_all",				&CUser::HandleTeleportAllCommand,				"Players send to home zone." },
		{ "summonknights",		&CUser::HandleKnightsSummonCommand,				"Teleport the clan users. Arguments: clan name" },
<<<<<<< HEAD
=======
		{ "warresult",			&CUser::HandleWarResultCommand,					"Set result for War"},
		{ "resetranking",		&CUser::HandleResetPlayerRankingCommand,		"Reset player ranking. Arguments : Zone ID"},
>>>>>>> koserver2
	};

	init_command_table(CUser, commandTable, s_commandTable);
}

void CUser::CleanupChatCommands() { free_command_table(s_commandTable); }

void CUser::Chat(Packet & pkt)
{
	Packet result;
	uint16 sessID;
	uint8 type = pkt.read<uint8>(), bOutType = type, seekingPartyOptions, bNation;
<<<<<<< HEAD
	string chatstr, finalstr, strSender, * strMessage;

	bool isAnnouncement = false;

	if (isMuted())
		return;	
=======
	string chatstr, finalstr, strSender, * strMessage, chattype;
	CUser *pUser;
	CKnights * pKnights;
	DateTime time;

	bool isAnnouncement = false;

	if (isMuted() || (GetZoneID() == ZONE_PRISON && !isGM()))
		return;
>>>>>>> koserver2

	pkt >> chatstr;
	if (chatstr.empty() || chatstr.size() > 128)
		return;

	// Process GM commands
	if (isGM() && ProcessChatCommand(chatstr))
<<<<<<< HEAD
		return;
=======
	{
		chattype = "GAME MASTER";
		g_pMain->WriteChatLogFile(string_format("[ %s - %d:%d:%d ] %s : %s ( Zone=%d, X=%d, Z=%d )\n",chattype.c_str(),time.GetHour(),time.GetMinute(),time.GetSecond(),GetName().c_str(),chatstr.c_str(),GetZoneID(),uint16(GetX()),uint16(GetZ())));
		return;
	}
>>>>>>> koserver2

	if (type == SEEKING_PARTY_CHAT)
		pkt >> seekingPartyOptions;

	// Handle GM notice & announcement commands
	if (type == PUBLIC_CHAT || type == ANNOUNCEMENT_CHAT)
	{
		// Trying to use a GM command without authorisation? Bad player!
		if (!isGM())
			return;

		if (type == ANNOUNCEMENT_CHAT)
			type = WAR_SYSTEM_CHAT;

		bOutType = type;

		// This is horrible, but we'll live with it for now.
		// Pull the notice string (#### NOTICE : %s ####) from the database.
		// Format the chat string around it, so our chat data is within the notice
		g_pMain->GetServerResource(IDP_ANNOUNCEMENT, &finalstr, chatstr.c_str());
		isAnnouncement = true;
	}


	if (isAnnouncement)
	{
		// GM notice/announcements show no name, so don't bother setting it.
		strMessage = &finalstr; // use the formatted message from the user
		bNation = KARUS; // arbitrary nation
		sessID = -1;
	}
	else
	{
		strMessage = &chatstr; // use the raw message from the user
		strSender = GetName(); // everything else uses a name, so set it
		bNation = GetNation();
		sessID = GetSocketID();
	}

	// GMs should use GM chat to help them stand out amongst players.
	if (type == GENERAL_CHAT && isGM())
		bOutType = GM_CHAT;

	ChatPacket::Construct(&result, bOutType, strMessage, &strSender, bNation, sessID);

	switch (type) 
	{
	case GENERAL_CHAT:
		g_pMain->Send_NearRegion(&result, GetMap(), GetRegionX(), GetRegionZ(), GetX(), GetZ());
<<<<<<< HEAD
=======
		chattype = "GENERAL_CHAT";
>>>>>>> koserver2
		break;

	case PRIVATE_CHAT:
		{
<<<<<<< HEAD
			// TODO : Kontrol Edilecek.
			CUser *pUser = g_pMain->GetUserPtr(m_sPrivateChatUser);
			if (pUser == nullptr || !pUser->isInGame()) 
				return;
		}
=======
			pUser = g_pMain->GetUserPtr(m_sPrivateChatUser);
			if (pUser == nullptr || !pUser->isInGame()) 
				return;

			chattype = "PRIVATE_CHAT";
		}

>>>>>>> koserver2
	case COMMAND_PM_CHAT:
		{
			if (type == COMMAND_PM_CHAT && GetFame() != COMMAND_CAPTAIN)
				return;

<<<<<<< HEAD
			CUser *pUser = g_pMain->GetUserPtr(m_sPrivateChatUser);
			if (pUser != nullptr) 
				pUser->Send(&result);
		} break;

	case PARTY_CHAT:
		if (isInParty())
			g_pMain->Send_PartyMember(GetPartyID(), &result);
=======
			pUser = g_pMain->GetUserPtr(m_sPrivateChatUser);
			if (pUser != nullptr) 
				pUser->Send(&result);
		}
		break;

	case PARTY_CHAT:
		if (isInParty())
		{
			g_pMain->Send_PartyMember(GetPartyID(), &result);
			chattype = "PARTY_CHAT";
		}
>>>>>>> koserver2
		break;

	case SHOUT_CHAT:
		if (m_sMp < (m_iMaxMp / 5))
			break;

		// Characters under level 35 require 3,000 coins to shout.
		if (!isGM()
			&& GetLevel() < 35
			&& !GoldLose(SHOUT_COIN_REQUIREMENT))
			break;

		MSpChange(-(m_iMaxMp / 5));
		SendToRegion(&result);
<<<<<<< HEAD
=======
		chattype = "SHOUT_CHAT";
>>>>>>> koserver2
		break;

	case KNIGHTS_CHAT:
		if (isInClan())
<<<<<<< HEAD
			g_pMain->Send_KnightsMember(GetClanID(), &result);
=======
		{
			pKnights = g_pMain->GetClanPtr(GetClanID());
			g_pMain->Send_KnightsMember(GetClanID(), &result);
			chattype = "KNIGHTS_CHAT";
		}
>>>>>>> koserver2
		break;
	case CLAN_NOTICE:
		if (isInClan() 
			&& isClanLeader())
		{
<<<<<<< HEAD
			CKnights * pKnights = g_pMain->GetClanPtr(GetClanID());
=======
			pKnights = g_pMain->GetClanPtr(GetClanID());
>>>>>>> koserver2
			if (pKnights == nullptr)
				return;

			pKnights->UpdateClanNotice(chatstr);
		}
		break;
	case PUBLIC_CHAT:
	case ANNOUNCEMENT_CHAT:
		if (isGM())
			g_pMain->Send_All(&result);
		break;
	case COMMAND_CHAT:
		if (GetFame() == COMMAND_CAPTAIN)
<<<<<<< HEAD
			g_pMain->Send_CommandChat(&result, m_bNation, this);
=======
		{
			g_pMain->Send_CommandChat(&result, m_bNation, this);
			chattype = "COMMAND_CHAT";
		}
>>>>>>> koserver2
		break;
	case MERCHANT_CHAT:
		if (isMerchanting())
			SendToRegion(&result);
		break;
	case ALLIANCE_CHAT:
		if (isInClan())
		{
<<<<<<< HEAD
			CKnights *pKnights = g_pMain->GetClanPtr(GetClanID());
			if (pKnights != nullptr && pKnights->isInAlliance())
				g_pMain->Send_KnightsAlliance(pKnights->GetAllianceID(), &result);
=======
			pKnights = g_pMain->GetClanPtr(GetClanID());
			if (pKnights != nullptr && pKnights->isInAlliance())
				g_pMain->Send_KnightsAlliance(pKnights->GetAllianceID(), &result);
			chattype = "ALLIANCE_CHAT";
>>>>>>> koserver2
		}
		break;
	case WAR_SYSTEM_CHAT:
		if (isGM())
			g_pMain->Send_All(&result);
		break;
	case SEEKING_PARTY_CHAT:
		if (m_bNeedParty == 2)
		{
			Send(&result);
			g_pMain->Send_Zone_Matched_Class(&result, GetZoneID(), this, GetNation(), seekingPartyOptions);
		}
		break;
	}
<<<<<<< HEAD
=======

	if (!chattype.empty())
	{
		if (pUser && type == 2)
			g_pMain->WriteChatLogFile(string_format("[ %s - %d:%d:%d ] %s > %s : %s ( Zone=%d, X=%d, Z=%d )\n",chattype.c_str(),time.GetHour(),time.GetMinute(),time.GetSecond(),strSender.c_str(),pUser->GetName().c_str(),chatstr.c_str(),GetZoneID(),uint16(GetX()),uint16(GetZ())));
		else if (pKnights && (type == 6 || type == 15))
			g_pMain->WriteChatLogFile(string_format("[ %s - %d:%d:%d ] %s > %s : %s ( Zone=%d, X=%d, Z=%d )\n",chattype.c_str(),time.GetHour(),time.GetMinute(),time.GetSecond(),strSender.c_str(),pKnights->GetName().c_str(),chatstr.c_str(),GetZoneID(),uint16(GetX()),uint16(GetZ())));
		else
			g_pMain->WriteChatLogFile(string_format("[ %s - %d:%d:%d ] %s : %s ( Zone=%d, X=%d, Z=%d )\n",chattype.c_str(),time.GetHour(),time.GetMinute(),time.GetSecond(),strSender.c_str(),chatstr.c_str(),GetZoneID(),uint16(GetX()),uint16(GetZ())));
	}
>>>>>>> koserver2
}

void CUser::ChatTargetSelect(Packet & pkt)
{
	uint8 type = pkt.read<uint8>();

	// TODO: Replace this with an enum
	// Attempt to find target player in-game
	if (type == 1)
	{
		Packet result(WIZ_CHAT_TARGET, type);
		std::string strUserID;
		pkt >> strUserID;
		if (strUserID.empty() || strUserID.size() > MAX_ID_SIZE)
			return;

		CUser *pUser = g_pMain->GetUserPtr(strUserID, TYPE_CHARACTER);
		if (pUser == nullptr || pUser == this)
<<<<<<< HEAD
		{
			result << int16(0); 
		}
		else if (pUser->isBlockingPrivateChat())
		{
			result << int16(-1);
		}
=======
			result << int16(0); 
		else if (pUser->isBlockingPrivateChat())
			result << int16(-1) << pUser->GetName();
>>>>>>> koserver2
		else
		{
			m_sPrivateChatUser = pUser->GetSocketID();
			result << int16(1) << pUser->GetName();
		}
		Send(&result);
	}
	else if (type == 3)
	{
		//Questions
		uint8 sSubType;
		uint8 sMessageLen;
		std::string sMessage;
		pkt >> sSubType >> sMessageLen >> sMessage;
	}
	// Allow/block PMs
	else
	{
		m_bBlockPrivateChat = pkt.read<bool>(); 
	}
}

/**
* @brief	Sends a notice to all users in the current zone
* 			upon death.
*
* @param	pKiller	The killer.
*/
void CUser::SendDeathNotice(Unit * pKiller, DeathNoticeType noticeType) 
{
	if (pKiller == nullptr)
		return;

	Packet result(WIZ_CHAT, uint8(DEATH_NOTICE));

	result.SByte();
	result	<< GetNation()
		<< uint8(noticeType)
		<< pKiller->GetID() // session ID?
		<< pKiller->GetName()
		<< GetID() // session ID?
		<< GetName()
		<< uint16(GetX()) << uint16(GetZ());

<<<<<<< HEAD
	SendToZone(&result);
=======
	SendToZone(&result,this,pKiller->GetEventRoom(),(isInArena() ? RANGE_20M : 0.0f));
>>>>>>> koserver2
}

bool CUser::ProcessChatCommand(std::string & message)
{
	// Commands require at least 2 characters
	if (message.size() <= 1
		// If the prefix isn't correct
			|| message[0] != CHAT_COMMAND_PREFIX
			// or if we're saying, say, ++++ (faster than looking for the command in the map)
			|| message[1] == CHAT_COMMAND_PREFIX)
			// we're not a command.
			return false;

	// Split up the command by spaces
	CommandArgs vargs = StrSplit(message, " ");
	std::string command = vargs.front(); // grab the first word (the command)
	vargs.pop_front(); // remove the command from the argument list

	// Make the command lowercase, for 'case-insensitive' checking.
	STRTOLOWER(command);

	// Command doesn't exist
	ChatCommandTable::iterator itr = s_commandTable.find(command.c_str() + 1); // skip the prefix character
	if (itr == s_commandTable.end())
		return false;

	// Run the command
	return (this->*(itr->second->Handler))(vargs, message.c_str() + command.size() + 1, itr->second->Help);
}

COMMAND_HANDLER(CUser::HandleTestCommand)
{
<<<<<<< HEAD
=======
	if (!isGM())
		return false;

>>>>>>> koserver2
	return true;
}

COMMAND_HANDLER(CUser::HandleGiveItemCommand)
{
<<<<<<< HEAD
=======
	if (!isGM())
		return false;

>>>>>>> koserver2
	// Char name | item ID | [stack size]
	if (vargs.size() < 2)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +give_item CharacterName ItemID StackSize");
		return true;
	}

	std::string strUserID = vargs.front();
	vargs.pop_front();

	CUser *pUser = g_pMain->GetUserPtr(strUserID, TYPE_CHARACTER);
	if (pUser == nullptr)
	{
		g_pMain->SendHelpDescription(this, "Error : User is not online");
		return true;
	}

	uint32 nItemID = atoi(vargs.front().c_str());
	vargs.pop_front();
	_ITEM_TABLE *pItem = g_pMain->GetItemPtr(nItemID);
	if (pItem == nullptr)
	{
		// send error message saying the item does not exist
<<<<<<< HEAD
=======
		g_pMain->SendHelpDescription(this, "Error : Item does not exist");
>>>>>>> koserver2
		return true;
	}

	uint16 sCount = 1;
	if (!vargs.empty())
		sCount = atoi(vargs.front().c_str());


	if (!pUser->GiveItem(nItemID, sCount))
<<<<<<< HEAD
	{
		// send error message saying the item couldn't be added
	}
=======
		g_pMain->SendHelpDescription(this, "Error : Item could not be added");
>>>>>>> koserver2

	return true;
}

COMMAND_HANDLER(CUser::HandleZoneChangeCommand)
{
<<<<<<< HEAD
=======
	if (!isGM())
		return false;

>>>>>>> koserver2
	if (vargs.empty())
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +zonechange ZoneNumber");
		return true;
	}

	// Behave as in official (we'll fix this later)
	int nZoneID = atoi(vargs.front().c_str());
<<<<<<< HEAD
	ZoneChange(nZoneID, m_curx, m_curz);
=======

	_START_POSITION * pStartPosition = g_pMain->GetStartPosition(nZoneID);
	if (pStartPosition == nullptr) 
		return false;

	ZoneChange(nZoneID, 
		(float)(GetNation() == KARUS ? pStartPosition->sKarusX : pStartPosition->sElmoradX + myrand(0, pStartPosition->bRangeX)), 
		(float)(GetNation() == KARUS ? pStartPosition->sKarusZ : pStartPosition->sElmoradZ + myrand(0, pStartPosition->bRangeZ)));

>>>>>>> koserver2
	return true;
}

COMMAND_HANDLER(CUser::HandleMonsterSummonCommand)
{
<<<<<<< HEAD
=======
	if (!isGM())
		return false;

>>>>>>> koserver2
	if (vargs.empty())
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +monsummon MonsterSID");
		return true;
	}

	int sSid = atoi(vargs.front().c_str());
	g_pMain->SpawnEventNpc(sSid, true, GetZoneID(), GetX(), GetY(), GetZ());

	return true;
}

COMMAND_HANDLER(CUser::HandleNPCSummonCommand)
{
<<<<<<< HEAD
=======
	if (!isGM())
		return false;

>>>>>>> koserver2
	if (vargs.empty())
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +npcsummon NPCSID");
		return true;
	}

	int sSid = atoi(vargs.front().c_str());
	g_pMain->SpawnEventNpc(sSid, false, GetZoneID(), GetX(), GetY(), GetZ());

	return true;
}

<<<<<<< HEAD
=======
COMMAND_HANDLER(CUser::HandleMonKillCommand)
{
	if (!isGM())
		return false;

	if (GetTargetID() == 0 && GetTargetID() < NPC_BAND)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : Select a NPC or Monster than use +monkills");
		return false;
	}

	CNpc *pNpc = g_pMain->GetNpcPtr(GetTargetID());

	if (pNpc)
		g_pMain->KillNpc(GetTargetID());

	return true;
}

>>>>>>> koserver2
bool CGameServerDlg::ProcessServerCommand(std::string & message)
{
	// Commands require at least 2 characters
	if (message.size() <= 1
		// If the prefix isn't correct
			|| message[0] != SERVER_COMMAND_PREFIX)
			// we're not a command.
			return false;

	// Split up the command by spaces
	CommandArgs vargs = StrSplit(message, " ");
	std::string command = vargs.front(); // grab the first word (the command)
	vargs.pop_front(); // remove the command from the argument list

	// Make the command lowercase, for 'case-insensitive' checking.
	STRTOLOWER(command);

	// Command doesn't exist
	ServerCommandTable::iterator itr = s_commandTable.find(command.c_str() + 1); // skip the prefix character
	if (itr == s_commandTable.end())
		return false;

	// Run the command
	return (this->*(itr->second->Handler))(vargs, message.c_str() + command.size() + 1, itr->second->Help);
}

COMMAND_HANDLER(CGameServerDlg::HandleNoticeCommand)
{
	if (vargs.empty())
		return true;

	SendNotice(args);
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleKillUserCommand)
{
	if (vargs.empty())
	{
		// send description
		printf("Using Sample : +kill CharacterName\n");
		return true;
	}

	std::string strUserID = vargs.front();
	CUser *pUser = GetUserPtr(strUserID, TYPE_CHARACTER);
	if (pUser == nullptr)
	{
		printf("Error : User is not online\n");
		return true;
	}

	// Disconnect the player
	pUser->Disconnect();

	// send a message saying the player was disconnected
	return true;
}

<<<<<<< HEAD
COMMAND_HANDLER(CUser::HandleWar1OpenCommand) { return g_pMain->HandleWar1OpenCommand(vargs, args, description); }
=======
COMMAND_HANDLER(CUser::HandleWar1OpenCommand) { return !isGM() ? false : g_pMain->HandleWar1OpenCommand(vargs, args, description); }
>>>>>>> koserver2
COMMAND_HANDLER(CGameServerDlg::HandleWar1OpenCommand)
{
	BattleZoneOpen(BATTLEZONE_OPEN, 1);
	return true;
}

<<<<<<< HEAD
COMMAND_HANDLER(CUser::HandleWar2OpenCommand) { return g_pMain->HandleWar2OpenCommand(vargs, args, description); }
=======
COMMAND_HANDLER(CUser::HandleWar2OpenCommand) { return !isGM() ? false : g_pMain->HandleWar2OpenCommand(vargs, args, description); }
>>>>>>> koserver2
COMMAND_HANDLER(CGameServerDlg::HandleWar2OpenCommand)
{
	BattleZoneOpen(BATTLEZONE_OPEN, 2);
	return true;
}

COMMAND_HANDLER(CUser::HandleWar3OpenCommand) { return g_pMain->HandleWar3OpenCommand(vargs, args, description); }
COMMAND_HANDLER(CGameServerDlg::HandleWar3OpenCommand)
{
<<<<<<< HEAD
=======
	g_pMain->m_byBattleZoneType = ZONE_ARDREAM;
>>>>>>> koserver2
	BattleZoneOpen(BATTLEZONE_OPEN, 3);
	return true;
}

<<<<<<< HEAD
COMMAND_HANDLER(CUser::HandleWar4OpenCommand) { return g_pMain->HandleWar4OpenCommand(vargs, args, description); }
=======
COMMAND_HANDLER(CUser::HandleWar4OpenCommand) { return !isGM() ? false : g_pMain->HandleWar4OpenCommand(vargs, args, description); }
>>>>>>> koserver2
COMMAND_HANDLER(CGameServerDlg::HandleWar4OpenCommand)
{
	BattleZoneOpen(BATTLEZONE_OPEN, 4);
	return true;
}

COMMAND_HANDLER(CUser::HandleWar5OpenCommand) { return g_pMain->HandleWar5OpenCommand(vargs, args, description); }
COMMAND_HANDLER(CGameServerDlg::HandleWar5OpenCommand)
{
	BattleZoneOpen(BATTLEZONE_OPEN, 5);
	return true;
}

<<<<<<< HEAD
COMMAND_HANDLER(CUser::HandleWar6OpenCommand) { return g_pMain->HandleWar6OpenCommand(vargs, args, description); }
=======
COMMAND_HANDLER(CUser::HandleWar6OpenCommand) { return !isGM() ? false : g_pMain->HandleWar6OpenCommand(vargs, args, description); }
>>>>>>> koserver2
COMMAND_HANDLER(CGameServerDlg::HandleWar6OpenCommand)
{
	BattleZoneOpen(BATTLEZONE_OPEN, 6);
	return true;
}

COMMAND_HANDLER(CUser::HandleSnowWarOpenCommand) { return g_pMain->HandleSnowWarOpenCommand(vargs, args, description); }
COMMAND_HANDLER(CGameServerDlg::HandleSnowWarOpenCommand)
{
	BattleZoneOpen(SNOW_BATTLEZONE_OPEN);
	return true;
}

<<<<<<< HEAD
COMMAND_HANDLER(CUser::HandleWarCloseCommand) { return g_pMain->HandleWarCloseCommand(vargs, args, description); }
COMMAND_HANDLER(CGameServerDlg::HandleWarCloseCommand)
{
	ResetBattleZone();
	m_byBanishFlag = true;
=======
COMMAND_HANDLER(CUser::HandleWarCloseCommand) { return !isGM() ? false : g_pMain->HandleWarCloseCommand(vargs, args, description); }
COMMAND_HANDLER(CGameServerDlg::HandleWarCloseCommand)
{
	BattleZoneClose();
>>>>>>> koserver2
	return true;
}

COMMAND_HANDLER(CUser::HandleLoyaltyChangeCommand)
{
<<<<<<< HEAD
=======
	if (!isGM())
		return false;

>>>>>>> koserver2
	// Char name | loyalty
	if (vargs.size() < 2)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +np_change CharacterName Loyalty(+/-)");
		return true;
	}

	std::string strUserID = vargs.front();
	vargs.pop_front();

	CUser *pUser = g_pMain->GetUserPtr(strUserID, TYPE_CHARACTER);
	if (pUser == nullptr)
	{
		g_pMain->SendHelpDescription(this, "Error : User is not online");
		return true;
	}

	uint32 nLoyalty = atoi(vargs.front().c_str());

	if (nLoyalty != 0)
		pUser->SendLoyaltyChange(nLoyalty, false);

	return true;
}

COMMAND_HANDLER(CUser::HandleExpChangeCommand)
{
<<<<<<< HEAD
=======
	if (!isGM())
		return false;

>>>>>>> koserver2
	// Char name | exp
	if (vargs.size() < 2)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +exp_change CharacterName Exp(+/-)");
		return true;
	}

	std::string strUserID = vargs.front();
	vargs.pop_front();

	CUser *pUser = g_pMain->GetUserPtr(strUserID, TYPE_CHARACTER);
	if (pUser == nullptr)
	{
		g_pMain->SendHelpDescription(this, "Error : User is not online");
		return true;
	}

	int64 nExp = atoi(vargs.front().c_str());

	if (nExp != 0)
		pUser->ExpChange(nExp);

	return true;
}

COMMAND_HANDLER(CUser::HandleGoldChangeCommand)
{
<<<<<<< HEAD
=======
	if (!isGM())
		return false;

>>>>>>> koserver2
	// Char name | coins
	if (vargs.size() < 2)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +gold_change CharacterName Gold(+/-)");
		return true;
	}

	std::string strUserID = vargs.front();
	vargs.pop_front();

	CUser *pUser = g_pMain->GetUserPtr(strUserID, TYPE_CHARACTER);
	if (pUser == nullptr)
	{
		g_pMain->SendHelpDescription(this, "Error : User is not online");
		return true;
	}

	uint32 nGold = atoi(vargs.front().c_str());

	if (nGold != 0)
	{
		if (nGold > 0)
			pUser->GoldGain(nGold);
		else
			pUser->GoldLose(nGold);
	}

	return true;
}

<<<<<<< HEAD
// Starts/stops the server XP event & sets its server-wide bonus.
COMMAND_HANDLER(CUser::HandleExpAddCommand)
{
=======
COMMAND_HANDLER(CUser::HandleLoyaltyAddCommand)
{
	if (!isGM())
		return false;

	// Expects the bonus np percent, e.g. '+np_add' for a +15 np boost.
	if (vargs.empty())
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +np_add Percent");
		return true;
	}

	g_pMain->m_byNPEventAmount = (uint8) atoi(vargs.front().c_str());

	// Don't send the announcement if we're turning the event off.
	if (g_pMain->m_byNPEventAmount == 0)
		return true;

	g_pMain->SendFormattedResource(IDS_NP_REPAY_EVENT, Nation::ALL, false, g_pMain->m_byNPEventAmount);
	return true;
}

// Starts/stops the server XP event & sets its server-wide bonus.
COMMAND_HANDLER(CUser::HandleExpAddCommand)
{
	if (!isGM())
		return false;

>>>>>>> koserver2
	// Expects the bonus XP percent, e.g. '+exp_add' for a +15 XP boost.
	if (vargs.empty())
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +exp_add Percent");
		return true;
	}

	g_pMain->m_byExpEventAmount = (uint8) atoi(vargs.front().c_str());

	// Don't send the announcement if we're turning the event off.
	if (g_pMain->m_byExpEventAmount == 0)
		return true;

	g_pMain->SendFormattedResource(IDS_EXP_REPAY_EVENT, Nation::ALL, false, g_pMain->m_byExpEventAmount);
	return true;
}

// Starts/stops the server coin event & sets its server-wide bonus.
COMMAND_HANDLER(CUser::HandleMoneyAddCommand)
{
<<<<<<< HEAD
=======
	if (!isGM())
		return false;

>>>>>>> koserver2
	// Expects the bonus coin percent, e.g. '+money_add' for a +15 dropped coin boost.
	if (vargs.empty())
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +money_add Percent");
		return true;
	}

	g_pMain->m_byCoinEventAmount = (uint8) atoi(vargs.front().c_str());

	// Don't send the announcement if we're turning the event off.
	if (g_pMain->m_byCoinEventAmount == 0)
		return true;

	g_pMain->SendFormattedResource(IDS_MONEY_REPAY_EVENT, Nation::ALL, false, g_pMain->m_byCoinEventAmount);
	return true;
}

<<<<<<< HEAD
COMMAND_HANDLER(CUser::HandlePermitConnectCommand) { return g_pMain->HandlePermitConnectCommand(vargs, args, description); }
=======
COMMAND_HANDLER(CUser::HandlePermitConnectCommand) { return !isGM() ? false : g_pMain->HandlePermitConnectCommand(vargs, args, description); }
>>>>>>> koserver2
COMMAND_HANDLER(CGameServerDlg::HandlePermitConnectCommand)
{
	// Char name
	if (vargs.size() < 1)
	{
		// send description
		printf("Using Sample : +permitconnect CharacterName\n");
		return true;
	}

	std::string strUserID = vargs.front();
	vargs.pop_front();

	g_DBAgent.UpdateUserAuthority(strUserID,AUTHORITY_PLAYER);

<<<<<<< HEAD
	std::string sNoticeMessage = string_format("%s has been unbanned..!", strUserID.c_str());
=======
	std::string sNoticeMessage = string_format("%s is currently regular player.", strUserID.c_str());
>>>>>>> koserver2

	if (!sNoticeMessage.empty())
		g_pMain->SendNotice(sNoticeMessage.c_str(),Nation::ALL);

	return true;
}

COMMAND_HANDLER(CUser::HandleTeleportAllCommand)
{
<<<<<<< HEAD
=======
	if (!isGM())
		return false;

>>>>>>> koserver2
	// Zone number
	if (vargs.size() < 1)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +tp_all ZoneNumber | +tp_all ZoneNumber TargetZoneNumber");
		return true;
	}

<<<<<<< HEAD
	int nZoneID;
	int nTargetZoneID;
=======
	int nZoneID = 0;
	int nTargetZoneID = 0;
>>>>>>> koserver2

	if (vargs.size() == 1)
		nZoneID = atoi(vargs.front().c_str());

	if (vargs.size() == 2)
	{
		nZoneID = atoi(vargs.front().c_str());
		vargs.pop_front();
		nTargetZoneID = atoi(vargs.front().c_str());
	}

<<<<<<< HEAD
	if (nZoneID > 0)
		g_pMain->KickOutZoneUsers(nZoneID);
=======
	if (nZoneID > 0 || nTargetZoneID > 0)
		g_pMain->KickOutZoneUsers(nZoneID,nTargetZoneID);

>>>>>>> koserver2
	return true;
}

COMMAND_HANDLER(CUser::HandleKnightsSummonCommand)
{
<<<<<<< HEAD
=======
	if (!isGM())
		return false;

>>>>>>> koserver2
	// Clan name
	if(vargs.empty())
	{
		// Send description
		g_pMain->SendHelpDescription(this, "Using Sample : +summonknights ClanName");
		return true;
	}

	CKnights * pKnights;
<<<<<<< HEAD
	foreach_stlmap(itr,g_pMain->m_KnightsArray)
	{
		if(itr->second->GetName() == vargs.front().c_str())
		{
		   pKnights = g_pMain->GetClanPtr(itr->first);
		   break;
=======
	foreach_stlmap (itr,g_pMain->m_KnightsArray)
	{
		if(itr->second->GetName() == vargs.front().c_str())
		{
			pKnights = g_pMain->GetClanPtr(itr->first);
			break;
>>>>>>> koserver2
		}
	}

	if(pKnights == nullptr)
		return true;

	foreach_array(i,pKnights->m_arKnightsUser)
	{
		_KNIGHTS_USER *p = &pKnights->m_arKnightsUser[i];
		if (!p->byUsed || p->pSession == nullptr)
			continue;

		CUser* pUser = p->pSession;
		if(!pUser->isInGame() || pUser->GetName() == GetName())
			continue;

		pUser->ZoneChange(GetZoneID(), m_curx, m_curz);
	}

	return true;
}

<<<<<<< HEAD
COMMAND_HANDLER(CUser::HandleShutdownCommand) { return g_pMain->HandleShutdownCommand(vargs, args, description); }
COMMAND_HANDLER(CGameServerDlg::HandleShutdownCommand)
{
	g_pMain->m_socketMgr.SuspendServer();
	printf("Server shutdown, %d users kicked out.\n", KickOutAllUsers());
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandlePauseCommand)
{
	g_pMain->m_socketMgr.SuspendServer();
	printf("Server no longer accepting connections.\n");
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleResumeCommand)
{
	g_pMain->m_socketMgr.ResumeServer();
	printf("Server accepting connections.\n");
=======
COMMAND_HANDLER(CUser::HandleResetPlayerRankingCommand)
{
	if (!isGM())
		return false;

	// Zone ID
	if(vargs.empty())
	{
		// Send description
		g_pMain->SendHelpDescription(this, "Using Sample : +resetranking ZoneID");
		return true;
	}

	uint8 nZoneID;
	nZoneID = atoi(vargs.front().c_str());

	if (nZoneID > 0)
		g_pMain->ResetPlayerRankings(nZoneID);

	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleShutdownCommand)
{
	printf("Server shutdown, %d users kicked out.\n", KickOutAllUsers());
	m_socketMgr.Shutdown();
>>>>>>> koserver2
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleDiscountCommand)
{
	m_sDiscount = 1;
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleGlobalDiscountCommand)
{
	m_sDiscount = 2;
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleDiscountOffCommand)
{
	m_sDiscount = 0;
	return true;
}

<<<<<<< HEAD
COMMAND_HANDLER(CUser::HandleCaptainCommand) { return g_pMain->HandleCaptainCommand(vargs, args, description); }
=======
COMMAND_HANDLER(CUser::HandleCaptainCommand) { return !isGM() ? false : g_pMain->HandleCaptainCommand(vargs, args, description); }
>>>>>>> koserver2
COMMAND_HANDLER(CGameServerDlg::HandleCaptainCommand)
{
	m_KnightsRatingArray[KARUS_ARRAY].DeleteAllData();
	m_KnightsRatingArray[ELMORAD_ARRAY].DeleteAllData();
<<<<<<< HEAD
	LoadKnightsRankTable(true);
=======
	LoadKnightsRankTable(true, true);
>>>>>>> koserver2
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleSantaCommand)
{
	m_bSantaOrAngel = FLYING_SANTA;
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleSantaOffCommand)
{
	m_bSantaOrAngel = FLYING_NONE;
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleAngelCommand)
{
	m_bSantaOrAngel = FLYING_ANGEL;
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandlePermanentChatCommand)
{
	if (vargs.empty())
	{
		// send error saying we need args (unlike the previous implementation of this command)
		return true;
	}

	SetPermanentMessage("%s", args);
	return true;
}

<<<<<<< HEAD
=======
COMMAND_HANDLER(CUser::HandleWarResultCommand) { return !isGM() ? false : g_pMain->HandleWarResultCommand(vargs, args, description); }
COMMAND_HANDLER(CGameServerDlg::HandleWarResultCommand)
{
	// Nation number
	if (vargs.size() < 1)
	{
		// send description
		printf("Using Sample : +warresult 1/2 (KARUS/HUMAN)\n");
		return true;
	}

	if (!isWarOpen())
	{
		// send description
		printf("Warning : Battle is not open.\n");
		return true;
	}

	uint8 winner_nation;
	winner_nation = atoi(vargs.front().c_str());

	if (winner_nation > 0 && winner_nation < 3)
		BattleZoneResult(winner_nation);

	return true;
}

>>>>>>> koserver2
void CGameServerDlg::SendHelpDescription(CUser *pUser, std::string sHelpMessage)
{
	if (pUser == nullptr || sHelpMessage == "")
		return;

	Packet result(WIZ_CHAT, (uint8)PUBLIC_CHAT);
	result << pUser->GetNation() << pUser->GetSocketID() << (uint8)0 << sHelpMessage;
	pUser->Send(&result);
}

void CGameServerDlg::SetPermanentMessage(const char * format, ...)
{
	char buffer[128];
	va_list ap;
	va_start(ap, format);
	vsnprintf(buffer, 128, format, ap);
	va_end(ap);

	m_bPermanentChatMode = true;
	m_strPermanentChat = buffer;

	Packet result;
	ChatPacket::Construct(&result, PERMANENT_CHAT, &m_strPermanentChat);
	Send_All(&result);
}

COMMAND_HANDLER(CGameServerDlg::HandlePermanentChatOffCommand)
{
	Packet result;
	ChatPacket::Construct(&result, END_PERMANENT_CHAT);
	m_bPermanentChatMode = false;
	Send_All(&result);
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleReloadNoticeCommand)
{
	// Reload the notice data
	LoadNoticeData();

	// and update all logged in players
	// if we're using the new notice format that's always shown
<<<<<<< HEAD
#if __VERSION >= 1453
	SessionMap & sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
=======
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
>>>>>>> koserver2
	foreach (itr, sessMap)
	{
		CUser * pUser = TO_USER(itr->second);
		if (pUser->isInGame())
			pUser->SendNotice();
	}
<<<<<<< HEAD
	g_pMain->m_socketMgr.ReleaseLock();
#endif
=======
>>>>>>> koserver2
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleReloadTablesCommand)
{
<<<<<<< HEAD
	printf("Reloads the in-game tables.\n");
	g_pMain->m_MagictableArray.DeleteAllData();
	g_pMain->m_Magictype1Array.DeleteAllData();
	g_pMain->m_Magictype2Array.DeleteAllData();
	g_pMain->m_Magictype3Array.DeleteAllData();
	g_pMain->m_Magictype4Array.DeleteAllData();
	g_pMain->m_Magictype5Array.DeleteAllData();
	g_pMain->m_Magictype6Array.DeleteAllData();
	g_pMain->m_Magictype8Array.DeleteAllData();
	g_pMain->m_Magictype9Array.DeleteAllData();
	g_pMain->LoadMagicTable();
	g_pMain->LoadMagicType1();
	g_pMain->LoadMagicType2();
	g_pMain->LoadMagicType3();
	g_pMain->LoadMagicType4();
	g_pMain->LoadMagicType5();
	g_pMain->LoadMagicType6();
	g_pMain->LoadMagicType7();
	g_pMain->LoadMagicType8();
	g_pMain->LoadMagicType9();
	g_pMain->m_UserPersonalRankMap.clear();
	g_pMain->m_UserKnightsRankMap.clear();
	g_pMain->LoadUserRankings();
	g_pMain->m_StartPositionArray.DeleteAllData();
	g_pMain->LoadStartPositionTable();
	g_pMain->m_ItemExchangeArray.DeleteAllData();
	g_pMain->LoadItemExchangeTable();
	g_pMain->m_ItemUpgradeArray.DeleteAllData();
	g_pMain->LoadItemUpgradeTable();
	g_pMain->m_QuestHelperArray.DeleteAllData();
	g_pMain->LoadQuestHelperTable();
	g_pMain->m_QuestMonsterArray.DeleteAllData();
	g_pMain->LoadQuestMonsterTable();
	g_pMain->m_EventTriggerArray.DeleteAllData();
	g_pMain->LoadEventTriggerTable();
=======
	m_StartPositionArray.DeleteAllData();
	LoadStartPositionTable();

	m_StartPositionRandomArray.DeleteAllData();
	LoadStartPositionRandomTable();

	m_ItemExchangeArray.DeleteAllData();
	LoadItemExchangeTable();

	m_ItemUpgradeArray.DeleteAllData();
	LoadItemUpgradeTable();

	m_EventTriggerArray.DeleteAllData();
	LoadEventTriggerTable();

	m_ServerResourceArray.DeleteAllData();
	LoadServerResourceTable();

	m_MonsterChallengeArray.DeleteAllData();
	LoadMonsterChallengeTable();

	m_MonsterChallengeSummonListArray.DeleteAllData();
	LoadMonsterChallengeSummonListTable();

	m_MonsterRespawnListArray.DeleteAllData();
	LoadMonsterRespawnListTable();

	m_MonsterRespawnListInformationArray.DeleteAllData();
	LoadMonsterRespawnListInformationTable();

	ReloadKnightAndUserRanks();

	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleReloadMagicsCommand)
{
	m_IsMagicTableInUpdateProcess = true;
	m_MagictableArray.DeleteAllData();
	m_Magictype1Array.DeleteAllData();
	m_Magictype2Array.DeleteAllData();
	m_Magictype3Array.DeleteAllData();
	m_Magictype4Array.DeleteAllData();
	m_Magictype5Array.DeleteAllData();
	m_Magictype6Array.DeleteAllData();
	m_Magictype8Array.DeleteAllData();
	m_Magictype9Array.DeleteAllData();
	LoadMagicTable();
	LoadMagicType1();
	LoadMagicType2();
	LoadMagicType3();
	LoadMagicType4();
	LoadMagicType5();
	LoadMagicType6();
	LoadMagicType7();
	LoadMagicType8();
	LoadMagicType9();
	m_IsMagicTableInUpdateProcess = false;

	return true;
}


COMMAND_HANDLER(CGameServerDlg::HandleReloadQuestCommand)
{
	m_QuestHelperArray.DeleteAllData();
	LoadQuestHelperTable();
	m_QuestMonsterArray.DeleteAllData();
	LoadQuestMonsterTable();
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleReloadRanksCommand)
{
	ReloadKnightAndUserRanks();
>>>>>>> koserver2
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleCountCommand)
{
	uint16 count = 0;
<<<<<<< HEAD
	SessionMap & sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
=======
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
>>>>>>> koserver2
	foreach (itr, sessMap)
	{
		if (TO_USER(itr->second)->isInGame())
			count++;
	}
<<<<<<< HEAD
	g_pMain->m_socketMgr.ReleaseLock();
=======
>>>>>>> koserver2

	printf("Online User Count : %d\n",count);
	return true;
}

void CGameServerDlg::SendFormattedResource(uint32 nResourceID, uint8 byNation, bool bIsNotice, ...)
{
	_SERVER_RESOURCE *pResource = m_ServerResourceArray.GetData(nResourceID);
	if (pResource == nullptr)
		return;

	string buffer;
	va_list ap;
	va_start(ap, bIsNotice);
	_string_format(pResource->strResource, &buffer, ap);
	va_end(ap);

	if (bIsNotice)
		SendNotice(buffer.c_str(), byNation);
	else
		SendAnnouncement(buffer.c_str(), byNation);
}
