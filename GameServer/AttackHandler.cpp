#include "stdafx.h"
#include "Map.h"

void CUser::Attack(Packet & pkt)
{
	int16 sid = -1, tid = -1, damage, delaytime, distance;	
	uint8 bType, bResult = 0;	
	Unit * pTarget = nullptr;

	pkt >> bType >> bResult >> tid >> delaytime >> distance;

	//	delaytime = delaytime / 100.0f;
	//	distance = distance / 10.0f;

	if (isIncapacitated())
		return;

	if (isInSafetyArea())
		return;

	if (m_bInvisibilityType != INVIS_NONE)
	{
		CMagicProcess::RemoveStealth(this, INVIS_DISPEL_ON_MOVE);
		CMagicProcess::RemoveStealth(this, INVIS_DISPEL_ON_ATTACK);
	}

	// If you're holding a weapon, do a client-based (ugh, do not trust!) delay check.
	_ITEM_TABLE *pTable = GetItemPrototype(RIGHTHAND);
	if (pTable != nullptr) 
	{
		if (delaytime < (pTable->m_sDelay + 10) // client adds 0.1 onto the interval (0.1 of 100 is 10)
			|| distance > pTable->m_sRange)
			return;	
	}
	// Empty handed.
	else if (delaytime < 100)
		return;			

	pTarget = g_pMain->GetUnitPtr(tid);
	bResult = ATTACK_FAIL;

	if (pTarget != nullptr 
		&& isInAttackRange(pTarget)
		&& CanAttack(pTarget))
	{
		if (isAttackable(pTarget) && CanCastRHit(GetSocketID()))
		{
			if (isInTempleEventZone() && !isSameEventRoom(pTarget))
				return;

			if (pTarget->hasBuff(BUFF_TYPE_FREEZE))
				return;

			CUser *pUser = g_pMain->GetUserPtr(GetSocketID());
			if (pUser != nullptr)
				pUser->m_RHitRepeatList.insert(std::make_pair(GetSocketID(), UNIXTIME));

			damage = GetDamage(pTarget);

			// Can't use R attacks in the Snow War.
			if (GetZoneID() == ZONE_SNOW_BATTLE && g_pMain->m_byBattleOpen == SNOW_BATTLE)
				damage = 0;

			if (damage > 0)
			{
				pTarget->HpChange(-damage, this);
				if (pTarget->isDead())
					bResult = ATTACK_TARGET_DEAD;
				else
					bResult = ATTACK_SUCCESS;

				// Every attack takes a little of your weapon's durability.
				ItemWoreOut(ATTACK, damage);

				// Every hit takes a little of the defender's armour durability.
				if (pTarget->isPlayer())
					TO_USER(pTarget)->ItemWoreOut(DEFENCE, damage);
			}
		}
	}

	Packet result(WIZ_ATTACK, bType);
	result << bResult << GetSocketID() << tid;
	SendToRegion(&result);
}

