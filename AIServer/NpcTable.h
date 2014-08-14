<<<<<<< HEAD
ï»¿#pragma once
=======
#pragma once
>>>>>>> koserver2

#define MONSTER_SPEED	1500

class CNpcTable  
{
public:
	uint16	m_sSid;				// MONSTER(NPC) Serial ID
	std::string	m_strName;		// MONSTER(NPC) Name
	uint16	m_sPid;				// MONSTER(NPC) Picture ID
<<<<<<< HEAD
	uint16	m_sSize;			// ìºë¦­í„°ì˜ ë¹„ìœ¨(100 í¼ì„¼íŠ¸ ê¸°ì¤€)
	uint32	m_iWeapon_1;		// ë¬´ê¸°ê³„ì—´(ì˜¤ë¥¸ì†)
	uint32	m_iWeapon_2;		// ë¬´ê¸°ê³„ì—´(ì™¼ì†)
	uint8	m_byGroup;			// ì†Œì†ì§‘ë‹¨
	uint8	m_byActType;		// í–‰ë™íŒ¨í„´
	uint8	m_tNpcType;			// NPC Type
	// 0 : Monster
	// 1 : Normal NPC
	uint8	m_byFamilyType;		// ëª¹ë“¤ì‚¬ì´ì—ì„œ ê°€ì¡±ê´€ê³„ë¥¼ ê²°ì •í•œë‹¤.
	uint8	m_byRank;			// ìž‘ìœ„
	uint8	m_byTitle;			// ì§€ìœ„
	uint32	m_iSellingGroup;	// ì•„ì´í…œ ê·¸ë£¹(ë¬¼ê±´ë§¤ë§¤ ë‹´ë‹¹ NPCì˜ ê²½ìš°ë§Œ)
	uint16	m_sLevel;			// level
	uint32	m_iExp;				// ê²½í—˜ì¹˜
	uint32	m_iLoyalty;			// loyalty

	uint32	m_iMaxHP;			// ìµœëŒ€ HP
	uint16	m_sMaxMP;			// ìµœëŒ€ MP

	uint16	m_sAttack;			// ê³µê²©ê°’
	uint16	m_sDefense;			// ë°©ì–´ê°’
	uint16	m_sHitRate;			// íƒ€ê²©ì„±ê³µë¥ 
	uint16	m_sEvadeRate;		// íšŒí”¼ì„±ê³µë¥ 
	uint16	m_sDamage;			// ê¸°ë³¸ ë°ë¯¸ì§€
	uint16	m_sAttackDelay;		// ê³µê²©ë”œë ˆì´
	uint16	m_sSpeed;			// ì´ë™ì†ë„	
	uint8	m_bySpeed_1;		// ê¸°ë³¸ ì´ë™ íƒ€ìž…
	uint8	m_bySpeed_2;		// ë›°ëŠ” ì´ë™ íƒ€ìž…..
	uint16	m_sStandTime;		// ì„œìžˆëŠ” ì‹œê°„
	uint32	m_iMagic1;			// ì‚¬ìš©ë§ˆë²• 1
	uint32	m_iMagic2;			// ì‚¬ìš©ë§ˆë²• 2
	uint32	m_iMagic3;			// ì‚¬ìš©ë§ˆë²• 3	

	uint16	m_byFireR;			// í™”ì—¼ ì €í•­ë ¥
	uint16	m_byColdR;			// ëƒ‰ê¸° ì €í•­ë ¥
	uint16	m_byLightningR;		// ì „ê¸° ì €í•­ë ¥
	uint16	m_byMagicR;			// ë§ˆë²• ì €í•­ë ¥
	uint16	m_byDiseaseR;		// ì €ì£¼ ì €í•­ë ¥
	uint16	m_byPoisonR;		// ë… ì €í•­ë ¥

	float	m_fBulk;

	uint8	m_bySearchRange;	// ì  íƒì§€ ë²”ìœ„
	uint8	m_byAttackRange;	// ì‚¬ì •ê±°ë¦¬
	uint8	m_byTracingRange;	// ì¶”ê²©ê±°ë¦¬

