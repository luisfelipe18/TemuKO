#include "stdafx.h"
#include "DBAgent.h"

void CUser::HandlePremium(Packet &pkt)
{
	uint8 opcode, pType;
	uint32 bResult = 0;
	pkt >> opcode >> pType;

	if (m_bPremiumInUse == pType)
		return;

	Packet result(WIZ_PREMIUM, uint8(opcode));
	Packet pktdb(WIZ_PREMIUM);
	_PREMIUM_DATA* pPremium = m_PremiumMap.GetData(pType);
	if (pPremium == nullptr
		|| pPremium->iPremiumTime < UNIXTIME)
		goto fail_return;

	m_bPremiumInUse = pType;
	bResult = 1;


	g_pMain->AddDatabaseRequest(pktdb, this);
	SendPremiumInfo();

	if (m_FlashExpBonus > 0
		|| m_FlashDcBonus > 0
		|| m_FlashWarBonus > 0)
	{
		SendFlashDCNotice(true);
		SendFlashEXPNotice(true);
		SendFlashCountNotice(true);

		if (m_FlashExpBonus > 0)
			m_FlashExpBonus = 0;

		if (m_FlashDcBonus > 0)
			m_FlashDcBonus = 0;

		if (m_FlashWarBonus > 0)
			m_FlashWarBonus = 0;

		CMagicProcess::RemoveType4Buff(BUFF_TYPE_FISHING, this, true);
	}
	return;
fail_return:
	result << m_bPremiumInUse
		<< bResult;
	Send(&result);
}
/**
* @brief	Sends the user's premium state.
*/
void CUser::SendPremiumInfo()
{
	Packet result(WIZ_PREMIUM, uint8(1));
	result << uint8(m_PremiumMap.GetSize());  // Premium count not expired
	int counter = 0;
	foreach_stlmap(itr, m_PremiumMap)
	{
		_PREMIUM_DATA * uPrem = itr->second;
		if (uPrem == nullptr
			|| uPrem->iPremiumTime == 0)
			continue;

		uint32 TimeRest = 0;
		uint16 TimeShow = 0;
		TimeRest = uint32(uPrem->iPremiumTime - UNIXTIME);
		if (TimeRest >= 1 && TimeRest <= 3600)
			TimeShow = 1;
		else
			TimeShow = TimeRest / 3600;

		result << uPrem->bPremiumType
			<< TimeShow;
		counter++;

		if (m_bPremiumInUse == NO_PREMIUM)
			m_bPremiumInUse = uPrem->bPremiumType;
	}

	result << m_bPremiumInUse
		<< uint32(0); // Unknown
	Send(&result);
}

/**
* @brief Gives user the premium bonus items.
*/
void CUser::GivePremiumItem(uint8 bPremiumType)
{
	Packet result(WIZ_SHOPPING_MALL, uint8(STORE_LETTER));
	result << uint8(LETTER_GIVE_PRE_ITEM) << bPremiumType;
	g_pMain->AddDatabaseRequest(result, this);
}

/**
* @brief	Get premium properties.
*/
uint16 CUser::GetPremiumProperty(PremiumPropertyOpCodes type)
{
	if (GetPremium() <= 0)
		return 0;

	_PREMIUM_ITEM * pPremiumItem = g_pMain->m_PremiumItemArray.GetData(GetPremium());
	if (pPremiumItem == nullptr)
		return 0;

	switch (type)
	{
		case PremiumExpRestorePercent:
			return pPremiumItem->ExpRestorePercent;
		case PremiumNoahPercent:
			return pPremiumItem->NoahPercent;
		case PremiumDropPercent:
			return pPremiumItem->DropPercent;
		case PremiumBonusLoyalty:
			return pPremiumItem->BonusLoyalty;
		case PremiumRepairDiscountPercent:
			return pPremiumItem->RepairDiscountPercent;
		case PremiumItemSellPercent:
			return pPremiumItem->ItemSellPercent;
		case PremiumExpPercent:
		{
			foreach_stlmap(itr, g_pMain->m_PremiumItemExpArray)
			{
				_PREMIUM_ITEM_EXP *pPremiumItemExp = g_pMain->m_PremiumItemExpArray.GetData(itr->first);

				if (pPremiumItemExp == nullptr)
					continue;

				if (GetPremium() == pPremiumItemExp->Type
					&& GetLevel() >= pPremiumItemExp->MinLevel 
					&& GetLevel() <= pPremiumItemExp->MaxLevel)
					return pPremiumItemExp->sPercent;
			}
		}
	}
	return 0;
}

void CUser::GivePremium(uint8 bPremiumType, uint16 sPremiumTime)
{
	if (bPremiumType <= 0
		|| bPremiumType > 13
		|| sPremiumTime <= 0
		|| m_PremiumMap.GetSize() > PREMIUM_TOTAL)
		return;

	_PREMIUM_DATA *pPremium = m_PremiumMap.GetData(bPremiumType);
	if (pPremium == nullptr)
	{
		pPremium = new _PREMIUM_DATA;
		pPremium->bPremiumType = bPremiumType;
		pPremium->iPremiumTime = uint32(UNIXTIME) + 24 * 60 * 60 * sPremiumTime;
		m_PremiumMap.PutData(bPremiumType, pPremium);
	}
	else
	{
		pPremium->bPremiumType = bPremiumType;
		pPremium->iPremiumTime = pPremium->iPremiumTime + 24 * 60 * 60 * sPremiumTime;
	}

	m_bPremiumInUse = bPremiumType;
	m_bAccountStatus = 1;

	Packet pkt(WIZ_PREMIUM);
	g_pMain->AddDatabaseRequest(pkt, this);
	SendPremiumInfo();
}

