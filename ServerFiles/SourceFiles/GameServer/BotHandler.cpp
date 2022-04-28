#include "stdafx.h"
#include "Map.h"
#include "../shared/DateTime.h"
#include "DBAgent.h"

CBot::CBot()
{
	Initialize();
}

void CBot::Initialize()
{
	MerchantChat.clear();
	LastWarpTime = 0;
	m_tLastKillTime = 0;
	m_sChallangeAchieveID = 0;
	m_pktcount = 0;
	m_strUserID.clear();
	m_strMemo.clear();
	m_strAccountID.clear();

	for (int i = 0; i < STAT_COUNT; i++)
		m_bRebStats[i] = 0;

	m_sAchieveCoverTitle = 0;

	m_bMerchantState = MERCHANT_STATE_NONE;
	m_sBind = -1;
	m_state = GAME_STATE_CONNECTED;
	m_bPartyLeader = false;
	m_bIsChicken = false;
	m_bIsHidingHelmet = false;
	m_bIsHidingCospre = false;

	m_bInParty = false;

	m_bInvisibilityType = INVIS_NONE;

	m_sDirection = ReturnSymbolisOK = 0;

	memset(&m_bStats, 0, sizeof(m_bStats));

	m_bAuthority = AUTHORITY_PLAYER;
	m_bLevel = 1;
	m_iExp = 0;
	m_iBank = m_iGold = 0;
	m_iLoyalty = 100;
	m_iLoyaltyMonthly = 0;
	m_iMannerPoint = 0;
	m_sHp = m_sMp = m_sSp = 0;

	memset(&m_arMerchantItems, 0, sizeof(m_arMerchantItems));
	m_bSellingMerchantPreparing = false;
	m_bBuyingMerchantPreparing = false;
	m_bMerchantOpen = false;
	m_bPremiumMerchant = false;
	m_bMerchantViewer = m_sMerchantsSocketID = -1;

	m_MaxHP = 0;
	m_MaxMp = 1;
	m_MaxSp = 120;

	m_bResHpType = USER_STANDING;
	m_bBlockPrivateChat = false;
	m_sPrivateChatUser = -1;
	m_bNeedParty = 0x01;
	m_bAbnormalType = ABNORMAL_NORMAL;	// User starts out in normal size.
	m_nOldAbnormalType = m_bAbnormalType;
	m_teamColour = TeamColourNone;
	m_bGenieStatus = m_bIsDevil = m_bIsHidingWings = false;
	m_bRank = 0;

	m_bSlaveMerchant = false;
	m_bSlaveUserID = -1;

	m_bPersonalRank = m_bKnightsRank = -1;
}

bool CBot::RegisterRegion()
{
	uint16
		new_region_x = GetNewRegionX(), new_region_z = GetNewRegionZ(),
		old_region_x = GetRegionX(), old_region_z = GetRegionZ();

	if (GetRegion() == nullptr
		|| (old_region_x == new_region_x
			&& old_region_z == new_region_z))
		return false;

	AddToRegion(new_region_x, new_region_z);

	RemoveRegion(old_region_x - new_region_x, old_region_z - new_region_z);
	InsertRegion(new_region_x - old_region_x, new_region_z - old_region_z);

	return true;
}

void CBot::AddToRegion(int16 new_region_x, int16 new_region_z)
{
	if (GetRegion())
		GetRegion()->Remove(this);

	SetRegion(new_region_x, new_region_z);

	if (GetRegion())
		GetRegion()->Add(this);
}

void CBot::RemoveRegion(int16 del_x, int16 del_z)
{
	if (GetMap() == nullptr)
		return;

	Packet result;
	GetInOut(result, INOUT_OUT);
	g_pMain->Send_OldRegions(&result, del_x, del_z, GetMap(), GetRegionX(), GetRegionZ(), nullptr, 0);
}

void CBot::InsertRegion(int16 insert_x, int16 insert_z)
{
	if (GetMap() == nullptr)
		return;

	Packet result;
	GetInOut(result, INOUT_IN);
	g_pMain->Send_NewRegions(&result, insert_x, insert_z, GetMap(), GetRegionX(), GetRegionZ(), nullptr, 0);
}

