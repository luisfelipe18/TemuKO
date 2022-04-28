#pragma once

class CLevelUpTableSet : public OdbcRecordset
{
public:
	CLevelUpTableSet(OdbcConnection * dbConnection, LevelUpArray * pMap)
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("LEVEL_UP"); }
	virtual tstring GetColumns() { return _T("ID, [Level], [Exp], RebithLevel"); }

	virtual bool Fetch()
	{
		int i = 1;

		auto * pData = new _LEVEL_UP();
		_dbCommand->FetchByte(i++, pData->ID);
		_dbCommand->FetchByte(i++, pData->Level);
		_dbCommand->FetchInt64(i++, pData->Exp);
		_dbCommand->FetchByte(i++, pData->RebithLevel);

		LevelUpArray::iterator itr = g_pMain->m_LevelUpArray.find(pData->ID);
		if (itr == g_pMain->m_LevelUpArray.end())
			m_pMap->insert(std::make_pair(pData->ID, pData));

		return true;
	}
	LevelUpArray *m_pMap;
};