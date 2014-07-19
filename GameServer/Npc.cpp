#include "stdafx.h"
#include "Map.h"
#include "MagicInstance.h"

CNpc::CNpc() : Unit(UnitNPC)
{
	Initialize();
}


CNpc::~CNpc()
{
}

/**
* @brief	Initializes this object.
*/
void CNpc::Initialize()
{
	Unit::Initialize();

	m_sSid = 0;
	m_sPid = 0;				// MONSTER(NPC) Picture ID
	m_sSize = 100;				// MONSTER(NPC) Size
	m_strName.clear();			// MONSTER(NPC) Name
	m_iMaxHP = 0;				// 최대 HP
	m_iHP = 0;					// 현재 HP
	//m_byState = 0;			// 몬스터 (NPC) 상태이상
	m_tNpcType = 0;				// NPC Type
	// 0 : Normal Monster
	// 1 : NPC
	// 2 : 각 입구,출구 NPC
	// 3 : 경비병
	m_iSellingGroup = 0;
	//	m_dwStepDelay = 0;		

	m_byDirection = 0;			// npc의 방향,,
	m_iWeapon_1 = 0;
	m_iWeapon_2 = 0;
	m_NpcState = NPC_LIVE;
	m_byGateOpen = true;
	m_byObjectType = NORMAL_OBJECT;

	m_byTrapNumber = 0;
}

/**
* @brief	Adds the NPC to the region.
*
* @param	new_region_x	The new region x coordinate.
* @param	new_region_z	The new region z coordinate.
*/
void CNpc::AddToRegion(int16 new_region_x, int16 new_region_z)
{
	GetRegion()->Remove(this);
	SetRegion(new_region_x, new_region_z);
	GetRegion()->Add(this);
}

/**
* @brief	Sends the movement packet for the NPC.
*
* @param	fPosX 	The position x coordinate.
* @param	fPosY 	The position y coordinate.
* @param	fPosZ 	The position z coordinate.
* @param	fSpeed	The speed.
*/
void CNpc::MoveResult(float fPosX, float fPosY, float fPosZ, float fSpeed)
{
	Packet result(WIZ_NPC_MOVE);

	SetPosition(fPosX, fPosY, fPosZ);
	RegisterRegion();

	result << GetID() << GetSPosX() << GetSPosZ() << GetSPosY() << uint16(fSpeed * 10);
	SendToRegion(&result);
}

/**
* @brief	Constructs an in/out packet for the NPC.
*
* @param	result	The packet buffer the constructed packet will be stored in.
* @param	bType 	The type (in or out).
*/
void CNpc::GetInOut(Packet & result, uint8 bType)
{
	result.Initialize(WIZ_NPC_INOUT);
	result << bType << GetID();
	if (bType != INOUT_OUT)
		GetNpcInfo(result);
}

/**
* @brief	Constructs and sends an in/out packet for the NPC.
*
* @param	bType	The type (in or out).
* @param	fX   	The x coordinate.
* @param	fZ   	The z coordinate.
* @param	fY   	The y coordinate.
*/
void CNpc::SendInOut(uint8 bType, float fX, float fZ, float fY)
{
	if (GetRegion() == nullptr)
	{
		SetRegion(GetNewRegionX(), GetNewRegionZ());
		if (GetRegion() == nullptr)
			return;
	}

	if (bType == INOUT_OUT)
	{
		GetRegion()->Remove(this);
	}
	else	
	{
		GetRegion()->Add(this);
		SetPosition(fX, fY, fZ);
	}

	Packet result;
	GetInOut(result, bType);
	SendToRegion(&result);
}

/**
* @brief	Gets NPC information for use in various NPC packets.
*
* @param	pkt	The packet the information will be stored in.
*/
void CNpc::GetNpcInfo(Packet & pkt)
{
	pkt << GetProtoID()
		<< uint8(isMonster() ? 1 : 2) // Monster = 1, NPC = 2 (need to use a better flag)
		<< m_sPid
		<< GetType()
		<< m_iSellingGroup
		<< m_sSize
		<< m_iWeapon_1 << m_iWeapon_2
		// Monsters require 0 regardless, otherwise they'll act as NPCs.
		<< uint8(isMonster() ? 0 : GetNation())
		<< GetLevel()
		<< GetSPosX() << GetSPosZ() << GetSPosY()
		<< uint32(isGateOpen())
		<< m_byObjectType
		<< uint16(0) << uint16(0) // unknown
		<< int16(m_byDirection);
}

