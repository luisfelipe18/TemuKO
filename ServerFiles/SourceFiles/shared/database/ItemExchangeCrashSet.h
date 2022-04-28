#pragma once

class CItemExchangeCreashSet : public OdbcRecordset
{
public:
	CItemExchangeCreashSet(OdbcConnection * dbConnection, ItemExchangeCrashArray *pMap)
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("ITEM_SMASH"); }
	virtual tstring GetColumns() { return _T("nIndex, nItemID, nCount, nRate"); }

	virtual bool Fetch()
	{
		_ITEM_EXCHANGE_CRASH * pData = new _ITEM_EXCHANGE_CRASH;

		int i = 1;

		_dbCommand->FetchUInt32(i++, pData->nIndex);
		_dbCommand->FetchUInt32(i++, pData->nItemID);
		_dbCommand->FetchByte(i++, pData->nCount);
		_dbCommand->FetchUInt16(i++, pData->sRate);

		if (!m_pMap->PutData(pData->nIndex, pData))
		{
			printf("Item Crash Put Data Fail %d \r\n", pData->nIndex);
			delete pData;
		}

		return true;
	}

	ItemExchangeCrashArray * m_pMap;
};