void CBot::SetRegion(uint16 x /*= -1*/, uint16 z /*= -1*/)
{
	m_sRegionX = x; m_sRegionZ = z;
	m_pRegion = m_pMap->GetRegion(x, z); // TODO: Clean this up
}

void CBot::StateChangeServerDirect(uint8 bType, uint32 nBuff)
{
	uint8 buff = *(uint8*)&nBuff; // don't ask
	switch (bType)
	{
	case 1:
		m_bResHpType = buff;
		break;

	case 2:
		m_bNeedParty = buff;
		break;

	case 3:
		m_nOldAbnormalType = m_bAbnormalType;


		m_bAbnormalType = nBuff;
		break;

	case 5:
		m_bAbnormalType = nBuff;
		break;

	case 6:
		nBuff = m_bPartyLeader; // we don't set this here.
		break;

	case 7:
		break;

	case 8: // beginner quest
		break;

	case 14:
		break;
	}

	Packet result(WIZ_STATE_CHANGE);
	result << GetID() << bType << nBuff;
	SendToRegion(&result);
}

void CBot::GetInOut(Packet& result, uint8 bType)
{
	result.Initialize(WIZ_USER_INOUT);
	result << uint16(bType) << GetID();
	if (bType != INOUT_OUT)
		GetUserInfo(result);
}

void CBot::UserInOut(uint8 bType)
{
	Packet result;
	GetInOut(result, bType);

	if (bType == INOUT_OUT)
		Remove();
	else
		Add();

	SendToRegion(&result);
}

void CBot::Add()
{
	Guard lock(g_pMain->m_BotcharacterNameLock);

	if (GetRegion())
		GetRegion()->Add(this);

	m_state = GAME_STATE_INGAME;
	g_DBAgent.InsertCurrentUser(GetName(), GetName());
	std::string upperName = GetName();
	STRTOUPPER(upperName);
	g_pMain->m_BotcharacterNameMap[upperName] = this;
}

void CBot::Remove()
{
	Guard lock(g_pMain->m_BotcharacterNameLock);

	if (GetRegion())
		GetRegion()->Remove(this);

	m_state = GAME_STATE_CONNECTED;
	HandleSurroundingUserRegionUpdate();
	g_DBAgent.RemoveCurrentUser(GetName());
	std::string upperName = GetName();
	STRTOUPPER(upperName);
	g_pMain->m_BotcharacterNameMap.erase(upperName);
}
void CBot::SendToRegion(Packet* pkt)
{
	g_pMain->Send_Region(pkt, GetMap(), GetRegionX(), GetRegionZ(), nullptr, 0);
}

