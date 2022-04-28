#pragma once

class CEventBeefSchedulePlayTimer : public OdbcRecordset
{
public:
	CEventBeefSchedulePlayTimer(OdbcConnection* dbConnection, BeefEventPlayTimerArray* pMap)
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("EVENT_BEEF_PLAY_TIMER"); }
	virtual tstring GetColumns()
	{
		return _T("EventLocalID, EventZoneID, EventName, MonumentTime, LoserSignTime, FarmingTime");
	}

	virtual bool Fetch()
	{
		int i = 1;
		_BEEF_EVENT_PLAY_TIMER* pData = new _BEEF_EVENT_PLAY_TIMER;
		_dbCommand->FetchByte(i++, pData->TableEventLocalID);
		_dbCommand->FetchUInt16(i++, pData->EventZoneID);
		_dbCommand->FetchString(i++, pData->EventName);
		_dbCommand->FetchUInt32(i++, pData->MonumentTime);
		_dbCommand->FetchUInt32(i++, pData->LoserSignTime);
		_dbCommand->FetchUInt32(i++, pData->FarmingTime);

		if (!m_pMap->PutData(pData->TableEventLocalID, pData))
			delete pData;

		return true;
	}
	BeefEventPlayTimerArray* m_pMap;
};