#pragma once

class CItemSpecialExchangeSet : public OdbcRecordset
{
public:
	CItemSpecialExchangeSet(OdbcConnection * dbConnection, ItemSpecialExchangeArray *pMap)
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("ITEM_SPECIAL_SEWING"); }
	virtual tstring GetColumns()
	{
		return _T("nIndex, "
			"nReqItemID1, nReqItemCount1, nReqItemID2, nReqItemCount2, "
			"nReqItemID3, nReqItemCount3, nReqItemID4, nReqItemCount4, "
			"nReqItemID5, nReqItemCount5, nReqItemID6, nReqItemCount6, "
			"nReqItemID7, nReqItemCount7, nReqItemID8, nReqItemCount8, "
			"nReqItemID9, nReqItemCount9, nReqItemID10, nReqItemCount10, "
			"nGiveItemID, nGiveItemCount, SuccessRate, sNpcID, isNotice,"
			"isShadowSucces");
	}

	virtual bool Fetch()
	{
		SPECIAL_PART_SEWING_EXCHANGE *pData = new SPECIAL_PART_SEWING_EXCHANGE;

		int i = 1;
		_dbCommand->FetchUInt32(i++, pData->nIndex);

		for (int x = 0; x < ITEMS_SPECIAL_EXCHANGE_GROUP; x++)
		{
			_dbCommand->FetchUInt32(i++, pData->nReqItemID[x]);
			_dbCommand->FetchUInt32(i++, pData->nReqItemCount[x]);
		}

		_dbCommand->FetchUInt32(i++, pData->nGiveItemID);
		_dbCommand->FetchUInt32(i++, pData->nGiveItemCount);
		_dbCommand->FetchUInt32(i++, pData->iSuccessRate);
		_dbCommand->FetchUInt16(i++, pData->sNpcNum);
		_dbCommand->FetchByte(i++, pData->isNotice);
		_dbCommand->FetchByte(i++, pData->isShadowSucces);

		if (!m_pMap->PutData(pData->nIndex, pData))
			delete pData;

		return true;
	}

	ItemSpecialExchangeArray * m_pMap;
};