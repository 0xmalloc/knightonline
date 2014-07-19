﻿#include "stdafx.h"
#include "DBAgent.h"

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
		{ "pause",				&CGameServerDlg::HandlePauseCommand,			"Prevents users from connecting to the server" },
		{ "resume",				&CGameServerDlg::HandleResumeCommand,			"Allows users to resume connecting to the server" },
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
		{ "count",				&CGameServerDlg::HandleCountCommand,			"Get online user count." },
		{ "permitconnect",		&CGameServerDlg::HandlePermitConnectCommand,	"Player unban" },
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
		{ "open1",				&CUser::HandleWar1OpenCommand,					"Opens war zone 1" },
		{ "open2",				&CUser::HandleWar2OpenCommand,					"Opens war zone 2" },
		{ "open3",				&CUser::HandleWar3OpenCommand,					"Opens war zone 3" },
		{ "open4",				&CUser::HandleWar4OpenCommand,					"Opens war zone 4" },
		{ "open5",				&CUser::HandleWar5OpenCommand,					"Opens war zone 5" },
		{ "open6",				&CUser::HandleWar6OpenCommand,					"Opens war zone 6" },
		{ "captain",			&CUser::HandleCaptainCommand,					"Sets the captains/commanders for the war" },
		{ "snowopen",			&CUser::HandleSnowWarOpenCommand,				"Opens the snow war zone" },
		{ "close",				&CUser::HandleWarCloseCommand,					"Closes the active war zone" },
		{ "down",				&CUser::HandleShutdownCommand,					"Shuts down the server" },
		{ "np_change",			&CUser::HandleLoyaltyChangeCommand,				"Change a player an loyalty" },
		{ "exp_change",			&CUser::HandleExpChangeCommand,					"Change a player an exp" },
		{ "gold_change",		&CUser::HandleGoldChangeCommand,				"Change a player an gold" },
		{ "exp_add",			&CUser::HandleExpAddCommand,					"Sets the server-wide XP event. If bonusPercent is set to 0, the event is ended. Arguments: bonusPercent" },
		{ "money_add",			&CUser::HandleMoneyAddCommand,					"Sets the server-wide coin event. If bonusPercent is set to 0, the event is ended. Arguments: bonusPercent" },
		{ "permitconnect",		&CUser::HandlePermitConnectCommand,				"Player unban" },
		{ "tp_all",				&CUser::HandleTeleportAllCommand,				"Players send to home zone." },
		{ "summonknights",		&CUser::HandleKnightsSummonCommand,				"Teleport the clan users. Arguments: clan name" },
	};

	init_command_table(CUser, commandTable, s_commandTable);
}

void CUser::CleanupChatCommands() { free_command_table(s_commandTable); }

void CUser::Chat(Packet & pkt)
{
	Packet result;
	uint16 sessID;
	uint8 type = pkt.read<uint8>(), bOutType = type, seekingPartyOptions, bNation;
	string chatstr, finalstr, strSender, * strMessage;

	bool isAnnouncement = false;

	if (isMuted())
		return;	

	pkt >> chatstr;
	if (chatstr.empty() || chatstr.size() > 128)
		return;

	// Process GM commands
	if (isGM() && ProcessChatCommand(chatstr))
		return;

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
		break;

	case PRIVATE_CHAT:
		{
			// TODO : Kontrol Edilecek.
			CUser *pUser = g_pMain->GetUserPtr(m_sPrivateChatUser);
			if (pUser == nullptr || !pUser->isInGame()) 
				return;
		}
	case COMMAND_PM_CHAT:
		{
			if (type == COMMAND_PM_CHAT && GetFame() != COMMAND_CAPTAIN)
				return;

			CUser *pUser = g_pMain->GetUserPtr(m_sPrivateChatUser);
			if (pUser != nullptr) 
				pUser->Send(&result);
		} break;

	case PARTY_CHAT:
		if (isInParty())
			g_pMain->Send_PartyMember(GetPartyID(), &result);
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
		break;

	case KNIGHTS_CHAT:
		if (isInClan())
			g_pMain->Send_KnightsMember(GetClanID(), &result);
		break;
	case CLAN_NOTICE:
		if (isInClan() 
			&& isClanLeader())
		{
			CKnights * pKnights = g_pMain->GetClanPtr(GetClanID());
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
			g_pMain->Send_CommandChat(&result, m_bNation, this);
		break;
	case MERCHANT_CHAT:
		if (isMerchanting())
			SendToRegion(&result);
		break;
	case ALLIANCE_CHAT:
		if (isInClan())
		{
			CKnights *pKnights = g_pMain->GetClanPtr(GetClanID());
			if (pKnights != nullptr && pKnights->isInAlliance())
				g_pMain->Send_KnightsAlliance(pKnights->GetAllianceID(), &result);
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
		{
			result << int16(0); 
		}
		else if (pUser->isBlockingPrivateChat())
		{
			result << int16(-1);
		}
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

	SendToZone(&result);
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
	return true;
}

COMMAND_HANDLER(CUser::HandleGiveItemCommand)
{
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
		return true;
	}

	uint16 sCount = 1;
	if (!vargs.empty())
		sCount = atoi(vargs.front().c_str());


	if (!pUser->GiveItem(nItemID, sCount))
	{
		// send error message saying the item couldn't be added
	}

	return true;
}

COMMAND_HANDLER(CUser::HandleZoneChangeCommand)
{
	if (vargs.empty())
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +zonechange ZoneNumber");
		return true;
	}

	// Behave as in official (we'll fix this later)
	int nZoneID = atoi(vargs.front().c_str());
	ZoneChange(nZoneID, m_curx, m_curz);
	return true;
}

