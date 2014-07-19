#include "stdafx.h"
#include "Map.h"
#include "KnightsManager.h"
#include "KingSystem.h"

using std::string;

void CUser::ItemRepair(Packet & pkt)
{
	if (isDead())
		return;

	Packet result(WIZ_ITEM_REPAIR);
	uint32 money, itemid;
	uint16 durability, quantity, sNpcID;
	_ITEM_TABLE* pTable = nullptr;
	CNpc *pNpc = nullptr;
	uint8 sPos, sSlot;

	pkt >> sPos >> sSlot >> sNpcID >> itemid;
	if (sPos == 1 ) {	// SLOT
		if (sSlot >= SLOT_MAX) goto fail_return;
		if (m_sItemArray[sSlot].nNum != itemid) goto fail_return;
	}
	else if (sPos == 2 ) {	// INVEN
		if (sSlot >= HAVE_MAX) goto fail_return;
		if (m_sItemArray[SLOT_MAX+sSlot].nNum != itemid) goto fail_return;
	}

	pNpc = g_pMain->GetNpcPtr(sNpcID);
	if (pNpc == nullptr || !isInRange(pNpc, MAX_NPC_RANGE))
		return;

	if (pNpc->GetType() == NPC_TINKER || pNpc->GetType() == NPC_MERCHANT)
	{
		pTable = g_pMain->GetItemPtr( itemid );
		if (pTable == nullptr
			|| pTable->m_iSellPrice == SellTypeNoRepairs) 
			goto fail_return;

		durability = pTable->m_sDuration;
		if( durability == 1 ) goto fail_return;
		if( sPos == 1 )
			quantity = pTable->m_sDuration - m_sItemArray[sSlot].sDuration;
		else if( sPos == 2 ) 
			quantity = pTable->m_sDuration - m_sItemArray[SLOT_MAX+sSlot].sDuration;

		money = (unsigned int)((((pTable->m_iBuyPrice-10) / 10000.0f) + pow((float)pTable->m_iBuyPrice, 0.75f)) * quantity / (double)durability);

		if (m_bPremiumType != 0)
			money = money * GetPremiumProperty(PremiumRepairDiscountPercent) / 100;

		if (!GoldLose(money, false))
			goto fail_return;

		if (sPos == 1)
			m_sItemArray[sSlot].sDuration = durability;
		else if( sPos == 2 )
			m_sItemArray[SLOT_MAX+sSlot].sDuration = durability;

		result << uint8(1) << GetCoins();
		Send(&result);
		return;
	}

fail_return:
	result << uint8(0) << GetCoins();
	Send(&result);
}

void CUser::ClientEvent(uint16 sNpcID)
{
	// Ensure AI's loaded
	if (!g_pMain->m_bPointCheckFlag
		|| isDead())
		return;

	int32 iEventID = 0;
	CNpc *pNpc = g_pMain->GetNpcPtr(sNpcID);
	if (pNpc == nullptr
		|| !isInRange(pNpc, MAX_NPC_RANGE))
		return;

	m_sEventNid = sNpcID;
	m_sEventSid = pNpc->GetProtoID(); // For convenience purposes with Lua scripts.

	// Aww.
	if (pNpc->GetType() == NPC_KISS)
	{
		KissUser();
		return;
	}

	FastGuard lock(g_pMain->m_questNpcLock);
	QuestNpcList::iterator itr = g_pMain->m_QuestNpcList.find(pNpc->GetProtoID());
	if (itr == g_pMain->m_QuestNpcList.end())
		return;

	QuestHelperList & pList = itr->second;
	_QUEST_HELPER * pHelper = nullptr;
	foreach (itr, pList)
	{
		if ((*itr) == nullptr
			|| (*itr)->sEventDataIndex
			|| (*itr)->bEventStatus
			|| ((*itr)->bNation != 3 && (*itr)->bNation != GetNation())
			|| ((*itr)->bClass != 5 && !JobGroupCheck((*itr)->bClass)))
			continue;

		pHelper = (*itr);
		break;
	}

	if (pHelper == nullptr)
		return;

	QuestV2RunEvent(pHelper, pHelper->nEventTriggerIndex);
}

void CUser::KissUser()
{
	Packet result(WIZ_KISS);
	result << uint32(GetID()) << m_sEventNid;
	GiveItem(910014000); // aw, you got a 'Kiss'. How literal.
	SendToRegion(&result);
}