/**
* @brief	Sends a gate status.
*
* @param	ObjectType  object type
* @param	bFlag  	The flag (open or shut).
* @param	bSendAI	true to update the AI server.
*/
void CNpc::SendGateFlag(uint8 objectType,uint8 bFlag /*= -1*/, bool bSendAI /*= true*/)
{
	Packet result(WIZ_OBJECT_EVENT, uint8(objectType));

	// If there's a flag to set, set it now.
	if (bFlag >= 0)
		m_byGateOpen = (bFlag == 1);

	// Tell everyone nearby our new status.
	result << uint8(1) << GetID() << m_byGateOpen;
	SendToRegion(&result);

	// Tell the AI server our new status
	if (bSendAI)
	{
		result.Initialize(AG_NPC_GATE_OPEN);
		result << GetID() << m_byGateOpen;
		Send_AIServer(&result);
	}
}

/**
* @brief	Changes an NPC's hitpoints.
*
* @param	amount   	The amount to adjust the HP by.
* @param	pAttacker	The attacker.
* @param	bSendToAI	true to update the AI server.
*/
void CNpc::HpChange(int amount, Unit *pAttacker /*= nullptr*/, bool bSendToAI /*= true*/) 
{
	uint16 tid = (pAttacker != nullptr ? pAttacker->GetID() : -1);

	// Implement damage/HP cap.
	if (amount < -MAX_DAMAGE)
		amount = -MAX_DAMAGE;
	else if (amount > MAX_DAMAGE)
		amount = MAX_DAMAGE;

	// Glorious copypasta.
	if (amount < 0 && -amount > m_iHP)
		m_iHP = 0;
	else if (amount >= 0 && m_iHP + amount > m_iMaxHP)
		m_iHP = m_iMaxHP;
	else
		m_iHP += amount;

	// NOTE: This will handle the death notification/looting.
	if (bSendToAI)
		SendHpChangeToAI(tid, amount);

	if (pAttacker != nullptr
		&& pAttacker->isPlayer())
		TO_USER(pAttacker)->SendTargetHP(0, GetID(), amount);
}

void CNpc::HpChangeMagic(int amount, Unit *pAttacker /*= nullptr*/, AttributeType attributeType /*= AttributeNone*/)
{
	uint16 tid = (pAttacker != nullptr ? pAttacker->GetID() : -1);

	// Implement damage/HP cap.
	if (amount < -MAX_DAMAGE)
		amount = -MAX_DAMAGE;
	else if (amount > MAX_DAMAGE)
		amount = MAX_DAMAGE;

	HpChange(amount, pAttacker, false);
	SendHpChangeToAI(tid, amount, attributeType);
}

void CNpc::SendHpChangeToAI(uint16 sTargetID, int amount, AttributeType attributeType /*= AttributeNone*/)
{
	Packet result(AG_NPC_HP_CHANGE);
	result << GetID() << sTargetID << m_iHP << amount << uint8(attributeType);
	Send_AIServer(&result);
}

/**
* @brief	Changes an NPC's mana.
*
* @param	amount	The amount to adjust the mana by.
*/
void CNpc::MSpChange(int amount)
{
#if 0 // TODO: Implement this
	// Glorious copypasta.
	// TODO: Make this behave unsigned.
	m_iMP += amount;
	if (m_iMP < 0)
		m_iMP = 0;
	else if (m_iMP > m_iMaxMP)
		m_iMP = m_iMaxMP;

	Packet result(AG_USER_SET_MP);
	result << GetID() << m_iMP;
	Send_AIServer(&result);
#endif
}

bool CNpc::CastSkill(Unit * pTarget, uint32 nSkillID)
{
	if (pTarget == nullptr)
		return false;

	MagicInstance instance;

	instance.bSendFail = false;
	instance.nSkillID = nSkillID;
	instance.sCasterID = GetID();
	instance.sTargetID = pTarget->GetID();

	instance.Run();

	return (instance.bSkillSuccessful);
}

float CNpc::GetRewardModifier(uint8 byLevel)
{
	int iLevelDifference = GetLevel() - byLevel;

	if (iLevelDifference <= -14)	
		return 0.2f;
	else if (iLevelDifference <= -8 && iLevelDifference >= -13)
		return 0.5f;
	else if (iLevelDifference <= -2 && iLevelDifference >= -7)
		return 0.8f;

	return 1.0f;
}

float CNpc::GetPartyRewardModifier(uint32 nPartyLevel, uint32 nPartyMembers)
{
	int iLevelDifference = GetLevel() - (nPartyLevel / nPartyMembers);

	if (iLevelDifference >= 8)
		return 2.0f;
	else if (iLevelDifference >= 5)
		return 1.5f;
	else if (iLevelDifference >= 2)
		return 1.2f;

	return 1.0f;
}

