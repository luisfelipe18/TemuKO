#pragma once

class CDarkKnightMaterialDeadGiftSet : public OdbcRecordset
{
public:
	CDarkKnightMaterialDeadGiftSet(OdbcConnection * dbConnection, DkmMonsterDeadGiftArray *pMap)
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("DKM_MONSTER_DEAD_GIFT"); }
	virtual tstring GetColumns() { return _T("iNum, nGiveItemNum, nGiveItemName, nGiveItemCount, nGivesPercent"); }

	virtual bool Fetch()
	{
		int i = 1;
		_DKM_MONSTER_DEAD_GIFT *pData = new _DKM_MONSTER_DEAD_GIFT;
		_dbCommand->FetchByte(i++, pData->iNum);
		_dbCommand->FetchUInt32(i++, pData->nGiveItemNum);
		_dbCommand->FetchString(i++, pData->nGiveItemName);
		_dbCommand->FetchUInt16(i++, pData->nGiveItemCount);
		_dbCommand->FetchUInt32(i++, pData->nGiveItemPercent);
		if (!m_pMap->PutData(pData->iNum, pData))
			delete pData;

		return true;
	}
	DkmMonsterDeadGiftArray * m_pMap;
};