void CUser::ClassChange(Packet & pkt, bool bFromClient /*= true */)
{
	Packet result(WIZ_CLASS_CHANGE);
	bool bSuccess = false;
	uint8 opcode = pkt.read<uint8>();
	if (opcode == CLASS_CHANGE_REQ)	
	{
		ClassChangeReq();
		return;
	}
	else if (opcode == ALL_POINT_CHANGE)	
	{
		AllPointChange();
		return;
	}
	else if (opcode == ALL_SKILLPT_CHANGE)	
	{
		AllSkillPointChange();
		return;
	}
	else if (opcode == CHANGE_MONEY_REQ)	
	{
		uint8 sub_type = pkt.read<uint8>(); // type is irrelevant
		uint32 money = (uint32)pow((GetLevel() * 2.0f), 3.4f);

		if (GetLevel() < 30)	
			money = (uint32)(money * 0.4f);
		else if (GetLevel() >= 60)
			money = (uint32)(money * 1.5f);

		// If nation discounts are enabled (1), and this nation has won the last war, get it half price.
		// If global discounts are enabled (2), everyone can get it for half price.
		if ((g_pMain->m_sDiscount == 1 && g_pMain->m_byOldVictory == GetNation())
			|| g_pMain->m_sDiscount == 2)
			money /= 2;

		result << uint8(CHANGE_MONEY_REQ) << money;
		Send(&result);
		return;
	}
	// If this packet was sent from the client, ignore it.
	else if (bFromClient)
		return;

	uint8 classcode = pkt.read<uint8>();
	switch (m_sClass)
	{
	case KARUWARRIOR:
		if( classcode == BERSERKER || classcode == GUARDIAN )
			bSuccess = true;
		break;
	case KARUROGUE:
		if( classcode == HUNTER || classcode == PENETRATOR )
			bSuccess = true;
		break;
	case KARUWIZARD:
		if( classcode == SORSERER || classcode == NECROMANCER )
			bSuccess = true;
		break;
	case KARUPRIEST:
		if( classcode == SHAMAN || classcode == DARKPRIEST )
			bSuccess = true;
		break;
	case ELMORWARRRIOR:
		if( classcode == BLADE || classcode == PROTECTOR )
			bSuccess = true;
		break;
	case ELMOROGUE:
		if( classcode == RANGER || classcode == ASSASSIN )
			bSuccess = true;
		break;
	case ELMOWIZARD:
		if( classcode == MAGE || classcode == ENCHANTER )
			bSuccess = true;
		break;
	case ELMOPRIEST:
		if( classcode == CLERIC || classcode == DRUID )
			bSuccess = true;
		break;
	case BERSERKER:
		if (classcode == GUARDIAN)
		    bSuccess = true;
		break;
	case HUNTER:
		if (classcode == PENETRATOR)
			bSuccess = true;
		break;
	case SORSERER:
		if (classcode == NECROMANCER)
			bSuccess = true;
		break;
	case SHAMAN:
		if (classcode == DARKPRIEST)
			bSuccess = true;
		break;
	case BLADE:
		if (classcode == PROTECTOR)
			bSuccess = true;
		break;
	case RANGER:
		if (classcode == ASSASSIN)
			bSuccess = true;
		break;
	case MAGE:
		if (classcode == ENCHANTER)
			bSuccess = true;
		break;
	case CLERIC:
		if (classcode == DRUID)
			bSuccess = true;
		break;
	}

	// Not allowed this job change
	if (!bSuccess)
	{
		result << uint8(CLASS_CHANGE_RESULT) << uint8(0);
		Send(&result);
		return;
	}

	m_sClass = classcode;
	if (isInParty())
	{
		// TODO: Move this somewhere better.
		result.SetOpcode(WIZ_PARTY);
		result << uint8(PARTY_CLASSCHANGE) << GetSocketID() << uint16(classcode);
		g_pMain->Send_PartyMember(GetPartyID(), &result);
	}
}

void CUser::RecvSelectMsg(Packet & pkt)	// Receive menu reply from client.
{
	uint8 bMenuID = pkt.read<uint8>();
	string szLuaFilename;
	int8 bySelectedReward = -1;
	pkt.SByte();
	pkt >> szLuaFilename >> bySelectedReward;

	if (!AttemptSelectMsg(bMenuID, bySelectedReward))
		memset(&m_iSelMsgEvent, -1, sizeof(m_iSelMsgEvent));
}

