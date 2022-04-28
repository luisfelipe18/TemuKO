#pragma once

class CDungeonDefenceMonsterListSet : public OdbcRecordset
{
public:
	CDungeonDefenceMonsterListSet(OdbcConnection * dbConnection, DungeonDefenceMonsterListArray *pMap)
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("DF_MONSTER_LIST"); }
	virtual tstring GetColumns()
	{
		return _T("ID, sStageID, MonsterID, PosX, PosZ, sDirection, isMonster");
	}

	virtual bool Fetch()
	{
		_DF_MONSTER_LIST *pData = new _DF_MONSTER_LIST;

		int i = 1;
		_dbCommand->FetchUInt32(i++, pData->ID);
		_dbCommand->FetchByte(i++, pData->sStageID);
		_dbCommand->FetchUInt16(i++, pData->MonsterID);
		_dbCommand->FetchUInt16(i++, pData->PosX);
		_dbCommand->FetchUInt16(i++, pData->PosZ);
		_dbCommand->FetchUInt16(i++, pData->sDirection);
		_dbCommand->FetchByte(i++, pData->isMonster);

		if (!m_pMap->PutData(pData->sIndex, pData))
			delete pData;

		return true;
	}

	DungeonDefenceMonsterListArray * m_pMap;
};