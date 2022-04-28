#include "stdafx.h"
#include "../shared/database/OdbcConnection.h"
#include "KnightsManager.h"
#include "DBAgent.h"
#include "../shared/DateTime.h"

CDBAgent g_DBAgent;

using std::string;
using std::unique_ptr;

CDBAgent::CDBAgent()
{
	m_GameDB = new OdbcConnection();
	m_AccountDB = new OdbcConnection();
}

CDBAgent::~CDBAgent()
{
	delete m_GameDB;
	delete m_AccountDB;
}
bool CDBAgent::Startup(bool bMarsEnabled, 
					   tstring & strAccountDSN, tstring & strAccountUID, tstring & strAccountPWD,
					   tstring & strGameDSN, tstring & strGameUID, tstring & strGamePWD)
{
	if (!Connect(bMarsEnabled,
		strAccountDSN, strAccountUID, strAccountPWD,
		strGameDSN, strGameUID, strGamePWD))
	{
		// we should probably be a little more specific (i.e. *which* database server)
		printf(_T("ERROR: Failed to connect to the database server.\n"));
		return false;
	}

	DatabaseThread::Startup();

	return true;
}

bool CDBAgent::Connect(bool bMarsEnabled, 
					   tstring & strAccountDSN, tstring & strAccountUID, tstring & strAccountPWD,
					   tstring & strGameDSN, tstring & strGameUID, tstring & strGamePWD)
{
	if (!m_AccountDB->Connect(strAccountDSN, strAccountUID, strAccountPWD, bMarsEnabled))
	{
		ReportSQLError(m_AccountDB->GetError());
		return false;
	}

	if (!m_GameDB->Connect(strGameDSN, strGameUID, strGamePWD, bMarsEnabled))
	{
		ReportSQLError(m_GameDB->GetError());
		return false;
	}

	return true;
}

void CDBAgent::ReportSQLError(OdbcError *pError)
{
	if (pError == nullptr)
		return;

	DateTime time;

	// This is *very* temporary.
	string errorMessage = string_format(_T("[ ODBC Error - %d.%d.%d %d:%d:%d ] ] Source: %s Error: %s Description: %s\n"),
		time.GetDay(),time.GetMonth(),time.GetYear(),time.GetHour(),time.GetMinute(),time.GetSecond(),
		pError->Source.c_str(), pError->ExtendedErrorMessage.c_str(), pError->ErrorMessage.c_str());

	Guard lock(m_lock);
	FILE *fp = fopen("./Logs/GameServer.log", "a");
	if (fp != nullptr)
	{
		fwrite(errorMessage.c_str(), errorMessage.length(), 1, fp);
		fclose(fp);
	}

	TRACE("Database error: %s\n", errorMessage.c_str());
	delete pError;
}

int8 CDBAgent::AccountLogin(string & strAccountID, string & strPasswd)
{
	int8 bRet = -1;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return bRet;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, strPasswd.c_str(), strPasswd.length());

	if (!dbCommand->Execute(_T("{? = CALL GAME_LOGIN_V2(?, ?)}")))
		ReportSQLError(m_GameDB->GetError());

	return bRet;
}

uint8 CDBAgent::NationSelect(string & strAccountID, uint8 bNation)
{
	uint8 bRet = 0;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());

	if (!dbCommand->Execute(string_format(_T("{? = CALL NATION_SELECT(?, %d)}"), bNation)))
		ReportSQLError(m_GameDB->GetError());

	return (bRet > 0 ? bNation : 0);
}

#pragma region AccountAddNPoints(std::string & strAccountID, uint16 sNPointAmount)
int8 CDBAgent::AccountAddNPoints(std::string & strAccountID, uint16 sNPointAmount)
{
	uint8 bRet = 0;
	unique_ptr<OdbcCommand> dbCommand(m_AccountDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());

	if (!dbCommand->Execute(string_format(_T("{? = CALL UPDATE_KNIGHT_CASH_ADD(?, %d)}"), sNPointAmount)))
		ReportSQLError(m_AccountDB->GetError());

	return bRet;
}
#pragma endregion

#pragma region CDBAgent::GetAllCharID(string & strAccountID, string & strCharID1, string & strCharID2, string & strCharID3, string & strCharID4)
bool CDBAgent::GetAllCharID(string & strAccountID, string & strCharID1, string & strCharID2, string & strCharID3, string & strCharID4)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	int8 bRet = -2; // generic error

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());
	if (!dbCommand->Execute(_T("{? = CALL GET_ALL_CHARID(?)}")))
	{
		ReportSQLError(m_GameDB->GetError());
		return false;
	}

	if (!dbCommand->hasData())
		return false;

	dbCommand->FetchString(1, strCharID1);
	dbCommand->FetchString(2, strCharID2);
	dbCommand->FetchString(3, strCharID3);
	dbCommand->FetchString(4, strCharID4);

	return true;
}
#pragma endregion

void CDBAgent::LoadCharInfo(string & strCharID, ByteBuffer & result)
{
	uint32 nHair = 0;
	uint16 sClass = 0;
	uint8 bRace = 0, bLevel = 0, bFace = 0, bZone = 0; 
	char strItem[INVENTORY_TOTAL * 8];
	char strItemExpiration[INVENTORY_TOTAL * 4];
	ByteBuffer itemData;

	// ensure it's all 0'd out initially.
	memset(strItem, 0x00, sizeof(strItem));
	memset(strItemExpiration, 0x00, sizeof(strItemExpiration));

	if (!strCharID.empty())
	{
		unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
		if (dbCommand.get() == nullptr)
			return;

		dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());

		if (!dbCommand->Execute(_T("{CALL LOAD_CHAR_INFO(?)}")))
			ReportSQLError(m_GameDB->GetError());

		if (dbCommand->hasData())
		{
			dbCommand->FetchByte(1, bRace);
			dbCommand->FetchUInt16(2, sClass);
			dbCommand->FetchUInt32(3, nHair);
			dbCommand->FetchByte(4, bLevel);
			dbCommand->FetchByte(5, bFace);
			dbCommand->FetchByte(6, bZone);
			dbCommand->FetchBinary(7, strItem, sizeof(strItem));
			dbCommand->FetchBinary(8, strItemExpiration, sizeof(strItemExpiration));
		}
	}

	itemData.append(strItem, sizeof(strItem));

	result	<< strCharID << bRace << sClass << bLevel << bFace << nHair << bZone;
	for (int i = 0; i < SLOT_MAX; i++) 
	{
		uint32 nItemID;
		uint16 sDurability, sCount;
		itemData >> nItemID >> sDurability >> sCount;
		if (i == HEAD || i == BREAST || i == SHOULDER || i == LEG || i == GLOVE || i == FOOT || i == RIGHTHAND || i == LEFTHAND)
			result << nItemID << sDurability;
	}
}

int8 CDBAgent::CreateNewChar(string & strAccountID, int index, string & strCharID, uint8 bRace, uint16 sClass, uint32 nHair, uint8 bFace, uint8 bStr, uint8 bSta, uint8 bDex, uint8 bInt, uint8 bCha)
{
	int8 bRet = -1;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return bRet;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());

	if (!dbCommand->Execute(string_format(_T("{? = CALL CREATE_NEW_CHAR(?, %d, ?, %d, %d, %d, %d, %d, %d, %d, %d, %d)}"), 
		index, bRace, sClass, nHair, bFace, bStr, bSta, bDex, bInt, bCha)))
		ReportSQLError(m_GameDB->GetError());

	return bRet;
}

void CDBAgent::LoadNewCharSet(std::string& strCharID, uint16 sClass)
{
	int8 bRet = -1;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());
	if (!dbCommand->Execute(string_format(_T("{CALL [LOAD_NEW_CHAR_SET](?,%d)}"), sClass)))
		ReportSQLError(m_GameDB->GetError());
}

void CDBAgent::LoadNewCharValue(std::string& strCharID, uint16 sClass)
{
	int8 bRet = -1;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());
	if (!dbCommand->Execute(string_format(_T("{CALL [LOAD_NEW_CHAR_VALUE](?,%d, %d)}"), sClass, g_pMain->m_nServerNo)))
		ReportSQLError(m_GameDB->GetError());
}

int8 CDBAgent::ChangeHair(std::string & strAccountID, std::string & strCharID, uint8 bOpcode, uint8 bFace, uint32 nHair)
{
	int8 bRet = 1; // failed
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return bRet;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());

	if (!dbCommand->Execute(string_format(_T("{? = CALL CHANGE_HAIR(?, ?, %d, %d, %d)}"), 
		bOpcode, bFace, nHair)))
		ReportSQLError(m_GameDB->GetError());

	return bRet;
}

int8 CDBAgent::DeleteChar(string & strAccountID, int index, string & strCharID, string & strSocNo)
{
	int8 bRet = -2; // generic error
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return bRet;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, strSocNo.c_str(), strSocNo.length());

	if (!dbCommand->Execute(string_format(_T("{? = CALL DELETE_CHAR(?, %d, ?, ?)}"), index)))
		ReportSQLError(m_GameDB->GetError());

	return bRet;
}

#pragma region CDBAgent::VIPStorageSetPassword(std::string &strSealPasswd, std::string & strAccountID, uint8 KeyCancel)
uint8 CDBAgent::VIPStorageSetPassword(std::string &strSealPasswd, std::string & strAccountID, uint8 KeyCancel)
{
	uint8 bRet = 1;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return 3;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, strSealPasswd.c_str(), strSealPasswd.length());

	if (!dbCommand->Execute(string_format(_T("{? = CALL VIP_STORAGE_PASSWORD(?, ?, %d)}"), KeyCancel)))
		ReportSQLError(m_GameDB->GetError());

	return bRet;
}
#pragma endregion

#pragma region CDBAgent::VIPStorageCancelPassword(std::string & strAccountID)
uint8 CDBAgent::VIPStorageCancelPassword(std::string & strAccountID, uint8 KeyCancel)
{
	uint8 bRet = 1;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return 2;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());
	if (!dbCommand->Execute(string_format(_T("{? = CALL VIP_STORAGE_CANCEL_PASSWORD(?,%d)}"), KeyCancel)))
		ReportSQLError(m_GameDB->GetError());

	return bRet;
}
#pragma endregion

#pragma region CDBAgent::VIPStorageUseVaultKey(std::string &strAccountID, uint32 iExpirationTime)

uint8 CDBAgent::VIPStorageUseVaultKey(std::string &strAccountID, uint32 iExpirationTime)
{
	uint8 bRet = 1;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return 3;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());

	if (!dbCommand->Execute(string_format(_T("{? = CALL VIP_STORAGE_USE_VAULTKEY(?, %d)}"), iExpirationTime)))
		ReportSQLError(m_GameDB->GetError());

	return bRet;
}

#pragma endregion

void CDBAgent::LoadRentalData(string & strAccountID, string & strCharID, UserRentalMap & rentalData)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());
	if (!dbCommand->Execute(_T("{CALL LOAD_RENTAL_DATA(?)}")))
	{
		ReportSQLError(m_GameDB->GetError());
		return;
	}

	if (!dbCommand->hasData())
		return;

	do
	{
		_USER_RENTAL_ITEM *pItem = new _USER_RENTAL_ITEM();
		_RENTAL_ITEM *pRentalItem = nullptr;

		dbCommand->FetchString(1, pItem->strUserID);
		if (STRCASECMP(pItem->strUserID.c_str(), strCharID.c_str()) != 0)
		{
			delete pItem;
			continue;
		}

		dbCommand->FetchByte(2, pItem->byRentalType);
		dbCommand->FetchByte(3, pItem->byRegType);
		dbCommand->FetchUInt32(4, pItem->nRentalIndex);
		dbCommand->FetchUInt32(5, pItem->nItemID);
		dbCommand->FetchUInt16(6, pItem->sDurability);
		dbCommand->FetchUInt64(7, pItem->nSerialNum);
		dbCommand->FetchUInt32(8, pItem->nRentalMoney);
		dbCommand->FetchUInt16(9, pItem->sRentalTime);
		dbCommand->FetchInt16(10, pItem->sMinutesRemaining);
		dbCommand->FetchString(11, pItem->szTimeRental, sizeof(pItem->szTimeRental));

		pRentalItem = g_pMain->m_RentalItemArray.GetData(pItem->nRentalIndex);
		if (pRentalItem == nullptr
			|| rentalData.find(pItem->nSerialNum) != rentalData.end())
			delete pItem;
		else
			rentalData.insert(std::make_pair(pItem->nSerialNum, pItem));

	} while (dbCommand->MoveNext());
}

void CDBAgent::LoadItemSealData(string & strAccountID, string & strCharID, UserItemSealMap & itemSealData)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());
	if (!dbCommand->Execute(_T("SELECT nItemSerial, nItemID, bSealType FROM SEALED_ITEMS WHERE strAccountID = ?")))
	{
		ReportSQLError(m_GameDB->GetError());
		return;
	}

	if (!dbCommand->hasData())
		return;

	do
	{
		_USER_SEAL_ITEM *pItem = new _USER_SEAL_ITEM;

		dbCommand->FetchUInt64(1, pItem->nSerialNum);
		dbCommand->FetchUInt32(2, pItem->nItemID);
		dbCommand->FetchByte(3, pItem->bSealType);

		if (pItem == nullptr)
			delete pItem;
		else
			itemSealData.insert(std::make_pair(pItem->nSerialNum, pItem));

	} while (dbCommand->MoveNext());
}

bool CDBAgent::LoadUserData(string & strAccountID, string & strCharID, CUser *pUser)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	if (pUser == nullptr 
		|| pUser->m_bLogout
		|| !pUser->GetName().empty()
		|| strCharID.length() > MAX_ID_SIZE)
		return false;

	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());

	if (!dbCommand->Execute(_T("{CALL LOAD_USER_DATA(?, ?)}")))
		ReportSQLError(m_GameDB->GetError());

	if (!dbCommand->hasData())
		return false;

	char strItem[INVENTORY_TOTAL * 8] = { 0 },
		strSerial[INVENTORY_TOTAL * 8] = { 0 },
		strItemExpiration[INVENTORY_TOTAL * 4],
		strDeletedItem[40 * 12] = { 0 };

	int field = 1;
	dbCommand->FetchByte(field++, pUser->m_bNation);
	dbCommand->FetchByte(field++, pUser->m_bRace);
	dbCommand->FetchUInt16(field++, pUser->m_sClass);
	dbCommand->FetchUInt32(field++, pUser->m_nHair);
	dbCommand->FetchByte(field++, pUser->m_bRank);
	dbCommand->FetchByte(field++, pUser->m_bTitle);
	dbCommand->FetchByte(field++, pUser->m_bLevel);
	dbCommand->FetchByte(field++, pUser->m_bRebirthLevel);
	dbCommand->FetchInt64(field++, pUser->m_iExp);
	dbCommand->FetchUInt32(field++, pUser->m_iLoyalty);
	dbCommand->FetchByte(field++, pUser->m_bFace);
	dbCommand->FetchByte(field++, pUser->m_bCity);
	dbCommand->FetchInt16(field++, pUser->m_bKnights);
	dbCommand->FetchByte(field++, pUser->m_bFame);
	dbCommand->FetchInt16(field++, pUser->m_sHp);
	dbCommand->FetchInt16(field++, pUser->m_sMp);
	dbCommand->FetchInt16(field++, pUser->m_sSp);
	dbCommand->FetchByte(field++, pUser->m_bStats[STAT_STR]);
	dbCommand->FetchByte(field++, pUser->m_bStats[STAT_STA]);
	dbCommand->FetchByte(field++, pUser->m_bStats[STAT_DEX]);
	dbCommand->FetchByte(field++, pUser->m_bStats[STAT_INT]);
	dbCommand->FetchByte(field++, pUser->m_bStats[STAT_CHA]);
	dbCommand->FetchByte(field++, pUser->m_bRebStats[STAT_STR]);
	dbCommand->FetchByte(field++, pUser->m_bRebStats[STAT_STA]);
	dbCommand->FetchByte(field++, pUser->m_bRebStats[STAT_DEX]);
	dbCommand->FetchByte(field++, pUser->m_bRebStats[STAT_INT]);
	dbCommand->FetchByte(field++, pUser->m_bRebStats[STAT_CHA]);
	dbCommand->FetchByte(field++, pUser->m_bAuthority);
	dbCommand->FetchInt16(field++, pUser->m_sPoints);
	dbCommand->FetchUInt32(field++, pUser->m_iGold);
	dbCommand->FetchByte(field++, pUser->m_bZone);
	dbCommand->FetchInt16(field++, pUser->m_sBind);
	pUser->m_curx = (float)(dbCommand->FetchInt32(field++) / 100.0f);
	pUser->m_curz = (float)(dbCommand->FetchInt32(field++) / 100.0f);
	pUser->m_cury = (float)(dbCommand->FetchInt32(field++) / 100.0f);
	pUser->m_oldx = pUser->m_curx;
	pUser->m_oldy = pUser->m_cury;
	pUser->m_oldz = pUser->m_curz;
	pUser->m_dwTime = dbCommand->FetchUInt32(field++) + 1;
	dbCommand->FetchString(field++, (char *)pUser->m_bstrSkill, sizeof(pUser->m_bstrSkill));
	dbCommand->FetchBinary(field++, strItem, sizeof(strItem));
	dbCommand->FetchBinary(field++, strDeletedItem, sizeof(strDeletedItem));
	dbCommand->FetchBinary(field++, strSerial, sizeof(strSerial));
	dbCommand->FetchBinary(field++, strItemExpiration, sizeof(strItemExpiration));
	dbCommand->FetchUInt32(field++, pUser->m_iMannerPoint);
	dbCommand->FetchUInt32(field++, pUser->m_iLoyaltyMonthly);
	dbCommand->FetchUInt32(field++, pUser->m_iTimePunishment);

	pUser->m_strUserID = strCharID;
	pUser->m_lastSaveTime = UNIXTIME;
	pUser->m_lastBonusTime = UNIXTIME;
	pUser->m_TimeOnline = UNIXTIME;

	ByteBuffer ItemBuffer, DeletedItemBuffer, SerialBuffer, ItemTimeBuffer;
	ItemBuffer.append(strItem, sizeof(strItem));
	DeletedItemBuffer.append(strDeletedItem, sizeof(strDeletedItem));
	SerialBuffer.append(strSerial, sizeof(strSerial));
	ItemTimeBuffer.append(strItemExpiration, sizeof(strItemExpiration));

	memset(pUser->m_sItemArray, 0x00, sizeof(pUser->m_sItemArray));
	memset(pUser->m_sItemKnightRoyalArray, 0x00, sizeof(pUser->m_sItemKnightRoyalArray));

	pUser->m_sDeletedItemMap.DeleteAllData();
	UserRentalMap rentalData;

	// Until this statement is cleaned up, 
	// no other statements can be processed.
	delete dbCommand.release();

	if (pUser->m_sealedItemMap.size())
	{
		foreach(itr, pUser->m_sealedItemMap)
		{
			if (itr->second != nullptr)
				delete itr->second;
		}
	}
	pUser->m_sealedItemMap.clear();

	LoadRentalData(strAccountID, strCharID, rentalData);
	LoadItemSealData(strAccountID, strCharID, pUser->m_sealedItemMap);

	for (int i = 0; i < INVENTORY_TOTAL; i++)
	{ 
		uint64 nSerialNum;
		uint32 nItemID;
		int16 sDurability, sCount;
		uint32 nItemTime;

		ItemBuffer >> nItemID >> sDurability >> sCount;
		SerialBuffer >> nSerialNum;
		ItemTimeBuffer >> nItemTime;

		_ITEM_TABLE *pTable = g_pMain->GetItemPtr(nItemID);
		if (pTable == nullptr || sCount <= 0)
			continue;

		if (!pTable->m_bCountable && sCount > 1)
			sCount = 1;
		else if (sCount > ITEMCOUNT_MAX)
			sCount = ITEMCOUNT_MAX;

		if (nSerialNum == 0)
			nSerialNum = g_pMain->GenerateItemSerial();

		_ITEM_DATA *pItem = pUser->GetItem(i);
		pItem->nNum = nItemID;
		pItem->sDuration = pTable->isAccessory() ? pTable->m_sDuration : sDurability;
		pItem->sCount = sCount;
		pItem->nSerialNum = nSerialNum;
		pItem->nExpirationTime = nItemTime;

		if (pTable->isPetItem())
			LoadPetData(pItem->nSerialNum);

		// If the serial was found in the rental data, mark as rented.
		UserRentalMap::iterator itr = rentalData.find(nSerialNum);
		if (itr != rentalData.end())
		{
			pItem->bFlag = ITEM_FLAG_RENTED;
			pItem->sRemainingRentalTime = itr->second->sMinutesRemaining;
		}

		UserItemSealMap::iterator sealitr = pUser->m_sealedItemMap.find(nSerialNum);
		if (sealitr != pUser->m_sealedItemMap.end())
		{
			if (sealitr->second->bSealType == 1)
				pItem->bFlag = ITEM_FLAG_SEALED;
			else if (sealitr->second->bSealType == 3)
				pItem->bFlag = ITEM_FLAG_BOUND;
			else if (sealitr->second->bSealType == 4)
				pItem->bFlag = ITEM_FLAG_NOT_BOUND;
		}

		g_pMain->AddUserItem(nItemID, nSerialNum);

		if (g_pMain->IsDuplicateItem(nItemID,nSerialNum))
			pItem->bFlag = ITEM_FLAG_DUPLICATE;
	}

	for (int i = 0; i < 20; i++)
	{ 
		uint32 nItemID = 0;
		uint32 iDeleteTime = 0;
		uint32 sCount = 0;
		DeletedItemBuffer >> nItemID >> sCount >> iDeleteTime;

		_ITEM_TABLE *pTable = g_pMain->GetItemPtr(nItemID);
		if (pTable == nullptr || sCount <= 0)
			continue;

		if (!pTable->m_bCountable && sCount > 1)
			sCount = 1;
		else if (sCount > ITEMCOUNT_MAX)
			sCount = ITEMCOUNT_MAX;

		_DELETED_ITEM *pItemDeleted = new _DELETED_ITEM;
		pItemDeleted->nNum = nItemID;
		pItemDeleted->sCount = sCount;
		pItemDeleted->iDeleteTime = iDeleteTime;
		uint32 size = pUser->m_sDeletedItemMap.GetSize();

		if (!pUser->m_sDeletedItemMap.PutData(size, pItemDeleted))
			delete pItemDeleted;
	}

	// Clean up the rental data
	foreach (itr, rentalData)
		delete itr->second;
	rentalData.clear();

	if (pUser->m_bAuthority == AUTHORITY_GAME_MASTER)
	{
		if (!g_DBAgent.LoadGameMasterSetting(strAccountID, strCharID, g_pMain->m_nServerNo, pUser))
		{
			printf("Ýzinsiz Gm Giriþ Yapýmý ! Gm AccountID: (%s) Character Name: (%s) \n", strAccountID.c_str(), strCharID.c_str());
			return false;
		}
	}

	if (!g_DBAgent.GetLifeSkills(pUser))
	{
		if (pUser == nullptr)
			return false;

		pUser->m_sLifeSkills.KarmaExp = pUser->m_sLifeSkills.SmitheryExp = pUser->m_sLifeSkills.WarExp = pUser->m_sLifeSkills.HuntingExp = 0;
	}

	if (!g_DBAgent.GetLoadKnightCash(pUser))
	{
		if (pUser == nullptr)
			return false;

		pUser->m_nKnightCash = pUser->m_nKnightCashBonus = 0;
	}

	pUser->m_bCharacterDataLoaded = true;
	return true;
}

