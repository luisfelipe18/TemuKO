#include "stdafx.h"

void CUser::SetRival(CUser * pRival)
{
	if (pRival == nullptr
		|| hasRival())
		return;

	Packet result(WIZ_PVP, uint8(PVPAssignRival));
	CKnights * pKnights = nullptr;

	result << pRival->GetID()
		<< GetCoins() << GetLoyalty();

	if (pRival->isInClan()
		&& (pKnights = g_pMain->GetClanPtr(pRival->GetClanID())))
		result << pKnights->GetName();
	else
		result << uint16(0); // 0 length clan name;

	result << pRival->GetName();

	m_sRivalID = pRival->GetID();
	m_tRivalExpiryTime = UNIXTIME + RIVALRY_DURATION;

	Send(&result);
}

/**
* @brief	Removes our rivalry state.
*/
void CUser::RemoveRival()
{
	if (!hasRival())
		return;

	// Reset our rival data
	m_tRivalExpiryTime = 0;
	m_sRivalID = -1;

	// Send the packet to let the client know that our rivalry has ended
	Packet result(WIZ_PVP, uint8(PVPRemoveRival));
	Send(&result);
}

/**
* @brief	Updates the player's anger gauge level, setting it to
* 			byAngerGauge.
*
* @param	byAngerGauge	The anger gauge level.
*/
void CUser::UpdateAngerGauge(uint8 byAngerGauge)
{
	Packet result(WIZ_PVP, uint8(byAngerGauge == 0 ? PVPResetHelmet : PVPUpdateHelmet));

	if (byAngerGauge > MAX_ANGER_GAUGE)
		byAngerGauge = MAX_ANGER_GAUGE;

	m_byAngerGauge = byAngerGauge;
	if (byAngerGauge > 0)
		result << uint8(byAngerGauge) << hasFullAngerGauge();

	Send(&result);
}