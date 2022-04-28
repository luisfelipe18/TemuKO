#pragma once

#include "ItemOrg.h"
#include "N3BASE/GameDef.h"

class CDropItemList
{
public:
	int m_iItemID;
	__TABLE_ITEM_BASIC* m_ItemOrg;

public:
	CDropItemList(int item);
	~CDropItemList();

private:

};

typedef std::list<CDropItemList*>			DropItemList;
typedef DropItemList::iterator				DropItemListItor;
typedef DropItemList::const_iterator		DropItemListItorConst;
typedef DropItemList::reverse_iterator		DropItemListReverseItor;