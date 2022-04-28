#include "stdafx.h"
#include "DBAgent.h"

#pragma region NTTGame Nation Transfer
void CUser::ReqNationTransfer(Packet & pkt)
{
	if (!isInGame())
		return;

	uint8 bOpcode = pkt.read<uint8>();

	switch (bOpcode)
	{
	case 1:
		ReqSendNationTransfer();
		break;
	case 2:
		ReqHandleNationChange(pkt);
		break;
	default:
		printf("Nation Transfer unhandled packets %d \n",bOpcode);
		TRACE("Nation Transfer unhandled packets %d \n", bOpcode);
		break;
	}
}

void CUser::SendNationTransfer()
{
	if (!isInGame())
		return;

	if (!CheckExistItem(ITEM_NATION_TRANSFER)) // Item does exist.	
	{
		Packet result(WIZ_NATION_TRANSFER);
		result << uint8(NationOpenBox)
			<< uint8(7);
		Send(&result);
		return;
	}

	Packet result(WIZ_NATION_TRANSFER, uint8(1));
	g_pMain->AddDatabaseRequest(result, this);
}

void CUser::ReqHandleNationChange(Packet & pkt)
{
	Packet result(WIZ_NATION_TRANSFER);
	uint8 kNation, kSuccess, kSuccess2, kAccountCount;
	_NATION_DATA requestData[4];
	CSTLMap <_NATION_DATA> m_NationTransferArray;
	uint16	CharClass = 0, nNum = 0;
	uint8	AccountCount = 0, bError = 0;
	std::string strCharID1, strCharID2, strCharID3, strCharID4;
	float Px = (float)818.000000, Pz = (float)435.000000, Py = (float)4.590000;
	uint8 bCurrentNation = GetNation();

	if (!CheckExistItem(ITEM_NATION_TRANSFER))
	{
		bError = 7;
		goto fail_return;
	}

	if (g_pMain->isWarOpen())
	{
		bError = 1;
		goto fail_war;
	}

	g_DBAgent.GetAllCharID(m_strAccountID, strCharID1, strCharID2, strCharID3, strCharID4);
	CKingSystem* pKingSystem = g_pMain->m_KingSystemArray.GetData(GetNation());

	if (pKingSystem != nullptr)
	{
		if ((strCharID1.length() > 0 && STRCASECMP(pKingSystem->m_strKingName.c_str(), strCharID1.c_str()) == 0)
			|| (strCharID2.length() > 0 && STRCASECMP(pKingSystem->m_strKingName.c_str(), strCharID2.c_str()) == 0)
			|| (strCharID3.length() > 0 && STRCASECMP(pKingSystem->m_strKingName.c_str(), strCharID3.c_str()) == 0)
			|| (strCharID4.length() > 0 && STRCASECMP(pKingSystem->m_strKingName.c_str(), strCharID4.c_str()) == 0))
		{
			bError = 3;
			goto fail_return;
		}
	}

	if (strCharID1.length() != 0)
	{
		_NATION_DATA* pNationTransfer = new _NATION_DATA();
		g_DBAgent.GetAllCharInfo(strCharID1, nNum, pNationTransfer);

		if (pNationTransfer->sClanID > 0)
		{
			if (pNationTransfer)
				delete pNationTransfer;

			bError = 2;
			goto fail_return;
		}

		m_NationTransferArray.PutData(pNationTransfer->nNum, pNationTransfer);
		AccountCount++;
	}

	if (strCharID2.length() != 0)
	{
		_NATION_DATA* pNationTransfer = new _NATION_DATA();
		g_DBAgent.GetAllCharInfo(strCharID2, nNum, pNationTransfer);

		if (pNationTransfer->sClanID > 0)
		{
			if (pNationTransfer)
				delete pNationTransfer;

			bError = 2;
			goto fail_return;
		}

		m_NationTransferArray.PutData(pNationTransfer->nNum, pNationTransfer);
		AccountCount++;
	}

	if (strCharID3.length() != 0)
	{
		_NATION_DATA* pNationTransfer = new _NATION_DATA();
		g_DBAgent.GetAllCharInfo(strCharID3, nNum, pNationTransfer);

		if (pNationTransfer->sClanID > 0)
		{
			if (pNationTransfer)
				delete pNationTransfer;

			bError = 2;
			goto fail_return;
		}

		m_NationTransferArray.PutData(pNationTransfer->nNum, pNationTransfer);
		AccountCount++;
	}

	if (strCharID4.length() != 0)
	{
		_NATION_DATA* pNationTransfer = new _NATION_DATA();
		g_DBAgent.GetAllCharInfo(strCharID4, nNum, pNationTransfer);

		if (pNationTransfer->sClanID > NO_CLAN)
		{
			if (pNationTransfer)
				delete pNationTransfer;
			bError = 2;
			goto fail_return;
		}

		m_NationTransferArray.PutData(pNationTransfer->nNum, pNationTransfer);
		AccountCount++;
	}

	pkt >> kSuccess >> kSuccess2 >> kAccountCount;

	if (AccountCount != kAccountCount)
	{
		bError = 5;
		goto fail_return;
	}

	for (int x = 0; x < kAccountCount; x++)
	{
		pkt >> requestData[x].nNum
			>> requestData[x].bCharName
			>> requestData[x].bRace
			>> requestData[x].bFace
			>> requestData[x].nHair;
	}

	for (int x = 0; x < kAccountCount; x++)
	{
		_NATION_DATA * pNation = m_NationTransferArray.GetData(requestData[x].nNum);
		uint8 nNewRace = 0;

		if (!pNation)
		{
			bError = 5;
			goto fail_return;
		}

		if (requestData[x].bCharName != pNation->bCharName)
		{
			bError = 5;
			goto fail_return;
		}

		if (bCurrentNation == KARUS)
		{
			if (pNation->bRace == KARUS_BIG)
			{
				if (requestData[x].bRace != BABARIAN
					&& requestData[x].bRace != ELMORAD_MAN
					&& requestData[x].bRace != ELMORAD_WOMAN)
				{
					bError = 5;
					goto fail_return;
				}
			}
			else if (pNation->bRace == KARUS_MIDDLE
				|| pNation->bRace == KARUS_SMALL
				|| pNation->bRace == KARUS_WOMAN)
			{
				if (requestData[x].bRace != ELMORAD_MAN
					&& requestData[x].bRace != ELMORAD_WOMAN)
				{
					bError = 5;
					goto fail_return;
				}
			}
			else if (pNation->bRace == KURIAN)
			{
				if (requestData[x].bRace != PORUTU)
				{
					bError = 5;
					goto fail_return;
				}
			}
			else
			{
				bError = 5;
				goto fail_return;
			}
		}
		else
		{
			// El Morad Warriors
			if (pNation->sClass % 100 == 1
				|| pNation->sClass % 100 == 5
				|| pNation->sClass % 100 == 6)
			{
				if (requestData[x].bRace != KARUS_BIG)
				{
					bError = 5;
					goto fail_return;
				}
			}
			// El Morad Rogues
			else if (pNation->sClass % 100 == 2
				|| pNation->sClass % 100 == 7
				|| pNation->sClass % 100 == 8)
			{
				if (requestData[x].bRace != KARUS_MIDDLE)
				{
					bError = 5;
					goto fail_return;
				}
			}
			// El Morad Mage
			else if (pNation->sClass % 100 == 3
				|| pNation->sClass % 100 == 9
				|| pNation->sClass % 100 == 10)
			{
				if (requestData[x].bRace != KARUS_SMALL
					&& requestData[x].bRace != KARUS_WOMAN)
				{
					bError = 5;
					goto fail_return;
				}
			}
			// Elmorad Priest
			else if (pNation->sClass % 100 == 4
				|| pNation->sClass % 100 == 11
				|| pNation->sClass % 100 == 12)
			{
				if (requestData[x].bRace != KARUS_WOMAN
					&& requestData[x].bRace != KARUS_MIDDLE)
				{
					bError = 5;
					goto fail_return;
				}
			}
			// Elmorad Porutu
			else if (pNation->sClass % 100 == 13
				|| pNation->sClass % 100 == 14
				|| pNation->sClass % 100 == 15)
			{
				if (requestData[x].bRace != KURIAN)
				{
					bError = 5;
					goto fail_return;
				}
			}
			else
			{
				bError = 5;
				goto fail_return;
			}

		}
	}

	for (int x = 0; x < kAccountCount; x++)
	{
		_NATION_DATA * pNation = m_NationTransferArray.GetData(requestData[x].nNum);
		uint8 nNewRace = 0;

		if (!pNation)
		{
			bError = 5;
			goto fail_return;
		}

		uint16 kClass = 0;
		if (bCurrentNation == KARUS)
		{
			kNation = ELMORAD;
			kClass = (pNation->sClass + 100);
		}
		else
		{
			kNation = KARUS;
			kClass = (pNation->sClass - 100);
		}

		if (requestData[x].bCharName == m_strUserID)
		{
			m_bRace = requestData[x].bRace;
			m_sClass = kClass;
			m_bNation = kNation;
			m_nHair = requestData[x].nHair;
			m_bFace = requestData[x].bFace;
			m_curx = Px;
			m_curz = Pz;
			m_cury = Py;
		}

		int8 bRet = g_DBAgent.NationTransferSaveUser(m_strAccountID, pNation->bCharName, requestData[x].nNum, requestData[x].bRace, kClass, kNation, 0, 0);

		if (bRet > 0)
		{
			bError = 0;
			goto fail_return;
		}
	}

	m_NationTransferArray.DeleteAllData();

	RobItem(ITEM_NATION_TRANSFER);
	Disconnect();
	result << kSuccess
		<< kSuccess2;
	Send(&result);
	return;

fail_return:
	result << uint8(NationOpenBox)
		<< uint8(bError);
	Send(&result);
	return;

fail_war:
	result << uint8(NationWarStatus)
		<< bError
		<< uint16(1)
		<< uint16(2);
	Send(&result);
}