bool CUser::AttemptSelectMsg(uint8 bMenuID, int8 bySelectedReward)
{
	_QUEST_HELPER * pHelper = nullptr;
	if (bMenuID >= MAX_MESSAGE_EVENT
		|| isDead()
		|| m_nQuestHelperID == 0)
		return false;

	// Get the event number that needs to be processed next.
	int32 selectedEvent = m_iSelMsgEvent[bMenuID];
	if (selectedEvent < 0
		|| (pHelper = g_pMain->m_QuestHelperArray.GetData(m_nQuestHelperID)) == nullptr
		|| !QuestV2RunEvent(pHelper, selectedEvent, bySelectedReward))
		return false;

	return true;
}

void CUser::SendSay(int32 nTextID[8])
{
	Packet result(WIZ_NPC_SAY);
	result << int32(-1) << int32(-1);
	foreach_array_n(i, nTextID, 8)
		result << nTextID[i];
	Send(&result);
}

void CUser::SelectMsg(uint8 bFlag, int32 nQuestID, int32 menuHeaderText, 
					  int32 menuButtonText[MAX_MESSAGE_EVENT], int32 menuButtonEvents[MAX_MESSAGE_EVENT])
{
	_QUEST_HELPER * pHelper = g_pMain->m_QuestHelperArray.GetData(m_nQuestHelperID);
	if (pHelper == nullptr)
		return;

	// Send the menu to the client
	Packet result(WIZ_SELECT_MSG);
	result.SByte();

	result << m_sEventSid << bFlag << nQuestID << menuHeaderText;
	foreach_array_n(i, menuButtonText, MAX_MESSAGE_EVENT)
		result << menuButtonText[i];
	result << pHelper->strLuaFilename;
	Send(&result);

	// and store the corresponding event IDs.
	memcpy(&m_iSelMsgEvent, menuButtonEvents, sizeof(int32) * MAX_MESSAGE_EVENT);
}

void CUser::NpcEvent(Packet & pkt)
{
	// Ensure AI is loaded first
	if (!g_pMain->m_bPointCheckFlag
		|| isDead())
		return;	

	Packet result;
	uint8 bUnknown = pkt.read<uint8>();
	uint16 sNpcID = pkt.read<uint16>();
	int32 nQuestID = pkt.read<int32>();

	CNpc *pNpc = g_pMain->GetNpcPtr(sNpcID);
	if (pNpc == nullptr
		|| !isInRange(pNpc, MAX_NPC_RANGE))
		return;

	switch (pNpc->GetType())
	{
	case NPC_MERCHANT:
	case NPC_TINKER:
		result.SetOpcode(pNpc->GetType() == NPC_MERCHANT ? WIZ_TRADE_NPC : WIZ_REPAIR_NPC);
		result << pNpc->m_iSellingGroup;
		Send(&result);
		break;

		/*case NPC_MENU:
		result.SetOpcode(WIZ_QUEST);
		result	<< uint8(7) << uint16(SendNPCMenu(pNpc->m_sSid))
		<< uint16(0) << uint16(pNpc->m_sSid);
		Send(&result);
		break; */

	case NPC_MARK:
		result.SetOpcode(WIZ_KNIGHTS_PROCESS);
		result << uint8(KNIGHTS_CAPE_NPC);
		Send(&result);
		break;

	case NPC_RENTAL:
		result.SetOpcode(WIZ_RENTAL);
		result	<< uint8(RENTAL_NPC) 
			<< uint16(1) // 1 = enabled, -1 = disabled 
			<< pNpc->m_iSellingGroup;
		Send(&result);
		break;

	case NPC_ELECTION:
	case NPC_TREASURY:
		{
			CKingSystem * pKingSystem = g_pMain->m_KingSystemArray.GetData(GetNation());
			result.SetOpcode(WIZ_KING);
			if (pNpc->GetType() == NPC_ELECTION)
			{
				// Ensure this still works as per official without a row in the table.
				string strKingName = (pKingSystem == nullptr ? "" : pKingSystem->m_strKingName);
				result.SByte();
				result	<< uint8(KING_NPC) << strKingName;
			}
			else
			{
				// Ensure this still works as per official without a row in the table.
				uint32 nTribute = (pKingSystem == nullptr ? 0 : pKingSystem->m_nTribute + pKingSystem->m_nTerritoryTax);
				uint32 nTreasury = (pKingSystem == nullptr ? 0 : pKingSystem->m_nNationalTreasury);
				result	<< uint8(KING_TAX) << uint8(1) // success
					<< uint16(isKing() ? 1 : 2) // 1 enables king-specific stuff (e.g. scepter), 2 is normal user stuff
					<< nTribute << nTreasury;
			}
			Send(&result);
		} break;

	case NPC_CAPTAIN:
		result.SetOpcode(WIZ_CLASS_CHANGE);
		result << uint8(CLASS_CHANGE_REQ);
		Send(&result);
		break;

	case NPC_WAREHOUSE:
		result.SetOpcode(WIZ_WAREHOUSE);
		result << uint8(WAREHOUSE_REQ);
		Send(&result);
		break;

	case NPC_CHAOTIC_GENERATOR:
	case NPC_CHAOTIC_GENERATOR2:
		SendAnvilRequest(sNpcID, ITEM_BIFROST_REQ);
		break;

	case NPC_CLAN: // this HAS to go.
		result << uint16(0); // page 0
		CKnightsManager::AllKnightsList(this, result);

	default:
		ClientEvent(sNpcID);
	}
}

