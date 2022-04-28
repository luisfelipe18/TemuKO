#pragma once

class CPetImageChane : public OdbcRecordset
{
public:
	CPetImageChane(OdbcConnection* dbConnection, PetTransformSystemArray* pMap)
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("PET_IMAGE_CHANGE"); }
	virtual tstring GetColumns() { return _T("sIndex, nReqItem0, nReqItem1, nReqItem2, nReplaceItem, sReplaceSpid, sReplaceSize, sPercent"); }

	virtual bool Fetch()
	{
		PET_TRANSFORM* pData = new PET_TRANSFORM;

		int i = 1;
		_dbCommand->FetchUInt32(i++, pData->sIndex);
		_dbCommand->FetchUInt32(i++, pData->nReqItem0);
		_dbCommand->FetchUInt32(i++, pData->nReqItem1);
		_dbCommand->FetchUInt32(i++, pData->nReqItem2);
		_dbCommand->FetchUInt32(i++, pData->nReplaceItem);
		_dbCommand->FetchUInt16(i++, pData->sReplaceSpid);
		_dbCommand->FetchUInt16(i++, pData->sReplaceSize);
		_dbCommand->FetchUInt16(i++, pData->sPercent);

		if (!m_pMap->PutData(pData->sIndex, pData))
			delete pData;

		return true;
	}
	PetTransformSystemArray* m_pMap;
};