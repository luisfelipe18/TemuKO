#pragma once

class CDungeonDefenceMonsterListSet : public OdbcRecordset
{
public:
	CDungeonDefenceMonsterListSet(OdbcConnection * dbConnection, DungeonDefenceMonsterListArray *pMap)
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("DF_MONSTER_LIST"); }
	virtual tstring GetColumns()
	{
		return _T("ID, Diffuculty, MonsterID, isMonster, PosX, PosZ, sCount, sDirection, sRadiusRange");
	}

	virtual bool Fetch()
	{
		_DUNGEON_DEFENCE_MONSTER_LIST *pData = new _DUNGEON_DEFENCE_MONSTER_LIST;

		int i = 1;
		_dbCommand->FetchUInt32(i++, pData->ID);
		_dbCommand->FetchByte(i++, pData->Diffuculty);
		_dbCommand->FetchUInt16(i++, pData->MonsterID);
		_dbCommand->FetchByte(i++, pData->isMonster);
		_dbCommand->FetchUInt16(i++, pData->PosX);
		_dbCommand->FetchUInt16(i++, pData->PosZ);
		_dbCommand->FetchUInt16(i++, pData->sCount);
		_dbCommand->FetchUInt16(i++, pData->sDirection);
		_dbCommand->FetchUInt16(i++, pData->sRadiusRange);


		if (!m_pMap->PutData(pData->ID, pData))
			delete pData;

		return true;
	}

	DungeonDefenceMonsterListArray * m_pMap;
};