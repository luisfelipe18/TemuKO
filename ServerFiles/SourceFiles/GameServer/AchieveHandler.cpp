#include "stdafx.h"
#include "DBAgent.h"

#pragma region CUser::HandleUserAchieve(Packet & pkt)
/*
* @brief	Handles all the user achieve system related packets.
*
* @param	pkt	The packet.
*/
void CUser::HandleUserAchieve(Packet & pkt)
{
	uint8 opcode;
	pkt >> opcode;

	switch (opcode)
	{
	case AchieveGiftRequest:
		HandleUserAchieveGiftRequest(pkt);
		break;
	case AchieveDetailShow:
		HandleUserAchieveUserDetail(pkt);
		break;
	case AchieveSummary:
		HandleUserAchieveSummary(pkt);
		break;
	case AchieveStart:
		HandleUserAchieveStart(pkt);
		break;
	case AchieveStop:
		HandleUserAchieveStop(pkt);
		break;
	case AchieveCoverTitle:
		HandleUserAchieveCoverTitle(pkt);
		break;
	case AchieveSkillTitle:
		HandleUserAchieveSkillTitle(pkt);
		break;
	case AchieveCoverTitleReset:
		HandleUserAchieveCoverTitleReset(pkt);
		break;
	case AchieveSkillTitleReset:
		HandleUserAchieveSkillTitleReset(pkt);
		break;
	}
}
#pragma endregion

#pragma region CUser::HandleUserAchieveSummary(Packet &pkt)
/*
* @brief	Sends the user summary screen information.
*
* @param	pkt	The packet.
*/
void CUser::HandleUserAchieveSummary(Packet &pkt)
{
	Packet result(WIZ_USER_ACHIEVE, uint8(AchieveSummary));
	result << uint32((uint32(UNIXTIME) - m_AchieveInfo.PlayTime) / 60)
		<< m_AchieveInfo.MonsterDefeatCount
		<< m_AchieveInfo.UserDefeatCount
		<< m_AchieveInfo.UserDeathCount
		<< m_AchieveInfo.TotalMedal;

	for (int i = 0; i < 3; i++)
	{
		result << m_AchieveInfo.RecentAchieve[i];
	}

	result << m_AchieveInfo.NormalCount
		<< m_AchieveInfo.QuestCount
		<< m_AchieveInfo.WarCount
		<< m_AchieveInfo.AdvantureCount
		<< m_AchieveInfo.ChallangeCount;

	Send(&result);
}
#pragma endregion

#pragma region CUser::HandleUserAchieveStart(Packet &pkt)
/*
* @brief	Starts an achievement that required completion in a predefined period.
*
* @param	pkt	The packet.
*/
void CUser::HandleUserAchieveStart(Packet &pkt)
{
	uint16 sIndex = pkt.read<uint16>();

	Packet result;

	_ACHIEVE_MAIN* pAchieveMain = g_pMain->m_AchieveMainArray.GetData(sIndex);
	if (pAchieveMain == nullptr)
	{
		result.Initialize(WIZ_USER_ACHIEVE);
		result << uint8(AchieveStart) << uint16(0) << uint16(-2);
		Send(&result);
		return;
	}

	_USER_ACHIEVE_INFO* pAchieveInfo = m_AchieveMap.GetData(sIndex);
	if (pAchieveInfo == nullptr)
	{
		result.clear();
		result.Initialize(WIZ_USER_ACHIEVE);
		result << uint8(AchieveStart) << uint16(0) << uint16(-2);
		Send(&result);
		return;
	}

	if (pAchieveMain->AchieveType == 4 
		&& (pAchieveMain->sIndex == 458 
			|| pAchieveMain->sIndex == 459))
	{
			result.clear();
			result.Initialize(WIZ_USER_ACHIEVE);
			result << uint8(AchieveChalengeFailed) << uint16(sIndex) << uint16(1);
			Send(&result);
			return;
	}

	_USER_ACHIEVE_TIMED_INFO* pAchieveTimedInfo = m_AchieveTimedMap.GetData(sIndex);
	if (pAchieveTimedInfo != nullptr 
		|| AchieveChalengeCount == true)
	{
		result.clear();
		result.Initialize(WIZ_USER_ACHIEVE);
		result << uint8(AchieveStart) << uint16(0) << uint16(-1);
		Send(&result);
		return;
	}

	uint32 expiration = uint32(UNIXTIME) + pAchieveMain->ReqTime;
	AchieveChalengeCount = true;
	pAchieveTimedInfo = new _USER_ACHIEVE_TIMED_INFO();
	pAchieveTimedInfo->iExpirationTime = expiration;	
	pAchieveInfo->bStatus = UserAchieveStatus::AchieveIncomplete;
	m_AchieveTimedMap.PutData(sIndex, pAchieveTimedInfo);

	result.clear();
	result.Initialize(WIZ_USER_ACHIEVE);
	result << uint8(AchieveStart) << uint16(sIndex) << uint16(1) << uint16(pAchieveMain->ReqTime);
	Send(&result);

	result.clear();
	result.Initialize(WIZ_USER_ACHIEVE);
	result << uint8(AchieveCountScreen) << uint16(sIndex) << uint8(1) << uint16(pAchieveInfo->iCount[0]) << uint16(pAchieveInfo->iCount[1]);
	Send(&result);
}
#pragma endregion

