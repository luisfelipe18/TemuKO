#include "../stdafx.h"
#include "MerchantSlot.h"

CMerchantSlot::CMerchantSlot(uint8 pos, uint32 price, bool isKC, uint16 count)
{
	m_iPos = pos;
	m_iPrice = price;
	m_bIsKC = isKC;
	m_iCount = count;
}

CMerchantSlot::~CMerchantSlot()
{
}