void CBot::GetUserInfo(Packet& pkt)
{
	pkt.SByte();
	pkt << GetName() << uint16(GetNation()) << uint8(0);
	pkt << GetClanID() << GetFame();

	CKnights* pKnights = g_pMain->GetClanPtr(GetClanID());
	if (pKnights == nullptr)
	{
		if (isKing())
			pkt << uint32(0) << uint16(0) << uint8(0) << uint16(KNIGHTS_KNIG_CAPE) << uint32(0) << uint8(0);
		else
			pkt << uint32(0) << uint16(0) << uint8(0) << uint16(-1) << uint32(0) << uint8(0);
			//pkt << uint32(0) << uint16(0) << uint8(0) << uint16(GetNation() == KARUS ? 97 : 98) << uint32(0) << uint8(0); // Bot Kral Pelerin
	}
	else
	{
		pkt << pKnights->GetAllianceID() << pKnights->GetName() << pKnights->m_byGrade << pKnights->m_byRanking << pKnights->m_sMarkVersion; // symbol/mark version

		CKnights* pMainClan = g_pMain->GetClanPtr(pKnights->GetAllianceID());
		_KNIGHTS_ALLIANCE* pAlliance = g_pMain->GetAlliancePtr(pKnights->GetAllianceID());

		if (pKnights->isInAlliance() && pMainClan != nullptr && pAlliance != nullptr)
		{
			if (!isKing())
			{
				_KNIGHTS_CASTELLAN_CAPE* pCastellanInfo = g_pMain->m_KnightCastellanCapeArray.GetData(pMainClan->GetID());
				if (pCastellanInfo != nullptr && pCastellanInfo->sRemainingTime >= (uint64)UNIXTIME)
				{
					if (pAlliance->sMainAllianceKnights == pKnights->GetID())
						pkt << pCastellanInfo->sCape << pCastellanInfo->sR << pCastellanInfo->sG << pCastellanInfo->sB << uint8(0);
					else if (pAlliance->sSubAllianceKnights == pKnights->GetID())
						pkt << pCastellanInfo->sCape << pKnights->m_bCapeR << pKnights->m_bCapeG << pKnights->m_bCapeB << uint8(0);
					else if (pAlliance->sMercenaryClan_1 == pKnights->GetID() || pAlliance->sMercenaryClan_2 == pKnights->GetID())
						pkt << pCastellanInfo->sCape << uint32(0); // only the cape will be present
					else
						pkt << pCastellanInfo->sCape << pMainClan->m_bCapeR << pMainClan->m_bCapeG << pMainClan->m_bCapeB << uint8(0); // this is stored in 4 bytes after all.
				}
				else
				{
					if (pAlliance->sMainAllianceKnights == pKnights->GetID())
						pkt << pMainClan->GetCapeID() << pMainClan->m_bCapeR << pMainClan->m_bCapeG << pMainClan->m_bCapeB << uint8(0);
					else if (pAlliance->sSubAllianceKnights == pKnights->GetID())
						pkt << pMainClan->GetCapeID() << pKnights->m_bCapeR << pKnights->m_bCapeG << pKnights->m_bCapeB << uint8(0);
					else if (pAlliance->sMercenaryClan_1 == pKnights->GetID() || pAlliance->sMercenaryClan_2 == pKnights->GetID())
						pkt << pMainClan->GetCapeID() << uint32(0); // only the cape will be present
					else
						pkt << pMainClan->GetCapeID() << pMainClan->m_bCapeR << pMainClan->m_bCapeG << pMainClan->m_bCapeB << uint8(0); // this is stored in 4 bytes after all.
				}
			}
			else
				pkt << uint16(KNIGHTS_KNIG_CAPE) << uint32(0); // cape ID

			// not sure what this is, but it (just?) enables the clan symbol on the cape 
			// value in dump was 9, but everything tested seems to behave as equally well...
			// we'll probably have to implement logic to respect requirements.
			pkt << ((pKnights->m_byFlag > 1 && pKnights->m_byGrade < 3) ? uint8(1) : uint8(0));
		}
		else
		{
			if (!isKing())
			{
				_KNIGHTS_CASTELLAN_CAPE* pCastellanInfo = g_pMain->m_KnightCastellanCapeArray.GetData(pKnights->GetID());
				if (pCastellanInfo != nullptr && pCastellanInfo->sRemainingTime >= (uint64)UNIXTIME)
					pkt << pCastellanInfo->sCape << pCastellanInfo->sR << pCastellanInfo->sG << pCastellanInfo->sB << uint8(0); // this is stored in 4 bytes after all.
				else
					pkt << pKnights->GetCapeID() << pKnights->m_bCapeR << pKnights->m_bCapeG << pKnights->m_bCapeB << uint8(0); // this is stored in 4 bytes after all.
			}
			else
				pkt << uint16(KNIGHTS_KNIG_CAPE) << uint32(0); // cape ID

			// not sure what this is, but it (just?) enables the clan symbol on the cape 
			// value in dump was 9, but everything tested seems to behave as equally well...
			// we'll probably have to implement logic to respect requirements.
			pkt << ((pKnights->m_byFlag > 1 && pKnights->m_byGrade < 3) ? uint8(1) : uint8(0));
		}
	}
	// There are two event-driven invisibility states; dispel on attack, and dispel on move.
	// These are handled primarily server-side; from memory the client only cares about value 1 (which we class as 'dispel on move').
	// As this is the only place where this flag is actually sent to the client, we'll just convert 'dispel on attack' 
	// back to 'dispel on move' as the client expects.
	uint8 bInvisibilityType = m_bInvisibilityType;
	if (bInvisibilityType != INVIS_NONE)
		bInvisibilityType = INVIS_DISPEL_ON_MOVE;

	pkt << GetLevel() << m_bRace << m_sClass
		<< GetSPosX() << GetSPosZ() << GetSPosY()
		<< m_bFace << m_nHair
		<< m_bResHpType << uint32(m_bAbnormalType)
		<< m_bNeedParty
		<< m_bAuthority
		<< m_bPartyLeader
		<< bInvisibilityType
		<< uint8(m_teamColour)
		<< m_bIsHidingHelmet
		<< m_bIsHidingCospre
		<< m_bIsDevil
		<< m_bIsHidingWings
		<< m_sDirection
		<< m_bIsChicken
		<< m_bRank
		<< (m_bKnightsRank <= m_bPersonalRank ? m_bKnightsRank : int8(-1))
		<< (m_bPersonalRank <= m_bKnightsRank ? m_bPersonalRank : int8(-1));

	uint8 equippedItems[] =
	{
		BREAST, LEG, HEAD, GLOVE, FOOT, SHOULDER, RIGHTHAND, LEFTHAND,
		CWING, CHELMET, CLEFT, CRIGHT, CTOP, CFAIRY, CTATTOO
	};

	uint8 equippedItemsKnightRoyale[] =
	{
		KNIGHT_ROYAL_LEFTHAND, KNIGHT_ROYAL_RIGHTHAND
	};

	bool isRoyaleSignEvent = (GetZoneID() == ZONE_KNIGHT_ROYALE);
	bool isWarOpen = (g_pMain->m_byBattleOpen == NATION_BATTLE && g_pMain->m_byBattleZone + ZONE_BATTLE_BASE != ZONE_BATTLE3);
	_ITEM_DATA* pItem = nullptr;

	if (isRoyaleSignEvent)
	{
		foreach_array(i, equippedItemsKnightRoyale)
		{
			pItem = GetItem(equippedItemsKnightRoyale[i]);
			if (pItem == nullptr)
				continue;

			pkt << pItem->nNum << pItem->sDuration << pItem->bFlag;
		}
	}
	else
	{
		foreach_array(i, equippedItems)
		{
			pItem = GetItem(equippedItems[i]);
			if (pItem == nullptr)
				continue;

			if (isWarOpen)
			{
				if (isWarrior())
				{
					if (i == RIGHTEAR)
						pkt << (uint32)WARRIOR_DRAGON_ARMOR_PAULDRON << pItem->sDuration << pItem->bFlag;
					else if (i == HEAD)
						pkt << (uint32)WARRIOR_DRAGON_ARMOR_PAD << pItem->sDuration << pItem->bFlag;
					else if (i == LEFTEAR)
						pkt << (uint32)WARRIOR_DRAGON_ARMOR_HELMET << pItem->sDuration << pItem->bFlag;
					else if (i == NECK)
						pkt << (uint32)WARRIOR_DRAGON_ARMOR_GAUNTLET << pItem->sDuration << pItem->bFlag;
					else if (i == BREAST)
						pkt << (uint32)WARRIOR_DRAGON_ARMOR_BOOTS << pItem->sDuration << pItem->bFlag;
					else
						pkt << pItem->nNum << pItem->sDuration << pItem->bFlag;
				}
				else if (isRogue())
				{
					if (i == RIGHTEAR)
						pkt << (uint32)ROGUE_DRAGON_ARMOR_PAULDRON << pItem->sDuration << pItem->bFlag;
					else if (i == HEAD)
						pkt << (uint32)ROGUE_DRAGON_ARMOR_PAD << pItem->sDuration << pItem->bFlag;
					else if (i == LEFTEAR)
						pkt << (uint32)ROGUE_DRAGON_ARMOR_HELMET << pItem->sDuration << pItem->bFlag;
					else if (i == NECK)
						pkt << (uint32)ROGUE_DRAGON_ARMOR_GAUNTLET << pItem->sDuration << pItem->bFlag;
					else if (i == BREAST)
						pkt << (uint32)ROGUE_DRAGON_ARMOR_BOOTS << pItem->sDuration << pItem->bFlag;
					else
						pkt << pItem->nNum << pItem->sDuration << pItem->bFlag;
				}
				else if (isMage())
				{
					if (i == RIGHTEAR)
						pkt << (uint32)MAGE_DRAGON_ARMOR_PAULDRON << pItem->sDuration << pItem->bFlag;
					else if (i == HEAD)
						pkt << (uint32)MAGE_DRAGON_ARMOR_PAD << pItem->sDuration << pItem->bFlag;
					else if (i == LEFTEAR)
						pkt << (uint32)MAGE_DRAGON_ARMOR_HELMET << pItem->sDuration << pItem->bFlag;
					else if (i == NECK)
						pkt << (uint32)MAGE_DRAGON_ARMOR_GAUNTLET << pItem->sDuration << pItem->bFlag;
					else if (i == BREAST)
						pkt << (uint32)MAGE_DRAGON_ARMOR_BOOTS << pItem->sDuration << pItem->bFlag;
					else
						pkt << pItem->nNum << pItem->sDuration << pItem->bFlag;
				}
				else if (isPriest())
				{
					if (i == RIGHTEAR)
						pkt << (uint32)PRIEST_DRAGON_ARMOR_PAULDRON << pItem->sDuration << pItem->bFlag;
					else if (i == HEAD)
						pkt << (uint32)PRIEST_DRAGON_ARMOR_PAD << pItem->sDuration << pItem->bFlag;
					else if (i == LEFTEAR)
						pkt << (uint32)PRIEST_DRAGON_ARMOR_HELMET << pItem->sDuration << pItem->bFlag;
					else if (i == NECK)
						pkt << (uint32)PRIEST_DRAGON_ARMOR_GAUNTLET << pItem->sDuration << pItem->bFlag;
					else if (i == BREAST)
						pkt << (uint32)PRIEST_DRAGON_ARMOR_BOOTS << pItem->sDuration << pItem->bFlag;
					else
						pkt << pItem->nNum << pItem->sDuration << pItem->bFlag;
				}
				else if (isPortuKurian())
				{
					if (i == RIGHTEAR)
						pkt << (uint32)WARRIOR_DRAGON_ARMOR_PAULDRON << pItem->sDuration << pItem->bFlag;
					else if (i == HEAD)
						pkt << (uint32)WARRIOR_DRAGON_ARMOR_PAD << pItem->sDuration << pItem->bFlag;
					else if (i == LEFTEAR)
						pkt << (uint32)WARRIOR_DRAGON_ARMOR_HELMET << pItem->sDuration << pItem->bFlag;
					else if (i == NECK)
						pkt << (uint32)WARRIOR_DRAGON_ARMOR_GAUNTLET << pItem->sDuration << pItem->bFlag;
					else if (i == BREAST)
						pkt << (uint32)WARRIOR_DRAGON_ARMOR_BOOTS << pItem->sDuration << pItem->bFlag;
					else
						pkt << pItem->nNum << pItem->sDuration << pItem->bFlag;
				}
			}
			else
			{
				pItem = GetItem(equippedItems[i]);
				if (pItem == nullptr)
					continue;

				pkt << pItem->nNum << pItem->sDuration << pItem->bFlag;
			}
		}
	}
	pkt << GetZoneID()
		<< uint8(-1)
		<< uint8(-1)
		<< uint32(0)
		<< m_bIsHidingWings
		<< m_bIsHidingHelmet
		<< m_bIsHidingCospre
		<< isGenieActive()
		<< GetRebirthLevel() /*// is reb exp 83+ thing << uint8(m_bLevel == 83) // is reb exp 83+ thing*/
		<< uint16(m_sAchieveCoverTitle)
		<< ReturnSymbolisOK // R symbol after name returned?
		<< uint32(0); // face time system
}

