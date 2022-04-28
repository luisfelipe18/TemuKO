#pragma once

#include "N3BASE/N3UIBase.h"
#include "N3BASE/N3UIString.h"
#include "N3BASE/N3UIImage.h"
#include "N3BASE/N3UIButton.h"
#include "N3BASE/N3UIList.h"
#include "N3BASE/N3UIIcon.h"
#include "N3BASE/N3UIProgress.h"
#include "Timer.h"

#include "DropItemList.h"
#include "DropGroupList.h"

class CUIDropListItem : CN3UIBase
{
public:
	CN3UIBase* m_grpParent;
	CN3UIBase* m_grpSlot;

	CN3UIArea* m_areaItem;
	CN3UIImage* m_imgIcon;

	std::string szIconFN;
	CN3UIImage* pUIIcon;

	int m_iPercent;
	CDropItemList* m_CurrentItem;
	CDropGroupList* m_CurrentGroup;

public:
	CUIDropListItem(CN3UIBase* parent);
	~CUIDropListItem();
	void Clear();
	void SetItem(CDropItemList* pItem);

private:

};

typedef std::list<CUIDropListItem*>			UIDropItemList;
typedef UIDropItemList::iterator			UIDropItemListItor;
typedef UIDropItemList::const_iterator		UIDropItemListItorConst;
typedef UIDropItemList::reverse_iterator	UIDropItemListReverseItor;