void CUser::GiveClanPremium(uint8 bPremiumType, uint16 sPremiumTime)
{
	if (!isInClan())
		return;

	CKnights* pKnights = g_pMain->GetClanPtr(GetClanID());

	if (pKnights == nullptr)
		return;

	if (!isClanLeader())
		return;

	if (pKnights->sPremiumTime < uint32(UNIXTIME))
		pKnights->sPremiumTime = uint32(UNIXTIME) + 24 * 60 * 60 * sPremiumTime;
	else
		pKnights->sPremiumTime = pKnights->sPremiumTime + 24 * 60 * 60 * sPremiumTime;

	pKnights->sPremiumInUse = PremiumTypes::Clan_Premium;

	foreach_stlmap(i, pKnights->m_arKnightsUser)
	{
		_KNIGHTS_USER* p = i->second;

		if (p == nullptr)
			continue;

		CUser* pUser = g_pMain->GetUserPtr(p->strUserName, TYPE_CHARACTER);
		if (pUser == nullptr
			|| !pUser->isInGame())
			continue;

		pUser->m_bClanPremiumInUse = 13;
	}

	Packet result(WIZ_CLAN_PREMIUM);
	result << uint8(pKnights->sPremiumInUse);
	g_pMain->AddDatabaseRequest(result, this);

	result.clear();
	result.Initialize(WIZ_PREMIUM);
	result << pKnights->sPremiumInUse << uint8(1);
	result << pKnights->sPremiumInUse << uint16(pKnights->sPremiumTime);
	result << pKnights->sPremiumInUse << uint32(pKnights->sPremiumTime);
	pKnights->Send(&result);
}

/**
* @brief	Get premium properties.
*/
uint16 CUser::GetClanPremiumProperty(PremiumPropertyOpCodes type)
{
	if (GetClanPremium() <= 0)
		return 0;

	_PREMIUM_ITEM* pPremiumItem = g_pMain->m_PremiumItemArray.GetData(GetClanPremium());
	if (pPremiumItem == nullptr)
		return 0;

	switch (type)
	{
	case PremiumExpRestorePercent:
		return pPremiumItem->ExpRestorePercent;
	case PremiumNoahPercent:
		return pPremiumItem->NoahPercent;
	case PremiumDropPercent:
		return pPremiumItem->DropPercent;
	case PremiumBonusLoyalty:
		return pPremiumItem->BonusLoyalty;
	case PremiumRepairDiscountPercent:
		return pPremiumItem->RepairDiscountPercent;
	case PremiumItemSellPercent:
		return pPremiumItem->ItemSellPercent;
	case PremiumExpPercent:
	{
		foreach_stlmap(itr, g_pMain->m_PremiumItemExpArray)
		{
			_PREMIUM_ITEM_EXP* pPremiumItemExp = g_pMain->m_PremiumItemExpArray.GetData(itr->first);

			if (pPremiumItemExp == nullptr)
				continue;

			if (GetClanPremium() == pPremiumItemExp->Type
				&& GetLevel() >= pPremiumItemExp->MinLevel
				&& GetLevel() <= pPremiumItemExp->MaxLevel)
				return pPremiumItemExp->sPercent;
		}
	}
	}
	return 0;
}

void CUser::SendClanPremiumInfo()
{
	uint32	TimeRest = 0;
	uint16	TimeShow = 0;
	uint8	sPremiumInUse = 0;
	uint32	sPremiumTime = 0;
	bool	sClanPremStatus = false;
	m_bClanPremiumInUse = NO_CLAN;

	CKnights* pKnights = g_pMain->GetClanPtr(GetClanID());
	if (pKnights == nullptr) {
		goto fail_return;
		return;
	}

	if (pKnights->sPremiumTime > 0) {
		TimeRest = uint32(pKnights->sPremiumTime - UNIXTIME);

		if (TimeRest >= 1 && TimeRest <= 3600)
			TimeShow = 1;
		else
			TimeShow = TimeRest / 3600;

		sPremiumInUse = pKnights->sPremiumInUse;
		sPremiumTime = pKnights->sPremiumTime;
		m_bClanPremiumInUse = 13;
		sClanPremStatus = true;
	}

	if (pKnights->sPremiumTime < uint32(UNIXTIME))
	{
		sPremiumInUse = NO_PREMIUM;
		sPremiumTime = NO_PREMIUM;
		m_bClanPremiumInUse = NO_PREMIUM;
		sClanPremStatus = false;
	}

fail_return:
	Packet result(WIZ_PREMIUM, uint8(sClanPremStatus == true ? PremiumTypes::Clan_Premium : NO_PREMIUM));
	result << uint8(sClanPremStatus) << sPremiumInUse << TimeShow << sPremiumInUse << sPremiumTime;
	Send(&result);
}