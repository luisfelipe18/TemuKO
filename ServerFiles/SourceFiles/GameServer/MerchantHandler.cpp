#include "stdafx.h"
#include "../shared/DateTime.h"
using std::string;

enum MerchantOpenResponseCodes
{
	MERCHANT_OPEN_SUCCESS = 1,
	MERCHANT_OPEN_NO_SESSION = -1,
	MERCHANT_OPEN_DEAD = -2,
	MERCHANT_OPEN_TRADING = -3,
	MERCHANT_OPEN_MERCHANTING = -4,
	MERCHANT_OPEN_INVALID_ZONE = -5,
	MERCHANT_OPEN_SHOPPING = -6,
	MERCHANT_OPEN_UNDERLEVELED = 30
};

#define ITEM_MENICIAS_LIST 810166000

void CUser::MerchantProcess(Packet & pkt)
{
	if (!isInGame())
		return;

	uint8 opcode = pkt.read<uint8>();
	switch (opcode)
	{
		// Regular merchants
	case MERCHANT_OPEN: 
		MerchantOpen(); 
		break;

	case MERCHANT_CLOSE: 
		MerchantClose(); 
		break;

	case MERCHANT_ITEM_ADD: 
		MerchantItemAdd(pkt); 
		break;

	case MERCHANT_ITEM_CANCEL: 
		MerchantItemCancel(pkt); 
		break;

	case MERCHANT_ITEM_LIST:  // 6805F9FF
		MerchantItemList(pkt); 
		break;

	case MERCHANT_ITEM_BUY: 
		MerchantItemBuy(pkt); 
		break;

	case MERCHANT_INSERT: 
		MerchantInsert(pkt); 
		break;

	case MERCHANT_TRADE_CANCEL: 
		CancelMerchant(); 
		break;

		// Buying merchants
	case MERCHANT_BUY_OPEN: 
		BuyingMerchantOpen(pkt); 
		break;

	case MERCHANT_BUY_CLOSE: 
		BuyingMerchantClose(); 
		break;

	case MERCHANT_BUY_LIST: 
		BuyingMerchantList(pkt); 
		break;

	case MERCHANT_BUY_INSERT: 
		BuyingMerchantInsert(pkt); 
		break;

	case MERCHANT_BUY_BUY: // seeya!
		BuyingMerchantBuy(pkt); 
		break;
	
	case MERCHANT_MENISIA_LIST:
		MerchantOfficialList(pkt);
		break;
	default:
		printf("Merchant unhandled packets %d \n", opcode);
		TRACE("Merchant unhandled packets %d \n", opcode);
		break;
	}
}

void CUser::MerchantOpen()
{
	if(isBuyingMerchantingPreparing())
		return;

	int16 errorCode = 0;
	if (isDead())
		errorCode = MERCHANT_OPEN_DEAD;
	else if (isStoreOpen())
		errorCode = MERCHANT_OPEN_SHOPPING;
	else if (isTrading())
		errorCode = MERCHANT_OPEN_TRADING;
	/*else if (GetZoneID() > ZONE_MORADON)
		errorCode = MERCHANT_OPEN_INVALID_ZONE;*/
	else if (GetLevel() < 30)
		errorCode = MERCHANT_OPEN_UNDERLEVELED;
	else if (isMerchanting())
		errorCode = MERCHANT_OPEN_MERCHANTING;
	else 
		errorCode = MERCHANT_OPEN_SUCCESS;

	if (isSlaveMerchant())
	{
		errorCode = MERCHANT_OPEN_MERCHANTING;

		Packet result;
		std::string m_sAutosystem;

		m_sAutosystem = string_format("[Slave Merchant] Activated.");
		result.Initialize(WIZ_HOOK_GUARD);
		result << uint8(WIZ_HOOK_INFOMESSAGE);
		result << m_sAutosystem;
		Send(&result);
	}

	Packet result(WIZ_MERCHANT, uint8(MERCHANT_OPEN));
	result << errorCode;
	Send(&result);

	// If we're already merchanting, user may be desynced
	// so we need to close our current merchant first.
	if (errorCode == MERCHANT_OPEN_MERCHANTING)
	{
		if (GetItem(SHOULDER)->nNum == SLAVE_MERCHANT)
			MerchantSlaveClose();
		else
			MerchantClose();
	}

	if (errorCode == MERCHANT_OPEN_SUCCESS)
		m_bSellingMerchantPreparing = true;

	DateTime time;
	g_pMain->WriteMerchantLogFile(string_format("MERCHANT SELLING-OPEN - %d:%d:%d || Seller=%s,Zone=%d,X=%d,Z=%d\n",time.GetHour(),time.GetMinute(),time.GetSecond(),GetName().c_str(),GetZoneID(),uint16(GetX()),uint16(GetZ())));
}

void CUser::MerchantSlaveClose()
{
	if (GetItem(SHOULDER)->nNum == SLAVE_MERCHANT)
	{
		m_bMerchantOpen = false;
		m_bMerchantViewer = -1;
		m_bSellingMerchantPreparing = false;
		m_bMerchantState = MERCHANT_STATE_NONE;
		Packet result(WIZ_MERCHANT, uint8(MERCHANT_CLOSE));
		result << GetSocketID();
		
		if (GetEventRoom() > 0)
			SendToRegion(&result, nullptr, GetEventRoom());
		else
			SendToRegion(&result);
	}
	memset(&m_arMerchantItems, 0, sizeof(m_arMerchantItems));
}

void CUser::MerchantClose()
{
	if(!isSellingMerchantingPreparing() 
		|| isBuyingMerchantingPreparing())
		return;

	DateTime time;
	GiveMerchantItems(); // Give back to the user that which hasn't been sold, if any.

	if (isOfflineSystem()) 
	{
		if (GetNation() == KARUS)
			StateChangeServerDirect(11, uint32(0));
		else if (GetNation() == ELMORAD)
			StateChangeServerDirect(11, uint32(0));

		StateChangeServerDirect(7, 0);
		m_bOfflineSystemSocketType = false;
		m_bOfflineSystemType = false;
	}

	m_bMerchantOpen = false;
	m_bMerchantViewer = -1;
	m_bSellingMerchantPreparing = false;
	m_bMerchantState = MERCHANT_STATE_NONE;
	Packet result(WIZ_MERCHANT, uint8(MERCHANT_CLOSE));
	result << GetSocketID();
	SendToRegion(&result,nullptr,GetEventRoom());
	g_pMain->WriteMerchantLogFile(string_format("MERCHANT SELLING-CLOSE - %d:%d:%d || Seller=%s,Zone=%d,X=%d,Z=%d\n",time.GetHour(),time.GetMinute(),time.GetSecond(),GetName().c_str(),GetZoneID(),uint16(GetX()),uint16(GetZ())));
}

void CUser::MerchantItemAdd(Packet & pkt)
{
	if(!isSellingMerchantingPreparing() || isBuyingMerchantingPreparing())
		return;

	Packet result(WIZ_MERCHANT, uint8(MERCHANT_ITEM_ADD));
	uint32 nGold, nItemID;
	uint16 sCount;
	uint8 bSrcPos, // It sends the "actual" inventory slot (SLOT_MAX -> INVENTORY_MAX-SLOT_MAX), so need to allow for it. 
		bDstPos, 
		bMode; // Might be a flag for normal / "premium" merchant mode, once skills are implemented take another look at this.

	pkt >> nItemID >> sCount >> nGold >> bSrcPos >> bDstPos >> bMode;

	// TODO: Implement the possible error codes for these various error cases.
	if (bSrcPos >= HAVE_MAX
		|| bDstPos >= MAX_MERCH_ITEMS)
		return;

	_ITEM_TABLE * pTable = g_pMain->GetItemPtr(nItemID);
	if (pTable == nullptr
		|| nItemID >= ITEM_NO_TRADE_MIN && nItemID <= ITEM_NO_TRADE_MAX // Cannot be traded, sold or stored.
		|| pTable->m_bRace == RACE_UNTRADEABLE // Cannot be traded or sold.
		|| (pTable->m_bKind == 255 && sCount != 1)) // Cannot be traded or sold.
		goto fail_return;

	bSrcPos += SLOT_MAX;
	_ITEM_DATA *pSrcItem = GetItem(bSrcPos);
	if (pSrcItem == nullptr 
		|| pSrcItem->nNum != nItemID
		|| pSrcItem->sCount == 0 
		|| sCount == 0
		|| pSrcItem->sCount < sCount
		|| pSrcItem->isRented()
		|| pSrcItem->isSealed()
		|| pSrcItem->isBound()
		|| pSrcItem->isDuplicate()
		|| pSrcItem->isExpirationTime())
		goto fail_return;

	_MERCH_DATA *pMerch = &m_arMerchantItems[bDstPos];

	if(pMerch == nullptr)
		return; 

	pMerch->nNum = nItemID;
	pMerch->nPrice = nGold;
	pMerch->sCount = sCount; // Selling Count
	if(pTable->m_bKind == 255)
		pMerch->bCount = sCount; // Original Count ( INVENTORY )
	else 
		pMerch->bCount = pSrcItem->sCount; // Original Count ( INVENTORY )

	pMerch->sDuration = pSrcItem->sDuration;
	pMerch->nSerialNum = pSrcItem->nSerialNum; // NOTE: Stackable items will have an issue with this.
	pMerch->bOriginalSlot = bSrcPos;
	pMerch->IsSoldOut = false;

	// Take the user's item.
	memset(pSrcItem, 0, sizeof(_ITEM_DATA));

	result	<< uint16(1)
		<< nItemID << sCount << pMerch->sDuration << nGold 
		<< bSrcPos << bDstPos;
	Send(&result);

fail_return:
	result  << uint16(0)
		<< nItemID << sCount << (uint16) bSrcPos + bDstPos << nGold 
		<< bSrcPos << bDstPos;
	Send(&result);
}

