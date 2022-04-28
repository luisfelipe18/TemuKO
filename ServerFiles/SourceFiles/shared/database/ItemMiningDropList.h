#pragma once

class CItemMiningDropList : public OdbcRecordset
{
public:
	CItemMiningDropList(OdbcConnection * dbConnection, ItemMiningDropArray * pMap) 
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("ITEM_MINING_LIST"); }
	virtual tstring GetColumns() { return _T("sIndex, Num, strName, sRateNormal, sRateGolden"); }

	virtual bool Fetch()
	{
		_ITEM_MINING_DROP_TABLE *pData = new _ITEM_MINING_DROP_TABLE;

		int i = 1;
		_dbCommand->FetchUInt16(i++, pData->m_sIndex);
		_dbCommand->FetchUInt32(i++, pData->m_iNum);
		_dbCommand->FetchString(i++, pData->m_sName);
		_dbCommand->FetchUInt16(i++, pData->m_sRateNormal);
		_dbCommand->FetchUInt16(i++, pData->m_sRateGolden);
		
		
		if (!m_pMap->PutData(pData->m_sIndex, pData))
			delete pData;

		g_pMain->m_ItemMiningDropListTotalRateNormal += pData->m_sRateNormal; 
		g_pMain->m_ItemMiningDropListTotalRateGolden += pData->m_sRateGolden; 
		// a very nice distribution depending really on their probabilities

		return true;
	}

	ItemMiningDropArray *m_pMap;
};