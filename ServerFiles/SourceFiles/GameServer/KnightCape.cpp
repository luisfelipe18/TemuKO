#include "stdafx.h"

void CUser::HandleCapeChange(Packet & pkt)
{
	Packet result(WIZ_CAPE);
	CKnights *pKnights = nullptr;
	CKnights *pMainKnights = nullptr;
	_KNIGHTS_ALLIANCE* pAlliance = nullptr;
	_KNIGHTS_CAPE *pCape = nullptr;
	uint32 nReqClanPoints = 0, nReqCoins = 0;
	int16 sErrorCode = 0, sCapeID;
	uint8 r, g, b;
	bool bApplyingPaint = false;
	uint8 TicketExchange;

	pkt >> TicketExchange >> sCapeID >> r >> g >> b;
	// If we're not a clan leader, what are we doing changing the cape?
	if (!isClanLeader()
		|| isDead())
	{
		sErrorCode = -1;
		result << sErrorCode;
		Send(&result);
		return;
	}

	// Does the clan exist?
	pKnights = g_pMain->GetClanPtr(GetClanID());
	if (pKnights == nullptr)
	{
		sErrorCode = -2;
		result << sErrorCode;
		Send(&result);
		return;
	}

	// Make sure we're promoted
	if (!pKnights->isPromoted())
	{
		sErrorCode = -1;
		result << sErrorCode;
		Send(&result);
		return;
	}

	// and that if we're in an alliance, we're the primary clan in the alliance.
	if (pKnights->isInAlliance())
	{
		pMainKnights = g_pMain->GetClanPtr(pKnights->GetAllianceID());
		pAlliance = g_pMain->GetAlliancePtr(pKnights->GetAllianceID());
		if (pMainKnights == nullptr
			|| pAlliance == nullptr
			|| (pAlliance->sMainAllianceKnights != pKnights->GetID()
				&& pAlliance->sSubAllianceKnights != pKnights->GetID())
			|| (pAlliance->sSubAllianceKnights == pKnights->GetID()
				&& sCapeID >= 0))
		{
			sErrorCode = -1;
			result << sErrorCode;
			Send(&result);
			return;
		}
	}
	if (TicketExchange == 1)
	{
		_KNIGHTS_SIEGE_WARFARE *pKnightSiege = g_pMain->GetSiegeMasterKnightsPtr(Aktive);
		if (pKnightSiege == nullptr)
		{
			sErrorCode = -10;
			result << sErrorCode;
			Send(&result);
			return;
		}

		if (pKnights->GetID() != pKnightSiege->sMasterKnights)
		{
			sErrorCode = -10;
			result << sErrorCode;
			Send(&result);
			return;
		}
		//New Cape Voucher (7 Günlük)
		if (!RobItem(914006000, 1))
		{
			sErrorCode = -10;
			result << sErrorCode;
			Send(&result);
			return;
		}
	}

	// Does this cape type exist?
	pCape = g_pMain->m_KnightsCapeArray.GetData(sCapeID);
	if (pCape == nullptr)
	{
		sErrorCode = -5;
		result << sErrorCode;
		Send(&result);
		return;
	}

	if (sCapeID >= 0)
	{
		/*if (pCape->Type == 3)
		{
			sErrorCode = -1;
			result << sErrorCode;
			Send(&result);
			return;
		}*/

		// Is our clan allowed to use this cape?
		if ((pCape->byGrade && pKnights->m_byGrade > pCape->byGrade)
			// not sure if this should use another error, need to confirm
			|| pKnights->m_byFlag < pCape->byRanking)
		{
			sErrorCode = -6;
			result << sErrorCode;
			Send(&result);
			return;
		}

		// NOTE: Error code -8 is for nDuration
		// It applies if we do not have the required item ('nDuration', awful name).
		// Since no capes seem to use it, we'll ignore it...

		if (TicketExchange == 0)
		{
			/*if (pCape->Type != 1 && pCape->Type != 2)
			{
				sErrorCode = -1;
				result << sErrorCode;
				Send(&result);
				return;
			}*/

			// Can we even afford this cape?
			if (!hasCoins(pCape->nReqCoins))
			{
				sErrorCode = -7;
				result << sErrorCode;
				Send(&result);
				return;
			}

			nReqCoins = pCape->nReqCoins;
			nReqClanPoints = pCape->nReqClanPoints;
		}
		else if (TicketExchange == 1)
		{
			if (pCape->Type != 3)
			{
				sErrorCode = -1;
				result << sErrorCode;
				Send(&result);
				return;
			}

			if (pCape->Ticket != TicketExchange)
			{
				sErrorCode = -1;
				result << sErrorCode;
				Send(&result);
				return;
			}
		}
	}

	// These are 0 when not used
	if (r != 0 
		|| g != 0 
		|| b != 0)
	{
		// To use paint, the clan needs to be accredited
		if (pKnights->m_byFlag < ClanTypeAccredited5)
		{
			sErrorCode = -1; // need to find the error code for this
			result << sErrorCode;
			Send(&result);
			return;
		}

		bApplyingPaint = true;
		nReqClanPoints += 1000; // does this need tweaking per clan rank?
	}

	if (TicketExchange == 0)
	{
		// If this requires clan points, does our clan have enough?
		if (GetCoins() < nReqCoins)
		{
			// this error may not be correct
			sErrorCode = -7;
			result << sErrorCode;
			Send(&result);
			return;
		}

		if (pKnights->m_nClanPointFund < nReqClanPoints)
		{
			// this error may not be correct
			sErrorCode = -9;
			result << sErrorCode;
			Send(&result);
			return;
		}

		if (nReqCoins > 0)
			GoldLose(nReqCoins);

		if (nReqClanPoints)
		{
			pKnights->m_nClanPointFund -= nReqClanPoints;
			pKnights->UpdateClanFund();
		}
	}

	//new array new table new prosedür.
	if (pCape->Type == 3)
	{
		if (g_pMain->pCswEvent.Started)
		{
			// this error may not be correct
			sErrorCode = -10;
			result << sErrorCode;
			Send(&result);
			return;
		}

		_KNIGHTS_CASTELLAN_CAPE * pCastellanInfo = g_pMain->m_KnightCastellanCapeArray.GetData(GetClanID());
		if (pCastellanInfo != nullptr)
		{
			// Are we changing the cape?
			if (sCapeID >= 0)
			{
				pCastellanInfo->sCape = sCapeID;
				uint64 iexpiration = 0;

				if (TicketExchange == 1)
					iexpiration = uint64(UNIXTIME + 60 * 60 * 24 * 7);
				else
					iexpiration = uint64(UNIXTIME + 60 * 60 * 24 * 30);

				pCastellanInfo->sRemainingTime = iexpiration;
			}
			if (bApplyingPaint)
			{
				pCastellanInfo->sR = r;
				pCastellanInfo->sG = g;
				pCastellanInfo->sB = b;
			}

			pCastellanInfo->sActive = 1;

			if (!pKnights->isInAlliance())
			{
				result << uint16(1) // success
					<< pKnights->GetAllianceID()
					<< pKnights->GetID()
					<< pCastellanInfo->sCape
					<< pCastellanInfo->sR << pCastellanInfo->sG << pCastellanInfo->sB
					<< uint8(0);
			}
			else if (pKnights->isInAlliance())
			{
				result << uint16(1) // success
					<< pKnights->GetAllianceID()
					<< pKnights->GetID()
					<< pMainKnights->GetCapeID()
					<< pCastellanInfo->sR << pCastellanInfo->sG << pCastellanInfo->sB
					<< uint8(0);
			}
			Send(&result);

			// When we implement alliances, this should send to the alliance
			// if the clan is part of one. Also, their capes should be updated.
			if (!pKnights->isInAlliance() 
				|| (pKnights->isInAlliance() 
					&& !pKnights->isAllianceLeader()))
				pKnights->SendUpdate();
			else if (pKnights->isInAlliance() 
				&& pKnights->isAllianceLeader())
				pKnights->SendAllianceUpdate();

			//Superior Cape Bonus
			pKnights->CheckKnightCastellanCapesBuffSystem();

			result.clear();
			result.Initialize(WIZ_CAPE);
			result << uint8(1) << pKnights->GetID() << pCastellanInfo->sCape
				<< pCastellanInfo->sR << pCastellanInfo->sG << pCastellanInfo->sB;
			g_pMain->AddDatabaseRequest(result, this);
			return;
		}
		else
		{
			_KNIGHTS_CASTELLAN_CAPE *pCastellanClanNewSign = new _KNIGHTS_CASTELLAN_CAPE;
			pCastellanClanNewSign->sClanID = GetClanID();
			pCastellanClanNewSign->sCape = sCapeID;
			pCastellanClanNewSign->sR = r;
			pCastellanClanNewSign->sG = g;
			pCastellanClanNewSign->sB = b;
			pCastellanClanNewSign->sActive = 1;

			uint64 iexpiration = 0;

			if (TicketExchange == 1)
				iexpiration = uint64(UNIXTIME + 60 * 60 * 24 * 7);
			else
				iexpiration = uint64(UNIXTIME + 60 * 60 * 24 * 30);

			pCastellanClanNewSign->sRemainingTime = iexpiration;

			if (!g_pMain->m_KnightCastellanCapeArray.PutData(pCastellanClanNewSign->sClanID, pCastellanClanNewSign))
				delete pCastellanClanNewSign;

			_KNIGHTS_CASTELLAN_CAPE * pCastellanInfo2 = g_pMain->m_KnightCastellanCapeArray.GetData(GetClanID());
			if (pCastellanInfo2 != nullptr)
			{
				if (!pKnights->isInAlliance())
				{
					result << uint16(1) // success
						<< pKnights->GetAllianceID()
						<< pKnights->GetID()
						<< pCastellanInfo2->sCape
						<< pCastellanInfo2->sR << pCastellanInfo2->sG << pCastellanInfo2->sB
						<< uint8(0);
				}
				else if (pKnights->isInAlliance())
				{
					result << uint16(1) // success
						<< pKnights->GetAllianceID()
						<< pKnights->GetID()
						<< pMainKnights->GetCapeID()
						<< pCastellanInfo2->sR << pCastellanInfo2->sG << pCastellanInfo2->sB
						<< uint8(0);
				}
				Send(&result);

				// When we implement alliances, this should send to the alliance
				// if the clan is part of one. Also, their capes should be updated.
				if (!pKnights->isInAlliance() 
					|| (pKnights->isInAlliance() && !pKnights->isAllianceLeader()))
					pKnights->SendUpdate();
				else if (pKnights->isInAlliance() && pKnights->isAllianceLeader())
					pKnights->SendAllianceUpdate();

				//Superior Cape Bonus
				pKnights->CheckKnightCastellanCapesBuffSystem();

				result.clear();
				result.Initialize(WIZ_CAPE);
				result << uint8(1) << pKnights->GetID() << pCastellanInfo2->sCape
					<< pCastellanInfo2->sR << pCastellanInfo2->sG << pCastellanInfo2->sB;
				g_pMain->AddDatabaseRequest(result, this);
				return;
			}
		}
	}
	else
	{
		// Are we changing the cape?
		if (sCapeID >= 0)
			pKnights->m_sCape = sCapeID;

		// Are we applying paint?
		if (bApplyingPaint)
		{
			pKnights->m_bCapeR = r;
			pKnights->m_bCapeG = g;
			pKnights->m_bCapeB = b;
		}

		if (!pKnights->isInAlliance())
		{
			result << uint16(1) // success
				<< pKnights->GetAllianceID()
				<< pKnights->GetID()
				<< pKnights->GetCapeID()
				<< pKnights->m_bCapeR << pKnights->m_bCapeG << pKnights->m_bCapeB
				<< uint8(0);
		}
		else if (pKnights->isInAlliance())
		{
			result << uint16(1) // success
				<< pKnights->GetAllianceID()
				<< pKnights->GetID()
				<< pMainKnights->GetCapeID()
				<< pKnights->m_bCapeR << pKnights->m_bCapeG << pKnights->m_bCapeB
				<< uint8(0);
		}
		Send(&result);

		// When we implement alliances, this should send to the alliance
		// if the clan is part of one. Also, their capes should be updated.
		if (!pKnights->isInAlliance() || (pKnights->isInAlliance() && !pKnights->isAllianceLeader()))
			pKnights->SendUpdate();
		else if (pKnights->isInAlliance() && pKnights->isAllianceLeader())
			pKnights->SendAllianceUpdate();

		// Now tell Database Agent to save (we don't particularly care whether it was able to do so or not).
		result.clear();
		result.Initialize(WIZ_CAPE);
		result << uint8(2) << pKnights->GetID() << pKnights->GetCapeID()
			<< pKnights->m_bCapeR << pKnights->m_bCapeG << pKnights->m_bCapeB;
		g_pMain->AddDatabaseRequest(result, this);
	}
	//I'am winner.
}