void CUser::MerchantItemCancel(Packet & pkt)
{
	if(!isSellingMerchantingPreparing() 
		|| isBuyingMerchantingPreparing())
		return;

	Packet result(WIZ_MERCHANT, uint8(MERCHANT_ITEM_CANCEL));
	_MERCH_DATA *pMerch = nullptr;
	_ITEM_DATA *pItem = nullptr;

	uint8 bSrcPos = pkt.read<uint8>();

	// Invalid source position
	if (bSrcPos >= MAX_MERCH_ITEMS)
		result << int16(-2);
	// There's no item in that list..?
	else if ((pMerch = &m_arMerchantItems[bSrcPos])->nNum == 0)
		result << int16(-3);
	// Check to make sure we've got a valid stack
	else if (pMerch->sCount + (pItem = &m_sItemArray[pMerch->bOriginalSlot])->sCount > ITEMCOUNT_MAX) 
		result << int16(-3); // custom error
	else
	{
		pItem->nNum = pMerch->nNum;
		pItem->sCount = pMerch->sCount;
		pItem->sDuration = pMerch->sDuration;
		pItem->nSerialNum = pMerch->nSerialNum; // NOTE: Stackable items will have an issue with this.
		memset(pMerch, 0, sizeof(_MERCH_DATA));
		result << int16(1) << bSrcPos;
	}

	Send(&result);
}

void CUser::MerchantInsert(Packet & pkt)
{
	if(!isSellingMerchantingPreparing() 
		|| isBuyingMerchantingPreparing())
		return;

	DateTime time;
	string advertMessage; // check here maybe to make sure they're not using it otherwise?
	pkt >> advertMessage;
	if (advertMessage.size() > MAX_MERCH_MESSAGE)
		return;

#pragma region SlaveMerchant
	if (GetItem(SHOULDER)->nNum == SLAVE_MERCHANT)
	{
		CBot* pSlaveUser = nullptr;
		uint16 m_bSlaveUsers = g_pMain->SpawnSlaveUserBot(2, this);
		if (m_bSlaveUsers > 0)
		{
			pSlaveUser = g_pMain->m_BotArray.GetData(m_bSlaveUsers);
			if (pSlaveUser == nullptr)
				return;

			pSlaveUser->m_bPremiumMerchant = true;
			Packet result(WIZ_MERCHANT, uint8(MERCHANT_INSERT));
			result << uint16(1) 
				<< advertMessage 
				<< pSlaveUser->GetID()
				<< pSlaveUser->m_bPremiumMerchant;

			std::string asdasd = advertMessage;

			if (!asdasd.empty())
				pSlaveUser->MerchantChat = string_format("%s(Location:%d,%d)", asdasd.c_str(), pSlaveUser->GetSPosX() / 10, pSlaveUser->GetSPosZ() / 10);
			else
				pSlaveUser->MerchantChat.clear();

			uint16 totalMerchItems = 0;
			for (int i = 0; i < MAX_MERCH_ITEMS; i++)
			{
				pSlaveUser->m_arMerchantItems[i] = m_arMerchantItems[i];

				if (pSlaveUser->m_arMerchantItems[i].nNum != 0 &&
					(pSlaveUser->m_arMerchantItems[i].bCount == 0
						|| pSlaveUser->m_arMerchantItems[i].bCount < m_arMerchantItems[i].sCount))
					return;

				result << pSlaveUser->m_arMerchantItems[i].nNum;

				if (pSlaveUser->m_arMerchantItems[i].nNum > 0)
				{
					totalMerchItems++;
					g_pMain->WriteMerchantLogFile(string_format("MERCHANT SLAVE SELLING-ITEMS - %d:%d:%d || Seller=%s,ItemID=%d,Count=%d/%d,Price=%d,Zone=%d,X=%d,Z=%d\n", time.GetHour(), time.GetMinute(), time.GetSecond(), pSlaveUser->GetName().c_str(), pSlaveUser->m_arMerchantItems[i].nNum, pSlaveUser->m_arMerchantItems[i].bCount, pSlaveUser->m_arMerchantItems[i].sCount, pSlaveUser->m_arMerchantItems[i].nPrice, pSlaveUser->GetZoneID(), uint16(pSlaveUser->GetX()), uint16(pSlaveUser->GetZ())));
				}
			}

			if (totalMerchItems == 0)
			{
				if (pSlaveUser->isSellingMerchant())
				{
					if (isSlaveMerchant()) {
						m_bSlaveMerchant = false;
						m_bSlaveUserID = -1;
					}
					Packet result(WIZ_MERCHANT, uint8(MERCHANT_CLOSE));
					result << pSlaveUser->GetID();
					pSlaveUser->SendToRegion(&result);
					g_pMain->WriteCheatLogFile(string_format("Merchant Slave hack - %d:%d:%d || %s is disconnected.\n", time.GetHour(), time.GetMinute(), time.GetSecond(), pSlaveUser->GetName().c_str()));
					pSlaveUser->UserInOut(INOUT_OUT);
					return;
				}

				Packet result(WIZ_MERCHANT, uint8(MERCHANT_CLOSE));
				result << pSlaveUser->GetID();
				pSlaveUser->SendToRegion(&result);

				g_pMain->WriteCheatLogFile(string_format("Merchant Slave hack - %d:%d:%d || %s is disconnected.\n", time.GetHour(), time.GetMinute(), time.GetSecond(), pSlaveUser->GetName().c_str()));
				return;
			}
			pSlaveUser->m_bMerchantState = MERCHANT_STATE_SELLING;
			pSlaveUser->SendToRegion(&result);
		}
		MerchantSlaveClose();
		InventorySystemReflesh((Packet)(WIZ_ITEM_MOVE, uint8(2)));
		return;
	}
#pragma endregion

#pragma region Bot Merchant
	if (isGM() && OpenGmBotSystem)
	{
		CBot* pTUser = nullptr;
		uint16 sCount = myrand(1, 10);
		for (int i = 0; i < 1; i++)
		{
			float Bonc = myrand(1, 15) * 1.0f;
			float Bonc2 = myrand(1, 15) * 1.0f;

			uint16 mSBotID = g_pMain->SpawnUserBot(3600, GetZoneID(), GetX() + Bonc, GetY(), GetZ() + Bonc2, 5, GetLevel());
			if (mSBotID)
			{
				pTUser = g_pMain->m_BotArray.GetData(mSBotID);
				if (pTUser == nullptr)
					return;

				pTUser->m_bPremiumMerchant = m_bPremiumMerchant;
				Packet result(WIZ_MERCHANT, uint8(MERCHANT_INSERT));
				result << uint16(1) << advertMessage << pTUser->GetID()
					<< m_bPremiumMerchant;

				std::string asdasd = advertMessage;

				if (!asdasd.empty())
					pTUser->MerchantChat = string_format("%s(Location:%d,%d)", asdasd.c_str(), pTUser->GetSPosX() / 10, pTUser->GetSPosZ() / 10);
				else
					pTUser->MerchantChat.clear();

				uint16 totalMerchItems = 0;
				for (int i = 0; i < MAX_MERCH_ITEMS; i++)
				{
					pTUser->m_arMerchantItems[i] = m_arMerchantItems[i];

					if (pTUser->m_arMerchantItems[i].nNum != 0 &&
						(pTUser->m_arMerchantItems[i].bCount == 0 
							|| pTUser->m_arMerchantItems[i].bCount < m_arMerchantItems[i].sCount))
						return;

					result << m_arMerchantItems[i].nNum;

					if (pTUser->m_arMerchantItems[i].nNum > 0)
					{
						totalMerchItems++;
						g_pMain->WriteMerchantLogFile(string_format("MERCHANT SELLING-ITEMS - %d:%d:%d || Seller=%s,ItemID=%d,Count=%d/%d,Price=%d,Zone=%d,X=%d,Z=%d\n", time.GetHour(), time.GetMinute(), time.GetSecond(), pTUser->GetName().c_str(), pTUser->m_arMerchantItems[i].nNum, pTUser->m_arMerchantItems[i].bCount, pTUser->m_arMerchantItems[i].sCount, pTUser->m_arMerchantItems[i].nPrice, pTUser->GetZoneID(), uint16(pTUser->GetX()), uint16(pTUser->GetZ())));
					}
				}

				if (totalMerchItems == 0)
				{
					Packet result(WIZ_MERCHANT, uint8(MERCHANT_CLOSE));
					result << pTUser->GetID();
					pTUser->SendToRegion(&result);

					g_pMain->WriteCheatLogFile(string_format("Merchant hack - %d:%d:%d || %s is disconnected.\n", time.GetHour(), time.GetMinute(), time.GetSecond(), pTUser->GetName().c_str()));
					return;
				}
				pTUser->m_bMerchantState = MERCHANT_STATE_SELLING;
				pTUser->SendToRegion(&result);
				printf("[Merchant Bot Manager] = %s Bot Created | Zone : %d | X : %d | Z : %d\n", pTUser->GetName().c_str(), GetZoneID(), uint16(GetX()), uint16(GetZ()));
			}
		}
		MerchantClose();
		return;
	}
#pragma endregion

#pragma region Merchant
	Packet result(WIZ_MERCHANT, uint8(MERCHANT_INSERT));
	result << uint16(1) 
		<< advertMessage 
		<< GetSocketID()
		<< m_bPremiumMerchant; 

	uint16 totalMerchItems = 0;
	for (int i = 0; i < MAX_MERCH_ITEMS; i++)
	{
		if(m_arMerchantItems[i].nNum != 0 && 
			(m_arMerchantItems[i].bCount == 0 || m_arMerchantItems[i].bCount < m_arMerchantItems[i].sCount))
			return;

		result << m_arMerchantItems[i].nNum;

		if(m_arMerchantItems[i].nNum > 0)
		{
			totalMerchItems++;
			g_pMain->WriteMerchantLogFile(string_format("MERCHANT SELLING-ITEMS - %d:%d:%d || Seller=%s,ItemID=%d,Count=%d/%d,Price=%d,Zone=%d,X=%d,Z=%d\n",time.GetHour(),time.GetMinute(),time.GetSecond(),GetName().c_str(),m_arMerchantItems[i].nNum,m_arMerchantItems[i].bCount, m_arMerchantItems[i].sCount, m_arMerchantItems[i].nPrice,GetZoneID(),uint16(GetX()),uint16(GetZ())));
		}
	}

	if(totalMerchItems == 0)
	{
		Disconnect();
		g_pMain->WriteCheatLogFile(string_format("Merchant hack - %d:%d:%d || %s is disconnected.\n", time.GetHour(),time.GetMinute(),time.GetSecond(),GetName().c_str()));
		return;
	}

	_ITEM_TABLE* pItem = GetItemPrototype(CFAIRY);
	if (pItem != nullptr)
	{
		if (pItem->GetNum() == OFFLINE_MERCHANT_ITEM)
		{
			if (GetNation() == KARUS)
				StateChangeServerDirect(11, uint32(1));
			else if (GetNation() == ELMORAD)
				StateChangeServerDirect(11, uint32(2));

			StateChangeServerDirect(7, 2);
			m_bOfflineSystemSocketType = true;
			m_bOfflineSystemType = true;
			g_pMain->WriteOfflineSystemLog(string_format("[Merchant Create - %d:%d:%d ] = %s\n", time.GetHour(), time.GetMinute(), time.GetSecond(), GetName().c_str()));
		}
	}

	m_bMerchantState = MERCHANT_STATE_SELLING;
	SendToRegion(&result,nullptr,GetEventRoom());
#pragma endregion
}

