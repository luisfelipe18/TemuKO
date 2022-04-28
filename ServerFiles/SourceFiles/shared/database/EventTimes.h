#pragma once
 
class CEventTimes : public OdbcRecordset
{
public:
	CEventTimes(OdbcConnection * dbConnection, EventTimesArray * pMap) 
    : OdbcRecordset(dbConnection), m_pMap(pMap) {}
	
	virtual tstring GetTableName() { return _T("EVENT_TIMES"); }
	virtual tstring GetColumns() { return _T("sIndex, bTime1, bTime2, bTime3, bLvMin, bLvMax, AllDays"); }
 
	virtual bool Fetch()
	{
		_EVENT_TIMES * pData = new _EVENT_TIMES;
		
		_dbCommand->FetchUInt16(1, pData->sIndex);
		_dbCommand->FetchByte(2, pData->bTime1);
		_dbCommand->FetchByte(3, pData->bTime2);
		_dbCommand->FetchByte(4, pData->bTime3);
		_dbCommand->FetchByte(5, pData->bLvMin);
		_dbCommand->FetchByte(6, pData->bLvMax);
		_dbCommand->FetchByte(7, pData->AllDays);
 
     if (!m_pMap->PutData(pData->sIndex, pData))
       delete pData;
 
     return true;
   }
 
   EventTimesArray *m_pMap;
 };