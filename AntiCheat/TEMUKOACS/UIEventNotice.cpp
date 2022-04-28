#include "UIEventNotice.h"

CUIEventNoticePlug::CUIEventNoticePlug()
{
	vector<int>offsets;
	offsets.push_back(0x370);
	offsets.push_back(0xc4);
	offsets.push_back(0);
	offsets.push_back(0x8);
	offsets.push_back(0);

	m_dVTableAddr = Engine->rdword(KO_DLG, offsets);
}

CUIEventNoticePlug::~CUIEventNoticePlug()
{
}

POINT CUIEventNoticePlug::GetPos()
{
	POINT curPt;
	Engine->GetUiPos(m_dVTableAddr, curPt);

	return curPt;
}