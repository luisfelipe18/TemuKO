#pragma once

#include <set>
#include "Define.h"
#include "GameDefine.h"
#include "../shared/STLMap.h"

typedef CSTLMap <_LOOT_BUNDLE>	ZoneItemArray;
typedef std::set<uint16>    ZoneUserArray;
typedef std::set<uint16>    ZoneNpcArray;
typedef std::set<uint16>    ZoneBotArray;

class CNpc;
class CUser;
class CBot;

class CRegion  
{
public:
	CRegion() : m_byMoving(0) {}
	std::recursive_mutex		m_lock;
	std::recursive_mutex		m_lockUserArray;
	std::recursive_mutex		m_lockNpcArray;
	std::recursive_mutex		m_lockBotArray;
	ZoneItemArray	m_RegionItemArray;
	ZoneUserArray	m_RegionUserArray;
	ZoneNpcArray	m_RegionNpcArray;
	ZoneBotArray	m_RegionBotArray;
	uint8	m_byMoving;			// move : 1, not moving : 0

	void Add(CUser * pUser);
	void Remove(CUser * pUser);
	void Add(CNpc * pNpc);
	void Remove(CNpc * pNpc);
	void Add(CBot* pBot);
	void Remove(CBot* pBot);
};
