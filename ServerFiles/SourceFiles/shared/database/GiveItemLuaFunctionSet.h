#pragma once

class CLuaGiveItemExchangeSet : public OdbcRecordset
{
public:
	CLuaGiveItemExchangeSet(OdbcConnection * dbConnection, LuaGiveItemExchangeArray *pMap)
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("ITEM_GIVE_EXCHANGE"); }
	virtual tstring GetColumns()
	{
		return _T("nExchangeIndex, "
			"nRobItemID1, nRobItemID2, "
			"nRobItemID3, nRobItemID4, "
			"nRobItemID5, nRobItemID6, "
			"nRobItemID7, nRobItemID8, "
			"nRobItemID9, nRobItemID10, "
			"nRobItemCount1, nRobItemCount2, "
			"nRobItemCount3, nRobItemCount4, "
			"nRobItemCount5, nRobItemCount6, "
			"nRobItemCount7, nRobItemCount8, "
			"nRobItemCount9, nRobItemCount10, "
			"nGiveItemID1, nGiveItemID2, "
			"nGiveItemID3, nGiveItemID4, "
			"nGiveItemID5, nGiveItemID6, "
			"nGiveItemID7, nGiveItemID8, "
			"nGiveItemID9, nGiveItemID10, "
			"nGiveItemCount1, nGiveItemCount2, "
			"nGiveItemCount3, nGiveItemCount4, "
			"nGiveItemCount5, nGiveItemCount6, "
			"nGiveItemCount7, nGiveItemCount8, "
			"nGiveItemCount9, nGiveItemCount10, "
			"nGiveItemTime1, nGiveItemTime2, "
			"nGiveItemTime3, nGiveItemTime4, "
			"nGiveItemTime5, nGiveItemTime6, "
			"nGiveItemTime7, nGiveItemTime8, "
			"nGiveItemTime9, nGiveItemTime10");
	}

	virtual bool Fetch()
	{
		_GIVE_LUA_ITEM_EXCHANGE *pData = new _GIVE_LUA_ITEM_EXCHANGE;

		int i = 1;
		_dbCommand->FetchUInt32(i++, pData->nExchangeID);

		for (int x = 0; x < ITEMS_IN_ROB_ITEM_GROUP_LUA; x++)
			_dbCommand->FetchUInt32(i++, pData->nRobItemID[x]);

		for (int x = 0; x < ITEMS_IN_ROB_ITEM_GROUP_LUA; x++)
			_dbCommand->FetchUInt32(i++, pData->nRobItemCount[x]);

		for (int j = 0; j < ITEMS_IN_GIVE_ITEM_GROUP_LUA; j++)
			_dbCommand->FetchUInt32(i++, pData->nGiveItemID[j]);

		for (int j = 0; j < ITEMS_IN_GIVE_ITEM_GROUP_LUA; j++)
			_dbCommand->FetchUInt32(i++, pData->nGiveItemCount[j]);

		for (int j = 0; j < ITEMS_IN_GIVE_ITEM_GROUP_LUA; j++)
			_dbCommand->FetchUInt32(i++, pData->nGiveItemTime[j]);

		if (!m_pMap->PutData(pData->nExchangeID, pData))
			delete pData;

		return true;
	}

	LuaGiveItemExchangeArray * m_pMap;
};