#pragma region CUser::HandleUserAchieveStop(Packet &pkt)
/*
* @brief	Stops an achievement that required completion in a predefined period.
*
* @param	pkt	The packet.
*/
void CUser::HandleUserAchieveStop(Packet &pkt)
{
	uint16 sIndex = pkt.read<uint16>();

	Packet result;

	_ACHIEVE_MAIN* pAchieveMain = g_pMain->m_AchieveMainArray.GetData(sIndex);
	if (pAchieveMain == nullptr)
	{
		result.Initialize(WIZ_USER_ACHIEVE);
		result << uint8(7) << uint16(0) << uint16(-2);
		Send(&result);
		return;
	}

	_USER_ACHIEVE_INFO* pAchieveInfo = m_AchieveMap.GetData(sIndex);
	if (pAchieveInfo == nullptr)
	{
		result.clear();
		result.Initialize(WIZ_USER_ACHIEVE);
		result << uint8(7) << uint16(0) << uint16(-2);
		Send(&result);
		return;
	}

	_USER_ACHIEVE_TIMED_INFO* pTimedInfo = m_AchieveTimedMap.GetData(sIndex);
	if (pTimedInfo == nullptr 
		|| AchieveChalengeCount == false)
	{
		result.clear();
		result.Initialize(WIZ_USER_ACHIEVE);
		result << uint8(7) << uint16(0) << uint16(-1);
		Send(&result);
		return;
	}

	m_AchieveTimedMap.DeleteData(sIndex);
	pAchieveInfo->bStatus = UserAchieveStatus::AchieveChalenngeInComplete;
	pAchieveInfo->iCount[0] = 0;
	pAchieveInfo->iCount[1] = 0;
	AchieveChalengeCount = false;

	result.clear();
	result.Initialize(WIZ_USER_ACHIEVE);
	result << uint8(7) << uint16(sIndex) << uint16(1);
	Send(&result);
}
#pragma endregion

#pragma region CUser::HandleUserAchieveGiftRequest(Packet & pkt)
/**
* @brief	Shows user the achieve details when the tab.
*			in the user detail (U) chosen.
*
* bStatus = 1: Unfinished 04: Finished 05: Complete(items taken)
*/
void CUser::HandleUserAchieveGiftRequest(Packet & pkt)
{
	uint16 sIndex = 0;
	pkt >> sIndex;
	bool bResult = false;

	// check whether we completed the achievement to get the item
	if (!CheckAchieveStatus(sIndex, AchieveFinished))
		goto fail_return;

	auto itr = g_pMain->m_AchieveMainArray.GetData(sIndex);

	// Is the achievement a valid one?
	if (itr == nullptr)
		goto fail_return;

	auto item = g_pMain->GetItemPtr(itr->ItemNum);

	// Make sure we have the item in our database.
	if (item == nullptr)
		goto fail_return;

	GiveItem(item->m_iNum, (uint16)itr->iCount);

	UpdateAchieve(sIndex, AchieveCompleted);
	bResult = true;
fail_return:
	Packet result(WIZ_USER_ACHIEVE, uint8(AchieveGiftRequest));
	result << sIndex << uint16(bResult);
	Send(&result);
}
#pragma endregion

#pragma region CUser::HandleUserAchieveUserDetail(Packet & pkt)
/**
* @brief	Shows user the achieve details when the tab.
*			in the user detail (U) chosen.
*
* bStatus = 1: Unfinished 04: Finished 05: Complete(items taken)
*/
void CUser::HandleUserAchieveUserDetail(Packet & pkt)
{
	uint16 sAchieveNum = 0;
	pkt >> sAchieveNum;

	TRACE("AchieveNum = %d", sAchieveNum);

	uint16 achievedata[10]; // temporarly 10 array size is assigned
	for (int i = 0; i< sAchieveNum; ++i)
	{
		pkt >> achievedata[i];
		TRACE("achievedata[i] = %d", achievedata[i]);
	}

	Packet result(WIZ_USER_ACHIEVE, uint8(AchieveDetailShow));
	result << sAchieveNum;

	for (int i = 0; i < sAchieveNum; i++)
	{
		_USER_ACHIEVE_INFO* pAchieveInfo = m_AchieveMap.GetData(achievedata[i]);
		if (pAchieveInfo == nullptr)
			result << achievedata[i] << uint8(1) << uint32(0);
		else
			result << achievedata[i] << pAchieveInfo->bStatus << uint32(pAchieveInfo->iCount[0]);
	}
	Send(&result);
}
#pragma endregion

#pragma region CUser::CheckAchieveStatus(uint16 sIndex, uint16 Status)
/*
* @brief	Checks if the specified achievement exists in user's achieve map with
*			the specified state.
*
* @param	sIndex	the ID of the achieve to be checked.
* @param	Status	the state of the achieve to be checked.
*/
bool CUser::CheckAchieveStatus(uint16 sIndex, uint16 Status)
{
	// Attempt to find a achieve with that ID in the map
	_USER_ACHIEVE_INFO* pAchieveInfo = m_AchieveMap.GetData(sIndex);

	// If it doesn't exist, it doesn't exist. 
	// Unless of course, we wanted it to not exist, in which case we're right!
	// (this does seem silly, but this behaviour is STILL expected, so do not remove it.)
	if (pAchieveInfo == nullptr)
		return Status == 1;

	return pAchieveInfo->bStatus == Status;
}
#pragma endregion