/**
* @brief	Executes the death action.
*
* @param	pKiller	The killer.
*/
void CNpc::OnDeath(Unit *pKiller)
{
	if (m_NpcState == NPC_DEAD)
		return;

	ASSERT(GetMap() != nullptr);
	ASSERT(GetRegion() != nullptr);

	m_NpcState = NPC_DEAD;

	if (m_byObjectType == SPECIAL_OBJECT)
	{
		_OBJECT_EVENT *pEvent = GetMap()->GetObjectEvent(GetProtoID());
		if (pEvent != nullptr)
			pEvent->byLife = 0;
	}

	Unit::OnDeath(pKiller);
	OnDeathProcess(pKiller);

	GetRegion()->Remove(TO_NPC(this));
	SetRegion();
}

/**
* @brief	Executes the death process.
*
* @param	pKiller	The killer.
*/
void CNpc::OnDeathProcess(Unit *pKiller)
{
	CUser * pUser = TO_USER(pKiller);

	if (TO_NPC(this) != nullptr && pUser != nullptr)
	{
		if (pUser->isPlayer())
		{
			if (!m_bMonster)
			{
				switch (m_tNpcType)
				{
				case NPC_BIFROST_MONUMENT:
					pUser->BifrostProcess(pUser);
					break;
				case NPC_PVP_MONUMENT:
					PVPMonumentProcess(pUser);
					break;
				default:
					break;
				}
			}
			else if (m_bMonster)
			{
				if (m_sSid == 700 || m_sSid == 750)
				{
					if (pUser->CheckExistEvent(STARTER_SEED_QUEST, 1))
						pUser->SaveEvent(STARTER_SEED_QUEST, 2);
				}
				else if (g_pMain->m_MonsterRespawnListArray.GetData(m_sSid) != nullptr) {
					if (pUser->isInPKZone() || GetZoneID() == ZONE_JURAD_MOUNTAIN)
						g_pMain->SpawnEventNpc(g_pMain->m_MonsterRespawnListArray.GetData(m_sSid)->sSid, true, GetZoneID(), GetX(), GetY(), GetZ(), g_pMain->m_MonsterRespawnListArray.GetData(m_sSid)->sCount);
				} else if (m_tNpcType == NPC_CHAOS_STONE && pUser->isInPKZone()) {
					ChaosStoneProcess(pUser,5);
				}
			}
		}
	}
}

/**
* @brief	Executes the death process.
*
* @param	pUser	The User.
* @param	MonsterCount The Respawn boss count.
*/
void CNpc::ChaosStoneProcess(CUser *pUser, uint16 MonsterCount)
{
	if (pUser == nullptr)
		return;

	g_pMain->SendNotice<CHAOS_STONE_ENEMY_NOTICE>("",GetZoneID(), Nation::ALL);

	std::vector<uint32> MonsterSpawned;
	std::vector<uint32> MonsterSpawnedFamily;
	bool bLoopBack = true;

	for (uint8 i = 0; i < MonsterCount;i++)
	{
		uint32 nMonsterNum = myrand(0, g_pMain->m_MonsterSummonListZoneArray.GetSize());

		_MONSTER_SUMMON_LIST_ZONE * pMonsterSummonListZone = g_pMain->m_MonsterSummonListZoneArray.GetData(nMonsterNum);

		if (pMonsterSummonListZone != nullptr)
		{
			if (pMonsterSummonListZone->ZoneID == GetZoneID())
			{
				if (std::find(MonsterSpawned.begin(),MonsterSpawned.end(),nMonsterNum) == MonsterSpawned.end())
				{
					if (std::find(MonsterSpawnedFamily.begin(),MonsterSpawnedFamily.end(),pMonsterSummonListZone->byFamily) == MonsterSpawnedFamily.end())
					{
						g_pMain->SpawnEventNpc(pMonsterSummonListZone->sSid, true,GetZoneID(), GetX(), GetY(), GetZ(), 1, CHAOS_STONE_MONSTER_RESPAWN_RADIUS);
						MonsterSpawned.push_back(nMonsterNum);
						MonsterSpawnedFamily.push_back(pMonsterSummonListZone->byFamily);
						bLoopBack = false;
					}
				}
			}
		}

		if (bLoopBack)
			i--;			
	}
}

/**
* @brief	Executes the pvp monument process.
*
* @param	pUser	The User.
*/
void CNpc::PVPMonumentProcess(CUser *pUser)
{
	if (pUser == nullptr)
		return;

	Packet result(WIZ_CHAT, uint8(MONUMENT_NOTICE));
	result << uint8(FORCE_CHAT) << pUser->GetNation() << pUser->GetName().c_str();
	g_pMain->Send_Zone(&result, GetZoneID(), nullptr, Nation::ALL);

	g_pMain->m_nPVPMonumentNation[GetZoneID()] = pUser->GetNation();
	g_pMain->NpcUpdate(m_sSid, m_bMonster, pUser->GetNation() == KARUS ? KARUS : ELMORAD, pUser->GetNation() == KARUS ? PVP_MONUMENT_KARUS_SPID : PVP_MONUMENT_ELMORAD_SPID);
}