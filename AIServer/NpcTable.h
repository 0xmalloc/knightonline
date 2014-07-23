#pragma once

#define MONSTER_SPEED	1500

class CNpcTable  
{
public:
	uint16	m_sSid;				// MONSTER(NPC) Serial ID
	std::string	m_strName;		// MONSTER(NPC) Name
	uint16	m_sPid;				// MONSTER(NPC) Picture ID
	uint16	m_sSize;			// ĳ������ ����(100 �ۼ�Ʈ ����)
	uint32	m_iWeapon_1;		// ����迭(������)
	uint32	m_iWeapon_2;		// ����迭(�޼�)
	uint8	m_byGroup;			// �Ҽ�����
	uint8	m_byActType;		// �ൿ����
	uint8	m_tNpcType;			// NPC Type
	// 0 : Monster
	// 1 : Normal NPC
	uint8	m_byFamilyType;		// ������̿��� �������踦 �����Ѵ�.
	uint8	m_byRank;			// ����
	uint8	m_byTitle;			// ����
	uint32	m_iSellingGroup;	// ������ �׷�(���ǸŸ� ��� NPC�� ��츸)
	uint16	m_sLevel;			// level
	uint32	m_iExp;				// ����ġ
	uint32	m_iLoyalty;			// loyalty

	uint32	m_iMaxHP;			// �ִ� HP
	uint16	m_sMaxMP;			// �ִ� MP

	uint16	m_sAttack;			// ���ݰ�
	uint16	m_sDefense;			// ��
	uint16	m_sHitRate;			// Ÿ�ݼ�����
	uint16	m_sEvadeRate;		// ȸ�Ǽ�����
	uint16	m_sDamage;			// �⺻ ������
	uint16	m_sAttackDelay;		// ���ݵ�����
	uint16	m_sSpeed;			// �̵��ӵ�	
	uint8	m_bySpeed_1;		// �⺻ �̵� Ÿ��
	uint8	m_bySpeed_2;		// �ٴ� �̵� Ÿ��..
	uint16	m_sStandTime;		// ���ִ� �ð�
	uint32	m_iMagic1;			// ��븶�� 1
	uint32	m_iMagic2;			// ��븶�� 2
	uint32	m_iMagic3;			// ��븶�� 3	

	uint16	m_byFireR;			// ȭ�� ���׷�
	uint16	m_byColdR;			// �ñ� ���׷�
	uint16	m_byLightningR;		// ���� ���׷�
	uint16	m_byMagicR;			// ���� ���׷�
	uint16	m_byDiseaseR;		// ���� ���׷�
	uint16	m_byPoisonR;		// �� ���׷�

	float	m_fBulk;

	uint8	m_bySearchRange;	// �� Ž�� ����
	uint8	m_byAttackRange;	// �����Ÿ�
	uint8	m_byTracingRange;	// �߰ݰŸ�

	uint32	m_iMoney;			// �������� ��
	uint16	m_iItem;			// �������� ������
	uint8	m_byDirectAttack;	// ���ݹ��( 0 : ����, 1:�հ���(��������), 2:��,�������� )
	uint8	m_byMagicAttack;	// ��������( 0:�������� ����, 1:��������, 2:������, 3:����)
	uint8	m_byGroupSpecial;

	CNpcTable() : m_sSpeed(MONSTER_SPEED)
	{
	}
};