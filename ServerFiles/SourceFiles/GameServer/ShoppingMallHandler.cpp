#include "stdafx.h"
#include "DBAgent.h"

void CUser::ShoppingMall(Packet & pkt)
{
	uint8 opcode = pkt.read<uint8>();

	switch (opcode)
	{
	case STORE_OPEN:
		HandleStoreOpen(pkt);
		break;

	case STORE_CLOSE:
		HandleStoreClose();
		break;

	case STORE_BUY:
	case STORE_MINI: // not sure what this is
	case STORE_PROCESS:
		/* fairly certain there's no need to emulate these as they're unused */
		break;

	case STORE_LETTER:
		LetterSystem(pkt);
		break;

	default:
		TRACE("Unknown shoppingmall packet: %X\n", opcode);
		printf("Unknown shoppingmall packet: %X\n", opcode);
	}
}

// We're opening the PUS...
void CUser::HandleStoreOpen(Packet & pkt)
{
	Packet result(WIZ_SHOPPING_MALL, uint8(STORE_OPEN));
	int16 sFreeSlot = 0;

	if (isDead()
		|| isTrading()
		|| isMerchanting()
		|| isStoreOpen())
	{
		result << uint16(-9);
		Send(&result);
		return;
	}

	// Not allowed in private arenas
	if (GetZoneID() >= 40 && GetZoneID() <= 45)
	{
		result << uint16(-9);
		Send(&result);
		return;
	}

	for (int i = SLOT_MAX; i < SLOT_MAX + HAVE_MAX; i++)
	{
		if (GetItem(i)->nNum == 0)
			sFreeSlot++;
	}

	if (sFreeSlot <= 0)
	{
		result << uint16(-8);
		Send(&result);
		return;
	}

	m_bStoreOpen = true;
	UserDataSaveToAgent();

	result << uint16(1) << sFreeSlot;
	Send(&result);
}

// We're closing the PUS so that we can call LOAD_WEB_ITEMMALL and load the extra items.
void CUser::HandleStoreClose()
{
	Packet result(WIZ_SHOPPING_MALL, uint8(STORE_CLOSE));
	m_bStoreOpen = false;
	g_pMain->AddDatabaseRequest(result, this);
}

void CUser::ReqLoadWebItemMall()
{
	Packet result(WIZ_SHOPPING_MALL, uint8(STORE_CLOSE));
	std::vector<_ITEM_DATA> itemList;

	/*
	WIZ_SHOPPING_MALL -> 
	6A
	02
	BCF9F339 1027 0100 00 0000 00000000 
	BCF9F339 1027 0100 00 0000 00000000 
	D03C3F36 0100 0100 00 0000 00000000 
	20D00D16 0100 8100 00 0000 00000000 
	08DE2F17 0100 4200 00 0000 00000000 
	C0E92F17 0100 0500 00 0000 00000000 
	90F12F17 0100 2C00 00 0000 00000000 
	D0564E17 0100 7D22 00 0000 00000000 
	50FF4D30 0100 0500 00 0000 00000000 
	C010AE35 0100 0100 00 0000 00000000 
	A814AE35 0100 0100 00 0000 00000000
	781CAE35 0100 0100 00 0000 00000000
	4824AE35 0100 1300 00 0000 00000000 
	00000000 0000 0000 00 0000 00000000 
	001EB135 0100 0A00 00 0000 00000000 
	B0E57837 0100 0200 00 0000 00000000
	10893617 0100 0900 00 0000 00000000
	E0133617 0100 0200 00 0000 00000000
	50253717 0100 0500 00 0000 00000000
	48364C30 0100 0600 00 0000 00000000
	B0529B16 0100 0100 00 0000 00000000
	00443B17 0100 0200 00 0000 00000000
	40FE3617 0100 0500 00 0000 00000000
	30D73617 0100 0400 00 0000 00000000
	08493317 0100 0100 00 0000 00000000
	F03A3617 0100 0100 00 0000 00000000
	400D491E 0100 0200 00 0000 00000000 
	00424C30 0100 1B00 00 0000 00000000
	*/

	if (!g_DBAgent.LoadWebItemMall(itemList, this))
		return;

	// reuse the GiveItem() method for giving them the item, just don't send the packet
	// as it's handled by STORE_CLOSE.
	foreach(itr, itemList)
	{
		GiveItem(itr->nNum, itr->sCount, false, itr->nExpirationTime);
	}
		
	for (int i = SLOT_MAX; i < SLOT_MAX + HAVE_MAX; i++)
	{
		_ITEM_DATA * pItem = GetItem(i);
		if (pItem == nullptr)
			continue;

		result << pItem->nNum
			<< pItem->sDuration
			<< pItem->sCount
			<< pItem->bFlag // item type flag (e.g. rented)
			<< pItem->sRemainingRentalTime // remaining time
			<< pItem->nExpirationTime; // expiration date in unix time
	}
	Send(&result);

	result.clear();
	result.Initialize(WIZ_ITEM_MOVE);
	result << uint8(STORE_CLOSE) << uint8(STORE_OPEN);
	for (int i = SLOT_MAX; i < SLOT_MAX + HAVE_MAX; i++)
	{
		_ITEM_DATA *pItem = GetItem(i);
		if (pItem == nullptr)
			continue;

		result	<< pItem->nNum 
			<< pItem->sDuration 
			<< pItem->sCount 
			<< pItem->bFlag
			<< pItem->sRemainingRentalTime;	// remaining time

		_ITEM_TABLE* pItemTable = g_pMain->GetItemPtr(pItem->nNum);
		if (pItemTable != nullptr)
		{
			if (pItemTable->isPetItem())
				ShowPetItemInfo(result, pItem->nSerialNum);
			else if (pItemTable->GetNum() == ITEM_CYPHER_RING)
				ShowCyperRingItemInfo(result, pItem->nSerialNum);
			else
				result << uint32(0); // Item Unique ID
		}
		else
			result << uint32(0); // Item Unique ID*/

		/*if (pItem->nNum == ITEM_CYPHER_RING)
			ShowCyperRingItemInfo(result, pItem->nSerialNum);
		else
			result << uint32(0); // Item Unique ID*/

		result << pItem->nExpirationTime; // expiration date in unix time
	}
	Send(&result);
}