#pragma once

class CMiningFishingTableSet : public OdbcRecordset
{
public:
	CMiningFishingTableSet(OdbcConnection * dbConnection, MiningFishingItemArray * pMap) 
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("MINING_FISHING_ITEM"); }
	virtual tstring GetColumns() { return _T("nIndex, nTableType, nWarStatus, UseItemType, nGiveItemName, nGiveItemID, nGiveItemCount, SuccessRate"); }

	virtual bool Fetch()
	{
		_MINING_FISHING_ITEM *pData = new _MINING_FISHING_ITEM;
		int i = 1;

		_dbCommand->FetchUInt32(i++, pData->nIndex);
		_dbCommand->FetchUInt32(i++, pData->nTableType);
		_dbCommand->FetchUInt32(i++, pData->nWarStatus);
		_dbCommand->FetchByte(i++, pData->UseItemType);
		_dbCommand->FetchString(i++, pData->nGiveItemName);
		_dbCommand->FetchUInt32(i++, pData->nGiveItemID);
		_dbCommand->FetchUInt32(i++, pData->nGiveItemCount);
		_dbCommand->FetchUInt32(i++, pData->SuccessRate);

		if (pData->SuccessRate > 10000)
			pData->SuccessRate = 10000;

		if (!m_pMap->PutData(pData->nIndex, pData))
			delete pData;
		
		return true;
	}

	MiningFishingItemArray * m_pMap;
};