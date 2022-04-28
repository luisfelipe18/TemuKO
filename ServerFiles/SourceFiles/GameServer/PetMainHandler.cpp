#include "stdafx.h"
#include "MagicInstance.h"
#include "DBAgent.h"

void CUser::MainPetProcess(Packet& pkt)
{
	enum PetOpCodes
	{
		ModeFunction = 1,
		PetUseSkill = 2
	};

	uint8 OpCode = pkt.read<uint8>();
	switch (OpCode)
	{
	case ModeFunction:
		SelectingModeFunction(pkt);
		break;
	case PetUseSkill:
		HandlePetUseSkill(pkt);
		break;
	default:
		printf("Pet System Main unhandled main case opcodes %d \n", OpCode);
		break;
	}
}

void CUser::HandlePetUseSkill(Packet& pkt)
{
	if (m_PettingOn == nullptr)
		return;

	uint8 bSubCode = pkt.read<uint8>();
	int32 nSkillID = pkt.read<uint32>();

	int16 sCasterID = pkt.read<int16>();
	int16 sTargetID = pkt.read<int16>();

	CNpc* pPet = g_pMain->GetPetPtr(GetSocketID(), GetZoneID());

	if (pPet == nullptr)
		return;

	Unit* pUnit = g_pMain->GetUnitPtr(sTargetID, GetZoneID());
	if (pUnit != nullptr)
	{
		if (pUnit->isPlayer())
		{
			if (pPet->GetPetID() != pUnit->GetID())
				return;
		}
		else if (pUnit->isNPC())
		{
			CNpc* pNpc = g_pMain->GetNpcPtr(pUnit->GetID(), pUnit->GetZoneID());
			if (pNpc != nullptr)
			{
				if (nSkillID != 490010
					&& nSkillID != 490011
					&& nSkillID != 490012
					&& nSkillID != 490013
					&& nSkillID != 490014
					&& nSkillID != 500145
					&& nSkillID != 490016
					&& nSkillID != 490017
					&& nSkillID != 490018
					&& nSkillID != 490019
					&& nSkillID != 490020
					&& nSkillID != 500146)
				{
					if (!pNpc->isMonster())
						return;

					if (pNpc->isPet())
						return;

					float	warp_x = pNpc->GetX() - pPet->GetX(),
						warp_z = pNpc->GetZ() - pPet->GetZ();

					// Unable to work out orientation, so we'll just fail (won't be necessary with m_sDirection).
					float	distance = sqrtf(warp_x * warp_x + warp_z * warp_z);
					if (distance == 0.0f)
						return;

					warp_x /= distance; warp_z /= distance;
					warp_x *= 2; warp_z *= 2;
					warp_x += pNpc->GetX(); warp_z += pNpc->GetZ();

					pPet->SendMoveResult(warp_x, 0, warp_z, distance);
				}

				if (nSkillID == 490010
					&& nSkillID == 490011
					&& nSkillID == 490012
					&& nSkillID == 490013
					&& nSkillID == 490014
					&& nSkillID == 500145
					&& nSkillID == 490016
					&& nSkillID == 490017
					&& nSkillID == 490018
					&& nSkillID == 490019
					&& nSkillID == 490020
					&& nSkillID == 500146)
				{
					if (pNpc->GetPetID() <= 0)
						return;

					if (pNpc->GetPetID() != GetSocketID())
						return;
				}
			}
		}
	}

	Packet result(WIZ_MAGIC_PROCESS, bSubCode); // here we emulate a skill packet to be handled.
	result << nSkillID << pPet->GetID() << sTargetID << int16(pPet->GetX()) << int16(pPet->GetY()) << int16(pPet->GetZ());
	CMagicProcess::MagicPacketNpc(result, pPet);

	if (m_PettingOn->sStateChange == MODE_DEFENCE)
	{
		result.clear();
		result.Initialize(WIZ_PET);
		result << uint8(5) << uint8(MODE_ATTACK);
		SelectingModeFunction(result);
	}
	PetSatisFactionUpdate(-10);
}

