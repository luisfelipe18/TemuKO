#pragma once
class CKnightRoyaleStartPositionRandomSet : public OdbcRecordset
{
public:
	CKnightRoyaleStartPositionRandomSet(OdbcConnection * dbConnection, KnStartPositionRandomArray * pMap)
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("KNIGHT_ROYALE_START_POSITION"); }
	virtual tstring GetColumns() { return _T("nIndeX, PosX, PosZ, GasStep"); }

	virtual bool Fetch()
	{
		_KN_START_POSITION_RANDOM * pData = new _KN_START_POSITION_RANDOM;

		int i = 1;
		_dbCommand->FetchUInt16(i++, pData->sIndex);
		_dbCommand->FetchInt16(i++, pData->PosX);
		_dbCommand->FetchInt16(i++, pData->PosZ);
		_dbCommand->FetchByte(i++, pData->GasStep);

		pData->sIndex = m_pMap->GetSize();

		if (!m_pMap->PutData(pData->sIndex, pData))
			delete pData;

		return true;
	}

	KnStartPositionRandomArray *m_pMap;
};