bool CDBAgent::LoadWarehouseData(string & strAccountID, CUser *pUser)
{
	char strItem[WAREHOUSE_MAX * 8], strSerial[WAREHOUSE_MAX * 8], strItemExpiration[WAREHOUSE_MAX * 4];

	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	if (pUser == nullptr 
		|| pUser->m_bLogout)
		return false;

	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());

	if (!dbCommand->Execute(_T("SELECT nMoney, WarehouseData, strSerial, WarehouseDataTime FROM WAREHOUSE WHERE strAccountID = ?")))
		ReportSQLError(m_GameDB->GetError());

	if (!dbCommand->hasData())
		return false;

	memset(strItem, 0x00, sizeof(strItem));
	memset(strSerial, 0x00, sizeof(strSerial));
	memset(strItemExpiration, 0x00, sizeof(strItemExpiration));

	dbCommand->FetchUInt32(1, pUser->m_iBank);
	dbCommand->FetchBinary(2, strItem, sizeof(strItem));
	dbCommand->FetchBinary(3, strSerial, sizeof(strSerial));
	dbCommand->FetchBinary(5, strItemExpiration, sizeof(strItemExpiration));

	ByteBuffer ItemBuffer, SerialBuffer, ItemTimeBuffer;
	ItemBuffer.append(strItem, sizeof(strItem));
	SerialBuffer.append(strSerial, sizeof(strSerial));
	ItemTimeBuffer.append(strItemExpiration, sizeof(strItemExpiration));

	memset(pUser->m_sWarehouseArray, 0x00, sizeof(pUser->m_sWarehouseArray));

	for (int i = 0; i < WAREHOUSE_MAX; i++) 
	{
		uint64 nSerialNum;
		uint32 nItemID, nItemTime;
		int16 sDurability, sCount;

		ItemBuffer >> nItemID >> sDurability >> sCount;
		SerialBuffer >> nSerialNum;
		ItemTimeBuffer >> nItemTime;

		_ITEM_TABLE *pTable = g_pMain->GetItemPtr(nItemID);
		if (pTable == nullptr || sCount <= 0)
			continue;

		if (!pTable->m_bCountable && sCount > 1)
			sCount = 1;
		else if (sCount > ITEMCOUNT_MAX)
			sCount = ITEMCOUNT_MAX;

		pUser->m_sWarehouseArray[i].nNum = nItemID;
		pUser->m_sWarehouseArray[i].sDuration = sDurability;
		pUser->m_sWarehouseArray[i].sCount = sCount;
		pUser->m_sWarehouseArray[i].nSerialNum = nSerialNum;
		pUser->m_sWarehouseArray[i].nExpirationTime = nItemTime;

		UserItemSealMap::iterator sealitr = pUser->m_sealedItemMap.find(nSerialNum);
		if (sealitr != pUser->m_sealedItemMap.end())
		{
			if (sealitr->second->bSealType == 1)
				pUser->m_sWarehouseArray[i].bFlag = ITEM_FLAG_SEALED;
			else if (sealitr->second->bSealType == 3)
				pUser->m_sWarehouseArray[i].bFlag = ITEM_FLAG_BOUND;
			else if (sealitr->second->bSealType == 4)
				pUser->m_sWarehouseArray[i].bFlag = ITEM_FLAG_NOT_BOUND;
		}

		if (g_pMain->IsDuplicateItem(nItemID,nSerialNum))
			pUser->m_sWarehouseArray[i].bFlag = ITEM_FLAG_DUPLICATE;
	}

	return true;
}

bool CDBAgent::LoadVipWarehouseData(string & strAccountID, CUser *pUser)
{
	char
		strItem[VIPWAREHOUSE_MAX * 8],
		strItemExpiration[VIPWAREHOUSE_MAX * 4],
		strSerial[VIPWAREHOUSE_MAX * 8];

	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	uint64 wait = 0;
	while (dbCommand->isOpen())
	{
		if (!dbCommand->isOpen())
			break;

		wait++;
	}

	if (pUser == nullptr
		|| pUser->m_bLogout)
		return false;

	pUser->m_bVIPStoragePassword = "";
	pUser->m_bVIPStorageVaultExpiration = 0;
	pUser->m_bWIPStorePassowrdRequest = 0;

	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());
	if (!dbCommand->Execute(_T("SELECT strPassword,strExpirationDate,WipPasswordRequest, VIPWarehouseData, strSerial, VIPWarehouseDataTime FROM VIP_WAREHOUSE WHERE strAccountID = ?")))
		ReportSQLError(m_GameDB->GetError());

	if (!dbCommand->hasData())
		return false;

	memset(strItem, 0x00, sizeof(strItem));
	memset(strSerial, 0x00, sizeof(strSerial));
	memset(strItemExpiration, 0x00, sizeof(strItemExpiration));

	dbCommand->FetchString(1, pUser->m_bVIPStoragePassword);
	dbCommand->FetchUInt32(2, pUser->m_bVIPStorageVaultExpiration);
	dbCommand->FetchByte(3, pUser->m_bWIPStorePassowrdRequest);
	dbCommand->FetchBinary(4, strItem, sizeof(strItem));
	dbCommand->FetchBinary(5, strSerial, sizeof(strSerial));
	dbCommand->FetchBinary(6, strItemExpiration, sizeof(strItemExpiration));

	ByteBuffer vItemBuffer, vSerialBuffer, vItemExBuffer;
	vItemBuffer.append(strItem, sizeof(strItem));
	vSerialBuffer.append(strSerial, sizeof(strSerial));
	vItemExBuffer.append(strItemExpiration, sizeof(strItemExpiration));

	memset(pUser->m_sVIPWarehouseArray, 0x00, sizeof(pUser->m_sVIPWarehouseArray));

	for (int i = 0; i < VIPWAREHOUSE_MAX; i++)
	{
		uint64 nSerialNum;
		uint32 nItemID, nItemEx;
		int16 sDurability, sCount;

		vItemBuffer >> nItemID >> sDurability >> sCount;
		vSerialBuffer >> nSerialNum;
		vItemExBuffer >> nItemEx;

		_ITEM_TABLE *pTable = g_pMain->GetItemPtr(nItemID);
		if (pTable == nullptr || sCount <= 0)
			continue;

		if (!pTable->m_bCountable && sCount > 1)
			sCount = 1;
		else if (sCount > ITEMCOUNT_MAX)
			sCount = ITEMCOUNT_MAX;

		pUser->m_sVIPWarehouseArray[i].nNum = nItemID;
		pUser->m_sVIPWarehouseArray[i].sDuration = sDurability;
		pUser->m_sVIPWarehouseArray[i].sCount = sCount;
		pUser->m_sVIPWarehouseArray[i].nSerialNum = nSerialNum;
		pUser->m_sVIPWarehouseArray[i].nExpirationTime = nItemEx;

		UserItemSealMap::iterator sealitr = pUser->m_sealedItemMap.find(nSerialNum);
		if (sealitr != pUser->m_sealedItemMap.end())
		{
			if (sealitr->second->bSealType == 1)
				pUser->m_sVIPWarehouseArray[i].bFlag = ITEM_FLAG_SEALED;
			else if (sealitr->second->bSealType == 3)
				pUser->m_sVIPWarehouseArray[i].bFlag = ITEM_FLAG_BOUND;
			else if (sealitr->second->bSealType == 4)
				pUser->m_sVIPWarehouseArray[i].bFlag = ITEM_FLAG_NOT_BOUND;
		}

		if (g_pMain->IsDuplicateItem(nItemID, nSerialNum))
			pUser->m_sVIPWarehouseArray[i].bFlag = ITEM_FLAG_DUPLICATE;
	}

	return true;
}

bool CDBAgent::LoadPremiumServiceUser(string & strAccountID, CUser *pUser)
{
	if (pUser == nullptr)
		return false;

	unique_ptr<OdbcCommand> dbCommand(m_AccountDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());
	if (!dbCommand->Execute(_T("{CALL LOAD_PREMIUM_SERVICE_USER_V2(?)}")))
	{
		ReportSQLError(m_AccountDB->GetError());
		return false;
	}

	if (!dbCommand->hasData())
		return false;

	char strPremium[30];
	memset(strPremium, 0, sizeof(strPremium));
	uint8 bPremiumCount = 0;

	dbCommand->FetchByte(1, pUser->m_bPremiumInUse);
	dbCommand->FetchByte(2, bPremiumCount);
	dbCommand->FetchBinary(3, strPremium, sizeof(strPremium));

	for (int i = 0, index = 0; i < bPremiumCount; i++, index += 5)
	{
		uint8	bPremiumType = *(uint8*)(strPremium + index);
		uint32	iPremiumTime = *(uint32*)(strPremium + index + 1);

		if (iPremiumTime < UNIXTIME)
			continue;

		_PREMIUM_DATA *pPremium = new _PREMIUM_DATA;
		pPremium->bPremiumType = bPremiumType;
		pPremium->iPremiumTime = iPremiumTime;
		if (!pUser->m_PremiumMap.PutData(bPremiumType, pPremium))
			delete pPremium;
	}

	if (pUser->m_PremiumMap.GetData(pUser->m_bPremiumInUse) == nullptr)
	{
		pUser->m_bPremiumInUse = NO_PREMIUM;

		foreach_stlmap(itr, pUser->m_PremiumMap)
		{
			_PREMIUM_DATA * uPrem = itr->second;
			if (uPrem == nullptr
				|| uPrem->iPremiumTime == 0)
				continue;

			pUser->m_bPremiumInUse = uPrem->bPremiumType;
			break;
		}
	}

	// this is hardcoded because we don't really care about the other mode
	if (pUser->m_bPremiumInUse != NO_PREMIUM)
		pUser->m_bAccountStatus = 1; // normal premium with expiry time
	else
		pUser->m_bAccountStatus = 0;

	return true;
}

#pragma region CDBAgent::LoadSavedMagic(CUser *pUser)

bool CDBAgent::LoadSavedMagic(CUser *pUser)
{
	if (pUser == nullptr)
		return false;

	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	dbCommand->AddParameter(SQL_PARAM_INPUT, pUser->m_strUserID.c_str(), pUser->m_strUserID.length());
	if (!dbCommand->Execute(_T("SELECT "
		"nSkill1, nDuring1, nSkill2, nDuring2, nSkill3, nDuring3, nSkill4, nDuring4, nSkill5, nDuring5, "
		"nSkill6, nDuring6, nSkill7, nDuring7, nSkill8, nDuring8, nSkill9, nDuring9, nSkill10, nDuring10 "
		"FROM USER_SAVED_MAGIC WHERE strCharID = ?")))
	{
		ReportSQLError(m_GameDB->GetError());
		return false;
	}


	if (!dbCommand->hasData())
		return false;

	Guard lock(pUser->m_savedMagicLock);
	pUser->m_savedMagicMap.clear();

	for (int i = 1; i <= 20; i += 2)
	{
		uint32 nSkillID, nExpiry;
		dbCommand->FetchUInt32(i, nSkillID);
		dbCommand->FetchUInt32(i + 1, nExpiry);

		if (nSkillID != 0)
			pUser->m_savedMagicMap[nSkillID] = (nExpiry * 1000 + UNIXTIME2);
	}

	return true;
}
#pragma endregion

#pragma region CDBAgent::UpdateSavedMagic(CUser *pUser)

bool CDBAgent::UpdateSavedMagic(CUser *pUser)
{
	if (pUser == nullptr)
		return false;

	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	Guard lock(pUser->m_savedMagicLock);
	uint32 nSkillID[10] = {0};
	uint32 tExpiryTime[10] = {0};
	uint32 i = 0;
	foreach (itr, pUser->m_savedMagicMap)
	{
		nSkillID[i]		= itr->first;
		if (itr->first != 0)
			tExpiryTime[i]	= (uint32)((itr->second - UNIXTIME2) / 1000);

		if (++i == 10)
			break;
	}
	int8 bRet = -2; // generic error

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, pUser->m_strUserID.c_str(), pUser->m_strUserID.length());
	if (!dbCommand->Execute(string_format(_T("{? = CALL UPDATE_SAVED_MAGIC(?, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)}"), 
		nSkillID[0], tExpiryTime[0], nSkillID[1], tExpiryTime[1], nSkillID[2], tExpiryTime[2], nSkillID[3], tExpiryTime[3], nSkillID[4], tExpiryTime[4],
		nSkillID[5], tExpiryTime[5], nSkillID[6], tExpiryTime[6], nSkillID[7], tExpiryTime[7], nSkillID[8], tExpiryTime[8], nSkillID[9], tExpiryTime[9])))
	{
		ReportSQLError(m_GameDB->GetError());
		return false;
	}

	return true;
}
#pragma endregion

bool CDBAgent::SetLogInInfo(string & strAccountID, string & strCharID, string & strServerIP, short sServerNo, string & strClientIP, uint8 bInit)
{
	uint8 result = 0;
	unique_ptr<OdbcCommand> dbCommand(m_AccountDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &result);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, strServerIP.c_str(), strServerIP.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, strClientIP.c_str(), strClientIP.length());

	if (!dbCommand->Execute(string_format(_T("{? = CALL SET_LOGIN_INFO(?, ?, %d, ?, ?, %d)}"), sServerNo, bInit)))
		ReportSQLError(m_AccountDB->GetError());

	return (bool)(result == 0 ? false : true);
}

#pragma region CDBAgent::RemoveCurrentUser(string & strAccountID)

bool CDBAgent::RemoveCurrentUser(string & strAccountID)
{

	unique_ptr<OdbcCommand> dbCommand(m_AccountDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());
	if (!dbCommand->Execute(_T("DELETE FROM CURRENTUSER WHERE strAccountID = ?")))
	{
		ReportSQLError(m_AccountDB->GetError());
		return false;
	}

	return true;
}
#pragma endregion

#pragma region CDBAgent::InsertCurrentUser(string & strAccountID)
bool CDBAgent::InsertCurrentUser(string& strAccountID, string& strCharID)
{

	unique_ptr<OdbcCommand> dbCommand(m_AccountDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());
	if (!dbCommand->Execute(_T("INSERT INTO CURRENTUSER VALUES (?,?,1,'127.0.0.1','127.0.0.2')")))
	{
		ReportSQLError(m_AccountDB->GetError());
		return false;
	}

	return true;
}
#pragma endregion

bool CDBAgent::LoadWebItemMall(std::vector<_ITEM_DATA> & itemList, CUser *pUser)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	dbCommand->AddParameter(SQL_PARAM_INPUT, pUser->m_strUserID.c_str(), pUser->m_strUserID.length());

	// TODO: Add an arg for the free slot count so we only need to pull/delete what we can hold.
	if (!dbCommand->Execute(_T("{CALL LOAD_WEB_ITEMMALL(?)}")))
		ReportSQLError(m_AccountDB->GetError());

	if (dbCommand->hasData())
	{
		do
		{
			_ITEM_DATA item;
			dbCommand->FetchUInt32(2, item.nNum); // 1 is the account name, which we don't need to use unless we're logging	
			dbCommand->FetchUInt16(3, item.sCount);
			dbCommand->FetchUInt32(5, item.nExpirationTime);
			itemList.push_back(item);
		} while (dbCommand->MoveNext());
	}

	return true;
}

#pragma region CDBAgent::LoadSkillShortcut(Packet & result, CUser *pUser)

bool CDBAgent::LoadSkillShortcut(Packet & result, CUser *pUser)
{
	if (pUser == nullptr)
		return false;

	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	uint16 sCount;
	char strSkillData[320];

	dbCommand->AddParameter(SQL_PARAM_INPUT, pUser->GetName().c_str(), pUser->GetName().length());
	if (!dbCommand->Execute(_T("SELECT nCount, strSkillData FROM USERDATA_SKILLSHORTCUT WHERE strCharID = ?")))
	{
		ReportSQLError(m_GameDB->GetError());
		return false;
	}

	if (!dbCommand->hasData())
		return false;

	dbCommand->FetchUInt16(1, sCount);
	dbCommand->FetchBinary(2, strSkillData, sizeof(strSkillData));

	result << sCount;
	for (uint32 i = 0; i < sCount; i++)
		result << *(uint32 *)(strSkillData + (i * sizeof(uint32)));

	return true;
}
#pragma endregion

#pragma region CDBAgent::SaveSkillShortcut(short sCount, char *buff, CUser *pUser)

void CDBAgent::SaveSkillShortcut(short sCount, char *buff, CUser *pUser)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	int8 bRet = -2; // generic error

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, pUser->GetName().c_str(), pUser->GetName().length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, (char *)buff, 320, SQL_BINARY);

	if (!dbCommand->Execute(string_format(_T("{? = CALL SKILLSHORTCUT_SAVE(?, %d, ?)}"), sCount)))
		ReportSQLError(m_GameDB->GetError());
}
#pragma endregion

bool CDBAgent::UpdatePremiumServiceUser(CUser *pUser)
{
	if (pUser == nullptr)
		return false;

	unique_ptr<OdbcCommand> dbCommand(m_AccountDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	int8 bRet = -2; // generic error

	char strPremium[30];
	int index = 0;
	memset(strPremium, 0, sizeof(strPremium));
	int counter = 0;

	uint32 time = 0;
	foreach_stlmap(itr, pUser->m_PremiumMap)
	{
		if (itr->second == nullptr
			|| itr->second->iPremiumTime < UNIXTIME)
			continue;

		if (time < itr->second->iPremiumTime)
			time = itr->second->iPremiumTime;

		*(uint8 *)(strPremium + index) = itr->first;
		*(uint32 *)(strPremium + 1 + index) = itr->second->iPremiumTime;
		index += 5;
		counter++;
	}

	if (time > uint32(UNIXTIME))
		time = uint32(time - UNIXTIME); // total premium seconds left

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, pUser->GetAccountName().c_str(), pUser->GetAccountName().length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, (char *)strPremium, sizeof(strPremium), SQL_BINARY);

	if (!dbCommand->Execute(string_format(_T("{? = CALL SAVE_PREMIUM_SERVICE_USER_V2(?, ?, %d, %d, %d)}"),
		counter, pUser->m_bPremiumInUse, time)))
		ReportSQLError(m_AccountDB->GetError());

	return true;
}

bool CDBAgent::UpdateClanPremiumServiceUser(CUser* pUser)
{
	if (pUser == nullptr)
		return false;

	CKnights* pKnights = g_pMain->GetClanPtr(pUser->GetClanID());
	if (pKnights == nullptr)
		return false;

	unique_ptr<OdbcCommand> dbCommand(m_AccountDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	if (!dbCommand->Execute(string_format(_T("{CALL SAVE_PREMIUM_SERVICE_CLAN(%d, %d, %d)}"), pKnights->GetID(), pKnights->sPremiumInUse, pKnights->sPremiumTime)))
	{
		ReportSQLError(m_GameDB->GetError());
		return false;
	}

	return true;
}

uint8 CDBAgent::HardWareIDLogin(std::string strAccountID, uint32 MACData, uint64 UserHardWareID, CUser* pUser)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return 3;

	if (pUser == nullptr)
		return 3;

	if (pUser->GetAccountName() != strAccountID)
		return 3;

	uint8 bRet = 1;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, pUser->GetAccountName().c_str(), pUser->GetAccountName().length());
	if (!dbCommand->Execute(string_format(_T("{? = CALL HWID_LOGIN(?, %d, " I64FMTD ")}"), MACData, UserHardWareID))) {
		ReportSQLError(m_GameDB->GetError());
		return 3;
	}

	return bRet;
}

uint8 CDBAgent::SealItem(string strSealPasswd, uint64 nItemSerial, uint32 nItemID, uint8 bSealType, CUser *pUser)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return 3;

	uint8 bRet = 1;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, pUser->GetAccountName().c_str(), pUser->GetAccountName().length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, pUser->GetName().c_str(), pUser->GetName().length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, strSealPasswd.c_str(), strSealPasswd.length());

	if (!dbCommand->Execute(string_format(_T("{? = CALL USER_ITEM_SEAL(?, ?, ?, " I64FMTD ", %d, %d)}"), nItemSerial, nItemID, bSealType)))
		ReportSQLError(m_GameDB->GetError());

	return bRet;
}

void CDBAgent::RequestFriendList(std::vector<string> & friendList, CUser *pUser)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	dbCommand->AddParameter(SQL_PARAM_INPUT, pUser->GetName().c_str(), pUser->GetName().length());
	if (!dbCommand->Execute(_T("SELECT * FROM FRIEND_LIST WHERE strUserID = ?")))
		ReportSQLError(m_GameDB->GetError());

	if (!dbCommand->hasData())
		return;

	string strCharID;
	for (int i = 2; i <= 25; i++)
	{
		if (dbCommand->FetchString(i, strCharID)
			&& strCharID.length())
			friendList.push_back(strCharID);
	}
}

FriendAddResult CDBAgent::AddFriend(short sid, short tid)
{
	CUser* pSrcUser = g_pMain->GetUserPtr(sid), * pTargetUser = g_pMain->GetUserPtr(tid);
	if (pSrcUser == nullptr)
		return FRIEND_ADD_ERROR;

	if (pTargetUser == nullptr)
	{
		CBot* pTargetUser = g_pMain->m_BotArray.GetData(tid);

		if (pTargetUser == nullptr)
			return FRIEND_ADD_ERROR;

		unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
		if (dbCommand.get() == nullptr)
			return FRIEND_ADD_ERROR;

		int16 sRet = (int16)FRIEND_ADD_ERROR;

		dbCommand->AddParameter(SQL_PARAM_OUTPUT, &sRet);
		dbCommand->AddParameter(SQL_PARAM_INPUT, pSrcUser->GetName().c_str(), pSrcUser->GetName().length());
		dbCommand->AddParameter(SQL_PARAM_INPUT, pTargetUser->GetName().c_str(), pTargetUser->GetName().length());

		if (!dbCommand->Execute(_T("{? = CALL INSERT_FRIEND_LIST(?, ?)}")))
			ReportSQLError(m_GameDB->GetError());

		return (FriendAddResult)sRet;
	}

	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return FRIEND_ADD_ERROR;

	int16 sRet = (int16)FRIEND_ADD_ERROR;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &sRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, pSrcUser->GetName().c_str(), pSrcUser->GetName().length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, pTargetUser->GetName().c_str(), pTargetUser->GetName().length());

	if (!dbCommand->Execute(_T("{? = CALL INSERT_FRIEND_LIST(?, ?)}")))
		ReportSQLError(m_GameDB->GetError());

	return (FriendAddResult)sRet;
}

FriendRemoveResult CDBAgent::RemoveFriend(string & strCharID, CUser *pUser)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return FRIEND_REMOVE_ERROR;

	int16 sRet = (int16)FRIEND_REMOVE_ERROR;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &sRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, pUser->m_strUserID.c_str(), pUser->m_strUserID.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());

	if (!dbCommand->Execute(_T("{? = CALL DELETE_FRIEND_LIST(?, ?)}")))
		ReportSQLError(m_GameDB->GetError());

	return (FriendRemoveResult)sRet;
}

