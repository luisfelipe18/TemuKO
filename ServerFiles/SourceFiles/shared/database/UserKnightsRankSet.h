#pragma once

class CUserKnightsRankSet : public OdbcRecordset
{
public:
	CUserKnightsRankSet(OdbcConnection * dbConnection, UserNameRankMap * pkarusMap, UserNameRankMap * pelmoMap)
		: OdbcRecordset(dbConnection), m_pKarusMap(pkarusMap), m_pElmoMap(pelmoMap) {}

	virtual tstring GetTableName() { return _T("USER_KNIGHTS_RANK"); }
	virtual tstring GetColumns() { return _T("shIndex, strName, strElmoUserID, strElmoKnightsName, sElmoKnights, nElmoLoyalty, strKarusUserID, strKarusKnightsName, sKarusKnights, nKarusLoyalty, nMoney"); }

	virtual bool Fetch()
	{
		_USER_RANK *pDataKarus = new _USER_RANK;
		_USER_RANK *pDataElmo = new _USER_RANK;

		std::string strRankName, strElmoUserID, strKarusUserID, strElmoClanName, strKarusClanName;
		uint32 sLoyaltyKarus = 0, sLoyaltyElmo = 0, nSalary = 0;
		uint16 nRank = 0, sElmoKnights = 0, sKarusKnighs = 0;
		int i = 1;
		_dbCommand->FetchUInt16(i++, nRank);
		_dbCommand->FetchString(i++, strRankName);
		_dbCommand->FetchString(i++, strElmoUserID);
		_dbCommand->FetchString(i++, strElmoClanName);
		_dbCommand->FetchUInt16(i++, sElmoKnights);
		_dbCommand->FetchUInt32(i++, sLoyaltyElmo);
		_dbCommand->FetchString(i++, strKarusUserID);
		_dbCommand->FetchString(i++, strKarusClanName);
		_dbCommand->FetchUInt16(i++, sKarusKnighs);
		_dbCommand->FetchUInt32(i++, sLoyaltyKarus);
		_dbCommand->FetchUInt32(i++, nSalary);


		// Copy into struct
		pDataElmo->nRank = nRank;
		pDataElmo->strRankName = strRankName;
		pDataElmo->strUserName = strElmoUserID;
		pDataElmo->strClanName = strElmoClanName;
		pDataElmo->sKnights = sElmoKnights;
		pDataElmo->nLoyalty = sLoyaltyElmo;
		pDataElmo->nSalary = nSalary;

		pDataKarus->nRank = nRank;
		pDataKarus->strRankName = strRankName;
		pDataKarus->strUserName = strKarusUserID;
		pDataKarus->strClanName = strKarusClanName;
		pDataKarus->sKnights = sKarusKnighs;
		pDataKarus->nLoyalty = sLoyaltyKarus;
		pDataKarus->nSalary = nSalary;

		// We're not going to insert either of them, so ignore this row and avoid a mem leak.
		if (strElmoUserID.empty())
		{
			delete pDataElmo;
		}

		if (strKarusUserID.empty())
		{
			delete pDataKarus;
		}

		STRTOUPPER(strKarusUserID);
		STRTOUPPER(strElmoUserID);

		if (!strElmoUserID.empty())
			m_pElmoMap->insert(make_pair(strElmoUserID, pDataElmo));

		if (!strKarusUserID.empty())
			m_pKarusMap->insert(make_pair(strKarusUserID, pDataKarus));

		return true;
	}
	UserNameRankMap *m_pKarusMap;
	UserNameRankMap *m_pElmoMap;
};