// NPC shops
void CUser::ItemTrade(Packet & pkt)
{
	Packet result(WIZ_ITEM_TRADE);
	uint32 transactionPrice;
	int itemid = 0, money = 0, group = 0;
	uint16 npcid;
	uint16 count, real_count = 0;
	_ITEM_TABLE* pTable = nullptr;
	CNpc* pNpc = nullptr;
	uint8 type, pos, destpos, errorCode = 1;
	bool bSuccess = true;

	if (isDead())
	{
		errorCode = 1;
		goto send_packet;
	}

	pkt >> type;
	// Buy == 1, Sell == 2
	if (type == 1 || type == 2)
	{
		pkt >> group >> npcid;
		if (!g_pMain->m_bPointCheckFlag
			|| (pNpc = g_pMain->GetNpcPtr(npcid)) == nullptr
			|| (pNpc->GetType() != NPC_MERCHANT && pNpc->GetType() != NPC_TINKER)
			|| pNpc->m_iSellingGroup != group
			|| !isInRange(pNpc, MAX_NPC_RANGE))
			goto fail_return;
	}

	pkt >> itemid >> pos;

	if (type == 3) 	// Move only (this is so useless mgame -- why not just handle it with the CUser::ItemMove(). Gah.)
		pkt >> destpos;
	else
		pkt >> count;

	// Moving an item in the inventory
	if (type == 3)
	{
		if (pos >= HAVE_MAX || destpos >= HAVE_MAX
			|| itemid != m_sItemArray[SLOT_MAX+pos].nNum)
		{
			errorCode = 4;
			goto send_packet;
		}

		short duration = m_sItemArray[SLOT_MAX+pos].sDuration;
		short itemcount = m_sItemArray[SLOT_MAX+pos].sCount;
		m_sItemArray[SLOT_MAX+pos].nNum = m_sItemArray[SLOT_MAX+destpos].nNum;
		m_sItemArray[SLOT_MAX+pos].sDuration = m_sItemArray[SLOT_MAX+destpos].sDuration;
		m_sItemArray[SLOT_MAX+pos].sCount = m_sItemArray[SLOT_MAX+destpos].sCount;
		m_sItemArray[SLOT_MAX+destpos].nNum = itemid;
		m_sItemArray[SLOT_MAX+destpos].sDuration = duration;
		m_sItemArray[SLOT_MAX+destpos].sCount = itemcount;

		result << uint8(3);
		Send(&result);
		return;
	}

	if (isTrading()
		|| (pTable = g_pMain->GetItemPtr(itemid)) == nullptr
		|| (type == 2 // if we're selling an item...
		&& (itemid >= ITEM_NO_TRADE // Cannot be traded, sold or stored.
		|| pTable->m_bRace == RACE_UNTRADEABLE))) // Cannot be traded or sold.
		goto fail_return;

	if (pos >= HAVE_MAX
		|| count <= 0 || count > MAX_ITEM_COUNT)
	{
		errorCode = 2;
		goto fail_return;
	}

	// Buying from an NPC
	if (type == 1)
	{	
		if (m_sItemArray[SLOT_MAX+pos].nNum != 0)
		{
			if (m_sItemArray[SLOT_MAX+pos].nNum != itemid)
			{
				errorCode = 2;
				goto fail_return;
			}

			if (!pTable->m_bCountable || count <= 0)
			{
				errorCode = 2;
				goto fail_return;
			}

			if (pTable->m_bCountable 
				&& (count + m_sItemArray[SLOT_MAX+pos].sCount) > MAX_ITEM_COUNT)
			{
				errorCode = 4;
				goto fail_return;				
			}
		}

		transactionPrice = ((uint32)pTable->m_iBuyPrice * count);
		if (!hasCoins(transactionPrice))
		{
			errorCode = 3;
			goto fail_return;
		}

		if (((pTable->m_sWeight * count) + m_sItemWeight) > m_sMaxWeight)
		{
			errorCode = 4;
			goto fail_return;
		}

		m_sItemArray[SLOT_MAX+pos].nNum = itemid;
		m_sItemArray[SLOT_MAX+pos].sDuration = pTable->m_sDuration;
		m_sItemArray[SLOT_MAX+pos].sCount += count;

		m_iGold -= transactionPrice;

		if (!pTable->m_bCountable)
			m_sItemArray[SLOT_MAX+pos].nSerialNum = g_pMain->GenerateItemSerial();

		SetUserAbility(false);
		SendItemWeight();
	}
	// Selling an item to an NPC
	else
	{
		_ITEM_DATA *pItem = &m_sItemArray[SLOT_MAX+pos];
		if (pItem->nNum != itemid
			|| pItem->isSealed() // need to check the error codes for these
			|| pItem->isRented())
		{
			errorCode = 2;
			goto fail_return;
		}

		if (pItem->sCount < count)
		{
			errorCode = 3;
			goto fail_return;
		}

		short oldDurability = pItem->sDuration;
		if (pTable->m_iSellPrice != SellTypeFullPrice)
			transactionPrice = ((pTable->m_iBuyPrice / 6) * count); // /6 is normal, /4 for prem/discount
		else
			transactionPrice = (pTable->m_iBuyPrice * count);

		GoldGain(transactionPrice, false);

		if (count >= pItem->sCount)
			memset(pItem, 0, sizeof(_ITEM_DATA));
		else
			pItem->sCount -= count;

		SetUserAbility(false);
		SendItemWeight();
	}

	goto send_packet;

fail_return:
	bSuccess = false;

send_packet:
	result << bSuccess;
	if (!bSuccess)
		result << errorCode;
	else 
		result << pTable->m_bSellingGroup << m_iGold << transactionPrice; // price bought or sold for
	Send(&result);
}