bool CDBAgent::UpdateUser(string & strCharID, UserUpdateType type, CUser *pUser)
{
	if (strCharID != pUser->GetName())
		return false;

	if (pUser->GetZoneID() == ZONE_KNIGHT_ROYALE)
		return false;

	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	if (type == UPDATE_PACKET_SAVE)
		pUser->m_dwTime++;
	else if (type == UPDATE_LOGOUT 
		|| type == UPDATE_ALL_SAVE)
		pUser->m_dwTime = 0;

	ByteBuffer ItemBuffer, SerialBuffer, ItemTimeBuffer;
	for (int i = 0; i < INVENTORY_TOTAL; i++)
	{
		_ITEM_DATA *pItem = &pUser->m_sItemArray[i];
		ItemBuffer << pItem->nNum << pItem->sDuration << pItem->sCount;
		SerialBuffer << pItem->nSerialNum;
		ItemTimeBuffer << pItem->nExpirationTime;

		_ITEM_TABLE *pTable = g_pMain->GetItemPtr(pItem->nNum);
		if (pTable == nullptr)
			continue;

		if (pTable->isPetItem())
			UpdatingUserPet(pItem->nSerialNum);

		g_pMain->DeleteUserItem(pItem->nNum, pItem->nSerialNum);
	}

	char strDeletedItem[480];
	memset(strDeletedItem, 0, sizeof(strDeletedItem));
	int index2 = 0;
	int counter = 0;
	pUser->m_sDeletedItemMap.m_lock.lock();

	foreach_stlmap_nolock (itr, pUser->m_sDeletedItemMap)
	{
		if(counter > 18)
			break;

		_DELETED_ITEM *pDeletedItem = itr->second;

		if(pDeletedItem == nullptr
			|| (UNIXTIME - pDeletedItem->iDeleteTime > 60 * 60 * 24 * 3))
			continue;

		*(uint32 *)(strDeletedItem + index2)		= pDeletedItem->nNum;
		*(uint32 *)(strDeletedItem + index2 + 4)	= pDeletedItem->sCount;
		*(uint32 *)(strDeletedItem + index2 + 8)	= pDeletedItem->iDeleteTime;
		counter++;
		index2 += 11;
	}
	pUser->m_sDeletedItemMap.m_lock.unlock();

	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, (char *)pUser->m_bstrSkill, sizeof(pUser->m_bstrSkill));
	dbCommand->AddParameter(SQL_PARAM_INPUT, (char *)ItemBuffer.contents(), ItemBuffer.size(), SQL_BINARY);
	dbCommand->AddParameter(SQL_PARAM_INPUT, (char *)strDeletedItem, sizeof(strDeletedItem), SQL_BINARY);
	dbCommand->AddParameter(SQL_PARAM_INPUT, (char *)SerialBuffer.contents(), SerialBuffer.size(), SQL_BINARY);
	dbCommand->AddParameter(SQL_PARAM_INPUT, (char *)ItemTimeBuffer.contents(), ItemTimeBuffer.size(), SQL_BINARY);
	
	if(type == UPDATE_LOGOUT)
	{
		if(pUser->isInMoradon())
			pUser->m_bZone = ZONE_MORADON;
		else if(pUser->isInLufersonCastle())
			pUser->m_bZone = ZONE_KARUS;
		else if(pUser->isInElmoradCastle())
			pUser->m_bZone = ZONE_ELMORAD;
		else if(pUser->isInKarusEslant())
			pUser->m_bZone = ZONE_KARUS_ESLANT;
		else if(pUser->isInElmoradEslant())
			pUser->m_bZone = ZONE_ELMORAD_ESLANT;
	}

	uint32 nDonatedNP = 0;
	CKnights* pKnights = g_pMain->GetClanPtr(pUser->GetClanID());
	if (pKnights != nullptr)
	{
		std:: string userid = pUser->GetName();
		STRTOUPPER(userid);
		_KNIGHTS_USER * pKnightUser = pKnights->m_arKnightsUser.GetData(userid);
		if(pKnightUser != nullptr)
		{
			pKnightUser->bFame = pUser->GetFame();
			pKnightUser->bLevel = pUser->GetLevel();
			pKnightUser->sClass = pUser->GetClass();
			pKnightUser->nLastLogin = int32(UNIXTIME);
			nDonatedNP = pKnightUser->nDonatedNP;
		}
	}

	if(pUser->isKing())
	{
		CKingSystem * pKingSystem = g_pMain->m_KingSystemArray.GetData(pUser->GetNation());

		if(!pKingSystem)
			pKingSystem->m_sKingClanID = pUser->GetClanID();
	}

	if (!dbCommand->Execute(string_format(_T("{CALL UPDATE_USER_DATA("
		"?, "								// strCharID 
		"%d, %d, %d, %d, %d, "				// nation, race, class, hair, rank
		"%d, %d, %d, " I64FMTD ", %d, %d, "	// title, level, levelreb, exp, loyalty, face
		"%d, %d, %d, "						// city, knights, fame
		"%d, %d, %d, "						// hp, mp, sp
		"%d, %d, %d, %d, %d, "				// str, sta, dex, int, cha
		"%d, %d, %d, %d, %d, "				// rebstr, rebsta, rebdex, rebint, rebcha
		"%d, %d, %d, %d, %d, "				// authority, free points, gold, zone, bind
		"%d, %d, %d, %d, "					// x, z, y, dwTime
		"?, ?, ?, ?, ?, "					// strSkill, strItem, strDeletedItem, strSerial, strItemExpiration
		"%d, %d, "							// manner points, monthly NP
		"%d, %d, %d)}"),					//LastLogin, nDonatedNP,m_iTimePunishment			
		pUser->m_bNation, pUser->m_bRace, pUser->m_sClass, pUser->m_nHair, pUser->m_bRank,
		pUser->m_bTitle, pUser->m_bLevel, pUser->m_bRebirthLevel, pUser->m_iExp /* temp hack, database needs to support it */, pUser->m_iLoyalty, pUser->m_bFace,
		pUser->m_bCity, pUser->m_bKnights, pUser->m_bFame,
		pUser->m_sHp, pUser->m_sMp, pUser->m_sSp,
		pUser->m_bStats[STAT_STR], pUser->m_bStats[STAT_STA], pUser->m_bStats[STAT_DEX], pUser->m_bStats[STAT_INT], pUser->m_bStats[STAT_CHA],
		pUser->m_bRebStats[STAT_STR], pUser->m_bRebStats[STAT_STA], pUser->m_bRebStats[STAT_DEX], pUser->m_bRebStats[STAT_INT], pUser->m_bRebStats[STAT_CHA],
		pUser->m_bAuthority, pUser->m_sPoints, pUser->m_iGold, pUser->m_bZone, pUser->m_sBind,
		(int)(pUser->m_curx * 100), (int)(pUser->m_curz * 100), (int)(pUser->m_cury * 100), pUser->m_dwTime,
		pUser->m_iMannerPoint, pUser->m_iLoyaltyMonthly,
		int32(UNIXTIME), nDonatedNP, pUser->m_iTimePunishment)))
	{
		ReportSQLError(m_GameDB->GetError());
		return false;
	}

	if (type == UPDATE_LOGOUT)
		g_DBAgent.SaveLifeSkills(pUser);

	g_DBAgent.UpdateKnightCash(pUser);

	pUser->m_lastSaveTime = UNIXTIME;
	pUser->m_lastBonusTime = UNIXTIME;
	return true;
}

bool CDBAgent::UpdateWarehouseData(string & strAccountID, UserUpdateType type, CUser *pUser)
{
	if (strAccountID.length() == 0)
		return false;

	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	if (type == UPDATE_LOGOUT 
		|| type == UPDATE_ALL_SAVE)
		pUser->m_dwTime = 0;

	// This *should* be padded like the database field is (unnecessarily), but I want to see how MSSQL responds.
	ByteBuffer ItemBuffer, SerialBuffer, ItemTimeBuffer;
	for (int i = 0; i < WAREHOUSE_MAX; i++)
	{
		_ITEM_DATA *pItem = &pUser->m_sWarehouseArray[i];
		ItemBuffer << pItem->nNum << pItem->sDuration << pItem->sCount;
		SerialBuffer << pItem->nSerialNum;
		ItemTimeBuffer << pItem->nExpirationTime;
	}

	dbCommand->AddParameter(SQL_PARAM_INPUT, (char *)ItemBuffer.contents(), ItemBuffer.size(), SQL_BINARY);
	dbCommand->AddParameter(SQL_PARAM_INPUT, (char *)SerialBuffer.contents(), SerialBuffer.size(), SQL_BINARY);
	dbCommand->AddParameter(SQL_PARAM_INPUT, (char *)ItemTimeBuffer.contents(), ItemTimeBuffer.size(), SQL_BINARY);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());

	if (!dbCommand->Execute(string_format(_T("UPDATE WAREHOUSE SET nMoney=%d, dwTime=%d, WarehouseData=?, strSerial=?, WarehouseDataTime=? WHERE strAccountID=?"), 
		pUser->m_iBank, pUser->m_dwTime)))
	{
		ReportSQLError(m_GameDB->GetError());
		return false;
	}

	return true;
}

bool CDBAgent::UpdateVipWarehouseData(string & strAccountID, UserUpdateType type, CUser *pUser)
{
	if (strAccountID.length() == 0)
		return false;

	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	uint64 wait = 0;
	while (dbCommand->isOpen())
	{
		if (!dbCommand->isOpen())
			break;

		wait++;
	}

	if (type == UPDATE_LOGOUT 
		|| type == UPDATE_ALL_SAVE)
		pUser->m_dwTime = 0;

	// This *should* be padded like the database field is (unnecessarily), but I want to see how MSSQL responds.
	ByteBuffer vItemBuffer, vSerialBuffer, vItemExBuffer;

	for (int i = 0; i < VIPWAREHOUSE_MAX; i++)
	{
		_ITEM_DATA *pItem = &pUser->m_sVIPWarehouseArray[i];
		vItemBuffer << pItem->nNum << pItem->sDuration << pItem->sCount;
		vSerialBuffer << pItem->nSerialNum;
		vItemExBuffer << pItem->nExpirationTime;
	}

	dbCommand->AddParameter(SQL_PARAM_INPUT, (char *)vItemBuffer.contents(), vItemBuffer.size(), SQL_BINARY);
	dbCommand->AddParameter(SQL_PARAM_INPUT, (char *)vSerialBuffer.contents(), vSerialBuffer.size(), SQL_BINARY);
	dbCommand->AddParameter(SQL_PARAM_INPUT, (char *)vItemExBuffer.contents(), vItemExBuffer.size(), SQL_BINARY);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());

	if (!dbCommand->Execute(string_format(_T("UPDATE VIP_WAREHOUSE SET VIPWarehouseData=?, strSerial=?, VIPWarehouseDataTime=?  WHERE strAccountID=?"))))
	{
		ReportSQLError(m_GameDB->GetError());
		return false;
	}

	return true;
}

/**
* @brief	Change authority is logged out character.
*
* @param	strUserID	Character's name.
* @param	nAuthority	New user authority.
*/

void CDBAgent::UpdateUserAuthority(std::string &GameMaster, string & strUserID, uint16 nAuthority, uint32 iPeriod, uint8 banType, string &reason, CUser *pUser/* = nullptr*/)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	int sRet = -1;
	std::string strRealUserID = "", strAccountID = "";
	dbCommand->AddParameter(SQL_PARAM_INPUT, strUserID.c_str(), strUserID.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, GameMaster.c_str(), GameMaster.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, reason.c_str(), reason.length());

	if (!dbCommand->Execute(string_format(_T("{CALL AUTHORITY_CHANGE(?, ?, ?, %d, %d, %d)}"), nAuthority, iPeriod, banType)))
	{
		ReportSQLError(m_GameDB->GetError());
		sRet = -1;
	}

	if (dbCommand->hasData())
	{
		dbCommand->FetchInt32(1, sRet);
		dbCommand->FetchString(2, strRealUserID);
		dbCommand->FetchString(3, strAccountID);
	}

	if (pUser != nullptr)
	{
		if (sRet == 1)
		{
			g_pMain->SendHelpDescription(pUser, "Invalid player name! ");
			return;
		}
		else if (sRet == 2)
		{
			g_pMain->SendHelpDescription(pUser, "You cannot change a GM's authority!");
			return;
		}
		else if (sRet == 3)
		{
			g_pMain->SendHelpDescription(pUser, "No operation was made since user already is of that authority!");
			return;
		}
	}

	if (sRet == 6)
	{
		g_pMain->SendHelpDescription(pUser, "Some error has occured during database process. Please try again!");
		return;
	}
	else if (sRet == 0) //operation was successful and this is 
	{
		std::string sNoticeMessage;
		CUser *pUserAccount = g_pMain->GetUserPtr(strAccountID, TYPE_ACCOUNT);
		CUser *pUserCharacter = g_pMain->GetUserPtr(strRealUserID, TYPE_CHARACTER);
		if (pUserAccount == nullptr 
			|| pUserCharacter == nullptr)
			return;

		switch (banType)
		{
		case BANNED:
			if (pUserAccount != nullptr)
				pUserAccount->Disconnect();

			sNoticeMessage = string_format("%s is currently blocked for using illegal software.", strRealUserID.c_str());
			g_pMain->SendNotice(sNoticeMessage.c_str(), Nation::ALL);
			break;
		case MUTE:
			if (pUserCharacter != nullptr)
				pUserCharacter->m_bAuthority = AUTHORITY_MUTED;
			break;
		case UNMUTE:
			if (pUserCharacter != nullptr)
				pUserCharacter->m_bAuthority = AUTHORITY_PLAYER;
			break;
		case DIS_ATTACK:
			if (pUserCharacter != nullptr)
				pUserCharacter->m_bAuthority = AUTHORITY_ATTACK_DISABLED;
			break;
		case ALLOW_ATTACK:
			if (pUserCharacter != nullptr)
				pUserCharacter->m_bAuthority = AUTHORITY_PLAYER;
			break;
		}
	}
}


uint8 CDBAgent::UpdateCharacterName(std::string & strAccountID, std::string & strUserID, std::string & strNewUserID)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return 2;

	uint8 sRet = 1;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &sRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, strUserID.c_str(), strUserID.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, strNewUserID.c_str(), strNewUserID.length());

	if (!dbCommand->Execute(string_format(_T("{? = CALL CHANGE_NEW_ID(%d, ?, ?, ?, 1)}"), g_pMain->m_nServerNo)))
		ReportSQLError(m_GameDB->GetError());

	return sRet;
}

uint8 CDBAgent::UpdateCharacterClanName(std::string & strClanID, std::string & strNewClanID)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return 0;

	uint8 sRet = 1;
	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &sRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strClanID.c_str(), strClanID.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, strNewClanID.c_str(), strNewClanID.length());

	if (!dbCommand->Execute(string_format(_T("{? = CALL CHANGE_NEW_CLANID(?, ?)}"))))
		ReportSQLError(m_GameDB->GetError());

	return sRet;
}
void CDBAgent::UpdateBattleEvent(string & strCharID, uint8 bNation)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());
	if (!dbCommand->Execute(string_format(_T("UPDATE BATTLE SET byNation=%d, strUserName=? WHERE sIndex=%d"), bNation, g_pMain->m_nServerNo)))
		ReportSQLError(m_GameDB->GetError());
}

void CDBAgent::AccountLogout(string & strAccountID)
{
	unique_ptr<OdbcCommand> dbCommand(m_AccountDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());
	if (!dbCommand->Execute(_T("{CALL ACCOUNT_LOGOUT(?)}")))
		ReportSQLError(m_AccountDB->GetError());
}

void CDBAgent::UpdateConCurrentUserCount(int nServerNo, int nZoneNo, int nCount)
{
	unique_ptr<OdbcCommand> dbCommand(m_AccountDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	if (!dbCommand->Execute(string_format(_T("UPDATE CONCURRENT SET zone%d_count = %d WHERE serverid = %d"), nZoneNo, nCount, nServerNo)))
		ReportSQLError(m_AccountDB->GetError());
}

// This is what everything says it should do, 
// but the client doesn't seem to care if it's over 1
uint8 CDBAgent::GetUnreadLetterCount(string & strCharID)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return 0;

	uint8 bCount = 0;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bCount);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());
	if (!dbCommand->Execute(_T("{? = CALL MAIL_BOX_CHECK_COUNT(?)}")))
	{
		ReportSQLError(m_GameDB->GetError());
		return 0;
	}

	return bCount;
}

bool CDBAgent::GetLetterList(string & strCharID, Packet & result, bool bNewLettersOnly /* = true*/)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	int8 bCount = 0;
	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bCount);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());
	if (!dbCommand->Execute(string_format(_T("{? = CALL MAIL_BOX_REQUEST_LIST(?, %d)}"), bNewLettersOnly)))
	{
		ReportSQLError(m_GameDB->GetError());
		return false;
	}

	result << uint8(1);
	int offset = (int) result.wpos();
	result << bCount; // placeholder for count

	if (!dbCommand->hasData())
		return true;

	result.SByte();
	do
	{
		string strSubject, strSender;
		uint32 nLetterID, nItemID, nCoins, nDate;
		uint16 sCount, sDaysRemaining;
		uint8 bStatus, bType;

		dbCommand->FetchUInt32(1, nLetterID);
		dbCommand->FetchByte(2, bStatus);
		dbCommand->FetchByte(3, bType);
		dbCommand->FetchString(4, strSubject);
		dbCommand->FetchString(5, strSender);
		dbCommand->FetchByte(6, bType);
		dbCommand->FetchUInt32(7, nItemID);
		dbCommand->FetchUInt16(8, sCount);
		dbCommand->FetchUInt32(9, nCoins);
		dbCommand->FetchUInt32(10, nDate);
		dbCommand->FetchUInt16(11, sDaysRemaining); 

		result	<< nLetterID // letter ID
			<< bStatus  // letter status, doesn't seem to affect anything
			<< strSubject << strSender
			<< bType;	

		if (bType == 2)
			result	<< nItemID << sCount << nCoins;

		result	<< nDate // date (yy*10000 + mm*100 + dd)
			<< sDaysRemaining;

	} while (dbCommand->MoveNext());

	result.put(offset, bCount); // set count now that the result set's been read

	return true;
}

int8 CDBAgent::SendLetter(string & strSenderID, string & strRecipientID, string & strSubject, string & strMessage, uint8 bType, _ITEM_DATA * pItem, int32 nCoins)
{
	uint64 nSerialNum = 0;
	uint32 nItemID = 0;
	uint16 sCount = 0, sDurability = 0;
	int8 bRet = 0;

	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return 0;

	// This is a little bit redundant, but best to be sure.
	if (bType == 2 
		&& pItem != nullptr)
	{
		nItemID = pItem->nNum;
		sCount = pItem->sCount;
		sDurability = pItem->sDuration;
		nSerialNum = pItem->nSerialNum;
	}

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strSenderID.c_str(), strSenderID.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, strRecipientID.c_str(), strRecipientID.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, strSubject.c_str(), strSubject.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, strMessage.c_str(), strMessage.length());

	// MSSQL uses signed types.
	if (!dbCommand->Execute(string_format(_T("{? = CALL MAIL_BOX_SEND(?, ?, ?, ?, %d, %d, %d, %d, " I64FMTD ", %d)}"), 
		bType, nItemID, sCount, sDurability, nSerialNum, nCoins)))
	{
		ReportSQLError(m_GameDB->GetError());
		return 0;
	}

	return bRet;
}

bool CDBAgent::ReadLetter(string & strCharID, uint32 nLetterID, string & strMessage)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());
	if (!dbCommand->Execute(string_format(_T("{CALL MAIL_BOX_READ(?, %d)}"), nLetterID)))
	{
		ReportSQLError(m_GameDB->GetError());
		return false;
	}

	if (!dbCommand->hasData())
		return false;

	dbCommand->FetchString(1, strMessage);
	return true;
}

int8 CDBAgent::GetItemFromLetter(string & strCharID, uint32 nLetterID, uint32 & nItemID, uint16 & sCount, uint16 & sDurability, uint32 & nCoins, uint64 & nSerialNum, uint32 & nItemTime)
{
	int8 bRet = -1; // error
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return bRet;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());
	if (!dbCommand->Execute(string_format(_T("{? = CALL MAIL_BOX_GET_ITEM(?, %d)}"), nLetterID)))
		ReportSQLError(m_GameDB->GetError());

	if (dbCommand->hasData())
	{
		dbCommand->FetchUInt32(1, nItemID);
		dbCommand->FetchUInt16(2, sCount);
		dbCommand->FetchUInt16(3, sDurability);
		dbCommand->FetchUInt32(4, nCoins);
		dbCommand->FetchUInt64(5, nSerialNum);
		dbCommand->FetchUInt32(6, nItemTime);
	}

	return bRet=1;
}

void CDBAgent::DeleteLetter(string & strCharID, uint32 nLetterID)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());
	// NOTE: The official implementation passes all 5 letter IDs.
	if (!dbCommand->Execute(string_format(_T("UPDATE MAIL_BOX SET bDeleted = 1 WHERE nLetterID = %d AND strRecipientID = ?"), nLetterID)))
		ReportSQLError(m_GameDB->GetError());
}

#pragma region King System Related Functions

#pragma region CDBAgent::UpdateElectionStatus(uint8 byType, uint8 byNation)

/**
* @brief	Updates the election status.
*
* @param	byType  	Election status.
* @param	byNation	Electoral nation.
*/
void CDBAgent::UpdateElectionStatus(uint8 byType, uint8 byNation)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	if (!dbCommand->Execute(string_format(_T("{CALL KING_UPDATE_ELECTION_STATUS(%d, %d)}"), byType, byNation)))
		ReportSQLError(m_GameDB->GetError());

	if(byType == ELECTION_TYPE_ELECTION)
	{
		CKingSystem * pKingSystem = g_pMain->m_KingSystemArray.GetData(byNation);
		if(pKingSystem != nullptr)
		{
			if (dbCommand->hasData())
			{
				dbCommand->FetchString(1, pKingSystem->m_strKingName);
				dbCommand->FetchUInt16(2, pKingSystem->m_sYear);
				dbCommand->FetchByte(3, pKingSystem->m_byMonth);
				dbCommand->FetchByte(4, pKingSystem->m_byDay);
				dbCommand->FetchByte(5, pKingSystem->m_byHour);
				dbCommand->FetchByte(6, pKingSystem->m_byMinute);

				CUser *pUser = g_pMain->GetUserPtr(pKingSystem->m_strKingName, TYPE_CHARACTER);

				if(pUser != nullptr && pUser->isInGame())
				{
					pUser->SendMyInfo();

					pUser->UserInOut(INOUT_OUT);
					pUser->SetRegion(pUser->GetNewRegionX(), pUser->GetNewRegionZ());
					pUser->UserInOut(INOUT_WARP);

					g_pMain->UserInOutForMe(pUser);
					g_pMain->NpcInOutForMe(pUser);
					g_pMain->MerchantUserInOutForMe(pUser);

					pUser->ResetWindows();

					pUser->InitType4();
					pUser->RecastSavedMagic();
				}
			}
		}
	}
}
#pragma endregion

#pragma region CDBAgent::UpdateElectionList(uint8 byDBType, uint8 byType, uint8 byNation, uint16 sKnights, uint32 nAmount, string & strNominee)

