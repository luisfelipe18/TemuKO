#include "UIHPBar.h"

CUIHPBarPlug::CUIHPBarPlug()
{
	m_dVTableAddr = NULL;

	vector<int>offsets;
	offsets.push_back(0x390);
	offsets.push_back(0);

	m_dVTableAddr = Engine->rdword(KO_DLG, offsets);

	ParseUIElements();
}

CUIHPBarPlug::~CUIHPBarPlug()
{
}

void CUIHPBarPlug::ParseUIElements()
{
	std::string find = xorstr("Text_HP");
	Engine->GetChildByID(m_dVTableAddr, find, text_hp);
	find = xorstr("Text_MSP");
	Engine->GetChildByID(m_dVTableAddr, find, text_mp);
}

void CUIHPBarPlug::HPChange(uint16 hp, uint16 maxhp)
{
	if (maxhp == 0)
		return;
	if (m_dVTableAddr != NULL) {
		std::string str = xorstr("%s/%s (%d%%)");
		int percent = (int)ceil((hp * 100) / maxhp);
		std::string max = Engine->StringHelper->NumberFormat(maxhp);
		std::string cur = Engine->StringHelper->NumberFormat(hp);
		Engine->SetString(text_hp, string_format(str, cur.c_str(), max.c_str(), percent));
	}
}

void CUIHPBarPlug::MPChange(uint16 mp, uint16 maxmp)
{
	if (maxmp == 0)
		return;
	if (m_dVTableAddr != NULL) {
		std::string str = xorstr("%s/%s (%d%%)");
		int percent = (int)ceil((mp * 100) / maxmp);
		std::string max = Engine->StringHelper->NumberFormat(maxmp);
		std::string cur = Engine->StringHelper->NumberFormat(mp);
		Engine->SetString(text_mp, string_format(str, cur.c_str(), max.c_str(), percent));
	}
}