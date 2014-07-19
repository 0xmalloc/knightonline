#include "stdafx.h"
#include "LuaEngine.h"
#include "User.h"
#include "Npc.h"

/* 
Classes
*/

/**
* Defining a class is simple: just #define LUA_CLASS to your class name, 
* then call DEFINE_LUA_CLASS(), specifying each method to expose.
*
* Remember to #undef LUA_CLASS when you're done.
*
* Doing so sacrifices a little readability, however it
* makes manually defining methods much quicker (less to type).
*
* Also, as our method doesn't support inheritance, you'll need
* to redefine the same methods for each class.
*
* This is because of the way we dynamically pull instance pointers;
* these are typedef'd within the class, so that we can refer to them 
* to grab the class name from within. As we type check these with Lua,
* they'll fail, so we won't be able to use them.
*
* I don't think this is such a big issue, as there's only a handful of
* cases that actually require this behaviour.
**/

#define LUA_CLASS CUser
DEFINE_LUA_CLASS
	(
	// Getters
	MAKE_LUA_METHOD(GetName)
	MAKE_LUA_METHOD(GetAccountName)
	MAKE_LUA_METHOD(GetZoneID)
	MAKE_LUA_METHOD(GetX)
	MAKE_LUA_METHOD(GetY)
	MAKE_LUA_METHOD(GetZ)
	MAKE_LUA_METHOD(GetNation)
	MAKE_LUA_METHOD(GetLevel)
	MAKE_LUA_METHOD(GetClass)
	MAKE_LUA_METHOD(GetCoins)
	MAKE_LUA_METHOD(GetInnCoins)
	MAKE_LUA_METHOD(GetLoyalty)
	MAKE_LUA_METHOD(GetMonthlyLoyalty)
	MAKE_LUA_METHOD(GetManner)
	MAKE_LUA_METHOD(GetActiveQuestID)
	MAKE_LUA_METHOD(GetClanGrade)
	MAKE_LUA_METHOD(GetClanPoint)
	MAKE_LUA_METHOD(GetClanRank)
	MAKE_LUA_METHOD(GetStat)

	MAKE_LUA_METHOD(isWarrior)
	MAKE_LUA_METHOD(isRogue)
	MAKE_LUA_METHOD(isMage)
	MAKE_LUA_METHOD(isPriest)

	MAKE_LUA_METHOD(isBeginner)
	MAKE_LUA_METHOD(isBeginnerWarrior)
	MAKE_LUA_METHOD(isBeginnerRogue)
	MAKE_LUA_METHOD(isBeginnerMage)
	MAKE_LUA_METHOD(isBeginnerPriest)

	MAKE_LUA_METHOD(isNovice)
	MAKE_LUA_METHOD(isNoviceWarrior)
	MAKE_LUA_METHOD(isNoviceRogue)
	MAKE_LUA_METHOD(isNoviceMage)
	MAKE_LUA_METHOD(isNovicePriest)

	MAKE_LUA_METHOD(isMastered)
	MAKE_LUA_METHOD(isMasteredWarrior)
	MAKE_LUA_METHOD(isMasteredRogue)
	MAKE_LUA_METHOD(isMasteredMage)
	MAKE_LUA_METHOD(isMasteredPriest)

	MAKE_LUA_METHOD(isInClan)
	MAKE_LUA_METHOD(isClanLeader)
	MAKE_LUA_METHOD(isInParty)
	MAKE_LUA_METHOD(isPartyLeader)
	MAKE_LUA_METHOD(isKing)

	// Shortcuts for lazy people
	MAKE_LUA_METHOD(hasCoins)
	MAKE_LUA_METHOD(hasInnCoins)
	MAKE_LUA_METHOD(hasLoyalty)
	MAKE_LUA_METHOD(hasMonthlyLoyalty)
	MAKE_LUA_METHOD(hasManner)

	// Here lie the useful methods.
	MAKE_LUA_METHOD(GiveItem)
	MAKE_LUA_METHOD(RobItem)
	MAKE_LUA_METHOD(CheckExistItem)
	MAKE_LUA_METHOD(GoldGain)
	MAKE_LUA_METHOD(GoldLose)
	MAKE_LUA_METHOD(ExpChange)
	MAKE_LUA_METHOD(GiveLoyalty)
	MAKE_LUA_METHOD(RobLoyalty)
	MAKE_LUA_METHOD(SaveEvent)
	MAKE_LUA_METHOD(SearchQuest)
	MAKE_LUA_METHOD(ShowMap)
	MAKE_LUA_METHOD(SelectMsg) // menu
	MAKE_LUA_METHOD(NpcSay) // dialog
	MAKE_LUA_METHOD(NpcMsg) // new automated quest prompt (does whatever's needed, menu, quest prompt, etc)
	MAKE_LUA_METHOD(CheckWeight)
	MAKE_LUA_METHOD(CheckSkillPoint)
	MAKE_LUA_METHOD(isRoomForItem) // FindSlotForItem()
	MAKE_LUA_METHOD(CountMonsterQuestSub)
	MAKE_LUA_METHOD(CountMonsterQuestMain)
	MAKE_LUA_METHOD(CheckExchange)
	MAKE_LUA_METHOD(RunExchange)
	MAKE_LUA_METHOD(SendNameChange)
	MAKE_LUA_METHOD(ResetSkillPoints)
	MAKE_LUA_METHOD(ResetStatPoints)
	MAKE_LUA_METHOD(PromoteUserNovice)
	MAKE_LUA_METHOD(PromoteUser)
	MAKE_LUA_METHOD(RobAllItemParty)
	MAKE_LUA_METHOD(ZoneChange)
	MAKE_LUA_METHOD(ZoneChangeParty)
	MAKE_LUA_METHOD(ZoneChangeClan)
	MAKE_LUA_METHOD(ShowEffect)
	MAKE_LUA_METHOD(ShowNpcEffect)
	MAKE_LUA_METHOD(KissUser)
	MAKE_LUA_METHOD(ChangeManner)
	MAKE_LUA_METHOD(PromoteClan)
	MAKE_LUA_METHOD(RobClanPoint)
	MAKE_LUA_METHOD(GetBeefRoastVictory)
	MAKE_LUA_METHOD(RequestPersonalRankReward)
	MAKE_LUA_METHOD(RequestReward)
	MAKE_LUA_METHOD(RunCountExchange)
	MAKE_LUA_METHOD(GetMaxExchange)
	MAKE_LUA_METHOD(GetPartyMemberAmount)
	MAKE_LUA_METHOD(GetUserDailyOp)
	MAKE_LUA_METHOD(GetEventTrigger)
	MAKE_LUA_METHOD(GetPremium)
	MAKE_LUA_METHOD(GetWarVictory)
	);
