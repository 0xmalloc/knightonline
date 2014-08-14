#include "stdafx.h"
#include <time.h>
#include <cstdarg>
#include "GameSocket.h"
#include "Npc.h"
#include "User.h"
#include "NpcThread.h"
#include "../GameServer/MagicProcess.h"

#include "../shared/database/OdbcRecordset.h"
#include "../shared/database/MagicTableSet.h"
#include "../shared/database/MagicType1Set.h"
#include "../shared/database/MagicType2Set.h"
#include "../shared/database/MagicType4Set.h"
<<<<<<< HEAD
=======
#include "../shared/database/ObjectPosSet.h"
>>>>>>> koserver2
#include "../shared/database/NpcPosSet.h"
#include "../shared/database/ZoneInfoSet.h"
#include "../shared/database/NpcItemSet.h"
#include "../shared/database/MakeItemGroupSet.h"
#include "../shared/database/NpcTableSet.h"
#include "../shared/database/MakeWeaponTableSet.h"
#include "../shared/database/MakeDefensiveTableSet.h"
#include "../shared/database/MakeGradeItemTableSet.h"
#include "../shared/database/MakeLareItemTableSet.h"
#include "../shared/database/ServerResourceSet.h"
#include "Region.h"
#include "../shared/Ini.h"
#include "../shared/packets.h"
#include "../shared/DateTime.h"

using namespace std;

bool g_bNpcExit	= false;
ZoneArray			g_arZone;

std::vector<Thread *> g_timerThreads;

CServerDlg::CServerDlg()
{
	m_iYear = 0; 
	m_iMonth = 0;
	m_iDate = 0;
	m_iHour = 0;
	m_iMin = 0;
	m_iWeather = 0;
	m_iAmount = 0;
	m_bIsNight = false;
	m_byBattleEvent = BATTLEZONE_CLOSE;
	m_sKillKarusNpc = 0;
	m_sKillElmoNpc = 0;
}

bool CServerDlg::Startup()
{
	g_timerThreads.push_back(new Thread(Timer_CheckAliveTest));
<<<<<<< HEAD
=======
	g_timerThreads.push_back(new Thread(Timer_CheckLiveTimes));
>>>>>>> koserver2

	m_sMapEventNpc = 0;
	m_bFirstServerFlag = false;			

	// Server Start
	DateTime time;
<<<<<<< HEAD
	printf("Server started on %02d-%02d-%04d at %02d:%02d.\n\n", time.GetDay(), time.GetMonth(), time.GetYear(), time.GetHour(), time.GetMinute());
=======
	printf("Server started on %04d-%02d-%02d at %02d:%02d\n\n", time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute());
>>>>>>> koserver2

	//----------------------------------------------------------------------
	//	DB part initialize
	//----------------------------------------------------------------------
	GetServerInfoIni();

<<<<<<< HEAD
	if (!m_GameDB.Connect(m_strGameDSN, m_strGameUID, m_strGamePWD, false))
=======
	if (!m_GameDB.Connect(m_strGameDSN, m_strGameUID, m_strGamePWD))
>>>>>>> koserver2
	{
		OdbcError *pError = m_GameDB.GetError();
		printf("ERROR: Could not connect to the database server, received error:\n%s\n", 
			pError->ErrorMessage.c_str());
		delete pError;
		return false;
	}

	//----------------------------------------------------------------------
	//	Communication Part Initialize ...
	//----------------------------------------------------------------------
<<<<<<< HEAD
	if (!m_socketMgr.Listen(AI_SERVER_PORT, MAX_SOCKET))
=======
	if (!m_socketMgr.Listen(m_AIServerPort, MAX_SOCKET))
>>>>>>> koserver2
		return false;

	//----------------------------------------------------------------------
	//	Load tables
	//----------------------------------------------------------------------
	if (!GetMagicTableData()
		|| !GetMagicType1Data()
		|| !GetMagicType2Data()
		|| !GetMagicType4Data()
		|| !GetNpcItemTable()
		|| !GetMakeItemGroupTable()
		|| !GetMakeWeaponItemTableData()
		|| !GetMakeDefensiveItemTableData()
		|| !GetMakeGradeItemTableData()
		|| !GetMakeLareItemTableData()
		|| !GetServerResourceTable()
<<<<<<< HEAD
=======
		|| !GetObjectPostTableData()
>>>>>>> koserver2
		|| !GetNpcTableData(false)
		|| !GetNpcTableData(true)
		// Load maps
		|| !MapFileLoad()
		// Spawn NPC threads
		|| !CreateNpcThread())
		return false;

	//----------------------------------------------------------------------
	//	Start NPC THREAD
	//----------------------------------------------------------------------
	ResumeAI();
	return true; 
}

