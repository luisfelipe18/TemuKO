#include "stdafx.h"
#include "DropListItem.h"

CUIDropListItem::CUIDropListItem(CN3UIBase* parent)
{
	std::string notFound = "NULL UI Component!!";
	m_grpParent = parent;													__ASSERT(m_grpParent, notFound);

	std::string find = "area_slot";
	m_areaItem = (CN3UIArea*)parent->GetChildByID(find);					__ASSERT(m_areaItem, notFound);

	find = "img_slot";
	m_imgIcon = (CN3UIImage*)parent->GetChildByID(find);					__ASSERT(m_imgIcon, notFound);
}

CUIDropListItem::~CUIDropListItem()
{
}

void CUIDropListItem::Clear()
{
	szIconFN = "";

	if (pUIIcon != NULL)
		pUIIcon = NULL;
}

void CUIDropListItem::SetItem(CDropItemList* pItem)
{
	if (pItem->m_ItemOrg == NULL)
		return;

	

	m_CurrentItem = pItem;
	m_CurrentGroup = NULL;

	std::vector<char> buffer(256, NULL);

	sprintf(&buffer[0], "ui\\itemicon_%.1d_%.4d_%.2d_%.1d.dxt",
		(pItem->m_ItemOrg->IconID / 10000000),
		(pItem->m_ItemOrg->IconID / 1000) % 10000,
		(pItem->m_ItemOrg->IconID / 10) % 100,
		pItem->m_ItemOrg->IconID % 10);

	szIconFN = &buffer[0];

	pUIIcon = new CN3UIImage;
	

}
