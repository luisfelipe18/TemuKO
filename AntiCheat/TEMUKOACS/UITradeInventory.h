#pragma once
class CUITradeInventoryPlug;
#include "Models/MerchantInvSlot.h"
#include "TEMUKOACS Engine.h"

class CUITradeInventoryPlug
{
public:
	int64 val;
	uint8 lastState;
	DWORD m_dVTableAddr;
	DWORD m_txtTotal;
	DWORD m_imgCoin;
	DWORD m_imgKC;
	MerchantInvSlotList m_Slots;
	bool lastVisible;
public:
	CUITradeInventoryPlug();
	~CUITradeInventoryPlug();
	void ParseUIElements();
	void UpdateTotal(int32 value, bool iskc = true, bool add = false);
	DWORD GetSlotVTable(int id);
	void Tick();
	void OnOpen();
	void OnClose();
private:

};