#include "stdafx.h"

void CUser::SpawnEventSystem(uint16 sSid, uint8 bIsmonster, uint8 byZone, float fX, float fY, float fZ)
{
	if (bIsmonster >= 0 && bIsmonster <= 1)
		g_pMain->SpawnEventNpc(sSid, bIsmonster == 0 ? true : false, byZone, fX, fY, fZ, 1, 0, 1 * HOUR);
	else
		return;
}

void CUser::KillNpcEvent()
{
	CNpc *pNpc = g_pMain->GetNpcPtr(m_sEventNid, GetZoneID());

	if (pNpc != nullptr)
		pNpc->Dead();
	else
		return;

	m_sEventNid = -1;
}

void CUser::ChangePosition()
{
	CNpc *pNpc = g_pMain->GetNpcPtr(GetTargetID(), GetZoneID());

	if (pNpc == nullptr)
		return;

	bool m_byTrapNumbers = (pNpc->m_byTrapNumber >= 1 && pNpc->m_byTrapNumber <= 4)
		&& (pNpc->m_bySpecialType == NpcSpecialTypeCycleSpawn);

	if (!m_byTrapNumbers)
		return;

	pNpc->SendInOut(INOUT_OUT, pNpc->GetX(), pNpc->GetZ(), pNpc->GetY());
}

void CUser::NpcEventSystem(uint32 m_iSellingGroup)
{
	if (!isInGame())
		return;

	Packet result;

	CNpc *pNpc = g_pMain->GetNpcPtr(GetTargetID(), GetZoneID());

	if (pNpc != nullptr)
		pNpc->m_iSellingGroup = m_iSellingGroup;
	else
		return;

	result.SetOpcode(WIZ_TRADE_NPC);
	result << m_iSellingGroup;
	Send(&result);
}