#pragma once

class CKingNominationListSet : public OdbcRecordset
{
public:
	CKingNominationListSet(OdbcConnection * dbConnection, KingSystemArray * pMap) 
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("KING_NOMINATION_LIST"); }
	virtual tstring GetColumns() { return _T("Nation, strNominator, strNominee"); }

	virtual bool Fetch()
	{
		CKingSystem * pData;
		uint8 byNation;
		std::string strNominator, strNominee;
		// Pull the nation first so we can use it as an ID.
		_dbCommand->FetchByte(1, byNation);

		// Hi, I'll take a map for 2 please.
		if (byNation != KARUS && byNation != ELMORAD)
			return false;

		// Do we have an entry for this nation already?
		pData = m_pMap->GetData(byNation);

		// We don't? Nothing to do here.
		if (pData == nullptr)
			return true;

		_dbCommand->FetchString(2, strNominator);
		_dbCommand->FetchString(3, strNominee);

		Guard lock(pData->m_KingSystemlock);
		// Is this user added to the list already?
		KingNominationList::iterator itr = pData->m_nominationList.find(strNominee);

		// Nope, let's add them.
		if (itr == pData->m_nominationList.end())
		{
			_KING_NOMINATION_LIST * pEntry = new _KING_NOMINATION_LIST;
			pEntry->strNominator = strNominator;
			pEntry->strNominee = strNominee;
		
			pData->m_nominationList.insert(make_pair(strNominee, pEntry));
		}
		return true;
	}

	KingSystemArray * m_pMap;
};