#pragma region CUser::UpdateAchieve(uint16 sIndex, uint8 Status)
/*
* @brief	Updates the specified achievement status.
*
* @param	sIndex	the ID of the achieve to be checked.
* @param	Status	the state of the achieve to be checked.
*/
void CUser::UpdateAchieve(uint16 sIndex, uint8 Status)
{
	// Attempt to find a achieve with that ID in the map
	_USER_ACHIEVE_INFO* pAchieveInfo = m_AchieveMap.GetData(sIndex);
	if (pAchieveInfo == nullptr)
		return;

	pAchieveInfo->bStatus = Status;
}
#pragma endregion

#pragma region CUser::AchieveMonsterCountAdd(uint16 sNpcID)
/*
* @brief	Checks if the specified NPC is a quest monster for this user.
*
* @param	sNpcID	the NPC ID to be checked for quests.
*/
void CUser::AchieveMonsterCountAdd(uint16 sNpcID)
{
	m_AchieveInfo.MonsterDefeatCount++;

	foreach_stlmap(itr, m_AchieveMap)
	{
		_USER_ACHIEVE_INFO* pAchieveData = itr->second;
		if (pAchieveData == nullptr)
			continue;

		_ACHIEVE_MAIN *pAchieveMain = g_pMain->m_AchieveMainArray.GetData(itr->first);
		if (pAchieveMain == nullptr
			|| (pAchieveMain->ZoneID > 0 && pAchieveMain->ZoneID != GetZoneID()))
			continue;

		// if the Achievement is completed or finished, why would we check for 'em?
		if (pAchieveData->bStatus == UserAchieveStatus::AchieveCompleted
			|| pAchieveData->bStatus == UserAchieveStatus::AchieveFinished)
			continue;

		switch (pAchieveMain->Type)
		{
		case AchieveWar:
		{
			_ACHIEVE_WAR *pAchieveWar = g_pMain->m_AchieveWarArray.GetData(pAchieveMain->sIndex);
			if (pAchieveWar == nullptr
				|| pAchieveWar->Type != AchieveDefeatMonsterRonarkL)
				continue;

			AchieveWarCheck(pAchieveData, pAchieveWar, pAchieveMain, sNpcID, nullptr);
		}
		break;
		case AchieveMonster:
		{
			_ACHIEVE_MONSTER *pAchieveMonster = g_pMain->m_AchieveMonsterArray.GetData(pAchieveMain->sIndex);
			if (pAchieveMonster == nullptr)
				continue;

			AchieveMonsterCheck(pAchieveData, pAchieveMonster, pAchieveMain, sNpcID);
		}
		break;
		case AchieveNormal:
		{
			_ACHIEVE_NORMAL *pAchieveNormal = g_pMain->m_AchieveNormalArray.GetData(pAchieveMain->sIndex);
			if (pAchieveNormal == nullptr
				|| pAchieveNormal->Type != AchieveDefeatMons)
				continue;

			AchieveNormalCheck(pAchieveData, pAchieveNormal, pAchieveMain, sNpcID);
		}
		break;
		default:
			break;
		}
	}
}
#pragma endregion

#pragma region CUser::AchieveWarCountAdd(UserAchieveWarTypes type, uint16 sNpcID /*= 0*/, CUser *pTarget /*= nullptr*/)
/*
* @brief	Checks if the any War achievement completed.
*
* @param	sNpcID	the NPC ID to be checked for quests.
*/
void CUser::AchieveWarCountAdd(UserAchieveWarTypes type, uint16 sNpcID /*= 0*/, CUser *pTarget /*= nullptr*/)
{
	foreach_stlmap(itr, m_AchieveMap)
	{
		_USER_ACHIEVE_INFO* pAchieveData = itr->second;
		if (pAchieveData == nullptr)
			continue;

		_ACHIEVE_MAIN *pAchieveMain = g_pMain->m_AchieveMainArray.GetData(itr->first);
		if (pAchieveMain == nullptr
			|| (pAchieveMain->ZoneID > 0 && pAchieveMain->ZoneID != GetZoneID())
			|| pAchieveMain->Type != AchieveWar)
			continue;

		// if the Achievement is completed or finished, why would we check for 'em?
		if (pAchieveData->bStatus == UserAchieveStatus::AchieveCompleted
			|| pAchieveData->bStatus == UserAchieveStatus::AchieveFinished)
			continue;

		switch (pAchieveMain->Type)
		{
		case AchieveWar:
		{
			_ACHIEVE_WAR *pAchieveWar = g_pMain->m_AchieveWarArray.GetData(pAchieveMain->sIndex);
			if (pAchieveWar == nullptr
				|| pAchieveWar->Type != type)
				continue;

			AchieveWarCheck(pAchieveData, pAchieveWar, pAchieveMain, sNpcID, pTarget);
		}
		break;
		default:
			break;
		}
	}
}
#pragma endregion

