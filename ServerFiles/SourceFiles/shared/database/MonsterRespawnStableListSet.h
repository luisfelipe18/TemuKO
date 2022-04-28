#pragma once

class CMonsterRespawnStableListSet : public OdbcRecordset
{
public:
	CMonsterRespawnStableListSet(OdbcConnection * dbConnection, MonsterStableRespawnListArray * pMap)
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("MONSTER_RESPAWN_STABLE_LIST"); }
	virtual tstring GetColumns() { return _T("sIndex,GroupNumber, sSid, isNpc,sZoneID, sCount,sRadius, isDeadTime"); }

	virtual bool Fetch()
	{
		_MONSTER_RESPAWN_STABLE_LIST * pData = new _MONSTER_RESPAWN_STABLE_LIST;
		int i = 1;
		_dbCommand->FetchUInt16(i++, pData->sIndex);
		_dbCommand->FetchUInt16(i++, pData->GroupNumber);
		_dbCommand->FetchUInt16(i++, pData->sSid);
		_dbCommand->FetchByte(i++, pData->isNpc);
		_dbCommand->FetchByte(i++, pData->sZoneID);
		_dbCommand->FetchUInt16(i++, pData->sCount);
		_dbCommand->FetchByte(i++, pData->sRadius);
		_dbCommand->FetchUInt32(i++, pData->isDeadTime);

		if (!m_pMap->PutData(pData->sIndex, pData))
			delete pData;

		return true;
	}

	MonsterStableRespawnListArray*m_pMap;
};