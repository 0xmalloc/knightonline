#pragma once

#include <map>
<<<<<<< HEAD
#include "Mutex.h"
=======
>>>>>>> koserver2

template <class T> 
class CSTLMap  
{
public:
	typedef typename std::map<uint32, T*>::iterator Iterator;
	std::map<uint32, T*> m_UserTypeMap;
<<<<<<< HEAD
	FastMutex m_lock;

	int GetSize()
	{
		FastGuard lock(m_lock);
=======
	std::recursive_mutex m_lock;

	int GetSize()
	{
		Guard lock(m_lock);
>>>>>>> koserver2
		return m_UserTypeMap.size(); 
	}

	bool IsExist(uint32 key)
	{
<<<<<<< HEAD
		FastGuard lock(m_lock);
=======
		Guard lock(m_lock);
>>>>>>> koserver2
		return (m_UserTypeMap.find(key) != m_UserTypeMap.end()); 
	}

	bool IsEmpty() 
	{ 
<<<<<<< HEAD
		FastGuard lock(m_lock);
=======
		Guard lock(m_lock);
>>>>>>> koserver2
		return m_UserTypeMap.empty(); 
	}

	bool PutData(uint32 key_value, T* pData) 
	{
<<<<<<< HEAD
		FastGuard lock(m_lock);
=======
		Guard lock(m_lock);
>>>>>>> koserver2
		return m_UserTypeMap.insert(std::make_pair(key_value, pData)).second; 
	}

	T* GetData(uint32 key_value)
	{
<<<<<<< HEAD
		FastGuard lock(m_lock);
=======
		Guard lock(m_lock);
>>>>>>> koserver2
		auto itr = m_UserTypeMap.find(key_value);
		return (itr != m_UserTypeMap.end() ? itr->second : nullptr);
	}

	void DeleteData(uint32 key_value)
	{
<<<<<<< HEAD
		FastGuard lock(m_lock);
=======
		Guard lock(m_lock);
>>>>>>> koserver2
		auto itr = m_UserTypeMap.find(key_value);
		if (itr!= m_UserTypeMap.end())
		{
			delete itr->second;
			m_UserTypeMap.erase(itr);
		}
	}

	void DeleteAllData()
	{
<<<<<<< HEAD
		FastGuard lock(m_lock);
=======
		Guard lock(m_lock);
>>>>>>> koserver2
		if (m_UserTypeMap.empty())
			return;

		foreach (itr, m_UserTypeMap)
			delete itr->second;

		m_UserTypeMap.clear();
	}

	~CSTLMap() { DeleteAllData(); }
};