void CUser::HandleNationChange(Packet & pkt)
{
	if (!isInGame() || g_pMain->isWarOpen())
	{
		Packet result(WIZ_NATION_TRANSFER);
		result << uint8(NationOpenBox)
			<< uint8(0);
		Send(&result);
		return;
	}

	if (!CheckExistItem(ITEM_NATION_TRANSFER))
	{
		Packet result(WIZ_NATION_TRANSFER);
		result << uint8(NationOpenBox)
			<< uint8(7);
		Send(&result);
		return;
	}

	uint8 kSuccess, kSuccess2, kAccountCount;
	_NATION_DATA requestData[4];
	pkt >> kSuccess >> kSuccess2 >> kAccountCount;

	if (kSuccess2 != 1)
		return;

	for (int x = 0; x < kAccountCount; x++)
	{
		pkt >> requestData[x].nNum
			>> requestData[x].bCharName
			>> requestData[x].bRace
			>> requestData[x].bFace
			>> requestData[x].nHair;
	}

	Packet result(WIZ_NATION_TRANSFER, uint8(2));
	result << kSuccess << kSuccess2 << kAccountCount;

	for (int x = 0; x < kAccountCount; x++)
	{
		result << requestData[x].nNum
			<< requestData[x].bCharName
			<< requestData[x].bRace
			<< requestData[x].bFace
			<< requestData[x].nHair;
	}

	g_pMain->AddDatabaseRequest(result, this);
}

