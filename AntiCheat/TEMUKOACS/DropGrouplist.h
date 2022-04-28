#pragma once

#include "stdafx.h"

class CDropGroupList
{
public:
	int m_iZoneID;
	int m_iMobID;
	int m_iGroupID;
	int m_iPercent;

public:
	CDropGroupList(int zone, int mob, int group, int percent);
	~CDropGroupList();

private:

};

typedef std::list<CDropGroupList*>			DropGroupList;
typedef DropGroupList::iterator				DropGroupListItor;
typedef DropGroupList::const_iterator		DropGroupListItorConst;
typedef DropGroupList::reverse_iterator		DropGroupListReverseItor;