#pragma region CUser::AchieveNormalCountAdd(UserAchieveNormalTypes type, uint16 sNpcID /*= 0*/, CUser *pTarget /*= nullptr*/)
/*
* @brief	Checks if the any War achievement completed.
*
* @param	sNpcID	the NPC ID to be checked for quests.
*/
void CUser::AchieveNormalCountAdd(UserAchieveNormalTypes type, uint16 sNpcID /*= 0*/, CUser *pTarget /*= nullptr*/)
{
	foreach_stlmap(itr, m_AchieveMap)
	{
		_USER_ACHIEVE_INFO* pAchieveData = itr->second;
		if (pAchieveData == nullptr)
			continue;

		_ACHIEVE_MAIN *pAchieveMain = g_pMain->m_AchieveMainArray.GetData(itr->first);
		if (pAchieveMain == nullptr
			|| (pAchieveMain->ZoneID > 0 && pAchieveMain->ZoneID != GetZoneID())
			|| pAchieveMain->Type != AchieveNormal)
			continue;

		// if the Achievement is completed or finished, why would we check for 'em?
		if (pAchieveData->bStatus == UserAchieveStatus::AchieveCompleted
			|| pAchieveData->bStatus == UserAchieveStatus::AchieveFinished)
			continue;

		switch (pAchieveMain->Type)
		{
		case AchieveNormal:
		{
			_ACHIEVE_NORMAL *pAchieveNormal = g_pMain->m_AchieveNormalArray.GetData(pAchieveMain->sIndex);
			if (pAchieveNormal == nullptr
				|| pAchieveNormal->Type != type)
				continue;

			AchieveNormalCheck(pAchieveData, pAchieveNormal, pAchieveMain, sNpcID, pTarget);
		}
		break;
		default:
			break;
		}
	}
}
#pragma endregion

#pragma region CUser::AchieveComCountAdd(UserAchieveComTypes type, uint16 sNpcID/* = 0*/, CUser *pTarget/* = nullptr*/)
/*
* @brief	Checks if the any War achievement completed.
*
* @param	sNpcID	the NPC ID to be checked for quests.
*/
void CUser::AchieveComCountAdd(UserAchieveComTypes type, uint16 sNpcID/* = 0*/, CUser *pTarget/* = nullptr*/)
{
	bool CheckAchieve = false;
	foreach_stlmap(itr, m_AchieveMap)
	{
		_USER_ACHIEVE_INFO* pAchieveData = itr->second;
		if (pAchieveData == nullptr)
			continue;

		_ACHIEVE_MAIN *pAchieveMain = g_pMain->m_AchieveMainArray.GetData(itr->first);
		if (pAchieveMain == nullptr
			|| (pAchieveMain->ZoneID > 0 && pAchieveMain->ZoneID != GetZoneID())
			|| pAchieveMain->Type != AchieveCom)
			continue;

		// if the Achievement is completed or finished, why would we check for 'em?
		if (pAchieveData->bStatus == UserAchieveStatus::AchieveCompleted
			|| pAchieveData->bStatus == UserAchieveStatus::AchieveFinished)
			continue;

		switch (pAchieveMain->Type)
		{
		case AchieveCom:
		{
			_ACHIEVE_COM *pAchieveCom = g_pMain->m_AchieveComArray.GetData(pAchieveMain->sIndex);
			if (pAchieveCom == nullptr
				|| pAchieveCom->Type != type)
				continue;

			CheckAchieve = AchieveComCheck(pAchieveData, pAchieveCom, pAchieveMain, sNpcID, pTarget);

		}
		break;
		default:
			break;
		}
	}

	if (CheckAchieve)
		AchieveComCountAdd(UserAchieveComTypes::AchieveRequireAchieve);
}
#pragma endregion

