#include "stdafx.h"
#include "../shared/KOSocketMgr.h"
using namespace std;

/*
1. The invitation to the party has been declined.
2. You cannot form a party because of the Level difference.
3. You cannot form a party with a user from the other nation.
4. A clan party cannot invite users from a different clan.
5. You cannot form a party with a user in a different zone.
6. You cannot form a party with a user that's not online.
7. You cannot form a party with this user.
8. Not a same clan
9. You cannot form a party in this region.
10. You cannot invite this player to a party. Invitee does not have required item
*/

#pragma region void CUser::PartySystemProcess(Packet & pkt)
void CUser::PartySystemProcess(Packet& pkt)
{
	uint8 opcode;
	pkt >> opcode;
	switch (opcode)
	{
	case PARTY_CREATE:
		PartyCreateRequest(pkt);
		break;
	case PARTY_INSERT:
		PartyInvitationRequest(pkt);
		break;
	case PARTY_PERMIT:
		PartyInsertOrCancel(pkt);
		break;
	case PARTY_PROMOTE:
		PartyLeaderPromote(pkt.read<uint16>());
		break;
	case PARTY_REMOVE:
		PartyNemberRemove(pkt.read<uint16>());
		break;
	case PARTY_DELETE:
		PartyisDelete();
		break;
	case PARTY_TARGET_NUMBER:
		PartyTargetNumber(pkt);
		break;
	case PARTY_ALERT:
		PartyAlert(pkt);
		break;
	case PARTY_COMMAND_PROMATE:
		PartyCommand(pkt);
		break;
	default:
		printf("Party unhandled packets %x \n", opcode);
		TRACE("Party unhandled packets %x \n", opcode);
		break;
	}
}
#pragma endregion

#pragma region void CUser::PartyCreateRequest(Packet & pkt)
void CUser::PartyCreateRequest(Packet& pkt)
{
	string strUserID = "";
	CUser* pUser = nullptr;
	uint8 PartyType = 0;

	pkt >> strUserID >> PartyType;

	if (strUserID.empty() || strUserID.size() > MAX_ID_SIZE)
		return;

	pUser = g_pMain->GetUserPtr(strUserID, TYPE_CHARACTER);
	if (pUser == nullptr)
		return;

	PartyCreateCheck(pUser->GetSocketID(), PartyType);
}
#pragma endregion

#pragma region void CUser::PartyCreateCheck(uint16 UserGetSocketID, uint8 PartyType
void CUser::PartyCreateCheck(uint16 UserGetSocketID, uint8 PartyType)
{
	Packet party(WIZ_PARTY);
	CUser* pUser = g_pMain->GetUserPtr(UserGetSocketID);
	_PARTY_GROUP* pParty = nullptr;

	if (pUser == nullptr
		|| pUser == this
		|| pUser->isInParty()
		|| pUser->isInApprovedParty())
	{
		party << uint8(PARTY_INSERT) << int16(-7);
		Send(&party);
		return;
	}

	if (GetNation() != pUser->GetNation()
		&& !isPartnerPartyZone())
	{
		party << uint8(PARTY_INSERT) << int16(-3);
		Send(&party);
		return;
	}

	if (GetZoneID() != pUser->GetZoneID())
	{
		party << uint8(PARTY_INSERT) << int16(-5);
		Send(&party);
		return;
	}

	if (GetZoneID() == ZONE_CHAOS_DUNGEON
		|| GetZoneID() == ZONE_PRISON
		|| GetZoneID() == ZONE_DUNGEON_DEFENCE)
	{
		party << uint8(PARTY_INSERT) << int16(-9);
		Send(&party);
		return;
	}

	if (!pUser->isInGame())
	{
		party << uint8(PARTY_INSERT) << int16(-6);
		Send(&party);
		return;
	}

	if (PartyType == 2)
	{
		if (!pUser->CheckExistItem(914057000, 1))
		{
			party << uint8(PARTY_INSERT) << int16(-10);
			Send(&party);
			return;
		}
	}

	if (GetZoneID() != ZONE_BORDER_DEFENSE_WAR && GetZoneID() != ZONE_JURAID_MOUNTAIN)
	{
		// Players taking the beginner/"chicken" quest can party anyone.
		if (!m_bIsChicken)
		{
			// Players in the same clan can party together, no matter the level difference.
			if (!isInClan()
				|| GetClanID() != pUser->GetClanID())
			{
				if (!((pUser->GetLevel() <= (int)(GetLevel() * 1.5) && pUser->GetLevel() >= (int)(GetLevel() * 2 / 3))
					|| (pUser->GetLevel() <= (GetLevel() + 8) && pUser->GetLevel() >= ((int)(GetLevel()) - 8))))
				{
					party << uint8(PARTY_INSERT) << int16(-2);
					Send(&party);
					return;
				}
			}
		}
	}

	if (isInParty() || isInApprovedParty())
	{
		party << uint8(PARTY_INSERT) << int16(-7);
		Send(&party);
		return;
	}

	pParty = g_pMain->CreateParty(this);
	if (pParty == nullptr)
	{
		party << uint8(PARTY_INSERT) << int16(-7);
		Send(&party);
		return;
	}

	m_sUserPartyType = PartyType;
	pUser->m_sUserPartyType = PartyType;
	m_bPartyLeader = true;
	StateChangeServerDirect(6, 1); // give party leader the 'P' symbol
	pUser->m_sPartyIndex = m_sPartyIndex;
	pUser->m_bInParty = true;
	party << uint8(PARTY_PERMIT) << GetSocketID() << GetName();
	pUser->Send(&party);
}
#pragma endregion