void CUser::MerchantItemList(Packet & pkt)
{
	uint16 uid = pkt.read<uint16>();

	CBot* pUserBot = g_pMain->m_BotArray.GetData(uid);
	if (pUserBot != nullptr)
	{
		if (!pUserBot->isMerchanting())
			return;

		if (pUserBot->m_bMerchantViewer >= 0)
		{
			if (pUserBot->m_bMerchantOpen == true)
			{
				CUser* pUser = g_pMain->GetUserPtr(pUserBot->m_bMerchantViewer);
				if (pUser != nullptr)
				{
					if (!pUser->isInGame())
					{
						pUserBot->m_bMerchantOpen = false;
						pUserBot->m_bMerchantViewer = -1;
						m_sMerchantsSocketID = -1;
					}
					else
					{
						Packet result(WIZ_MERCHANT, uint8(MERCHANT_ITEM_LIST));
						result.SByte();
						result << uint16(-7) << pUser->GetName();
						Send(&result);
						return;
					}
				}
				else
				{
					pUserBot->m_bMerchantOpen = false;
					pUserBot->m_bMerchantViewer = -1;
					m_sMerchantsSocketID = -1;
				}
			}
		}

		pUserBot->m_bMerchantOpen = true;
		pUserBot->m_bMerchantViewer = GetID();
		m_sMerchantsSocketID = uid;

		Packet result(WIZ_MERCHANT, uint8(MERCHANT_ITEM_LIST));
		result << uint16(1) << uint16(uid);
		for (int i = 0; i < MAX_MERCH_ITEMS; i++)
		{
			_MERCH_DATA* pMerch = &pUserBot->m_arMerchantItems[i];
			result << pMerch->nNum
				<< pMerch->sCount
				<< pMerch->sDuration
				<< pMerch->nPrice;

			if (pMerch->nNum == ITEM_CYPHER_RING)
				ShowCyperRingItemInfo(result, pMerch->nSerialNum);
			else
				result << uint32(0); // Item Unique ID
		}

		for (int i = 0; i < MAX_MERCH_ITEMS; i++)
		{
			_MERCH_DATA* pMerch = &pUserBot->m_arMerchantItems[i];
			uint8 isKC = pMerch->isKC ? 1 : 0;
			result << isKC;
		}

		Send(&result);
		return;
	}

	CUser* pUserMerchant = g_pMain->GetUserPtr(uid);
	if (pUserMerchant == nullptr
		|| !isInRange(pUserMerchant, MAX_NPC_RANGE)
		|| !pUserMerchant->isMerchanting()
		|| !pUserMerchant->isSellingMerchantingPreparing()
		|| pUserMerchant->isBuyingMerchantingPreparing())
		return;

	if (pUserMerchant->m_bMerchantViewer >= 0)
	{
		if (pUserMerchant->m_bMerchantOpen == true)
		{
			CUser* pUser = g_pMain->GetUserPtr(pUserMerchant->m_bMerchantViewer);
			if (pUser != nullptr)
			{
				if (!pUser->isInGame())
				{
					pUserMerchant->m_bMerchantOpen = false;
					pUserMerchant->m_bMerchantViewer = -1;
					m_sMerchantsSocketID = -1;
				}
				else
				{
					Packet result(WIZ_MERCHANT, uint8(MERCHANT_ITEM_LIST));
					result.SByte();
					result << uint16(-7) << pUser->GetName();
					Send(&result);
					return;
				}
			}
			else
			{
				pUserMerchant->m_bMerchantOpen = false;
				pUserMerchant->m_bMerchantViewer = -1;
				m_sMerchantsSocketID = -1;
			}
		}
	}

	pUserMerchant->m_bMerchantOpen = true;
	pUserMerchant->m_bMerchantViewer = GetID();
	m_sMerchantsSocketID = uid;

	Packet result(WIZ_MERCHANT, uint8(MERCHANT_ITEM_LIST));
	result << uint16(1) << uint16(uid);
	for (int i = 0; i < MAX_MERCH_ITEMS; i++)
	{
		_MERCH_DATA* pMerch = &pUserMerchant->m_arMerchantItems[i];
		result << pMerch->nNum
			<< pMerch->sCount
			<< pMerch->sDuration
			<< pMerch->nPrice;

		_ITEM_TABLE* pItemTable = g_pMain->GetItemPtr(pMerch->nNum);
		if (pItemTable != nullptr)
		{
			if (pItemTable->isPetItem())
				ShowPetItemInfo(result, pMerch->nSerialNum);
			else if (pItemTable->GetNum() == ITEM_CYPHER_RING)
				ShowCyperRingItemInfo(result, pMerch->nSerialNum);
			else
				result << uint32(0); // Item Unique ID
		}
		else
			result << uint32(0); // Item Unique ID*/
	}

	for (int i = 0; i < MAX_MERCH_ITEMS; i++)
	{
		_MERCH_DATA* pMerch = &pUserMerchant->m_arMerchantItems[i];
		uint8 isKC = pMerch->isKC ? 1 : 0;
		result << isKC;
	}

	Send(&result);
}

