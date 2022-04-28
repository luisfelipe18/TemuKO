#include "stdafx.h"

void CUser::UserRegionChat(Packet& pkt)
{
	uint8 opcode;
	pkt >> opcode;

	switch (opcode)
	{
	case UserInfo:
		HandleSurroundingAllUserInfo(UserInfo);
		break;
	case UserInfoDetail:
		HandleSurroundingUserInfoDetail(pkt);
		break;
	case UserInfoAll:
		HandleSurroundingAllUserInfo(UserInfoAll);
		break;
	case UserInfoShow:
		HandleSurroundingUserRegionUpdate();
		break;
	case 8:
	case 9:
	case 10:
	case 12:
	case 13:
	case 14:
	case 15:
		break;
	default:
		printf("User region chat unhandled packets %d \n", opcode);
		TRACE("User region chat unhandled packets %d \n", opcode);
		break;
	}
}

void CUser::HandleSurroundingAllUserInfo(uint8 type)
{
	if (UNIXTIME2 - m_tLastSurroundingUpdate < 500)
		return;

	KOMap* pMap = GetMap();
	if (pMap == nullptr)
		return;

	uint16 sCount = 0;
	size_t wpos = 0;
	m_tLastSurroundingUpdate = UNIXTIME2;

	Packet result(WIZ_USER_INFORMATIN, uint8(type));
	result << uint8(1) << uint16(GetZoneID());
	sCount = 0;
	wpos = result.wpos();
	result << sCount;

	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach(itr, sessMap)
	{
		CUser* pUser = TO_USER(itr->second);
		if (pUser == nullptr
			|| !pUser->isInGame()
			|| pUser->GetZoneID() != GetZoneID())
			continue;

		if (GetEventRoom() >= 0
			&& GetEventRoom() != pUser->GetEventRoom())
			continue;

		if (pUser->isGM())
			continue;

		CKnights* pKnight = g_pMain->GetClanPtr(pUser->GetClanID());

		result.SByte();
		result << pUser->GetName()
			<< uint8(pUser->GetNation())
			<< uint16(1)
			<< uint16(pUser->GetSPosX())
			<< uint16(pUser->GetSPosZ())
			<< uint16(pUser->GetClanID()) // clanid
			<< (pKnight != nullptr ? uint16(pKnight->m_sMarkVersion) : uint16(0))
			<< (pKnight != nullptr ? uint8(pKnight->m_byFlag) : uint8(0))  // clan flag
			<< (pKnight != nullptr ? uint8(pKnight->m_byGrade) : uint8(0)) // clan grade
			<< uint16(0);
		sCount++;
	}
	foreach_stlmap(itr, g_pMain->m_BotArray)
	{
		CBot* pUser = itr->second;

		if (pUser == nullptr
			|| !pUser->isInGame()
			|| pUser->GetZoneID() != GetZoneID())
			continue;

		CKnights* pKnight = g_pMain->GetClanPtr(pUser->GetClanID());

		result.SByte();
		result << pUser->GetName()
			<< uint8(pUser->GetNation())
			<< uint16(1)
			<< uint16(pUser->GetSPosX())
			<< uint16(pUser->GetSPosZ())
			<< uint16(pUser->GetClanID()) // clanid
			<< (pKnight != nullptr ? uint16(pKnight->m_sMarkVersion) : uint16(0))
			<< (pKnight != nullptr ? uint8(pKnight->m_byFlag) : uint8(0))  // clan flag
			<< (pKnight != nullptr ? uint8(pKnight->m_byGrade) : uint8(0)) // clan grade
			<< uint16(0);
		sCount++;
	}
	result.put(wpos, sCount);
	Send(&result);
}

