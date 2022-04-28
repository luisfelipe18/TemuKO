#include "stdafx.h"

using namespace std;

void CUser::SelectWarpList(Packet & pkt)
{
	if (isDead())
		return;

	uint16 npcid, warpid;
	pkt >> npcid >> warpid;

	_WARP_INFO *pWarp = GetMap()->GetWarp(warpid);
	if (pWarp == nullptr
		|| (pWarp->sNation != 0 && pWarp->sNation != GetNation()))
		return;

	C3DMap *pMap = g_pMain->GetZoneByID(pWarp->sZone);
	if (pMap == nullptr)
		return;

	_ZONE_SERVERINFO *pInfo = g_pMain->m_ServerArray.GetData(pMap->m_nServerNo);
	if (pInfo == nullptr)
		return;

	float rx = 0.0f, rz = 0.0f;
	rx = (float)myrand(0, (int)pWarp->fR * 2);
	if (rx < pWarp->fR)
		rx = -rx;

	rz = (float)myrand(0, (int)pWarp->fR * 2);
	if (rz < pWarp->fR)
		rz = -rz;

	if (m_bZone == pWarp->sZone)
	{
		m_bZoneChangeSameZone = true;

		Packet result(WIZ_WARP_LIST, uint8(2));
		result << uint8(1);
		Send(&result);
	}

	ZoneChange(pWarp->sZone, pWarp->fX + rx, pWarp->fZ + rz);

	if (GetZoneID() == pWarp->sZone && pWarp->dwPay > 0 && hasCoins(pWarp->dwPay))
		GoldLose(pWarp->dwPay);
}

void CUser::ServerChangeOk(Packet & pkt)
{
	return;
	C3DMap* pMap = GetMap();
	if (pMap == nullptr)
		return;

	uint16 warpid = pkt.read<uint16>();
	_WARP_INFO* pWarp = pMap->GetWarp(warpid);
	if (pWarp == nullptr)
		return;

	float rx = 0.0f, rz = 0.0f;
	rx = (float)myrand(0, (int)pWarp->fR * 2);
	if (rx < pWarp->fR)
		rx = -rx;

	rz = (float)myrand(0, (int)pWarp->fR * 2);
	if (rz < pWarp->fR)
		rz = -rz;

	ZoneChange(pWarp->sZone, pWarp->fX + rx, pWarp->fZ + rz);
}

bool CUser::GetWarpList(int warp_group)
{
	Packet result(WIZ_WARP_LIST, uint8(1));
	C3DMap* pMap = GetMap();
	set<_WARP_INFO*> warpList;

	if (pMap == nullptr)
		return false;

	pMap->GetWarpList(warp_group, warpList);

	result << uint16(warpList.size());
	foreach(itr, warpList)
	{
		C3DMap *pDstMap = g_pMain->GetZoneByID((*itr)->sZone);
		if (pDstMap == nullptr)
			continue;

		if (g_pMain->OpenArdream
			&& ((*itr)->sZone == ZONE_RONARK_LAND
				|| (*itr)->sZone == ZONE_RONARK_LAND_BASE
				|| (*itr)->sZone == ZONE_PARTY_VS_3))
			continue;

		/*if (g_pMain->OpenCZ // Burayý açýk býrakýrsan Gate'de sadece CZ olur.
			&& (*itr)->sZone == ZONE_ARDREAM
				|| (*itr)->sZone == ZONE_RONARK_LAND_BASE)
			continue;*/

		if (g_pMain->OpenCZ
			&& ((*itr)->sZone == ZONE_PARTY_VS_3
				|| (*itr)->sZone == ZONE_RONARK_LAND_BASE
				|| (*itr)->sZone == ZONE_ARDREAM))
			continue;

		/*if (g_pMain->OpenAllClanWar
			&& ((*itr)->sZone == ZONE_RONARK_LAND
				|| (*itr)->sZone == ZONE_RONARK_LAND_BASE
				|| (*itr)->sZone == ZONE_ARDREAM))
			continue;*/

		if (g_pMain->OpenBaseLand
			&& ((*itr)->sZone == ZONE_RONARK_LAND
				|| (*itr)->sZone == ZONE_PARTY_VS_3
				|| (*itr)->sZone == ZONE_ARDREAM))
			continue;

		if (pDstMap->m_Status == 0)
			continue;

		if (g_pMain->m_byBattleOpen == NATION_BATTLE
			&& ((g_pMain->m_byBattleZoneType != ZONE_ARDREAM
				&& ((*itr)->sZone == ZONE_ARDREAM
					|| (*itr)->sZone == ZONE_RONARK_LAND_BASE
					|| (*itr)->sZone == ZONE_RONARK_LAND))
				|| (g_pMain->m_byBattleZoneType == ZONE_ARDREAM
					&& (*itr)->sZone == ZONE_ARDREAM)))
			continue;

		result << (*itr)->sWarpID
			<< (*itr)->strWarpName << (*itr)->strAnnounce
			<< (*itr)->sZone
			<< pDstMap->m_sMaxUser
			<< uint32((*itr)->dwPay);
	}

	Send(&result);
	return true;
}