void CBot::SetMaxHp(int iFlag)
{
	_CLASS_COEFFICIENT* p_TableCoefficient = nullptr;
	p_TableCoefficient = g_pMain->m_CoefficientArray.GetData(m_sClass);
	if (!p_TableCoefficient) return;

	int temp_sta = GetStat(STAT_STA);

	if (GetZoneID() == ZONE_SNOW_BATTLE && iFlag == 0)
	{
		if (GetFame() == COMMAND_CAPTAIN || isKing())
			m_MaxHP = 300;
		else
			m_MaxHP = 100;
	}
	else if (m_bZone == ZONE_CHAOS_DUNGEON && iFlag == 0)
		m_MaxHP = 10000 / 10;
	else
	{
		m_MaxHP = (short)(((p_TableCoefficient->HP * GetLevel() * GetLevel() * temp_sta)
			+ 0.1 * (GetLevel() * temp_sta) + (temp_sta / 5)) + 20);

		// A player's max HP should be capped at (currently) 14,000 HP.
		if (m_MaxHP > MAX_PLAYER_HP)
			m_MaxHP = MAX_PLAYER_HP;

		if (iFlag == 1)
			m_sHp = m_MaxHP;
		else if (iFlag == 2)
			m_MaxHP = 100;
	}

	if (m_MaxHP < m_sHp)
		m_sHp = m_MaxHP;
}