bool CServerDlg::GetMagicTableData()
{
<<<<<<< HEAD
	LOAD_TABLE(CMagicTableSet, &m_GameDB, &m_MagictableArray, false);
=======
	LOAD_TABLE(CMagicTableSet, &m_GameDB, &m_MagictableArray, false, false);
>>>>>>> koserver2
}

bool CServerDlg::GetMagicType1Data()
{
<<<<<<< HEAD
	LOAD_TABLE(CMagicType1Set, &m_GameDB, &m_Magictype1Array, false);
=======
	LOAD_TABLE(CMagicType1Set, &m_GameDB, &m_Magictype1Array, false, false);
>>>>>>> koserver2
}

bool CServerDlg::GetMagicType2Data()
{
<<<<<<< HEAD
	LOAD_TABLE(CMagicType2Set, &m_GameDB, &m_Magictype2Array, false);
=======
	LOAD_TABLE(CMagicType2Set, &m_GameDB, &m_Magictype2Array, false, false);
>>>>>>> koserver2
}

bool CServerDlg::GetMagicType4Data()
{
<<<<<<< HEAD
	LOAD_TABLE(CMagicType4Set, &m_GameDB, &m_Magictype4Array, false);
=======
	LOAD_TABLE(CMagicType4Set, &m_GameDB, &m_Magictype4Array, false, false);
>>>>>>> koserver2
}

bool CServerDlg::GetMakeWeaponItemTableData()
{
<<<<<<< HEAD
	LOAD_TABLE(CMakeWeaponTableSet, &m_GameDB, &m_MakeWeaponItemArray, true);
=======
	LOAD_TABLE(CMakeWeaponTableSet, &m_GameDB, &m_MakeWeaponItemArray, true, false);
>>>>>>> koserver2
}

bool CServerDlg::GetMakeDefensiveItemTableData()
{
<<<<<<< HEAD
	LOAD_TABLE(CMakeDefensiveTableSet, &m_GameDB, &m_MakeDefensiveItemArray, true);
=======
	LOAD_TABLE(CMakeDefensiveTableSet, &m_GameDB, &m_MakeDefensiveItemArray, true, false);
>>>>>>> koserver2
}

bool CServerDlg::GetMakeGradeItemTableData()
{
<<<<<<< HEAD
	LOAD_TABLE(CMakeGradeItemTableSet, &m_GameDB, &m_MakeGradeItemArray, false);
=======
	LOAD_TABLE(CMakeGradeItemTableSet, &m_GameDB, &m_MakeGradeItemArray, false, false);
>>>>>>> koserver2
}

bool CServerDlg::GetMakeLareItemTableData()
{
<<<<<<< HEAD
	LOAD_TABLE(CMakeLareItemTableSet, &m_GameDB, &m_MakeLareItemArray, false);
=======
	LOAD_TABLE(CMakeLareItemTableSet, &m_GameDB, &m_MakeLareItemArray, false, false);
>>>>>>> koserver2
}

bool CServerDlg::GetServerResourceTable()
{
<<<<<<< HEAD
	LOAD_TABLE(CServerResourceSet, &m_GameDB, &m_ServerResourceArray, false);
=======
	LOAD_TABLE(CServerResourceSet, &m_GameDB, &m_ServerResourceArray, false, false);
>>>>>>> koserver2
}

bool CServerDlg::GetNpcItemTable()
{
<<<<<<< HEAD
	LOAD_TABLE(CNpcItemSet, &m_GameDB, &m_NpcItemArray, false);
=======
	LOAD_TABLE(CNpcItemSet, &m_GameDB, &m_NpcItemArray, false, false);
>>>>>>> koserver2
}

bool CServerDlg::GetMakeItemGroupTable()
{
<<<<<<< HEAD
	LOAD_TABLE(CMakeItemGroupSet, &m_GameDB, &m_MakeItemGroupArray, false);
=======
	LOAD_TABLE(CMakeItemGroupSet, &m_GameDB, &m_MakeItemGroupArray, false, false);
}

bool CServerDlg::GetObjectPostTableData()
{
	LOAD_TABLE(CObjectPosSet, &m_GameDB, &m_ObjectEventArray, false, false);
>>>>>>> koserver2
}

bool CServerDlg::GetNpcTableData(bool bNpcData /*= true*/)
{
<<<<<<< HEAD
	if (bNpcData)	{ LOAD_TABLE(CNpcTableSet, &m_GameDB, &m_arNpcTable, false); }
	else			{ LOAD_TABLE(CMonTableSet, &m_GameDB, &m_arMonTable, false); }
=======
	if (bNpcData)	{ LOAD_TABLE(CNpcTableSet, &m_GameDB, &m_arNpcTable, false, false); }
	else			{ LOAD_TABLE(CMonTableSet, &m_GameDB, &m_arMonTable, false, false); }
>>>>>>> koserver2
}

