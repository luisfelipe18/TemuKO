#include "stdafx.h"
#include "DBAgent.h"

using std::string;

#define ITEM_SEAL_PRICE 1000000

enum
{
	SEAL_TYPE_SEAL		= 1,
	SEAL_TYPE_UNSEAL	= 2,
	SEAL_TYPE_KROWAZ	= 3,
	SEAL_TYPE_OLD_ITEM	= 4
};

enum SealErrorCodes
{
	SealErrorNone			= 0, // no error, success!
	SealNoError				= 1,
	SealErrorFailed			= 2, // "Seal Failed."
	SealErrorNeedCoins		= 3, // "Not enough coins."
	SealErrorInvalidCode	= 4, // "Invalid Citizen Registry Number" (i.e. invalid code/password)
	SealErrorPremiumOnly	= 5, // "Only available to premium users"
	SealErrorFailed2		= 6, // "Seal Failed."
	SealErrorTooSoon		= 7, // "Please try again. You may not repeat this function instantly."
};

/**
* @brief	Packet handler for the item sealing system.
*
* @param	pkt	The packet.
*/
void CUser::ItemSealProcess(Packet & pkt)
{
	// Seal type
	uint8 opcode = pkt.read<uint8>();

	Packet result(WIZ_ITEM_UPGRADE, uint8(ITEM_SEAL));
	result << opcode;

	switch (opcode)
	{
	// Used when sealing an item.
	case SEAL_TYPE_SEAL:
		{
			string strPasswd;
			uint32 nItemID; 
			int16 unk0; // set to -1 in this case
			uint8 bSrcPos, bResponse = SealErrorNone;
			pkt >> unk0 >> nItemID >> bSrcPos >> strPasswd;

			/* 
			Most of these checks are handled client-side, so we shouldn't need to provide error messages.
			Also, item sealing requires certain premium types (gold, platinum, etc) - need to double-check 
			these before implementing this check.
			*/

			// is this a valid position? (need to check if it can be taken from new slots)
			if (bSrcPos >= HAVE_MAX 
				// does the item exist where the client says it does?
					|| GetItem(SLOT_MAX + bSrcPos)->nNum != nItemID
					// i ain't be allowin' no stealth items to be sealed!
					|| GetItem(SLOT_MAX + bSrcPos)->nSerialNum == 0)
					bResponse = SealErrorFailed;
			// is the password valid by client limits?
			else if (strPasswd.empty() || strPasswd.length() > 8)
				bResponse = SealErrorInvalidCode;
			// do we have enough coins?
			else if (!hasCoins(ITEM_SEAL_PRICE))
				bResponse = SealErrorNeedCoins;

			_ITEM_TABLE* pItem = g_pMain->m_ItemtableArray.GetData(nItemID);

			if(pItem == nullptr)
				return;

			// If no error, pass it along to the database.
			if (bResponse == SealErrorNone)
			{
				result << nItemID << bSrcPos << strPasswd << bResponse;
				g_pMain->AddDatabaseRequest(result, this);
			}
			// If there's an error, tell the client.
			// From memory though, there was no need -- it handled all of these conditions itself
			// so there was no need to differentiate (just ignore the packet). Need to check this.
			else 
			{
				result << bResponse;
				Send(&result);
			}
		} break;

	// Used when unsealing an item.
	case SEAL_TYPE_UNSEAL:
		{
			string strPasswd;
			uint32 nItemID; 
			int16 unk0; // set to -1 in this case
			uint8 bSrcPos, bResponse = SealErrorNone;
			pkt >> unk0 >> nItemID >> bSrcPos >> strPasswd;

			if (bSrcPos >= HAVE_MAX
				|| GetItem(SLOT_MAX+bSrcPos)->bFlag != ITEM_FLAG_SEALED
				|| GetItem(SLOT_MAX+bSrcPos)->nNum != nItemID)
				bResponse = SealErrorFailed;
			else if (strPasswd.empty() || strPasswd.length() > 8)
				bResponse = SealErrorInvalidCode;

			// If no error, pass it along to the database.
			if (bResponse == SealErrorNone)
			{
				result << nItemID << bSrcPos << strPasswd << bResponse;
				g_pMain->AddDatabaseRequest(result, this);
			}
			// If there's an error, tell the client.
			// From memory though, there was no need -- it handled all of these conditions itself
			// so there was no need to differentiate (just ignore the packet). Need to check this.
			else
			{
				result << bResponse;
				Send(&result);
			}
		} break;

	// Used when binding a Krowaz item (used to take it from not bound -> bound)
	case SEAL_TYPE_KROWAZ:
		{
			string strPasswd = "0"; //Dummy, not actually used.
			uint32 nItemID;
			uint8 bSrcPos = 0 , unk3, bResponse = SealErrorNone;
			uint16 unk1, unk2;
			pkt >> unk1 >> nItemID >> bSrcPos >> unk3 >> unk2;

			if (bSrcPos >= HAVE_MAX
				|| GetItem(SLOT_MAX+bSrcPos)->nNum != nItemID)
				bResponse = SealErrorFailed;

			if (bResponse == SealErrorNone)
			{
				result << nItemID << bSrcPos << strPasswd << bResponse;
				g_pMain->AddDatabaseRequest(result, this);
			}
		} break;

	// Used when binding a Old item (used to take it from bound -> not bound)
	case SEAL_TYPE_OLD_ITEM:
		{
			string strPasswd, strPasswdA;
			uint32 nItemID;
			int16 unk0;
			uint8 bSrcPos, bResponse = SealErrorNone;
			pkt >> unk0 >> nItemID >> bSrcPos >> strPasswd;

			_ITEM_TABLE * pTable = g_pMain->m_ItemtableArray.GetData(nItemID);
			//g_DBAgent.LoadPasswd(m_strAccountID, strPasswdA);// bunu yanlýþ sildim galiba bu neymis seal key þifresi geri getirirz.

			if (bSrcPos >= HAVE_MAX
				|| GetItem(SLOT_MAX + bSrcPos)->nNum != nItemID)
				bResponse = SealErrorFailed;
			else if (strPasswd.empty() 
				|| strPasswd.length() > 8 
				|| strPasswd != strPasswdA)
				bResponse = SealErrorInvalidCode;
			else if (pTable == nullptr)
				bResponse = SealErrorFailed;//dene
			else if(!CheckExistItem(810890000, pTable->m_Bound))
				bResponse = SealErrorFailed;

			if (bResponse == SealErrorNone)
			{
				RobItem(810890000, pTable->m_Bound);

				result << nItemID << bSrcPos << strPasswd << bResponse;
				g_pMain->AddDatabaseRequest(result, this);
			}
			else
			{
				result << bResponse;
				Send(&result);
			}
		} break;
	}
}