void CUser::SelectingModeFunction(Packet& pkt)
{
	enum PetOpCodes
	{
		NormalMode = 5,
		FoodMode = 16
	};

	if (m_PettingOn == nullptr)
		return;

	uint8 SupCode = pkt.read<uint8>();
	uint8 Mode = pkt.read<uint8>();
	switch (SupCode)
	{
	case NormalMode:
	{
		switch (Mode)
		{
		case MODE_ATTACK:
		case MODE_DEFENCE:
		case MODE_LOOTING:
		{
			m_PettingOn->sStateChange = Mode;

			if (m_PettingOn->sStateChange == MODE_DEFENCE)
			{
				if (m_PettingOn->sAttackStart)
				{
					m_PettingOn->sAttackStart = false;
					m_PettingOn->sAttackTargetID = -1;
				}
			}

			Packet result(WIZ_PET);
			result << uint8(1)
				<< SupCode
				<< Mode
				<< uint16(1);
			Send(&result);
		}
		break;
		case MODE_CHAT:
		{
			pkt.DByte();
			std::string chat;
			pkt >> chat;

			Packet result(WIZ_PET);
			result << uint8(1)
				<< SupCode
				<< Mode;
			result << uint16(1);
			result.append(chat.c_str(), chat.length());

			Send(&result);
		}
		break;
		default:
			printf("SelectingModeFunction unhandled main Mode opcodes %d \n", Mode);
			break;
		}

	}
	break;
	case FoodMode:
		PetFeeding(pkt, Mode);
		break;
	default:
		printf("SelectingModeFunction unhandled main SupCode opcodes %d \n", SupCode);
		break;
	}


}
void CUser::PetFeeding(Packet& pkt, uint8 bType)
{
	if (m_PettingOn == nullptr)
		return;

	_ITEM_DATA* pDstItem = nullptr;
	uint32 nItemID = pkt.read<uint32>();

	Packet result(WIZ_PET, uint8(0x01));
	result << uint8(MODE_FOOD);

	pDstItem = GetItem(SLOT_MAX + bType);
	if (pDstItem == nullptr
		|| pDstItem->nNum != nItemID
		|| pDstItem->isDuplicate()
		|| pDstItem->isExpirationTime()
		|| pDstItem->isRented())
		return;

	if (nItemID != 389570000
		&& nItemID != 389580000
		&& nItemID != 389590000)
		return;

	uint16 sOldSatisfaction = m_PettingOn->sSatisfaction;

	switch (nItemID) {
	case 389570000:
		sOldSatisfaction += (sOldSatisfaction * 20) / 100;
		break;
	case 389580000:
		sOldSatisfaction += (sOldSatisfaction * 50) / 100;
		break;
	case 389590000:
		sOldSatisfaction += (sOldSatisfaction * 100) / 100;
		break;
	}

	if (sOldSatisfaction > 10000)
		sOldSatisfaction = 10000;

	pDstItem->sCount -= 1;

	if (pDstItem->sCount <= 0)
		memset(pDstItem, 0, sizeof(_ITEM_DATA));

	PetSatisFactionUpdate(sOldSatisfaction);
	SetUserAbility(false);
	SendItemWeight();

	result << uint8(1) << bType << pDstItem->nNum << pDstItem->sCount << uint16(0) << uint32(0) << uint16(10000 - sOldSatisfaction);
	Send(&result);
}

void CUser::PetSatisFactionUpdate(int16 amount)
{
	if (m_PettingOn == nullptr)
		return;

	Packet result(WIZ_PET, uint8(1));
	m_PettingOn->sSatisfaction += amount;

	if (m_PettingOn->sSatisfaction >= 10000)
		m_PettingOn->sSatisfaction = 10000;
	else if (m_PettingOn->sSatisfaction <= 0)
		m_PettingOn->sSatisfaction = 0;

	if (m_PettingOn->sSatisfaction <= 0)
	{
		PetOnDeath();
		return;
	}

	result << uint8(MODE_SATISFACTION_UPDATE)
		<< m_PettingOn->sSatisfaction
		<< m_PettingOn->sNid;
	Send(&result);
}