#undef LUA_CLASS

#define LUA_CLASS CNpc
DEFINE_LUA_CLASS
	(
	// Getters
	MAKE_LUA_METHOD(GetID)
	MAKE_LUA_METHOD(GetProtoID)
	MAKE_LUA_METHOD(GetName)
	MAKE_LUA_METHOD(GetNation)
	MAKE_LUA_METHOD(GetType)
	MAKE_LUA_METHOD(GetZoneID)
	MAKE_LUA_METHOD(GetX)
	MAKE_LUA_METHOD(GetY)
	MAKE_LUA_METHOD(GetZ)
	MAKE_LUA_METHOD(CastSkill)

	// Useful methods
	// MAKE_LUA_METHOD(CycleSpawn) // i.e. ChangePosition(), used to cycle a spawn through the various trap numbers (like 7 key quest NPCs)

	);
#undef LUA_CLASS


/* 
Global functions 
*/
LUA_FUNCTION(CheckPercent)
{
	LUA_RETURN(CheckPercent(LUA_ARG(int, 1)));
}

#define Lua_GetUser() g_pMain->GetUserPtr(LUA_ARG(uint16, 1))

LUA_FUNCTION(HowmuchItem) 
{
	CUser * pUser = Lua_GetUser();
	uint32 result = 0;

	if (pUser != nullptr) 
	{
		uint32 nItemID = LUA_ARG(uint32, 2);
		if (nItemID == ITEM_GOLD)
			result = pUser->GetCoins();
		else
			result = pUser->GetItemCount(nItemID);
	}

	LUA_RETURN(result);
}

LUA_FUNCTION(CheckNation)
{
	CUser * pUser = Lua_GetUser();
	uint8 bNation = Nation::NONE;

	if (pUser != nullptr)
		bNation = pUser->GetNation();

	LUA_RETURN(bNation);
}

LUA_FUNCTION(CheckClass)
{
	CUser * pUser = Lua_GetUser();
	uint8 bClassType = 0;

	if (pUser != nullptr)
		bClassType = pUser->GetClassType();

	LUA_RETURN(bClassType);
}

LUA_FUNCTION(CheckLevel)
{
	CUser * pUser = Lua_GetUser();
	uint8 bClassType = 0;

	if (pUser != nullptr)
		bClassType = pUser->GetLevel();

	LUA_RETURN(bClassType);
}

LUA_FUNCTION(CheckSkillPoint)
{
	CUser * pUser = Lua_GetUser();
	uint8 bPoints = 0;

	if (pUser != nullptr)
		bPoints = pUser->GetSkillPoints((SkillPointCategory)LUA_ARG(uint32, 2));

	LUA_RETURN(bPoints);
}