#pragma region void CUser::PartyInvitationRequest(Packet & pkt)
void CUser::PartyInvitationRequest(Packet& pkt)
{
	string strUserID = "";
	CUser* pUser = nullptr;
	uint8 PartyType = 0;

	pkt >> strUserID >> PartyType;

	if (strUserID.empty() || strUserID.size() > MAX_ID_SIZE)
		return;

	pUser = g_pMain->GetUserPtr(strUserID, TYPE_CHARACTER);
	if (pUser == nullptr)
		return;

	PartyInvitationCheck(pUser->GetSocketID(), PartyType);
}
#pragma endregion

#pragma region void CUser::PartyInvitationCheck(uint16 UserGetSocketID, uint8 PartyType)
void CUser::PartyInvitationCheck(uint16 UserGetSocketID, uint8 PartyType)
{
	Packet party(WIZ_PARTY);
	CUser* pUser = g_pMain->GetUserPtr(UserGetSocketID);
	_PARTY_GROUP* pParty = nullptr;
	int16 PartyMemberCount = 0;

	if (pUser == nullptr
		|| pUser == this
		|| pUser->isInParty()
		|| pUser->isInApprovedParty())
	{
		party << uint8(PARTY_INSERT) << int16(-7);
		Send(&party);
		return;
	}

	if (GetNation() != pUser->GetNation()
		&& !isPartnerPartyZone())
	{
		party << uint8(PARTY_INSERT) << int16(-3);
		Send(&party);
		return;
	}

	if (GetZoneID() != pUser->GetZoneID())
	{
		party << uint8(PARTY_INSERT) << int16(-5);
		Send(&party);
		return;
	}

	if (GetZoneID() == ZONE_CHAOS_DUNGEON
		|| GetZoneID() == ZONE_PRISON
		|| GetZoneID() == ZONE_DUNGEON_DEFENCE)
	{
		party << uint8(PARTY_INSERT) << int16(-9);
		Send(&party);
		return;
	}

	if (!pUser->isInGame())
	{
		party << uint8(PARTY_INSERT) << int16(-6);
		Send(&party);
		return;
	}

	if (PartyType == 2)
	{
		if (!pUser->CheckExistItem(914057000, 1))
		{
			party << uint8(PARTY_INSERT) << int16(-10);
			Send(&party);
			return;
		}
	}

	if (GetZoneID() != ZONE_BORDER_DEFENSE_WAR && GetZoneID() != ZONE_JURAID_MOUNTAIN)
	{
		// Players taking the beginner/"chicken" quest can party anyone.
		if (!m_bIsChicken)
		{
			// Players in the same clan can party together, no matter the level difference.
			if (!isInClan()
				|| GetClanID() != pUser->GetClanID())
			{
				if (!((pUser->GetLevel() <= (int)(GetLevel() * 1.5) && pUser->GetLevel() >= (int)(GetLevel() * 2 / 3))
					|| (pUser->GetLevel() <= (GetLevel() + 8) && pUser->GetLevel() >= ((int)(GetLevel()) - 8))))
				{
					party << uint8(PARTY_INSERT) << int16(-2);
					Send(&party);
					return;
				}
			}
		}
	}

	pParty = g_pMain->GetPartyPtr(GetPartyID());
	if (pParty == nullptr)
	{
		party << uint8(PARTY_INSERT) << int16(-1);
		Send(&party);
		return;
	}

	for (PartyMemberCount = 0; PartyMemberCount < MAX_PARTY_USERS; PartyMemberCount++)
	{
		if (pParty->uid[PartyMemberCount] < 0)
			break;
	}

	if (PartyMemberCount == MAX_PARTY_USERS)
	{
		party << uint8(PARTY_INSERT) << int16(-1);
		Send(&party);
		return;
	}

	pUser->m_sPartyIndex = m_sPartyIndex;
	pUser->m_bInParty = true;
	pUser->m_sUserPartyType = PartyType;
	party << uint8(PARTY_PERMIT) << GetSocketID() << GetName();
	pUser->Send(&party);
}
#pragma endregion

#pragma region void CUser::PartyInsertOrCancel(Packet & pkt)
void CUser::PartyInsertOrCancel(Packet& pkt)
{
	uint8 OkorCancel;
	pkt >> OkorCancel;
	switch (OkorCancel)
	{
	case 0:
		DoNotAcceptJoiningTheParty();
		break;
	case 1:
		AgreeToJoinTheParty();
		break;
	default:
		printf("Party System Ok or Cansel unhandled opcode %d \n", OkorCancel);
		TRACE("Party System Ok or Cansel unhandled opcode %d \n", OkorCancel);
		break;
	}
}
#pragma endregion