void CUser::SealItem(uint8 bSealType, uint8 bSrcPos)
{
	_ITEM_DATA * pItem = GetItem(SLOT_MAX + bSrcPos);
	if (pItem == nullptr)
		return;

	switch (bSealType)
	{
	case SEAL_TYPE_SEAL:
		pItem->bFlag = ITEM_FLAG_SEALED;
		GoldLose(ITEM_SEAL_PRICE);
		break;

	case SEAL_TYPE_UNSEAL:
		pItem->bFlag = 0;
		break;

	case SEAL_TYPE_KROWAZ:
		pItem->bFlag = ITEM_FLAG_BOUND;
		break;

	case SEAL_TYPE_OLD_ITEM:
		pItem->bFlag = ITEM_FLAG_NOT_BOUND;
		break;
	}
}

/**
* @brief	Packet handler for the character sealing system.
*
* @param	pkt	The packet.
*/
void CUser::CharacterSealProcess(Packet & pkt)
{
	uint8 bOpcode = pkt.read<uint8>();
	
	switch (bOpcode)
	{
	case CharacterSealOpcodes::ShowList:
		CharacterSealShowList(pkt);
		break;
	case CharacterSealOpcodes::UseScroll:
		CharacterSealUseScroll(pkt);
		break;
	case CharacterSealOpcodes::UseRing:
		CharacterSealUseRing(pkt);
		break;
	case CharacterSealOpcodes::Preview:
		CharacterSealPreview(pkt);
		break;
	case CharacterSealOpcodes::AchieveList:
		CharacterSealAchieveList(pkt);
		break;
	default:
		TRACE("Unhandled Character Seal Opcode: %X\n", bOpcode);
		printf("Unhandled Character Seal Opcode: %X\n", bOpcode);
		break;

	}
}

