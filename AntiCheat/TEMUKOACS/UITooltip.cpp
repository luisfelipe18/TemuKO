#include "UITooltip.h"

CUITooltip::CUITooltip()
{
	vector<int>offsets;
	offsets.push_back(0x1BC);
	offsets.push_back(0x124);
	offsets.push_back(0x1A4);
	offsets.push_back(0x0);

	m_dVTableAddr = Engine->rdword(KO_DLG, offsets);

	ParseUIElements();
}

CUITooltip::~CUITooltip()
{
}

void CUITooltip::ParseUIElements()
{
	std::string find = xorstr("string_0");
	Engine->GetChildByID(m_dVTableAddr, find, string0);
}