#pragma region void CUser::AgreeToJoinTheParty()
void CUser::AgreeToJoinTheParty()
{
	Packet result(WIZ_PARTY);
	_PARTY_GROUP* pParty = nullptr;
	uint8 byIndex = 0xFF;
	int leader_id = -1;

	if (!isInApprovedParty())
		return;

	pParty = g_pMain->GetPartyPtr(GetPartyID());
	if (pParty == nullptr)
	{
		m_bInParty = false;
		m_bInEnterParty = false;
		m_sUserPartyType = 0;
		m_sPartyIndex = -1;
		return;
	}

	CUser* pLeader = g_pMain->GetUserPtr(pParty->uid[0]);
	if (pLeader == nullptr || !pLeader->isInGame())
		return;

	if (pLeader->GetZoneID() != GetZoneID()
		|| GetPartyMemberAmount(pParty) == MAX_PARTY_USERS)
	{
		DoNotAcceptJoiningTheParty();
		return;
	}

	// make sure user isn't already in the array...
	// kind of slow, but it works for the moment
	foreach_array(i, pParty->uid)
	{
		if (pParty->uid[i] == GetSocketID())
		{
			m_bInParty = false;
			m_bInEnterParty = false;
			m_sUserPartyType = 0;
			m_sPartyIndex = -1;
			pParty->uid[i] = -1;
			return;
		}
	}

	int PartyNemberCount = 0;
	for (int i = 0; i < MAX_PARTY_USERS; i++)
	{
		if (pParty->uid[i] >= 0)
			PartyNemberCount++;
	}

	if (PartyNemberCount == 1)
		pLeader->m_bInEnterParty = pLeader->m_bInParty = true;

	// Send the player who just joined the existing party list
	for (int i = 0; i < MAX_PARTY_USERS; i++)
	{
		// No player set?
		if (pParty->uid[i] < 0)
		{
			// If we're not in the list yet, add us.
			if (byIndex == 0xFF)
			{
				pParty->uid[i] = GetSocketID();
				byIndex = i;
			}
			continue;
		}

		// For everyone else, 
		CUser* pUser = g_pMain->GetUserPtr(pParty->uid[i]);
		if (pUser == nullptr || !pUser->isInGame())
			continue;

		result.clear();
		result << uint8(PARTY_INSERT) << pParty->uid[i]
			<< uint8(1) // success
			<< pUser->GetName()
			<< pUser->m_MaxHp << pUser->m_sHp
			<< pUser->GetLevel() << pUser->m_sClass
			<< pUser->m_MaxMp << pUser->m_sMp
			<< pUser->GetNation()
			<< pUser->m_sUserPartyType;
		Send(&result);
	}

	if (pLeader->m_bNeedParty == 2 || m_bNeedParty == 2)
	{
		if (pLeader->m_bNeedParty == 2)
		{
			g_pMain->m_SeekingPartyArrayLock.lock();
			// You don't need anymore seek
			foreach(itr, g_pMain->m_SeekingPartyArray)
			{
				if ((*itr) == nullptr)
					continue;

				if ((*itr)->m_sGetID == pLeader->GetID())
				{
					g_pMain->m_SeekingPartyArray.erase(itr);
					break;
				}
			}
			g_pMain->m_SeekingPartyArrayLock.unlock();
		}
		if (m_bNeedParty == 2)
		{
			g_pMain->m_SeekingPartyArrayLock.lock();
			// You don't need anymore seek
			foreach(itr, g_pMain->m_SeekingPartyArray)
			{
				if ((*itr) == nullptr)
					continue;

				if ((*itr)->m_sGetID == GetID())
				{
					g_pMain->m_SeekingPartyArray.erase(itr);
					break;
				}
			}
			g_pMain->m_SeekingPartyArrayLock.unlock();
		}
	}

	m_bInEnterParty = m_bInParty = true;
	pLeader->m_bInEnterParty = pLeader->m_bInParty = true;

	if (pLeader->m_bNeedParty == 2 && pLeader->isInParty())
		pLeader->StateChangeServerDirect(2, 1);

	if (m_bNeedParty == 2 && isInParty())
		StateChangeServerDirect(2, 1);

	result.clear();
	result << uint8(PARTY_INSERT) << GetSocketID()
		<< uint8(1) // success
		<< GetName()
		<< m_MaxHp << m_sHp
		<< GetLevel() << GetClass()
		<< m_MaxMp << m_sMp
		<< GetNation()
		<< m_sUserPartyType;
	g_pMain->Send_PartyMember(GetPartyID(), &result);
}
#pragma endregion

#pragma region void CUser::DoNotAcceptJoiningTheParty()
void CUser::DoNotAcceptJoiningTheParty()
{
	int LeaderGetID = -1, PartyNemberCount = 0;
	if (!isInApprovedParty())
		return;

	_PARTY_GROUP* pParty = g_pMain->GetPartyPtr(GetPartyID());
	if (pParty == nullptr)
		return;

	LeaderGetID = pParty->uid[0];
	CUser* pLeader = g_pMain->GetUserPtr(LeaderGetID);
	if (pLeader == nullptr)
		return;

	m_sUserPartyType = 0;
	m_bInParty = false;
	m_bInEnterParty = false;
	m_sPartyIndex = -1;

	for (int i = 0; i < MAX_PARTY_USERS; i++)
	{
		if (pParty->uid[i] >= 0)
			PartyNemberCount++;
	}

	if (PartyNemberCount == 1)
		pLeader->PartyisDelete();

	Packet result(WIZ_PARTY, uint8(PARTY_INSERT));
	result << int16(-1);
	pLeader->Send(&result);
}
#pragma endregion

