#pragma once

class CMiningExchange : public OdbcRecordset
{
public:
	CMiningExchange(OdbcConnection * dbConnection, MiningExchangeArray * pMap)
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("MINING_EXCHANGE"); }
	virtual tstring GetColumns() { return _T("nIndex, sNpcID, GiveEffect, OreType, nOriginItemName, nOriginItemNum, nGiveItemName, nGiveItemNum,nGiveItemCount,SuccesRate"); }

	virtual bool Fetch()
	{
		_MINING_EXCHANGE *pData = new _MINING_EXCHANGE;
		int i = 1;

		_dbCommand->FetchUInt16(i++, pData->nIndex);
		_dbCommand->FetchUInt16(i++, pData->sNpcID);
		_dbCommand->FetchByte(i++, pData->GiveEffect);
		_dbCommand->FetchByte(i++, pData->OreType);
		_dbCommand->FetchString(i++, pData->nOriginItemName);
		_dbCommand->FetchUInt32(i++, pData->nOriginItemNum);
		_dbCommand->FetchString(i++, pData->nGiveItemName);
		_dbCommand->FetchUInt32(i++, pData->nGiveItemNum);
		_dbCommand->FetchUInt16(i++, pData->nGiveItemCount);
		_dbCommand->FetchUInt32(i++, pData->SuccesRate);

		if (!m_pMap->PutData(pData->nIndex, pData))
			delete pData;

		return true;
	}

	MiningExchangeArray * m_pMap;
};