#pragma region CUser::BindObjectEvent(_OBJECT_EVENT *pEvent)
bool CUser::BindObjectEvent(_OBJECT_EVENT *pEvent)
{
	if (pEvent->sBelong != 0 && pEvent->sBelong != GetNation())
		return false;

	Packet result(WIZ_OBJECT_EVENT, uint8(pEvent->sType));

	m_sBind = pEvent->sIndex;

	result << uint8(1);
	Send(&result);
	return true;
}
#pragma endregion

bool CUser::LogLeverBuringLog(_OBJECT_EVENT *pEvent, int nid)
{
	if (pEvent == nullptr)
		return false;

	CNpc* pNpc, *pGateNpc;

	if ((pNpc = g_pMain->FindNpcInZone(pEvent->sIndex, GetZoneID())) == nullptr)
		return false;

	if (!g_pMain->isWarOpen()
		|| pEvent->sBelong != GetNation())
		return false;

	CNpcThread* zoneitrThread = g_pMain->m_arNpcThread.GetData(pNpc->GetZoneID());

	if (zoneitrThread == nullptr)
		return false;

	foreach_stlmap(itr, zoneitrThread->m_arNpcArray)
	{
		pGateNpc = itr->second;
		if (pGateNpc == nullptr
			// This isn't a typo, it's actually just a hack.
			// The picture/model ID of most spawns is the same as their prototype ID.
			// When there's more than one spawn prototype (i.e. different sSid), we keep using
			// the same picture/model ID. So we check this instead of the sSid...
			|| pGateNpc->GetProtoID() != pEvent->sControlNpcID)
			continue;

		if (pGateNpc->GetZoneID() != pEvent->sZoneID)
			continue;

		// Open/close the gate.
		pGateNpc->SendGateFlag((pGateNpc->m_byGateOpen == 0 ? 1 : 0));

		if (!pGateNpc->isDead())
			pGateNpc->m_WoodOwnerID = GetID();
	}

	// Move the lever (up/down).
	pNpc->SendGateFlag((pNpc->m_byGateOpen == 0 ? 1 : 0));
	return true;
}

