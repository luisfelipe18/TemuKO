#pragma once
class CUITradeItemDiplayPlug;
#include "stdafx.h"
#include "TEMUKOACS Engine.h"
#include "Models/MerchantInvSlot.h"

class CUITradeItemDiplayPlug
{
public:
	DWORD m_dVTableAddr;
	MerchantInvSlotList m_Slots;

public:
	CUITradeItemDiplayPlug();
	~CUITradeItemDiplayPlug();
	void ParseUIElements();
	DWORD GetSlotVTable(int id);

private:

};