/**
* @brief	Updates the election list.
*
* @param	byDBType  	Procedure-specific database action.
* 						If 1, insert. If 2, delete.
* @param	byType	  	Flag to specify what the user's in the election list for (election, impeachment, and thereof).
* @param	byNation  	Electoral nation.
* @param	sKnights  	The nominee's clan ID.
* @param	nAmount		Vote count.
* @param	strNominee	The nominee's name.
*/
void CDBAgent::UpdateElectionList(uint8 byDBType, uint8 byType, uint8 byNation, uint16 sKnights, uint32 nAmount, string & strNominee)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	dbCommand->AddParameter(SQL_PARAM_INPUT, strNominee.c_str(), strNominee.length());
	if (!dbCommand->Execute(string_format(_T("{CALL KING_UPDATE_ELECTION_LIST(%d, %d, %d, %d, %d, ?)}"), 
		byDBType, byType, byNation, sKnights, nAmount)))
		ReportSQLError(m_GameDB->GetError());
}

#pragma endregion

#pragma region CDBAgent::UpdateCandidacyRecommend(std::string & strNominator, std::string & strNominee, uint8 byNation)

/**
* @brief	Nominates/recommends strNominee as King.
*
* @param	strNominator	The nominator.
* @param	strNominee  	The nominee.
* @param	byNation		Their nation.
*
* @return	.
*/
int16 CDBAgent::UpdateCandidacyRecommend(std::string & strNominator, std::string & strNominee, uint8 byNation)
{
	int16 sRet = -6;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return sRet;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &sRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strNominator.c_str(), strNominator.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, strNominee.c_str(), strNominee.length());

	if (!dbCommand->Execute(string_format(_T("{? = CALL KING_CANDIDACY_RECOMMEND(?, ?, %d)}"), byNation)))
		ReportSQLError(m_GameDB->GetError());

	return sRet;
}
#pragma endregion

#pragma region CDBAgent::UpdateElectionVoteList(uint8 byNation, std::string & strVoterAccountID, std::string & strVoterUserID, std::string & strNominee)
/**
* @brief	Updates the election vote list.
*
* @param	byNation  	Electoral nation.
* @param	strVoterAccountID  	Voter User Account ID
* @param	strVoterUserID		Voter User ID
* @param	strNominee	The nominee's name.
*/
int16 CDBAgent::UpdateElectionVoteList(uint8 byNation, std::string & strVoterAccountID, std::string & strVoterUserID, std::string & strNominee)
{
	int16 bRet = -6; // error
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return bRet;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strVoterAccountID.c_str(), strVoterAccountID.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, strVoterUserID.c_str(), strVoterUserID.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, strNominee.c_str(), strNominee.length());
	if (!dbCommand->Execute(string_format(_T("{? = CALL KING_ELECTION_PROC(?, ?, %d, ?)}"), byNation)))
		ReportSQLError(m_GameDB->GetError());

	return bRet;
}

#pragma endregion

#pragma region CDBAgent::GetElectionResults(uint8 byNation)
/**
* @brief	Updates the election vote list.
*
* @param	byNation  	Electoral nation.
*/
void CDBAgent::GetElectionResults(uint8 byNation)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	if (!dbCommand->Execute(string_format(_T("{CALL KING_ELECTION_RESULTS(%d)}"), byNation)))
	{
		ReportSQLError(m_GameDB->GetError());
		return;
	}

	if (!dbCommand->hasData())
		return;

	CKingSystem * pKingSystem = g_pMain->m_KingSystemArray.GetData(byNation);
	if (pKingSystem == nullptr)
		return;

	pKingSystem->m_bElectionUnderProgress = true;
	// All db processes are done.
	uint16 count = 0;
	do
	{
		string strUserID;
		uint8 bRank, bMonth;
		uint16 sClanID, sYear;
		uint32 kingvotes, totalvotes;
		// update the king system respectively.
		dbCommand->FetchString(1, strUserID);
		dbCommand->FetchByte(2, bRank);
		dbCommand->FetchUInt16(3, sClanID);
		dbCommand->FetchByte(4, bMonth);
		dbCommand->FetchUInt16(5, sYear);
		dbCommand->FetchUInt32(6, kingvotes);
		dbCommand->FetchUInt32(7, totalvotes);

		pKingSystem->m_byNextMonth = bMonth;
		pKingSystem->m_sNextYear = sYear;

		if(bRank == 2)
		{
			_KING_ELECTION_LIST * pEntry = new _KING_ELECTION_LIST;
			pEntry->sKnights = sClanID;
			pEntry->nVotes = 0;
			pKingSystem->m_senatorList.insert(make_pair(strUserID, pEntry));
		}
		else if ( bRank = 1) // this is the king!
		{
			pKingSystem->m_strNewKingName = strUserID;
			pKingSystem->m_sKingClanID = sClanID;
			pKingSystem->m_KingVotes = kingvotes;
			pKingSystem->m_TotalVotes = totalvotes;
		}
	} while (dbCommand->MoveNext());

	pKingSystem->m_bElectionUnderProgress = false;
}

#pragma endregion

#pragma region CDBAgent::UpdateCandidacyNoticeBoard(string & strCharID, uint8 byNation, string & strNotice)

/**
* @brief	Updates the candidacy notice board.
*
* @param	strCharID	Candidate's name.
* @param	byNation 	Candidate's nation.
* @param	strNotice	The notice.
*/
void CDBAgent::UpdateCandidacyNoticeBoard(string & strCharID, uint8 byNation, string & strNotice)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, strNotice.c_str(), strNotice.length());
	if (!dbCommand->Execute(string_format(_T("{CALL KING_CANDIDACY_NOTICE_BOARD_PROC(?, %d, ?)}"), byNation)))
		ReportSQLError(m_GameDB->GetError());
}
#pragma endregion

#pragma region CDBAgent::UpdateNoahOrExpEvent(uint8 byType, uint8 byNation, uint8 byAmount, uint8 byDay, uint8 byHour, uint8 byMinute, uint16 sDuration, uint32 nCoins)

void CDBAgent::UpdateNoahOrExpEvent(uint8 byType, uint8 byNation, uint8 byAmount, uint8 byDay, uint8 byHour, uint8 byMinute, uint16 sDuration, uint32 nCoins)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	if (!dbCommand->Execute(string_format(_T("{CALL KING_UPDATE_NOAH_OR_EXP_EVENT(%d, %d, %d, %d, %d, %d, %d %d)}"), 
		byType, byNation, byAmount, byDay, byHour, byMinute, sDuration, nCoins)))
		ReportSQLError(m_GameDB->GetError());
}
#pragma endregion

#pragma region CDBAgent::UpdateKingSystemDB(uint8 byNation, uint32 nNationalTreasury, uint32 KingTax)

void CDBAgent::UpdateKingSystemDB(uint8 byNation, uint32 nNationalTreasury, uint32 KingTax)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	int16 sRet = -2;
	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &sRet);
	if (!dbCommand->Execute(string_format(_T("{? = CALL KING_UPDATE_DATABASE(%d, %d, %d)}"), byNation, nNationalTreasury, KingTax)))
		ReportSQLError(m_GameDB->GetError());
}

#pragma endregion

#pragma region CDBAgent::InsertPrizeEvent(uint8 byType, uint8 byNation, uint32 nCoins, std::string & strCharID)

void CDBAgent::InsertPrizeEvent(uint8 byType, uint8 byNation, uint32 nCoins, std::string & strCharID)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	int16 sRet = -2;
	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &sRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());
	if (!dbCommand->Execute(string_format(_T("{? = CALL KING_INSERT_PRIZE_EVENT(%d, %d, %d, ?)}"), 
		byType, byNation, nCoins)))
		ReportSQLError(m_GameDB->GetError());
}

#pragma endregion

#pragma region CDBAgent::InsertTaxEvent(uint8 opcode, uint8 TerritoryTariff, uint8 Nation, uint32 TerritoryTax)
void CDBAgent::InsertTaxEvent(uint8 opcode, uint8 TerritoryTariff, uint8 Nation, uint32 TerritoryTax)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	int16 sRet = -2;
	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &sRet);
	if (!dbCommand->Execute(string_format(_T("{? = CALL KING_CHANGE_TAX (%d, %d, %d, %d)}"),opcode, Nation, TerritoryTariff, TerritoryTax)))
		ReportSQLError(m_GameDB->GetError());
}
#pragma endregion

#pragma region CDBAgent::UpdateNationIntro(uint32 iServerNo, uint8 nation, std::string & strKingName, std::string &strKingNotice)
void CDBAgent::UpdateNationIntro(uint32 iServerNo, uint8 nation, std::string & strKingName, std::string &strKingNotice)
{
	unique_ptr<OdbcCommand> dbCommand(m_AccountDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	dbCommand->AddParameter(SQL_PARAM_INPUT, strKingName.c_str(), strKingName.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, strKingNotice.c_str(), strKingNotice.length());


	if(nation == 1)
	{
		if (!dbCommand->Execute(string_format(_T("UPDATE GAME_SERVER_LIST SET strKarusKingName = ?, strKarusNotice = ? WHERE sGroupID = %d"), iServerNo)))
		{
			ReportSQLError(m_AccountDB->GetError());
			return;
		}
	}
	else if(nation == 2)
	{
		if (!dbCommand->Execute(string_format(_T("UPDATE GAME_SERVER_LIST SET strElMoradKingName = ?, strElMoradNotice = ? WHERE sGroupID = %d"), iServerNo)))
		{
			ReportSQLError(m_AccountDB->GetError());
			return;
		}
	}
}

#pragma endregion



#pragma endregion

/**
* @brief	Resets the monthly NP total accumulated in the last month.
*/
void CDBAgent::ResetLoyaltyMonthly()
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	if (!dbCommand->Execute(_T("{CALL RESET_LOYALTY_MONTHLY}")))
		ReportSQLError(m_GameDB->GetError());
}

/**
* @brief	Clears the remaining users who were connected to this server
from the logged in user list that may still be there as the 
result of an improper shutdown.
*/
void CDBAgent::ClearRemainUsers()
{
	_ZONE_SERVERINFO * pInfo = g_pMain->m_ServerArray.GetData(g_pMain->m_nServerNo);
	if (pInfo == nullptr)
		return;

	unique_ptr<OdbcCommand> dbCommand(m_AccountDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	dbCommand->AddParameter(SQL_PARAM_INPUT, pInfo->strServerIP.c_str(), pInfo->strServerIP.length());
	if (!dbCommand->Execute(_T("{CALL CLEAR_REMAIN_USERS(?)}")))
		ReportSQLError(m_AccountDB->GetError());
}

void CDBAgent::InsertUserDailyOp(_USER_DAILY_OP * pUserDailyOp)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	dbCommand->AddParameter(SQL_PARAM_INPUT, pUserDailyOp->strUserId.c_str(), pUserDailyOp->strUserId.length());
	if (!dbCommand->Execute(string_format(_T("{CALL INSERT_USER_DAILY_OP(?, %d, %d, %d, %d, %d, %d, %d, %d)}"), 
		pUserDailyOp->ChaosMapTime, pUserDailyOp->UserRankRewardTime, pUserDailyOp->PersonalRankRewardTime, pUserDailyOp->KingWingTime)))
		ReportSQLError(m_GameDB->GetError());	
}

void CDBAgent::UpdateUserDailyOp(std::string strUserId, uint8 type, int32 sUnixTime)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	dbCommand->AddParameter(SQL_PARAM_INPUT, strUserId.c_str(), strUserId.length());
	if (!dbCommand->Execute(string_format(_T("{CALL UPDATE_USER_DAILY_OP(?, %d, %d)}"), type, sUnixTime)))
		ReportSQLError(m_GameDB->GetError());	
}

void CDBAgent::UpdateRanks()
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	if (!dbCommand->Execute(_T("{CALL UPDATE_RANKS}")))
		ReportSQLError(m_GameDB->GetError());
}

void CDBAgent::CheckBannedAccounts()
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	int16 sRet = -2;
	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &sRet);
	if (!dbCommand->Execute(_T("{? = CALL CHECK_BANNED_ACCOUNTS}")))
		ReportSQLError(m_GameDB->GetError());
}

int8 CDBAgent::NationTransfer(std::string strAccountID)
{
	int8 bRet = 3;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());

	if (!dbCommand->Execute(_T("{? = CALL ACCOUNT_NATION_TRANSFER(?)}")))
		ReportSQLError(m_GameDB->GetError());

	return bRet;
}

bool CDBAgent::GetAllCharInfo(string & strCharID, uint16 & nNum, _NATION_DATA* m_NationTransfer)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());

	if (!dbCommand->Execute(_T("SELECT USERDATA.strUserID, Race, Class, Face, HairRGB, Knights FROM USERDATA \
							   INNER JOIN USER_KNIGHTDATA ON USER_KNIGHTDATA.strUserID = USERDATA.strUserID \
							   WHERE USERDATA.strUserID = ?")))
	{
		ReportSQLError(m_GameDB->GetError());
		return false;
	}

	string dCharName;
	uint8 dRace, bFace;
	uint16 dClass, sClanID;
	uint32 nHair;

	if (dbCommand->hasData())
	{
		dbCommand->FetchString(1, dCharName);
		dbCommand->FetchByte(2, dRace);
		dbCommand->FetchUInt16(3, dClass);
		dbCommand->FetchByte(4, bFace);
		dbCommand->FetchUInt32(5, nHair);
		dbCommand->FetchUInt16(6, sClanID);
	}
	else
		return false;

	m_NationTransfer->nNum = nNum;
	m_NationTransfer->bCharName = dCharName;
	m_NationTransfer->bRace = dRace;
	m_NationTransfer->sClass = dClass;
	m_NationTransfer->bFace = bFace;
	m_NationTransfer->nHair = nHair;
	m_NationTransfer->sClanID = sClanID;

	nNum++;
	return true;
}

#pragma region CDBAgent::LoadBotTable()
bool CDBAgent::LoadBotTable()
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());

	if (dbCommand.get() == nullptr)
		return false;

	if (!dbCommand->Execute(_T("{CALL LOAD_USER_BOTS}")))
	{
		printf(" Error LOAD_USER_BOTS. \n");
		ReportSQLError(m_GameDB->GetError());
		return false;
	}

	if (!dbCommand->hasData())
	{
		printf(" Error LOAD_USER_BOTS. \n");
		return false;
	}

	do
	{
		CBot* pUser = new CBot();
		int field = 1;

		pUser->Initialize();

		char strItem[INVENTORY_TOTAL * 8];
		memset(strItem, 0x00, sizeof(strItem));

		dbCommand->FetchString(field++, pUser->m_strUserID);
		dbCommand->FetchByte(field++, pUser->m_bNation);
		dbCommand->FetchByte(field++, pUser->m_bRace);
		dbCommand->FetchUInt16(field++, pUser->m_sClass);
		dbCommand->FetchUInt32(field++, pUser->m_nHair);
		dbCommand->FetchByte(field++, pUser->m_bLevel);
		dbCommand->FetchByte(field++, pUser->m_bFace);
		dbCommand->FetchInt16(field++, pUser->m_bKnights);
		dbCommand->FetchByte(field++, pUser->m_bFame);
		dbCommand->FetchByte(field++, pUser->m_bZone);
		pUser->m_curx = (float)(dbCommand->FetchInt32(field++) / 100.0f);
		pUser->m_curz = (float)(dbCommand->FetchInt32(field++) / 100.0f);
		pUser->m_cury = (float)(dbCommand->FetchInt32(field++) / 100.0f);
		pUser->m_oldx = pUser->m_curx;
		pUser->m_oldy = pUser->m_cury;
		pUser->m_oldz = pUser->m_curz;


		memset(pUser->m_sItemArray, 0x00, sizeof(pUser->m_sItemArray));
		dbCommand->FetchBinary(field++, strItem, sizeof(strItem));
		ByteBuffer itemData;
		itemData.append(strItem, sizeof(strItem));

		for (int i = 0; i < INVENTORY_TOTAL; i++)
		{
			uint32 nItemID;
			uint16 sDurability, sCount;
			itemData >> nItemID >> sDurability >> sCount;
			_ITEM_DATA* pItem = &pUser->m_sItemArray[i];
			pItem->nNum = nItemID;
			pItem->sDuration = sDurability;
			pItem->sCount = sCount;
		}

		dbCommand->FetchUInt16(field++, pUser->m_sSid);
		dbCommand->FetchByte(field++, pUser->m_reblvl);
		dbCommand->FetchBinary(field++, (char*)pUser->m_bstrSkill, sizeof(pUser->m_bstrSkill));
		dbCommand->FetchUInt32(field++, pUser->m_iGold);
		dbCommand->FetchInt16(field++, pUser->m_sPoints);
		dbCommand->FetchByte(field++, pUser->m_bStats[STAT_STR]);
		dbCommand->FetchByte(field++, pUser->m_bStats[STAT_STA]);
		dbCommand->FetchByte(field++, pUser->m_bStats[STAT_DEX]);
		dbCommand->FetchByte(field++, pUser->m_bStats[STAT_INT]);
		dbCommand->FetchByte(field++, pUser->m_bStats[STAT_CHA]);
		pUser->m_sSid += MAX_USER;

		if (!g_pMain->m_BotArray.PutData(pUser->m_sSid, pUser))
			delete pUser;

	} while (dbCommand->MoveNext());

	return true;
}
#pragma endregion

#pragma region CDBAgent::NationTransferSaveUser(std::string & strAccountID, std::string & strCharID, uint16 & nNum, uint8 bnewRace, uint16 bNewClass, uint8 bNewNation, uint8 bNewFace, uint32 iNewHair)
int8 CDBAgent::NationTransferSaveUser(std::string & strAccountID, std::string & strCharID, uint16 & nNum, uint8 bnewRace, uint16 bNewClass, uint8 bNewNation, uint8 bNewFace, uint32 iNewHair)
{
	int8 bRet = 3;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());

	if (!dbCommand->Execute(string_format(_T("{? = CALL NATION_TRANSFER_SAVEUSER(?, ?, %d, %d, %d, %d, %d, %d)}"),nNum, bnewRace, bNewClass, bNewNation, bNewFace, iNewHair)))
		ReportSQLError(m_GameDB->GetError());

	return bRet;
}
#pragma endregion

#pragma region CDBAgent::UpdateAccountKnightCash(string & strAccountID, uint32 KnightCash)
/**
* @brief	Change knight cash is account.
*
* @param	strAccountID	Character's Account name.
* @param	KnightCash		Added knight cash amount
*/
void CDBAgent::UpdateAccountKnightCash(string & strAccountID, uint32 KnightCash)
{
	unique_ptr<OdbcCommand> dbCommand(m_AccountDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());
	if (!dbCommand->Execute(string_format(_T("{CALL UPDATE_KNIGHT_CASH (?, %d)}"), KnightCash)))
		ReportSQLError(m_AccountDB->GetError());	
}
#pragma endregion

#pragma region CDBAgent::UpdateSiegeWarfareDB(uint32 nMoradonTax, uint32 nDelosTax, uint32 nDungeonCharge)
void CDBAgent::UpdateSiegeWarfareDB(uint32 nMoradonTax, uint32 nDelosTax, uint32 nDungeonCharge)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	int16 sRet = -2;
	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &sRet);
	if (!dbCommand->Execute(string_format(_T("{? = CALL UPDATE_SIEGE_DATABASE(%d, %d, %d)}"), nMoradonTax, nDelosTax, nDungeonCharge)))
		ReportSQLError(m_GameDB->GetError());
}

void CDBAgent::UpdateSiege(int16 m_sCastleIndex, int16 m_sMasterKnights, int16 m_bySiegeType, int16 m_byWarDay, int16 m_byWarTime, int16 m_byWarMinute)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	if (!dbCommand->Execute(string_format(_T("{CALL UPDATE_SIEGE (%d, %d, %d, %d, %d, %d)}"), m_sCastleIndex, m_sMasterKnights, m_bySiegeType, m_byWarDay, m_byWarTime, m_byWarMinute)))
		ReportSQLError(m_AccountDB->GetError());	
}

void CDBAgent::UpdateSiegeTax(uint8 Zone, int16 ZoneTarrif)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());

	if (dbCommand.get() == nullptr)
		return;

	if (Zone == ZONE_DELOS)
	{
		if (!dbCommand->Execute(string_format(_T("UPDATE KNIGHTS_SIEGE_WARFARE SET sDellosTariff = %d"), ZoneTarrif)))
			ReportSQLError(m_GameDB->GetError());
	}
	else if (Zone >= ZONE_MORADON && Zone <= ZONE_MORADON5)
	{
		if (!dbCommand->Execute(string_format(_T("UPDATE KNIGHTS_SIEGE_WARFARE SET sMoradonTariff = %d"), ZoneTarrif)))
			ReportSQLError(m_GameDB->GetError());
	}
	else if (Zone == 0)
	{
		if (!dbCommand->Execute(string_format(_T("UPDATE KNIGHTS_SIEGE_WARFARE SET nDungeonCharge = 0, nMoradonTax = 0, nDellosTax = 0"))))
			ReportSQLError(m_GameDB->GetError());
	}
}
#pragma endregion

#pragma region CDBAgent::UpdateClanWarehouseData(uint16 & sClanID, UserUpdateType type)
void CDBAgent::UpdateClanWarehouseData(uint16 sClanID, UserUpdateType type)
{
	CKnights* pKnights = g_pMain->GetClanPtr(sClanID);
	if (pKnights == nullptr)
		return;

	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	// This *should* be padded like the database field is (unnecessarily), but I want to see how MSSQL responds.
	ByteBuffer ItemBuffer, SerialBuffer, ItemTimeBuffer;
	for (int i = 0; i < WAREHOUSE_MAX; i++)
	{
		_ITEM_DATA* pItem = &pKnights->m_sWarehouseArray[i];
		ItemBuffer << pItem->nNum << pItem->sDuration << pItem->sCount;
		SerialBuffer << pItem->nSerialNum;
		ItemTimeBuffer << pItem->nExpirationTime;
	}

	dbCommand->AddParameter(SQL_PARAM_INPUT, (char*)ItemBuffer.contents(), ItemBuffer.size(), SQL_BINARY);
	dbCommand->AddParameter(SQL_PARAM_INPUT, (char*)SerialBuffer.contents(), SerialBuffer.size(), SQL_BINARY);
	dbCommand->AddParameter(SQL_PARAM_INPUT, (char*)ItemTimeBuffer.contents(), ItemTimeBuffer.size(), SQL_BINARY);

	if (!dbCommand->Execute(string_format(_T("UPDATE KNIGHTS SET nMoney = %d, WarehouseData = ?, strSerial = ?, WarehouseDataTime = ? WHERE IDNum = %d"),
		pKnights->m_iBank, pKnights->GetID())))
	{
		ReportSQLError(m_GameDB->GetError());
		return;
	}
}
#pragma endregion

uint16 CDBAgent::GetClanIDWithCharID(string & strCharID)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());

	if (!dbCommand->Execute(_T("SELECT Knights FROM USERDATA WHERE strUserId = ?")))
	{
		ReportSQLError(m_GameDB->GetError());
		return -1;
	}

	if (!dbCommand->hasData())
		return -1;

	uint16 sKnights = -1;
	dbCommand->FetchUInt16(1, sKnights);

	return sKnights;
}