/**
* @brief	Handles the name change response packet
* 			containing the specified new name.
*
* @param	pkt	The packet.
*/
void CUser::HandleNameChange(Packet & pkt)
{
	uint8 opcode;
	pkt >> opcode;

	switch (opcode)
	{
	case NameChangePlayerRequest:
		HandlePlayerNameChange(pkt);
		break;
	}
}

/**
* @brief	Handles the character name change response packet
* 			containing the specified new character's name.
*
* @param	pkt	The packet.
*/
void CUser::HandlePlayerNameChange(Packet & pkt)
{
	NameChangeOpcode response = NameChangeSuccess;
	string strUserID;
	pkt >> strUserID;

	if (strUserID.empty() || strUserID.length() > MAX_ID_SIZE)
		response = NameChangeInvalidName;
	else if (isInClan())
		response = NameChangeInClan;
	else if (isKing())
		response = NameChangeKing; 

	if (response != NameChangeSuccess)
	{
		SendNameChange(response);
		return;
	}

	// Ensure we have the scroll before handling this request.
	if (!CheckExistItem(ITEM_SCROLL_OF_IDENTITY))
		return;

	Packet result(WIZ_NAME_CHANGE, uint8(NameChangePlayerRequest));
	result << strUserID;
	g_pMain->AddDatabaseRequest(result, this);
}

/**
* @brief	Sends a name change packet.
*
* @param	opcode	Name change packet opcode.
* 					NameChangeShowDialog shows the dialog where you can set your name.
* 					NameChangeSuccess confirms the name was changed.
* 					NameChangeInvalidName throws an error reporting the name is invalid.
* 					NameChangeInClan throws an error reporting the user's still in a clan (and needs to leave).
*					NameChangeIsKing if the user is king
*/
void CUser::SendNameChange(NameChangeOpcode opcode /*= NameChangeShowDialog*/)
{
	Packet result(WIZ_NAME_CHANGE, uint8(opcode));
	Send(&result);
}