bool CServerDlg::CreateNpcThread()
{
	m_TotalNPC = m_sMapEventNpc;
	m_CurrentNPC = 0;

<<<<<<< HEAD
	LOAD_TABLE_ERROR_ONLY(CNpcPosSet, &m_GameDB, nullptr, false);

	FastGuard lock(m_eventThreadLock);
=======
	LOAD_TABLE_ERROR_ONLY(CNpcPosSet, &m_GameDB, nullptr, false, false);

	Guard lock(m_npcThreadLock);
	Guard lock2(m_eventThreadLock);
>>>>>>> koserver2
	foreach_stlmap (itr, g_arZone)
	{
		CNpcThread * pNpcThread = new CNpcThread();
		m_arNpcThread.insert(make_pair(itr->first, pNpcThread));
		m_arEventNpcThread.insert(make_pair(itr->first, new CNpcThread()));

		foreach_stlmap (npcItr, m_arNpc)
		{
			if (npcItr->second->GetZoneID() != itr->first)
				continue;

			CNpc * pNpc = npcItr->second;
			pNpc->Init();
			pNpcThread->m_pNpcs.insert(pNpc);
		}
	}

	printf("Monster Init - %d, threads = %lld\n", (uint16) m_TotalNPC, (long long) m_arNpcThread.size());
	return true;
}

