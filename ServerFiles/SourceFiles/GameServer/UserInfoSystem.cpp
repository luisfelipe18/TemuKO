#include "stdafx.h"
#include "KingSystem.h"
#include "DBAgent.h"

using namespace std;

/**
* @brief	Sends the player's information on initial login.
*/
void CUser::SendMyInfo()
{
	C3DMap* pMap = GetMap();
	CKnights* pKnights = nullptr;

	if (!pMap->IsValidPosition(GetX(), GetZ(), 0.0f))
	{
		short x = 0, z = 0;
		GetStartPosition(x, z);

		m_curx = (float)x;
		m_curz = (float)z;
	}

	AchieveNormalCountAdd(UserAchieveNormalTypes::AchieveBecomeKing, 0, nullptr);

	Packet result(WIZ_MYINFO);

	// Load up our user rankings (for our NP symbols).
	g_pMain->GetUserRank(this);

	// Are we the King? Let's see, shall we?
	CKingSystem * pData = g_pMain->m_KingSystemArray.GetData(GetNation());
	if (pData != nullptr
		&& STRCASECMP(pData->m_strKingName.c_str(), m_strUserID.c_str()) == 0)
		m_bRank = 1; // We're da King, man.
	else
		m_bRank = 0; // totally not da King.

	result.SByte(); // character name has a single byte length
	result << GetSocketID() 
		<< GetName() 
		<< GetSPosX() 
		<< GetSPosZ() 
		<< GetSPosY() 
		<< GetNation()
		<< m_bRace 
		<< m_sClass 
		<< m_bFace	
		<< m_nHair
		<< m_bRank 
		<< m_bTitle 
		<< m_bIsHidingHelmet
		<< m_bIsHidingCospre 
		<< GetLevel() 
		<< m_sPoints 
		<< m_iMaxExp 
		<< m_iExp 
		<< GetLoyalty()
		<< GetMonthlyLoyalty()
		<< GetClanID()
		<< GetFame();

	if (isInClan())
		pKnights = g_pMain->GetClanPtr(GetClanID());

	if (pKnights == nullptr)
	{
		if (isKing())
			result << uint64(0) 
			<< uint16(KNIGHTS_KNIG_CAPE) 
			<< uint32(0);
		else
			result << uint64(0)
			<< uint16(-1)
			<< uint32(0);

		/*result << uint64(0)
		<< uint16(GetNation() == KARUS ? 97 : 98)
		<< uint32(0);*/ // Böyle kullanýrsan karakterler giriþte KRAL pelerini ile baþlar.
	}
	else
	{
		result << pKnights->GetAllianceID()	
			<< pKnights->m_byFlag 
			<< pKnights->GetName() 
			<< pKnights->m_byGrade 
			<< pKnights->m_byRanking 
			<< pKnights->m_sMarkVersion;

		CKnights * pMainClan = g_pMain->GetClanPtr(pKnights->GetAllianceID());
		_KNIGHTS_ALLIANCE* pAlliance = g_pMain->GetAlliancePtr(pKnights->GetAllianceID());

		if (pKnights->isInAlliance() 
			&& pMainClan != nullptr 
			&& pAlliance != nullptr)
		{
			if (isKing())
				result << uint16(KNIGHTS_KNIG_CAPE) 
				<< uint32(0);
			else
			{
				_KNIGHTS_CASTELLAN_CAPE * pCastellanInfo = g_pMain->m_KnightCastellanCapeArray.GetData(pMainClan->GetID());
				if (pCastellanInfo != nullptr 
					&& pCastellanInfo->sRemainingTime >= (uint64)UNIXTIME)
				{
					if (pAlliance->sMainAllianceKnights == pKnights->GetID())
						result << pCastellanInfo->sCape 
						<< pCastellanInfo->sR 
						<< pCastellanInfo->sG 
						<< pCastellanInfo->sB 
						<< ((pKnights->m_byFlag > 1 && pKnights->m_byGrade < 3) ? uint8(1) : uint8(0));
					else if (pAlliance->sSubAllianceKnights == pKnights->GetID())
						result << pCastellanInfo->sCape 
						<< pKnights->m_bCapeR 
						<< pKnights->m_bCapeG 
						<< pKnights->m_bCapeB 
						<< ((pKnights->m_byFlag > 1 
							&& pKnights->m_byGrade < 3) ? uint8(1) : uint8(0));
					else if (pAlliance->sMercenaryClan_1 == pKnights->GetID() 
						|| pAlliance->sMercenaryClan_2 == pKnights->GetID())
						result << pCastellanInfo->sCape 
						<< uint32(0); // only the cape will be present
				}
				else
				{
					if (pAlliance->sMainAllianceKnights == pKnights->GetID())
						result << pMainClan->GetCapeID() 
						<< pMainClan->m_bCapeR 
						<< pMainClan->m_bCapeG 
						<< pMainClan->m_bCapeB 
						<< ((pKnights->m_byFlag > 1 
							&& pKnights->m_byGrade < 3) ? uint8(1) : uint8(0));
					else if (pAlliance->sSubAllianceKnights == pKnights->GetID())
						result << pMainClan->GetCapeID() 
						<< pKnights->m_bCapeR 
						<< pKnights->m_bCapeG 
						<< pKnights->m_bCapeB 
						<< ((pKnights->m_byFlag > 1 
							&& pKnights->m_byGrade < 3) ? uint8(1) : uint8(0));
					else if (pAlliance->sMercenaryClan_1 == pKnights->GetID() 
						|| pAlliance->sMercenaryClan_2 == pKnights->GetID())
						result << pMainClan->GetCapeID() 
						<< uint32(0); // only the cape will be present
				}
			}
		}
		else
		{
			pKnights->OnLogin(this);
			if (isKing())
				result << uint16(KNIGHTS_KNIG_CAPE) 
				<< uint32(0);
			else
			{
				_KNIGHTS_CASTELLAN_CAPE * pCastellanInfo = g_pMain->m_KnightCastellanCapeArray.GetData(pKnights->GetID());
				if (pCastellanInfo != nullptr 
					&& pCastellanInfo->sRemainingTime >= (uint64)UNIXTIME)
					result << pCastellanInfo->sCape 
					<< pCastellanInfo->sR 
					<< pCastellanInfo->sG 
					<< pCastellanInfo->sB 
					<< ((pKnights->m_byFlag > 1 
						&& pKnights->m_byGrade < 3) ? uint8(1) : uint8(0));
				else
					result << pKnights->GetCapeID()	
					<< pKnights->m_bCapeR 
					<< pKnights->m_bCapeG 
					<< pKnights->m_bCapeB 
					<< ((pKnights->m_byFlag > 1 
						&& pKnights->m_byGrade < 3) ? uint8(1) : uint8(0));
			}
		}
	}
	result << uint8(0) << uint8(0) << uint8(0) << uint8(0) // unknown
		<< m_MaxHp << m_sHp
		<< m_MaxMp << m_sMp
		<< m_sMaxWeight << m_sItemWeight
		<< GetStat(STAT_STR) << uint8(GetStatItemBonus(STAT_STR))
		<< GetStat(STAT_STA) << uint8(GetStatItemBonus(STAT_STA))
		<< GetStat(STAT_DEX) << uint8(GetStatItemBonus(STAT_DEX))
		<< GetStat(STAT_INT) << uint8(GetStatItemBonus(STAT_INT))
		<< GetStat(STAT_CHA) << uint8(GetStatItemBonus(STAT_CHA))
		<< m_sTotalHit << m_sTotalAc
		<< uint8(m_sFireR) << uint8(m_sColdR) << uint8(m_sLightningR)
		<< uint8(m_sMagicR) << uint8(m_sDiseaseR) << uint8(m_sPoisonR)
		<< m_iGold
		<< m_bAuthority
		<< (m_bKnightsRank <= m_bPersonalRank ? m_bKnightsRank : int8(-1))		// national rank, leader rank
		<< (m_bPersonalRank <= m_bKnightsRank ? m_bPersonalRank : int8(-1));	// national rank, leader rank

	result.append(m_bstrSkill, 9);

	_ITEM_DATA *pItem = nullptr;
	for (int i = 0; i < INVENTORY_TOTAL; i++)
	{
		if (GetZoneID() == ZONE_KNIGHT_ROYALE)
			pItem = GetKnightRoyaleItem(i);
		else
			pItem = GetItem(i);

		if (pItem == nullptr)
			continue;

		result << pItem->nNum
			<< pItem->sDuration
			<< pItem->sCount
			<< pItem->bFlag
			<< pItem->sRemainingRentalTime;	// remaining time

		_ITEM_TABLE* pItemTable = g_pMain->GetItemPtr(pItem->nNum);
		if (pItemTable != nullptr)
		{
			if (pItemTable->isPetItem())
				ShowPetItemInfo(result, pItem->nSerialNum);
			else if (pItemTable->GetNum() == ITEM_CYPHER_RING)
				ShowCyperRingItemInfo(result, pItem->nSerialNum);
			else
				result << uint32(0); // Item Unique ID
		}
		else
			result << uint32(0); // Item Unique ID*/

		result << pItem->nExpirationTime; // expiration date in unix time
	}
	result.SByte();
	m_bIsChicken = CheckExistEvent(50, 1);
	result << uint8(m_bAccountStatus)	// account status (0 = none, 1 = normal prem with expiry in hours, 2 = pc room)
		<< uint8(m_PremiumMap.GetSize());

	foreach_stlmap(itr, m_PremiumMap)
	{
		_PREMIUM_DATA * pPreData = itr->second;
		if (pPreData == nullptr
			|| pPreData->iPremiumTime == 0)
			continue;

		uint32 TimeRest;
		uint16 TimeShow;
		TimeRest = uint32(pPreData->iPremiumTime - UNIXTIME);

		if (TimeRest >= 1 && TimeRest <= 3600)
			TimeShow = 1;
		else
			TimeShow = TimeRest / 3600;

		result << pPreData->bPremiumType
			<< TimeShow;
	}
	result << m_bPremiumInUse
		<< uint8(m_bIsChicken)	// chicken/beginner flag
		<< m_iMannerPoint
		<< g_pMain->m_sKarusMilitary
		<< g_pMain->m_sHumanMilitary
		<< g_pMain->m_sKarusEslantMilitary
		<< g_pMain->m_sHumanEslantMilitary
		<< g_pMain->m_sMoradonMilitary
		<< uint8(0)
		<< uint16(GetGenieTime())
		<< GetRebirthLevel()
		<< GetRebStatBuff(STAT_STR)
		<< GetRebStatBuff(STAT_STA)
		<< GetRebStatBuff(STAT_DEX)
		<< GetRebStatBuff(STAT_INT)
		<< GetRebStatBuff(STAT_CHA)
		<< uint64(m_iSealedExp)
		<< uint16(m_sCoverTitle)
		<< uint16(m_sSkillTitle)
		<< ReturnSymbolisOK
		<< m_bIsHidingWings;

	SendCompressed(&result);
	g_pMain->AddCharacterName(this);
	SetZoneAbilityChange(GetZoneID());
	HOOK_HpInfo();
}

