#pragma once

class CDungeonDefenceStageListSet : public OdbcRecordset
{
public:
	CDungeonDefenceStageListSet(OdbcConnection * dbConnection, DungeonDefenceStageListArray *pMap)
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("DF_STAGE_LIST"); }
	virtual tstring GetColumns()
	{
		return _T("ID, Difficulty, DifficultyName, sStageID");
	}

	virtual bool Fetch()
	{
		_DUNGEON_DEFENCE_STAGE_LIST *pData = new _DUNGEON_DEFENCE_STAGE_LIST;

		int i = 1;
		_dbCommand->FetchUInt32(i++, pData->ID);
		_dbCommand->FetchByte(i++, pData->Difficulty);
		_dbCommand->FetchString(i++, pData->DifficultyName);
		_dbCommand->FetchByte(i++, pData->sStageID);

		if (!m_pMap->PutData(pData->ID, pData))
			delete pData;

		return true;
	}

	DungeonDefenceStageListArray * m_pMap;
};