void CBot::SetMaxMp()
{
	_CLASS_COEFFICIENT* p_TableCoefficient = nullptr;
	p_TableCoefficient = g_pMain->m_CoefficientArray.GetData(m_sClass);
	if (!p_TableCoefficient)
		return;

	int temp_intel = 0, temp_sta = 0;
	temp_intel = GetStat(STAT_INT) + 30;

	if (temp_intel > 255)
		temp_intel = 255;

	temp_sta = GetStat(STAT_STA);
	if (temp_sta > 255)
		temp_sta = 255;

	if (p_TableCoefficient->MP != 0)
	{
		m_MaxMp = (short)((p_TableCoefficient->MP * GetLevel() * GetLevel() * temp_intel)
			+ (0.1f * GetLevel() * 2 * temp_intel) + (temp_intel / 5) + 20);
	}
	else if (p_TableCoefficient->SP != 0)
	{
		m_MaxMp = (short)((p_TableCoefficient->SP * GetLevel() * GetLevel() * temp_sta)
			+ (0.1f * GetLevel() * temp_sta) + (temp_sta / 5));
	}

	if (m_MaxMp < m_sMp)
		m_sMp = m_MaxMp;
}

bool CBot::JobGroupCheck(short jobgroupid)
{
	if (jobgroupid > 100)
		return GetClass() == jobgroupid;

	ClassType subClass = GetBaseClassType();
	switch (jobgroupid)
	{
	case GROUP_WARRIOR:
		return (subClass == ClassWarrior || subClass == ClassWarriorNovice || subClass == ClassWarriorMaster);

	case GROUP_ROGUE:
		return (subClass == ClassRogue || subClass == ClassRogueNovice || subClass == ClassRogueMaster);

	case GROUP_MAGE:
		return (subClass == ClassMage || subClass == ClassMageNovice || subClass == ClassMageMaster);

	case GROUP_CLERIC:
		return (subClass == ClassPriest || subClass == ClassPriestNovice || subClass == ClassPriestMaster);

	case GROUP_PORTU_KURIAN:
		return (subClass == ClassPortuKurian || subClass == ClassPortuKurianNovice || subClass == ClassPortuKurianMaster);
	}

	return (subClass == jobgroupid);
}

