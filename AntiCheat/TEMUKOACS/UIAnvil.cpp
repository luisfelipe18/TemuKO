#include "UIAnvil.h"

CUIAnvil::CUIAnvil()
{
	vector<int>offsets;
	offsets.push_back(0x258);
	offsets.push_back(0);

	m_dVTableAddr = Engine->rdword(KO_DLG, offsets);

	ParseUIElements();
}

CUIAnvil::~CUIAnvil()
{
}

void CUIAnvil::ParseUIElements()
{
	std::string find = xorstr("txt_uprate");
	Engine->GetChildByID(m_dVTableAddr, find, m_txtUprate);
	find = xorstr("a_upgrade");
	Engine->GetChildByID(m_dVTableAddr, find, m_areaUpgrade);
}

void CUIAnvil::SetResult(std::string result)
{
	Engine->SetString(m_txtUprate, result);
}