#pragma region void CUser::PartyisDelete()
void CUser::PartyisDelete()
{
	if (!isInApprovedParty())
		return;

	_PARTY_GROUP* pParty = g_pMain->GetPartyPtr(GetPartyID());
	if (pParty == nullptr)
	{
		m_sUserPartyType = 0;
		m_bInEnterParty = false;
		m_bInParty = false;
		m_sPartyIndex = -1;
		return;
	}

	for (int i = 0; i < MAX_PARTY_USERS; i++)
	{
		CUser* pUser = g_pMain->GetUserPtr(pParty->uid[i]);
		if (pUser != nullptr)
		{
			pUser->m_sUserPartyType = 0;
			pUser->m_bInEnterParty = false;
			pUser->m_bInParty = false;
			pUser->m_sPartyIndex = -1;
		}
	}

	if (m_bNeedParty == 3)
	{
		// You don't need anymore seek
		foreach(itr, g_pMain->m_SeekingPartyArray)
		{
			if ((*itr) == nullptr)
				continue;

			if ((*itr)->m_sGetID == GetID())
			{
				g_pMain->m_SeekingPartyArray.erase(itr);
				break;
			}
		}
		StateChangeServerDirect(2, 1);
	}

	Packet result(WIZ_PARTY, uint8(PARTY_DELETE));
	g_pMain->Send_PartyMember(pParty->wIndex, &result);

	m_bPartyLeader = false;
	StateChangeServerDirect(6, 0); // remove 'P' symbol from party leader
	g_pMain->DeleteParty(pParty->wIndex);
}
#pragma endregion

#pragma region void CUser::PartyLeaderPromote(uint16 GetLeaderID)
void CUser::PartyLeaderPromote(uint16 GetLeaderID)
{
	// Only the existing party leader can promote a new party leader.
	if (!isPartyLeader())
		return;

	// Ensure this user exists and that they're in our party already.
	CUser* pUser = g_pMain->GetUserPtr(GetLeaderID);
	if (pUser == nullptr
		|| pUser->GetPartyID() != GetPartyID())
		return;

	// Ensure this party exists.
	_PARTY_GROUP* pParty = g_pMain->GetPartyPtr(GetPartyID());
	if (pParty == nullptr)
		return;

	// Find the position of the user to promote in the array.
	uint8 pos = 0;
	for (uint8 i = 1; i < MAX_PARTY_USERS; i++)
	{
		if (pParty->uid[i] != GetLeaderID)
			continue;

		pos = i;
		break;
	}

	// Didn't find it? (leader's always position 0, no need to check there)
	if (pos == 0)
		return;

	// Swap the IDs around, so they have the leadership position.
	std::swap(pParty->uid[0], pParty->uid[pos]);

	// Swap the seeking party & leader flags
	std::swap(m_bNeedParty, pUser->m_bNeedParty);
	std::swap(m_bPartyLeader, pUser->m_bPartyLeader);

	// Remove our leadership state from the client
	StateChangeServerDirect(6, 0); // remove 'P' symbol from old party leader	
	StateChangeServerDirect(2, m_bNeedParty); // seeking a party

											  // Make them leader.
	pUser->StateChangeServerDirect(6, 1); // assign 'P' symbol to new party leader
	pUser->StateChangeServerDirect(2, pUser->m_bNeedParty); // seeking a party

	Packet result(WIZ_PARTY, uint8(PARTY_INSERT));
	result << pUser->GetSocketID()
		<< uint8(100) // reset position to leader
		<< pUser->GetName()
		<< pUser->m_MaxHp << pUser->m_sHp
		<< pUser->GetLevel() << pUser->GetClass()
		<< pUser->m_MaxMp << pUser->m_sMp
		<< pUser->GetNation();
	g_pMain->Send_PartyMember(GetPartyID(), &result);
}
#pragma endregion

#pragma region void CUser::PartyNemberRemove(uint16 UserID)
void CUser::PartyNemberRemove(uint16 UserID)
{
	if (!isInApprovedParty())
		return;

	CUser* pUser = g_pMain->GetUserPtr(UserID);
	if (pUser == nullptr)
		return;

	_PARTY_GROUP* pParty = g_pMain->GetPartyPtr(GetPartyID());
	if (pParty == nullptr)
	{
		m_bInParty = pUser->m_bInParty = false;
		m_bInEnterParty = pUser->m_bInEnterParty = false;
		m_sPartyIndex = pUser->m_sPartyIndex = -1;
		m_sUserPartyType = pUser->m_sUserPartyType = 0;
		return;
	}

	if (pUser->GetSocketID() != GetSocketID())
	{
		if (pParty->uid[0] != GetSocketID())
			return;
	}
	else
	{
		if (pParty->uid[0] == pUser->GetSocketID())
		{
			PartyisDelete();
			return;
		}
	}

	int count = 0, memberPos = -1;
	for (int i = 0; i < MAX_PARTY_USERS; i++)
	{
		if (pParty->uid[i] < 0)
			continue;

		else if (pParty->uid[i] == pUser->GetSocketID())
		{
			memberPos = i;
			continue;
		}
		count++;
	}

	if (count == 1)
	{
		CUser* pPartyLeader = g_pMain->GetUserPtr(pParty->uid[0]);
		if (pPartyLeader == nullptr)
			return;
		else
			pPartyLeader->PartyisDelete();
		return;
	}

	Packet result(WIZ_PARTY, uint8(PARTY_REMOVE));
	result << pUser->GetSocketID();
	g_pMain->Send_PartyMember(m_sPartyIndex, &result);

	if (memberPos >= 0)
	{
		pUser->m_sUserPartyType = 0;
		pUser->m_bInParty = false;
		pUser->m_bInEnterParty = false;
		pUser->m_sPartyIndex = pParty->uid[memberPos] = -1;
	}
}
#pragma endregion