void CUser::ReqSendNationTransfer()
{
	Packet result(WIZ_NATION_TRANSFER);
	uint16	CharClass = 0, nNum = 0;
	uint8	CharNation, CharRace, AccountCount = 0, error = 1;
	std::string strCharID1, strCharID2, strCharID3, strCharID4;
	CSTLMap <_NATION_DATA> m_NationTransferArray;

	if (g_pMain->isWarOpen())
	{
		error = 1;
		goto ErrorWar;
	}

	g_DBAgent.GetAllCharID(m_strAccountID, strCharID1, strCharID2, strCharID3, strCharID4);
	CKingSystem* pKingSystem = g_pMain->m_KingSystemArray.GetData(GetNation());

	if (pKingSystem != nullptr)
	{
		if ((strCharID1.length() > 0 && STRCASECMP(pKingSystem->m_strKingName.c_str(), strCharID1.c_str()) == 0)
			|| (strCharID2.length() > 0 && STRCASECMP(pKingSystem->m_strKingName.c_str(), strCharID2.c_str()) == 0)
			|| (strCharID3.length() > 0 && STRCASECMP(pKingSystem->m_strKingName.c_str(), strCharID3.c_str()) == 0)
			|| (strCharID4.length() > 0 && STRCASECMP(pKingSystem->m_strKingName.c_str(), strCharID4.c_str()) == 0))
		{
			error = 3;
			goto ErrorInClan_IsKing;
		}
	}

	//error 3 king.
	//error 4 þerif sisteminde soruþturma altýnda ise.
	//error 9 making over.soru

	if (strCharID1.length() != 0)
	{
		_NATION_DATA* pNationTransfer = new _NATION_DATA();
		g_DBAgent.GetAllCharInfo(strCharID1, nNum, pNationTransfer);

		if (pNationTransfer->sClanID > 0)
		{
			error = 2;
			goto ErrorInClan_IsKing;
		}

		m_NationTransferArray.PutData(pNationTransfer->nNum, pNationTransfer);
		AccountCount++;
	}
	if (strCharID2.length() != 0)
	{
		_NATION_DATA* pNationTransfer = new _NATION_DATA();
		g_DBAgent.GetAllCharInfo(strCharID2, nNum, pNationTransfer);

		if (pNationTransfer->sClanID > 0)
		{
			error = 2;
			goto ErrorInClan_IsKing;
		}

		m_NationTransferArray.PutData(pNationTransfer->nNum, pNationTransfer);
		AccountCount++;
	}

	if (strCharID3.length() != 0)
	{
		_NATION_DATA* pNationTransfer = new _NATION_DATA();
		g_DBAgent.GetAllCharInfo(strCharID3, nNum, pNationTransfer);

		if (pNationTransfer->sClanID > 0)
		{
			error = 2;
			goto ErrorInClan_IsKing;
		}

		m_NationTransferArray.PutData(pNationTransfer->nNum, pNationTransfer);
		AccountCount++;
	}

	if (strCharID4.length() != 0)
	{
		_NATION_DATA* pNationTransfer = new _NATION_DATA();
		g_DBAgent.GetAllCharInfo(strCharID4, nNum, pNationTransfer);

		if (pNationTransfer->sClanID > NO_CLAN)
		{
			error = 2;
			goto ErrorInClan_IsKing;
		}

		m_NationTransferArray.PutData(pNationTransfer->nNum, pNationTransfer);
		AccountCount++;
	}

	result << uint8(NationOpenBox)		// 1 = war status || 2 = open box || 3 = success transfered
		<< uint8(error)						// 1 Sopen Box || 2 = if any in clan error
		<< uint8(AccountCount);				// Character count in Account

	for (int x = 0; x < AccountCount; x++)
	{
		_NATION_DATA * pNation = m_NationTransferArray.GetData(x);

		if (!pNation)
		{
			error = 5;
			goto ErrorInClan_IsKing;
		}

		if (GetNation() == KARUS)
		{
			if (pNation->bRace == KARUS_BIG)
				CharRace = BABARIAN;
			else if (pNation->bRace == KARUS_MIDDLE)
				CharRace = ELMORAD_MAN;
			else if (pNation->bRace == KARUS_SMALL)
				CharRace = ELMORAD_MAN;
			else if (pNation->bRace == KARUS_WOMAN)
				CharRace = ELMORAD_WOMAN;
			else if (pNation->bRace == KURIAN)
				CharRace = PORUTU;

			CharNation = ELMORAD;
			CharClass = (pNation->sClass + 100);
		}
		else
		{
			if (m_bRace == BABARIAN)
				CharRace = KARUS_BIG;
			else if (m_bRace == ELMORAD_MAN && pNation->sClass == 206)
				CharRace = KARUS_BIG;
			else if (m_bRace == ELMORAD_WOMAN && pNation->sClass == 206)
				CharRace = KARUS_BIG;
			else if (m_bRace == ELMORAD_MAN && pNation->sClass == 208)
				CharRace = KARUS_MIDDLE;
			else if (m_bRace == ELMORAD_WOMAN && pNation->sClass == 208)
				CharRace = KARUS_MIDDLE;
			else if (m_bRace == ELMORAD_MAN && pNation->sClass == 210)
				CharRace = KARUS_SMALL;
			else if (m_bRace == ELMORAD_WOMAN && pNation->sClass == 210)
				CharRace = KARUS_WOMAN;
			else if (m_bRace == ELMORAD_MAN && pNation->sClass == 212)
				CharRace = KARUS_MIDDLE;
			else if (m_bRace == ELMORAD_WOMAN && pNation->sClass == 212)
				CharRace = KARUS_WOMAN;
			else if (m_bRace == PORUTU)
				CharRace = KURIAN;

			CharNation = KARUS;
			CharClass = (pNation->sClass - 100);
		}

		result << uint16(pNation->nNum)
			<< pNation->bCharName
			<< uint8(CharRace)
			<< uint8(CharNation)
			<< uint16(CharClass)
			<< uint8(pNation->bFace)
			<< uint32(pNation->nHair);
	}

	Send(&result);
	return;

ErrorInClan_IsKing:
	result << uint8(NationOpenBox)
		<< error;
	Send(&result);
	return;

ErrorWar:
	result << uint8(NationWarStatus)
		<< error
		<< uint16(1)
		<< uint16(2);
	Send(&result);
}