#define _LUA_WRAPPER_USER_FUNCTION(name, methodName) \
	LUA_FUNCTION(name) { \
	CUser * pUser = Lua_GetUser(); /* get the user from the stack using the specified user ID */ \
	lua_tpush(L, pUser); /* push the user pointer onto the stack, as our code expects */ \
	lua_remove(L, 1); /* removes the user ID from the stack */ \
	lua_insert(L, 1); /* moves the user pointer to the start of the bottom of the stack where it's expected */ \
	return CUser::Lua_ ## methodName(L); \
}

#define LUA_WRAPPER_USER_FUNCTION(name) \
	_LUA_WRAPPER_USER_FUNCTION(name, name)

LUA_WRAPPER_USER_FUNCTION(ShowMap);
LUA_WRAPPER_USER_FUNCTION(SaveEvent);
LUA_WRAPPER_USER_FUNCTION(CheckExchange);
LUA_WRAPPER_USER_FUNCTION(RunExchange);
LUA_WRAPPER_USER_FUNCTION(SearchQuest);
LUA_WRAPPER_USER_FUNCTION(NpcMsg);
LUA_WRAPPER_USER_FUNCTION(ShowEffect);
LUA_WRAPPER_USER_FUNCTION(ShowNpcEffect);
_LUA_WRAPPER_USER_FUNCTION(ExistMonsterQuestSub, GetActiveQuestID);
_LUA_WRAPPER_USER_FUNCTION(PromoteKnight, PromoteClan);
_LUA_WRAPPER_USER_FUNCTION(GetName, GetName);
_LUA_WRAPPER_USER_FUNCTION(GetAccountName, GetAccountName);
_LUA_WRAPPER_USER_FUNCTION(GetZoneID, GetZoneID);
_LUA_WRAPPER_USER_FUNCTION(GetX, GetX);
_LUA_WRAPPER_USER_FUNCTION(GetY, GetY);
_LUA_WRAPPER_USER_FUNCTION(GetZ, GetZ);
_LUA_WRAPPER_USER_FUNCTION(GetNation, GetNation);
_LUA_WRAPPER_USER_FUNCTION(GetLevel, GetLevel);
_LUA_WRAPPER_USER_FUNCTION(GetClass, GetClass);
_LUA_WRAPPER_USER_FUNCTION(GetInnCoins, GetInnCoins);
_LUA_WRAPPER_USER_FUNCTION(GetCoins, GetCoins);
_LUA_WRAPPER_USER_FUNCTION(GetLoyalty, GetLoyalty);
_LUA_WRAPPER_USER_FUNCTION(GetMonthlyLoyalty, GetMonthlyLoyalty);
_LUA_WRAPPER_USER_FUNCTION(GetManner, GetManner);
_LUA_WRAPPER_USER_FUNCTION(GetActiveQuestID, GetActiveQuestID);
_LUA_WRAPPER_USER_FUNCTION(isWarrior, isWarrior);
_LUA_WRAPPER_USER_FUNCTION(isRogue, isRogue);
_LUA_WRAPPER_USER_FUNCTION(isMage, isMage);
_LUA_WRAPPER_USER_FUNCTION(isPriest, isPriest);
_LUA_WRAPPER_USER_FUNCTION(isInClan, isInClan);
_LUA_WRAPPER_USER_FUNCTION(isClanLeader, isClanLeader);
_LUA_WRAPPER_USER_FUNCTION(isInParty, isInParty);
_LUA_WRAPPER_USER_FUNCTION(isPartyLeader, isPartyLeader);
_LUA_WRAPPER_USER_FUNCTION(isKing, isKing);
_LUA_WRAPPER_USER_FUNCTION(hasCoins, hasCoins);
_LUA_WRAPPER_USER_FUNCTION(hasInnCoins, hasInnCoins);
_LUA_WRAPPER_USER_FUNCTION(hasLoyalty, hasLoyalty);
_LUA_WRAPPER_USER_FUNCTION(hasMonthlyLoyalty, hasMonthlyLoyalty);
_LUA_WRAPPER_USER_FUNCTION(hasManner, hasManner);
_LUA_WRAPPER_USER_FUNCTION(GiveItem, GiveItem);
_LUA_WRAPPER_USER_FUNCTION(RobItem, RobItem);
_LUA_WRAPPER_USER_FUNCTION(CheckExistItem, CheckExistItem);
_LUA_WRAPPER_USER_FUNCTION(GoldGain, GoldGain);
_LUA_WRAPPER_USER_FUNCTION(GoldLose, GoldLose);
_LUA_WRAPPER_USER_FUNCTION(ExpChange, ExpChange);
_LUA_WRAPPER_USER_FUNCTION(GiveLoyalty, GiveLoyalty);
_LUA_WRAPPER_USER_FUNCTION(RobLoyalty, RobLoyalty);
_LUA_WRAPPER_USER_FUNCTION(NpcSay, NpcSay);
_LUA_WRAPPER_USER_FUNCTION(CheckWeight, CheckWeight);
_LUA_WRAPPER_USER_FUNCTION(isRoomForItem, isRoomForItem);
_LUA_WRAPPER_USER_FUNCTION(SendNameChange, SendNameChange);
_LUA_WRAPPER_USER_FUNCTION(SendStatSkillDistribute, SendStatSkillDistribute);
_LUA_WRAPPER_USER_FUNCTION(ResetSkillPoints, ResetSkillPoints);
_LUA_WRAPPER_USER_FUNCTION(ResetStatPoints, ResetStatPoints);
_LUA_WRAPPER_USER_FUNCTION(PromoteUserNovice, PromoteUserNovice);
_LUA_WRAPPER_USER_FUNCTION(PromoteUser, PromoteUser);
_LUA_WRAPPER_USER_FUNCTION(RobAllItemParty, RobAllItemParty);
_LUA_WRAPPER_USER_FUNCTION(ZoneChange, ZoneChange);
_LUA_WRAPPER_USER_FUNCTION(ZoneChangeParty, ZoneChangeParty);
_LUA_WRAPPER_USER_FUNCTION(ZoneChangeClan, ZoneChangeClan);
_LUA_WRAPPER_USER_FUNCTION(KissUser, KissUser);
_LUA_WRAPPER_USER_FUNCTION(ChangeManner, ChangeManner);
_LUA_WRAPPER_USER_FUNCTION(PromoteClan, PromoteClan);
_LUA_WRAPPER_USER_FUNCTION(CheckClanGrade, GetClanGrade);
_LUA_WRAPPER_USER_FUNCTION(CheckClanPoint, GetClanPoint);
_LUA_WRAPPER_USER_FUNCTION(CheckLoyalty, GetLoyalty);
_LUA_WRAPPER_USER_FUNCTION(CheckKnight, GetClanRank);
_LUA_WRAPPER_USER_FUNCTION(CheckStatPoint, GetStat); 
_LUA_WRAPPER_USER_FUNCTION(RobClanPoint, RobClanPoint);
_LUA_WRAPPER_USER_FUNCTION(CheckBeefRoastVictory, GetBeefRoastVictory);
_LUA_WRAPPER_USER_FUNCTION(RequestPersonalRankReward, RequestPersonalRankReward);
_LUA_WRAPPER_USER_FUNCTION(RequestReward, RequestReward);
_LUA_WRAPPER_USER_FUNCTION(RunCountExchange, RunCountExchange);
_LUA_WRAPPER_USER_FUNCTION(GetMaxExchange, GetMaxExchange);
_LUA_WRAPPER_USER_FUNCTION(PartyCountMembers, GetPartyMemberAmount);
_LUA_WRAPPER_USER_FUNCTION(GetUserDailyOp, GetUserDailyOp);
_LUA_WRAPPER_USER_FUNCTION(CountMonsterQuestSub, CountMonsterQuestSub);
_LUA_WRAPPER_USER_FUNCTION(CountMonsterQuestMain, CountMonsterQuestMain);
_LUA_WRAPPER_USER_FUNCTION(GetEventTrigger, GetEventTrigger);
_LUA_WRAPPER_USER_FUNCTION(GetPremium, GetPremium);
_LUA_WRAPPER_USER_FUNCTION(CheckWarVictory, GetWarVictory);