#pragma region void CUser::PartyAlert(Packet & pkt)
void CUser::PartyAlert(Packet& pkt)
{
	uint8 SubOpCode = 0;
	uint32 EffectID = 0;

	pkt >> SubOpCode >> EffectID;

	if (!isInParty() 
		&& !isPartyLeader())
		return;

	_PARTY_GROUP* pParty = g_pMain->GetPartyPtr(GetPartyID());
	if (pParty == nullptr)
		return;

	Packet result(WIZ_PARTY);
	result << uint8(PARTY_ALERT) << SubOpCode;

	for (uint8 i = 0; i < MAX_PARTY_USERS; i++)
	{
		auto* PartyUser = g_pMain->GetUserPtr(pParty->uid[i]);
		if (PartyUser == nullptr
			|| !PartyUser->isInGame())
			continue;

		PartyUser->Send(&result);
	}
}
#pragma endregion

#pragma region void CUser::PartyCommand(Packet & pkt)
void CUser::PartyCommand(Packet& pkt)
{
	uint16 NenberID = -1;
	pkt >> NenberID;

	if (!isInParty() 
		&& !isPartyLeader())
		return;

	CUser* pUser = g_pMain->GetUserPtr(uint16(NenberID));
	if (pUser == nullptr
		|| pUser->GetPartyID() != GetPartyID())
		return;

	Packet result(WIZ_PARTY);
	result << uint8(PARTY_COMMAND_PROMATE) << pUser->GetSocketID() << pUser->GetName();
	g_pMain->Send_PartyMember(GetPartyID(), &result);
}
#pragma endregion

#pragma region void CUser::PartyTargetNumber(Packet & pkt)
void CUser::PartyTargetNumber(Packet& pkt)
{
	int8_t Succes = -1;
	int16_t TargetID = -1;
	uint32_t EffectID = 0;

	pkt >> TargetID >> EffectID >> Succes;

	Unit* pTarget = g_pMain->GetUnitPtr(TargetID, GetZoneID());
	if (pTarget == nullptr 
		|| pTarget->isDead() 
		|| (pTarget->isPlayer() && !TO_USER(pTarget)->isInGame()))
		return;

	if (!isInParty() && !isPartyLeader())
		return;

	_PARTY_GROUP* pParty = g_pMain->GetPartyPtr(GetPartyID());
	if (pParty == nullptr)
		return;

	Packet result(WIZ_PARTY);
	result << uint8(PARTY_TARGET_NUMBER) << pTarget->GetID() << Succes;

	for (int32_t i = 0; i < MAX_PARTY_USERS; i++)
	{
		auto* PartyUser = g_pMain->GetUserPtr(pParty->uid[i]);
		if (PartyUser == nullptr
			|| !PartyUser->isInGame())
			continue;

		PartyUser->Send(&result);
	}
}
#pragma endregion

#pragma region CGameServerDlg::DeleteParty(uint16 sIndex)
void CGameServerDlg::DeleteParty(uint16 sIndex)
{
	m_PartyArray.DeleteData(sIndex);
}
#pragma endregion

void CUser::PartyBBS(Packet& pkt)
{
	uint8 Type, opcode;
	pkt >> Type >> opcode;
	switch (Type)
	{
	case 0:
	{
		switch (opcode)
		{
		case PARTY_BBS_REGISTER:
			PartyBBSRegister(pkt);
			break;
		case PARTY_BBS_DELETE:
			PartyBBSDelete(pkt);
			break;
		case PARTY_BBS_NEEDED:
			PartyBBSNeeded(pkt);
			break;
		case PARTY_BBS_WANTED:
			PartyBBSWanted(pkt);
			break;
		case PARTY_BBS_PARTY_CHANGE:
			PartyBBSPartyChange(pkt);
			break;
		case PARTY_BBS_PARTY_DELETE:
			PartyBBSPartyDelete(pkt);
			break;
		case PARTY_BBS_LIST:
			SendPartyBBSNeeded(pkt);
			break;
		default:
			printf("Party Seeking System Type 0 unhandled Opcode %d \n", opcode);
			break;
		}
	}break;
	case 2:
	{
		switch (opcode)
		{
		case PARTY_BBS_REGISTER:
			DungeonDefencePartyBBSRegister(pkt);
			break;
		case PARTY_BBS_DELETE:
			DungeonDefencePartyBBSDelete(pkt);
			break;
		case PARTY_BBS_NEEDED:
			DungeonDefencePartyBBSNeeded(pkt);
			break;
		case PARTY_BBS_WANTED:
			DungeonDefencePartyBBSWanted(pkt);
			break;
		case PARTY_BBS_PARTY_CHANGE:
			DungeonDefencePartyBBSPartyChange(pkt);
			break;
		case PARTY_BBS_PARTY_DELETE:
			DungeonDefencePartyBBSPartyDelete(pkt);
			break;
		case PARTY_BBS_LIST:
			DungeonDefenceSendPartyBBSNeeded(pkt);
			break;
		default:
			printf("Party Board unhandled opcode %d \n", opcode);
			break;
		}
	}
	break;
	default:
		printf("Party Seeking System unhandled Type %d \n", Type);
		break;
	}
}