#pragma region CUser::AchieveWarCheck(_USER_ACHIEVE_INFO *pAchieveData, _ACHIEVE_WAR *pAchieveWar,  _ACHIEVE_MAIN *pAchieveMain, uint16 sNpcID/* = 0*/, CUser *pTarget/* = nullptr*/)
/**
* @brief	Checks the Achieve_War table for the specified Achiement
*			whether the sent parameters fit for this achievement.
*
* @param	pAchieveData	the Achieve data in user achieve map.
* @param	pAchieveMain	the pointer for the table data in the Achive_Main
* @param	pNpc			the Npc to be examined.
* @param	pUser			the User to be examined.
*
*/
void CUser::AchieveWarCheck(_USER_ACHIEVE_INFO *pAchieveData, _ACHIEVE_WAR *pAchieveWar, _ACHIEVE_MAIN *pAchieveMain, uint16 sNpcID/* = 0*/, CUser *pTarget/* = nullptr*/)
{
	bool bAchieveFinished = false;

	switch (pAchieveWar->Type)
	{
	case AchieveDefeatEnemy:
	{
		if (pTarget == nullptr
			|| (pAchieveMain->ZoneID > 0 && pTarget->GetZoneID() != pAchieveMain->ZoneID)
			|| (pAchieveMain->ZoneID > 0 && GetZoneID() != pAchieveMain->ZoneID))
			return;

		if (pAchieveMain->ReqTime > 0)
		{
			_USER_ACHIEVE_TIMED_INFO* pTimedInfo = m_AchieveTimedMap.GetData(pAchieveWar->sIndex);
			if (pTimedInfo == nullptr || pTimedInfo->iExpirationTime < uint32(UNIXTIME))
				return;

			pAchieveData->iCount[0]++;
			if (pAchieveWar->Count <= pAchieveData->iCount[0])
			{
				pAchieveData->iCount[0] = pAchieveWar->Count;
				bAchieveFinished = true;
			}

		}
		else
		{
			pAchieveData->iCount[0]++;
			if (pAchieveWar->Count <= pAchieveData->iCount[0])
			{
				pAchieveData->iCount[0] = pAchieveWar->Count;
				bAchieveFinished = true;
			}
		}
	}
	break;
	case AchieveTakeRevange:
		pAchieveData->iCount[0]++;
		if (pAchieveWar->Count <= pAchieveData->iCount[0])
		{
			pAchieveData->iCount[0] = pAchieveWar->Count;
			bAchieveFinished = true;
		}
		break;
	case AchieveWinWar:
		m_AchieveInfo.WarCount++;
		if (pAchieveWar->Count <= m_AchieveInfo.WarCount)
		{
			pAchieveData->iCount[0] = pAchieveWar->Count;
			bAchieveFinished = true;
		}
		break;
	case AchieveWinNationGuardBattle:
		pAchieveData->iCount[0]++;
		if (pAchieveWar->Count <= pAchieveData->iCount[0])
		{
			pAchieveData->iCount[0] = pAchieveWar->Count;
			bAchieveFinished = true;
		}
		break;
	case AchieveWinJuraid:
		pAchieveData->iCount[0]++;
		if (pAchieveWar->Count <= pAchieveData->iCount[0])
		{
			pAchieveData->iCount[0] = pAchieveWar->Count;
			bAchieveFinished = true;
		}
		break;
	case AchieveWinChaos1:
		pAchieveData->iCount[0]++;
		if (pAchieveWar->Count <= pAchieveData->iCount[0])
		{
			pAchieveData->iCount[0] = pAchieveWar->Count;
			bAchieveFinished = true;
		}
		break;
	case AchieveWinChaos2:
		pAchieveData->iCount[0]++;
		if (pAchieveWar->Count <= pAchieveData->iCount[0])
		{
			pAchieveData->iCount[0] = pAchieveWar->Count;
			bAchieveFinished = true;
		}
		break;
	case AchieveWinChaos3:
		pAchieveData->iCount[0]++;
		if (pAchieveWar->Count <= pAchieveData->iCount[0])
		{
			pAchieveData->iCount[0] = pAchieveWar->Count;
			bAchieveFinished = true;
		}
		break;
	case AchieveWinCSW:
		pAchieveData->iCount[0]++;
		if (pAchieveWar->Count <= pAchieveData->iCount[0])
		{
			pAchieveData->iCount[0] = pAchieveWar->Count;
			bAchieveFinished = true;
		}
		break;
	case AchieveKillCountChaos:
		pAchieveData->iCount[0]++;
		if (pAchieveWar->Count <= pAchieveData->iCount[0])
		{
			pAchieveData->iCount[0] = pAchieveWar->Count;
			bAchieveFinished = true;
		}
		break;
	case AchieveDefeatMonsterRonarkL:
		if (pAchieveMain->ZoneID > 0 && pAchieveMain->ZoneID != GetZoneID())
			return;

		pAchieveData->iCount[0]++;
		if (pAchieveWar->Count <= pAchieveData->iCount[0])
		{
			pAchieveData->iCount[0] = pAchieveWar->Count;
			bAchieveFinished = true;
		}
		break;
	case AchieveDrakiTowerFinished:
		pAchieveData->iCount[0]++;
		if (pAchieveWar->Count <= pAchieveData->iCount[0])
		{
			pAchieveData->iCount[0] = pAchieveWar->Count;
			bAchieveFinished = true;
		}
		break;
	default:
		break;
	}

	if (bAchieveFinished)
	{
		switch (pAchieveMain->AchieveType)
		{
		case 0:
			m_AchieveInfo.NormalCount++;
			break;
		case 1:
			m_AchieveInfo.QuestCount++;
			break;
		case 2:
			m_AchieveInfo.WarCount++;
			break;
		case 3:
			m_AchieveInfo.AdvantureCount++;
			break;
		case 4:
			m_AchieveInfo.ChallangeCount++;
			break;
		default:
			break;
		}

		m_AchieveInfo.RecentAchieve[2] = m_AchieveInfo.RecentAchieve[1];
		m_AchieveInfo.RecentAchieve[1] = m_AchieveInfo.RecentAchieve[0];
		m_AchieveInfo.RecentAchieve[0] = pAchieveMain->sIndex;

		pAchieveData->bStatus = UserAchieveStatus::AchieveFinished;
		m_AchieveInfo.TotalMedal += pAchieveMain->Point;
		Packet result(WIZ_USER_ACHIEVE, uint8(AchieveSuccess));
		result << uint16(pAchieveMain->sIndex);
		Send(&result);

		AchieveComCountAdd(UserAchieveComTypes::AchieveRequireAchieve);
	}

}
#pragma endregion

#pragma region CUser::AchieveNormalCheck(_USER_ACHIEVE_INFO *pAchieveData, _ACHIEVE_NORMAL *pAchieveNormal, _ACHIEVE_MAIN *pAchieveMain, uint16 sNpcID /*= 0*/, CUser *pTarget /*= nullptr*/)

