#pragma once
#include "stdafx.h"
class CUIHPBarPlug;
#include "TEMUKOACS Engine.h"

class CUIHPBarPlug
{
public:
	CUIHPBarPlug();
	~CUIHPBarPlug();

	void ParseUIElements();

	DWORD m_dVTableAddr;
	DWORD text_hp;
	DWORD text_mp;

	void HPChange(uint16 hp, uint16 maxhp);
	void MPChange(uint16 mp, uint16 maxmp);
};