#include "stdafx.h"

uint8 CUser::GetUserDailyOp(uint8 type)
{
	if (type == 0)
		return 0;

	int32 nUnixTime = -1;

	UserDailyOpMap::iterator itr = g_pMain->m_UserDailyOpMap.find(GetName());

	if (itr != g_pMain->m_UserDailyOpMap.end())
	{
		if (type == DAILY_CHAOS_MAP)
			nUnixTime = int(itr->second->ChaosMapTime);
		else if (type == DAILY_USER_RANK_REWARD)
			nUnixTime = int(itr->second->UserRankRewardTime);
		else if (type == DAILY_USER_PERSONAL_RANK_REWARD)
			nUnixTime = int(itr->second->PersonalRankRewardTime);
		else if (type == DAILY_KING_WING)
			nUnixTime = int(itr->second->KingWingTime);
		else if (type == DAILY_WARDER_KILLER_WING1)
			nUnixTime = int(itr->second->WarderKillerTime1);
		else if (type == DAILY_WARDER_KILLER_WING2)
			nUnixTime = int(itr->second->WarderKillerTime2);
		else if (type == DAILY_KEEPER_KILLER_WING)
			nUnixTime = int(itr->second->KeeperKillerTime);
		else if (type == DAILY_USER_LOYALTY_WING_REWARD)
			nUnixTime = int(itr->second->UserLoyaltyWingRewardTime);

		if (nUnixTime == -1)
			SetUserDailyOp(type);
		else
		{
			if (((int32(UNIXTIME) - nUnixTime) / 60) > DAILY_OPERATIONS_MINUTE)
				SetUserDailyOp(type);
			else
				return 0;
		}
	}
	else
		SetUserDailyOp(type, true);

	return 1;
}

void CUser::SetUserDailyOp(uint8 type, bool isInsert)
{
	enum UserDailyOpDB
	{
		UPDATE = 1,
		INSERT = 2,
	};

	if (type == 0)
		return;

	int32 nUnixTime = int32(UNIXTIME);

	if (isInsert)
	{
		_USER_DAILY_OP * pData = new _USER_DAILY_OP;

		pData->strUserId = GetName();
		pData->ChaosMapTime = -1;
		pData->UserRankRewardTime = -1;
		pData->PersonalRankRewardTime = -1;
		pData->KingWingTime = -1;
		pData->WarderKillerTime1 = -1;
		pData->WarderKillerTime2 = -1;
		pData->KeeperKillerTime = -1;
		pData->UserLoyaltyWingRewardTime = -1;

		if (type == DAILY_CHAOS_MAP)
			pData->ChaosMapTime = nUnixTime;
		else if (type == DAILY_USER_RANK_REWARD)
			pData->UserRankRewardTime = nUnixTime;
		else if (type == DAILY_USER_PERSONAL_RANK_REWARD)
			pData->PersonalRankRewardTime = nUnixTime;
		else if (type == DAILY_KING_WING)
			pData->KingWingTime = nUnixTime;
		else if (type == DAILY_WARDER_KILLER_WING1)
			pData->WarderKillerTime1 = nUnixTime;
		else if (type == DAILY_WARDER_KILLER_WING2)
			pData->WarderKillerTime2 = nUnixTime;
		else if (type == DAILY_KEEPER_KILLER_WING)
			pData->KeeperKillerTime = nUnixTime;
		else if (type == DAILY_USER_LOYALTY_WING_REWARD)
			pData->UserLoyaltyWingRewardTime = nUnixTime;

		g_pMain->m_UserDailyOpMap.insert(make_pair(pData->strUserId, pData));
		Packet pkt(WIZ_DB_DAILY_OP, uint8(INSERT));
		pkt << pData->strUserId;
		pkt << pData->ChaosMapTime;
		pkt << pData->UserRankRewardTime;
		pkt << pData->PersonalRankRewardTime;
		pkt << pData->KingWingTime;
		pkt << pData->WarderKillerTime1;
		pkt << pData->WarderKillerTime2;
		pkt << pData->KeeperKillerTime;
		pkt << pData->UserLoyaltyWingRewardTime;
		g_pMain->AddDatabaseRequest(pkt, this);
	}
	else
	{
		UserDailyOpMap::iterator itr = g_pMain->m_UserDailyOpMap.find(GetName());
		if (itr != g_pMain->m_UserDailyOpMap.end())
		{
			if (type == DAILY_CHAOS_MAP)
				itr->second->ChaosMapTime = nUnixTime;
			else if (type == DAILY_USER_RANK_REWARD)
				itr->second->UserRankRewardTime = nUnixTime;
			else if (type == DAILY_USER_PERSONAL_RANK_REWARD)
				itr->second->PersonalRankRewardTime = nUnixTime;
			else if (type == DAILY_KING_WING)
				itr->second->KingWingTime = nUnixTime;
			else if (type == DAILY_WARDER_KILLER_WING1)
				itr->second->WarderKillerTime1 = nUnixTime;
			else if (type == DAILY_WARDER_KILLER_WING2)
				itr->second->WarderKillerTime2 = nUnixTime;
			else if (type == DAILY_KEEPER_KILLER_WING)
				itr->second->KeeperKillerTime = nUnixTime;
			else if (type == DAILY_USER_LOYALTY_WING_REWARD)
				itr->second->UserLoyaltyWingRewardTime = nUnixTime;

			Packet pkt(WIZ_DB_DAILY_OP, uint8(UPDATE));
			pkt << GetName() << type << int32(UNIXTIME);
			g_pMain->AddDatabaseRequest(pkt, this);
		}
	}
}