#pragma region CKnights::SendUpdate()
void CKnights::SendUpdate()
{
	Packet result(WIZ_KNIGHTS_PROCESS, uint8(KNIGHTS_UPDATE));

	if (isInAlliance())
	{
		CKnights * pMainClan = g_pMain->GetClanPtr(GetAllianceID());
		_KNIGHTS_ALLIANCE* pAlliance = g_pMain->GetAlliancePtr(GetAllianceID());
		
		if (pMainClan == nullptr
			|| pAlliance == nullptr)
			return;
		
		_KNIGHTS_CASTELLAN_CAPE * pCastellanInfo = g_pMain->m_KnightCastellanCapeArray.GetData(pMainClan->GetID());
		if (pCastellanInfo == nullptr)
		{
			if (pAlliance->sMainAllianceKnights == GetID())
				result << GetID() << m_byFlag << pMainClan->GetCapeID() << pMainClan->m_bCapeR << pMainClan->m_bCapeG << pMainClan->m_bCapeB << uint8(0);
			else if (pAlliance->sSubAllianceKnights == GetID())
				result << GetID() << m_byFlag << pMainClan->GetCapeID() << m_bCapeR << m_bCapeG << m_bCapeB << uint8(0);
			else if (pAlliance->sMercenaryClan_1 == GetID()	|| pAlliance->sMercenaryClan_2 == GetID())
				result << GetID() << m_byFlag << pMainClan->GetCapeID() << uint32(0); // only the cape will be present
		}
		else 
		{
			if (pAlliance->sMainAllianceKnights == GetID())
				result << GetID() << m_byFlag << pCastellanInfo->sCape << pCastellanInfo->sR << pCastellanInfo->sG << pCastellanInfo->sB << uint8(0);
			else if (pAlliance->sSubAllianceKnights == GetID())
				result << GetID() << m_byFlag << pCastellanInfo->sCape << m_bCapeR << m_bCapeG << m_bCapeB << uint8(0);
			else if (pAlliance->sMercenaryClan_1 == GetID()	|| pAlliance->sMercenaryClan_2 == GetID())
				result << GetID() << m_byFlag << pCastellanInfo->sCape << uint32(0); // only the cape will be present
		}
	}
	else
	{
		_KNIGHTS_CASTELLAN_CAPE * pCastellanInfo = g_pMain->m_KnightCastellanCapeArray.GetData(GetID());
		if (pCastellanInfo == nullptr)
			result << GetID() << m_byFlag << GetCapeID() << m_bCapeR << m_bCapeG << m_bCapeB << uint8(0);
		else
			result << GetID() << m_byFlag << pCastellanInfo->sCape << pCastellanInfo->sR << pCastellanInfo->sG << pCastellanInfo->sB << uint8(0);
	}
	Send(&result);
}
#pragma endregion 

#pragma region CKnights::SendAllianceUpdate()
void CKnights::SendAllianceUpdate()
{
	_KNIGHTS_ALLIANCE * pAlliance = g_pMain->GetAlliancePtr(GetAllianceID());
	CKnights * pKnights1 = g_pMain->GetClanPtr(pAlliance->sMercenaryClan_1),
		*pKnights2 = g_pMain->GetClanPtr(pAlliance->sMercenaryClan_2),
		*pKnights3 = g_pMain->GetClanPtr(pAlliance->sSubAllianceKnights);

	if (pKnights1 != nullptr)
		pKnights1->SendUpdate();

	if (pKnights2 != nullptr)
		pKnights2->SendUpdate();

	if (pKnights3 != nullptr)
		pKnights3->SendUpdate();
}
#pragma endregion 