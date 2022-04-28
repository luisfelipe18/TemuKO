#include "stdafx.h"
#include "UIGenieSub.h"

CUIGenieSubPlug::CUIGenieSubPlug()
{
	vector<int>offsets;
	offsets.push_back(0x510);
	offsets.push_back(0x140);
	offsets.push_back(0xBC);
	offsets.push_back(0x154);
	offsets.push_back(0xBC);
	offsets.push_back(0xBC);
	offsets.push_back(0);

	m_dVTableAddr = Engine->rdword(KO_DLG, offsets);
}

CUIGenieSubPlug::~CUIGenieSubPlug()
{
}