#pragma once

class CAchieveMonsterSet : public OdbcRecordset
{
public:
	CAchieveMonsterSet(OdbcConnection * dbConnection, AchieveMonsterArray *pMap) 
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("ACHIEVE_MON"); }
	virtual tstring GetColumns() 
	{
		return _T("sIndex, Type, byte, Monster1_1, Monster1_2, Monster1_3, Monster1_4, MonCount1, \
				  Monster2_1, Monster2_2, Monster2_3, Monster2_4, MonCount2");
	}

	virtual bool Fetch()
	{
		_ACHIEVE_MONSTER *pData = new _ACHIEVE_MONSTER();
		uint32 col = 1;

		_dbCommand->FetchUInt32(col++, pData->sIndex);
		_dbCommand->FetchByte(col++, pData->Type);
		_dbCommand->FetchByte(col++, pData->byte);

		for (uint32 group = 0; group < QUEST_MOB_GROUPS; group++)
		{
			for (uint32 i = 0; i < QUEST_MOBS_PER_GROUP; i++)
				_dbCommand->FetchUInt32(col++, pData->sNum[group][i]);
			_dbCommand->FetchUInt32(col++, pData->sCount[group]);
		}

		if (!m_pMap->PutData(pData->sIndex, pData))
			delete pData;

		return true;
	}

	AchieveMonsterArray * m_pMap;
};