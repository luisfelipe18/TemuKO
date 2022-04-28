#include "stdafx.h"
#include "DropGroupList.h"

CDropGroupList::CDropGroupList(int zone, int mob, int group, int percent)
{
	m_iZoneID = zone;
	m_iMobID = mob;
	m_iGroupID = group;
	m_iPercent = percent;
}

CDropGroupList::~CDropGroupList()
{
}