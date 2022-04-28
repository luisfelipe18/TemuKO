#pragma once

class CEventRoyaleSchedulePlayTimer : public OdbcRecordset
{
public:
	CEventRoyaleSchedulePlayTimer(OdbcConnection* dbConnection, RoyaleEventPlayTimerArray* pMap)
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("EVENT_ROYALE_PLAY_TIMER"); }
	virtual tstring GetColumns()
	{
		return _T("EventLocalID, EventZoneID, EventName, EventSignTime, EventManuelCloseTime, EventGasStepTimer1, EventGasStepTimer2, EventGasStepTimer3, EventGasStepTimer4, EventGasStepTimer5");
	}

	virtual bool Fetch()
	{
		int i = 1;
		_ROYALE_EVENT_PLAY_TIMER* pData = new _ROYALE_EVENT_PLAY_TIMER;
		_dbCommand->FetchByte(i++, pData->TableEventLocalID);
		_dbCommand->FetchUInt16(i++, pData->EventZoneID);
		_dbCommand->FetchString(i++, pData->EventName);
		_dbCommand->FetchUInt32(i++, pData->EventSignTime);
		_dbCommand->FetchUInt32(i++, pData->EventManuelCloseTimer);
		_dbCommand->FetchUInt32(i++, pData->EventGasStepTimer1);
		_dbCommand->FetchUInt32(i++, pData->EventGasStepTimer2);
		_dbCommand->FetchUInt32(i++, pData->EventGasStepTimer3);
		_dbCommand->FetchUInt32(i++, pData->EventGasStepTimer4);
		_dbCommand->FetchUInt32(i++, pData->EventGasStepTimer5);

		if (!m_pMap->PutData(pData->TableEventLocalID, pData))
			delete pData;

		return true;
	}
	RoyaleEventPlayTimerArray* m_pMap;
};