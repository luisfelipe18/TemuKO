#pragma once

class CMonsterRespawnListSet : public OdbcRecordset
{
public:
	CMonsterRespawnListSet(OdbcConnection * dbConnection, MonsterRespawnListArray * pMap)
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("MONSTER_RESPAWN_LIST"); }
	virtual tstring GetColumns() { return _T("sIndex, sSid, sType, sCount, isDeadTime"); }

	virtual bool Fetch()
	{
		_MONSTER_RESPAWN_LIST * pData = new _MONSTER_RESPAWN_LIST;

		_dbCommand->FetchUInt16(1, pData->sIndex);
		_dbCommand->FetchUInt16(2, pData->sSid);
		_dbCommand->FetchByte(3, pData->sType);
		_dbCommand->FetchUInt16(4, pData->sCount);
		_dbCommand->FetchUInt32(5, pData->isDeadTime);

		if (!m_pMap->PutData(pData->sIndex, pData))
			delete pData;

		return true;
	}

	MonsterRespawnListArray *m_pMap;
};