void CUser::PetOnDeath()
{
	if (m_PettingOn == nullptr)
		return;

	CNpc* pPet = g_pMain->GetPetPtr(GetSocketID(), GetZoneID());
	if (pPet == nullptr)
		return;

	Packet result(WIZ_PET, uint8(1));
	result << uint8(5) << uint8(2) << uint16(1) << m_PettingOn->nIndex;
	Send(&result);

	g_pMain->KillNpc(pPet->GetID(), pPet->GetZoneID(), pPet);
	m_PettingOn = nullptr;
	pPet->m_sPetId = -1;
	MagicInstance instance;
	instance.pSkillCaster = this;
	instance.nSkillID = 500117;
	instance.Type9Cancel();
}

void CUser::ShowPetItemInfo(Packet& pkt, uint64 nSerialNum)
{
	Guard Lock(g_pMain->m_PetSystemlock);
	PetDataSystemArray::iterator itr = g_pMain->m_PetDataSystemArray.find(nSerialNum);
	if (itr != g_pMain->m_PetDataSystemArray.end())
	{
		pkt.DByte();
		PET_DATA* pPetData = itr->second;

		if (pPetData == nullptr || pPetData->info == nullptr)
		{
			pkt << uint32(0);
			return;
		}

		pkt << pPetData->nIndex
			<< pPetData->strPetName
			<< uint8(pPetData->info->PetAttack)
			<< uint8(pPetData->bLevel)
			<< uint16((float)pPetData->nExp / (float)pPetData->info->PetExp * 100.0f * 100.0f)
			<< pPetData->sSatisfaction;
	}
	else
		pkt << uint32(0);
}

void CUser::PetSpawnProcess(bool LevelUp)
{
	if (m_PettingOn == nullptr)
		return;

	Packet result;

	m_PettingOn->sHP = m_PettingOn->info->PetMaxHP;
	m_PettingOn->sMP = m_PettingOn->info->PetMaxMP;

	result.DByte();
	result.Initialize(WIZ_PET);
	result << uint8(0x01) << uint8(0x05) << uint8(0x01) << uint8(0x01) << uint8(0x00)
		<< m_PettingOn->nIndex
		<< m_PettingOn->strPetName
		<< uint8(119)
		<< m_PettingOn->bLevel
		<< uint16(((float)m_PettingOn->nExp / (float)m_PettingOn->info->PetExp) * 100) //exp percent 81.00 => 8100
		<< m_PettingOn->info->PetMaxHP
		<< m_PettingOn->sHP
		<< m_PettingOn->info->PetMaxMP
		<< m_PettingOn->sMP
		<< m_PettingOn->sSatisfaction // satisfaction percent?? % ? -> 7400
		<< m_PettingOn->info->PetAttack
		<< m_PettingOn->info->PetDefense
		<< m_PettingOn->info->PetRes
		<< m_PettingOn->info->PetRes
		<< m_PettingOn->info->PetRes
		<< m_PettingOn->info->PetRes
		<< m_PettingOn->info->PetRes
		<< m_PettingOn->info->PetRes;
	for (uint8 j = 0; j < 3; j++)
	{
		result << uint32(0)
			<< uint32(0)
			<< uint32(0)
			<< uint16(0)
			<< uint8(0)
			<< uint32(0);
	}
	for (uint8 j = 0; j < PET_INVENTORY_TOTAL; j++)
	{
		result << m_PettingOn->sItem[j].nNum
			<< m_PettingOn->sItem[j].sCount
			<< m_PettingOn->sItem[j].sDuration
			<< uint16(0) << uint16(0) << uint16(0) << uint8(0)
			<< m_PettingOn->sItem[j].nExpirationTime;
	}
	Send(&result);

	if (LevelUp)
		return;

	result.clear();
	result.Initialize(WIZ_OBJECT_EVENT);
	result << uint8(0x0B) << uint8(0x01) << GetID() << uint32(30403);
	SendToRegion(&result);
}