uint16 CGameServerDlg::SpawnSlaveUserBot(int Minute, CUser* pUser)
{
	foreach_stlmap(itr, m_BotArray)
	{
		CBot* pSlaveUser = itr->second;

		if (pSlaveUser->m_state == GAME_STATE_INGAME)
			continue;

		if (pSlaveUser->GetLevel() > pUser->GetLevel()
			|| pSlaveUser->GetNation() != pUser->GetNation())
			continue;

		pSlaveUser->Initialize();
		pSlaveUser->LastWarpTime = UNIXTIME + (Minute * 99999); // Slave Merchant Bot Süresi
		pSlaveUser->m_pMap = GetZoneByID(pUser->GetZoneID());
		pSlaveUser->m_bZone = pUser->GetZoneID();
		pSlaveUser->m_sClass = pUser->GetClass();
		pSlaveUser->m_bRace = pUser->m_bRace;
		pSlaveUser->m_curx = pUser->m_curx;
		pSlaveUser->m_curz = pUser->m_curz;
		pSlaveUser->m_cury = pUser->m_cury;
		pSlaveUser->m_oldx = pUser->m_oldx;
		pSlaveUser->m_oldy = pUser->m_oldy;
		pSlaveUser->m_oldz = pUser->m_oldz;
		pSlaveUser->m_nHair = pUser->m_nHair;
		pSlaveUser->m_bFace = pUser->m_bFace;
		pSlaveUser->m_bFame = pUser->m_bFame;
		pSlaveUser->m_bLevel = pUser->GetLevel();

		memset(pSlaveUser->m_sItemArray, 0x00, sizeof(pSlaveUser->m_sItemArray));
		memset(pSlaveUser->m_arMerchantItems, 0x00, sizeof(pSlaveUser->m_arMerchantItems));

		for (int i = 0; i < INVENTORY_TOTAL; i++)
			pSlaveUser->m_sItemArray[i] = pUser->m_sItemArray[i];

		pSlaveUser->m_reblvl = pUser->m_bRebirthLevel;

		for (int i = 0; i < 10; i++)
			pSlaveUser->m_bstrSkill[i] = pUser->m_bstrSkill[i];

		pSlaveUser->m_iGold = pUser->m_iGold;
		pSlaveUser->m_sPoints = pUser->m_sPoints;

		for (int i = 0; i < 5; i++)
			pSlaveUser->m_bStats[i] = pUser->m_bStats[i];

		pSlaveUser->m_strUserID = "[Slave]" + pUser->GetName();
		pSlaveUser->m_bKnightsRank = pUser->m_bKnightsRank;
		pSlaveUser->m_bPersonalRank = pUser->m_bPersonalRank;
		pSlaveUser->m_sAchieveCoverTitle = pUser->m_sCoverTitle;
		pSlaveUser->ReturnSymbolisOK = pUser->ReturnSymbolisOK;
		pSlaveUser->m_bResHpType = pUser->m_bResHpType;

		pSlaveUser->SetPosition(pUser->GetX(), pUser->GetY(), pUser->GetZ());
		pSlaveUser->SetRegion(pUser->GetNewRegionX(), pUser->GetNewRegionZ());

		pSlaveUser->m_bSlaveMerchant = true;
		pSlaveUser->m_bSlaveUserID = pUser->GetSocketID();

		pUser->m_bSlaveMerchant = true;
		pUser->m_bSlaveUserID = pSlaveUser->GetID();

		if (pUser->GetNation() == KARUS)
			pSlaveUser->m_teamColour = TeamColourBlue;
		else
			pSlaveUser->m_teamColour = TeamColourRed;

		pSlaveUser->SetMaxHp(1);
		pSlaveUser->SetMaxMp();
		pSlaveUser->UserInOut(INOUT_IN);
		return pSlaveUser->GetID();
	}
	return 0;
}