bool CUser::KrowazGateEvent(_OBJECT_EVENT* pEvent, int nid)
{
	_OBJECT_EVENT *pGateEvent;
	CNpc *pGateNpc;
	// Does the lever (object) NPC exist?
	if ((pGateEvent = GetMap()->GetObjectEvent(pEvent->sControlNpcID)) == nullptr
		// Does the corresponding gate (object) NPC exist?
		|| (pGateNpc = g_pMain->FindNpcInZone(pEvent->sControlNpcID, GetZoneID())) == nullptr
		// Is it even a gate?
		|| !pGateNpc->isGate()
		// If the gate's closed (i.e. the lever is down), we can't open it unless the lever isn't nation-specific
		// or we're the correct nation. Seems the other nation cannot close them.
		|| (pGateNpc->isGateOpen()))
		return false;

	if (pGateNpc->GetName() == "Blue Key Gate" && CheckExistItem(ITEM_BLUE_KEY))
	{
		// Open/close the gate.
		pGateNpc->SendGateFlag((pGateNpc->m_byGateOpen == 0 ? 1 : 0));
		RobItem(ITEM_BLUE_KEY);
	}
	else if (pGateNpc->GetName() == "Red Key Gate" && CheckExistItem(ITEM_RED_KEY))
	{
		// Open/close the gate.
		pGateNpc->SendGateFlag((pGateNpc->m_byGateOpen == 0 ? 1 : 0));
		RobItem(ITEM_RED_KEY);
	}
	else if (pGateNpc->GetName() == "Black Key Gate" && CheckExistItem(ITEM_BLACK_KEY))
	{
		// Open/close the gate.
		pGateNpc->SendGateFlag((pGateNpc->m_byGateOpen == 0 ? 1 : 0));
		RobItem(ITEM_BLACK_KEY);
	}
	else if ((pGateNpc->GetName() == "Accomplisher Gate" || pGateNpc->GetName() == "Benshar Gate")
		&& CheckExistItem(ITEM_BLACK_KEY)
		&& CheckExistItem(ITEM_RED_KEY)
		&& CheckExistItem(ITEM_BLUE_KEY))
	{
		// Open/close the gate.
		pGateNpc->SendGateFlag((pGateNpc->m_byGateOpen == 0 ? 1 : 0));
		RobItem(ITEM_RED_KEY);
		RobItem(ITEM_BLUE_KEY);
		RobItem(ITEM_BLACK_KEY);
	}
	else
		return false;
	return true;
}

bool CUser::GateLeverObjectEvent(_OBJECT_EVENT *pEvent, int nid)
{
	_OBJECT_EVENT *pGateEvent;
	CNpc* pNpc, *pGateNpc;
	_KNIGHTS_SIEGE_WARFARE *pKnightSiegewars = g_pMain->GetSiegeMasterKnightsPtr(KARUS);
	CKnights *pKnight = g_pMain->GetClanPtr(GetClanID());

	// Does the lever (object) NPC exist?
	if ((pNpc = g_pMain->GetNpcPtr(nid, GetZoneID())) == nullptr
		// Does the corresponding gate object event exist?
		|| (pGateEvent = GetMap()->GetObjectEvent(pEvent->sControlNpcID)) == nullptr
		// Does the corresponding gate (object) NPC exist?
		|| (pGateNpc = g_pMain->FindNpcInZone(pEvent->sControlNpcID, GetZoneID())) == nullptr
		// Is it even a gate?
		|| !pGateNpc->isGate())
		return false;

	if (GetZoneID() == ZONE_DELOS
		&& isInClan())
	{
		if (pKnight == nullptr
			|| pKnightSiegewars == nullptr)
			return false;

		if (pKnightSiegewars->sMasterKnights == 0)
			return false;
		else if (pKnight->GetID() != pKnightSiegewars->sMasterKnights)
			return false;

		// Move the lever (up/down).
		pNpc->SendGateFlag((pNpc->m_byGateOpen == 0 ? 1 : 0));

		// Open/close the gate.
		pGateNpc->SendGateFlag((pGateNpc->m_byGateOpen == 0 ? 1 : 0));
		return true;
	}
	else if ((g_pMain->m_byBattleOpen == NATION_BATTLE
		&& GetMap()->isWarZone())
		|| GetZoneID() == ZONE_BIFROST)
	{
		// If the gate's closed (i.e. the lever is down), we can't open it unless the lever isn't nation-specific
		// or we're the correct nation. Seems the other nation cannot close them.
		if (GetZoneID() != ZONE_BIFROST
			&& (pNpc->GetNation() != 0 && pGateEvent->sBelong != GetNation()))
			return false;

		// Move the lever (up/down).
		pNpc->SendGateFlag((pNpc->m_byGateOpen == 0 ? 1 : 0));

		// Open/close the gate.
		pGateNpc->SendGateFlag((pGateNpc->m_byGateOpen == 0 ? 1 : 0));
		return true;
	}
	else
		return false;
}