void CUser::HatchingImageTransformExchange(Packet& pkt)
{
	uint16 sNpcID;
	uint32 IncomingItemID[4] = { 0,0,0,0 };
	uint8 IncomingPosID[4] = { 0,0,0,0 };
	PET_TRANSFORM* pExchange;
	std::vector<uint32> ExchangeIndexList;
	int offset = 0;
	uint32 bRandArray[10000];
	memset(&bRandArray, 0, sizeof(bRandArray));
	pkt >> sNpcID;

	for (int Imagine = 0; Imagine < 4; Imagine++)
	{
		pkt >> IncomingItemID[Imagine] >> IncomingPosID[Imagine];

		if (IncomingItemID[Imagine] != 0)
		{
			_ITEM_DATA* pItem = nullptr; _ITEM_TABLE* pTable = nullptr;
			pTable = g_pMain->m_ItemtableArray.GetData(IncomingItemID[Imagine]);
			pItem = &m_sItemArray[SLOT_MAX + IncomingPosID[Imagine]];
			if (pTable == nullptr
				|| pItem == nullptr
				|| pItem->nNum != IncomingItemID[Imagine]
				|| pItem->isBound()
				|| pItem->isDuplicate()
				|| pItem->isRented()
				|| pItem->isSealed()
				|| IncomingPosID[Imagine] >= HAVE_MAX)
			{
				Packet x(WIZ_ITEM_UPGRADE, uint8(PET_IMAGE_TRANSFORM));
				x << uint8(0) << uint8(1);
				Send(&x);
				return;
			}
		}
	}

	if (g_pMain->m_PetTransformSystemArray.GetSize() != 0)
	{
		foreach_stlmap(itr, g_pMain->m_PetTransformSystemArray)
		{
			PET_TRANSFORM* pPetTransform = itr->second;
			if (pPetTransform == nullptr)
				continue;

			if (IncomingItemID[1] != 0)
			{
				if (pPetTransform->nReqItem0 == IncomingItemID[1])
					ExchangeIndexList.push_back(pPetTransform->sIndex);
			}
		}
	}

	if (ExchangeIndexList.empty())
	{
		Packet x(WIZ_ITEM_UPGRADE, uint8(PET_IMAGE_TRANSFORM));
		x << uint8(0) << uint8(1);
		Send(&x);
		return;
	}

	foreach(itr, ExchangeIndexList)
	{
		pExchange = g_pMain->m_PetTransformSystemArray.GetData(*itr);
		if (pExchange == nullptr)
			continue;

		if (offset >= 10000)
			break;

		for (int i = 0; i < int(pExchange->sPercent); i++)
		{
			if (i + offset >= 10000)
				break;

			bRandArray[offset + i] = pExchange->sIndex;
		}
		offset += int(pExchange->sPercent);
	}

	uint32 bRandSlot = myrand(0, offset);
	uint32 PetImagesIndex = bRandArray[bRandSlot];

	pExchange = g_pMain->m_PetTransformSystemArray.GetData(PetImagesIndex);
	if (pExchange == nullptr)
	{
		Packet x(WIZ_ITEM_UPGRADE, uint8(PET_IMAGE_TRANSFORM));
		x << uint8(0) << uint8(1);
		Send(&x);
		return;
	}

	_ITEM_TABLE* pTable = g_pMain->m_ItemtableArray.GetData(pExchange->nReplaceItem);
	if (pTable == nullptr)
	{
		Packet x(WIZ_ITEM_UPGRADE, uint8(PET_IMAGE_TRANSFORM));
		x << uint8(0) << uint8(1);
		Send(&x);
		return;
	}

	_ITEM_DATA* pImageItem = &m_sItemArray[SLOT_MAX + IncomingPosID[1]];
	if (pImageItem == nullptr)
	{
		Packet x(WIZ_ITEM_UPGRADE, uint8(PET_IMAGE_TRANSFORM));
		x << uint8(0) << uint8(1);
		Send(&x);
		return;
	}

	if (pImageItem->nNum == 0 || pImageItem->sCount == 0)
		memset(pImageItem, 0, sizeof(pImageItem));

	pImageItem->sCount--;
	SendStackChange(pImageItem->nNum, pImageItem->sCount, pImageItem->sDuration, IncomingPosID[1], false);
	if (pImageItem->sCount <= 0)
		memset(pImageItem, 0x00, sizeof(pImageItem));

	_ITEM_DATA* pKaulItem = &m_sItemArray[SLOT_MAX + IncomingPosID[0]];
	if (pKaulItem == nullptr)
	{
		Packet x(WIZ_ITEM_UPGRADE, uint8(PET_IMAGE_TRANSFORM));
		x << uint8(0) << uint8(1);
		Send(&x);
		return;
	}

	pKaulItem->nNum = pExchange->nReplaceItem;

	Guard Lock(g_pMain->m_PetSystemlock);
	PetDataSystemArray::iterator itr3 = g_pMain->m_PetDataSystemArray.find(pKaulItem->nSerialNum);
	if (itr3 == g_pMain->m_PetDataSystemArray.end())
	{
		Packet x(WIZ_ITEM_UPGRADE, uint8(PET_IMAGE_TRANSFORM));
		x << uint8(0) << uint8(1);
		Send(&x);
		return;
	}

	PET_DATA* pPet = itr3->second;
	pPet->sPid = pExchange->sReplaceSpid;
	pPet->sSize = pExchange->sReplaceSize;

	Packet result(WIZ_ITEM_UPGRADE, uint8(PET_HATCHING_TRANSFROM));
	result << uint8(1) << pKaulItem->nNum << IncomingPosID[0] << pPet->nIndex << pPet->strPetName << uint8(101) << pPet->bLevel
		<< (uint16)pPet->nExp << pPet->sSatisfaction;
	for (int i = 0; i < 3; i++)
		result << IncomingItemID[1 + i] << IncomingPosID[1 + i];
	Send(&result);
}

