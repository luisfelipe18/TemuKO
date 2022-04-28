#pragma once

class CKnightsCastellanCapeListSet : public OdbcRecordset
{
public:
	CKnightsCastellanCapeListSet(OdbcConnection * dbConnection, KnightCastellanCapeArray * pMap)
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("KNIGHTS_CASTELLAN"); }
	virtual tstring GetColumns() { return _T("IDNum, sCape, bCapeR, bCapeG,bCapeB,isActive, RemainingTime"); }

	virtual bool Fetch()
	{
		_KNIGHTS_CASTELLAN_CAPE * pData = new _KNIGHTS_CASTELLAN_CAPE;

		_dbCommand->FetchUInt16(1, pData->sClanID);
		_dbCommand->FetchUInt16(2, pData->sCape);
		_dbCommand->FetchByte(3, pData->sR);
		_dbCommand->FetchByte(4, pData->sG);
		_dbCommand->FetchByte(5, pData->sB);
		_dbCommand->FetchByte(6, pData->sActive);
		_dbCommand->FetchUInt64(7, pData->sRemainingTime);

		if (!m_pMap->PutData(pData->sClanID, pData))
			delete pData;

		return true;
	}

	KnightCastellanCapeArray *m_pMap;
};