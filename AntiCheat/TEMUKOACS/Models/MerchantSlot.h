#pragma once
class CMerchantSlot;
#include "../stdafx.h"
#include "MerchantInvSlot.h"

class CMerchantSlot
{
public:
	uint8 m_iPos;
	uint32 m_iPrice;
	uint16 m_iCount;
	bool m_bIsKC;

public:
	CMerchantSlot(uint8 pos, uint32 price, bool isKC, uint16 count = 1);
	~CMerchantSlot();

private:

};

typedef std::list<CMerchantSlot*>				MerchantSlotList;
typedef MerchantSlotList::iterator				MerchantSlotListItor;
typedef MerchantSlotList::const_iterator		MerchantSlotListItorConst;
typedef MerchantSlotList::reverse_iterator		MerchantSlotListReverseItor;