void CUser::CharacterSealShowList(Packet & pkt)
{
	if (!CheckExistItem(ITEM_SEAL_SCROLL) && !CheckExistItem(ITEM_CYPHER_RING))
		return;

	Packet result(WIZ_ITEM_UPGRADE, uint8(ITEM_CHARACTER_SEAL));
	result << uint8(CharacterSealOpcodes::ShowList);
	g_pMain->AddDatabaseRequest(result, this);
}

void CUser::CharacterSealUseScroll(Packet & pkt)
{
	uint16 unknow;
	uint32 nItemID;
	string userName, strPasswd;
	uint8 bSrcPos = 0, bResponse = SealErrorNone;
	pkt >> unknow >> bSrcPos >> nItemID >> userName >> strPasswd;

	Packet result(WIZ_ITEM_UPGRADE, uint8(ITEM_CHARACTER_SEAL));

	if (STRCASECMP(userName.c_str(), GetName().c_str()) == 0)
	{
		result << uint8(CharacterSealOpcodes::UseScroll) << uint16(0);
		Send(&result);
		return;
	}

	if (bSrcPos >= HAVE_MAX
		|| GetItem(SLOT_MAX + bSrcPos) == nullptr
		|| GetItem(SLOT_MAX + bSrcPos)->bFlag != ITEM_FLAG_NONE
		|| GetItem(SLOT_MAX + bSrcPos)->nNum != ITEM_SEAL_SCROLL)
	{
		result << uint8(CharacterSealOpcodes::UseScroll) << uint16(0);
		Send(&result);
		return;
	}

	int8 ItemCheckResult = g_DBAgent.CharacterSealItemCheck(userName);
	if (ItemCheckResult != 1)
	{
		result << uint8(CharacterSealOpcodes::UseScroll) << uint16(0);
		Send(&result);
		return;
	}

	result << uint8(CharacterSealOpcodes::UseScroll) << bSrcPos << userName << strPasswd;
	g_pMain->AddDatabaseRequest(result, this);
}

void CUser::CharacterSealPreview(Packet & pkt)
{
	Packet result(WIZ_ITEM_UPGRADE, uint8(ITEM_CHARACTER_SEAL));
	uint32 nUniqueID = 0;
	pkt >> nUniqueID;

	_CHARACTER_SEAL_ITEM_MAPPING * pDataMapping = g_pMain->m_CharacterSealItemMapping.GetData(nUniqueID);
	if (pDataMapping == nullptr)
	{
		result << uint8(CharacterSealOpcodes::Preview) << uint8(2);
		Send(&result);
		return;
	}

	bool isExist = false;
	for (int i = 0; i < INVENTORY_TOTAL; i++)
	{
		_ITEM_DATA *pItem = GetItem(i);
		if (pItem == nullptr)
			continue;

		if (pItem->nSerialNum == pDataMapping->nItemSerial)
		{
			isExist = true;
			break;
		}
	}

	if (isExist == false)
	{
		result << uint8(CharacterSealOpcodes::Preview) << uint8(2);
		Send(&result);
		return;
	}

	_CHARACTER_SEAL_ITEM * pData = g_pMain->m_CharacterSealItemArray.GetData(pDataMapping->nItemSerial);
	if (pData == nullptr)
	{
		result << uint8(CharacterSealOpcodes::Preview) << uint8(2);
		Send(&result);
		return;
	}

	result.SByte();
	result << uint8(CharacterSealOpcodes::Preview) << uint8(1)
		<< pData->GetName()
		<< uint8(1)
		<< pData->GetRace()
		<< pData->GetClass()
		<< pData->GetLevel()
		<< pData->m_iLoyalty
		<< pData->m_bStats[STAT_STR]
		<< pData->m_bStats[STAT_STA]
		<< pData->m_bStats[STAT_DEX]
		<< pData->m_bStats[STAT_INT]
		<< pData->m_bStats[STAT_CHA]
		<< pData->m_iGold // Weird Sending this here hm
		<< pData->m_bstrSkill[SkillPointFree]
		<< uint32(1) // -> Reading Which Skill has how many pointd
		<< pData->m_bstrSkill[SkillPointCat1]
		<< pData->m_bstrSkill[SkillPointCat2]
		<< pData->m_bstrSkill[SkillPointCat3]
		<< pData->m_bstrSkill[SkillPointMaster];

	/*Time to Send the inventory Itemz and the Equipped itemz ??*/
	for (int i = 0; i < INVENTORY_COSP; i++)
	{
		_ITEM_DATA *pItem = pData->GetItem(i);
		if (pItem == nullptr)
			continue;

		result << pItem->nNum
			<< pItem->sDuration
			<< pItem->sCount
			<< pItem->bFlag;// item type flag (e.g. rented)
	}
	Send(&result);
}

