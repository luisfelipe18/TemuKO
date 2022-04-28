#pragma once
#include "stdafx.h"
class CUITooltip;
#include "TEMUKOACS Engine.h"

class CUITooltip
{
public:
	CUITooltip();
	~CUITooltip();

	DWORD string0;

	void ParseUIElements();

	DWORD m_dVTableAddr;

private:

};