/*void CUser::HandleSurroundingUserInfoDetail(Packet & pkt)
{
	Packet result(WIZ_USER_INFORMATIN, uint8(UserInfoDetail));
	std::string strCharName;
	pkt.SByte();
	pkt >> strCharName;
	CUser* pUser = g_pMain->GetUserPtr(strCharName, TYPE_CHARACTER);
	CKnights* pKnights = nullptr;

	if (pUser == nullptr)
	{
		CBot* pUser = g_pMain->GetBotPtr(strCharName, TYPE_CHARACTER);
		if (pUser == nullptr)
			result << uint8(0) << strCharName << uint64(0) << uint64(0) << uint64(0) << uint8(0);
		else
		{
			result.SByte();
			result << uint8(pUser->m_state) << pUser->GetName() << pUser->GetLevel() << pUser->GetClass() << pUser->GetLoyalty() << pUser->GetMonthlyLoyalty() << pUser->m_bResHpType;

			pKnights = g_pMain->GetClanPtr(pUser->GetClanID());

			if (pKnights == nullptr)
				result << uint16(0) << uint16(0) << uint16(0) << uint16(0);
			else
				result << pUser->GetClanID() << pKnights->m_sMarkVersion << pKnights->m_byFlag << pKnights->m_byGrade << pKnights->m_strName << pKnights->m_strChief;

			result << uint8(pUser->GetRebirthLevel()) << uint32(0);
		}
	}
	else
	{
		result.SByte();
		result << uint8(pUser->GetState()) << pUser->GetName() << pUser->GetLevel() << pUser->GetClass() << pUser->GetLoyalty() << pUser->GetMonthlyLoyalty() << pUser->m_bResHpType;

		pKnights = g_pMain->GetClanPtr(pUser->GetClanID());

		if (pKnights == nullptr)
			result << uint16(0) << uint16(0) << uint16(0) << uint16(0);
		else
			result << pUser->GetClanID() << pKnights->m_sMarkVersion << pKnights->m_byFlag << pKnights->m_byGrade << pKnights->m_strName << pKnights->m_strChief;

		result << uint8(pUser->GetRebirthLevel()) << uint32(0);
	}
	Send(&result);
}*/

