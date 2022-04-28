#pragma once

class CMonsterResourceSet : public OdbcRecordset
{
public:
	CMonsterResourceSet(OdbcConnection * dbConnection, MonsterResourceArray * pMap) 
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("MONSTER_RESOURCE"); }
	virtual tstring GetColumns() { return _T("sSid, strSidName, strResource, sNoticeZone, sNoticeType"); }

	virtual bool Fetch()
	{
		_MONSTER_RESOURCE *pData = new _MONSTER_RESOURCE;

		_dbCommand->FetchUInt16(1, pData->sSid);
		_dbCommand->FetchString(2, pData->strSidName);
		_dbCommand->FetchString(3, pData->strResource);
		_dbCommand->FetchUInt16(4, pData->sNoticeZone);
		_dbCommand->FetchUInt16(5, pData->sNoticeType);
		
		if (!m_pMap->PutData(pData->sSid, pData))
			delete pData;

		return true;
	}

	MonsterResourceArray *m_pMap;
};