LUA_FUNCTION(SelectMsg)
{
	CUser * pUser = Lua_GetUser();
	if (pUser == nullptr) 
		return LUA_NO_RESULTS;

	uint32 arg = 2; // start from after the user instance.
	int32 menuButtonText[MAX_MESSAGE_EVENT], 
		menuButtonEvents[MAX_MESSAGE_EVENT];
	uint8 bFlag = LUA_ARG(uint8, arg++);
	int32 nQuestID = LUA_ARG_OPTIONAL(int32, -1, arg++);
	int32 menuHeaderText = LUA_ARG(int32, arg++);
	arg++; // skip the NPC ID
	foreach_array(i, menuButtonText)
	{
		menuButtonText[i] = LUA_ARG_OPTIONAL(int32, -1, arg++);
		menuButtonEvents[i] = LUA_ARG_OPTIONAL(int32, -1, arg++);
	}

	LUA_NO_RETURN(pUser->SelectMsg(bFlag, nQuestID, menuHeaderText, menuButtonText, menuButtonEvents));
}

LUA_FUNCTION(CastSkill)
{
	CUser * pUser = Lua_GetUser();
	bool bResult = false;
	if (pUser != nullptr)
	{
		CNpc * pNpc = g_pMain->GetNpcPtr(pUser->m_sEventNid);
		if (pNpc != nullptr)
		{
			bResult = pNpc->CastSkill(
				reinterpret_cast<Unit *>(pUser),
				LUA_ARG(uint32, 2)
				);
		}
	}
	return bResult;
}