void CUser::CharacterSealAchieveList(Packet & pkt)
{
	uint8 test;
	pkt >> test;
	uint32 sCount = 0;

	Packet result(WIZ_ITEM_UPGRADE, uint8(ITEM_CHARACTER_SEAL));
	result << uint8(CharacterSealOpcodes::AchieveList) << uint8(1);
	//size_t wpos = result.wpos();
	result << sCount;
	foreach_stlmap(itr, m_AchieveMap)
	{
		_USER_ACHIEVE_INFO* pAchieveData = itr->second;
		if (pAchieveData == nullptr
			|| (pAchieveData->bStatus != UserAchieveStatus::AchieveFinished
				&& pAchieveData->bStatus != UserAchieveStatus::AchieveCompleted))
			continue;

		_ACHIEVE_MAIN *pAchieveMain = g_pMain->m_AchieveMainArray.GetData(itr->first);
		if (pAchieveMain == nullptr
			|| pAchieveMain->TitleID == 0)
			continue;

		result << pAchieveMain->sIndex;
		sCount++;
	}
	result.put(4, sCount);
	Send(&result);
}

void CUser::CharacterSealUseRing(Packet & pkt)
{
	uint16 unknow;
	uint32 nItemID;
	uint8 bSrcPos = 0, bSelectedCharIndex;
	pkt >> unknow >> bSrcPos >> nItemID >> bSelectedCharIndex;


	if (bSrcPos >= HAVE_MAX
		|| nItemID != ITEM_CYPHER_RING
		|| GetItem(SLOT_MAX + bSrcPos) == nullptr
		|| GetItem(SLOT_MAX + bSrcPos)->bFlag != ITEM_FLAG_NONE
		|| GetItem(SLOT_MAX + bSrcPos)->nNum != ITEM_CYPHER_RING
		|| GetItem(SLOT_MAX + bSrcPos)->nNum != nItemID
		|| bSelectedCharIndex >= 4
		|| bSelectedCharIndex < 0)
		return;

	Packet result(WIZ_ITEM_UPGRADE, uint8(ITEM_CHARACTER_SEAL));
	result << uint8(CharacterSealOpcodes::UseRing) << bSrcPos << bSelectedCharIndex;
	g_pMain->AddDatabaseRequest(result, this);
}

#pragma region Item Upgrade Related Database Methods

void CUser::ReqSealItem(Packet & pkt)
{
	uint8 bSrcPos, bSealType, bSealResult;
	uint32 nItemID;
	uint64 nItemSerial;
	string strSealPasswd;

	pkt >> bSealType >> nItemID >> bSrcPos >> strSealPasswd >> bSealResult;
	_ITEM_DATA *pItem = GetItem(SLOT_MAX + bSrcPos);
	if (pItem == nullptr)
		return;

	nItemSerial = pItem->nSerialNum;

	if (!bSealResult)
		bSealResult = g_DBAgent.SealItem(strSealPasswd, nItemSerial, nItemID, bSealType, this);

	Packet result(WIZ_ITEM_UPGRADE, uint8(ITEM_SEAL));
	result << bSealType << bSealResult << nItemID << bSrcPos;
	Send(&result);

	if (bSealResult == 1)
		SealItem(bSealType, bSrcPos);
}

void CUser::ReqCharacterSealProcess(Packet & pkt)
{
	if (isTrading()
		|| isMining()
		|| isFishing()
		|| isSellingMerchantingPreparing()
		|| isBuyingMerchantingPreparing()
		|| isMerchanting()
		|| !isInGame())
		return;

	uint8 bOpcode = pkt.read<uint8>();

	switch (CharacterSealOpcodes(bOpcode))
	{
	case ShowList:
		ReqCharacterSealShowList(pkt);
		break;
	case UseScroll:
		ReqCharacterSealUseScroll(pkt);
		break;
	case UseRing:
		ReqCharacterSealUseRing(pkt);
		break;
	case Preview:
		break;
	default:
		printf("Character Seal Unhandled packet %d \n", bOpcode);
		TRACE("Character Seal Unhandled packet %d \n", bOpcode);
		break;
	}
}