void CUser::AchieveNormalCheck(_USER_ACHIEVE_INFO *pAchieveData, _ACHIEVE_NORMAL *pAchieveNormal, _ACHIEVE_MAIN *pAchieveMain, uint16 sNpcID /*= 0*/, CUser *pTarget /*= nullptr*/)
{
	CNpc* pNpc = nullptr;
	bool bAchieveFinished = false;

	switch (pAchieveNormal->Type)
	{
	case AchieveBecomeKing:
	{
		CKingSystem * pKingSystem = g_pMain->m_KingSystemArray.GetData(GetNation());
		if (pKingSystem != nullptr && pKingSystem->m_strKingName == GetName())
		{
			pAchieveData->iCount[0]++;
			if (pAchieveNormal->Count == pAchieveData->iCount[0])
				bAchieveFinished = true;
		}
	}
	break;
	case AchieveReachNP:
		pAchieveData->iCount[0] = GetLoyalty();
		if (pAchieveNormal->Count <= pAchieveData->iCount[0])
		{
			pAchieveData->iCount[0] = pAchieveNormal->Count;
			bAchieveFinished = true;
		}
		break;
	case AchieveReachLevel:
		pAchieveData->iCount[0] = GetLevel();
		if (pAchieveNormal->Count <= pAchieveData->iCount[0])
		{
			pAchieveData->iCount[0] = pAchieveNormal->Count;
			bAchieveFinished = true;
		}
		break;
	case AchieveBecomeSpecial:
		pAchieveData->iCount[0]++;
		if (pAchieveNormal->Count <= pAchieveData->iCount[0])
		{
			pAchieveData->iCount[0] = pAchieveNormal->Count;
			bAchieveFinished = true;
		}
		break;
	case AchieveKnightContribution: // Achieve Knight Contribution Point of XXX
		if (GetClanID() > NO_CLAN && m_pKnightsUser != nullptr)
		{
			pAchieveData->iCount[0] = m_pKnightsUser->nDonatedNP;
			if (pAchieveNormal->Count <= pAchieveData->iCount[0])
			{
				pAchieveData->iCount[0] = pAchieveNormal->Count;
				bAchieveFinished = true;
			}
		}
		break;
	case AchieveDefeatMons: // Do not get killed and defeat XXX monsters
		pAchieveData->iCount[0]++;
		if (pAchieveNormal->Count <= pAchieveData->iCount[0])
		{
			pAchieveData->iCount[0] = pAchieveNormal->Count;
			bAchieveFinished = true;
		}
		break;
	case AchieveBecomeKnightMember:
		if (GetClanID() > NO_CLAN)
		{
			CKnights * pKnights = g_pMain->GetClanPtr(GetClanID());
			if (pKnights == nullptr)
				return;

			_KNIGHTS_SIEGE_WARFARE *pSiegeWarFare = g_pMain->GetSiegeMasterKnightsPtr(Aktive);
			if (pSiegeWarFare == nullptr)
				return;

			if (pAchieveNormal->sIndex == 419
				&& pKnights->m_byFlag == ClanTypeFlag::ClanTypeTraining) // Becoming a member of Knight Trainees
			{
				pAchieveData->iCount[0]++;
				bAchieveFinished = true;
			}

			else if (pAchieveNormal->sIndex == 420
				&& pKnights->m_byFlag > ClanTypeFlag::ClanTypeTraining) // Becoming a member of Knights
			{
				pAchieveData->iCount[0]++;
				bAchieveFinished = true;
			}

			else if (pAchieveNormal->sIndex == 421
				&& pSiegeWarFare->sMasterKnights > 0
				&& pKnights->GetID() == pSiegeWarFare->sMasterKnights) // Becoming a member of Castle Knights
			{
				pAchieveData->iCount[0]++;
				bAchieveFinished = true;
			}
		}
		break;
	default:
		break;
	}

	if (bAchieveFinished)
	{
		switch (pAchieveMain->AchieveType)
		{
		case 0:
			m_AchieveInfo.NormalCount++;
			break;
		case 1:
			m_AchieveInfo.QuestCount++;
			break;
		case 2:
			m_AchieveInfo.WarCount++;
			break;
		case 3:
			m_AchieveInfo.AdvantureCount++;
			break;
		case 4:
			m_AchieveInfo.ChallangeCount++;
			break;
		default:
			break;
		}

		m_AchieveInfo.RecentAchieve[2] = m_AchieveInfo.RecentAchieve[1];
		m_AchieveInfo.RecentAchieve[1] = m_AchieveInfo.RecentAchieve[0];
		m_AchieveInfo.RecentAchieve[0] = pAchieveMain->sIndex;

		pAchieveData->bStatus = UserAchieveStatus::AchieveFinished;
		m_AchieveInfo.TotalMedal += pAchieveMain->Point;
		Packet result(WIZ_USER_ACHIEVE, uint8(AchieveSuccess));
		result << uint16(pAchieveMain->sIndex);
		Send(&result);

		AchieveComCountAdd(UserAchieveComTypes::AchieveRequireAchieve);
	}

}
#pragma endregion