void CUser::MerchantItemBuy(Packet& pkt)
{
	uint32 itemid, req_gold;
	uint16 item_count, leftover_count;
	uint8 item_slot, dest_slot;
	Packet result(WIZ_MERCHANT);

	CBot* pUserBot = g_pMain->m_BotArray.GetData(m_sMerchantsSocketID);
	if (pUserBot != nullptr)
	{
		if (pUserBot->GetMerchantState() != MERCHANT_STATE_SELLING)
		{
			result << uint8(6) << uint16(-18);
			Send(&result);
			return;
		}

		pkt >> itemid >> item_count >> item_slot >> dest_slot;

		// Make sure the slots are correct and that we're actually buying at least 1 item.
		if (item_slot >= MAX_MERCH_ITEMS
			|| dest_slot >= HAVE_MAX
			|| item_count == 0)
		{
			result << uint8(6) << uint16(-18);
			Send(&result);
			return;
		}

		// Grab pointers to the items.
		_MERCH_DATA* pMerch = &pUserBot->m_arMerchantItems[item_slot];
		_ITEM_DATA* pItem = GetItem(SLOT_MAX + dest_slot);
		if (pItem == nullptr)
		{
			result << uint8(6) << uint16(-18);
			Send(&result);
			return;
		}

		// Make sure the merchant actually has that item in that slot
		// and that they have enough
		if (pMerch->nNum != itemid
			|| pMerch->sCount < item_count
			|| pMerch->bCount < item_count)
		{
			result << uint8(6) << uint16(-18);
			Send(&result);
			return;
		}

		// If it's not stackable, and we're specifying something other than 1
		// we really don't care to handle this request...
		_ITEM_TABLE* proto = g_pMain->GetItemPtr(itemid);
		if (proto == nullptr
			|| !proto->m_bCountable && item_count != 1)
		{
			result << uint8(6) << uint16(-18);
			Send(&result);
			return;
		}

		// Do we have enough coins?
		req_gold = pMerch->nPrice * item_count;
		if (pMerch->isKC)
		{
			if (m_nKnightCash < req_gold)
			{
				result << uint8(6) << uint16(-18);
				Send(&result);
				return;
			}
		}
		else
		{
			if (m_iGold < req_gold)
			{
				result << uint8(6) << uint16(-18);
				Send(&result);
				return;
			}
		}
		// If the slot's not empty
		if (pItem->nNum != 0
			// and we already have an item that isn't the same item
			// or it's the same item but the item's not stackable...
			&& (pItem->nNum != itemid || !proto->m_bCountable))
		{
			result << uint8(6) << uint16(-18);
			Send(&result);
			return;
		}

		if (pMerch->isKC)
		{
			if (!HOOK_CashLose(req_gold))
			{
				result << uint8(6) << uint16(-18);
				Send(&result);
				return;
			}

			if (pUserBot->isSlaveMerchant())
			{
				CUser* pSlaveUser = g_pMain->GetUserPtr(pUserBot->m_bSlaveUserID);
				if (pSlaveUser != nullptr)
					pSlaveUser->HOOK_CashGain(req_gold);
			}
		}
		else
		{
			if (!GoldLose(req_gold))
			{
				result << uint8(6) << uint16(-18);
				Send(&result);
				return;
			}

			if (pUserBot->isSlaveMerchant())
			{
				CUser* pSlaveUser = g_pMain->GetUserPtr(pUserBot->m_bSlaveUserID);
				if (pSlaveUser != nullptr)
				{
					if (pSlaveUser->m_iGold + req_gold > COIN_MAX)
					{
						result << uint8(6) << uint16(-18);
						Send(&result);
						return;
					}
					pSlaveUser->GoldGain(req_gold);
				}
			}
		}

		leftover_count = pMerch->sCount - item_count;

		pItem->nNum = pMerch->nNum;
		pItem->sCount += item_count;
		pItem->sDuration = pMerch->sDuration;
		pItem->nSerialNum = g_pMain->GenerateItemSerial();

		pMerch->sCount -= item_count;
		pMerch->bCount -= item_count;

		SendStackChange(itemid, pItem->sCount, pItem->sDuration, dest_slot,
			(pItem->sCount == item_count)); // is it a new item?

		if (pMerch->sCount == 0 && pMerch->bCount == 0)
			memset(pMerch, 0, sizeof(_MERCH_DATA));
		else if (pMerch->sCount == 0 && pMerch->bCount != 0) // Countable item protect.
			pMerch->IsSoldOut = true;

		result.clear();
		result.Initialize(WIZ_MERCHANT);
		result << uint8(MERCHANT_ITEM_PURCHASED) 
			<< itemid 
			<< GetName();

		if (pUserBot->isSlaveMerchant())
		{
			CUser* pSlaveUser = g_pMain->GetUserPtr(pUserBot->GetSlaveGetID());
			if (pSlaveUser != nullptr)
			{
				if (pSlaveUser->isSlaveMerchant())
					pSlaveUser->Send(&result);
			}
		}

		result.clear();
		result.Initialize(WIZ_MERCHANT);
		result << uint8(MERCHANT_ITEM_BUY) 
			<< uint16(1)
			<< itemid
			<< leftover_count
			<< item_slot 
			<< dest_slot;
		Send(&result);

		if (item_slot < 4 && leftover_count == 0)
		{
			result.clear();
			result.Initialize(WIZ_MERCHANT_INOUT);
			result << uint8(2) << m_sMerchantsSocketID << uint8(1) << uint8(0) << item_slot;
			pUserBot->SendToRegion(&result);
		}

		int nItemsRemaining = 0;
		for (int i = 0; i < MAX_MERCH_ITEMS; i++){
			if (pUserBot->m_arMerchantItems[i].nNum != 0 && !pUserBot->m_arMerchantItems[i].IsSoldOut)
				nItemsRemaining++;
		}

		if (nItemsRemaining == 0)
		{
			if (pUserBot->isSlaveMerchant())
			{
				CUser* pSlaveUser = g_pMain->GetUserPtr(pUserBot->GetSlaveGetID());
				if (pSlaveUser != nullptr)
				{
					if (pSlaveUser->isSlaveMerchant())
					{
						pSlaveUser->m_bSlaveMerchant = false;
						pSlaveUser->m_bSlaveUserID = -1;
					}
				}
				result.clear();
				result.Initialize(WIZ_MERCHANT);
				result << uint8(MERCHANT_CLOSE) << pUserBot->GetID();
				pUserBot->SendToRegion(&result);

				pUserBot->UserInOut(INOUT_OUT);
				return;
			}

			pUserBot->LastWarpTime = UNIXTIME + 10;

			pUserBot->m_bMerchantOpen = false;
			pUserBot->m_bMerchantViewer = -1;
			pUserBot->m_bSellingMerchantPreparing = false;
			pUserBot->m_bMerchantState = MERCHANT_STATE_NONE;

			result.clear();
			result.Initialize(WIZ_MERCHANT);
			result << uint8(MERCHANT_CLOSE) << pUserBot->GetID();
			pUserBot->SendToRegion(&result);
			DateTime time;
			g_pMain->WriteMerchantLogFile(string_format("MERCHANT SELLING-CLOSE - %d:%d:%d || Seller=%s,Zone=%d,X=%d,Z=%d\n", time.GetHour(), time.GetMinute(), time.GetSecond(), pUserBot->GetName().c_str(), GetZoneID(), uint16(GetX()), uint16(GetZ())));
			return;
		}

		DateTime time;
		g_pMain->WriteMerchantLogFile(string_format("MERCHANT SELLING-BUY - %d:%d:%d || Seller=%s,Buyer=%s,ItemID=%d,Count=%d,Price=%d,Zone=%d,X=%d,Z=%d\n", time.GetHour(), time.GetMinute(), time.GetSecond(), pUserBot->GetName().c_str(), GetName().c_str(), itemid, item_count, req_gold, GetZoneID(), uint16(GetX()), uint16(GetZ())));
		return;
	}

	CUser* pMerchant = g_pMain->GetUserPtr(m_sMerchantsSocketID);
	if (pMerchant == nullptr
		|| pMerchant->GetMerchantState() != MERCHANT_STATE_SELLING
		|| !pMerchant->isSellingMerchantingPreparing()
		|| pMerchant->isBuyingMerchantingPreparing())
	{
		result << uint8(6) << uint16(-18);
		Send(&result);
		return;
	}

	pkt >> itemid >> item_count >> item_slot >> dest_slot;

	// Make sure the slots are correct and that we're actually buying at least 1 item.
	if (item_slot >= MAX_MERCH_ITEMS
		|| dest_slot >= HAVE_MAX
		|| item_count == 0)
	{
		result << uint8(6) << uint16(-18);
		Send(&result);
		return;
	}

	// Grab pointers to the items.
	_MERCH_DATA* pMerch = &pMerchant->m_arMerchantItems[item_slot];
	_ITEM_DATA* pItem = GetItem(SLOT_MAX + dest_slot);
	if (pItem == nullptr)
	{
		result << uint8(6) << uint16(-18);
		Send(&result);
		return;
	}

	// Make sure the merchant actually has that item in that slot
	// and that they have enough
	if (pMerch->nNum != itemid
		|| pMerch->sCount < item_count
		|| pMerch->bCount < item_count)
	{
		result << uint8(6) << uint16(-18);
		Send(&result);
		return;
	}

	// If it's not stackable, and we're specifying something other than 1
	// we really don't care to handle this request...
	_ITEM_TABLE* proto = g_pMain->GetItemPtr(itemid);
	if (proto == nullptr
		|| !proto->m_bCountable && item_count != 1)
	{
		result << uint8(6) << uint16(-18);
		Send(&result);
		return;
	}

	// Do we have enough coins?
	req_gold = pMerch->nPrice * item_count;
	if (pMerch->isKC)
	{
		if (m_nKnightCash < req_gold)
		{
			result << uint8(6) << uint16(-18);
			Send(&result);
			return;
		}
	}
	else
	{
		if (m_iGold < req_gold)
		{
			result << uint8(6) << uint16(-18);
			Send(&result);
			return;
		}
	}

	// If the slot's not empty
	if (pItem->nNum != 0
		// and we already have an item that isn't the same item
		// or it's the same item but the item's not stackable...
		&& (pItem->nNum != itemid || !proto->m_bCountable))
	{
		result << uint8(6) << uint16(-18);
		Send(&result);
		return;
	}

	if (pMerch->isKC)
	{
		if (!HOOK_CashLose(req_gold))
		{
			result << uint8(6) << uint16(-18);
			Send(&result);
			return;
		}

		pMerchant->HOOK_CashGain(req_gold);
	}
	else
	{
		if (pMerchant->m_iGold + req_gold > COIN_MAX)
		{
			result << uint8(6) << uint16(-18);
			Send(&result);
			return;
		}

		if (!GoldLose(req_gold))
		{
			result << uint8(6) << uint16(-18);
			Send(&result);
			return;
		}

		pMerchant->GoldGain(req_gold);
	}

	leftover_count = pMerch->sCount - item_count;

	pItem->nNum = pMerch->nNum;
	pItem->sCount += item_count;
	pItem->sDuration = pMerch->sDuration;
	pItem->nSerialNum = pMerch->nSerialNum;

	pMerch->sCount -= item_count;
	pMerch->bCount -= item_count;

	SendStackChange(itemid, pItem->sCount, pItem->sDuration, dest_slot,
		(pItem->sCount == item_count)); // is it a new item?

	pMerchant->SendStackChange(pMerch->nNum, pMerch->bCount, pMerch->sDuration,
		pMerch->bOriginalSlot - SLOT_MAX);

	if (pMerch->sCount == 0 && pMerch->bCount == 0)
		memset(pMerch, 0, sizeof(_MERCH_DATA));
	else if (pMerch->sCount == 0 && pMerch->bCount != 0) // Countable item protect.
		pMerch->IsSoldOut = true;

	result.clear();
	result.Initialize(WIZ_MERCHANT);
	result << uint8(MERCHANT_ITEM_PURCHASED) 
		<< itemid 
		<< GetName();

	pMerchant->Send(&result);

	result.clear();
	result.Initialize(WIZ_MERCHANT);
	result << uint8(MERCHANT_ITEM_BUY) 
		<< uint16(1)
		<< itemid 
		<< leftover_count
		<< item_slot 
		<< dest_slot;

	Send(&result);

	if (item_slot < 4 && leftover_count == 0)
	{
		result.clear();
		result.Initialize(WIZ_MERCHANT_INOUT);
		result << uint8(2)
			<< m_sMerchantsSocketID 
			<< uint8(1)
			<< uint8(0)
			<< item_slot;

		if (pMerchant->GetEventRoom() > 0)
			pMerchant->SendToRegion(&result, nullptr, GetEventRoom());
		else
			pMerchant->SendToRegion(&result);
	}

	int nItemsRemaining = 0;
	for (int i = 0; i < MAX_MERCH_ITEMS; i++)
	{
		if (pMerchant->m_arMerchantItems[i].nNum != 0 && !pMerchant->m_arMerchantItems[i].IsSoldOut)
			nItemsRemaining++;
	}

	if (nItemsRemaining == 0)
		pMerchant->MerchantClose();

	DateTime time;
	g_pMain->WriteMerchantLogFile(string_format("MERCHANT SELLING-BUY - %d:%d:%d || Seller=%s,Buyer=%s,ItemID=%d,Count=%d,Price=%d,Zone=%d,X=%d,Z=%d\n", time.GetHour(), time.GetMinute(), time.GetSecond(), pMerchant->GetName().c_str(), GetName().c_str(), itemid, item_count, req_gold, GetZoneID(), uint16(GetX()), uint16(GetZ())));
}