/***
* Not sure what this is used for, so keeping logic the same just in case.
***/
bool CUser::FlagObjectEvent(_OBJECT_EVENT *pEvent, int nid)
{
	_OBJECT_EVENT *pFlagEvent;
	CNpc *pNpc, *pFlagNpc;

	// Does the flag object NPC exist?
	if ((pNpc = g_pMain->GetNpcPtr(nid, GetZoneID())) == nullptr
		// Does the corresponding flag event exist?
		|| (pFlagEvent = GetMap()->GetObjectEvent(pEvent->sControlNpcID)) == nullptr
		// Does the corresponding flag object NPC exist?
		|| (pFlagNpc = g_pMain->FindNpcInZone(pEvent->sControlNpcID, GetZoneID())) == nullptr
		// Is this marked a gate? (i.e. can control)
		|| !pFlagNpc->isGate()
		// Is the war over or the gate closed?
		|| g_pMain->m_bVictory > 0 || pNpc->isGateClosed())
		return false;

	// Reset objects
	pNpc->SendGateFlag(0);
	pFlagNpc->SendGateFlag(0);

	// Add flag score (not sure what this is, is this even used anymore?)
	if (GetNation() == KARUS)
		g_pMain->m_bKarusFlag++;
	else
		g_pMain->m_bElmoradFlag++;

	// Did one of the teams win?
	g_pMain->BattleZoneVictoryCheck();
	return true;
}

bool CUser::WarpListObjectEvent(_OBJECT_EVENT *pEvent)
{
	// If the warp gate belongs to a nation, which isn't us...
	if (pEvent->sBelong != 0 && pEvent->sBelong != GetNation()
		// or we're in the opposing nation's zone...
		|| (GetZoneID() != GetNation() && GetZoneID() <= ELMORAD)
		// or we're unable to retrieve the warp list...
		|| !GetWarpList(pEvent->sControlNpcID))
		return false;

	return true;
}

void CUser::ObjectEvent(Packet & pkt)
{
	if (isDead())
		return;

	bool bSuccess = false;
	uint16 objectindex, nid;
	pkt >> objectindex >> nid;

	_OBJECT_EVENT * pEvent = GetMap()->GetObjectEvent(objectindex);
	if (pEvent != nullptr
		&& isInRange(pEvent->fPosX, pEvent->fPosZ, MAX_OBJECT_RANGE))
	{
		switch (pEvent->sType)
		{
		case OBJECT_GATE:
		case OBJECT_BIND:
		case OBJECT_REMOVE_BIND:
			bSuccess = BindObjectEvent(pEvent);
			break;

		case OBJECT_WOOD:
			bSuccess = LogLeverBuringLog(pEvent, nid);
			break;

		case OBJECT_KROWASGATE:
			bSuccess = KrowazGateEvent(pEvent, nid);
			break;

		case OBJECT_WOOD_LEVER:
		case OBJECT_GATE_LEVER:
			bSuccess = GateLeverObjectEvent(pEvent, nid);
			break;

		case OBJECT_FLAG_LEVER:
			bSuccess = FlagObjectEvent(pEvent, nid);
			break;

		case OBJECT_WARP_GATE:
			bSuccess = WarpListObjectEvent(pEvent);
			if (bSuccess)
				return;
			break;

		case OBJECT_ANVIL:
			SendAnvilRequest(nid);
			return;
		default:
			printf("Object unhandled packets %d \n", pEvent->sType);
			TRACE("Object unhandled packets %d \n", pEvent->sType);
			break;
		}
	}

	if (!bSuccess)
	{
		Packet result(WIZ_OBJECT_EVENT, uint8(pEvent == nullptr ? 0 : pEvent->sType));
		result << uint8(0);
		Send(&result);
	}
}