void CUser::ReqCharacterSealShowList(Packet & pkt)
{
	Packet result(WIZ_ITEM_UPGRADE, uint8(ITEM_CHARACTER_SEAL));

	if ((!CheckExistItem(ITEM_SEAL_SCROLL) && !CheckExistItem(ITEM_CYPHER_RING))
		|| GetAccountName().length() == 0)
	{
		result << uint8(CharacterSealOpcodes::UseScroll) << uint16(0);
		Send(&result);
		return;
	}

	result << uint8(CharacterSealOpcodes::ShowList) << uint8(1);
	string strCharID1, strCharID2, strCharID3, strCharID4;

	g_DBAgent.GetAllCharID(m_strAccountID, strCharID1, strCharID2, strCharID3, strCharID4);
	g_DBAgent.LoadCharInfo(strCharID1, result);
	g_DBAgent.LoadCharInfo(strCharID2, result);
	g_DBAgent.LoadCharInfo(strCharID3, result);
	g_DBAgent.LoadCharInfo(strCharID4, result);
	Send(&result);
}

void CUser::ReqCharacterSealUseScroll(Packet & pkt)
{
	Packet result(WIZ_ITEM_UPGRADE, uint8(ITEM_CHARACTER_SEAL));

	if (!CheckExistItem(ITEM_SEAL_SCROLL)
		|| GetAccountName().length() == 0)
	{
		result << uint8(CharacterSealOpcodes::UseScroll) << uint16(0);
		Send(&result);
		return;
	}

	uint8 bSrcPos = 0;
	string strRequestedChar, strPassword, strCharID[4];
	
	result << uint8(CharacterSealOpcodes::UseScroll);

	pkt >> bSrcPos >> strRequestedChar >> strPassword;

	if (STRCASECMP(strRequestedChar.c_str(), GetName().c_str()) == 0)
	{
		result << uint16(0);
		Send(&result);
		return;
	}

	g_DBAgent.GetAllCharID(m_strAccountID, strCharID[0], strCharID[1], strCharID[2], strCharID[3]);

	uint8 bCharRequested = 5;
	if (STRCASECMP(strRequestedChar.c_str(), strCharID[0].c_str()) == 0)
		bCharRequested = 0;
	else if (STRCASECMP(strRequestedChar.c_str(), strCharID[1].c_str()) == 0)
		bCharRequested = 1;
	else if (STRCASECMP(strRequestedChar.c_str(), strCharID[2].c_str()) == 0)
		bCharRequested = 2;
	else if (STRCASECMP(strRequestedChar.c_str(), strCharID[3].c_str()) == 0)
		bCharRequested = 3;

	if (bCharRequested == 5)
	{
		result << uint16(0);
		Send(&result);
		return;
	}

	_ITEM_DATA * pSrcItem = GetItem(SLOT_MAX + bSrcPos);
	if (pSrcItem == nullptr
		|| pSrcItem->bFlag != ITEM_FLAG_NONE
		|| pSrcItem->nNum != ITEM_SEAL_SCROLL
		|| pSrcItem->isRented()
		|| pSrcItem->isDuplicate())
	{
		result << uint16(0);
		Send(&result);
		return;
	}

	uint64 nItemSerial = pSrcItem->nSerialNum;

	int8 bResult = g_DBAgent.CharacterSealProcess(GetAccountName(), strCharID[bCharRequested], strPassword, nItemSerial);
	if (bResult != 1)
	{
		result << uint16(bResult) << pSrcItem->nNum << bSrcPos;
		Send(&result);
		return;
	}

	_CHARACTER_SEAL_ITEM *pData = new _CHARACTER_SEAL_ITEM;
	pData->nSerialNum = nItemSerial;
	pData->m_strUserID = strCharID[bCharRequested];
	pData->strAccountID = GetAccountName();

	if (!g_DBAgent.LoadCharacterSealUserData(pData->strAccountID, pData->m_strUserID, pData))
	{
		delete pData;
		result << uint16(0);
		Send(&result);
		return;
	}

	if (!g_pMain->m_CharacterSealItemArray.PutData(pData->nSerialNum, pData))
	{
		delete pData;
		result << uint16(0);
		Send(&result);
		return;
	}

	_CHARACTER_SEAL_ITEM_MAPPING *pDataMap = new _CHARACTER_SEAL_ITEM_MAPPING;
	pDataMap->nItemSerial = nItemSerial;
	pDataMap->nUniqueID = pData->nUniqueID;

	if (!g_pMain->m_CharacterSealItemMapping.PutData(pData->nUniqueID, pDataMap))
	{
		delete pData;
		delete pDataMap;
		result << uint16(0);
		Send(&result);
		return;
	}

	pSrcItem->nNum = ITEM_CYPHER_RING;
	SetUserAbility(false);
	SendItemWeight();

	result << uint8(1) << uint8(bSrcPos) << uint32(ITEM_CYPHER_RING)
		<< uint32(pData->nUniqueID)
		<< pData->GetName()
		<< uint8(pData->GetClass())
		<< pData->GetLevel()
		<< uint16(0)
		<< pData->GetRace() << uint8(0);
	Send(&result);
}

