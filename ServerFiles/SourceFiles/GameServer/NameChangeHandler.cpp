#include "stdafx.h"
#include "DBAgent.h"
#include "KnightsManager.h"

using std::string;
extern CDBAgent g_DBAgent;

#pragma region CUser::SendNameChange()
void CUser::SendNameChange()
{
	if (!isInGame()
		|| isDead()
		|| isTrading()
		|| isMerchanting()
		|| isStoreOpen()
		|| isSellingMerchant()
		|| isBuyingMerchant()
		|| isMining()
		|| isFishing())
		return;

	Packet result(WIZ_NAME_CHANGE, uint8(NameChangeShowDialog));
	Send(&result);
}
#pragma endregion

#pragma region CUser::SendClanNameChange()
void CUser::SendClanNameChange()
{
	if (!isInGame()
		|| isDead()
		|| isTrading()
		|| isMerchanting()
		|| isStoreOpen()
		|| isSellingMerchant()
		|| isBuyingMerchant()
		|| isMining()
		|| isFishing())
		return;

	Packet result(WIZ_NAME_CHANGE, uint8(ClanNameChange));
	result << uint8(ShowDialog);
	Send(&result);
}
#pragma endregion

#pragma region CUser::HandlePlayerClanNameChange(Packet & pkt)
void CUser::HandleSelectCharacterNameChange(Packet & pkt)
{
	Packet result(WIZ_NAME_CHANGE, uint8(CharSelectNameChange));
	result << uint16(2) << uint8(16);
	Send(&result);
}
#pragma endregion

#pragma region CUser::HandleNameChange(Packet & pkt)
void CUser::HandleNameChange(Packet & pkt)
{
	uint8 opcode;
	pkt >> opcode;

	if (!isInGame()
		|| isDead()
		|| isTrading()
		|| isMerchanting()
		|| isStoreOpen()
		|| isSellingMerchant()
		|| isBuyingMerchant()
		|| isMining()
		|| isFishing())
		return;

	switch (opcode)
	{
	case CharNameChange:
		HandlePlayerNameChange(pkt);
		break;
	case CharSelectNameChange:
		HandleSelectCharacterNameChange(pkt);
		break;
	case ClanNameChange:
		HandlePlayerClanNameChange(pkt);
		break;
	default:
		printf("NameChange Unhandled packets opcode (%u) \n", opcode);
		TRACE("NameChange Unhandled packets opcode (%u) \n", opcode);
		break;
	}
}
#pragma endregion

#pragma region CUser::HandlePlayerClanNameChange(Packet & pkt)
void CUser::HandlePlayerClanNameChange(Packet & pkt)
{
	Packet result(WIZ_NAME_CHANGE, uint8(ClanNameChange));
	string strClanID;
	pkt >> strClanID;

	// Ensure we have the scroll before handling this request.
	if (!CheckExistItem(ITEM_CLAN_NAME_SCROLL))
	{
		result << uint8(ShowDialog);
		Send(&result);
		return;
	}

	if (strClanID.empty()
		|| strClanID.length() > MAX_ID_SIZE)
	{
		result << uint8(InvalidName);
		Send(&result);
		return;
	}

	CKnights * pKnights = g_pMain->GetClanPtr(GetClanID());
	if (pKnights == nullptr
		|| !isClanLeader()
		|| !isInClan())
	{
		result << uint8(NotClan);
		Send(&result);
		return;
	}

	foreach_stlmap(itr, g_pMain->m_KnightsArray)
	{
		CKnights * pKnights = itr->second;
		if (pKnights == nullptr)
			continue;

		if (strClanID == pKnights->GetName())
		{
			result << uint8(InvalidName);
			Send(&result);
			return;
		}
	}

	result << strClanID;
	g_pMain->AddDatabaseRequest(result, this);
}
#pragma endregion


#pragma region CUser::HandlePlayerNameChange(Packet & pkt)
void CUser::HandlePlayerNameChange(Packet & pkt)
{
	Packet result(WIZ_NAME_CHANGE);
	string strUserID;
	pkt >> strUserID;

	// Ensure we have the scroll before handling this request.
	if (!CheckExistItem(ITEM_SCROLL_OF_IDENTITY))
	{
		result << uint8(NameChangeShowDialog);
		Send(&result);
		return;
	}

	if (strUserID.empty()
		|| strUserID.length() > MAX_ID_SIZE)
	{
		result << uint8(NameChangeInvalidName);
		Send(&result);
		return;
	}

	if (isKing())
	{
		result << uint8(NameChangeKing);
		Send(&result);
		return;
	}

	if (isInClan())
	{
		result << uint8(NameChangeInClan);
		Send(&result);
		return;
	}

	result << uint8(CharNameChange) << strUserID;
	g_pMain->AddDatabaseRequest(result, this);
}
#pragma endregion


