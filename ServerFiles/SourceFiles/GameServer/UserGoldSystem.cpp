#include "stdafx.h"

void CUser::GoldChange(short tid, int gold)
{
	if (GetZoneID() == ZONE_SNOW_BATTLE
		|| GetZoneID() == ZONE_DESPERATION_ABYSS
		|| GetZoneID() == ZONE_HELL_ABYSS
		|| GetZoneID() == ZONE_DRAGON_CAVE
		|| GetZoneID() == ZONE_CAITHAROS_ARENA
		|| GetZoneID() == ZONE_ISILOON_ARENA
		|| GetZoneID() == ZONE_FELANKOR_ARENA)
		return;

	CUser* pTUser = g_pMain->GetUserPtr(tid);
	if (pTUser == nullptr || pTUser->m_iGold <= 0)
		return;

	// Reward money in war zone
	if (gold == 0)
	{
		// If we're not in a party, we can distribute cleanly.
		if (!isInParty())
		{
			GoldGain((pTUser->m_iGold * 4) / 10);
			pTUser->GoldLose(pTUser->m_iGold / 2);
			return;
		}

		// Otherwise, if we're in a party, we need to divide it up.
		_PARTY_GROUP* pParty = g_pMain->GetPartyPtr(GetPartyID());
		if (pParty == nullptr)
			return;

		int userCount = 0, levelSum = 0, temp_gold = (pTUser->m_iGold * 4) / 10;
		pTUser->GoldLose(pTUser->m_iGold / 2);

		// TODO: Clean up the party system. 
		for (int i = 0; i < MAX_PARTY_USERS; i++)
		{
			CUser *pUser = g_pMain->GetUserPtr(pParty->uid[i]);
			if (pUser == nullptr)
				continue;

			userCount++;
			levelSum += pUser->GetLevel();
		}

		// No users (this should never happen! Needs to be cleaned up...), don't bother with the below loop.
		if (userCount == 0)
			return;

		for (int i = 0; i < MAX_PARTY_USERS; i++)
		{
			CUser * pUser = g_pMain->GetUserPtr(pParty->uid[i]);
			if (pUser == nullptr)
				continue;

			pUser->GoldGain((int)(temp_gold * (float)(pUser->GetLevel() / (float)levelSum)));
		}
		return;
	}

	// Otherwise, use the coin amount provided.

	// Source gains money
	if (gold > 0)
	{
		GoldGain(gold);
		pTUser->GoldLose(gold);
	}
	// Source loses money
	else
	{
		GoldLose(gold);
		pTUser->GoldGain(gold);
	}
}

void CUser::GoldGain(uint32 gold, bool bSendPacket /*= true*/, bool bApplyBonus /*= false*/)
{
	// Assuming it works like this, although this affects (probably) all gold gained (including kills in PvP zones)
	// If this is wrong and it should ONLY affect gold gained from monsters, let us know!
	if (bApplyBonus)
		gold = gold * (m_bNoahGainAmount + m_bItemNoahGainAmount) / 100;

	if (m_iGold + gold > COIN_MAX)
		m_iGold = COIN_MAX;
	else
		m_iGold += gold;

	if (bSendPacket)
	{
		Packet result(WIZ_GOLD_CHANGE, uint8(CoinGain));
		result << gold << GetCoins();
		Send(&result);
	}
}

void CUser::LuaGoldGain(uint32 gold, bool bSendPacket /*= true*/)
{
	Packet result;
	if (m_iGold + gold > COIN_MAX)
	{
		result.Initialize(WIZ_QUEST);
		result << uint8(13) << uint8(2);
		Send(&result);
		return;
	}
	else
		m_iGold += gold;

	if (bSendPacket)
	{
		result.Initialize(WIZ_GOLD_CHANGE);
		result << uint8(CoinGain) << gold << GetCoins();
		Send(&result);
	}
}

bool CUser::GoldLose(uint32 gold, bool bSendPacket /*= true*/)
{
	if (!hasCoins(gold))
		return false;

	m_iGold -= gold;

	if (bSendPacket)
	{
		Packet result(WIZ_GOLD_CHANGE, uint8(CoinLoss));
		result << gold << GetCoins();
		Send(&result);
	}
	return true;
}