void CUser::PartyBBSPartyDelete(Packet& pkt)
{
	if (m_bNeedParty == 1
		|| !isPartyLeader())
		return;

	// You don't need anymore 
	Guard lock(g_pMain->m_SeekingPartyArrayLock);
	foreach(itr, g_pMain->m_SeekingPartyArray)
	{
		if ((*itr) == nullptr)
			continue;

		if ((*itr)->m_sGetID == GetID())
		{
			g_pMain->m_SeekingPartyArray.erase(itr);
			break;
		}
	}

	StateChangeServerDirect(2, 1); // not looking for a party
	SendPartyBBSNeeded(pkt);
}

void CUser::PartyBBSPartyChange(Packet& pkt)
{
	uint16 page_index = 0;
	if (!isPartyLeader())
		return;

	_PARTY_GROUP* pParty = g_pMain->GetPartyPtr(GetPartyID());
	if (pParty == nullptr)
		return;

	Packet result(WIZ_PARTY_BBS, uint8(PARTY_TYPE_SEEKING));
	pkt.DByte();
	pkt >> pParty->sWantedClass >> page_index >> pParty->WantedMessage;
	result << uint8(PARTY_BBS_WANTED) << uint8(1);

	_SEEKING_PARTY_USER* pPartyUser = nullptr;
	uint16 seeking_index = 0; // holds the index where we located at in the vector
	Guard lock(g_pMain->m_SeekingPartyArrayLock);
	foreach(itr, g_pMain->m_SeekingPartyArray)
	{
		if ((*itr) == nullptr)
			continue;

		seeking_index++;
		if ((*itr)->m_sGetID == GetID())
		{
			pPartyUser = (*itr);
			pPartyUser->m_strSeekingNote = pParty->WantedMessage;
			pPartyUser->m_sClass = pParty->sWantedClass;
			break;
		}
	}

	SendPartyBBSNeeded(pkt);

	uint16 counter = 0;
	foreach(itr, g_pMain->m_SeekingPartyArray)
	{
		if ((*itr) == nullptr)
			continue;

		counter++;
		if (counter < seeking_index)
			continue;

		result << (*itr)->m_strUserID << uint8((*itr)->m_sLevel) << (*itr)->m_sClass;
		if (counter >= 12)
			break;
	}

	uint16 s = counter - seeking_index;
	// You still need to fill up ten slots.
	if (s < MAX_BBS_PAGE)
	{
		for (int j = s; j < MAX_BBS_PAGE; j++)
			result << uint16(0) << uint16(0)
			<< uint16(0) << uint8(0)
			<< uint8(0) << uint8(0)
			<< uint16(0)
			<< uint8(0);
	}
	result << uint16(0) << uint8(0) << uint16(s);
	Send(&result);
}

void CUser::PartyBBSRegister(Packet& pkt)
{
	_SEEKING_PARTY_USER* pUser = nullptr;
	string seeking_msg = "";
	Packet result(WIZ_PARTY_BBS, uint8(PARTY_TYPE_SEEKING));
	pkt.SByte();
	pkt >> seeking_msg;

	result << uint8(PARTY_BBS_REGISTER) << uint8(1);
	StateChangeServerDirect(2, 2); // seeking a party

	uint16 seeking_index = 0; // holds the index where we located at in the vector
	Guard lock(g_pMain->m_SeekingPartyArrayLock);
	foreach(itr, g_pMain->m_SeekingPartyArray)
	{
		if ((*itr) == nullptr)
			continue;

		seeking_index++;
		if ((*itr)->m_sGetID == GetID())
		{
			pUser = (*itr);
			pUser->m_strSeekingNote = seeking_msg;
			break;
		}
	}

	if (pUser == nullptr)
	{
		pUser = new _SEEKING_PARTY_USER;
		pUser->m_bNation = GetNation();
		pUser->m_strUserID = GetName();
		pUser->m_sClass = GetClass();
		pUser->m_bSeekType = 0;
		pUser->m_bZone = GetZoneID();
		pUser->m_strSeekingNote = pUser->m_strSeekingNote = seeking_msg;
		pUser->m_sLevel = GetLevel();
		pUser->m_sGetID = GetID();
		pUser->m_sPartyID = GetPartyID();
		pUser->isPartyLeader = isPartyLeader();
		pUser->m_bloginType = 0;
		g_pMain->m_SeekingPartyArray.push_back(pUser);
	}

	SendPartyBBSNeeded(pkt);

	uint16 counter = 0;
	foreach(itr, g_pMain->m_SeekingPartyArray)
	{
		if ((*itr) == nullptr)
			continue;

		counter++;
		if (counter < seeking_index)
			continue;

		result << (*itr)->m_strUserID << uint8((*itr)->m_sLevel) << (*itr)->m_sClass;
		if (counter >= 12)
			break;
	}

	uint16 s = counter - seeking_index;
	// You still need to fill up ten slots.
	if (s < MAX_BBS_PAGE)
	{
		for (int j = s; j < MAX_BBS_PAGE; j++)
			result << uint16(0) << uint16(0)
			<< uint16(0) << uint8(0)
			<< uint8(0) << uint8(0)
			<< uint16(0)
			<< uint8(0);
	}
	result << uint16(0) << uint8(0) << uint16(s);

	Send(&result);
}