COMMAND_HANDLER(CUser::HandleMonsterSummonCommand)
{
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

COMMAND_HANDLER(CUser::HandleWar1OpenCommand) { return g_pMain->HandleWar1OpenCommand(vargs, args, description); }
COMMAND_HANDLER(CGameServerDlg::HandleWar1OpenCommand)
{
	BattleZoneOpen(BATTLEZONE_OPEN, 1);
	return true;
}

COMMAND_HANDLER(CUser::HandleWar2OpenCommand) { return g_pMain->HandleWar2OpenCommand(vargs, args, description); }
COMMAND_HANDLER(CGameServerDlg::HandleWar2OpenCommand)
{
	BattleZoneOpen(BATTLEZONE_OPEN, 2);
	return true;
}

COMMAND_HANDLER(CUser::HandleWar3OpenCommand) { return g_pMain->HandleWar3OpenCommand(vargs, args, description); }
COMMAND_HANDLER(CGameServerDlg::HandleWar3OpenCommand)
{
	BattleZoneOpen(BATTLEZONE_OPEN, 3);
	return true;
}

COMMAND_HANDLER(CUser::HandleWar4OpenCommand) { return g_pMain->HandleWar4OpenCommand(vargs, args, description); }
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

COMMAND_HANDLER(CUser::HandleWar6OpenCommand) { return g_pMain->HandleWar6OpenCommand(vargs, args, description); }
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

COMMAND_HANDLER(CUser::HandleWarCloseCommand) { return g_pMain->HandleWarCloseCommand(vargs, args, description); }
COMMAND_HANDLER(CGameServerDlg::HandleWarCloseCommand)
{
	ResetBattleZone();
	m_byBanishFlag = true;
	return true;
}

COMMAND_HANDLER(CUser::HandleLoyaltyChangeCommand)
{
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

// Starts/stops the server XP event & sets its server-wide bonus.
COMMAND_HANDLER(CUser::HandleExpAddCommand)
{
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

COMMAND_HANDLER(CUser::HandlePermitConnectCommand) { return g_pMain->HandlePermitConnectCommand(vargs, args, description); }
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

	std::string sNoticeMessage = string_format("%s has been unbanned..!", strUserID.c_str());

	if (!sNoticeMessage.empty())
		g_pMain->SendNotice(sNoticeMessage.c_str(),Nation::ALL);

	return true;
}

COMMAND_HANDLER(CUser::HandleTeleportAllCommand)
{
	// Zone number
	if (vargs.size() < 1)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +tp_all ZoneNumber | +tp_all ZoneNumber TargetZoneNumber");
		return true;
	}

	int nZoneID;
	int nTargetZoneID;

	if (vargs.size() == 1)
		nZoneID = atoi(vargs.front().c_str());

	if (vargs.size() == 2)
	{
		nZoneID = atoi(vargs.front().c_str());
		vargs.pop_front();
		nTargetZoneID = atoi(vargs.front().c_str());
	}

	if (nZoneID > 0)
		g_pMain->KickOutZoneUsers(nZoneID);
	return true;
}

COMMAND_HANDLER(CUser::HandleKnightsSummonCommand)
{
	// Clan name
	if(vargs.empty())
	{
		// Send description
		g_pMain->SendHelpDescription(this, "Using Sample : +summonknights ClanName");
		return true;
	}

	CKnights * pKnights;
	foreach_stlmap(itr,g_pMain->m_KnightsArray)
	{
		if(itr->second->GetName() == vargs.front().c_str())
		{
		   pKnights = g_pMain->GetClanPtr(itr->first);
		   break;
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

COMMAND_HANDLER(CUser::HandleCaptainCommand) { return g_pMain->HandleCaptainCommand(vargs, args, description); }
COMMAND_HANDLER(CGameServerDlg::HandleCaptainCommand)
{
	m_KnightsRatingArray[KARUS_ARRAY].DeleteAllData();
	m_KnightsRatingArray[ELMORAD_ARRAY].DeleteAllData();
	LoadKnightsRankTable(true);
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
#if __VERSION >= 1453
	SessionMap & sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach (itr, sessMap)
	{
		CUser * pUser = TO_USER(itr->second);
		if (pUser->isInGame())
			pUser->SendNotice();
	}
	g_pMain->m_socketMgr.ReleaseLock();
#endif
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleReloadTablesCommand)
{
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
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleCountCommand)
{
	uint16 count = 0;
	SessionMap & sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach (itr, sessMap)
	{
		if (TO_USER(itr->second)->isInGame())
			count++;
	}
	g_pMain->m_socketMgr.ReleaseLock();

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
