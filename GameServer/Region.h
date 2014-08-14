#pragma once

#include <set>
#include "Define.h"
#include "GameDefine.h"
#include "../shared/STLMap.h"

typedef CSTLMap <_LOOT_BUNDLE>	ZoneItemArray;
<<<<<<< HEAD
typedef std::set<uint16>		ZoneUserArray;
typedef std::set<uint16>		ZoneNpcArray;
=======
typedef std::set<uint16>    ZoneUserArray;
typedef std::set<uint16>    ZoneNpcArray;
>>>>>>> koserver2

class CNpc;
class CUser;

class CRegion  
{
public:
<<<<<<< HEAD
	FastMutex		m_lock;
=======
	std::recursive_mutex		m_lock;
>>>>>>> koserver2
	ZoneItemArray	m_RegionItemArray;
	ZoneUserArray	m_RegionUserArray;
	ZoneNpcArray	m_RegionNpcArray;

	void Add(CUser * pUser);
	void Remove(CUser * pUser);
	void Add(CNpc * pNpc);
	void Remove(CNpc * pNpc);
};
