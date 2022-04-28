#pragma once

class CNpcItemSet : public OdbcRecordset
{
public:
	CNpcItemSet(OdbcConnection * dbConnection, NpcItemArray * pMap)
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("K_NPC_ITEM"); }
	virtual tstring GetColumns() { return _T("sIndex, iItem01, sPersent01, iItem02, sPersent02, iItem03, sPersent03, iItem04, sPersent04, iItem05, sPersent05, iItem06, sPersent06, iItem07, sPersent07"); }

	virtual bool Fetch()
	{
		_K_NPC_ITEM *pData = new _K_NPC_ITEM;

		_dbCommand->FetchUInt16(1, pData->sIndex);
		for (int i = 0, j = 2; i < LOOT_DROP_ITEMS; i++)
		{
			_dbCommand->FetchUInt32(j++, pData->iItem[i]);
			_dbCommand->FetchUInt16(j++, pData->sPercent[i]);
		}

		if (!m_pMap->PutData(pData->sIndex, pData))
			delete pData;

		return true;
	}

	NpcItemArray *m_pMap;
};