bool CServerDlg::LoadSpawnCallback(OdbcCommand *dbCommand)
{
	// Avoid allocating stack space for these.
	// This method will only ever run in the same thread.
	static int nRandom = 0;
	static double dbSpeed = 0;
	static CNpcTable * pNpcTable = nullptr;
	static CRoomEvent* pRoom = nullptr;
	static char szPath[500];
	static float fRandom_X = 0.0f, fRandom_Z = 0.0f;

	// Unfortunately we cannot simply read what we need directly
	// into the CNpc instance. We have to resort to creating
	// copies of the data to allow for the way they handle multiple spawns...
	// Best we can do, I think, is to avoid allocating it on the stack.
	static uint8	bNumNpc, bZoneID, bActType, bRegenType, bDungeonFamily, bSpecialType,
		bTrapNumber, bDirection, bDotCnt;	
	static uint16	sSid, sRegTime;
	static uint32	nServerNum;
	static int32	iLeftX, iTopZ, iRightX, iBottomZ,
		iLimitMinX, iLimitMinZ, iLimitMaxX, iLimitMaxZ;

	dbCommand->FetchByte(1, bZoneID);
	dbCommand->FetchUInt16(2, sSid);
	dbCommand->FetchByte(3, bActType);
	dbCommand->FetchByte(4, bRegenType);
	dbCommand->FetchByte(5, bDungeonFamily);
	dbCommand->FetchByte(6, bSpecialType);
	dbCommand->FetchByte(7, bTrapNumber);
	dbCommand->FetchInt32(8, iLeftX);
	dbCommand->FetchInt32(9, iTopZ);
	dbCommand->FetchInt32(10, iRightX);
	dbCommand->FetchInt32(11, iBottomZ);
<<<<<<< HEAD
	dbCommand->FetchInt32(12, iLimitMinZ);
	dbCommand->FetchInt32(13, iLimitMinX);
=======
	dbCommand->FetchInt32(12, iLimitMinX);
	dbCommand->FetchInt32(13, iLimitMinZ);
>>>>>>> koserver2
	dbCommand->FetchInt32(14, iLimitMaxX);
	dbCommand->FetchInt32(15, iLimitMaxZ);
	dbCommand->FetchByte(16, bNumNpc);
	dbCommand->FetchUInt16(17, sRegTime);
	dbCommand->FetchByte(18, bDirection);
	dbCommand->FetchByte(19, bDotCnt);
	dbCommand->FetchString(20, szPath, sizeof(szPath));

<<<<<<< HEAD
=======

>>>>>>> koserver2
	uint8 bPathSerial = 1;
	for (uint8 j = 0; j < bNumNpc; j++)
	{
		CNpc * pNpc = new CNpc();

		pNpc->m_byMoveType = bActType;
		pNpc->m_byInitMoveType = bActType;

		bool bMonster = (bActType < 100);
		if (bMonster)
		{
			pNpcTable = m_arMonTable.GetData(sSid);
		}
		else 
		{
			pNpc->m_byMoveType = bActType - 100;
			pNpcTable = m_arNpcTable.GetData(sSid);
		}

		if (pNpcTable == nullptr)
		{
			printf("NPC %d not found in %s table.\n", sSid, bMonster ? "K_MONSTER" : "K_NPC");
			delete pNpc;
			return false;
		}

		pNpc->Load(++m_TotalNPC, pNpcTable, bMonster);
		pNpc->m_byBattlePos = 0;

		if (pNpc->m_byMoveType >= 2)
		{
			pNpc->m_byBattlePos = myrand(1, 3);
			pNpc->m_byPathCount = bPathSerial++;
		}

		pNpc->InitPos();

		pNpc->m_bZone = bZoneID;
<<<<<<< HEAD
		pNpc->m_bEventRoom = 0;
=======
>>>>>>> koserver2

		nRandom = abs(iLeftX - iRightX);
		if (nRandom <= 1)
			fRandom_X = (float)iLeftX;
		else
		{
			if (iLeftX < iRightX)
				fRandom_X = (float)myrand(iLeftX, iRightX);
			else
				fRandom_X = (float)myrand(iRightX, iLeftX);
		}

		nRandom = abs(iTopZ - iBottomZ);
		if (nRandom <= 1)
			fRandom_Z = (float)iTopZ;
		else
		{
			if (iTopZ < iBottomZ)
				fRandom_Z = (float)myrand(iTopZ, iBottomZ);
			else
				fRandom_Z = (float)myrand(iBottomZ, iTopZ);
		}

		pNpc->SetPosition(fRandom_X, 0.0f, fRandom_Z);

		pNpc->m_sRegenTime		= sRegTime * SECOND;
		pNpc->m_byDirection		= bDirection;
		pNpc->m_sMaxPathCount	= bDotCnt;

		if ((pNpc->m_byMoveType == 2 || pNpc->m_byMoveType == 3) && bDotCnt == 0)
		{
			pNpc->m_byMoveType = 1;
			TRACE("##### ServerDlg:CreateNpcThread - Path type Error :  nid=%d, sid=%d, name=%s, acttype=%d, path=%d #####\n", 
				pNpc->GetID(), pNpc->GetProtoID(), pNpc->GetName().c_str(), pNpc->m_byMoveType, pNpc->m_sMaxPathCount);
		}

		if (bDotCnt > 0)
		{
			int index = 0;
			for (int l = 0; l < bDotCnt; l++)
			{
				static char szX[5], szZ[5];

				memset(szX, 0, sizeof(szX));
				memset(szZ, 0, sizeof(szZ));

				memcpy(szX, szPath + index, 4);
				index += 4;

				memcpy(szZ, szPath + index, 4);
				index += 4;

				pNpc->m_PathList.pPattenPos[l].x = atoi(szX);
				pNpc->m_PathList.pPattenPos[l].z = atoi(szZ);
			}
		}

		pNpc->m_nInitMinX = pNpc->m_nLimitMinX		= iLeftX;
		pNpc->m_nInitMinY = pNpc->m_nLimitMinZ		= iTopZ;
		pNpc->m_nInitMaxX = pNpc->m_nLimitMaxX		= iRightX;
		pNpc->m_nInitMaxY = pNpc->m_nLimitMaxZ		= iBottomZ;

		// dungeon work
		pNpc->m_byDungeonFamily	= bDungeonFamily;
		pNpc->m_bySpecialType	= (NpcSpecialType) bSpecialType;
		pNpc->m_byRegenType		= bRegenType;
		pNpc->m_byTrapNumber    = bTrapNumber;

<<<<<<< HEAD
=======
		pNpc->m_oSocketID = -1;
		pNpc->m_bEventRoom = 0;

>>>>>>> koserver2
		if (pNpc->m_byDungeonFamily > 0)
		{
			pNpc->m_nLimitMinX = iLimitMinX;
			pNpc->m_nLimitMinZ = iLimitMinZ;
			pNpc->m_nLimitMaxX = iLimitMaxX;
			pNpc->m_nLimitMaxZ = iLimitMaxZ;
		}	

		pNpc->m_pMap = GetZoneByID(pNpc->GetZoneID());
		if (pNpc->GetMap() == nullptr)
		{
<<<<<<< HEAD
			printf(_T("ERROR: NPC %d in zone %d that does not exist.\n"), sSid, bZoneID);
			//CodeChange by zealot
			/*delete pNpc;
			return false;*/
			continue;
=======
			printf(_T("ERROR: NPC %d in zone %d that does not exist."), sSid, bZoneID);
			delete pNpc;
			return false;
>>>>>>> koserver2
		}

		if (!m_arNpc.PutData(pNpc->GetID(), pNpc))
		{
			--m_TotalNPC;
			TRACE("Npc PutData Fail - %d\n", pNpc->GetID());
			delete pNpc;
			continue;
		}

		if (pNpc->GetMap()->m_byRoomEvent > 0 && pNpc->m_byDungeonFamily > 0)
		{
			pRoom = pNpc->GetMap()->m_arRoomEventArray.GetData(pNpc->m_byDungeonFamily);
			if (pRoom == nullptr)
			{
				printf("ERROR: Map Room Npc Fail!!\n");
				delete pNpc;
				return false;
			}

			// this is why their CSTLMap class sucks.
			int *pInt = new int;
			*pInt = pNpc->GetID();
			if (!pRoom->m_mapRoomNpcArray.PutData(*pInt, pInt))
			{
				delete pInt;
				TRACE("### Map - Room Array MonsterNid Fail : nid=%d, sid=%d ###\n", 
					pNpc->GetID(), pNpc->GetProtoID());
			}
		}
	}
<<<<<<< HEAD

=======
>>>>>>> koserver2
	return true;
}