void CDBAgent::LoadLadderRankList(Packet & result, uint8 bNation, uint8& count)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());

	if (dbCommand.get() == nullptr)
		return;

	if (!dbCommand->Execute(string_format(_T("SELECT TOP(10)USERDATA.strUserID,  ISNULL(Knights.IDNum, 0 ), ISNULL(Knights.IDName, '' ), ISNULL(Knights.sMarkVersion, '' ) FROM USERDATA FULL OUTER JOIN Knights ON Knights.IDNum = USERDATA.Knights where USERDATA.strUserID IS NOT NULL and USERDATA.Nation = %d and USERDATA.Authority = 1 order by USERDATA.LoyaltyMonthly desc"), bNation)))
	{
		ReportSQLError(m_GameDB->GetError());
		return;
	}

	if (!dbCommand->hasData())
		return;

	int i = 1101;
	do
	{
		uint16 sKnightsIndex, sMarkVersion;
		string strKnightsName, strChief;

		dbCommand->FetchString(1, strChief);
		dbCommand->FetchUInt16(2, sKnightsIndex);
		dbCommand->FetchString(3, strKnightsName);
		dbCommand->FetchUInt16(4, sMarkVersion);

		result << uint16(i++)
			<< strChief
			<< uint16(0)
			<< sKnightsIndex
			<< sMarkVersion
			<< strKnightsName
			<< uint16(0);
		count++;
		if (i >= 1110) break;
	} while (dbCommand->MoveNext());
}

void CDBAgent::LoadKnightsLeaderList(Packet & result, uint8 bNation, uint8& count)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	if (!dbCommand->Execute(_T("SELECT IDNum, Chief, IDName, sMarkVersion, Nation FROM KNIGHTS WHERE Chief IN (SELECT strUserID FROM USERDATA WHERE Authority = 1) ORDER BY Flag DESC, ClanPointFund DESC, Points DESC")))
	{
		ReportSQLError(m_GameDB->GetError());
		return;
	}

	if (!dbCommand->hasData())
		return;

	int i = 1201;
	do
	{
		uint16 sKnightsIndex, sMarkVersion;
		string strKnightsName, strChief;
		uint8 Nation;

		dbCommand->FetchUInt16(1, sKnightsIndex);
		dbCommand->FetchString(2, strChief);
		dbCommand->FetchString(3, strKnightsName);
		dbCommand->FetchUInt16(4, sMarkVersion);
		dbCommand->FetchByte(5, Nation);

		if (bNation != Nation) continue;

		result << uint16(i++)
			<< strChief
			<< uint16(0)
			<< sKnightsIndex
			<< sMarkVersion
			<< strKnightsName
			<< uint16(0);

		count++;
		if (i >= 1210) break;
	} while (dbCommand->MoveNext());
}

#pragma region Knights Related Database Functions

#pragma region CDBAgent::LoadAllKnights(KnightsArray &m_KnightsArray)
/**
* @brief Loads all Knights data on server start.
* 
* @param m_KnightsArray	the Knights array used by the GameServerDLG.
*/
bool CDBAgent::LoadAllKnights(KnightsArray &m_KnightsArray)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	if (!dbCommand->Execute(_T("{CALL KNIGHTS_LOAD}")))	
	{
		ReportSQLError(m_GameDB->GetError());
		return false;
	}

	if (!dbCommand->hasData())
		return true;

	do
	{
		char strItem[WAREHOUSE_MAX * 8], strSerial[WAREHOUSE_MAX * 8], strItemExpiration[WAREHOUSE_MAX * 4];

		CKnights *pData = new CKnights();
		int i = 1;
		dbCommand->FetchUInt16(i++, pData->m_sIndex);
		dbCommand->FetchByte(i++, pData->m_byFlag);
		dbCommand->FetchByte(i++, pData->m_byNation);
		dbCommand->FetchByte(i++, pData->m_byRanking);
		dbCommand->FetchString(i++, pData->m_strName);
		dbCommand->FetchUInt64(i++, pData->m_nMoney);
		dbCommand->FetchUInt16(i++, pData->m_sDomination);
		dbCommand->FetchBinary(i++, pData->m_Image, sizeof(pData->m_Image));
		dbCommand->FetchUInt16(i++, pData->m_sMarkVersion);
		dbCommand->FetchUInt16(i++, pData->m_sMarkLen);
		dbCommand->FetchUInt16(i++, pData->m_sCape);
		dbCommand->FetchByte(i++, pData->m_bCapeR);
		dbCommand->FetchByte(i++, pData->m_bCapeG);
		dbCommand->FetchByte(i++, pData->m_bCapeB);
		dbCommand->FetchUInt16(i++, pData->m_sAlliance);
		dbCommand->FetchUInt32(i++, (uint32 &) pData->m_nClanPointFund);
		dbCommand->FetchString(i++, pData->m_strClanNotice);
		dbCommand->FetchByte(i++, pData->m_sClanPointMethod);
		dbCommand->FetchByte(i++, pData->bySiegeFlag);
		dbCommand->FetchUInt16(i++, pData->nLose);
		dbCommand->FetchUInt16(i++, pData->nVictory);
		dbCommand->FetchByte(i++, pData->sPremiumInUse);
		dbCommand->FetchUInt32(i++, pData->sPremiumTime);

		dbCommand->FetchUInt32(i++, pData->m_iBank);
		dbCommand->FetchBinary(i++, strItem, sizeof(strItem));
		dbCommand->FetchBinary(i++, strSerial, sizeof(strSerial));
		dbCommand->FetchBinary(i++, strItemExpiration, sizeof(strItemExpiration));

		ByteBuffer ItemBuffer, SerialBuffer, ItemTimeBuffer;
		ItemBuffer.append(strItem, sizeof(strItem));
		SerialBuffer.append(strSerial, sizeof(strSerial));
		ItemTimeBuffer.append(strItemExpiration, sizeof(strItemExpiration));

		memset(pData->m_sWarehouseArray, 0x00, sizeof(pData->m_sWarehouseArray));

		for (int i = 0; i < WAREHOUSE_MAX; i++)
		{
			uint64 nSerialNum;
			uint32 nItemID, nItemTime;
			int16 sDurability, sCount;

			ItemBuffer >> nItemID >> sDurability >> sCount;
			SerialBuffer >> nSerialNum;
			ItemTimeBuffer >> nItemTime;

			_ITEM_TABLE* pTable = g_pMain->GetItemPtr(nItemID);
			if (pTable == nullptr || sCount <= 0)
				continue;

			if (!pTable->m_bCountable && sCount > 1)
				sCount = 1;
			else if (sCount > ITEMCOUNT_MAX)
				sCount = ITEMCOUNT_MAX;

			pData->m_sWarehouseArray[i].nNum = nItemID;
			pData->m_sWarehouseArray[i].sDuration = sDurability;
			pData->m_sWarehouseArray[i].sCount = sCount;
			pData->m_sWarehouseArray[i].nSerialNum = nSerialNum;
			pData->m_sWarehouseArray[i].nExpirationTime = nItemTime;

			if (g_pMain->IsDuplicateItem(nItemID, nSerialNum))
				pData->m_sWarehouseArray[i].bFlag = ITEM_FLAG_DUPLICATE;
		}

		if (!m_KnightsArray.PutData(pData->m_sIndex, pData))
			delete pData;

	} while (dbCommand->MoveNext());

	if (!dbCommand->Execute(_T("{CALL KNIGHTS_LOAD_MEMBERSDATA}")))	
	{
		ReportSQLError(m_GameDB->GetError());
		return false;
	}

	if (!dbCommand->hasData())
		return true;

	uint16 sIDNum, sClass;
	std::string strUserID, strMemo;
	uint32 nDonatedNP;
	uint8 fame, level;
	int32 lastLogin, Loyalty;
	do
	{
		dbCommand->FetchUInt16(1, sIDNum);
		dbCommand->FetchString(2, strUserID);
		dbCommand->FetchUInt32(3, nDonatedNP);
		dbCommand->FetchString(4, strMemo);
		dbCommand->FetchByte(5, fame);
		dbCommand->FetchUInt16(6, sClass);
		dbCommand->FetchByte(7, level);
		dbCommand->FetchInt32(8, lastLogin);
		dbCommand->FetchInt32(9, Loyalty);

		CKnightsManager::AddKnightsUser(sIDNum, strUserID,strMemo, fame, sClass, level, lastLogin, Loyalty);
		CKnightsManager::AddUserDonatedNP(sIDNum, strUserID, nDonatedNP, false);

	} while (dbCommand->MoveNext());

	return true;
}
#pragma endregion

#pragma region CDBAgent::CreateKnights(uint16 sClanID, uint8 bNation, string & strKnightsName, string & strChief, uint8 bFlag)

int8 CDBAgent::CreateKnights(uint16 sClanID, uint8 bNation, string & strKnightsName, string & strChief, uint8 bFlag)
{
	int8 bRet = -1;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return bRet;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strKnightsName.c_str(), strKnightsName.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, strChief.c_str(), strChief.length());

	if (!dbCommand->Execute(string_format(_T("{? = CALL CREATE_KNIGHTS(%d, %d, %d, ?, ?)}"), sClanID, bNation, bFlag)))
		ReportSQLError(m_GameDB->GetError());

	return bRet;
}
#pragma endregion

#pragma region CDBAgent::KnightsMemberJoin(string & strCharID, uint16 sClanID)

int CDBAgent::KnightsMemberJoin(string & strCharID, uint16 sClanID)
{
	int16 sRet = -1;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return sRet;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &sRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());

	if (!dbCommand->Execute(string_format(_T("{? = CALL KNIGHTS_MEMBER_JOIN(?, %d)}"), sClanID)))
		ReportSQLError(m_GameDB->GetError());

	return sRet;
}
#pragma endregion

#pragma region CDBAgent::KnightsMemberRemove(string & strCharID, uint16 sClanID)

int CDBAgent::KnightsMemberRemove(string & strCharID, uint16 sClanID)
{
	int16 sRet = -1;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return sRet;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &sRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());

	if (!dbCommand->Execute(string_format(_T("{? = CALL KNIGHTS_MEMBER_REMOVE(?, %d)}"), sClanID)))
		ReportSQLError(m_GameDB->GetError());

	return sRet;
}
#pragma endregion

#pragma region CDBAgent::KnightsMemberLeave(string & strCharID, uint16 sClanID)

int CDBAgent::KnightsMemberLeave(string & strCharID, uint16 sClanID)
{
	int16 sRet = -1;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return sRet;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &sRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());

	if (!dbCommand->Execute(string_format(_T("{? = CALL KNIGHTS_MEMBER_LEAVE(?, %d)}"), sClanID)))
		ReportSQLError(m_GameDB->GetError());

	return sRet;
}
#pragma endregion

#pragma region CDBAgent::KnightsMemberAdmit(string & strCharID, uint16 sClanID)

int CDBAgent::KnightsMemberAdmit(string & strCharID, uint16 sClanID)
{
	int16 sRet = -1;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return sRet;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &sRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());

	if (!dbCommand->Execute(string_format(_T("{? = CALL KNIGHTS_MEMBER_ADMIT(?, %d)}"), sClanID)))
		ReportSQLError(m_GameDB->GetError());

	return sRet;
}

#pragma endregion

#pragma region CDBAgent::KnightsMemberReject(string & strCharID, uint16 sClanID)

int CDBAgent::KnightsMemberReject(string & strCharID, uint16 sClanID)
{
	int16 sRet = -1;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return sRet;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &sRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());

	if (!dbCommand->Execute(string_format(_T("{? = CALL KNIGHTS_MEMBER_REJECT(?, %d)}"), sClanID)))
		ReportSQLError(m_GameDB->GetError());

	return sRet;
}

#pragma endregion

#pragma region CDBAgent::KnightsMemberChief(string & strCharID, uint16 sClanID)

int CDBAgent::KnightsMemberChief(string & strCharID, uint16 sClanID)
{
	int16 sRet = -1;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return sRet;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &sRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());

	if (!dbCommand->Execute(string_format(_T("{? = CALL KNIGHTS_MEMBER_CHIEF(?, %d)}"), sClanID)))
		ReportSQLError(m_GameDB->GetError());

	return sRet;
}

#pragma endregion

#pragma region CDBAgent::KnightsMemberViceChief(string & strCharID, uint16 sClanID)

int CDBAgent::KnightsMemberViceChief(string & strCharID, uint16 sClanID)
{
	int16 sRet = -1;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return sRet;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &sRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());

	if (!dbCommand->Execute(string_format(_T("{? = CALL KNIGHTS_MEMBER_VICECHIEF(?, %d)}"), sClanID)))
		ReportSQLError(m_GameDB->GetError());

	return sRet;
}

#pragma endregion

#pragma region CDBAgent::KnightsMemberOfficer(string & strCharID, uint16 sClanID)

int CDBAgent::KnightsMemberOfficer(string & strCharID, uint16 sClanID)
{
	int16 sRet = -1;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return sRet;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &sRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());

	if (!dbCommand->Execute(string_format(_T("{? = CALL KNIGHTS_MEMBER_OFFICER(?, %d)}"), sClanID)))
		ReportSQLError(m_GameDB->GetError());

	return sRet;
}

#pragma endregion

#pragma region CDBAgent::KnightsMemberPunish(string & strCharID, uint16 sClanID)

int CDBAgent::KnightsMemberPunish(string & strCharID, uint16 sClanID)
{
	int16 sRet = -1;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return sRet;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &sRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());

	if (!dbCommand->Execute(string_format(_T("{? = CALL KNIGHTS_MEMBER_PUNISH(?, %d)}"), sClanID)))
		ReportSQLError(m_GameDB->GetError());

	return sRet;
}

#pragma endregion

#pragma region CDBAgent::KnightsPointMethodChange(uint16 sClanID, uint8 bDomination)

int CDBAgent::KnightsPointMethodChange(uint16 sClanID, uint8 bDomination)
{
	int16 sRet = -1;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return sRet;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &sRet);

	if (!dbCommand->Execute(string_format(_T("{? = CALL KNIGHTS_POINT_METHOD_CHANGE(%d, %d)}"), sClanID, bDomination)))
		ReportSQLError(m_GameDB->GetError());

	return sRet;
}

#pragma endregion

#pragma region CDBAgent::KnightsDestroy(uint16 sClanID)

int CDBAgent::KnightsDestroy(uint16 sClanID)
{
	int16 sRet = -1;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return sRet;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &sRet);
	if (!dbCommand->Execute(string_format(_T("{? = CALL KNIGHTS_DESTROY (%d)}"), sClanID)))
		ReportSQLError(m_GameDB->GetError());

	return sRet;
}
#pragma endregion

#pragma region CDBAgent::KnightsSymbolUpdate(uint16 sClanID, uint16 sSymbolSize, char *clanSymbol)

int CDBAgent::KnightsSymbolUpdate(uint16 sClanID, uint16 sSymbolSize, char *clanSymbol)
{
	int16 sRet = 0;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return sRet;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &sRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, clanSymbol, MAX_KNIGHTS_MARK, SQL_BINARY);
	if (!dbCommand->Execute(string_format(_T("{? = CALL KNIGHTS_UPDATE_MARK(%d, ?, %d)}"), sClanID, sSymbolSize)))
		ReportSQLError(m_GameDB->GetError());

	return sRet;
}
#pragma endregion

#pragma region CDBAgent::KnightsCapeUpdate(uint16 sClanID, uint16 sCapeID, uint8 r, uint8 g, uint8 b)
/**
* @brief	Handles clan cape database updates.
*
* @param	sClanID	Identifier for the clan.
* @param	sCapeID	Identifier for the cape.
* @param	r 	Red colour component.
* @param	g 	Green colour component.
* @param	b 	Blue colour component.
*
* @return	true if it succeeds, false if it fails.
*/
int CDBAgent::KnightsCapeUpdate(uint16 sClanID, uint16 sCapeID, uint8 r, uint8 g, uint8 b)
{
	int16 sRet = 1;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return sRet;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &sRet);
	if (!dbCommand->Execute(string_format(_T("{? = CALL KNIGHTS_UPDATE_CAPE (%d, %d, %d, %d, %d)}"), sClanID, sCapeID, r, g, b)))
		ReportSQLError(m_GameDB->GetError());

	return sRet;
}

#pragma endregion

#pragma region CDBAgent::KnightsCastellanCapeUpdate(uint8 type, uint16 sClanID, uint16 sCapeID, uint8 r, uint8 g, uint8 b)
/**
* @brief	Handles clan cape database updates.
*
* @param	sClanID	Identifier for the clan.
* @param	sCapeID	Identifier for the cape.
* @param	r 	Red colour component.
* @param	g 	Green colour component.
* @param	b 	Blue colour component.
*
* @return	true if it succeeds, false if it fails.
*/
int CDBAgent::KnightsCastellanCapeUpdate(uint16 sClanID, uint16 sCapeID, uint8 r, uint8 g, uint8 b, uint64 time, uint8 Active)
{
	int16 sRet = 1;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return sRet;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &sRet);
	if (!dbCommand->Execute(string_format(_T("{? = CALL KNIGHTS_CASTELLAN_UPDATE_CAPE (%d, %d, %d, %d, %d, " I64FMTD ", %d)}"), sClanID, sCapeID, r, g, b, time, Active)))
		ReportSQLError(m_GameDB->GetError());

	return sRet;
}

#pragma endregion

#pragma region CDBAgent::KnightsHandover(std::string & strCharID, uint16 sClanID)

int CDBAgent::KnightsHandover(std::string & strCharID, uint16 sClanID)
{
	int16 sRet = -1;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return sRet;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &sRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());

	if (!dbCommand->Execute(string_format(_T("{? = CALL KNIGHTS_HANDOVER(?, %d)}"), sClanID)))
		ReportSQLError(m_GameDB->GetError());

	return sRet;
}

#pragma endregion

#pragma region CDBAgent::KnightsGradeUpdate(uint16 sClanID, uint8 byFlag, uint16 sCapeID)
/**
* @brief	Updates the clan grade.
*
* @param	sClanID	Identifier for the clan.
* @param	byFlag 	The clan type (training, promoted, etc).
* @param	sCapeID	Identifier for the cape.
*/
int CDBAgent::KnightsGradeUpdate(uint16 sClanID, uint8 byFlag, uint16 sCapeID)
{
	int16 sRet = 1;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return sRet;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &sRet);

	if (!dbCommand->Execute(string_format(_T("{? = CALL KNIGHTS_UPDATE_GRADE(%d, %d, %d)}"), sClanID, sCapeID, byFlag)))
		ReportSQLError(m_GameDB->GetError());

	return sRet;
}
#pragma endregion

#pragma region CDBAgent::KnightsClanNoticeUpdate(uint16 sClanID, std::string & strClanNotice)

/**
* @brief	Updates the clan notice.
*
* @param	sClanID		 	Identifier for the clan.
* @param	strClanNotice	The clan notice.
*/
int CDBAgent::KnightsClanNoticeUpdate(uint16 sClanID, std::string & strClanNotice)
{
	int16 sRet = 1;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return sRet;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &sRet);
	if (strClanNotice.empty())
	{
		if (!dbCommand->Execute(string_format(_T("UPDATE KNIGHTS SET strClanNotice = NULL WHERE IDNum=%d"), sClanID)))
		{
			ReportSQLError(m_GameDB->GetError());
			return sRet;
		}
		return 0;
	}
	else
	{
		dbCommand->AddParameter(SQL_PARAM_INPUT, strClanNotice.c_str(), strClanNotice.length());
		if (!dbCommand->Execute(string_format(_T("{? = CALL KNIGHTS_UPDATE_CLANNOTICE(%d,?)}"), sClanID)))
			ReportSQLError(m_GameDB->GetError());
	}
	return sRet;
}
#pragma endregion

#pragma region CDBAgent::KnightsAllianceNoticeUpdate(uint16 sClanID, std::string & strClanNotice)

/**
* @brief	Updates the clan notice.
*
* @param	sClanID		 	Identifier for the clan.
* @param	strClanNotice	The clan notice.
*/
int CDBAgent::KnightsAllianceNoticeUpdate(uint16 sClanID, std::string & strClanNotice)
{
	int16 sRet = 1;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return sRet;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &sRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strClanNotice.c_str(), strClanNotice.length());
	if (!dbCommand->Execute(string_format(_T("{? = CALL KNIGHTS_ALLIANCE_UPDATE_ALLIANCENOTICE(%d,?)}"), sClanID)))
		ReportSQLError(m_GameDB->GetError());

	return sRet;
}
#pragma endregion

#pragma region CDBAgent::KnightsFundUpdate(uint16 sClanID, uint32 nClanPointFund)
/**
* @brief	Updates the clan fund.
*
* @param	sClanID		  	Identifier for the clan.
* @param	nClanPointFund	The current clan point fund.
*/
int CDBAgent::KnightsFundUpdate(uint16 sClanID, uint32 nClanPointFund)
{
	int16 sRet = -1;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return sRet;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &sRet);

	if (!dbCommand->Execute(string_format(_T("{? = CALL KNIGHTS_UPDATE_FUND(%d, %d)}"), sClanID, nClanPointFund)))
		ReportSQLError(m_GameDB->GetError());

	return sRet;
}
#pragma endregion

#pragma region CDBAgent::KnightsUserMemoUpdate(std::string strUserID, std::string strMemo)
/**
* @brief	Updates the user memo
*
* @param	strUserID	  	the Identifier.
* @param	strMemo			the Memo to be saved..
*/
int CDBAgent::KnightsUserMemoUpdate(std::string strUserID, std::string strMemo)
{
	int16 sRet = 1;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return sRet;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &sRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strUserID.c_str(), strUserID.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, strMemo.c_str(), strMemo.length());
	if (!dbCommand->Execute(string_format(_T("{? = CALL KNIGHTS_UPDATE_USERMEMO(?,?)}"))))
		ReportSQLError(m_GameDB->GetError());

	return sRet;
}
#pragma endregion

#pragma region CDBAgent::KnightsUserDonate(CUser * pUser,  uint32 amountNP, uint32 UserAmountNP)
/**
* @brief	Donates (clanPoints) clan points to the specified user's clan.
* 			Also increases the user's total NP donated.
*
* @param	pUser	  	The donor user.
* @param	amountNP  	The number of national points being donated by the user.
*
* @return	true if it succeeds, false if it fails.
*/
int CDBAgent::KnightsUserDonate(CUser * pUser,  uint32 amountNP, uint32 UserAmountNP)
{
	int16 sRet = 1;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return sRet;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &sRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, pUser->m_strUserID.c_str(), pUser->m_strUserID.length());
	if (!dbCommand->Execute(string_format(_T("{? = CALL KNIGHTS_MEMBER_DONATE(?, %d, %d, %d)}"), pUser->GetClanID(), amountNP, UserAmountNP)))
		ReportSQLError(m_GameDB->GetError());

	return sRet;
}

#pragma endregion

#pragma region CDBAgent::LoadKnightsAllMembers(uint16 sClanID, Packet & result)

uint16 CDBAgent::LoadKnightsAllMembers(uint16 sClanID, Packet & result)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return 0;

	int16 sRet = -2;
	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &sRet);

	//if (!dbCommand->Execute(string_format(_T("SELECT strUserID, Fame, [Level], Class FROM USERDATA WHERE Knights = %d"), sClanID)))
	if (!dbCommand->Execute(string_format(_T("{? = CALL KNIGHTS_LOAD_MEMBERS (%d)}"), sClanID)))	
		ReportSQLError(m_GameDB->GetError());

	if (!dbCommand->hasData())
		return 0;

	uint16 count = 0;
	do
	{
		string strCharID; uint16 sClass; uint8 bFame, bLevel;
		dbCommand->FetchString(1, strCharID);
		dbCommand->FetchByte(2, bFame);
		dbCommand->FetchByte(3, bLevel);
		dbCommand->FetchUInt16(4, sClass);

		result << strCharID << bFame << bLevel << sClass 
			// check if user's logged in (i.e. grab logged in state)
			<< uint8(g_pMain->GetUserPtr(strCharID, TYPE_CHARACTER) == nullptr ? 0 : 1);
		count++;
	} while (dbCommand->MoveNext());

	return count;
}
#pragma endregion