#pragma region CUser::AchieveComCheck(_USER_ACHIEVE_INFO *pAchieveData, _ACHIEVE_COM *pAchieveCom, _ACHIEVE_MAIN *pAchieveMain, uint16 sNpcID /*= 0*/, CUser *pTarget /*= nullptr*/)
bool CUser::AchieveComCheck(_USER_ACHIEVE_INFO *pAchieveData, _ACHIEVE_COM *pAchieveCom, _ACHIEVE_MAIN *pAchieveMain, uint16 sNpcID /*= 0*/, CUser *pTarget /*= nullptr*/)
{
	bool bAchieveFinished = false;

	switch (pAchieveCom->Type)
	{
	case UserAchieveComTypes::AchieveRequireQuest:
	{
		_USER_QUEST_INFO* pQuestInfoReq1 = m_questMap.GetData(pAchieveCom->Required1);
		if (pQuestInfoReq1 != nullptr && pQuestInfoReq1->QuestState == 2)
		{
			if (pAchieveCom->Required2 > 0)
			{
				_USER_QUEST_INFO* pQuestInfoReq2 = m_questMap.GetData(pAchieveCom->Required2);
				if (pQuestInfoReq2 != nullptr && pQuestInfoReq2->QuestState == 2)
				{
					pAchieveData->iCount[0]++;
					bAchieveFinished = true;
				}
			}
			else
			{
				pAchieveData->iCount[0]++;
				bAchieveFinished = true;
			}
		}
	}
	break;
	case UserAchieveComTypes::AchieveRequireAchieve:
	{
		_USER_ACHIEVE_INFO* pAchieveInfoReq1 = m_AchieveMap.GetData(pAchieveCom->Required1);
		if (pAchieveInfoReq1 != nullptr
			&& (pAchieveInfoReq1->bStatus == UserAchieveStatus::AchieveCompleted
				|| pAchieveInfoReq1->bStatus == UserAchieveStatus::AchieveFinished))
		{
			if (pAchieveCom->Required2 > 0)
			{
				_USER_ACHIEVE_INFO* pAchieveInfoReq2 = m_AchieveMap.GetData(pAchieveCom->Required2);
				if (pAchieveInfoReq2 != nullptr
					&& (pAchieveInfoReq2->bStatus == UserAchieveStatus::AchieveCompleted
						|| pAchieveInfoReq2->bStatus == UserAchieveStatus::AchieveFinished))
				{
					pAchieveData->iCount[0]++;
					bAchieveFinished = true;
				}
			}
			else
			{
				pAchieveData->iCount[0]++;
				bAchieveFinished = true;
			}
		}
	}
	break;
	default:
		break;
	}

	if (bAchieveFinished)
	{
		switch (pAchieveMain->AchieveType)
		{
		case 0:
			m_AchieveInfo.NormalCount++;
			break;
		case 1:
			m_AchieveInfo.QuestCount++;
			break;
		case 2:
			m_AchieveInfo.WarCount++;
			break;
		case 3:
			m_AchieveInfo.AdvantureCount++;
			break;
		case 4:
			m_AchieveInfo.ChallangeCount++;
			break;
		default:
			break;
		}

		m_AchieveInfo.RecentAchieve[2] = m_AchieveInfo.RecentAchieve[1];
		m_AchieveInfo.RecentAchieve[1] = m_AchieveInfo.RecentAchieve[0];
		m_AchieveInfo.RecentAchieve[0] = pAchieveMain->sIndex;

		pAchieveData->bStatus = UserAchieveStatus::AchieveFinished;
		m_AchieveInfo.TotalMedal += pAchieveMain->Point;
		Packet result(WIZ_USER_ACHIEVE, uint8(AchieveSuccess));
		result << uint16(pAchieveMain->sIndex);
		Send(&result);
		return true;
	}

	return false;

}

#pragma endregion

#pragma region CUser::AchieveMonsterCheck(_USER_ACHIEVE_INFO *pAchieveData, _ACHIEVE_MONSTER *pAchieveMonster, _ACHIEVE_MAIN *pAchieveMain, uint16 sNpcID)

void CUser::AchieveMonsterCheck(_USER_ACHIEVE_INFO *pAchieveData, _ACHIEVE_MONSTER *pAchieveMonster, _ACHIEVE_MAIN *pAchieveMain, uint16 sNpcID)
{
	if (pAchieveMain->ReqTime > 0)
	{
		_USER_ACHIEVE_TIMED_INFO* pTimedInfo = m_AchieveTimedMap.GetData(pAchieveMonster->sIndex);
		if (pTimedInfo == nullptr || pTimedInfo->iExpirationTime < uint32(UNIXTIME))
			return;
	}

	for (int group = 0; group < ACHIEVE_MOB_GROUPS; group++)
	{
		for (int i = 0; i < ACHIEVE_MOBS_PER_GROUP; i++)
		{
			if (pAchieveMonster->sNum[group][i] != sNpcID) // are we killing da monster for this achievement?
				continue;

			// do we exceed the total number of monster kill for this achievement?
			if (pAchieveData->iCount[group] + 1 > pAchieveMonster->sCount[group])
				return;

			pAchieveData->iCount[group]++; // well, congrats ya, you've killed a achieve monster so lets increase it up

										   // we finished the achiement huh!!
			if (pAchieveData->iCount[0] == pAchieveMonster->sCount[0]
				&& pAchieveData->iCount[1] == pAchieveMonster->sCount[1])
			{
				switch (pAchieveMain->AchieveType)
				{
				case 0:
					m_AchieveInfo.NormalCount++;
					break;
				case 1:
					m_AchieveInfo.QuestCount++;
					break;
				case 2:
					m_AchieveInfo.WarCount++;
					break;
				case 3:
					m_AchieveInfo.AdvantureCount++;
					break;
				case 4:
					m_AchieveInfo.ChallangeCount++;
					break;
				default:
					break;
				}

				m_AchieveInfo.RecentAchieve[2] = m_AchieveInfo.RecentAchieve[1];
				m_AchieveInfo.RecentAchieve[1] = m_AchieveInfo.RecentAchieve[0];
				m_AchieveInfo.RecentAchieve[0] = pAchieveMain->sIndex;

				pAchieveData->bStatus = UserAchieveStatus::AchieveFinished;
				m_AchieveInfo.TotalMedal += pAchieveMain->Point;
				Packet result(WIZ_USER_ACHIEVE, uint8(AchieveSuccess));
				result << uint16(pAchieveMain->sIndex);
				Send(&result);

				AchieveComCountAdd(UserAchieveComTypes::AchieveRequireAchieve);
				return;
			}
		}
	}
}

#pragma endregion