void CServerDlg::ResumeAI()
{
<<<<<<< HEAD
	foreach (itr, m_arNpcThread)
		itr->second->m_thread.start(NpcThreadProc, itr->second);

	FastGuard lock(m_eventThreadLock);
=======
	Guard lock(m_npcThreadLock);
	foreach (itr, m_arNpcThread)
		itr->second->m_thread.start(NpcThreadProc, itr->second);

	Guard lock2(m_eventThreadLock);
>>>>>>> koserver2
	foreach (itr, m_arEventNpcThread)
		itr->second->m_thread.start(NpcThreadProc, itr->second);

	m_zoneEventThread.start(ZoneEventThreadProc, this);
}

bool CServerDlg::MapFileLoad()
{
	ZoneInfoMap zoneMap;

	m_sTotalMap = 0;
<<<<<<< HEAD
	LOAD_TABLE_ERROR_ONLY(CZoneInfoSet, &m_GameDB, &zoneMap, false); 
=======
	LOAD_TABLE_ERROR_ONLY(CZoneInfoSet, &m_GameDB, &zoneMap, false, false); 
>>>>>>> koserver2

	foreach (itr, zoneMap)
	{
		_ZONE_INFO *pZone = itr->second;

		MAP *pMap = new MAP();
		if (!pMap->Initialize(pZone))
		{
			printf("ERROR: Unable to load SMD - %s\n", pZone->m_MapName.c_str());
			delete pZone;
			delete pMap;
			g_arZone.DeleteAllData();
			m_sTotalMap = 0;
			return false;
		}

		delete pZone;
		g_arZone.PutData(pMap->m_nZoneNumber, pMap);
		m_sTotalMap++;
	}

	return true;
}

/**
* @brief	Gets & formats a cached server resource (_SERVER_RESOURCE entry).
*
* @param	nResourceID	Identifier for the resource.
* @param	result	   	The string to store the formatted result in.
*/
void CServerDlg::GetServerResource(int nResourceID, string * result, ...)
{
	_SERVER_RESOURCE *pResource = m_ServerResourceArray.GetData(nResourceID);
	if (pResource == nullptr)
	{
		*result = nResourceID;
		return;
	}

	va_list args;
	va_start(args, result);
	_string_format(pResource->strResource, result, args);
	va_end(args);
}

// game server에 모든 npc정보를 전송..
void CServerDlg::AllNpcInfo()
{
	Packet result(NPC_INFO_ALL);
	result.SByte();
	foreach_stlmap (itr, g_arZone)
	{
		uint32 nZone = itr->first;
		uint8 bCount = 0;

		result.clear();
		result << bCount;

		foreach_stlmap (itr, m_arNpc)
		{
			CNpc *pNpc = itr->second;
			if (pNpc == nullptr
				|| pNpc->GetZoneID() != nZone)	
				continue;

			pNpc->FillNpcInfo(result);
			if (++bCount == NPC_NUM)
			{
				result.put(0, bCount);
				m_socketMgr.SendAllCompressed(&result);

				// Reset packet buffer
				bCount = 0;
				result.clear();
				result << bCount;
			}
		}	

		if (bCount != 0 && bCount < NPC_NUM)
		{
			result.put(0, bCount);
			m_socketMgr.SendAllCompressed(&result);
		}

		Packet serverInfo(AG_SERVER_INFO, uint8(nZone));
		serverInfo << uint16(m_TotalNPC);
		m_socketMgr.SendAll(&serverInfo);
	}
}

Unit * CServerDlg::GetUnitPtr(uint16 id)
{
	if (id < NPC_BAND)
		return GetUserPtr(id);

	return GetNpcPtr(id);
}

CNpc * CServerDlg::GetNpcPtr(uint16 npcId)
{
	return m_arNpc.GetData(npcId);
}