void CUser::HactchingTransformExchange(Packet& pkt)
{
	enum results
	{
		Failed = 1,
		InvalidName = 2,
		Familiarcannot = 3,
		LimitExceeded = 4,
	};

	uint16 sNpcID; int32 nItemID; int8 bPos; std::string strKaulName;
	_ITEM_DATA* pItem = nullptr; _ITEM_TABLE* pTable = nullptr;
	PET_INFO* pPetInfo = nullptr;
	Packet result(WIZ_ITEM_UPGRADE, uint8(PET_HATCHING_TRANSFROM));

	if (isTrading()
		|| isMerchanting()
		|| isMining()
		|| !isInGame()
		|| isFishing())
	{
		result << uint8(0) << uint8(Failed);
		Send(&result);
		return;
	}

	pkt.DByte();
	pkt >> sNpcID >> nItemID >> bPos >> strKaulName;

	if (strKaulName.length() == 0 || strKaulName.length() > 15)
	{
		result << uint8(0) << uint8(InvalidName);
		Send(&result);
		return;
	}

	if (bPos != -1 && bPos >= HAVE_MAX)
	{
		result << uint8(0) << uint8(Failed);
		Send(&result);
		return;
	}

	pTable = g_pMain->m_ItemtableArray.GetData(PET_START_ITEM);
	if (pTable == nullptr)
	{
		result << uint8(0) << uint8(Failed);
		Send(&result);
		return;
	}

	pItem = &m_sItemArray[SLOT_MAX + bPos];
	if (pItem == nullptr
		|| pItem->nNum != nItemID
		|| pItem->isBound()
		|| pItem->isDuplicate()
		|| pItem->isRented()
		|| pItem->isSealed())
	{
		result << uint8(0) << uint8(Failed);
		Send(&result);
		return;
	}

	pPetInfo = g_pMain->m_PetInfoSystemArray.GetData(PET_START_LEVEL);
	if (pPetInfo == nullptr)
	{
		result << uint8(0) << uint8(Failed);
		Send(&result);
		return;
	}

	uint64 nSerialID = g_pMain->GenerateItemSerial();
	uint32 nStatus = g_DBAgent.CreateNewPet(nSerialID, PET_START_LEVEL, strKaulName);
	if (nStatus < 10)
	{
		result << uint8(nStatus)
			<< pTable->m_iNum
			<< bPos << uint32(0)
			<< strKaulName
			<< uint8(pTable->m_sDamage)
			<< uint8(1)
			<< uint16(0)
			<< uint16(9000);
		Send(&result);
		return;
	}

	memset(pItem, 0x00, sizeof(_ITEM_DATA));
	pItem->nNum = pTable->m_iNum;
	pItem->nSerialNum = nSerialID;
	pItem->sCount = 1;
	pItem->sDuration = pTable->m_sDuration;

	PET_DATA* pPet = new PET_DATA();
	pPet->bLevel = PET_START_LEVEL;
	pPet->nIndex = (uint32)nStatus;
	pPet->sHP = pPetInfo->PetMaxHP;
	pPet->sMP = pPetInfo->PetMaxMP;
	pPet->sSatisfaction = 9000;
	pPet->nExp = 0;
	pPet->strPetName = strKaulName;
	memset(pPet->sItem, 0x00, sizeof(pPet->sItem));
	pPet->info = pPetInfo;

	Guard Lock(g_pMain->m_PetSystemlock);
	g_pMain->m_PetDataSystemArray.insert(std::make_pair(pItem->nSerialNum, pPet));

	result << uint8(1)
		<< pTable->m_iNum
		<< bPos
		<< pPet->nIndex
		<< strKaulName
		<< uint8(pTable->m_sDamage)
		<< uint8(1)
		<< uint16(0)
		<< pPet->sSatisfaction;
	Send(&result);
}
void CUser::SendPetHP(int tid, int damage)
{
	if (m_PettingOn == nullptr)
		return;

	Packet result(WIZ_PET);
	result << uint8(1) << uint8(8) << int16(tid) << uint8(0)
		<< uint8(7) << uint8(0) << uint8(0) << uint8(0)
		<< uint8(4) << uint8(0) << uint8(0) << uint8(0)
		<< int16(damage); //kaç damage vurdu
	Send(&result);
}