#pragma region CDBAgent::LoadKnightsAllList()

void CDBAgent::LoadKnightsAllList()
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	const tstring szSQL = _T("SELECT IDNum, Points, Ranking FROM KNIGHTS WHERE Points != 0 ORDER BY Points DESC"); 

	if (dbCommand.get() == nullptr)
		return;

	if (!dbCommand->Execute(szSQL))
		ReportSQLError(m_GameDB->GetError());

	if (!dbCommand->hasData())
		return;

	Packet result(WIZ_KNIGHTS_PROCESS);
	uint8 bCount = 0;
	int offset;

	do
	{
		if (bCount == 0)
		{
			result.clear();
			offset = (int)result.wpos();
			result << uint8(0);
		}

		uint32 nPoints; uint16 sClanID; uint8 bRanking;
		dbCommand->FetchUInt16(1, sClanID);
		dbCommand->FetchUInt32(2, nPoints);
		dbCommand->FetchByte(3, bRanking);

		result << sClanID << nPoints << bRanking;

		// only send 100 clans at a time (no shared memory limit, yay!)
		if (++bCount >= 100)
		{
			// overwrite the count
			result.put(offset, bCount);

			CKnightsManager::RecvKnightsAllList(result);
			bCount = 0;
		}
	} while (dbCommand->MoveNext());

	// didn't quite make it in the last batch (if any)? send the remainder.
	if (bCount < 100)
	{
		result.put(offset, bCount);
		CKnightsManager::RecvKnightsAllList(result);
	}
}
#pragma endregion

#pragma region CDBAgent::KnightsSave(uint16 sClanID, uint8 bFlag, uint32 nClanFund)
/**
* @brief	Donates (clanPoints) clan points to the specified user's clan.
* 			Also increases the user's total NP donated.
*
* @param	sClanID	  	The donor user.
* @param	bFlag  		The clan flag.
* @param	nClanFund  	The total clan fund.
*
* @return	true if it succeeds, false if it fails.
*/
int CDBAgent::KnightsSave(uint16 sClanID, uint8 bFlag, uint32 nClanFund)
{
	int16 sRet = 1;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return sRet;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &sRet);
	if (!dbCommand->Execute(string_format(_T("{? = CALL KNIGHTS_SAVE(%d, %d, %d)}"), sClanID, bFlag, nClanFund)))
		ReportSQLError(m_GameDB->GetError());

	return sRet;
}

#pragma endregion

#pragma region CDBAgent::KnightsRefundNP(string & strUserID, uint32 nRefundNP)
/**
* @brief	Refunds the specified amount of NP to a logged out character.
*
* @param	strUserID	Character's name.
* @param	nRefundNP	The amount of NP to refund.
*/
int CDBAgent::KnightsRefundNP(string & strUserID, uint32 nRefundNP)
{
	int16 sRet = 1;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return sRet;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &sRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strUserID.c_str(), strUserID.length());
	if (!dbCommand->Execute(string_format(_T("{? = CALL KNIGHTS_MEMBER_REFUNDNP(?, %d)}"), nRefundNP)))
		ReportSQLError(m_GameDB->GetError());

	return sRet;
}
#pragma endregion

#pragma endregion
/////////////////////////////////

/////////////////////////////////
#pragma region Knight Alliance Related Database Methods

#pragma region CDBAgent::KnightsAllianceCreate(uint16 sMainClanID, uint16 sSubClanID, uint8  byEmptyIndex)
/**
* @brief	Initiates a new alliance creation progress.
*
* @param	sMainClanID	the alliance index.
* @param	sSubClanID the knights index.
* @param	byEmptyIndex
*/
int CDBAgent::KnightsAllianceCreate(uint16 sMainClanID, uint16 sSubClanID, uint8  byEmptyIndex)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return 0;

	int16 sRet = 1;
	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &sRet);
	if (!dbCommand->Execute(string_format(_T("{? = CALL KNIGHTS_ALLIANCE_CREATE(%d, %d, %d)}"), sMainClanID, sSubClanID, byEmptyIndex)))
		ReportSQLError(m_GameDB->GetError());

	return sRet;
}
#pragma endregion

#pragma region CDBAgent::KnightsAllianceInsert(uint16 sMainClanID, uint16 sSubClanID, uint8  byEmptyIndex)
/**
* @brief	Inserts a knight to an existing alliance.
*
* @param	sMainClanID	the alliance index.
* @param	sSubClanID the knights index.
* @param	byEmptyIndex
*/
int CDBAgent::KnightsAllianceInsert(uint16 sMainClanID, uint16 sSubClanID, uint8  byEmptyIndex)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return 0;

	int16 sRet = 1;
	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &sRet);
	if (!dbCommand->Execute(string_format(_T("{? = CALL KNIGHTS_ALLIANCE_INSERT(%d, %d, %d)}"), sMainClanID, sSubClanID, byEmptyIndex)))
		ReportSQLError(m_GameDB->GetError());

	return sRet;
}
#pragma endregion

#pragma region CDBAgent::KnightsAllianceDestroy(uint16 sAllianceID)
/**
* @brief	Destroys the alliance.
*
* @param	sAllianceID	the alliance index.
*/
int CDBAgent::KnightsAllianceDestroy(uint16 sAllianceID)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return 0;

	int16 sRet = 1;
	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &sRet);
	if (!dbCommand->Execute(string_format(_T("{? = CALL KNIGHTS_ALLIANCE_DESTROY(%d)}"), sAllianceID)))
		ReportSQLError(m_GameDB->GetError());

	return sRet;
}
#pragma endregion

#pragma region CDBAgent::KnightsAllianceRemove(uint16 sMainClanID, uint16 sSubClanID)
/**
* @brief	Removes a knight from the alliance.
*
* @param	sMainClanID	the alliance index.
* @param	sSubClanID the knights index.
*/
int CDBAgent::KnightsAllianceRemove(uint16 sMainClanID, uint16 sSubClanID)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return 0;

	int16 sRet = 1;
	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &sRet);
	if (!dbCommand->Execute(string_format(_T("{? = CALL KNIGHTS_ALLIANCE_REMOVE(%d, %d)}"), sMainClanID, sSubClanID)))
		ReportSQLError(m_GameDB->GetError());

	return sRet;
}
#pragma endregion

#pragma region CDBAgent::KnightsAlliancePunish(uint16 sMainClanID, uint16 sSubClanID)
/**
* @brief	Punishes a knight from the alliance.
*
* @param	sMainClanID	the alliance index.
* @param	sSubClanID the knights index.
*/
int CDBAgent::KnightsAlliancePunish(uint16 sMainClanID, uint16 sSubClanID)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return 0;

	int16 sRet = 1;
	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &sRet);
	if (!dbCommand->Execute(string_format(_T("{? = CALL KNIGHTS_ALLIANCE_PUNISH(%d, %d)}"), sMainClanID, sSubClanID)))
		ReportSQLError(m_GameDB->GetError());

	return sRet;
}
#pragma endregion

bool CDBAgent::SaveSheriffReports(std::string ReportedUser, std::string ReportedSheriff, std::string Crime, uint8 Yes, std::string SheriffA, uint8 No, std::string ReportDateTime)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	dbCommand->AddParameter(SQL_PARAM_INPUT, ReportedUser.c_str(), ReportedUser.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, ReportedSheriff.c_str(), ReportedSheriff.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, Crime.c_str(), Crime.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, SheriffA.c_str(), SheriffA.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, ReportDateTime.c_str(), ReportDateTime.length());


	if (!dbCommand->Execute(string_format(_T("{CALL SHERIF_REPORTS_SAVE(?, ?, ?, %d, ?, %d, ?)}"), Yes, No)))
	{
		ReportSQLError(m_GameDB->GetError());
		return false;
	}

	return true;
}

bool CDBAgent::UpdateSheriffTable(uint8 Type, std::string ReportedUser, uint8 Yes, uint8 No, std::string Sheriff)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	dbCommand->AddParameter(SQL_PARAM_INPUT, ReportedUser.c_str(), ReportedUser.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, Sheriff.c_str(), Sheriff.length());

	if (!dbCommand->Execute(string_format(_T("{CALL UPDATE_SHERIFF_REPORTS(%d, ?, %d, %d, ?)}"), Type, Yes, No)))
	{
		ReportSQLError(m_GameDB->GetError());
		return false;
	}

	if (Yes > 2)
	{
		auto pSelected = g_pMain->GetUserPtr(ReportedUser, TYPE_CHARACTER);
		if (pSelected && pSelected->isInGame()) // In game or Not we send him to the prison anyhow :)
			pSelected->ZoneChange(ZONE_PRISON, 167, 125); // Random for now
		g_pMain->m_SheriffArray.erase(ReportedUser);
	}
	else if (No >= 2)
	{
		g_pMain->m_SheriffArray.erase(ReportedUser);
	}
	return true;
}


#pragma endregion

#pragma region CDBAgent::SetAllCharID(string & strAccountID, string & strCharID1, string & strCharID2, string & strCharID3, string & strCharID4)
bool CDBAgent::SetAllCharID(string & strAccountID, string & strCharID1, string & strCharID2, string & strCharID3, string & strCharID4)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	string ID1, ID2, ID3, ID4;

	if (strCharID1 == "")
		ID1 = "";
	else
		ID1 = strCharID1;

	if (strCharID2 == "")
		ID2 = "";
	else
		ID2 = strCharID2;

	if (strCharID3 == "")
		ID3 = "";
	else
		ID3 = strCharID3;

	if (strCharID4 == "")
		ID4 = "";
	else
		ID4 = strCharID4;

	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, ID1.c_str(), ID1.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, ID2.c_str(), ID2.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, ID3.c_str(), ID3.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, ID4.c_str(), ID4.length());

	if (!dbCommand->Execute(_T("{CALL UPDATE_ALL_CHAR_ID("
		"?, ?, ?, ?, ?)}")))
	{
		ReportSQLError(m_GameDB->GetError());
		return false;
	}

	return true;
}
#pragma endregion

#pragma region CDBAgent::UpdateAchieveData(CUser *pUser)
bool CDBAgent::UpdateAchieveData(CUser *pUser)
{
	if (pUser == nullptr)
		return false;

	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	// achieve system
	char strAchieve[ACHIEVE_ARRAY_SIZE];
	memset(strAchieve, 0, sizeof(strAchieve));

	int index2 = 36;

	*(uint32 *)(strAchieve) = pUser->m_AchieveInfo.PlayTime;
	*(uint32 *)(strAchieve + 4) = pUser->m_AchieveInfo.MonsterDefeatCount;
	*(uint32 *)(strAchieve + 8) = pUser->m_AchieveInfo.UserDefeatCount;
	*(uint32 *)(strAchieve + 12) = pUser->m_AchieveInfo.UserDeathCount;
	*(uint32 *)(strAchieve + 16) = pUser->m_AchieveInfo.TotalMedal;
	*(uint16 *)(strAchieve + 20) = pUser->m_AchieveInfo.RecentAchieve[0];
	*(uint16 *)(strAchieve + 22) = pUser->m_AchieveInfo.RecentAchieve[1];
	*(uint16 *)(strAchieve + 24) = pUser->m_AchieveInfo.RecentAchieve[2];
	*(uint16 *)(strAchieve + 26) = pUser->m_AchieveInfo.NormalCount;
	*(uint16 *)(strAchieve + 28) = pUser->m_AchieveInfo.QuestCount;
	*(uint16 *)(strAchieve + 30) = pUser->m_AchieveInfo.WarCount;
	*(uint16 *)(strAchieve + 32) = pUser->m_AchieveInfo.AdvantureCount;
	*(uint16 *)(strAchieve + 34) = pUser->m_AchieveInfo.ChallangeCount;

	foreach_stlmap(itr, pUser->m_AchieveMap)
	{
		if (itr->second == nullptr)
			continue;

		*(uint16 *)(strAchieve + index2) = itr->first;
		*(uint8 *)(strAchieve + index2 + 2) = itr->second->bStatus;
		*(uint32 *)(strAchieve + index2 + 3) = itr->second->iCount[0];
		*(uint32 *)(strAchieve + index2 + 7) = itr->second->iCount[1];
		index2 += 11;
	}

	// end of achieve system loading
	dbCommand->AddParameter(SQL_PARAM_INPUT, (char *)strAchieve, sizeof(strAchieve), SQL_BINARY);
	dbCommand->AddParameter(SQL_PARAM_INPUT, pUser->m_strUserID.c_str(), pUser->m_strUserID.length());

	if (!dbCommand->Execute(string_format(_T("UPDATE USER_ACHIEVE_DATA SET sCoverID=%d, sSkillID=%d, sAchieveCount=%d, strAchieve=? WHERE strUserID=?"),
		pUser->m_sCoverID, pUser->m_sSkillID, pUser->m_AchieveMap.GetSize())))
	{
		ReportSQLError(m_GameDB->GetError());
		return false;
	}

	return true;
}
#pragma endregion

#pragma region CDBAgent::LoadAchieveData(CUser *pUser)
bool CDBAgent::LoadAchieveData(CUser *pUser)
{
	if (pUser == nullptr)
		return false;

	if (pUser->m_AchieveMap.GetSize() > 0)
		return false;

	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	dbCommand->AddParameter(SQL_PARAM_INPUT, pUser->m_strUserID.c_str(), pUser->m_strUserID.length());
	if (!dbCommand->Execute(_T("SELECT sCoverID, sSkillID, sAchieveCount, strAchieve FROM USER_ACHIEVE_DATA WHERE strUserID = ?")))
	{
		ReportSQLError(m_GameDB->GetError());
		return false;
	}

	if (!dbCommand->hasData())
		return false;

	char strAchieve[ACHIEVE_ARRAY_SIZE];
	memset(strAchieve, 0, sizeof(strAchieve));
	uint16 sAchieveCount = 0;

	dbCommand->FetchUInt16(1, pUser->m_sCoverID);
	dbCommand->FetchUInt16(2, pUser->m_sSkillID);
	dbCommand->FetchUInt16(3, sAchieveCount);
	dbCommand->FetchBinary(4, strAchieve, sizeof(strAchieve));

	// Load User Achieve Data
	pUser->m_AchieveInfo.PlayTime = uint32(UNIXTIME);
	pUser->m_AchieveInfo.MonsterDefeatCount = *(uint32 *)(strAchieve + 4);
	pUser->m_AchieveInfo.UserDefeatCount = *(uint32 *)(strAchieve + 8);
	pUser->m_AchieveInfo.UserDeathCount = *(uint32 *)(strAchieve + 12);
	pUser->m_AchieveInfo.TotalMedal = *(uint32 *)(strAchieve + 16);
	pUser->m_AchieveInfo.RecentAchieve[0] = *(uint16 *)(strAchieve + 20);
	pUser->m_AchieveInfo.RecentAchieve[1] = *(uint16 *)(strAchieve + 22);
	pUser->m_AchieveInfo.RecentAchieve[2] = *(uint16 *)(strAchieve + 24);
	pUser->m_AchieveInfo.NormalCount = 0;
	pUser->m_AchieveInfo.QuestCount = 0;
	pUser->m_AchieveInfo.WarCount = 0;
	pUser->m_AchieveInfo.AdvantureCount = 0;
	pUser->m_AchieveInfo.ChallangeCount = 0;

	if (sAchieveCount == AchieveChalenngeInComplete)
	{
		foreach_stlmap(itr, g_pMain->m_AchieveMainArray)
		{
			_USER_ACHIEVE_INFO *pAchieve = new _USER_ACHIEVE_INFO;
			_ACHIEVE_MAIN *pAchieveMain = g_pMain->m_AchieveMainArray.GetData(itr->second->sIndex);

			if (pAchieveMain == nullptr)
				continue;

			if (pAchieveMain->byte2 == 41 
				|| pAchieveMain->byte2 == 42)
				pAchieve->bStatus = AchieveChalenngeInComplete;
			else
				pAchieve->bStatus = AchieveIncomplete;

			pAchieve->iCount[0] = AchieveChalenngeInComplete;
			pAchieve->iCount[1] = AchieveChalenngeInComplete;
			pUser->m_AchieveMap.PutData(itr->second->sIndex, pAchieve);
		}
	}
	else
	{
		for (int i = 0, index = 36; i < sAchieveCount; i++, index += 11)
		{
			uint16	sAchieveID = *(uint16*)(strAchieve + index);
			uint8	bStatus = *(uint8*)(strAchieve + index + 2);
			uint32	iCount1 = *(uint32*)(strAchieve + index + 3);
			uint32	iCount2 = *(uint32*)(strAchieve + index + 7);

			_USER_ACHIEVE_INFO *pAchieve = new _USER_ACHIEVE_INFO;
			_ACHIEVE_MAIN *pAchieveMain = g_pMain->m_AchieveMainArray.GetData(sAchieveID);
			if (bStatus == UserAchieveStatus::AchieveCompleted
				|| bStatus == UserAchieveStatus::AchieveFinished)
			{
				if (pAchieveMain)
				{
					switch (pAchieveMain->AchieveType)
					{
					case 0:
						pUser->m_AchieveInfo.NormalCount++;
						break;
					case 1:
						pUser->m_AchieveInfo.QuestCount++;
						break;
					case 2:
						pUser->m_AchieveInfo.WarCount++;
						break;
					case 3:
						pUser->m_AchieveInfo.AdvantureCount++;
						break;
					case 4:
						pUser->m_AchieveInfo.ChallangeCount++;
						break;
					default:
						break;
					}
				}
			}

			if (pAchieveMain->AchieveType == 4
				&& bStatus == UserAchieveStatus::AchieveIncomplete)
				pAchieve->bStatus = AchieveChalenngeInComplete;
			else
				pAchieve->bStatus = bStatus;

			pAchieve->iCount[0] = iCount1;
			pAchieve->iCount[1] = iCount2;
			pUser->m_AchieveMap.PutData(sAchieveID, pAchieve);
		}
	}

	_ACHIEVE_MAIN *pAchieveMain = g_pMain->m_AchieveMainArray.GetData(pUser->m_sCoverID);
	if (pAchieveMain)
		pUser->m_sCoverTitle = pAchieveMain->TitleID;

	pAchieveMain = g_pMain->m_AchieveMainArray.GetData(pUser->m_sSkillID);
	if (pAchieveMain)
	{
		pUser->m_sSkillTitle = pAchieveMain->TitleID;
		_ACHIEVE_TITLE *pAchieveTitle = g_pMain->m_AchieveTitleArray.GetData(pUser->m_sSkillTitle);
		if (pAchieveTitle != nullptr)
		{
			pUser->m_sStatAchieveBonuses[ACHIEVE_STAT_STR] = pAchieveTitle->Str;
			pUser->m_sStatAchieveBonuses[ACHIEVE_STAT_STA] = pAchieveTitle->Hp;
			pUser->m_sStatAchieveBonuses[ACHIEVE_STAT_DEX] = pAchieveTitle->Dex;
			pUser->m_sStatAchieveBonuses[ACHIEVE_STAT_INT] = pAchieveTitle->Int;
			pUser->m_sStatAchieveBonuses[ACHIEVE_STAT_CHA] = pAchieveTitle->Mp;
			pUser->m_sStatAchieveBonuses[ACHIEVE_STAT_ATTACK] = pAchieveTitle->Attack;
			pUser->m_sStatAchieveBonuses[ACHIEVE_STAT_DEFENCE] = pAchieveTitle->Defence;
		}
	}

	// end of achieve loading
	return true;
}
#pragma endregion

#pragma region int8 CDBAgent::CharacterSealProcess(std::string & strAcountID, std::string & strUserID, std::string & strPassword, uint64 nItemSerial)
/**
* @brief	Proceeds character seal process.
*
* @param	strAccountID	Character's Account name.
* @param	strUserID		the Character to be sealed within the Cypher Ring
* @param	strPassword		the Account Seal Password
*/
int8 CDBAgent::CharacterSealProcess(std::string& strAccountID, std::string& strUserID, std::string& strPassword, uint64 nItemSerial)
{
	unique_ptr<OdbcCommand> dbCommand(m_AccountDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return 3;

	uint8 bRet = 1;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, strPassword.c_str(), strPassword.length());

	if (!dbCommand->Execute(string_format(_T("{? = CALL USER_ITEM_SEAL_PASSWORD(?, ?)}"))))
		ReportSQLError(m_AccountDB->GetError());

	if (bRet > 1)
		return bRet;

	unique_ptr<OdbcCommand> dbCommand2(m_GameDB->CreateCommand());

	if (dbCommand2.get() == nullptr)
		return 3;

	dbCommand2->AddParameter(SQL_PARAM_OUTPUT, &bRet);
	dbCommand2->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());
	dbCommand2->AddParameter(SQL_PARAM_INPUT, strUserID.c_str(), strUserID.length());
	if (!dbCommand2->Execute(string_format(_T("{? = CALL CHARACTER_SEAL_PROCEED(?, ?, " I64FMTD ")}"), nItemSerial)))
		ReportSQLError(m_GameDB->GetError());

	return bRet;
}

#pragma endregion 

int8 CDBAgent::CharacterSealItemCheck(std::string& strUserID)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return 0;

	uint8 bRet = 1;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strUserID.c_str(), strUserID.length());
	if (!dbCommand->Execute(_T("{? = CALL CHARACTER_SEAL_ITEM_CHECK(?)}")))
		ReportSQLError(m_GameDB->GetError());

	return bRet;
}

#pragma region int8 CDBAgent::CharacterUnSealProcess(std::string & strAcountID, uint8 bSelectedIndex, uint64 nItemSerial)
/**
* @brief	Proceeds character seal process.
*
* @param	strAccountID	Character's Account name.
* @param	strUserID		the Character to be sealed within the Cypher Ring
* @param	strPassword		the Account Seal Password
*/
int8 CDBAgent::CharacterUnSealProcess(std::string& strAccountID, uint8 bSelectedIndex, uint64 nItemSerial)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());

	if (dbCommand.get() == nullptr)
		return 3;

	uint8 bRet = 1;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());
	if (!dbCommand->Execute(string_format(_T("{? = CALL CHARACTER_UNSEAL_PROCEED(?, %d,  " I64FMTD ")}"), bSelectedIndex, nItemSerial)))
		ReportSQLError(m_GameDB->GetError());

	return bRet;
}

#pragma endregion

#pragma region CDBAgent::LoadAllCharacterSealItems(CharacterSealItemArray & m_CharacterSealArray)
/**
* @brief Loads all Character Seal Items data on server start.
*
* @param m_CharacterSealArray	the Character Seal array used by the GameServerDLG.
*/
bool  CDBAgent::LoadAllCharacterSealItems(CharacterSealItemArray& m_CharacterSealArray)
{
	foreach_stlmap(itr, g_pMain->m_CharacterSealItemArray)
	{
		_CHARACTER_SEAL_ITEM* pData = itr->second;
		if (pData == nullptr)
			continue;

		if (!LoadCharacterSealUserData(pData->strAccountID, pData->m_strUserID, pData))
			return false;

		_CHARACTER_SEAL_ITEM_MAPPING* pDataMapping = new _CHARACTER_SEAL_ITEM_MAPPING;
		pDataMapping->nItemSerial = pData->nSerialNum;
		pDataMapping->nUniqueID = pData->nUniqueID;

		if (!g_pMain->m_CharacterSealItemMapping.PutData(pDataMapping->nUniqueID, pDataMapping))
			return false;
	}

	return true;
}
#pragma endregion