CUser* CServerDlg::GetUserPtr(uint16 sessionId)
{
<<<<<<< HEAD
	FastGuard lock(m_userLock);
=======
	Guard lock(m_userLock);
>>>>>>> koserver2
	auto itr = m_pUser.find(sessionId);
	if (itr == m_pUser.end())
		return nullptr;

	return itr->second;
}

bool CServerDlg::SetUserPtr(uint16 sessionId, CUser * pUser)
{
	if (sessionId >= MAX_USER)
		return false;

<<<<<<< HEAD
	FastGuard lock(m_userLock);
=======
	Guard lock(m_userLock);
>>>>>>> koserver2
	auto itr = m_pUser.find(sessionId);
	if (itr != m_pUser.end())
	{
		TRACE("Warning: User %u has not been removed from the session map.\n", sessionId);
		return false; 
	}

	m_pUser[sessionId] = pUser;
	return true;
}

void CServerDlg::DeleteUserPtr(uint16 sessionId)
{
<<<<<<< HEAD
	FastGuard lock(m_userLock);
=======
	Guard lock(m_userLock);
>>>>>>> koserver2
	auto itr = m_pUser.find(sessionId);
	if (itr != m_pUser.end())
	{
		delete itr->second;
		m_pUser.erase(itr);
	}
}

<<<<<<< HEAD
=======
void CServerDlg::CheckAliveTest()
{
	Packet result(AG_CHECK_ALIVE_REQ);
	SessionMap sessMap = m_socketMgr.GetActiveSessionMap();
	uint32 count = 0, sessCount = sessMap.size();
	foreach (itr, sessMap)
	{
		if (itr->second->Send(&result))
			count++;
	}

	if (sessCount > 0 && count == 0)
		DeleteAllUserList();
}

>>>>>>> koserver2
uint32 THREADCALL CServerDlg::Timer_CheckAliveTest(void * lpParam)
{
	while (g_bRunning)
	{
		g_pMain->CheckAliveTest();
		sleep(10 * SECOND);
	}
	return 0;
}

