#pragma once
class CMerchantManager;
#include "Models/MerchantSlot.h"
#include "TEMUKOACS Engine.h"

class CMerchantManager
{
public:
	CMerchantSlot* m_RecentReq;
	MerchantSlotList m_MerchantSlots;
	MerchantOpcodes status;

public:
	CMerchantManager();
	~CMerchantManager();
	bool NeedToCheckTooltip();
	bool NeedToCheckDisplayTooltip();
	bool UpdateTooltipString(std::string& str);
	void SetRecentItemAddReq(Packet& pkt);
	void UpdateRecentItemAddReq(Packet& pkt);
	void RemoveItemFromSlot(Packet& pkt);
	void MerchantCreated(Packet& pkt);
	void UpdateItemDisplaySlots(Packet& pkt);
	void SetTheirMerchantSlots(Packet& pkt);
	void ResetMerchant();
	bool IsMerchanting();
	bool UpdateSlot(CMerchantSlot* item);

private:

};