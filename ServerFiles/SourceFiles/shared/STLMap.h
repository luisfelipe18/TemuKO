#pragma once

#include <map>

template <class T, class Key = uint32> 
class CSTLMap  
{
public:
	typedef typename std::map<Key, T*>::iterator Iterator;
	std::map<Key, T*> m_UserTypeMap;
	std::recursive_mutex m_lock;

	int GetSize()
	{
		Guard lock(m_lock);
		return (int) m_UserTypeMap.size(); 
	}

	bool IsExist(Key key)
	{
		Guard lock(m_lock);
		return (m_UserTypeMap.find(key) != m_UserTypeMap.end()); 
	}

	bool IsEmpty() 
	{ 
		Guard lock(m_lock);
		return m_UserTypeMap.empty(); 
	}

	bool PutData(Key key_value, T* pData) 
	{
		Guard lock(m_lock);
		return m_UserTypeMap.insert(std::make_pair(key_value, pData)).second; 
	}

	T* GetData(Key key_value)
	{
		Guard lock(m_lock);
		auto itr = m_UserTypeMap.find(key_value);
		return (itr != m_UserTypeMap.end() ? itr->second : nullptr);
	}

	void DeleteData(Key key_value)
	{
		Guard lock(m_lock);
		auto itr = m_UserTypeMap.find(key_value);
		if (itr!= m_UserTypeMap.end())
		{
			if(itr->second)
				delete itr->second;
			m_UserTypeMap.erase(itr);
		}
	}

	void DeleteAllData()
	{
		Guard lock(m_lock);
		if (m_UserTypeMap.empty())
			return;

		foreach (itr, m_UserTypeMap)
		{
			if(itr->second)
				delete itr->second;
		}

		m_UserTypeMap.clear();
	}

	~CSTLMap() { DeleteAllData(); }
};