void CUser::GiveMerchantItems()
{
	if(m_bMerchantState == MERCHANT_STATE_SELLING 
		|| m_bSellingMerchantPreparing)
	{
		for (int i = 0; i < MAX_MERCH_ITEMS; i++)
		{
			_MERCH_DATA *pMerch = &m_arMerchantItems[i];
			if (pMerch->nNum == 0) continue;
			_ITEM_DATA *pItem = &m_sItemArray[pMerch->bOriginalSlot];

			pItem->nNum = pMerch->nNum;
			pItem->nSerialNum = pMerch->nSerialNum;
			pItem->sCount = pMerch->bCount;
			pItem->sDuration = pMerch->sDuration;

			// NOTE: Don't need to update the client, the client doesn't see any change.
		}
	}
	// remove the items from the array now that they've been restored to the user
	memset(&m_arMerchantItems, 0, sizeof(m_arMerchantItems));
}

void CUser::GiveSlaveMerchantItems()
{
	CBot* pSlaveUser = g_pMain->m_BotArray.GetData(m_bSlaveUserID);
	if (pSlaveUser != nullptr)
	{
		if (pSlaveUser->isMerchanting()) {
			for (int i = 0; i < MAX_MERCH_ITEMS; i++)
			{
				_MERCH_DATA* pMerch = &pSlaveUser->m_arMerchantItems[i];

				if (pMerch->nNum == 0)
					continue;

				int SlotForItem = FindSlotForItem(pMerch->nNum, pMerch->sCount);

				if (SlotForItem < 0)
					GiveWerehouseItem(pMerch->nNum, pMerch->sCount);
				else
					GiveItem(pMerch->nNum, pMerch->sCount);
			}
			Packet result;
			// remove the items from the array now that they've been restored to the user
			memset(&pSlaveUser->m_arMerchantItems, 0, sizeof(pSlaveUser->m_arMerchantItems));

			result.clear();
			result.Initialize(WIZ_MERCHANT);
			result << uint8(MERCHANT_CLOSE) << pSlaveUser->GetID();
			pSlaveUser->SendToRegion(&result);

			m_bSlaveMerchant = false;
			m_bSlaveUserID = -1;
		}
	}
}

void CUser::CancelMerchant()
{
	if (m_sMerchantsSocketID < 0)
		return;

	RemoveFromMerchantLookers();
	Packet result(WIZ_MERCHANT, uint8(MERCHANT_TRADE_CANCEL));
	result << uint16(1);
	Send(&result);
}

void CUser::BuyingMerchantOpen(Packet & pkt)
{
	if(isSellingMerchantingPreparing())
		return;

	int16 errorCode = 0;
	if (isDead())
		errorCode = MERCHANT_OPEN_DEAD;
	else if (isStoreOpen())
		errorCode = MERCHANT_OPEN_SHOPPING;
	else if (isTrading())
		errorCode = MERCHANT_OPEN_TRADING;
	/*else if (GetZoneID() > ZONE_MORADON || GetZoneID() <= ELMORAD)
		errorCode = MERCHANT_OPEN_INVALID_ZONE;*/
	else if (GetLevel() < 30)
		errorCode = MERCHANT_OPEN_UNDERLEVELED;
	else if (isMerchanting())
		errorCode = MERCHANT_OPEN_MERCHANTING;
	else 
		errorCode = MERCHANT_OPEN_SUCCESS;

	Packet result(WIZ_MERCHANT, uint8(MERCHANT_BUY_OPEN));
	result << errorCode;
	Send(&result);

	if (errorCode == MERCHANT_OPEN_MERCHANTING)
		BuyingMerchantClose();

	if (errorCode == MERCHANT_OPEN_SUCCESS)
		m_bBuyingMerchantPreparing = true;

	memset(&m_arMerchantItems, 0, sizeof(m_arMerchantItems));
	DateTime time;
	g_pMain->WriteMerchantLogFile(string_format("MERCHANT BUYING-OPEN - %d:%d:%d || Buyer=%s,Zone=%d,X=%d,Z=%d\n",time.GetHour(),time.GetMinute(),time.GetSecond(),GetName().c_str(),GetZoneID(),uint16(GetX()),uint16(GetZ())));
}

void CUser::BuyingMerchantClose()
{
	if (isMerchanting() && isBuyingMerchantingPreparing())
	{
		m_bMerchantViewer = -1;
		m_bBuyingMerchantPreparing = false;
		m_bMerchantOpen = false;
		m_bMerchantState = MERCHANT_STATE_NONE;
		memset(m_arMerchantItems, 0, sizeof(m_arMerchantItems));

		Packet result(WIZ_MERCHANT, uint8(MERCHANT_BUY_CLOSE));
		result << GetSocketID();
		
		if (GetEventRoom() > 0)
			SendToRegion(&result, nullptr, GetEventRoom());
		else
			SendToRegion(&result);

		DateTime time;
		g_pMain->WriteMerchantLogFile(string_format("MERCHANT BUYING-CLOSE - %d:%d:%d || Buyer=%s,Zone=%d,X=%d,Z=%d\n", time.GetHour(), time.GetMinute(), time.GetSecond(), GetName().c_str(), GetZoneID(), uint16(GetX()), uint16(GetZ())));
	}
	else if (m_sMerchantsSocketID >= 0)
	{
		RemoveFromMerchantLookers();
		Packet result2(WIZ_MERCHANT, uint8(MERCHANT_BUY_CLOSE));
		result2 << GetSocketID();
		Send(&result2);

		DateTime time;
		g_pMain->WriteMerchantLogFile(string_format("MERCHANT BUYING-CLOSE - %d:%d:%d || Buyer=%s,Zone=%d,X=%d,Z=%d\n", time.GetHour(), time.GetMinute(), time.GetSecond(), GetName().c_str(), GetZoneID(), uint16(GetX()), uint16(GetZ())));
	}
}