void CUser::ReqCharacterSealUseRing(Packet & pkt)
{

	if (!CheckExistItem(ITEM_CYPHER_RING)
		|| GetAccountName().length() == 0)
		return;

	uint8 bSrcPos = 0, bSelectedCharIndex;
	string strCharID[4];
	Packet result(WIZ_ITEM_UPGRADE, uint8(ITEM_CHARACTER_SEAL));
	result << uint8(CharacterSealOpcodes::UseRing);

	pkt >> bSrcPos >> bSelectedCharIndex;

	_ITEM_DATA * pSrcItem = GetItem(SLOT_MAX + bSrcPos);
	if (pSrcItem == nullptr
		|| pSrcItem->bFlag != ITEM_FLAG_NONE
		|| pSrcItem->nNum != ITEM_CYPHER_RING
		|| pSrcItem->isRented()
		|| pSrcItem->isDuplicate())
		return;

	uint64 nItemSerial = pSrcItem->nSerialNum;

	_CHARACTER_SEAL_ITEM * pData = g_pMain->m_CharacterSealItemArray.GetData(nItemSerial);
	if (pData == nullptr)
		return;

	if ((pData->GetClass() / 100) != GetNation())
	{
		result << uint16(0);
		Send(&result);
		return;
	}

	g_DBAgent.GetAllCharID(m_strAccountID, strCharID[0], strCharID[1], strCharID[2], strCharID[3]);

	if (strCharID[bSelectedCharIndex].length() != 0)
	{
		result << uint16(3);
		Send(&result);
		return;
	}

	uint32 nItemUniqueID = pData->nUniqueID;

	int8 bResult = g_DBAgent.CharacterUnSealProcess(GetAccountName(), bSelectedCharIndex, nItemSerial);
	if (bResult != 1)
	{
		result << uint8(0) << bSrcPos << pSrcItem->nNum;
		Send(&result);
		return;
	}

	g_pMain->m_CharacterSealItemArray.DeleteData(nItemSerial);
	g_pMain->m_CharacterSealItemMapping.DeleteData(nItemUniqueID);

	memset(pSrcItem, 0, sizeof(pSrcItem));
	SetUserAbility(false);
	SendItemWeight();

	result << uint8(bResult) << bSrcPos << uint32(0);
	Send(&result);
}
#pragma endregion

void CUser::ShowCyperRingItemInfo(Packet &pkt, uint64 nSerialNum)
{
	_CHARACTER_SEAL_ITEM *pCharSealItem = g_pMain->m_CharacterSealItemArray.GetData(nSerialNum);
	if (pCharSealItem != nullptr)
	{
		int64 ExpRate = 0;
		ExpRate = ((pCharSealItem->m_iExp * 50) / (g_pMain->GetExpByLevel(pCharSealItem->GetLevel(), GetRebirthLevel())) * 100);

		if (ExpRate > 10000)
			ExpRate = 10000;

		if (ExpRate < 0)
			ExpRate = 0;

		pkt.DByte();
		pkt << uint32(pCharSealItem->nUniqueID)
			<< pCharSealItem->GetName()
			<< uint8(pCharSealItem->GetClass())
			<< pCharSealItem->GetLevel()
			<< uint16(ExpRate)
			<< pCharSealItem->GetRace() << uint8(0);
	}
	else
		pkt << uint32(0);
}