#pragma region CUser::NameChangeSystem(Packet & pkt)
void CUser::NameChangeSystem(Packet & pkt)
{
	Packet result(WIZ_NAME_CHANGE);
	uint8 opcode;
	string strName, strClanName;

	pkt >> opcode;

	switch (opcode)
	{
	case CharNameChange:
	{
		pkt >> strName;
		uint8 bResult = 0;

		bResult = g_DBAgent.UpdateCharacterName(GetAccountName(), GetName(), strName);
		if (bResult == 3)
		{
			if (isInClan())
			{
				CKnights* pKnights = g_pMain->GetClanPtr(GetClanID());
				if (pKnights != nullptr)
				{
					std::string userid = GetName();
					std::string useridcopy = strName;
					STRTOUPPER(userid);
					STRTOUPPER(useridcopy);
					_KNIGHTS_USER * pKnightUser = pKnights->m_arKnightsUser.GetData(userid);
					if (pKnightUser != nullptr)
					{
						_KNIGHTS_USER * pKnightUserCopy = new _KNIGHTS_USER();
						pKnightUserCopy->strUserName = strName;
						pKnightUserCopy->bLevel = pKnightUser->bLevel;
						pKnightUserCopy->sClass = pKnightUser->sClass;
						pKnightUserCopy->nLastLogin = pKnightUser->nLastLogin;
						pKnightUserCopy->bFame = pKnightUser->bFame;
						pKnightUserCopy->nLoyalty = pKnightUser->nLoyalty;
						pKnightUserCopy->nDonatedNP = pKnightUser->nDonatedNP;
						pKnightUserCopy->strMemo = pKnightUser->strMemo;
						m_pKnightsUser = pKnightUserCopy;

						if (pKnights->m_arKnightsUser.PutData(useridcopy, pKnightUserCopy))
							pKnights->m_arKnightsUser.DeleteData(userid);
					}
				}
			}
			HandleSurroundingUserRegionUpdate();

			// Replace the character's name (in both the session and the character lookup hashmap).
			g_pMain->ReplaceCharacterName(this, strName);

			if (isInClan() && m_pKnightsUser != nullptr)
				m_pKnightsUser->strUserName = strName;

			// Remove user from others' view & make them reappear again so 
			// the name can be updated for those currently in range.
			UserInOut(INOUT_OUT);
			UserInOut(INOUT_IN);

			result.DByte();
			result << uint8(NameChangeSuccess) << strName;
			Send(&result);

			// Take the scroll...
			RobItem(ITEM_SCROLL_OF_IDENTITY);
		}
		else
		{
			result << uint8(bResult);
			Send(&result);
			return;
		}
	}
	case CharSelectNameChange:
		break;
	case ClanNameChange:
	{
		pkt >> strClanName;

		CKnights * pKnights = g_pMain->GetClanPtr(GetClanID());
		if (pKnights == nullptr)
		{
			result << uint8(ClanNameChange) << uint8(NotClan);
			Send(&result);
			return;
		}

		uint8 bResult = 0;
		bResult = g_DBAgent.UpdateCharacterClanName(pKnights->GetName(), strClanName);
		if (bResult == 3)
		{
			// Replace the Clan Name.
			g_pMain->ReloadKnightAndUserRanks();

			pKnights->m_strName = strClanName;
			// Remove user from others' view & make them reappear again so 
			// the Clan Name can be updated for those currently in range.
			UserInOut(INOUT_OUT);
			UserInOut(INOUT_IN);

			result.DByte();
			result << uint8(ClanNameChange) << uint8(Succes) << strClanName;
			Send(&result);

			g_pMain->Send_KnightsMember(pKnights->GetID(), &result);

			// Take the scroll...
			RobItem(ITEM_CLAN_NAME_SCROLL);
		}
		else
		{
			result << uint8(ClanNameChange) << uint8(bResult);
			Send(&result);
		}
	}
	break;
	default:
		printf("NameChangeSystem Unhandled opcode packets (%x) \n", opcode);
		TRACE("NameChangeSystem Unhandled opcode packets (%x) \n", opcode);
		break;
	}
}
#pragma endregion
