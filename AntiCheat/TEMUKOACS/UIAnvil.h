#pragma once
#include "stdafx.h"
class CUIAnvil;
#include "TEMUKOACS Engine.h"
#include "Models/MerchantInvSlot.h"

class CUIAnvil
{
public:
	CUIAnvil();
	~CUIAnvil();

	void ParseUIElements();

	DWORD m_dVTableAddr;
	DWORD m_txtUprate;
	DWORD m_areaUpgrade;

	void SetResult(std::string result);

private:

};