// k2homeko Sorun Yok ! (Reb Level Gösteriyor)
void CUser::HandleSurroundingUserInfoDetail(Packet& pkt)
{
	Packet result(WIZ_USER_INFORMATIN, uint8(2));
	result.SByte();
	pkt.SByte();
	std::string strCharName = "";
	pkt >> strCharName;
	CUser* pUser = g_pMain->GetUserPtr(strCharName, TYPE_CHARACTER);
	CKnights* pKnight = nullptr;
	if (pUser == nullptr)
	{
		CBot* pBot = g_pMain->GetBotPtr(strCharName, TYPE_CHARACTER);
		if (pBot == nullptr)
			result << uint8(0) << strCharName << uint64(0) << uint64(0) << uint64(0) << uint8(0);
		else
		{
			result.SByte();
			result << uint8(pBot->m_state) << pBot->GetName() << pBot->GetLevel() << pBot->GetClass() << pBot->GetLoyalty() << pBot->GetMonthlyLoyalty() << pBot->m_bResHpType;

			pKnight = g_pMain->GetClanPtr(pBot->GetClanID());

			if (pKnight == nullptr)
				result << uint16(0) << uint16(0) << uint16(0) << uint16(0);
			else
				result << pBot->GetClanID() << pKnight->m_sMarkVersion << pKnight->m_byFlag << pKnight->m_byGrade << pKnight->m_strName << pKnight->m_strChief;

			result << pBot->GetRebirthLevel();

			Send(&result);

			result.clear();
			result.Initialize(WIZ_ITEM_UPGRADE);
			result << uint8(ITEM_CHARACTER_SEAL);
			result.SByte();
			result << uint8(CharacterSealOpcodes::Preview) << uint8(1)
				<< pBot->GetName()
				<< pBot->GetNation()
				<< pBot->GetRace()
				<< pBot->GetClass()
				<< pBot->GetLevel();
#if __VERSION > 2083
			result << pBot->m_iLoyalty;
#endif
			result << pBot->m_bStats[STAT_STR]
				<< pBot->m_bStats[STAT_STA]
				<< pBot->m_bStats[STAT_DEX]
				<< pBot->m_bStats[STAT_INT]
				<< pBot->m_bStats[STAT_CHA]
				<< pBot->m_iGold // Weird Sending this here hm
				<< pBot->m_bstrSkill[SkillPointFree]
				<< uint32(1) // -> Reading Which Skill has how many pointd
				<< pBot->m_bstrSkill[SkillPointCat1]
				<< pBot->m_bstrSkill[SkillPointCat2]
				<< pBot->m_bstrSkill[SkillPointCat3]
				<< pBot->m_bstrSkill[SkillPointMaster];

			for (int i = 0; i < INVENTORY_COSP; i++)
			{
				_ITEM_DATA* pItem = pBot->GetItem(i);
				if (pItem == nullptr)
					continue;

				result << pItem->nNum
					<< pItem->sDuration
					<< pItem->sCount
					<< pItem->bFlag;// item type flag (e.g. rented)
			}
			result << pBot->GetRebirthLevel();
			Send(&result);
		}
	}
	else //
	{
		result.SByte();
		result << uint8(pUser->GetState()) << pUser->GetName() << pUser->GetLevel() << pUser->GetClass() << pUser->GetLoyalty() << pUser->GetMonthlyLoyalty() << pUser->m_bResHpType;

		pKnight = g_pMain->GetClanPtr(pUser->GetClanID());

		if (pKnight == nullptr)
			result << uint16(0) << uint16(0) << uint16(0) << uint16(0);
		else
			result << pUser->GetClanID() << pKnight->m_sMarkVersion << pKnight->m_byFlag << pKnight->m_byGrade << pKnight->m_strName << pKnight->m_strChief;

		result << uint8(pUser->GetRebirthLevel()) << uint32(0);

		Send(&result);

		result.clear();
		result.Initialize(WIZ_ITEM_UPGRADE);
		result << uint8(ITEM_CHARACTER_SEAL);
		result.SByte();
		result << uint8(CharacterSealOpcodes::Preview) << uint8(1)
			<< pUser->GetName()
			<< pUser->GetNation()
			<< pUser->GetRace()
			<< pUser->GetClass()
			<< pUser->GetLevel();
#if __VERSION > 2083
		result << pUser->m_iLoyalty;
#endif
		result << pUser->m_bStats[STAT_STR]
			<< pUser->m_bStats[STAT_STA]
			<< pUser->m_bStats[STAT_DEX]
			<< pUser->m_bStats[STAT_INT]
			<< pUser->m_bStats[STAT_CHA]
			<< pUser->m_iGold // Weird Sending this here hm
			<< pUser->m_bstrSkill[SkillPointFree]
			<< uint32(1) // -> Reading Which Skill has how many pointd
			<< pUser->m_bstrSkill[SkillPointCat1]
			<< pUser->m_bstrSkill[SkillPointCat2]
			<< pUser->m_bstrSkill[SkillPointCat3]
			<< pUser->m_bstrSkill[SkillPointMaster];

		for (int i = 0; i < INVENTORY_COSP; i++)
		{
			_ITEM_DATA* pItem = pUser->GetItem(i);
			if (pItem == nullptr)
				continue;

			result << pItem->nNum
				<< pItem->sDuration
				<< pItem->sCount
				<< pItem->bFlag;// item type flag (e.g. rented)
		}
		result << pUser->GetRebirthLevel();
		Send(&result);
	}
}

void CUser::HandleSurroundingUserRegionUpdate()
{
	Packet result(WIZ_USER_INFORMATIN, uint8(UserInfoShow));
	result.SByte();
	result << GetName();
	g_pMain->Send_Zone(&result, GetZoneID());
}