#include "stdafx.h"
#include "Region.h"
#include "User.h"
#include "Npc.h"

/**
* @brief	Adds user instance to the region.
*
* @param	pUser	The user to add.
*/
void CRegion::Add(CUser * pUser)
{
<<<<<<< HEAD
	FastGuard lock(m_lock);
=======
	Guard lock(m_lock);
>>>>>>> koserver2
	m_RegionUserArray.insert(pUser->GetID());
}

/**
* @brief	Removes the given user instance from the region.
*
* @param	pUser	The user to remove.
*/
void CRegion::Remove(CUser * pUser)
{
<<<<<<< HEAD
	FastGuard lock(m_lock);
=======
	Guard lock(m_lock);
>>>>>>> koserver2
	m_RegionUserArray.erase(pUser->GetID());
}

/**
* @brief	Adds the given NPC to the region.
*
* @param	pNpc	The NPC to add.
*/
void CRegion::Add(CNpc * pNpc)
{
<<<<<<< HEAD
	FastGuard lock(m_lock);
=======
	Guard lock(m_lock);
>>>>>>> koserver2
	m_RegionNpcArray.insert(pNpc->GetID());
}

/**
* @brief	Removes the given NPC from the region.
*
* @param	pNpc	The NPC to remove.
*/
void CRegion::Remove(CNpc * pNpc)
{
<<<<<<< HEAD
	FastGuard lock(m_lock);
=======
	Guard lock(m_lock);
>>>>>>> koserver2
	m_RegionNpcArray.erase(pNpc->GetID());
}