void CUser::SendPetHpChange(int tid, int damage)
{
	if (m_PettingOn == nullptr)
		return;

	Packet result(WIZ_PET);
	CNpc* pPet = g_pMain->GetPetPtr(GetSocketID(), GetZoneID());

	if (pPet == nullptr)
		return;

	if (damage < 0 && -damage > pPet->m_iHP)
		pPet->m_iHP = 0;
	else if (damage >= 0 && pPet->m_iHP + damage > (int32)pPet->m_MaxHP)
		pPet->m_iHP = pPet->m_MaxHP;
	else
		pPet->m_iHP += damage;

	result << uint8(1) << uint8(7)
		<< uint16(pPet->GetMaxHealth())
		<< uint16(pPet->GetHealth())
		<< uint16(tid);
	Send(&result);
}

void CUser::PetHome(uint16 x, uint16 y, uint16 z)
{
	if (m_PettingOn == nullptr)
		return;

	CNpc* pPet = g_pMain->GetPetPtr(GetSocketID(), GetZoneID());
	if (pPet == nullptr)
		return;

	float real_x = x / 10.0f, real_z = z / 10.0f;
	if (!pPet->GetMap()->IsValidPosition(real_x, real_z, 0.0f))
	{
		TRACE("Invalid position %f,%f\n", real_x, real_z);
		return;
	}

	m_PettingOn->sAttackStart = false;
	m_PettingOn->sAttackTargetID = -1;
	pPet->SendInOut(INOUT_OUT, pPet->GetX(), pPet->GetZ(), pPet->GetY());
	pPet->SetPosition(GetX(), GetY(), GetZ());
	pPet->RegisterRegion();
	pPet->SendInOut(INOUT_IN, pPet->GetX(), pPet->GetZ(), pPet->GetY());
}