void CUser::BuyingMerchantInsert(Packet& pkt)
{
	if (isSellingMerchantingPreparing() || !isBuyingMerchantingPreparing())
		return;

	uint8 amount_of_items;
	uint32 itemid, buying_price;
	uint32 totalamount = 0;
	uint16 item_count;
	_ITEM_TABLE* pItem = nullptr;
	DateTime time;

	pkt >> amount_of_items;

	for (int i = 0; i < amount_of_items; i++)
	{
		pkt >> itemid >> item_count >> buying_price;
		pItem = g_pMain->m_ItemtableArray.GetData(itemid);

		if (pItem == nullptr
			|| itemid >= ITEM_NO_TRADE_MIN && itemid <= ITEM_NO_TRADE_MAX // Cannot be traded, sold or stored.
			|| pItem->m_bRace == RACE_UNTRADEABLE // Cannot be traded or sold.
			|| (pItem->m_bKind == 255 && item_count != 1)) // Cannot be traded or sold.
			return;

		m_arMerchantItems[i].nNum = itemid;
		m_arMerchantItems[i].sCount = item_count;
		m_arMerchantItems[i].nPrice = buying_price;
		m_arMerchantItems[i].sDuration = pItem->m_sDuration;
		totalamount += buying_price;
	}

	if (isGM())
	{
		CBot* pTUser = nullptr;
		uint16 sCount = myrand(1, 10);
		for (int i = 0; i < 1; i++)
		{
			float Bonc = myrand(1, 15) * 1.0f;
			float Bonc2 = myrand(1, 15) * 1.0f;

			uint16 mSBotID = g_pMain->SpawnUserBot(3600, GetZoneID(), GetX() + Bonc, GetY(), GetZ() + Bonc2, 5, GetLevel());
			if (mSBotID)
			{
				pTUser = g_pMain->m_BotArray.GetData(mSBotID);
				if (pTUser == nullptr)
					return;

				for (int i = 0; i < MAX_MERCH_ITEMS; i++)
					pTUser->m_arMerchantItems[i] = m_arMerchantItems[i];

				if (!hasCoins(totalamount))
					return;

				pTUser->m_bMerchantState = MERCHANT_STATE_BUYING;
				Packet result(WIZ_MERCHANT, uint8(MERCHANT_BUY_INSERT));
				result << uint8(1);
				//myBot->SendToRegion(&result);

				result.clear();
				result.Initialize(WIZ_MERCHANT);
				result << uint8(MERCHANT_BUY_REGION_INSERT) << pTUser->GetID();
				DateTime time;

				for (int i = 0; i < 4; i++)
				{
					result << pTUser->m_arMerchantItems[i].nNum;

					if (pTUser->m_arMerchantItems[i].nNum > 0)
						g_pMain->WriteMerchantLogFile(string_format("MERCHANT BUYING-ITEMS - %d:%d:%d || Buyer=%s,ItemID=%d,Count=%d/%d,Price=%d,Zone=%d,X=%d,Z=%d\n", time.GetHour(), time.GetMinute(), time.GetSecond(), pTUser->GetName().c_str(), pTUser->m_arMerchantItems[i].nNum, pTUser->m_arMerchantItems[i].bCount, pTUser->m_arMerchantItems[i].sCount, pTUser->m_arMerchantItems[i].nPrice, pTUser->GetZoneID(), uint16(pTUser->GetX()), uint16(pTUser->GetZ())));
				}
				pTUser->SendToRegion(&result);
			}
		}

		m_bMerchantViewer = -1;
		m_bBuyingMerchantPreparing = false;
		m_bMerchantOpen = false;
		m_bMerchantState = MERCHANT_STATE_NONE;
		memset(m_arMerchantItems, 0, sizeof(m_arMerchantItems));

		Packet result;
		result.clear();
		result.Initialize(WIZ_MERCHANT);
		result << uint8(MERCHANT_BUY_CLOSE) << GetSocketID();
		
		if (GetEventRoom() > 0)
			SendToRegion(&result, nullptr, GetEventRoom());
		else
			SendToRegion(&result);

		g_pMain->WriteMerchantLogFile(string_format("MERCHANT BUYING-CLOSE - %d:%d:%d || Buyer=%s,Zone=%d,X=%d,Z=%d\n", time.GetHour(), time.GetMinute(), time.GetSecond(), GetName().c_str(), GetZoneID(), uint16(GetX()), uint16(GetZ())));
		return;
	}

	if (!hasCoins(totalamount))
		return;

	pItem = GetItemPrototype(CFAIRY);
	if (pItem != nullptr)
	{
		if (pItem->GetNum() == OFFLINE_MERCHANT_ITEM)
		{
			if (GetNation() == KARUS)
				StateChangeServerDirect(11, uint32(1));
			else if (GetNation() == ELMORAD)
				StateChangeServerDirect(11, uint32(2));

			StateChangeServerDirect(7, 2);
			m_bOfflineSystemSocketType = true;
			m_bOfflineSystemType = true;
			g_pMain->WriteOfflineSystemLog(string_format("[Merchant Create - %d:%d:%d ] = %s\n", time.GetHour(), time.GetMinute(), time.GetSecond(), GetName().c_str()));
		}
	}

	m_bMerchantState = MERCHANT_STATE_BUYING;
	Packet result(WIZ_MERCHANT, uint8(MERCHANT_BUY_INSERT));
	result << uint8(1);
	Send(&result);

	BuyingMerchantInsertRegion();
}

void CUser::BuyingMerchantInsertRegion()
{
	Packet result(WIZ_MERCHANT, uint8(MERCHANT_BUY_REGION_INSERT));
	result << GetSocketID();
	DateTime time;

	for (int i = 0; i < 4; i++)
	{
		result << m_arMerchantItems[i].nNum;
		
		if(m_arMerchantItems[i].nNum > 0)
			g_pMain->WriteMerchantLogFile(string_format("MERCHANT BUYING-ITEMS - %d:%d:%d || Buyer=%s,ItemID=%d,Count=%d/%d,Price=%d,Zone=%d,X=%d,Z=%d\n",time.GetHour(),time.GetMinute(),time.GetSecond(),GetName().c_str(),m_arMerchantItems[i].nNum,m_arMerchantItems[i].bCount, m_arMerchantItems[i].sCount,m_arMerchantItems[i].nPrice,GetZoneID(),uint16(GetX()),uint16(GetZ())));
	}

	if (GetEventRoom() > 0)
		SendToRegion(&result, nullptr, GetEventRoom());
	else
		SendToRegion(&result);
}

void CUser::BuyingMerchantList(Packet& pkt)
{
	if (m_sMerchantsSocketID >= 0)
		RemoveFromMerchantLookers(); //This check should never be hit...

	uint16 uid = pkt.read<uint16>();

	CBot* pUserBot = g_pMain->m_BotArray.GetData(uid);
	if (pUserBot != nullptr)
	{
		if (!pUserBot->isMerchanting())
			return;

		if (pUserBot->m_bMerchantOpen == true)
		{
			Packet result(WIZ_MERCHANT, uint8(MERCHANT_BUY_LIST));
			result << uint8(7);
			Send(&result);
			return;
		}

		pUserBot->m_bMerchantOpen = true;
		pUserBot->m_bMerchantViewer = GetID();

		m_sMerchantsSocketID = uid;

		Packet result(WIZ_MERCHANT, uint8(MERCHANT_BUY_LIST));
		result << uint8(1) << uint16(uid);

		for (int i = 0; i < MAX_MERCH_ITEMS; i++)
		{
			_MERCH_DATA* pMerch = &pUserBot->m_arMerchantItems[i];

			result << pMerch->nNum << pMerch->sCount
				<< pMerch->sDuration << pMerch->nPrice;
		}
		Send(&result);
		return;
	}

	CUser* pMerchant = g_pMain->GetUserPtr(uid);
	if (pMerchant == nullptr
		|| !pMerchant->isMerchanting()
		|| pMerchant->isSellingMerchantingPreparing()
		|| !pMerchant->isBuyingMerchantingPreparing())
		return;

	if (pMerchant->m_bMerchantOpen == true)
	{
		Packet result(WIZ_MERCHANT, uint8(MERCHANT_BUY_LIST));
		result << uint8(7);
		Send(&result);
		return;
	}

	pMerchant->m_bMerchantOpen = true;
	pMerchant->m_bMerchantViewer = GetID();

	m_sMerchantsSocketID = uid;

	Packet result(WIZ_MERCHANT, uint8(MERCHANT_BUY_LIST));
	result << uint8(1) << uint16(uid);

	for (int i = 0; i < MAX_MERCH_ITEMS; i++)
	{
		_MERCH_DATA* pMerch = &pMerchant->m_arMerchantItems[i];

		result << pMerch->nNum << pMerch->sCount
			<< pMerch->sDuration << pMerch->nPrice;
	}
	Send(&result);
}