void CUser::PartyBBSDelete(Packet& pkt)
{
	if (m_bNeedParty == 1)
		return;

	Guard lock(g_pMain->m_SeekingPartyArrayLock);
	// You don't need anymore 
	foreach(itr, g_pMain->m_SeekingPartyArray)
	{
		if ((*itr) == nullptr)
			continue;

		if ((*itr)->m_sGetID == GetID())
		{
			g_pMain->m_SeekingPartyArray.erase(itr);
			break;
		}
	}

	StateChangeServerDirect(2, 1); // not looking for a party
	SendPartyBBSNeeded(pkt);
}

void CUser::PartyBBSNeeded(Packet& pkt)
{
	/*uint16 page_index = 0;
	uint8 typefilter = 0, locationFilter = 0, levelFilter = 0;
	pkt >> page_index >> typefilter >> locationFilter >> levelFilter;
	SendPartyBBSNeeded(page_index,type,typefilter, locationFilter, levelFilter);*/
}

void CUser::SendPartyBBSNeeded(Packet& pkt)
{
	Packet result(WIZ_PARTY_BBS);
	uint16 page_index = 0;
	uint8 typefilter = 0, locationFilter = 0, levelFilter = 0;
	pkt >> page_index >> typefilter >> locationFilter >> levelFilter;
	uint16 start_counter = 0, BBS_Counter = 0;
	start_counter = page_index * MAX_BBS_PAGE;

	if (start_counter >= MAX_USER)
	{
		result << uint8(PARTY_TYPE_SEEKING) << uint8(PARTY_BBS_NEEDED) << uint8(0);
		Send(&result);
		return;
	}

	result << uint8(PARTY_TYPE_SEEKING) << uint8(11) << uint8(1) << page_index << uint16(BBS_Counter);

	Guard lock(g_pMain->m_SeekingPartyArrayLock);
	foreach(itr, g_pMain->m_SeekingPartyArray)
	{
		_SEEKING_PARTY_USER* pUser = (*itr);
		_PARTY_GROUP* pParty = nullptr;
		uint8 PartyMembers = 0;

		if (pUser == nullptr)
			continue;

		uint16 sClass = pUser->m_sClass;
		if ((BBS_Counter < page_index * MAX_BBS_PAGE)
			|| (typefilter == 2 && pUser->isPartyLeader == 1)
			|| (typefilter == 3 && pUser->isPartyLeader == 0)
			|| (locationFilter > 0 && locationFilter != pUser->m_bZone)
			|| (pUser->m_bloginType == 2)
			|| (levelFilter > 0 && levelFilter == 1 && pUser->m_sLevel > 11)
			|| (levelFilter > 0 && levelFilter == 2 && (pUser->m_sLevel < 11 || pUser->m_sLevel > 20))
			|| (levelFilter > 0 && levelFilter == 3 && (pUser->m_sLevel < 21 || pUser->m_sLevel > 30))
			|| (levelFilter > 0 && levelFilter == 4 && (pUser->m_sLevel < 31 || pUser->m_sLevel > 40))
			|| (levelFilter > 0 && levelFilter == 5 && (pUser->m_sLevel < 41 || pUser->m_sLevel > 50))
			|| (levelFilter > 0 && levelFilter == 6 && (pUser->m_sLevel < 51 || pUser->m_sLevel > 60))
			|| (levelFilter > 0 && levelFilter == 7 && (pUser->m_sLevel < 61 || pUser->m_sLevel > 70))
			|| (levelFilter > 0 && levelFilter == 8 && (pUser->m_sLevel < 71 || pUser->m_sLevel > 80))
			|| (levelFilter > 0 && levelFilter == 9 && pUser->m_sLevel < 81))
			continue;

		BBS_Counter++;

		if (BBS_Counter >= MAX_BBS_PAGE)
			break;

		if (pUser->isPartyLeader)
		{
			pParty = g_pMain->GetPartyPtr(pUser->m_sPartyID);
			if (pParty == nullptr) //Shouldn't be hit.
				return;

			pUser->m_strSeekingNote = pParty->WantedMessage;
			PartyMembers = GetPartyMemberAmount(pParty);
			sClass = pParty->sWantedClass;
		}

		result.DByte();
		result << pUser->m_bNation << uint8(pUser->m_bSeekType)
			<< pUser->m_strUserID
			<< sClass
			<< uint16(0) << pUser->m_sLevel //Not sure what that uint16 does.
			<< uint8(pUser->isPartyLeader ? 3 : 2); //2 is player, 3 is party leader
		result.SByte();
		result << pUser->m_strSeekingNote
			<< pUser->m_bZone
			<< PartyMembers;
	}

	// You still need to fill up ten slots.
	if (BBS_Counter < MAX_BBS_PAGE)
	{
		for (int j = BBS_Counter; j < MAX_BBS_PAGE; j++)
			result << uint16(0) << uint16(0)
			<< uint16(0) << uint8(0)
			<< uint8(0) << uint8(0)
			<< uint16(0)
			<< uint8(0);
	}
	int page_number = (int)g_pMain->m_SeekingPartyArray.size() / MAX_BBS_PAGE;
	if (page_number % MAX_BBS_PAGE != 0) page_number++;
	result << page_index << page_number;
	result.put(5, BBS_Counter);
	Send(&result);
}