#pragma region CDBAgent::LoadCharacterSealUserData(std::string & strAccountID, std::string & strCharID, _CHARACTER_SEAL_ITEM *pUser)

bool CDBAgent::LoadCharacterSealUserData(std::string& strAccountID, std::string& strCharID, _CHARACTER_SEAL_ITEM* pUser)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());

	DateTime time;

	if (dbCommand.get() == nullptr)
		return false;

	int8 bRet = -2; // generic error

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());

	if (!dbCommand->Execute(_T("{? = CALL CHARACTER_SEAL_LOAD_USER_DATA(?, ?)}")))
	{
		ReportSQLError(m_GameDB->GetError());
		return false;
	}

	if (!dbCommand->hasData())
		return false;

	char strItem[INVENTORY_TOTAL * 8] = { 0 }, strSerial[INVENTORY_TOTAL * 8] = { 0 }, strItemExpiration[INVENTORY_TOTAL * 4] = { 0 };

	int field = 1;
	pUser->nUniqueID = g_pMain->GenerateItemUniqueID();
	dbCommand->FetchByte(field++, pUser->m_bRace);
	dbCommand->FetchUInt16(field++, pUser->m_sClass);
	dbCommand->FetchUInt32(field++, pUser->m_nHair);
	dbCommand->FetchByte(field++, pUser->m_bRank);
	dbCommand->FetchByte(field++, pUser->m_bTitle);
	dbCommand->FetchByte(field++, pUser->m_bLevel);
	dbCommand->FetchByte(field++, pUser->m_bRebirthLevel);
	dbCommand->FetchInt64(field++, pUser->m_iExp);
	dbCommand->FetchUInt32(field++, pUser->m_iLoyalty);
	dbCommand->FetchByte(field++, pUser->m_bFace);
	dbCommand->FetchByte(field++, pUser->m_bCity);
	dbCommand->FetchInt16(field++, pUser->m_bKnights);
	dbCommand->FetchByte(field++, pUser->m_bFame);
	dbCommand->FetchInt16(field++, pUser->m_sHp);
	dbCommand->FetchInt16(field++, pUser->m_sMp);
	dbCommand->FetchInt16(field++, pUser->m_sSp);
	dbCommand->FetchByte(field++, pUser->m_bStats[STAT_STR]);
	dbCommand->FetchByte(field++, pUser->m_bStats[STAT_STA]);
	dbCommand->FetchByte(field++, pUser->m_bStats[STAT_DEX]);
	dbCommand->FetchByte(field++, pUser->m_bStats[STAT_INT]);
	dbCommand->FetchByte(field++, pUser->m_bStats[STAT_CHA]);
	dbCommand->FetchByte(field++, pUser->m_bRebStats[STAT_STR]);
	dbCommand->FetchByte(field++, pUser->m_bRebStats[STAT_STA]);
	dbCommand->FetchByte(field++, pUser->m_bRebStats[STAT_DEX]);
	dbCommand->FetchByte(field++, pUser->m_bRebStats[STAT_INT]);
	dbCommand->FetchByte(field++, pUser->m_bRebStats[STAT_CHA]);
	dbCommand->FetchByte(field++, pUser->m_bAuthority);
	dbCommand->FetchInt16(field++, pUser->m_sPoints);
	dbCommand->FetchUInt32(field++, pUser->m_iGold);
	dbCommand->FetchByte(field++, pUser->m_bZone);
	dbCommand->FetchString(field++, (char*)pUser->m_bstrSkill, sizeof(pUser->m_bstrSkill));
	dbCommand->FetchBinary(field++, strItem, sizeof(strItem));
	dbCommand->FetchBinary(field++, strSerial, sizeof(strSerial));
	dbCommand->FetchBinary(field++, strItemExpiration, sizeof(strItemExpiration));
	dbCommand->FetchUInt32(field++, pUser->m_iMannerPoint);
	dbCommand->FetchUInt32(field++, pUser->m_iLoyaltyMonthly);

	ByteBuffer itemBuffer, serialBuffer, itemexpirationbuffer;
	itemBuffer.append(strItem, sizeof(strItem));
	serialBuffer.append(strSerial, sizeof(strSerial));
	itemexpirationbuffer.append(strItemExpiration, sizeof(strItemExpiration));


	memset(pUser->m_sItemArray, 0x00, sizeof(pUser->m_sItemArray));

	UserRentalMap rentalData;

	// Until this statement is cleaned up, 
	// no other statements can be processed.
	delete dbCommand.release();

	for (int i = 0; i < INVENTORY_TOTAL; i++)
	{
		uint64 nSerialNum;
		uint32 nItemID;
		uint32 nExpirationTime;
		int16 sDurability, sCount;
		itemBuffer >> nItemID >> sDurability >> sCount;
		serialBuffer >> nSerialNum;
		itemexpirationbuffer >> nExpirationTime;

		_ITEM_TABLE* pTable = g_pMain->GetItemPtr(nItemID);
		if (pTable == nullptr || sCount <= 0)
			continue;

		if (!pTable->m_bCountable && sCount > 1)
			sCount = 1;
		else if (sCount > ITEMCOUNT_MAX)
			sCount = ITEMCOUNT_MAX;

		if (nSerialNum == 0)
			nSerialNum = g_pMain->GenerateItemSerial();

		_ITEM_DATA* pItem = pUser->GetItem(i);

		if (pItem == nullptr)
			continue;

		pItem->nNum = nItemID;
		pItem->sDuration = pTable->isAccessory() ? pTable->m_sDuration : sDurability;
		pItem->sCount = sCount;
		pItem->nExpirationTime = nExpirationTime; // set this to 0 for now till the expiration system completed
		pItem->nSerialNum = nSerialNum;

		if (g_pMain->IsDuplicateItem(nItemID, nSerialNum))
			pItem->bFlag = ITEM_FLAG_DUPLICATE;
	}
	return true;
}
#pragma endregion

#pragma region CDBAgent::LoadUserSealExpData(CUser *pUser)
bool CDBAgent::LoadUserSealExpData(CUser *pUser)
{
	if (pUser == nullptr)
		return false;

	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());

	if (dbCommand.get() == nullptr)
		return false;

	dbCommand->AddParameter(SQL_PARAM_INPUT, pUser->m_strUserID.c_str(), pUser->m_strUserID.length());

	if (!dbCommand->Execute(_T("{CALL LOAD_USER_SEAL_EXP(?)}")))
		ReportSQLError(m_GameDB->GetError());

	if (!dbCommand->hasData())
		return false;

	dbCommand->FetchUInt32(1, pUser->m_iSealedExp);

	return true;
}
#pragma endregion

#pragma region CDBAgent::UpdateUserSealExpData(CUser *pUser)
bool CDBAgent::UpdateUserSealExpData(CUser *pUser)
{
	if (pUser == nullptr)
		return false;

	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());

	if (dbCommand.get() == nullptr)
		return false;

	dbCommand->AddParameter(SQL_PARAM_INPUT, pUser->m_strUserID.c_str(), pUser->m_strUserID.length());

	if (!dbCommand->Execute(string_format(_T("{CALL UPDATE_USER_SEAL_EXP(?, %d)}"), pUser->m_iSealedExp)))
	{
		ReportSQLError(m_GameDB->GetError());
		return false;
	}

	return true;
}
#pragma endregion

#pragma region CDBAgent::DrakiSignCreateDatas(std::string & strCharID, CUser *pUser)
int8 CDBAgent::DrakiSignCreateDatas(std::string & strCharID, CUser *pUser)
{
	int8 bRet = 1;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return 0;

	if (pUser == nullptr)
		return 0;

	uint8 Class = 0;
	if (pUser->isWarrior())
		Class = 1;
	else if (pUser->isRogue())
		Class = 2;
	else if (pUser->isMage())
		Class = 3;
	else if (pUser->isPriest())
		Class = 4;
	else if (pUser->isPortuKurian())
		Class = 13;

	dbCommand->AddParameter(SQL_PARAM_INPUT, pUser->m_strUserID.c_str(), pUser->m_strUserID.length());
	if (!dbCommand->Execute(_T("SELECT * FROM USER_DRAKI_TOWER_DATA WHERE strUserID = ?")))
	{
		ReportSQLError(m_GameDB->GetError());
		return 0;
	}

	if (!dbCommand->hasData())
	{
		unique_ptr<OdbcCommand> dbCommand2(m_GameDB->CreateCommand());
		if (dbCommand2.get() == nullptr)
			return 0;

		dbCommand2->AddParameter(SQL_PARAM_OUTPUT, &bRet);
		dbCommand2->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());

		if (!dbCommand2->Execute(string_format(_T("{? = CALL CREATE_NEW_DRAKI_DATA(?,%d)}"), Class)))
		{
			ReportSQLError(m_GameDB->GetError());
			return 0;
		}
	}

	return bRet;
}
#pragma endregion

#pragma region CDBAgent::LoadUserDrakiTowerData(CUser *pUser)
bool CDBAgent::LoadUserDrakiTowerData(CUser *pUser)
{
	if (pUser == nullptr)
		return false;

	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());

	if (dbCommand.get() == nullptr)
		return false;

	dbCommand->AddParameter(SQL_PARAM_INPUT, pUser->m_strUserID.c_str(), pUser->m_strUserID.length());

	if (!dbCommand->Execute(_T("{CALL LOAD_DRAKI_TOWER(?)}")))
		ReportSQLError(m_GameDB->GetError());

	if (!dbCommand->hasData())
	{
		pUser->m_iDrakiTime = 3600;
		pUser->m_bDrakiStage = 1;
		pUser->m_bDrakiEnteranceLimit = 3;
	}
	else
	{
		dbCommand->FetchInt32(1, pUser->m_iDrakiTime);
		dbCommand->FetchByte(2, pUser->m_bDrakiStage);
		dbCommand->FetchByte(3, pUser->m_bDrakiEnteranceLimit);
	}

	return true;
}
#pragma endregion

#pragma region CDBAgent::UpdateDaysDrakiTowerLimit()
bool CDBAgent::UpdateDaysDrakiTowerLimit()
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());

	if (dbCommand.get() == nullptr)
		return false;

	if (!dbCommand->Execute(_T("{CALL UPDATE_DRAKI_LIMIT_DAY}")))
	{
		ReportSQLError(m_GameDB->GetError());
		return false;
	}

	return true;
}
#pragma endregion

#pragma region CDBAgent::UpdateDrakiTowerLimitLastUpdate(std::string & strUserID, uint8 EnteranceLimit)
bool CDBAgent::UpdateDrakiTowerLimitLastUpdate(std::string & strUserID, uint8 EnteranceLimit)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());

	if (dbCommand.get() == nullptr)
		return false;

	dbCommand->AddParameter(SQL_PARAM_INPUT, strUserID.c_str(), strUserID.length());
	if (!dbCommand->Execute(string_format(_T("{CALL UPDATE_DRAKI_LIMIT(?, %d)}"), EnteranceLimit)))
	{
		ReportSQLError(m_GameDB->GetError());
		return false;
	}

	return true;
}
#pragma endregion

#pragma region CDBAgent::LoadUserHackToolTables()
bool CDBAgent::LoadUserHackToolTables()
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());

	if (dbCommand.get() == nullptr)
		return false;

	if (!dbCommand->Execute(_T("{CALL LOAD_HACKTOOL_LIST}")))
	{
		ReportSQLError(m_GameDB->GetError());
		return false;
	}

	if (!dbCommand->hasData())
		return false;

	do
	{
		USER_HACKTOOL_LIST *pData = new USER_HACKTOOL_LIST;

		dbCommand->FetchUInt32(1, pData->nIndex);
		dbCommand->FetchString(2, pData->nChecking);

		if (!g_pMain->m_HackToolListArray.PutData(pData->nIndex, pData))
			delete pData;

	} while (dbCommand->MoveNext());
	return true;
}
#pragma endregion

#pragma region CDBAgent::LoadDrakiTowerTables()
bool CDBAgent::LoadDrakiTowerTables()
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());

	if (dbCommand.get() == nullptr)
		return false;

	if (!dbCommand->Execute(_T("{CALL LOAD_DRAKI_MONSTER_LIST}")))
	{
		ReportSQLError(m_GameDB->GetError());
		return false;
	}

	if (!dbCommand->hasData())
		return false;

	do
	{
		DRAKI_MONSTER_LIST *pData = new DRAKI_MONSTER_LIST;

		dbCommand->FetchUInt32(1, pData->nIndex);
		dbCommand->FetchByte(2, pData->bDrakiStage);
		dbCommand->FetchUInt16(3, pData->sMonsterID);
		dbCommand->FetchUInt16(4, pData->sPosX);
		dbCommand->FetchUInt16(5, pData->sPosZ);
		dbCommand->FetchUInt16(6, pData->sDirection);
		dbCommand->FetchByte(7, pData->bMonster);


		if (!g_pMain->m_DrakiMonsterListArray.PutData(pData->nIndex, pData))
			delete pData;

	} while (dbCommand->MoveNext());

	if (!dbCommand->Execute(_T("{CALL LOAD_DRAKI_TOWER_STAGES}")))
	{
		ReportSQLError(m_GameDB->GetError());
		return false;
	}

	if (!dbCommand->hasData())
		return false;

	do
	{
		DRAKI_ROOM_LIST *pData = new DRAKI_ROOM_LIST;

		dbCommand->FetchByte(1, pData->bIndex);
		dbCommand->FetchByte(2, pData->bDrakiStage);
		dbCommand->FetchByte(3, pData->bDrakiSubStage);
		dbCommand->FetchByte(4, pData->bDrakiNpcStage);

		if (!g_pMain->m_DrakiRoomListArray.PutData(pData->bIndex, pData))
			delete pData;

	} while (dbCommand->MoveNext());

	return true;
}
#pragma endregion

#pragma region CDBAgent::UpdateDrakiTowerData(uint32 sTime, uint8 bStage, std::string & strUserID)
void CDBAgent::UpdateDrakiTowerData(uint32 sTime, uint8 bStage, std::string & strUserID)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());

	if (dbCommand.get() == nullptr)
		return;

	dbCommand->AddParameter(SQL_PARAM_INPUT, strUserID.c_str(), strUserID.length());

	if (!dbCommand->Execute(string_format(_T("{CALL UPDATE_DRAKI_TOWER_DATA(?, %d, %d)}"), sTime, bStage)))
		ReportSQLError(m_GameDB->GetError());

	if (!dbCommand->hasData())
		return;
}
#pragma endregion

#pragma region CDBAgent::ReqDrakiTowerList(Packet & pkt, CUser* pUser)
void CDBAgent::ReqDrakiTowerList(Packet & pkt, CUser* pUser)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr || pUser == nullptr)
		return;

	if (!dbCommand->Execute(_T("{CALL LOAD_DRAKI_RANK}")))
		ReportSQLError(m_GameDB->GetError());

	pkt.SByte();
	uint8 Counter = 0;
	if (!dbCommand->hasData())
	{
		for (int i = 0; i < 5; i++)
		{
			Counter++;
			pkt << uint8(++Counter)
				<< ""
				<< uint32(3600)
				<< uint32(1);
		}

		pkt << uint8(-1)			// Rank
			<< pUser->m_strUserID	// Name
			<< uint32(3600)			// Time
			<< uint32(1)			// Max Stage
			<< uint32(1)			// Enterance Area
			<< uint32(3);			// Enterance Limit
		return;
	}
	else
	{
		std::string strRankID;
		uint32 pFinishTime, iRank, sIndex, Class;
		uint8  bStage;
		uint16 Counter2 = 0;
		do {
			dbCommand->FetchUInt32(1, sIndex);
			dbCommand->FetchUInt32(2, Class);
			dbCommand->FetchUInt32(3, iRank);
			dbCommand->FetchString(4, strRankID);
			dbCommand->FetchByte(5, bStage);
			dbCommand->FetchUInt32(6, pFinishTime);

			if (pUser->isWarrior())
			{
				if (Class == 1)
				{
					Counter2++;
					pkt << uint8(iRank)
						<< strRankID
						<< pFinishTime
						<< uint32(bStage);
				}
			}
			else if (pUser->isRogue())
			{
				if (Class == 2)
				{
					Counter2++;
					pkt << uint8(iRank)
						<< strRankID
						<< pFinishTime
						<< uint32(bStage);
				}
			}
			else if (pUser->isMage())
			{
				if (Class == 3)
				{
					Counter2++;
					pkt << uint8(iRank)
						<< strRankID
						<< pFinishTime
						<< uint32(bStage);
				}
			}
			else if (pUser->isPriest())
			{
				if (Class == 4)
				{
					Counter2++;
					pkt << uint8(iRank)
						<< strRankID
						<< pFinishTime
						<< uint32(bStage);
				}
			}
			else if (pUser->isPortuKurian())
			{
				if (Class == 13)
				{
					Counter2++;
					pkt << uint8(iRank)
						<< strRankID
						<< pFinishTime
						<< uint32(bStage);
				}
			}
		} while (dbCommand->MoveNext());

		uint8 thyke = (5 - Counter2);
		if (thyke < 0)
			thyke = 0;

		if (thyke > 0)
		{
			for (int i = 0; i < thyke; i++)
			{
				pkt << uint8(++Counter2)
					<< ""
					<< uint32(3600)
					<< uint32(1);
			}
		}

		dbCommand->AddParameter(SQL_PARAM_INPUT, pUser->m_strUserID.c_str(), pUser->m_strUserID.length());

		if (!dbCommand->Execute(_T("SELECT * FROM DRAKI_TOWER_RIFT_RANK WHERE strUserID = ?")))
		{
			ReportSQLError(m_GameDB->GetError());
			return;
		}

		if (!dbCommand->hasData())
		{
			pkt << uint8(-1)			// Rank
				<< pUser->m_strUserID	// Name
				<< uint32(3600)			// Time
				<< uint32(1)			// Max Stage
				<< uint32(1)			// Enterance Area
				<< uint32(3);			// Enterance Limit
			return;
		}
		else
		{
			dbCommand->FetchUInt32(1, sIndex);
			dbCommand->FetchUInt32(2, Class);
			dbCommand->FetchUInt32(4, iRank);
			dbCommand->FetchString(5, strRankID);
			dbCommand->FetchByte(6, bStage);
			dbCommand->FetchUInt32(7, pFinishTime);

			if (iRank > 254)
				iRank = -1;

			uint8 kk = bStage % 6;
			uint32 MaxStages = (bStage - kk) / 6 + (kk > 0 ? 1 : 0);

			pkt << uint8(iRank)								// Rank
				<< pUser->m_strUserID						// Name
				<< pFinishTime								// Time
				<< uint32(bStage)							// Max Stage
				<< uint32(MaxStages == 0 ? 1 : MaxStages)	// Enterance Area
				<< uint32(pUser->m_bDrakiEnteranceLimit);	// Enterance Limit
			return;
		}
	}
	return;
}
#pragma endregion

#pragma region CDBAgent::CharacterSelectPrisonCheck(std::string & strCharID, uint8 & ZoneID)
bool CDBAgent::CharacterSelectPrisonCheck(std::string & strCharID, uint8 & ZoneID)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());

	if (dbCommand.get() == nullptr)
		return false;

	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());

	if (!dbCommand->Execute(_T("SELECT Zone FROM USERDATA WHERE strUserID = ?")))
		ReportSQLError(m_GameDB->GetError());

	if (!dbCommand->hasData())
		return false;

	dbCommand->FetchByte(1, ZoneID);
	return true;
}
#pragma endregion

#pragma region CDBAgent::SelectCharacterChecking(string & strAccountID, string & strCharID)
int8 CDBAgent::SelectCharacterChecking(string & strAccountID, string & strCharID)
{
	int8 bRet = 0;

	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());

	if (dbCommand.get() == nullptr)
		return bRet;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());

	if (!dbCommand->Execute(string_format(_T("{? = CALL SELECT_CHARACTER(?, ?)}"))))
		ReportSQLError(m_GameDB->GetError());

	return bRet;
}
#pragma endregion

#pragma region CDBAgent::UpdateSelectingCharacterName(std::string & strAccountID, std::string & strUserID, std::string & strNewUserID)
ServerUniteSelectingCharNameErrorOpcode CDBAgent::UpdateSelectingCharacterName(std::string & strAccountID, std::string & strUserID, std::string & strNewUserID)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());

	if (dbCommand.get() == nullptr)
		return CannotCharacterID;

	int16 bRet = 0;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, strUserID.c_str(), strUserID.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, strNewUserID.c_str(), strNewUserID.length());


	if (!dbCommand->Execute(string_format(_T("{? = CALL CHARACTER_SELECT_NAME_CHANGE(?, ?, ?)}"))))
		ReportSQLError(m_GameDB->GetError());

	return ServerUniteSelectingCharNameErrorOpcode(bRet);
}
#pragma endregion

#pragma region CDBAgent::GetLoadKnightCash(CUser* pUser)
bool CDBAgent::GetLoadKnightCash(CUser* pUser)
{
	if (pUser == nullptr)
		return false;

	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	dbCommand->AddParameter(SQL_PARAM_INPUT, pUser->GetAccountName().c_str(), pUser->GetAccountName().length());
	if (!dbCommand->Execute(_T("SELECT CashPoint, BonusCashPoint FROM TB_USER WHERE strAccountID = ?")))
	{
		ReportSQLError(m_GameDB->GetError());
		return false;
	}

	if (!dbCommand->hasData())
		return false;

	dbCommand->FetchUInt32(1, pUser->m_nKnightCash);dbCommand->FetchUInt32(2, pUser->m_nKnightCashBonus);

	return true;
}
#pragma endregion

#pragma region CDBAgent::UpdateKnightCash(CUser* pUser)
bool CDBAgent::UpdateKnightCash(CUser* pUser)
{
	if (pUser == nullptr)
		return false;

	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	uint32 cash = pUser->m_nKnightCash;
	uint32 bonusCash = pUser->m_nKnightCashBonus;

	dbCommand->AddParameter(SQL_PARAM_INPUT, pUser->GetAccountName().c_str(), pUser->GetAccountName().length());
	if (!dbCommand->Execute(string_format(_T("UPDATE TB_USER SET CashPoint = %d, BonusCashPoint = %d WHERE strAccountID = ?"), cash, bonusCash)))
		ReportSQLError(m_AccountDB->GetError());

	return true;
}
#pragma endregion

#pragma region CDBAgent::GetLifeSkills(CUser* pUser)
bool CDBAgent::GetLifeSkills(CUser* pUser)
{
	if (pUser == nullptr)
		return false;

	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	dbCommand->AddParameter(SQL_PARAM_INPUT, pUser->GetAccountName().c_str(), pUser->GetAccountName().length());
	if (!dbCommand->Execute(_T("SELECT LifeSkillWarExp, LifeSkillHuntingExp, LifeSkillSmitheryExp, LifeSkillKarmaExp FROM ACCOUNT_CHAR WHERE strAccountID = ?")))
	{
		ReportSQLError(m_GameDB->GetError());
		return false;
	}

	if (!dbCommand->hasData())
		return false;

	dbCommand->FetchUInt64(1, pUser->m_sLifeSkills.WarExp);	dbCommand->FetchUInt64(2, pUser->m_sLifeSkills.HuntingExp);	dbCommand->FetchUInt64(3, pUser->m_sLifeSkills.SmitheryExp); dbCommand->FetchUInt64(4, pUser->m_sLifeSkills.KarmaExp);

	return true;
}
#pragma endregion

