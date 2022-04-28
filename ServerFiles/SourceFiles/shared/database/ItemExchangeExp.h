#pragma once

class CItemExchangeExpSet : public OdbcRecordset
{
public:
	CItemExchangeExpSet(OdbcConnection * dbConnection, ItemExchangeExpArray *pMap) 
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("ITEM_EXCHANGE_EXP"); }
	virtual tstring GetColumns() 
	{
		return _T("nIndex, bRandomFlag,"
			"nExchangeItemNum1, nExchangeItemCount1," 
			"nExchangeItemNum2, nExchangeItemCount2," 
			"nExchangeItemNum3, nExchangeItemCount3, "
			"nExchangeItemNum4, nExchangeItemCount4, "
			"nExchangeItemNum5, nExchangeItemCount5");
	}

	virtual bool Fetch()
	{
		_ITEM_EXCHANGE_EXP *pData = new _ITEM_EXCHANGE_EXP;

		int i = 1;
		_dbCommand->FetchUInt32(i++, pData->nIndex);
		_dbCommand->FetchByte(i++, pData->bRandomFlag);


		for (int j = 0; j < ITEMS_IN_EXCHANGE_GROUP; j++)
		{
			_dbCommand->FetchUInt32(i++, pData->nExchangeItemNum[j]);
			_dbCommand->FetchUInt32(i++, pData->sExchangeItemCount[j]);
		}

		if (!m_pMap->PutData(pData->nIndex, pData))
			delete pData;

		return true;
	}

	ItemExchangeExpArray * m_pMap;
};