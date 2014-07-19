#pragma once

#include "../shared/STLMap.h"

#define MAX_CHECK_EVENT	10

typedef CSTLMap <int>			mapNpcArray;

struct _RoomEvent
{
	short	sNumber;			// ��ɾ�, ���ǹ� ��ȣ
	short	sOption_1;			// option 1 (������ ��ȣ�� �ַ� ������ ����)
	short	sOption_2;			// option 2 (������ ������)
};

class CNpc;

enum RoomStatus
{
	RoomStatusInitialised	= 1,
	RoomStatusInProgress	= 2,
	RoomStatusCleared		= 3
};

class CRoomEvent  
{
public:
	INLINE RoomStatus GetStatus() { return m_byStatus; }

	INLINE bool isInitialised() { return GetStatus() == RoomStatusInitialised; }
	INLINE bool isInProgress() { return GetStatus() == RoomStatusInProgress; }
	INLINE bool isCleared() { return GetStatus() == RoomStatusCleared; }

	int     m_iZoneNumber;		// zone number
	short	m_sRoomNumber;		// room number (0:empty room)
	RoomStatus	m_byStatus;		// room status (1:init, 2:progress, 3:clear)
	uint8	m_byCheck;			// ���ǹ��� ����
	uint8	m_byRoomType;		// ���� Ÿ��(0:�Ϲ�, 1:������, 2:,,,,)

	int		m_iInitMinX;		// room region x
	int		m_iInitMinZ;
	int		m_iInitMaxX;
	int		m_iInitMaxZ;

	int		m_iEndMinX;			// room end region x ��������,,
	int		m_iEndMinZ;
	int		m_iEndMaxX;
	int		m_iEndMaxZ;

	_RoomEvent  m_Logic[MAX_CHECK_EVENT];		// ���ǵ�
	_RoomEvent  m_Exec[MAX_CHECK_EVENT];		// ���๮

	time_t   m_tDelayTime;						// time

	mapNpcArray	m_mapRoomNpcArray;				// room npc uid array

private:
	uint8    m_byLogicNumber;	// ������ ���ǹ� �˻� ��ȣ (���ǹ�ȣ�� 1���� ���۵�) (m_byCheck�� m_byLogicNumber�� ���ٸ� Ŭ���� ����)


public:
	CRoomEvent();
	virtual ~CRoomEvent();

	void MainRoom();
	void InitializeRoom();

private:
	void Initialize();
	bool  CheckEvent(int event_num);
	bool  RunEvent( int event_num );
	bool  CheckMonsterCount( int sid, int count, int type );
	CNpc* GetNpcPtr( int sid );
	void  EndEventSay( int option1, int option2 );

};