#pragma endregion

#pragma region Nation Transfer
uint8 CUser::NationChange()
{
	if (!CheckExistItem(ITEM_NATION_TRANSFER))
		return 1; // Item does exist.

	uint8 nRet = g_DBAgent.NationTransfer(GetAccountName());

	if (nRet > 0)
		return nRet; // 2 = Character(s) in the clan / 3 = Database error.

	RobItem(ITEM_NATION_TRANSFER);

	// if database operation is successfull but inline code have a problem disconnect to user...
	if (GetNewRace() == 0)
		Disconnect();

	m_bRank = 0;
	m_bTitle = 0;
	m_bRace = GetNewRace();
	m_sClass = GetNation() == KARUS ? GetClass() + 100 : GetClass() - 100;
	m_bNation = GetNation() == KARUS ? ELMORAD : KARUS;

	if (GetHealth() < (GetMaxHealth() / 2))
		HpChange(GetMaxHealth());

	SendMyInfo();

	UserInOut(INOUT_OUT);
	SetRegion(GetNewRegionX(), GetNewRegionZ());
	UserInOut(INOUT_WARP);

	g_pMain->RegionUserInOutForMe(this);
	g_pMain->RegionNpcInfoForMe(this);
	g_pMain->MerchantUserInOutForMe(this);

	ResetWindows();

	InitType4();
	RecastSavedMagic();

	return 0; // Successfull
}