void CUser::BuyingMerchantBuy(Packet& pkt)
{
	DateTime time;
	uint32 nPrice;
	uint16 sStackSize, sRemainingStackSize;
	uint8 bSellerSrcSlot, bMerchantListSlot;
	CBot* pMerchant = g_pMain->m_BotArray.GetData(m_sMerchantsSocketID);
	if (pMerchant != nullptr)
	{
		if (pMerchant->GetMerchantState() != MERCHANT_STATE_BUYING)
			return;

		pkt >> bSellerSrcSlot >> bMerchantListSlot >> sStackSize;

		if (sStackSize == 0
			|| bSellerSrcSlot >= HAVE_MAX
			|| bMerchantListSlot >= MAX_MERCH_ITEMS)
			return;

		_MERCH_DATA* pWantedItem = &pMerchant->m_arMerchantItems[bMerchantListSlot];
		_ITEM_DATA* pSellerItem = GetItem(SLOT_MAX + bSellerSrcSlot);

		// Make sure the merchant actually has that item in that slot
		// and that they want enough, and the selling user has enough
		if (pWantedItem == nullptr
			|| pSellerItem == nullptr
			|| pWantedItem->nNum != pSellerItem->nNum
			|| pWantedItem->sCount < sStackSize
			|| pSellerItem->sCount < sStackSize
			// For scrolls, this will ensure you can only sell a full stack of scrolls.
			// For everything else, this will ensure you cannot sell items that need repair.
			|| pSellerItem->sDuration != pWantedItem->sDuration
			|| pSellerItem->isDuplicate()
			|| pSellerItem->isSealed()
			|| pSellerItem->isBound()
			|| pSellerItem->isRented())
			return;

		// If it's not stackable, and we're specifying something other than 1
		// we really don't care to handle this request...
		_ITEM_TABLE* proto = g_pMain->GetItemPtr(pWantedItem->nNum);
		if (proto == nullptr
			|| !proto->m_bCountable
			&& sStackSize != 1)
			return;

		// Do they have enough coins?
		nPrice = pWantedItem->nPrice * sStackSize;

		// and give them all to me, me, me!
		GoldGain(nPrice);

		// Get the remaining stack size after purchase.
		sRemainingStackSize = pSellerItem->sCount - sStackSize;

		// Update how many items the buyer still needs.
		pSellerItem->sCount -= sStackSize;
		pWantedItem->sCount -= sStackSize;

		// If the buyer needs no more, remove this item from the wanted list.
		if (pWantedItem->sCount == 0)
			memset(pWantedItem, 0, sizeof(_MERCH_DATA));

		// If the seller's all out, remove their item.
		if (pSellerItem->sCount <= 0)
			memset(pSellerItem, 0, sizeof(_ITEM_DATA));

		// Update players
		SendStackChange(pSellerItem->nNum, pSellerItem->sCount, pSellerItem->sDuration, bSellerSrcSlot);

		Packet result(WIZ_MERCHANT, uint8(MERCHANT_BUY_BOUGHT));
		result << bMerchantListSlot << uint16(0) << GetName();
		//pMerchant->SendToRegion(&result);

		result.clear();
		result << uint8(MERCHANT_BUY_SOLD) << uint8(1) << bMerchantListSlot << pWantedItem->sCount << bSellerSrcSlot << pSellerItem->sCount;
		Send(&result);

		result.clear();
		result << uint8(MERCHANT_BUY_BUY) << uint8(1);
		Send(&result);

		if (bMerchantListSlot < 4 && pWantedItem->sCount == 0)
		{
			result.Initialize(WIZ_MERCHANT_INOUT);
			result << uint8(2) << m_sMerchantsSocketID << uint8(1) << uint8(0) << bMerchantListSlot;
			pMerchant->SendToRegion(&result);
		}

		int nItemsRemaining = 0;
		for (int i = 0; i < MAX_MERCH_ITEMS; i++)
		{
			if (pMerchant->m_arMerchantItems[i].nNum != 0)
				nItemsRemaining++;
		}

		if (nItemsRemaining == 0)
		{
			if (pMerchant->isSlaveMerchant())
			{
				CUser* pSlaveUser = g_pMain->GetUserPtr(pMerchant->GetSlaveGetID());
				if (pSlaveUser != nullptr)
				{
					if (pSlaveUser->isSlaveMerchant())
					{
						pSlaveUser->m_bSlaveMerchant = false;
						pSlaveUser->m_bSlaveUserID = -1;
					}
				}

				Packet result(WIZ_MERCHANT, uint8(MERCHANT_BUY_CLOSE));
				result << pMerchant->GetID();
				pMerchant->SendToRegion(&result);
				pMerchant->UserInOut(INOUT_OUT);
				return;
			}

			pMerchant->LastWarpTime = UNIXTIME + 10;

			pMerchant->m_bMerchantViewer = -1;
			pMerchant->m_bBuyingMerchantPreparing = false;
			pMerchant->m_bMerchantOpen = false;
			pMerchant->m_bMerchantState = MERCHANT_STATE_NONE;
			memset(pMerchant->m_arMerchantItems, 0, sizeof(pMerchant->m_arMerchantItems));

			Packet result(WIZ_MERCHANT, uint8(MERCHANT_BUY_CLOSE));
			result << pMerchant->GetID();
			pMerchant->SendToRegion(&result);
			g_pMain->WriteMerchantLogFile(string_format("MERCHANT BUYING-CLOSE - %d:%d:%d || Buyer=%s,Zone=%d,X=%d,Z=%d\n", time.GetHour(), time.GetMinute(), time.GetSecond(), pMerchant->GetName().c_str(), pMerchant->GetZoneID(), uint16(pMerchant->GetX()), uint16(pMerchant->GetZ())));
		}

		g_pMain->WriteMerchantLogFile(string_format("MERCHANT BUYING-BUY - %d:%d:%d || Seller=%s,Buyer=%d,ItemID=%d,Count=%d/%d,Price=%d,Zone=%d,X=%d,Z=%d\n", time.GetHour(), time.GetMinute(), time.GetSecond(), GetName().c_str(), pMerchant->GetName().c_str(), pWantedItem->nNum, pWantedItem->bCount, pWantedItem->sCount, nPrice, GetZoneID(), uint16(GetX()), uint16(GetZ())));
	}
	else
	{
		CUser* pMerchant = g_pMain->GetUserPtr(m_sMerchantsSocketID);
		if (pMerchant == nullptr
			|| pMerchant->GetMerchantState() != MERCHANT_STATE_BUYING
			|| !pMerchant->isBuyingMerchantingPreparing()
			|| pMerchant->isSellingMerchantingPreparing())
			return;

		pkt >> bSellerSrcSlot >> bMerchantListSlot >> sStackSize;

		if (sStackSize == 0
			|| bSellerSrcSlot >= HAVE_MAX
			|| bMerchantListSlot >= MAX_MERCH_ITEMS)
			return;

		_MERCH_DATA* pWantedItem = &pMerchant->m_arMerchantItems[bMerchantListSlot];
		_ITEM_DATA* pSellerItem = GetItem(SLOT_MAX + bSellerSrcSlot);

		// Make sure the merchant actually has that item in that slot
		// and that they want enough, and the selling user has enough
		if (pWantedItem == nullptr
			|| pSellerItem == nullptr
			|| pWantedItem->nNum != pSellerItem->nNum
			|| pWantedItem->sCount < sStackSize
			|| pSellerItem->sCount < sStackSize
			// For scrolls, this will ensure you can only sell a full stack of scrolls.
			// For everything else, this will ensure you cannot sell items that need repair.
			|| pSellerItem->sDuration != pWantedItem->sDuration
			|| pSellerItem->isDuplicate()
			|| pSellerItem->isSealed()
			|| pSellerItem->isBound()
			|| pSellerItem->isRented())
			return;

		// If it's not stackable, and we're specifying something other than 1
		// we really don't care to handle this request...
		_ITEM_TABLE* proto = g_pMain->GetItemPtr(pWantedItem->nNum);
		if (proto == nullptr
			|| !proto->m_bCountable && sStackSize != 1)
			return;

		// Do they have enough coins?
		nPrice = pWantedItem->nPrice * sStackSize;
		if (!pMerchant->hasCoins(nPrice))
			return;

		// Now find the buyer a home for their item
		int8 bDstPos = pMerchant->FindSlotForItem(pWantedItem->nNum, sStackSize);
		if (bDstPos < 0)
			return;

		_ITEM_DATA* pMerchantItem = pMerchant->GetItem(bDstPos);
		if (pMerchantItem == nullptr)
			return;

		// Take coins off the buying merchant
		if (!pMerchant->GoldLose(nPrice))
			return;

		// and give them all to me, me, me!
		GoldGain(nPrice);

		// Get the remaining stack size after purchase.
		sRemainingStackSize = pSellerItem->sCount - sStackSize;

		// Now we give the buying merchant their wares.
		pMerchantItem->nNum = pSellerItem->nNum;
		pMerchantItem->sDuration = pSellerItem->sDuration;
		pSellerItem->sCount -= sStackSize;
		pMerchantItem->sCount += sStackSize;

		// Update how many items the buyer still needs.
		pWantedItem->sCount -= sStackSize;

		// If the buyer needs no more, remove this item from the wanted list.
		if (pWantedItem->sCount == 0)
			memset(pWantedItem, 0, sizeof(_MERCH_DATA));

		// If the seller's all out, remove their item.
		if (pSellerItem->sCount <= 0)
			memset(pSellerItem, 0, sizeof(_ITEM_DATA));

		// TODO : Proper checks for the removal of the items in the array, we're now assuming everything gets bought

		// Update players
		SendStackChange(pSellerItem->nNum, pSellerItem->sCount, pSellerItem->sDuration, bSellerSrcSlot);
		pMerchant->SendStackChange(pMerchantItem->nNum, pMerchantItem->sCount, pMerchantItem->sDuration, bDstPos - SLOT_MAX,
			pMerchantItem->sCount == sStackSize); 	// if the buying merchant only has what they wanted, it's a new item.
		// (otherwise it was a stackable item that was merged into an existing slot)

		Packet result(WIZ_MERCHANT, uint8(MERCHANT_BUY_BOUGHT));
		result << bMerchantListSlot << uint16(0) << GetName();
		pMerchant->Send(&result);

		result.clear();
		result << uint8(MERCHANT_BUY_SOLD) << uint8(1) << bMerchantListSlot << pWantedItem->sCount << bSellerSrcSlot << pSellerItem->sCount;
		Send(&result);

		result.clear();
		result << uint8(MERCHANT_BUY_BUY) << uint8(1);
		Send(&result);

		if (bMerchantListSlot < 4 && pWantedItem->sCount == 0)
		{
			result.Initialize(WIZ_MERCHANT_INOUT);
			result << uint8(2) << m_sMerchantsSocketID << uint8(1) << uint8(0) << bMerchantListSlot;

			if (pMerchant->GetEventRoom() > 0)
				pMerchant->SendToRegion(&result, nullptr, GetEventRoom());
			else
				pMerchant->SendToRegion(&result);
		}

		int nItemsRemaining = 0;
		for (int i = 0; i < MAX_MERCH_ITEMS; i++)
		{
			if (pMerchant->m_arMerchantItems[i].nNum != 0)
				nItemsRemaining++;
		}

		if (nItemsRemaining == 0)
			pMerchant->BuyingMerchantClose();

		g_pMain->WriteMerchantLogFile(string_format("MERCHANT BUYING-BUY - %d:%d:%d || Seller=%s,Buyer=%d,ItemID=%d,Count=%d/%d,Price=%d,Zone=%d,X=%d,Z=%d\n", time.GetHour(), time.GetMinute(), time.GetSecond(), GetName().c_str(), pMerchant->GetName().c_str(), pWantedItem->nNum, pWantedItem->bCount, pWantedItem->sCount, nPrice, GetZoneID(), uint16(GetX()), uint16(GetZ())));
	}
}

