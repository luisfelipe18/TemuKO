#pragma once

class CMonsterRespawnVeriableListSet : public OdbcRecordset
{
public:
	CMonsterRespawnVeriableListSet(OdbcConnection * dbConnection, MonsterVeriableRespawnListArray * pMap)
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("MONSTER_RESPAWN_VARIABLE_LIST"); }
	virtual tstring GetColumns() { return _T("sIndex, sSid, sType,sZoneID, sCount, sRadius, isDeadTime"); }

	virtual bool Fetch()
	{
		_MONSTER_RESPAWN_VERIABLE_LIST * pData = new _MONSTER_RESPAWN_VERIABLE_LIST;

		_dbCommand->FetchUInt16(1, pData->sIndex);
		_dbCommand->FetchUInt16(2, pData->sSid);
		_dbCommand->FetchByte(3, pData->sType);
		_dbCommand->FetchByte(4, pData->sZoneID);
		_dbCommand->FetchUInt16(5, pData->sCount);
		_dbCommand->FetchUInt16(6, pData->sRadius);
		_dbCommand->FetchUInt32(7, pData->isDeadTime);

		if (!m_pMap->PutData(pData->sIndex, pData))
			delete pData;

		return true;
	}

	MonsterVeriableRespawnListArray *m_pMap;
};