uint8 CUser::GetNewRace()
{
	uint8 nNewRace = 0;

	if (GetNation() == KARUS)
	{
		if (m_bRace == KARUS_BIG)
			nNewRace = BABARIAN;
		else if (m_bRace == KARUS_MIDDLE)
			nNewRace = ELMORAD_MAN;
		else if (m_bRace == KARUS_SMALL)
			nNewRace = ELMORAD_MAN;
		else if (m_bRace == KARUS_WOMAN)
			nNewRace = ELMORAD_WOMAN;
		else if (m_bRace == KURIAN)
			nNewRace = PORUTU;
	}
	else
	{
		if (m_bRace == BABARIAN)
			nNewRace = KARUS_BIG;
		// El Morad Male and El Morad Warriors
		else if (m_bRace == ELMORAD_MAN && isWarrior())
			nNewRace = KARUS_BIG;
		// El Morad Female and El Morad Warriors
		else if (m_bRace == ELMORAD_WOMAN && isWarrior())
			nNewRace = KARUS_BIG;
		// El Morad Male and El Morad Rogues
		else if (m_bRace == ELMORAD_MAN && isRogue())
			nNewRace = KARUS_MIDDLE;
		// El Morad Female and El Morad Rogues
		else if (m_bRace == ELMORAD_WOMAN && isRogue())
			nNewRace = KARUS_MIDDLE;
		// El Morad Male and El Morad Magicians
		else if (m_bRace == ELMORAD_MAN && isMage())
			nNewRace = KARUS_SMALL;
		// El Morad Female and El Morad Magicians
		else if (m_bRace == ELMORAD_WOMAN && isMage())
			nNewRace = KARUS_WOMAN;
		// El Morad Male and El Morad Priests
		else if (m_bRace == ELMORAD_MAN && isPriest())
			nNewRace = KARUS_MIDDLE;
		// El Morad Female and El Morad Priests
		else if (m_bRace == ELMORAD_WOMAN && isPriest())
			nNewRace = KARUS_WOMAN;
		else if (m_bRace == PORUTU)
			nNewRace = KURIAN;
	}
	return nNewRace;
}

#pragma endregion