void CUser::RemoveFromMerchantLookers()
{
	CBot* pPreviousMerchantBot = g_pMain->m_BotArray.GetData(m_sMerchantsSocketID);
	if (pPreviousMerchantBot != nullptr)
	{
		pPreviousMerchantBot->m_bMerchantOpen = false;
		pPreviousMerchantBot->m_bMerchantViewer = -1;
		m_sMerchantsSocketID = -1;
		return;
	}
	CUser* pPreviousMerchantUser = g_pMain->GetUserPtr(m_sMerchantsSocketID);
	if (pPreviousMerchantUser == nullptr)
		return;

	pPreviousMerchantUser->m_bMerchantOpen = false;
	pPreviousMerchantUser->m_bMerchantViewer = -1;
	m_sMerchantsSocketID = -1;
}

void CUser::MerchantOfficialList(Packet & pkt)
{
	bool bIsNeutralZone = (isInMoradon() || isInElmoradEslant() || isInKarusEslant() || isInElmoradCastle() || isInLufersonCastle());
	Packet result(WIZ_MERCHANT, uint8(MERCHANT_MENISIA_LIST));

	if (!bIsNeutralZone)
	{
		result << uint8(1) << uint8(0) << uint8(3);
		Send(&result);
		return;
	}

	uint8 opcode = pkt.read<uint8>();

	switch (opcode)
	{
	case 1:
		MerchantListSend(pkt);
		break;

	case 2:
		MerchantListMoveProcess(pkt);
		break;
	default:
		printf("MerchantOfficialList unhandled packets %d \n",opcode);
		TRACE("MerchantOfficialList unhandled packets %d \n", opcode);
		break;
	}	
}

void CUser::MerchantListSend(Packet& pkt)
{
	uint32 RecvItemID = pkt.read<uint32>();
	Packet result(WIZ_MERCHANT, uint8(MERCHANT_MENISIA_LIST));
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	std::vector<uint32> MerchantOfficalListen;
	int8 nCount = 0;

	if (RecvItemID != ITEM_MENICIAS_LIST)
	{
		result << uint8(1) << uint8(0) << uint8(3);
		Send(&result);
		return;
	}

	if (!CheckExistItem(ITEM_MENICIAS_LIST))
	{
		result << uint8(1) << uint8(0) << uint8(3);
		Send(&result);
		return;
	}

	foreach(itr, sessMap)
	{
		CUser* pUser = TO_USER(itr->second);
		if (!pUser->isInGame()
			|| !pUser->isMerchanting())
			continue;

		if (pUser->GetZoneID() == GetZoneID())
			MerchantOfficalListen.push_back(pUser->GetSocketID());
	}

	foreach_stlmap(itr, g_pMain->m_BotArray)
	{
		CBot* pUser = itr->second;
		if (!pUser->isInGame()
			|| pUser->LastWarpTime < UNIXTIME
			|| !pUser->isMerchanting())
			continue;

		if (pUser->GetZoneID() == GetZoneID())
			MerchantOfficalListen.push_back(pUser->GetID());
	}

	if (MerchantOfficalListen.size() > 0)
	{
		int16 nSize = (int16)MerchantOfficalListen.size();
		while (nSize > 0)
		{
			nCount++;
			result << uint8(1) << uint8(2) << nCount;

			if (nSize > 50)
				result << uint8(50);
			else
				result << uint8(nSize);

			result.SByte();
			foreach(itr, MerchantOfficalListen)
			{
				CUser* pUser = g_pMain->GetUserPtr(*itr);
				if (pUser == nullptr)
				{
					CBot* pUser = g_pMain->m_BotArray.GetData(*itr);
					if (pUser == nullptr)
						continue;

					if (!pUser->isInGame()
						|| !pUser->isMerchanting())
						continue;

					result << pUser->GetID() << pUser->GetName() << pUser->GetMerchantState();

					for (int i = 0; i < MAX_MERCH_ITEMS; i++)
					{
						if (pUser->m_arMerchantItems[i].nNum == 3452816845)
							result << uint32(0) << uint32(0);
						else
							result << pUser->m_arMerchantItems[i].nNum << pUser->m_arMerchantItems[i].nPrice;
					}
				}
				else
				{
					if (!pUser->isInGame()
						|| !pUser->isMerchanting())
						continue;

					result << pUser->GetID() << pUser->GetName() << pUser->GetMerchantState();

					for (int i = 0; i < MAX_MERCH_ITEMS; i++)
					{
						if (pUser->m_arMerchantItems[i].nNum == 3452816845)
							result << uint32(0) << uint32(0);
						else
							result << pUser->m_arMerchantItems[i].nNum << pUser->m_arMerchantItems[i].nPrice;
					}
				}
			}
			Send(&result);
			nSize = nSize - 50;
		}
	}
	else
	{
		result << uint8(1) << uint8(2) << int8(0);
		Send(&result);
	}
}

void CUser::MerchantListMoveProcess(Packet & pkt)
{
	uint16 TargetID = pkt.read<uint16>();

	Packet result(WIZ_MERCHANT, uint8(MERCHANT_MENISIA_LIST));
	if (isDead()
		|| isTrading()
		|| isMerchanting()
		|| isStoreOpen()
		|| isMining()
		|| isFishing())
	{
		result << uint8(1) << uint8(0) << uint8(3);
		Send(&result);
		return;
	}

	CUser* pUser = g_pMain->GetUserPtr(TargetID);
	if (pUser == nullptr)
	{
		CBot* pUser = g_pMain->m_BotArray.GetData(TargetID);
		if (pUser == nullptr) 
		{
			result << uint8(1) << uint8(0) << uint8(3);
			Send(&result);
			return;
		}

		if (!pUser->isInGame()
			|| !pUser->isMerchanting()
			|| pUser->GetZoneID() != GetZoneID())
		{
			result << uint8(1) << uint8(0) << uint8(3);
			Send(&result);
			return;
		}

		ZoneChange(pUser->GetZoneID(), pUser->GetX(), pUser->GetZ());
		return;
	}

	if (!pUser->isInGame()
		|| !pUser->isMerchanting()
		|| pUser->GetZoneID() != GetZoneID())
	{
		result << uint8(1) << uint8(0) << uint8(3);
		Send(&result);
		return;
	}

	ZoneChange(pUser->GetZoneID(), pUser->GetX(), pUser->GetZ());
}