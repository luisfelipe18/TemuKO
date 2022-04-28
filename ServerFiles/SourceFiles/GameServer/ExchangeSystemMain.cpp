#include "stdafx.h"

/**
* @brief	Packet handler for the assorted systems that
* 			were deemed to come under the 'upgrade' system.
*
* @param	pkt	The packet.
*/
void CUser::ExchangeSystemProcess(Packet & pkt)
{
	if (isTrading()
		|| isDead()
		|| isMining()
		|| isFishing()
		|| isSellingMerchantingPreparing()
		|| isBuyingMerchantingPreparing()
		|| isMerchanting()
		|| !isInGame())
		return;

	uint8 opcode = pkt.read<uint8>();

	switch (opcode)
	{
	case ITEM_UPGRADE:
		ItemUpgrade(pkt);
		break;
	case ITEM_ACCESSORIES:
		ItemUpgradeAccessories(pkt);
		break;
	case ITEM_BIFROST_EXCHANGE:
		BifrostPieceProcess(pkt);
		break;
	case PET_HATCHING_TRANSFROM:
		HactchingTransformExchange(pkt);
		break;
	case ITEM_UPGRADE_REBIRTH:
		ItemUpgradeRebirth(pkt);
		break;
	case ITEM_SEAL:
		ItemSealProcess(pkt);
		break;
	case ITEM_CHARACTER_SEAL:
		CharacterSealProcess(pkt);
		break;
	case PET_IMAGE_TRANSFORM:
		HatchingImageTransformExchange(pkt);
		break;
	case SPECIAL_PART_SEWING:
		SpecialItemExchange(pkt);
		break;
	case ITEM_OLDMAN_EXCHANGE:
		ItemSmashExchange(pkt);
		break;
	default:
		TRACE("Upgrade OpCode %u \n", opcode);
		printf("Upgrade OpCode %u \n", opcode);
		break;
	}
}

/*#pragma region
void CUser::ItemUpgradePetExchange(Packet & pkt)
{
enum ResultOpCodes
{
Failed			= 1,
InvalidName		= 2,
Familiarcannot	= 3,
LimitExceeded	= 4,
};

Packet result(WIZ_ITEM_UPGRADE, uint8(PET_ITEM_EXCHANGE));
int8 bResult = UpgradeErrorCodes::UpgradeFailed;
uint16 sNpcID;
int32 nItemID; int8 bPos;
ITEM_UPGRADE_ITEM pUpgradeItem;
memset(&pUpgradeItem, 0, sizeof(pUpgradeItem));
std::string strKaulName;
_ITEM_DATA  * pItem =  nullptr;
_ITEM_TABLE * pTable =  nullptr;
uint8 Error = 0;

if (isTrading() || isMerchanting() || isMining() || !isInGame() || isFishing())
{
Error = Failed;
goto fail_return;
}
pkt.DByte();
pkt >> sNpcID >> nItemID >> bPos >> strKaulName;

if(strKaulName.length() == 0 || strKaulName.length() > 15)
Error = InvalidName;
goto fail_return;

if (bPos != -1 && bPos >= HAVE_MAX)
Error = Failed;
goto fail_return;

if(nItemID > 0 && bPos >= 0 && bPos <= HAVE_MAX)
{
pUpgradeItem.nItemID = nItemID;
pUpgradeItem.bPos = bPos;
}
else
Error = Failed;
goto fail_return;

pItem = GetItem(SLOT_MAX + pUpgradeItem.bPos);
pTable =  g_pMain->GetItemPtr(pUpgradeItem.nItemID);
if(pItem == nullptr
|| pTable == nullptr
|| pTable->m_bRace == 20
|| pItem->nNum != pUpgradeItem.nItemID)
goto fail_return;

else if(pItem->isBound()
|| pItem->isDuplicate()
|| pItem->isRented()
|| pItem->isSealed())
{
Error = Familiarcannot;
goto fail_return;
}

bResult =  UpgradeErrorCodes::UpgradeSucceeded;

result.DByte();
result << bResult << result << uint32(pItem->nNum) <<  pUpgradeItem.bPos << uint8(0) << uint8(1) << uint8(0) << uint8(1);
Send(&result);

fail_return:
result << uint8(0) << uint8(Error);
Send(&result);
};
#pragma endregion

void CUser::ItemUpgradePetImage(Packet & pkt){};*/