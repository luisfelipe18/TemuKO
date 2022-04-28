#pragma once

class CEventScheduleStatus : public OdbcRecordset
{
public:
	CEventScheduleStatus(OdbcConnection* dbConnection, EventScheduleArray* pMap)
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("EVENT_START_SCHEDULE"); }
	virtual tstring GetColumns()
	{
		return _T("EventLocalID, EventType,"
			"EventZoneID,EventName,StartDays,EventStatus, "
			"StartHour1, StartMinute1,TimeActive1, "
			"StartHour2, StartMinute2,TimeActive2, "
			"StartHour3, StartMinute3,TimeActive3, "
			"StartHour4, StartMinute4,TimeActive4, "
			"StartHour5, StartMinute5,TimeActive5 ");
	}

	virtual bool Fetch()
	{
		int i = 1;
		_EVENT_SCHEDULE* pData = new _EVENT_SCHEDULE;
		_dbCommand->FetchByte(i++, pData->EventLocalID);
		_dbCommand->FetchByte(i++, pData->EventType);
		_dbCommand->FetchUInt16(i++, pData->EventZoneID);
		_dbCommand->FetchString(i++, pData->EventName);
		_dbCommand->FetchString(i++, pData->StartDays);
		_dbCommand->FetchByte(i++, pData->EventStatus);

		for (int x = 0; x < EVENT_START_TIMES; x++)
		{
			_dbCommand->FetchUInt32(i++, pData->EventStartHour[x]);
			_dbCommand->FetchUInt32(i++, pData->EventStartMinutes[x]);
			_dbCommand->FetchByte(i++, pData->TimeActive[x]);
		}

		if (!m_pMap->PutData(pData->EventLocalID, pData))
			delete pData;

		return true;
	}
	EventScheduleArray* m_pMap;
};