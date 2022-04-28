#include "stdafx.h"

/**
* @brief 1.ID : Mysterious Ore 2.ID : Gold Ore
* Kodlamak Sanattýr. TheThyke
*/
#pragma region CUser::MiningExchange(uint16 ID)
void CUser::MiningExchange(uint16 ID)
{
	uint32 bRandArray[10000];
	memset(&bRandArray, 0, sizeof(bRandArray));
	int offset = 0;
	_MINING_EXCHANGE * pMiningExchange;
	std::vector<uint32> MiningExchangeList;
	uint8 sItemSlot = 0;
	uint16 sEffect = 0;
	uint16 MiningÝndex = 0;

	if (g_pMain->m_MiningExchangeArray.GetSize() > 0)
	{
		foreach_stlmap(itr, g_pMain->m_MiningExchangeArray)
		{
			if (itr->second == nullptr)
				continue;

			if (std::find(MiningExchangeList.begin(), MiningExchangeList.end(), itr->second->nIndex) == MiningExchangeList.end())
			{
				switch (ID)
				{
				case 1:
					if (itr->second->OreType == 1 && itr->second->sNpcID == 31511)
						MiningExchangeList.push_back(itr->second->nIndex);
					break;
				case 2:
					if (itr->second->OreType == 2 && itr->second->sNpcID == 31511)
						MiningExchangeList.push_back(itr->second->nIndex);
					break;
				default:
					printf("Mining Exchange unhandled OreType %d \n", itr->second->OreType);
					TRACE("Mining Exchange unhandled OreType %d \n", itr->second->OreType);
					break;
				}
			}
		}
	}

	if (MiningExchangeList.size() > 0)
	{
		foreach(itr, MiningExchangeList)
		{
			pMiningExchange = g_pMain->m_MiningExchangeArray.GetData(*itr);

			if (pMiningExchange == nullptr)
				return;

			if (offset >= 10000)
				break;

			for (int i = 0; i < int(pMiningExchange->SuccesRate / 5); i++)
			{
				if (i + offset >= 10000)
					break;

				bRandArray[offset + i] = pMiningExchange->nGiveItemNum;
			}

			offset += int(pMiningExchange->SuccesRate / 5);
		}

		uint32 bRandSlot = myrand(0, offset);
		uint32 nItemID = bRandArray[bRandSlot];

		uint32 nIndex;
		foreach_stlmap(str, g_pMain->m_MiningExchangeArray)
		{
			if (str->second->nGiveItemNum == nItemID)
			{
				nIndex = str->second->nIndex;
				break;
			}
			else
				continue;
		}

		_ITEM_TABLE *pItem = g_pMain->m_ItemtableArray.GetData(nItemID);
		if (pItem == nullptr)
			return;

		uint8 bFreeSlots = 0;
		for (int i = SLOT_MAX; i < SLOT_MAX + HAVE_MAX; i++)
		{
			if (GetItem(i)->nNum == 0 && ++bFreeSlots >= ITEMS_IN_EXCHANGE_GROUP)
				break;
		}

		if (bFreeSlots < 1)
			return;

		int SlotForItem = FindSlotForItem(pItem->m_iNum, 1);
		if (SlotForItem == -1)
			return;

		_MINING_EXCHANGE * pMiningExchangeEffect = g_pMain->m_MiningExchangeArray.GetData(nIndex);
		if (pMiningExchangeEffect == nullptr)
			return;

		sItemSlot = GetEmptySlot() - SLOT_MAX;
		RobItem(pMiningExchange->nOriginItemNum, 1);
		GiveItem(nItemID, 1);

		if (pMiningExchangeEffect->GiveEffect == 1)
		{
			sEffect = 13081; // "Item" effect
			uint16 resultCode = MiningResultSuccess;
			Packet x(WIZ_MINING, uint8(MiningAttempt));
			x << resultCode << GetID() << sEffect;
			Send(&x);
		}
	}
}
#pragma endregion