void CUser::SendPetExpChange(int32 iExp, int tid/* = -1*/)
{
	if (m_PettingOn == nullptr)
		return;

	CNpc* pNpc = g_pMain->GetNpcPtr(tid, GetZoneID());

	if (pNpc == nullptr)
		return;

	if (!pNpc->isPet())
		return;

	if (m_PettingOn->nExp + iExp >= m_PettingOn->info->PetExp)
	{
		if (m_PettingOn->bLevel >= 60)
			return;

		m_PettingOn->bLevel++;
		int dExp = iExp;
		int diff = m_PettingOn->info->PetExp - m_PettingOn->nExp;
		dExp -= diff;
		m_PettingOn->nExp = dExp;

		Packet result(WIZ_PET);
		result << uint8(1) << uint8(11) << pNpc->GetID();
		SendToRegion(&result);

		pNpc->m_bLevel = m_PettingOn->bLevel;
		PET_INFO* pPetInfo = g_pMain->m_PetInfoSystemArray.GetData(pNpc->m_bLevel);
		if (pPetInfo)
			m_PettingOn->info = pPetInfo;

		SetPetInfo(m_PettingOn, pNpc->GetID());

		pNpc->HpChange(m_PettingOn->info->PetMaxHP);
		pNpc->MSpChange(m_PettingOn->info->PetMaxMP);

		PetSpawnProcess(true);
	}
	else
		m_PettingOn->nExp += iExp;

	uint16 percent = uint16((float(m_PettingOn->nExp) * 100.0f) / float(m_PettingOn->info->PetExp) * 100.0f);

	Packet result(WIZ_PET);
	result << uint8(1)
		<< uint8(10)
		<< uint64(iExp) //gained exp
		<< percent //exp percent
		<< m_PettingOn->bLevel
		<< m_PettingOn->sSatisfaction; //satisfaction percent
	Send(&result);
}

void CUser::SetPetInfo(PET_DATA* pPet, int tid /*= -1*/)
{
	if (pPet == nullptr)
		return;

	CNpc* pNpc = g_pMain->GetNpcPtr(tid, GetZoneID());

	if (pNpc == nullptr)
		return;

	if (!pNpc->isPet())
		return;

	pNpc->m_iHP = pPet->sHP;
	pNpc->m_MaxHP = pPet->info->PetMaxHP;
	pNpc->m_sMP = pPet->sMP;
	pNpc->m_MaxMP = pPet->info->PetMaxMP;
	pNpc->m_bLevel = pPet->bLevel;
	pNpc->m_sAttack = pPet->info->PetAttack;
	pNpc->m_sTotalAc = pPet->info->PetDefense;
	pNpc->m_sTotalAcTemp = pPet->info->PetDefense;
	pNpc->m_fTotalHitrate = pPet->info->PetRes;
	pNpc->m_fTotalEvasionrate = pPet->info->PetRes;
	pNpc->m_sTotalHit = pPet->info->PetAttack;
}

void CUser::LootingPet(float x, float z)
{
	if (m_PettingOn == nullptr)
		return;

	if (m_PettingOn->sStateChange != MODE_LOOTING)
		return;

	CNpc* pPet = g_pMain->GetPetPtr(GetSocketID(), GetZoneID());
	if (pPet == nullptr)
		return;

	float	warp_x = x - pPet->GetX(),
		warp_z = z - pPet->GetZ();

	// Unable to work out orientation, so we'll just fail (won't be necessary with m_sDirection).
	float	distance = sqrtf(warp_x * warp_x + warp_z * warp_z);
	if (distance == 0.0f)
		return;

	warp_x /= distance; warp_z /= distance;
	warp_x *= 2; warp_z *= 2;
	warp_x += x; warp_z += z;

	pPet->SendMoveResult(warp_x, 0, warp_z, distance);
}

void CUser::SendPetMSpChange(int tid, int damage)
{
	if (m_PettingOn == nullptr)
		return;

	Packet result(WIZ_PET);
	CNpc* pPet = g_pMain->GetPetPtr(GetSocketID(), GetZoneID());

	if (pPet == nullptr)
		return;

	if (damage < 0 && -damage > pPet->m_sMP)
		pPet->m_sMP = 0;
	else if (damage >= 0 && pPet->m_sMP + damage > (uint16)pPet->m_MaxMP)
		pPet->m_sMP = pPet->m_MaxMP;
	else
		pPet->m_sMP += damage;

	result << uint8(1)
		<< uint8(13)
		<< uint16(pPet->GetMaxMana())
		<< uint16(pPet->GetMana())
		<< uint16(tid);
	Send(&result);
}