	uint32	m_iMoney;			// ë–¨ì–´ì§€ëŠ” ëˆ
	uint16	m_iItem;			// ë–¨ì–´ì§€ëŠ” ì•„ì´í…œ
	uint8	m_byDirectAttack;	// ê³µê²©ë°©ë²•( 0 : ì§ì ‘, 1:ë¡±ê³µê²©(ê°„ì ‘ê³µê²©), 2:ì§,ê°„ì ‘ê³µê²© )
	uint8	m_byMagicAttack;	// ë§ˆë²•ê³µê²©( 0:ë§ˆë²•ê³µê²© ì—†ìŒ, 1:ë§ˆë²•ê³µê²©, 2:ë…ê³µê²©, 3:ížë§)
=======
	uint16	m_sSize;			// Ä³¸¯ÅÍÀÇ ºñÀ²(100 ÆÛ¼¾Æ® ±âÁØ)
	uint32	m_iWeapon_1;		// ¹«±â°è¿­(¿À¸¥¼Õ)
	uint32	m_iWeapon_2;		// ¹«±â°è¿­(¿Þ¼Õ)
	uint8	m_byGroup;			// ¼Ò¼ÓÁý´Ü
	uint8	m_byActType;		// Çàµ¿ÆÐÅÏ
	uint8	m_tNpcType;			// NPC Type
	// 0 : Monster
	// 1 : Normal NPC
	uint8	m_byFamilyType;		// ¸÷µé»çÀÌ¿¡¼­ °¡Á·°ü°è¸¦ °áÁ¤ÇÑ´Ù.
	uint8	m_byRank;			// ÀÛÀ§
	uint8	m_byTitle;			// ÁöÀ§
	uint32	m_iSellingGroup;	// ¾ÆÀÌÅÛ ±×·ì(¹°°Ç¸Å¸Å ´ã´ç NPCÀÇ °æ¿ì¸¸)
	uint16	m_sLevel;			// level
	uint32	m_iExp;				// °æÇèÄ¡
	uint32	m_iLoyalty;			// loyalty

	uint32	m_iMaxHP;			// ÃÖ´ë HP
	uint16	m_sMaxMP;			// ÃÖ´ë MP

	uint16	m_sAttack;			// °ø°Ý°ª
	uint16	m_sDefense;			// ¹æ¾î°ª
	uint16	m_sHitRate;			// Å¸°Ý¼º°ø·ü
	uint16	m_sEvadeRate;		// È¸ÇÇ¼º°ø·ü
	uint16	m_sDamage;			// ±âº» µ¥¹ÌÁö
	uint16	m_sAttackDelay;		// °ø°Ýµô·¹ÀÌ
	uint16	m_sSpeed;			// ÀÌµ¿¼Óµµ	
	uint8	m_bySpeed_1;		// ±âº» ÀÌµ¿ Å¸ÀÔ
	uint8	m_bySpeed_2;		// ¶Ù´Â ÀÌµ¿ Å¸ÀÔ..
	uint16	m_sStandTime;		// ¼­ÀÖ´Â ½Ã°£
	uint32	m_iMagic1;			// »ç¿ë¸¶¹ý 1
	uint32	m_iMagic2;			// »ç¿ë¸¶¹ý 2
	uint32	m_iMagic3;			// »ç¿ë¸¶¹ý 3	

	uint16	m_byFireR;			// È­¿° ÀúÇ×·Â
	uint16	m_byColdR;			// ³Ã±â ÀúÇ×·Â
	uint16	m_byLightningR;		// Àü±â ÀúÇ×·Â
	uint16	m_byMagicR;			// ¸¶¹ý ÀúÇ×·Â
	uint16	m_byDiseaseR;		// ÀúÁÖ ÀúÇ×·Â
	uint16	m_byPoisonR;		// µ¶ ÀúÇ×·Â

	float	m_fBulk;

	uint8	m_bySearchRange;	// Àû Å½Áö ¹üÀ§
	uint8	m_byAttackRange;	// »çÁ¤°Å¸®
	uint8	m_byTracingRange;	// Ãß°Ý°Å¸®

	uint32	m_iMoney;			// ¶³¾îÁö´Â µ·
	uint16	m_iItem;			// ¶³¾îÁö´Â ¾ÆÀÌÅÛ
	uint8	m_byDirectAttack;	// °ø°Ý¹æ¹ý( 0 : Á÷Á¢, 1:·Õ°ø°Ý(°£Á¢°ø°Ý), 2:Á÷,°£Á¢°ø°Ý )
	uint8	m_byMagicAttack;	// ¸¶¹ý°ø°Ý( 0:¸¶¹ý°ø°Ý ¾øÀ½, 1:¸¶¹ý°ø°Ý, 2:µ¶°ø°Ý, 3:Èú¸µ)
>>>>>>> koserver2
	uint8	m_byGroupSpecial;

	CNpcTable() : m_sSpeed(MONSTER_SPEED)
	{
	}
};