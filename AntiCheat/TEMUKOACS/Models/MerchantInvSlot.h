#pragma once
#include "../stdafx.h"

class CMerchantInvSlot
{
public:
	uint8 m_iID;
	DWORD m_dVTableAddr;

public:
	CMerchantInvSlot(uint8 id, DWORD vTable);
	~CMerchantInvSlot();

private:

};

typedef std::list<CMerchantInvSlot*>				MerchantInvSlotList;
typedef MerchantInvSlotList::iterator				MerchantInvSlotListItor;
typedef MerchantInvSlotList::const_iterator			MerchantInvSlotListItorConst;
typedef MerchantInvSlotList::reverse_iterator		MerchantInvSlotListReverseItor;