#pragma region CUser::HandleUserAchieveCoverTitle(Packet & pkt)
/*
* @brief	Sends user achievement cover title info.
*
* @param	pkt	The packet.
*/
void CUser::HandleUserAchieveCoverTitle(Packet & pkt)
{
	uint16 sCoverID, sSkillID;
	pkt >> sCoverID >> sSkillID;
	Packet result(WIZ_USER_ACHIEVE);
	_USER_ACHIEVE_INFO* pAchieveInfo = m_AchieveMap.GetData(sCoverID);
	if (pAchieveInfo == nullptr
		|| (pAchieveInfo->bStatus != UserAchieveStatus::AchieveFinished && pAchieveInfo->bStatus != UserAchieveStatus::AchieveCompleted))
	{
		result << uint8(AchieveCoverTitleReset) << uint16(sCoverID) << uint16(sSkillID) << uint8(1) << uint8(0);
		Send(&result);
		return;
	}

	_ACHIEVE_MAIN* pAchieveMain = g_pMain->m_AchieveMainArray.GetData(sCoverID);
	if (pAchieveMain)
	{
		m_sCoverID = sCoverID;
		m_sCoverTitle = pAchieveMain->TitleID;
		result << uint8(AchieveCoverTitle) << uint16(sCoverID) << uint16(sSkillID) << uint8(1) << uint8(0);
		Send(&result);
	}
}

#pragma endregion

#pragma region CUser::HandleUserAchieveSkillTitle(Packet & pkt)
/*
* @brief	Sends user achievement skill title info.
*
* @param	pkt	The packet.
*/
void CUser::HandleUserAchieveSkillTitle(Packet & pkt)
{
	uint16 sCoverID, sSkillID;
	pkt >> sCoverID >> sSkillID;
	Packet result(WIZ_USER_ACHIEVE);
	_USER_ACHIEVE_INFO* pAchieveInfo = m_AchieveMap.GetData(sCoverID);
	_ACHIEVE_TITLE *pAchieveTitle = g_pMain->m_AchieveTitleArray.GetData(sSkillID);
	if (pAchieveInfo == nullptr
		|| pAchieveTitle == nullptr
		|| (pAchieveInfo->bStatus != UserAchieveStatus::AchieveFinished && pAchieveInfo->bStatus != UserAchieveStatus::AchieveCompleted))
	{
		m_sSkillTitle = 0;
		result << uint8(AchieveSkillTitleReset) << uint16(sCoverID) << uint16(sSkillID) << uint8(1) << uint8(0);
		Send(&result);
		return;
	}

	_ACHIEVE_MAIN* pAchieveMain = g_pMain->m_AchieveMainArray.GetData(sCoverID);
	if (!pAchieveMain)
		return;

	m_sSkillID = sCoverID;
	m_sSkillTitle = pAchieveMain->TitleID;

	result << uint8(AchieveSkillTitle) << uint16(sCoverID) << uint16(m_sSkillTitle) << uint8(1) << uint8(0);
	Send(&result);


	m_sStatAchieveBonuses[ACHIEVE_STAT_STR] = pAchieveTitle->Str;
	m_sStatAchieveBonuses[ACHIEVE_STAT_STA] = pAchieveTitle->Hp;
	m_sStatAchieveBonuses[ACHIEVE_STAT_DEX] = pAchieveTitle->Dex;
	m_sStatAchieveBonuses[ACHIEVE_STAT_INT] = pAchieveTitle->Int;
	m_sStatAchieveBonuses[ACHIEVE_STAT_CHA] = pAchieveTitle->Mp;
	m_sStatAchieveBonuses[ACHIEVE_STAT_ATTACK] = pAchieveTitle->Attack;
	m_sStatAchieveBonuses[ACHIEVE_STAT_DEFENCE] = pAchieveTitle->Defence;
	SetUserAbility(true);
}

#pragma endregion

#pragma region CUser::HandleUserAchieveCoverTitleReset(Packet & pkt)
/*
* @brief	Sends user achievement skill title info.
*
* @param	pkt	The packet.
*/
void CUser::HandleUserAchieveCoverTitleReset(Packet & pkt)
{
	Packet result(WIZ_USER_ACHIEVE, uint8(AchieveCoverTitleReset));
	m_sCoverTitle = 0;
	m_sCoverID = 0;
	result << uint8(1) << uint8(0);
	Send(&result);
}

#pragma endregion

#pragma region CUser::HandleUserAchieveSkillTitleReset(Packet & pkt)
/*
* @brief	Sends user achievement skill title info.
*
* @param	pkt	The packet.
*/
void CUser::HandleUserAchieveSkillTitleReset(Packet & pkt)
{
	Packet result(WIZ_USER_ACHIEVE, uint8(AchieveSkillTitleReset));
	m_sSkillTitle = 0;
	m_sSkillID = 0;

	result << uint8(1) << uint8(0);
	Send(&result);

	m_sStatAchieveBonuses[ACHIEVE_STAT_STR] = 0;
	m_sStatAchieveBonuses[ACHIEVE_STAT_STA] = 0;
	m_sStatAchieveBonuses[ACHIEVE_STAT_DEX] = 0;
	m_sStatAchieveBonuses[ACHIEVE_STAT_INT] = 0;
	m_sStatAchieveBonuses[ACHIEVE_STAT_CHA] = 0;
	m_sStatAchieveBonuses[ACHIEVE_STAT_ATTACK] = 0;
	m_sStatAchieveBonuses[ACHIEVE_STAT_DEFENCE] = 0;

	SetUserAbility(true);
}
#pragma endregion
