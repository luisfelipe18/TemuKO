#include "../stdafx.h"
#include "MerchantInvSlot.h"

CMerchantInvSlot::CMerchantInvSlot(uint8 id, DWORD vTable)
{
	m_iID = id;
	m_dVTableAddr = vTable;
}

CMerchantInvSlot::~CMerchantInvSlot()
{
}