void CBot::HandleSurroundingUserRegionUpdate()
{
	Packet result(WIZ_USER_INFORMATIN, uint8(UserInfoShow));
	result.SByte();
	result << GetName();
	g_pMain->Send_Zone(&result, GetZoneID());
}

uint16 CGameServerDlg::SpawnUserBot(int Minute, uint8 byZone, float fX, float fY, float fZ, uint8 Restipi, uint8 minlevel /* = 1*/)
{
	foreach_stlmap(itr, m_BotArray)
	{
		CBot* pUser = itr->second;

		if (pUser->m_state == GAME_STATE_INGAME)
			continue;

		if (byZone <= ZONE_ELMORAD && byZone != pUser->m_bNation
			|| (byZone >= ZONE_KARUS_ESLANT
				&& byZone <= ZONE_ELMORAD_ESLANT
				&& byZone != (pUser->m_bNation + 10)))
			continue;

		if (pUser->m_bLevel > minlevel)
			continue;

		pUser->m_bMerchantState = MERCHANT_STATE_NONE;
		pUser->LastWarpTime = UNIXTIME + (Minute * 60);
		pUser->m_pMap = GetZoneByID(byZone);
		pUser->m_bZone = byZone;

		if (Restipi == 1)
		{
			_ITEM_DATA* pItem = &pUser->m_sItemArray[RIGHTHAND];
			if (pItem == nullptr)
			{
				_ITEM_TABLE* pTable = GetItemPtr(MATTOCK);
				if (pTable == nullptr)
					continue;

				_ITEM_DATA* pTItem = &pUser->m_sItemArray[LEFTHAND];
				if (pTItem != nullptr)
					memset(pTItem, 0x00, sizeof(_ITEM_DATA));

				memset(pItem, 0x00, sizeof(_ITEM_DATA));
				pItem->nNum = pTable->m_iNum;
				pItem->nSerialNum = GenerateItemSerial();
				pItem->sCount = 1;
				pItem->sDuration = pTable->m_sDuration;

				pUser->m_bResHpType = USER_MINING;
			}
			else
			{
				_ITEM_TABLE* pTable = GetItemPtr(pItem->nNum);
				if (pTable == nullptr)
					continue;

				if (pTable->isPickaxe())
					pUser->m_bResHpType = USER_MINING;
				else
				{
					pTable = GetItemPtr(MATTOCK);
					if (pTable == nullptr)
						continue;

					_ITEM_DATA* pTItem = &pUser->m_sItemArray[LEFTHAND];
					if (pTItem != nullptr)
						memset(pTItem, 0x00, sizeof(_ITEM_DATA));

					memset(pItem, 0x00, sizeof(_ITEM_DATA));
					pItem->nNum = pTable->m_iNum;
					pItem->nSerialNum = GenerateItemSerial();
					pItem->sCount = 1;
					pItem->sDuration = pTable->m_sDuration;

					pUser->m_bResHpType = USER_MINING;
				}
			}
		}
		else if (Restipi == 2)
		{
			_ITEM_DATA* pItem = &pUser->m_sItemArray[RIGHTHAND];
			if (pItem == nullptr)
			{
				_ITEM_TABLE* pTable = GetItemPtr(FISHING);
				if (pTable == nullptr)
					continue;

				_ITEM_DATA* pTItem = &pUser->m_sItemArray[LEFTHAND];
				if (pTItem != nullptr)
					memset(pTItem, 0x00, sizeof(_ITEM_DATA));

				memset(pItem, 0x00, sizeof(_ITEM_DATA));
				pItem->nNum = pTable->m_iNum;
				pItem->nSerialNum = GenerateItemSerial();
				pItem->sCount = 1;
				pItem->sDuration = pTable->m_sDuration;

				pUser->m_bResHpType = USER_FLASHING;
			}
			else
			{
				_ITEM_TABLE* pTable = GetItemPtr(pItem->nNum);
				if (pTable == nullptr)
					continue;

				if (pTable->isFishing())
					pUser->m_bResHpType = USER_FLASHING;
				else
				{
					pTable = GetItemPtr(FISHING);
					if (pTable == nullptr)
						continue;

					_ITEM_DATA* pTItem = &pUser->m_sItemArray[LEFTHAND];
					if (pTItem != nullptr)
						memset(pTItem, 0x00, sizeof(_ITEM_DATA));

					memset(pItem, 0x00, sizeof(_ITEM_DATA));
					pItem->nNum = pTable->m_iNum;
					pItem->nSerialNum = GenerateItemSerial();
					pItem->sCount = 1;
					pItem->sDuration = pTable->m_sDuration;

					pUser->m_bResHpType = USER_FLASHING;
				}
			}
		}
		else if (Restipi == 3
			|| Restipi == 4)
			pUser->m_bResHpType = Restipi == 3 ? USER_STANDING : USER_SITDOWN;
		else if (Restipi == 5)
		{
			int Random = myrand(0, 10000);

			pUser->m_bResHpType = Random > 5000 ? USER_STANDING : USER_SITDOWN;
		}
		else
			continue;

		pUser->SetPosition(fX, fY, fZ);
		pUser->SetRegion(pUser->GetNewRegionX(), pUser->GetNewRegionZ());

		pUser->SetMaxHp(1);
		pUser->SetMaxMp();
		pUser->UserInOut(INOUT_IN);
		return pUser->GetID();
	}
	return true;
}