void CUser::Regene(uint8 regene_type, uint32 magicid /*= 0*/)
{
	ASSERT(GetMap() != nullptr);

	_OBJECT_EVENT* pEvent = nullptr;
	_HOME_INFO* pHomeInfo = nullptr;
	float x = 0.0f, z = 0.0f;

	if (!isDead())
		return;

	if (regene_type != 1 && regene_type != 2)
		regene_type = 1;

	if (regene_type == 2) 
	{
		magicid = 490041;	// The Stone of Ressurection magic ID

		// Is our level high enough to be able to resurrect using this skill?
		if (GetLevel() <= 5
			// Do we have enough resurrection stones?
				|| !RobItem(379006000, 3 * GetLevel()))
				return;
	}

	// If we're in a home zone, we'll want the coordinates from there. Otherwise, assume our own home zone.
	pHomeInfo = g_pMain->m_HomeArray.GetData(GetZoneID() <= ZONE_ELMORAD ? GetZoneID() : GetNation());
	if (pHomeInfo == nullptr)
		return;

	UserInOut(INOUT_OUT);

	pEvent = GetMap()->GetObjectEvent(m_sBind);	

	// If we're not using a spell to resurrect.
	if (magicid == 0) 
	{
		// Resurrect at a bind/respawn point
		if (pEvent != nullptr && pEvent->byLife == 1)
		{
			SetPosition(pEvent->fPosX + x, 0.0f, pEvent->fPosZ + z);
		}
		// Are we trying to respawn in a home zone?
		else if (GetZoneID() <= ZONE_ELMORAD) 
		{
			// Use the proper respawn area for our nation, as the opposite nation can
			// enter this zone at a war's invasion stage.
			if (GetNation() == KARUS) 
			{
				x = (float)(pHomeInfo->KarusZoneX + myrand(0, pHomeInfo->KarusZoneLX));
				z = (float)(pHomeInfo->KarusZoneZ + myrand(0, pHomeInfo->KarusZoneLZ));			
			}
			else 
			{
				x = (float)(pHomeInfo->ElmoZoneX + myrand(0, pHomeInfo->ElmoZoneLX));
				z = (float)(pHomeInfo->ElmoZoneZ + myrand(0, pHomeInfo->ElmoZoneLZ));
			}		
		}
		else
		{
			// If we're in a war zone (aside from snow wars, which apparently use different coords), use BattleZone coordinates.
			if (GetZoneID() != ZONE_SNOW_BATTLE 
				&& GetZoneID() == (ZONE_BATTLE_BASE + g_pMain->m_byBattleZone))
			{
				x = (float)(pHomeInfo->BattleZoneX + myrand(0, pHomeInfo->BattleZoneLX));
				z = (float)(pHomeInfo->BattleZoneZ + myrand(0, pHomeInfo->BattleZoneLZ));
			}
			// If we died in the Moradon arena, we need to spawn near the Arena.
			else if (GetZoneID() == ZONE_MORADON && isInArena())
			{
				x = (float)(MINI_ARENA_RESPAWN_X + myrand(-MINI_ARENA_RESPAWN_RADIUS, MINI_ARENA_RESPAWN_RADIUS));
				z = (float)(MINI_ARENA_RESPAWN_Z + myrand(-MINI_ARENA_RESPAWN_RADIUS, MINI_ARENA_RESPAWN_RADIUS));
			}
			// For all else, just grab the start position (/town coordinates) from the START_POSITION table.
			else
			{
				short sx, sz;
				GetStartPosition(sx, sz);

				x = sx;
				z = sz;
			}
		}

		SetPosition(x, 0.0f, z);

		m_bResHpType = USER_STANDING;	
		m_bRegeneType = REGENE_NORMAL;
	}
	else // we're respawning using a resurrect skill.
	{
		_MAGIC_TYPE5 * pType = g_pMain->m_Magictype5Array.GetData(magicid);     
		if (pType == nullptr)
			return;

		MSpChange(-m_iMaxMp); // reset us to 0 MP. 

		if (m_sWhoKilledMe == -1) 
			ExpChange((m_iLostExp * pType->bExpRecover) / 100); // Restore 

		m_bResHpType = USER_STANDING;
		m_bRegeneType = REGENE_MAGIC;
	}

	Packet result(WIZ_REGENE);
	result << GetSPosX() << GetSPosZ() << GetSPosY();
	Send(&result);

	HpChange(GetMaxHealth());

	m_tLastRegeneTime = UNIXTIME;
	m_sWhoKilledMe = -1;
	m_iLostExp = 0;

	if (magicid == 0)
		BlinkStart();

	if (!isBlinking())
	{
		result.Initialize(AG_USER_REGENE);
		result << GetSocketID() << m_sHp;
		Send_AIServer(&result);
	}

	SetRegion(GetNewRegionX(), GetNewRegionZ());

	UserInOut(INOUT_RESPAWN);		

	g_pMain->RegionUserInOutForMe(this);
	g_pMain->RegionNpcInfoForMe(this);

	InitializeStealth();
	SendUserStatusUpdate(USER_STATUS_DOT, USER_STATUS_CURE);
	SendUserStatusUpdate(USER_STATUS_POISON, USER_STATUS_CURE);

	if (isInArena())
		SendUserStatusUpdate(USER_STATUS_SPEED, USER_STATUS_CURE);

	RecastSavedMagic();

	// If we actually respawned (i.e. we weren't resurrected by a skill)...
	if (magicid == 0)
	{
		// In PVP zones (not war zones), we must kick out players if they no longer
		// have any national points.
		if (GetLoyalty() == 0 
			&& (GetMap()->isWarZone()
			|| isInPKZone()))
			KickOutZoneUser();
	}
}