<<<<<<< HEAD
void CServerDlg::CheckAliveTest()
{
	Packet result(AG_CHECK_ALIVE_REQ);
	SessionMap & sessMap = m_socketMgr.GetActiveSessionMap();
	uint32 count = 0, sessCount = sessMap.size();
	foreach (itr, sessMap)
	{
		if (itr->second->Send(&result))
			count++;
	}
	m_socketMgr.ReleaseLock();

	if (sessCount > 0 && count == 0)
		DeleteAllUserList();
=======
uint32 THREADCALL CServerDlg::Timer_CheckLiveTimes(void * lpParam)
{
	while (g_bRunning)
	{
		g_pMain->CheckLiveTimes();
		sleep(1 * SECOND);
	}
	return 0;
}

void CServerDlg::CheckLiveTimes()
{
	std::vector<uint16> deleted;

	foreach_stlmap_nolock (itr, m_NpcLiveTimeArray)
	{
		if (int32(UNIXTIME) - itr->second->SpawnedTime > itr->second->Duration)
		{
			CNpc *pNpc = GetNpcPtr(itr->second->Nid);

			if (pNpc)
				pNpc->Dead();

			deleted.push_back(itr->second->nIndex);
		}
	}

	foreach (itr, deleted)
		m_NpcLiveTimeArray.DeleteData(*itr);
>>>>>>> koserver2
}

void CServerDlg::DeleteAllUserList(CGameSocket *pSock)
{
	// If a server disconnected, show it...
	if (pSock != nullptr)
	{
<<<<<<< HEAD
		printf("Game Server disconnected - %s]\n", pSock->GetRemoteIP().c_str());
=======
		printf("Game Server disconnected - %s\n", pSock->GetRemoteIP().c_str());
>>>>>>> koserver2
		return;
	}

	// Server didn't disconnect? 
	if (!m_bFirstServerFlag)
		return;

	// If there's no servers even connected, cleanup.
	TRACE("*** DeleteAllUserList - Start *** \n");
	foreach_stlmap (itr, g_arZone)
	{
		MAP * pMap = itr->second;
		if (pMap == nullptr)	
			continue;
		for (int i = 0; i < pMap->GetXRegionMax(); i++)
		{
			for (int j = 0; j < pMap->GetZRegionMax(); j++)
				pMap->m_ppRegion[i][j].m_RegionUserArray.DeleteAllData();
		}
	}

<<<<<<< HEAD
	FastGuard lock(m_userLock);
=======
	Guard lock(m_userLock);
>>>>>>> koserver2
	foreach (itr, m_pUser)
	{
		if (itr->second == nullptr)  
			continue;

		delete itr->second;
	}
	m_pUser.clear();

	// Party Array Delete 
	m_arParty.DeleteAllData();

	m_bFirstServerFlag = false;
	TRACE("*** DeleteAllUserList - End *** \n");

	printf("Delete All User List\n");
}

void CServerDlg::Send(Packet * pkt)
{
	m_socketMgr.SendAll(pkt);
}

void CServerDlg::GameServerAcceptThread()
{
	m_socketMgr.RunServer();
}

bool CServerDlg::AddObjectEventNpc(_OBJECT_EVENT* pEvent, MAP * pMap)
{
<<<<<<< HEAD
	int sSid = (pEvent->sType == OBJECT_ANVIL || pEvent->sType == OBJECT_ARTIFACT 
		? pEvent->sIndex : pEvent->sControlNpcID);
=======
	int sSid = 0;

	if (pEvent->sType == OBJECT_GATE 
		|| pEvent->sType == OBJECT_GATE2
		|| pEvent->sType == OBJECT_GATE_LEVER
		|| pEvent->sType == OBJECT_ANVIL 
		|| pEvent->sType == OBJECT_ARTIFACT)
		sSid = pEvent->sIndex;
	else 
		sSid =pEvent->sControlNpcID;

>>>>>>> koserver2
	if (sSid <= 0)
		return false;

	CNpcTable * pNpcTable = m_arNpcTable.GetData(sSid);
	if(pNpcTable == nullptr)	{
		// TRACE("#### AddObjectEventNpc Fail : [sid = %d], zone=%d #####\n", pEvent->sIndex, zone_number);
		return false;
	}

	CNpc *pNpc = new CNpc();

	pNpc->m_byMoveType = 0;
	pNpc->m_byInitMoveType = 0;

	pNpc->m_byBattlePos = 0;

	pNpc->m_byObjectType = SPECIAL_OBJECT;
	pNpc->m_byGateOpen	= (pEvent->sStatus == 1);

	pNpc->m_bZone	= pMap->m_nZoneNumber;
	pNpc->SetPosition(pEvent->fPosX, pEvent->fPosY, pEvent->fPosZ);

	pNpc->m_nInitMinX	= (int)pEvent->fPosX-1;
	pNpc->m_nInitMinY	= (int)pEvent->fPosZ-1;
	pNpc->m_nInitMaxX	= (int)pEvent->fPosX+1;
	pNpc->m_nInitMaxY	= (int)pEvent->fPosZ+1;	

	pNpc->Load(m_sMapEventNpc++, pNpcTable, false);
	pNpc->m_pMap = pMap;

	if (pNpc->GetMap() == nullptr
		|| !m_arNpc.PutData(pNpc->GetID(), pNpc))
	{
		m_sMapEventNpc--;
		TRACE("Npc PutData Fail - %d\n", pNpc->GetID());
		delete pNpc;
		return false;
	}

	m_TotalNPC = m_sMapEventNpc;
	return true;
}

<<<<<<< HEAD
CNpc * CServerDlg::SpawnEventNpc(uint16 sSid, bool bIsMonster, uint8 byZone, float fX, float fY, float fZ, int16 nEventRoom)
=======
CNpc * CServerDlg::SpawnEventNpc(uint16 sSid, bool bIsMonster, uint8 byZone, float fX, float fY, float fZ, uint16 sDuration, uint8 nation, int16 socketID, uint16 nEventRoom)
>>>>>>> koserver2
{
	CNpcTable * proto = nullptr;
	MAP * pZone = GetZoneByID(byZone);

	if (pZone == nullptr)
		return nullptr;

	if (bIsMonster)
		proto = m_arMonTable.GetData(sSid);
	else
		proto = m_arNpcTable.GetData(sSid);

	if (proto == nullptr)
		return nullptr;

<<<<<<< HEAD
	FastGuard lock(m_eventThreadLock);
=======
	Guard lock(m_eventThreadLock);
>>>>>>> koserver2
	auto itr = m_arEventNpcThread.find(byZone);
	if (itr == m_arEventNpcThread.end())
		return false;

	CNpc * pNpc = new CNpc();

	pNpc->m_bIsEventNpc = true;
	pNpc->m_byMoveType = (bIsMonster ? 1 : 0);
	pNpc->m_byInitMoveType = 1;
	pNpc->m_byBattlePos = 0;

	pNpc->m_bZone = byZone;
	pNpc->SetPosition(fX, fY, fZ);
	pNpc->m_pMap = pZone;
<<<<<<< HEAD
	pNpc->m_bEventRoom = nEventRoom;

	pNpc->Load(++m_TotalNPC, proto, bIsMonster);
=======
	pNpc->m_oSocketID = socketID;
	pNpc->m_bEventRoom = nEventRoom;

	pNpc->Load(++m_TotalNPC, proto, bIsMonster, nation);
>>>>>>> koserver2
	pNpc->InitPos();

	itr->second->AddNPC(pNpc);
	m_arNpc.PutData(pNpc->GetID(), pNpc);

<<<<<<< HEAD
=======
	if (sDuration > 0) // Duration npc or monsters
	{
		_NPC_LIVE_TIME * pData = new _NPC_LIVE_TIME();
		pData->nIndex = m_NpcLiveTimeArray.GetSize() + 1;
		pData->SocketID = socketID;
		pData->Nid = pNpc->m_sNid;
		pData->Duration = sDuration;
		pData->SpawnedTime = int32(UNIXTIME);
		if (!m_NpcLiveTimeArray.PutData(pData->nIndex,pData))
			delete pData;
	}

>>>>>>> koserver2
	return pNpc;
}

void CServerDlg::RemoveEventNPC(CNpc * pNpc)
{
<<<<<<< HEAD
	FastGuard lock(m_eventThreadLock);
=======
	Guard lock(m_eventThreadLock);
>>>>>>> koserver2
	auto itr = m_arEventNpcThread.find(pNpc->GetZoneID());
	if (itr == m_arEventNpcThread.end())
		return;

	itr->second->RemoveNPC(pNpc);
}

void CServerDlg::NpcUpdate(uint16 sSid, bool bIsMonster, uint8 byGroup, uint16 sPid)
{
	CNpcTable * proto = nullptr;

	if (bIsMonster)
		proto = m_arMonTable.GetData(sSid);
	else
		proto = m_arNpcTable.GetData(sSid);

	if (proto == nullptr)
		return;

	if (byGroup > 0)
		proto->m_byGroupSpecial = byGroup;

	if (sPid > 0)
		proto->m_sPid = sPid;
}

MAP * CServerDlg::GetZoneByID(int zonenumber)
{
	return g_arZone.GetData(zonenumber);
}

void CServerDlg::GetServerInfoIni()
{
	CIni ini("./AIServer.ini");
	ini.GetString("ODBC", "GAME_DSN", "KO_GAME", m_strGameDSN, false);
	ini.GetString("ODBC", "GAME_UID", "username", m_strGameUID, false);
	ini.GetString("ODBC", "GAME_PWD", "password", m_strGamePWD, false);
<<<<<<< HEAD
=======

	m_AIServerPort = ini.GetInt("SETTINGS","PORT", 10020);
>>>>>>> koserver2
}

void CServerDlg::SendSystemMsg(std::string & pMsg, int type)
{
	Packet result(AG_SYSTEM_MSG, uint8(type));
	result << pMsg;
	Send(&result);
}

void CServerDlg::ResetBattleZone()
{
	TRACE("ServerDlg - ResetBattleZone() : start \n");
	foreach_stlmap (itr, g_arZone)
	{
		MAP *pMap = itr->second;
		if (pMap == nullptr || pMap->m_byRoomEvent == 0) 
			continue;
		//if( pMap->IsRoomStatusCheck() == true )	continue;	// 전체방이 클리어 되었다면
		pMap->InitializeRoom();
	}
	TRACE("ServerDlg - ResetBattleZone() : end \n");
}

CServerDlg::~CServerDlg() 
{
	g_bNpcExit = true;

	printf("Waiting for NPC threads to exit...");
<<<<<<< HEAD
=======

	Guard lock(m_npcThreadLock);
>>>>>>> koserver2
	foreach (itr, m_arNpcThread)
	{
		CNpcThread * pThread = itr->second;
		pThread->m_thread.waitForExit();
		delete pThread;
	}
	m_arNpcThread.clear();

<<<<<<< HEAD
	FastGuard lock(m_eventThreadLock);
=======
	Guard lock2(m_eventThreadLock);
>>>>>>> koserver2
	foreach (itr, m_arEventNpcThread)
	{
		CNpcThread * pThread = itr->second;
		pThread->m_thread.waitForExit();
		delete pThread;
	}
	m_arEventNpcThread.clear();

	printf(" exited.\n");

	printf("Waiting for zone event thread to exit...");
	m_zoneEventThread.waitForExit();
	printf(" exited.\n");

	printf("Waiting for timer threads to exit...");
	foreach (itr, g_timerThreads)
	{
		(*itr)->waitForExit();
		delete (*itr);
	}
	printf(" exited.\n");

	printf("Freeing user sessions...");
	for (int i = 0; i < MAX_USER; i++)
	{
		if (m_pUser[i] != nullptr)
		{
			delete m_pUser[i];
			m_pUser[i] = nullptr;
		}
	}
	printf(" done.\n");

	m_ZoneNpcList.clear();

	printf("Shutting down socket system...");
	m_socketMgr.Shutdown();
	printf(" done.\n");
<<<<<<< HEAD
}
=======
}
>>>>>>> koserver2
