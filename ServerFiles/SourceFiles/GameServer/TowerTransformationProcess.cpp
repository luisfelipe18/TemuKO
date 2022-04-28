#include "stdafx.h"
#include "MagicInstance.h"

void CUser::TowerExitsFunciton(bool Dead)
{
	if (!isTowerOwner())
		return;

	CNpc* pNpc = g_pMain->GetNpcPtr(GetTowerID(), GetZoneID());
	if (pNpc == nullptr
		|| pNpc->GetType() != NPC_KARUS_WARDER2)
		return;

	pNpc->StateChange(NPC_SHOW);
	m_TowerOwnerID = -1;

	if (Dead)
	{
		Packet result(WIZ_MOVING_TOWER);
		result << uint8(17) << uint16(1);
		Send(&result);
	}
}

#pragma region CUser::HandleTowerPackets(Packet & pkt)
void CUser::HandleTowerPackets(Packet & pkt)
{
	uint8 command = pkt.read<uint8>();
	Packet result(WIZ_MOVING_TOWER);
	switch (command)
	{
	case 1:
	{
		CUser * pTUser = g_pMain->GetUserPtr(GetTargetID());

		if (pTUser == nullptr)
			return;

		Warp(uint16(pTUser->GetX() * 10), uint16(pTUser->GetZ() * 10));

		result << (uint8)command << (uint16)command;
		Send(&result);
	}
	break;
	case 2:
	{
		uint16 x, z;
		pkt >> x >> z;

		Warp(x, z);

		result << (uint8)command << uint16(1);
		Send(&result);
	}
	break;
	case 16:
	{
		uint16 x;
		pkt >> x;

		if (GetTowerID() != -1)
			return;

		CNpc* pNpc = g_pMain->GetNpcPtr(x, GetZoneID());
		if (pNpc != nullptr)
		{
			pNpc->StateChange(NPC_HIDE);
			m_TowerOwnerID = pNpc->GetID();

			Warp((uint16)(pNpc->GetX() * 10), (uint16)(pNpc->GetZ() * 10));
			StateChangeServerDirect(3, 450018);

			result << (uint8)command << uint8(1) << GetID() << pNpc->GetID()
				<< (uint16)pNpc->GetSPosX() << (uint16)pNpc->GetSPosZ() << (uint16)pNpc->GetSPosZ();
			Send(&result);
		}
	}
	break;
	case 17:
	{
		CNpc* pNpc = g_pMain->GetNpcPtr(GetTowerID(), GetZoneID());
		if (pNpc != nullptr)
		{
			if (pNpc->GetType() == 191)
			{
				pNpc->StateChange(NPC_SHOW);
				StateChangeServerDirect(3, ABNORMAL_NORMAL);
				m_TowerOwnerID = -1;

				result << (uint8)command << uint16(1);
				Send(&result);
			}
		}
		else
		{
			result << (uint8)command << uint16(1);
			Send(&result);
		}
	}
	break;
	default:
		printf("Moving Tower unhandled packets %d \n", command);
		TRACE("Moving Tower unhandled packets %d \n", command);
		break;
	}
}
#pragma endregion

/**
* @brief	Changes a player's fame.
*
* @param	bFame	The fame.
*/
void CUser::ChangeFame(uint8 bFame)
{
	Packet result(WIZ_AUTHORITY_CHANGE, uint8(COMMAND_AUTHORITY));

	m_bFame = bFame;
	result << GetSocketID() << GetFame();
	SendToRegion(&result);
}