void CUser::GetUserInfo(Packet & pkt)
{
	pkt.SByte();
	pkt << GetName() << GetNation();
	pkt << uint8(0) << uint8(0);
	pkt << GetClanID() << uint8(isInPKZone() == true ? uint8(0) : GetFame());

	CKnights * pKnights = g_pMain->GetClanPtr(GetClanID());
	if (pKnights == nullptr)
	{
		if (isKing())
			pkt << uint32(0) << uint16(0) << uint8(0) << uint16(KNIGHTS_KNIG_CAPE) << uint32(0) << uint8(0);
		else
			pkt << uint32(0) << uint16(0) << uint8(0) << uint16(-1) << uint32(0) << uint8(0);
			//pkt << uint32(0) << uint16(0) << uint8(0) << uint16(GetNation() == KARUS ? 97 : 98) << uint32(0) << uint8(0); /* Kullanýcýlar Kral Pelerini Görsün */
	}
	else
	{
		pkt << pKnights->GetAllianceID() << pKnights->GetName() << pKnights->m_byGrade << pKnights->m_byRanking << pKnights->m_sMarkVersion; // symbol/mark version

		CKnights * pMainClan = g_pMain->GetClanPtr(pKnights->GetAllianceID());
		_KNIGHTS_ALLIANCE* pAlliance = g_pMain->GetAlliancePtr(pKnights->GetAllianceID());

		if (pKnights->isInAlliance() && pMainClan != nullptr && pAlliance != nullptr)
		{
			if (!isKing())
			{
				_KNIGHTS_CASTELLAN_CAPE * pCastellanInfo = g_pMain->m_KnightCastellanCapeArray.GetData(pMainClan->GetID());
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
				_KNIGHTS_CASTELLAN_CAPE * pCastellanInfo = g_pMain->m_KnightCastellanCapeArray.GetData(pKnights->GetID());
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
	_ITEM_DATA *pItem = nullptr;

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
		<< uint16(m_sCoverTitle)
		<< ReturnSymbolisOK // R symbol after name returned?
		<< uint32(0); // face time system
}

void CUser::SendInfo()
{
	DateTime time;
	std::string Zaman;

	if (isGM())
		SendChat(ChatType::PUBLIC_CHAT, "[GM] : You can use +help command to see game master's commands. Have Fun!");

	SendChat(ChatType::GM_CHAT, string_format("%s, Welcome to Knight Online, Enjoy Your Stay.", m_strUserID.c_str()));

	//Packet result;
	/*std::string s_Notice = string_format("Selam %s", GetName().c_str());
	ChatPacket::Construct(&result, ChatType::PRIVATE_CHAT, s_Notice.c_str(), "[DUYURU]", GetNation());
	Send(&result);
	s_Notice.clear();
	s_Notice = string_format("VIP Werehouse Þifreniz.. '1234' ve Item Kitleme Ve Character Seal Þifreleriniz.. '12345678' dir.");
	result.clear();
	ChatPacket::Construct(&result, ChatType::PRIVATE_CHAT, s_Notice.c_str(), "[DUYURU]", GetNation());
	Send(&result);
	s_Notice.clear();
	s_Notice = string_format("Þifreleri deðiþtirmek için KOPanel Hesabýnýza Girerek Deðiþtirebilirsiniz.");
	result.clear();
	ChatPacket::Construct(&result, ChatType::PRIVATE_CHAT, s_Notice.c_str(), "[DUYURU]", GetNation());
	Send(&result);
	s_Notice.clear();*/
	/*std::string s_Notice = string_format("#### This server is protected by current !####");
	result.clear();
	ChatPacket::Construct(&result, ChatType::ALLIANCE_CHAT, s_Notice.c_str(), "[SYSTEM]", GetNation());
	Send(&result);*/
}