#pragma region CDBAgent::SaveLifeSkills(CUser* pUser)
bool CDBAgent::SaveLifeSkills(CUser* pUser)
{
	if (pUser == nullptr)
		return false;

	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	dbCommand->AddParameter(SQL_PARAM_INPUT, pUser->GetAccountName().c_str(), pUser->GetAccountName().length());
	if (!dbCommand->Execute(string_format(_T("UPDATE ACCOUNT_CHAR set LifeSkillWarExp = %d, LifeSkillHuntingExp = %d, LifeSkillSmitheryExp = %d, LifeSkillKarmaExp = %d WHERE strAccountID = ?"), pUser->m_sLifeSkills.WarExp, pUser->m_sLifeSkills.HuntingExp, pUser->m_sLifeSkills.SmitheryExp, pUser->m_sLifeSkills.KarmaExp)))
		ReportSQLError(m_AccountDB->GetError());

	return true;
}
#pragma endregion

#pragma region CDBAgent::LoadGameMasterSetting(string & strAccountID, string & strCharID, uint16 sServerNo, CUser *pUser)
bool CDBAgent::LoadGameMasterSetting(string & strAccountID, string & strCharID, uint16 sServerNo, CUser *pUser)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());
	if (!dbCommand->Execute(string_format(_T("{CALL LOAD_GAMEMASTER_SETTINGS(?, ?, %d)}"), sServerNo)))
		ReportSQLError(m_GameDB->GetError());

	if (!dbCommand->hasData())
		return false;

	int field = 1;
	dbCommand->FetchByte(field++, pUser->m_GameMastersMute);
	dbCommand->FetchByte(field++, pUser->m_GameMastersUnMute);
	dbCommand->FetchByte(field++, pUser->m_GameMastersUnBan);
	dbCommand->FetchByte(field++, pUser->m_GameMastersBanPermit);
	dbCommand->FetchByte(field++, pUser->m_GameMastersAllowAttack);
	dbCommand->FetchByte(field++, pUser->m_GameMastersDisabledAttack);
	dbCommand->FetchByte(field++, pUser->m_GameMastersNpAdd);
	dbCommand->FetchByte(field++, pUser->m_GameMastersExpAdd);
	dbCommand->FetchByte(field++, pUser->m_GameMastersMoneyAdd);
	dbCommand->FetchByte(field++, pUser->m_GameMastersLoyaltyChange);
	dbCommand->FetchByte(field++, pUser->m_GameMastersExpChange);
	dbCommand->FetchByte(field++, pUser->m_GameMastersMoneyChange);
	dbCommand->FetchByte(field++, pUser->m_GameMastersGiveItem);
	dbCommand->FetchByte(field++, pUser->m_GameMastersGiveItemSelf);
	dbCommand->FetchByte(field++, pUser->m_GameMastersSummonUser);
	dbCommand->FetchByte(field++, pUser->m_GameMastersTpOnUser);
	dbCommand->FetchByte(field++, pUser->m_GameMastersZoneChange);
	dbCommand->FetchByte(field++, pUser->m_GameMastersLocationChange);
	dbCommand->FetchByte(field++, pUser->m_GameMastersMonsterSummon);
	dbCommand->FetchByte(field++, pUser->m_GameMastersNpcSummon);
	dbCommand->FetchByte(field++, pUser->m_GameMastersMonKilled);
	dbCommand->FetchByte(field++, pUser->m_GameMastersTeleportAllUser);
	dbCommand->FetchByte(field++, pUser->m_GameMastersClanSummon);
	dbCommand->FetchByte(field++, pUser->m_GameMastersResetPlayerRanking);
	dbCommand->FetchByte(field++, pUser->m_GameMastersChangeEventRoom);
	dbCommand->FetchByte(field++, pUser->m_GameMastersWarOpen1);
	dbCommand->FetchByte(field++, pUser->m_GameMastersWarOpen2);
	dbCommand->FetchByte(field++, pUser->m_GameMastersWarOpen3);
	dbCommand->FetchByte(field++, pUser->m_GameMastersWarOpen4);
	dbCommand->FetchByte(field++, pUser->m_GameMastersWarOpen5);
	dbCommand->FetchByte(field++, pUser->m_GameMastersWarOpen6);
	dbCommand->FetchByte(field++, pUser->m_GameMastersWarOpen9);
	dbCommand->FetchByte(field++, pUser->m_GameMastersWarSiegeOpen);
	dbCommand->FetchByte(field++, pUser->m_GameMastersWarClose);
	dbCommand->FetchByte(field++, pUser->m_GameMastersCaptainElection);
	dbCommand->FetchByte(field++, pUser->m_GameMastersCastleSiegeWarClose);
	dbCommand->FetchByte(field++, pUser->m_GameMastersSendPrsion);

	return true;
}
#pragma endregion

#pragma region CDBAgent::LoadServerSettings()
bool CDBAgent::LoadServerSettings()
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
	{
		printf("Server Settings Load Error: 1\n");
		return false;
	}

	if (!dbCommand->Execute(_T("{CALL LOAD_SERVER_SETTINGS}")))
	{
		ReportSQLError(m_GameDB->GetError());
		printf("Server Settings Load Error: 2\n");
		return false;
	}

	if (!dbCommand->hasData())
	{
		printf("Server Settings Load Error: 3\n");
		return false;
	}

	do
	{
		_SERVER_SETTINGS *pData = new _SERVER_SETTINGS;
		dbCommand->FetchUInt32(1, pData->nServerNo);
		dbCommand->FetchByte(2, pData->MaximumLevelChange);
		dbCommand->FetchByte(3, pData->DropNotice);
		dbCommand->FetchByte(4, pData->UpgradeNotice);
		dbCommand->FetchByte(5, pData->UserMaxUpgradeCount);
		dbCommand->FetchByte(6, pData->OpenQuestSkill);
		if (!g_pMain->m_ServerSettingsArray.PutData(pData->nServerNo, pData))
			delete pData;

	} while (dbCommand->MoveNext());

	return true;
}

#pragma region CDBAgent::LoadQuestSkillSetUp()
bool CDBAgent::LoadQuestSkillSetUp()
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
	{
		printf("Load Open Quest Error: 1\n");
		return false;
	}

	if (!dbCommand->Execute(_T("{CALL LOAD_QUEST_OPEN_SKILL}")))
	{
		ReportSQLError(m_GameDB->GetError());
		printf("Load Open Quest Error: 2\n");
		return false;
	}

	if (!dbCommand->hasData())
	{
		printf("Load Open Quest Error: 3\n");
		return false;
	}

	do
	{
		_LOAD_QUEST_SKILL* pData = new _LOAD_QUEST_SKILL;
		dbCommand->FetchUInt32(1, pData->nIndex);
		dbCommand->FetchUInt16(2, pData->nEventDataIndex);
		if (!g_pMain->m_LoadOpenQuestArray.PutData(pData->nIndex, pData))
			delete pData;

	} while (dbCommand->MoveNext());

	return true;
}
#pragma endregion

bool CDBAgent::GameStartClearRemainUser()
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
	{
		printf("Clear Remain Users Error\n");
		return false;
	}

	if (!dbCommand->Execute(_T("{CALL GAME_START_CLEAR_REMAIN_USERS}")))
	{
		ReportSQLError(m_GameDB->GetError());
		printf("Clear Remain Users Error\n");
		return false;
	}
	return true;
}
#pragma endregion


void CDBAgent::LoadPetData(uint64 nSerial)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	if (!dbCommand->Execute(string_format(_T("{CALL LOAD_PET_DATA(" I64FMTD ")}"), nSerial)))
	{
		ReportSQLError(m_GameDB->GetError());
		return;
	}

	if (!dbCommand->hasData())
		return;

	char strItem[81];
	memset(strItem, 0x00, 81);

	PET_DATA* p = new PET_DATA();
	dbCommand->FetchString(1, p->strPetName);
	dbCommand->FetchByte(2, p->bLevel);
	dbCommand->FetchUInt16(3, p->sHP);
	dbCommand->FetchUInt16(4, p->sMP);
	dbCommand->FetchUInt32(5, p->nIndex);
	dbCommand->FetchInt16(6, p->sSatisfaction);
	dbCommand->FetchUInt32(7, p->nExp);
	dbCommand->FetchUInt16(8, p->sPid);
	dbCommand->FetchUInt16(9, p->sSize);
	dbCommand->FetchBinary(10, strItem, sizeof(strItem));

	PET_INFO* pInfo = g_pMain->m_PetInfoSystemArray.GetData(p->bLevel);
	if (pInfo == nullptr)
		pInfo = g_pMain->m_PetInfoSystemArray.GetData(PET_START_LEVEL);

	p->info = pInfo;
	int index = 0;
	for (int i = 0; i < 3; i++)
	{
		_ITEM_DATA* pItem = &p->sItem[i];
		pItem->nNum = *(uint32*)(strItem + index); index += 4;
		pItem->sDuration = *(uint16*)(strItem + index); index += 2;
		pItem->sCount = *(uint16*)(strItem + index); index += 2;
		pItem->bFlag = *(uint8*)(strItem + index); index++;
		pItem->sRemainingRentalTime = *(uint16*)(strItem + index); index += 2;
		pItem->nExpirationTime = *(uint32*)(strItem + index); index += 4;
		pItem->nSerialNum = *(uint64*)(strItem + index); index += 8;
	}

	Guard Lock(g_pMain->m_PetSystemlock);
	g_pMain->m_PetDataSystemArray.insert(std::make_pair(nSerial, p));
}

uint32 CDBAgent::CreateNewPet(uint64 nSerialID, uint8 bLevel, std::string& strPetName)
{
	uint32 bRet = 3;

	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return bRet;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strPetName.c_str(), strPetName.length());
	if (!dbCommand->Execute(string_format(_T("{? = CALL CREATE_NEW_PET(" I64FMTD ",%d, ?)}"), nSerialID, bLevel)))
		ReportSQLError(m_GameDB->GetError());

	return bRet;
}

int8 CDBAgent::CreateNewPetCheck(uint64 nSerialID, std::string& strPetName)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return 0;

	uint32 bRet = 0;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strPetName.c_str(), strPetName.length());
	if (!dbCommand->Execute(string_format(_T("{? = CALL CREATE_NEW_PET(" I64FMTD ", ?)}"), nSerialID)))
		ReportSQLError(m_GameDB->GetError());

	return bRet;
}

void CDBAgent::UpdatingUserPet(uint64 nSerial)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	Guard Lock(g_pMain->m_PetSystemlock);
	PetDataSystemArray::iterator itr = g_pMain->m_PetDataSystemArray.find(nSerial);
	if (itr == g_pMain->m_PetDataSystemArray.end())
		return;

	PET_DATA* pPet = itr->second;
	if (pPet == nullptr)
		return;

	char strItems[81];
	memset(strItems, 0, sizeof(strItems));
	int index = 0;
	for (int i = 0; i < 3; i++)
	{
		_ITEM_DATA* pItem = &pPet->sItem[i];

		*(uint32*)(strItems + index) = pItem->nNum;
		index += 4;
		*(uint16*)(strItems + index) = pItem->sDuration;
		index += 2;
		*(uint16*)(strItems + index) = pItem->sCount;
		index += 2;
		*(uint8*)(strItems + index) = pItem->bFlag;
		index++;
		*(uint16*)(strItems + index) = pItem->sRemainingRentalTime;
		index += 2;
		*(uint32*)(strItems + index) = pItem->nExpirationTime;
		index += 4;
		*(uint64*)(strItems + index) = pItem->nSerialNum;
		index += 8;
	}

	dbCommand->AddParameter(SQL_PARAM_INPUT, pPet->strPetName.c_str(), pPet->strPetName.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, (char*)strItems, sizeof(strItems), SQL_BINARY);
	if (!dbCommand->Execute(string_format(_T("{CALL UPDATE_USER_PET(" I64FMTD ", ?, %d, %d, %d, %d, %d, %d, %d, ?)}"), nSerial, pPet->bLevel, pPet->sHP, pPet->sMP, pPet->sSatisfaction, pPet->nExp, pPet->sPid, pPet->sSize)))
		ReportSQLError(m_GameDB->GetError());
}

#pragma region CDBAgent::LoadMonsterDropMainTable()
bool CDBAgent::LoadMonsterDropMainTable()
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
	{
		printf("Load Open LOAD_MONSTER_DROP_MAIN Error: 1\n");
		return false;
	}

	if (!dbCommand->Execute(_T("{CALL LOAD_MONSTER_DROP_MAIN}")))
	{
		ReportSQLError(m_GameDB->GetError());
		printf("Load Open LOAD_MONSTER_DROP_MAIN Error: 2\n");
		return false;
	}

	if (!dbCommand->hasData())
	{
		printf("Load Open LOAD_MONSTER_DROP_MAIN Error: 3\n");
		return false;
	}

	do
	{
		_MONSTER_DROP_MAIN* pData = new _MONSTER_DROP_MAIN;
		dbCommand->FetchUInt16(1, pData->MonsterID);
		dbCommand->FetchByte(2, pData->MonsterOpenDrop);
		dbCommand->FetchUInt32(3, pData->MonsterMoney);

		for (int i = 0, a = 4; i < LOOT_GROUP_ITEMS; i++)
		{
			dbCommand->FetchUInt32(a++, pData->ItemListID[i]);
			dbCommand->FetchUInt32(a++, pData->ItemListPersent[i]);
		}

		for (int i = 0, b = 18; i < LOOT_GROUP_ITEMS; i++)
		{
			dbCommand->FetchUInt32(b++, pData->ItemSingleID[i]);
			dbCommand->FetchUInt32(b++, pData->ItemSinglePersent[i]);
		}

		if (!g_pMain->m_MonsterDropMainArray.PutData(pData->MonsterID, pData))
			delete pData;

	} while (dbCommand->MoveNext());

	return true;
}
#pragma endregion

#pragma region CDBAgent::LoadMonsterItemGroupItemTable()
bool CDBAgent::LoadMonsterItemGroupItemTable()
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
	{
		printf("Load Open LOAD_MONSTER_ITEM_GROUP_ITEM_LIST Error: 1\n");
		return false;
	}

	if (!dbCommand->Execute(_T("{CALL LOAD_MONSTER_ITEM_GROUP_ITEM_LIST}")))
	{
		ReportSQLError(m_GameDB->GetError());
		printf("Load Open LOAD_MONSTER_ITEM_GROUP_ITEM_LIST Error: 2\n");
		return false;
	}

	if (!dbCommand->hasData())
	{
		printf("Load Open LOAD_MONSTER_ITEM_GROUP_ITEM_LIST Error: 3\n");
		return false;
	}

	do
	{
		_MONSTER_GROUP_ITEM_LIST* pData = new _MONSTER_GROUP_ITEM_LIST;
		dbCommand->FetchUInt32(1, pData->GroupNumber);
		for (int i = 1; i <= 70; i++)
		{
			uint32 iItem;
			dbCommand->FetchUInt32(i + 1, iItem);

			if (iItem == 0)
				continue;

			// Insert regardless of whether it's set or not. This is official behaviour.
			pData->ItemID.push_back(iItem);
		}
		if (!g_pMain->m_MonsterItemGroupItemListArray.PutData(pData->GroupNumber, pData))
			delete pData;

	} while (dbCommand->MoveNext());

	return true;
}
#pragma endregion

#pragma region uint8 CDBAgent::UserHWIDCheck(std::string& strAccountID, string& HWIDKey, uint32 MacKey)
uint8 CDBAgent::UserHWIDCheck(std::string& strAccountID, string& HWIDKey, uint32 MacKey)
{
	/* Return values:
	9 IsValidCharacter Error
	3 Banned HardwareID or MacID
	2 Login Successfully
	4 Unknow Error Type
	6 Is Exist Banned Computer.
	*/
	uint8 bRet = -1;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return 8;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, HWIDKey.c_str(), HWIDKey.length());

	if (!dbCommand->Execute(string_format(_T("{? = CALL INSERT_USER_HDD_BANS(?,?, %d)}"), HWIDKey, MacKey)))
		ReportSQLError(m_GameDB->GetError());

	return bRet;
}
#pragma endregion


#pragma region CDBAgent::LoadSettings()
bool CDBAgent::LoadSettings()
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());

	if (dbCommand.get() == nullptr)
		return false;

	if (!dbCommand->Execute(_T("{CALL LOAD_HOOK_SETTINGS}")))
	{
		printf(" Error LOAD_HOOK_SETTINGS. \n");
		ReportSQLError(m_GameDB->GetError());
		return false;
	}

	if (!dbCommand->hasData())
	{
		printf(" Error LOAD_HOOK_SETTINGS. \n");
		return false;
	}

	do
	{
		uint32 id = 0;
		int i = 1;
		_HOOK_SETTINGS *pData = new _HOOK_SETTINGS;

		dbCommand->FetchByte(i++, pData->ACS_Validation);
		dbCommand->FetchByte(i++, pData->Button_Facebook);
		dbCommand->FetchByte(i++, pData->Button_Discord);
		dbCommand->FetchByte(i++, pData->Button_Live);
		dbCommand->FetchByte(i++, pData->Button_Support);
		dbCommand->FetchString(i++, pData->URL_Facebook);
		dbCommand->FetchString(i++, pData->URL_Discord);
		dbCommand->FetchString(i++, pData->URL_Live);
		dbCommand->FetchString(i++, pData->URL_KCBayi);
		dbCommand->FetchUInt32(i++, pData->KCMerchant_MinPrice);
		dbCommand->FetchUInt32(i++, pData->KCMerchant_MaxPrice);
		dbCommand->FetchByte(i++, pData->State_TempItemList);
		dbCommand->FetchByte(i++, pData->State_StatReset);
		dbCommand->FetchByte(i++, pData->State_SkillReset);
		dbCommand->FetchByte(i++, pData->State_PUS);

		if (!g_pMain->SettingsArray.PutData(id, pData))
			delete pData;

	} while (dbCommand->MoveNext());

	return true;
}
#pragma endregion

#pragma region CDBAgent::LoadPusLoad()
bool CDBAgent::LoadPusLoad()
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());

	if (dbCommand.get() == nullptr)
		return false;

	if (!dbCommand->Execute(_T("{CALL LOAD_HOOK_PUS}")))
	{
		printf(" Error LOAD_HOOK_PUS. \n");
		ReportSQLError(m_GameDB->GetError());
		return false;
	}

	if (!dbCommand->hasData())
	{
		printf(" Error LOAD_HOOK_PUS. \n");
		return false;
	}

	do
	{
		int i = 1;
		_HOOK_PUS_ITEM* pData = new _HOOK_PUS_ITEM;

		dbCommand->FetchUInt32(i++, pData->sItemID);
		dbCommand->FetchUInt32(i++, pData->sBuyCount);
		dbCommand->FetchUInt32(i++, pData->sPrice);
		dbCommand->FetchByte(i++, pData->sType);

		if (!g_pMain->PusItemArray.PutData(pData->sItemID, pData))
			delete pData;

	} while (dbCommand->MoveNext());

	return true;
}
#pragma endregion

#pragma region CDBAgent::LoadCollectionRaceSettings()
bool CDBAgent::LoadCollectionRaceSettings()
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());

	if (dbCommand.get() == nullptr)
		return false;

	if (!dbCommand->Execute(_T("{CALL LOAD_HOOK_COLLECTIONRACE_SETTINGS}")))
	{
		printf(" Error LOAD_HOOK_COLLECTIONRACE_SETTINGS. \n");
		ReportSQLError(m_GameDB->GetError());
		return false;
	}

	if (!dbCommand->hasData())
	{
		printf(" Error LOAD_HOOK_COLLECTIONRACE_SETTINGS. \n");
		return false;
	}

	do
	{
		COLLECTIONRACE_SETTINGS* pData = new COLLECTIONRACE_SETTINGS;

		int i = 1;

		dbCommand->FetchUInt32(i++, pData->ID);

		dbCommand->FetchUInt16(i++, pData->CREventMaxUser);
		dbCommand->FetchByte(i++, pData->CREventMaxLevel);
		dbCommand->FetchByte(i++, pData->CREventMinLevel);

		for (int j = 0; j < 2; j++)
			dbCommand->FetchUInt32(i++, pData->CREventItemID[j]);

		for (int j = 0; j < 2; j++)
			dbCommand->FetchString(i++, pData->CREventstrName[j]);

		for (int j = 0; j < 2; j++)
			dbCommand->FetchUInt32(i++, pData->CREventItemCount[j]);

		for (int j = 0; j < 2; j++)
			dbCommand->FetchUInt32(i++, pData->CREventItemTimed[j]);

		dbCommand->FetchUInt32(i++, pData->CREventWinCoin);
		dbCommand->FetchUInt32(i++, pData->CREventWinEXP);
		dbCommand->FetchUInt32(i++, pData->CREventWinKC);
		dbCommand->FetchUInt32(i++, pData->CREventWinNP);

		if (!g_pMain->CollectionRaceSettingsArray.PutData(pData->ID, pData))
			delete pData;

	} while (dbCommand->MoveNext());

	return true;
}
#pragma endregion

#pragma region CDBAgent::LoadCollectionRaceHuntList()

bool CDBAgent::LoadCollectionRaceHuntList()
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());

	if (dbCommand.get() == nullptr)
		return false;

	if (!dbCommand->Execute(_T("{CALL LOAD_HOOK_COLLECTIONRACE_HUNT_LIST}")))
	{
		printf(" Error LOAD_HOOK_COLLECTIONRACE_HUNT_LIST. \n");
		ReportSQLError(m_GameDB->GetError());
		return false;
	}

	if (!dbCommand->hasData())
	{
		printf(" Error LOAD_HOOK_COLLECTIONRACE_HUNT_LIST. \n");
		return false;
	}

	do
	{
		COLLECTIONRACE_HUNT_LIST* pData = new COLLECTIONRACE_HUNT_LIST;

		int i = 1;

		dbCommand->FetchByte(i++, pData->QuestID);

		dbCommand->FetchUInt16(i++, pData->MonsterNum1);
		dbCommand->FetchString(i++, pData->MonsterName1);
		dbCommand->FetchUInt16(i++, pData->MonsterKillCount1);

		dbCommand->FetchUInt16(i++, pData->MonsterNum2);
		dbCommand->FetchString(i++, pData->MonsterName2);
		dbCommand->FetchUInt16(i++, pData->MonsterKillCount2);

		dbCommand->FetchUInt16(i++, pData->MonsterNum3);
		dbCommand->FetchString(i++, pData->MonsterName3);
		dbCommand->FetchUInt16(i++, pData->MonsterKillCount3);

		dbCommand->FetchUInt16(i++, pData->MonsterNum4);
		dbCommand->FetchString(i++, pData->MonsterName4);
		dbCommand->FetchUInt16(i++, pData->MonsterKillCount4);

		dbCommand->FetchByte(i++, pData->QuestZone);

		if (!g_pMain->CollectionRaceHuntListArray.PutData(pData->QuestID, pData))
			delete pData;

	} 
	while (dbCommand->MoveNext());
	return true;
}

#pragma endregion

void CDBAgent::UpdatePPCard(uint32 FirstBox, uint8 bType, std::string & strAccountID)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());
	if (!dbCommand->Execute(string_format(_T("UPDATE PPCARD SET Used = %d, UseTime = getdate(), strAccountID = ? WHERE FirstBox = %d"), bType, FirstBox)))
		ReportSQLError(m_GameDB->GetError());
}