void CUser::PartyBBSWanted(Packet& pkt)
{
	uint16 page_index = 0;
	if (!isPartyLeader())
		return;

	_PARTY_GROUP* pParty = g_pMain->GetPartyPtr(GetPartyID());
	if (pParty == nullptr)
		return;

	Packet result(WIZ_PARTY_BBS, uint8(PARTY_TYPE_SEEKING));
	pkt.DByte();
	pkt >> pParty->sWantedClass >> page_index >> pParty->WantedMessage;
	result << uint8(PARTY_BBS_WANTED) << uint8(1);

	_SEEKING_PARTY_USER* pPartyUser = nullptr;
	uint16 seeking_index = 0; // holds the index where we located at in the vector
	Guard lock(g_pMain->m_SeekingPartyArrayLock);
	foreach(itr, g_pMain->m_SeekingPartyArray)
	{
		if ((*itr) == nullptr)
			continue;

		seeking_index++;
		if ((*itr)->m_sGetID == GetID())
		{
			pPartyUser = (*itr);
			pPartyUser->m_strSeekingNote = pParty->WantedMessage;
			pPartyUser->m_sClass = pParty->sWantedClass;
			break;
		}
	}

	if (pPartyUser == nullptr)
	{
		pPartyUser = new _SEEKING_PARTY_USER;
		pPartyUser->m_bNation = GetNation();
		pPartyUser->m_strUserID = GetName();
		pPartyUser->m_sClass = pParty->sWantedClass;
		pPartyUser->m_bSeekType = 0;
		pPartyUser->m_bZone = GetZoneID();
		pPartyUser->m_strSeekingNote = pPartyUser->m_strSeekingNote = pParty->WantedMessage;
		pPartyUser->m_sLevel = GetLevel();
		pPartyUser->m_sGetID = GetID();
		pPartyUser->m_sPartyID = GetPartyID();
		pPartyUser->isPartyLeader = isPartyLeader();
		pPartyUser->m_bloginType = 0;
		g_pMain->m_SeekingPartyArray.push_back(pPartyUser);
	}

	StateChangeServerDirect(2, 3); // Looking for party nember
	SendPartyBBSNeeded(pkt);

	uint16 counter = 0;
	foreach(itr, g_pMain->m_SeekingPartyArray)
	{
		if ((*itr) == nullptr)
			continue;

		counter++;
		if (counter < seeking_index)
			continue;

		result << (*itr)->m_strUserID << uint8((*itr)->m_sLevel) << (*itr)->m_sClass;
		if (counter >= 12)
			break;
	}

	uint16 s = counter - seeking_index;
	// You still need to fill up ten slots.
	if (s < MAX_BBS_PAGE)
	{
		for (int j = s; j < MAX_BBS_PAGE; j++)
			result << uint16(0) << uint16(0)
			<< uint16(0) << uint8(0)
			<< uint8(0) << uint8(0)
			<< uint16(0)
			<< uint8(0);
	}
	result << uint16(0) << uint8(0) << uint16(s);
	Send(&result);
}

void CUser::PartyBBSUserLoqOut()
{
	if (g_pMain->m_SeekingPartyArray.size() > 0)
	{
		Guard lock(g_pMain->m_SeekingPartyArrayLock);
		// You don't need anymore because user is exit.
		foreach(itr, g_pMain->m_SeekingPartyArray)
		{
			if ((*itr) == nullptr)
				continue;

			if ((*itr)->m_sGetID == GetID())
			{
				g_pMain->m_SeekingPartyArray.erase(itr);
				break;
			}
		}
	}
}

uint8 CUser::GetPartyMemberAmount(_PARTY_GROUP* pParty)
{
	if (pParty == nullptr)
		pParty = g_pMain->GetPartyPtr(GetPartyID());

	if (pParty == nullptr)
		return 0;

	uint8 PartyMembers = 0;
	for (int i = 0; i < MAX_PARTY_USERS; i++)
	{
		if (pParty->uid[i] >= 0)
			PartyMembers++;
	}
	return PartyMembers;
}

void CUser::SendPartyStatusUpdate(uint8 bStatus, uint8 bResult /*= 0*/)
{
	if (!isInParty())
		return;

	Packet result(WIZ_PARTY, uint8(PARTY_STATUSCHANGE));
	result << GetSocketID() << bStatus << bResult;
	g_pMain->Send_PartyMember(GetPartyID(), &result);
}

void CUser::GrantChickenManner()
{
	uint8 bLevel = GetLevel(), bManner = 0;
	// No manner points if you're not a chicken anymore nor when you're not in a party.
	if (!m_bIsChicken || !isInParty())
		return;

	_PARTY_GROUP* pParty = nullptr;
	pParty = g_pMain->GetPartyPtr(GetPartyID());

	if (pParty == nullptr)
		return;

	for (int i = 0; i < MAX_PARTY_USERS; i++)
	{
		CUser* pTargetUser = nullptr;
		if (pParty->uid[i] != GetSocketID())
			pTargetUser = g_pMain->GetUserPtr(pParty->uid[i]);

		if (pTargetUser == nullptr
			|| pTargetUser->isDead()
			|| pTargetUser->m_bIsChicken)
			continue;

		if (!isInRange(pTargetUser, RANGE_50M))
			continue;

		if (pTargetUser->GetLevel() > 20 && pTargetUser->GetLevel() < 40)
			bManner = pTargetUser->GetLevel() / 10;
		else
			bManner = 1;

		pTargetUser->SendMannerChange(bManner);
	}
}