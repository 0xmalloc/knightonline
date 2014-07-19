#pragma once

//
//	Defines About Communication
//
#define AI_SERVER_PORT		10020
#define MAX_SOCKET					100
#define MAX_PATH_LINE		100

#define MAX_NPC_SIZE		30
#define MAX_WEAPON_NAME_SIZE	40
#define VIEW_DIST			48		// ���ðŸ�
#define MAX_UPGRADE_WEAPON	12

//enum MOVE_SPEED {SPEED_SLOW=0, SPEED_NORMAL, SPEED_FAST};
//enum USER_TYPE {TYPE_USER=0, TYPE_MONSTER, TYPE_NPC, TYPE_DOOR, TYPE_GUARD};

// Npc InOut
#define NPC_IN					0X01
#define NPC_OUT					0X02

#define TILE_SIZE		4
#define CELL_SIZE		4

#define COMPARE(x,min,max) ((x>=min)&&(x<max))

struct _NpcPosition
{
	typedef struct { long x; long y; } Point;

	uint8	byType;			// type
	uint8	bySpeed;		// speed
	Point	pPoint;			// position
	float fXPos;
	float fZPos;
};

//
//	About USER
//
#define AI_USER_DEAD				0X00
#define AI_USER_LIVE				0X01

//
//	About NPC
//
#define NPC_NUM					20
#define MAX_DUNGEON_BOSS_MONSTER	20

#define NPC_PASSIVE				150
#define NPC_MAX_MOVE_RANGE		100

//
//	About Map Object
//
#define USER_BAND				0			// Map ���� ������ �ִ�.
#define NPC_BAND				10000		// Map ���� NPC(������)�� �ִ�.
#define INVALID_BAND			30000		// �߸��� ID BAND

//
//  Item
//
#define TYPE_MONEY				0
#define TYPE_ITEM				1

////////////////////////////////////////////////////////////
// Durability Type
#define ATTACK				0x01
#define DEFENCE				0x02
////////////////////////////////////////////////////////////

#define GREAT_SUCCESS			0X01		// �뼺��
#define SUCCESS					0X02		// ����
#define NORMAL					0X03		// ����
#define	FAIL					0X04		// ����

#define DIR_DOWN			0			// �� �����ִ� ������ �����Ѵ�.
#define	DIR_DOWNLEFT		1
#define DIR_LEFT			2
#define	DIR_UPLEFT			3
#define DIR_UP				4
#define DIR_UPRIGHT			5
#define DIR_RIGHT			6
#define	DIR_DOWNRIGHT		7

////////////////////////////////////////////////////////////
// Npc Type
// Monster�� 0���� ���� 10������ Ÿ��
#define NPCTYPE_MONSTER				0	// monster

// Attack Type
#define DIRECT_ATTACK		0
#define LONG_ATTACK			1
#define MAGIC_ATTACK		2
#define DURATION_ATTACK		3

#define NORMAL_OBJECT		0
#define SPECIAL_OBJECT		1

// Battlezone Announcement
#define BATTLEZONE_OPEN         0x00
#define BATTLEZONE_CLOSE        0x01           
#define DECLARE_WINNER          0x02

const int TYPE_MONEY_SID		=	900000000;