void CUser::HandleCapeChange(Packet & pkt)
{
	Packet result(WIZ_CAPE);
	CKnights *pKnights = nullptr;
	_KNIGHTS_CAPE *pCape = nullptr;
	uint32 nReqClanPoints = 0, nReqCoins = 0;
	int16 sErrorCode = 0, sCapeID;
	uint8 r, g, b;
	bool bApplyingPaint = false;

	pkt >> sCapeID >> r >> g >> b;

	// If we're not a clan leader, what are we doing changing the cape?
	if (!isClanLeader()
		|| isDead())
	{
		sErrorCode = -1;
		goto fail_return;
	}

	// Does the clan exist?
	if ((pKnights = g_pMain->GetClanPtr(GetClanID())) == nullptr)
	{
		sErrorCode = -2;
		goto fail_return;
	}

	// Make sure we're promoted
	if (!pKnights->isPromoted()
		// and that if we're in an alliance, we're the primary clan in the alliance.
			|| (pKnights->isInAlliance() && !pKnights->isAllianceLeader()))
	{
		sErrorCode = -1;
		goto fail_return;
	}

	if (sCapeID >= 0)
	{
		// Does this cape type exist?
		if ((pCape = g_pMain->m_KnightsCapeArray.GetData(sCapeID)) == nullptr)
		{
			sErrorCode = -5;
			goto fail_return;
		}

		// Is our clan allowed to use this cape?
		if ((pCape->byGrade && pKnights->m_byGrade > pCape->byGrade)
			// not sure if this should use another error, need to confirm
				|| pKnights->m_byFlag < pCape->byRanking)
		{
			sErrorCode = -6;
			goto fail_return;
		}

		// NOTE: Error code -8 is for nDuration
		// It applies if we do not have the required item ('nDuration', awful name).
		// Since no capes seem to use it, we'll ignore it...

		// Can we even afford this cape?
		if (!hasCoins(pCape->nReqCoins))
		{
			sErrorCode = -7;
			goto fail_return;
		}

		nReqCoins = pCape->nReqCoins;
		nReqClanPoints = pCape->nReqClanPoints;
	}

	// These are 0 when not used
	if (r != 0 || g != 0 || b != 0)
	{
		// To use paint, the clan needs to be accredited
		if (pKnights->m_byFlag < ClanTypeAccredited5)
		{
			sErrorCode = -1; // need to find the error code for this
			goto fail_return;
		}

		bApplyingPaint = true;
		nReqClanPoints += 1000; // does this need tweaking per clan rank?
	}

	// If this requires clan points, does our clan have enough?
	if (pKnights->m_nClanPointFund < nReqClanPoints)
	{
		// this error may not be correct
		sErrorCode = -7;
		goto fail_return;
	}

	if (nReqCoins > 0)
		GoldLose(nReqCoins);

	if (nReqClanPoints)
	{
		pKnights->m_nClanPointFund -= nReqClanPoints;
		pKnights->UpdateClanFund();
	}

	// Are we changing the cape?
	if (sCapeID >= 0)
		pKnights->m_sCape = sCapeID;

	// Are we applying paint?
	if (bApplyingPaint)
	{
		pKnights->m_bCapeR = r;
		pKnights->m_bCapeG = g;
		pKnights->m_bCapeB = b;
	}

	result	<< uint16(1) // success
		<< pKnights->GetAllianceID()
		<< pKnights->GetID()
		<< pKnights->GetCapeID()
		<< pKnights->m_bCapeR << pKnights->m_bCapeG << pKnights->m_bCapeB
		<< uint8(0);

	Send(&result);

	// TODO:
	// When we implement alliances, this should send to the alliance
	// if the clan is part of one. Also, their capes should be updated.
	pKnights->SendUpdate();

	// TODO: Send to other servers via UDP.

	// Now tell Aujard to save (we don't particularly care whether it was able to do so or not).
	result.Initialize(WIZ_CAPE);
	result	<< pKnights->GetID() << pKnights->GetCapeID()
		<< pKnights->m_bCapeR << pKnights->m_bCapeG << pKnights->m_bCapeB;
	g_pMain->AddDatabaseRequest(result, this);
	return;

fail_return:
	result << sErrorCode;
	Send(&result);
}
