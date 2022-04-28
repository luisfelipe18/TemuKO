#include "stdafx.h"
using namespace std;

#pragma region CUser::OperatorCommand(Packet & pkt)
void CUser::OperatorCommand(Packet & pkt)
{
	if (!isGM())
		return;

	Packet result(WIZ_AUTHORITY_CHANGE);
	std::string strUserID;
	uint8 opcode;
	bool bIsOnline = false;
	DateTime time;
	std::string sNoticeMessage, sOperatorCommandType;
	pkt >> opcode >> strUserID;

	if (strUserID.empty() || strUserID.size() > MAX_ID_SIZE)
		return;

	CUser *pUser = g_pMain->GetUserPtr(strUserID, TYPE_CHARACTER);
	if (pUser == nullptr)
		bIsOnline = false;
	else
		bIsOnline = true;

	switch (opcode)
	{
	case OPERATOR_ARREST:
		if (bIsOnline)
		{
			ZoneChange(pUser->GetZoneID(), pUser->m_curx, pUser->m_curz);
			sOperatorCommandType = "OPERATOR_ARREST";
			g_pMain->WriteBanListLogFile(string_format("%d:%d:%d || Sender=%s,Command=%s,User=%s,Period=Permanent,Zone=%d(%d,%d)\n",
				time.GetHour(), time.GetMinute(), time.GetSecond(), GetName().c_str(), sOperatorCommandType.c_str(), strUserID.c_str(), GetZoneID(), uint16(GetX()), uint16(GetZ())));
		}
		break;
	case OPERATOR_SUMMON:
		if (bIsOnline)
		{
			pUser->ZoneChange(GetZoneID(), m_curx, m_curz);
			sOperatorCommandType = "OPERATOR_SUMMON";
			g_pMain->WriteBanListLogFile(string_format("%d:%d:%d || Sender=%s,Command=%s,User=%s,Period=Permanent,Zone=%d(%d,%d)\n",
				time.GetHour(), time.GetMinute(), time.GetSecond(), GetName().c_str(), sOperatorCommandType.c_str(), strUserID.c_str(), GetZoneID(), uint16(GetX()), uint16(GetZ())));
		}
		break;
	case OPERATOR_CUTOFF:
		if (bIsOnline)
		{
			pUser->Disconnect();
			sOperatorCommandType = "OPERATOR_CUTOFF";
			g_pMain->WriteBanListLogFile(string_format("%d:%d:%d || Sender=%s,Command=%s,User=%s,Period=Permanent,Zone=%d(%d,%d)\n",
				time.GetHour(), time.GetMinute(), time.GetSecond(), GetName().c_str(), sOperatorCommandType.c_str(), strUserID.c_str(), GetZoneID(), uint16(GetX()), uint16(GetZ())));
		}
		break;
	case OPERATOR_BAN:
	case OPERATOR_BAN_ACCOUNT: // ban account is meant to call a proc to do so
		sOperatorCommandType = "OPERATOR_BAN";
		result << GetName() << strUserID << uint16(AUTHORITY_BANNED) << uint8(BanTypes::BANNED) << uint32(999) << sOperatorCommandType;
		g_pMain->AddDatabaseRequest(result, this);
		g_pMain->WriteBanListLogFile(string_format("%d:%d:%d || Sender=%s,Command=%s,User=%s,Period=Permanent,Zone=%d(%d,%d)\n",
			time.GetHour(), time.GetMinute(), time.GetSecond(), GetName().c_str(), sOperatorCommandType.c_str(), strUserID.c_str(), GetZoneID(), uint16(GetX()), uint16(GetZ())));
		break;
	case OPERATOR_MUTE:
		sOperatorCommandType = "OPERATOR_MUTE";
		result << GetName() << strUserID << uint16(AUTHORITY_MUTED) << uint8(BanTypes::MUTE) << uint32(3) << sOperatorCommandType;
		g_pMain->AddDatabaseRequest(result, this);
		g_pMain->WriteBanListLogFile(string_format("%d:%d:%d || Sender=%s,Command=%s,User=%s,Period=Permanent,Zone=%d(%d,%d)\n",
			time.GetHour(), time.GetMinute(), time.GetSecond(), GetName().c_str(), sOperatorCommandType.c_str(), strUserID.c_str(), GetZoneID(), uint16(GetX()), uint16(GetZ())));
		break;
	case OPERATOR_DISABLE_ATTACK:
		sOperatorCommandType = "OPERATOR_DISABLE_ATTACK";
		result << GetName() << strUserID << uint16(AUTHORITY_ATTACK_DISABLED) << uint8(BanTypes::DIS_ATTACK) << uint32(3) << sOperatorCommandType;
		g_pMain->AddDatabaseRequest(result, this);
		g_pMain->WriteBanListLogFile(string_format("%d:%d:%d || Sender=%s,Command=%s,User=%s,Period=Permanent,Zone=%d(%d,%d)\n",
			time.GetHour(), time.GetMinute(), time.GetSecond(), GetName().c_str(), sOperatorCommandType.c_str(), strUserID.c_str(), GetZoneID(), uint16(GetX()), uint16(GetZ())));
		break;
	case OPERATOR_ENABLE_ATTACK:
		sOperatorCommandType = "OPERATOR_ENABLE_ATTACK";
		result << GetName() << strUserID << uint16(AUTHORITY_PLAYER) << uint8(BanTypes::ALLOW_ATTACK) << uint32(0) << sOperatorCommandType;
		g_pMain->AddDatabaseRequest(result, this);
		g_pMain->WriteBanListLogFile(string_format("%d:%d:%d || Sender=%s,Command=%s,User=%s,Period=Permanent,Zone=%d(%d,%d)\n",
			time.GetHour(), time.GetMinute(), time.GetSecond(), GetName().c_str(), sOperatorCommandType.c_str(), strUserID.c_str(), GetZoneID(), uint16(GetX()), uint16(GetZ())));
		break;
	case OPERATOR_UNMUTE:
		sOperatorCommandType = "OPERATOR_UNMUTE";
		result << GetName() << strUserID << uint16(AUTHORITY_PLAYER) << uint8(BanTypes::UNMUTE) << uint32(0) << sOperatorCommandType;
		g_pMain->AddDatabaseRequest(result, this);
		g_pMain->WriteBanListLogFile(string_format("%d:%d:%d || Sender=%s,Command=%s,User=%s,Period=Permanent,Zone=%d(%d,%d)\n",
			time.GetHour(), time.GetMinute(), time.GetSecond(), GetName().c_str(), sOperatorCommandType.c_str(), strUserID.c_str(), GetZoneID(), uint16(GetX()), uint16(GetZ())));
		break;
	}
}

#pragma endregion 

#pragma region CUser::HandleHelpCommand
COMMAND_HANDLER(CUser::HandleHelpCommand)
{
	if (!isGM())
		return false;

	foreach(itr, s_commandTable)
	{
		auto i = itr->second;
		std::string s_Command = string_format("Command: +%s, Description: %s", i->Name, i->Help);
		g_pMain->SendHelpDescription(this, s_Command);
	}
	return true;
}
#pragma endregion

#pragma region CUser::HandleMuteCommand
COMMAND_HANDLER(CUser::HandleMuteCommand)
{
	if (!isGM())
		return false;

	if (m_GameMastersMute != 1)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Gerekli Yetki Yok. Lütfen Yetki Sýnýrlandýrmasý için Yönetici ile konuþunuz.");
		return true;
	}

	// Char name
	if (vargs.size() < 1)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +mute CharacterName");
		return true;
	}

	std::string strUserID = vargs.front();
	vargs.pop_front();

	if (strUserID.empty() || strUserID.size() > MAX_ID_SIZE)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +mute CharacterName");
		return true;
	}

	Packet result(WIZ_AUTHORITY_CHANGE);
	DateTime time;
	std::string sOperatorCommandType = "Mute";

	result << GetName() << strUserID << uint16(AUTHORITY_MUTED) << uint8(BanTypes::MUTE) << uint32(3) << sOperatorCommandType;
	g_pMain->AddDatabaseRequest(result, this);

	g_pMain->WriteBanListLogFile(string_format("%d:%d:%d || Sender=%s,Command=%s,User=%s,Period=Permanent,Zone=%d(%d,%d)\n",
		time.GetHour(), time.GetMinute(), time.GetSecond(), GetName().c_str(), sOperatorCommandType.c_str(), strUserID.c_str(), GetZoneID(), uint16(GetX()), uint16(GetZ())));
	return true;
}

#pragma endregion

#pragma region CUser::HandleUnMuteCommand
COMMAND_HANDLER(CUser::HandleUnMuteCommand)
{
	if (!isGM())
		return false;

	if (m_GameMastersUnMute != 1)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Gerekli Yetki Yok. Lütfen Yetki Sýnýrlandýrmasý için Yönetici ile konuþunuz.");
		return true;
	}

	// Char name
	if (vargs.size() < 1)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +unmute CharacterName");
		return true;
	}

	std::string strUserID = vargs.front();
	vargs.pop_front();

	if (strUserID.empty() || strUserID.size() > MAX_ID_SIZE)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +unmute CharacterName");
		return true;
	}

	Packet result(WIZ_AUTHORITY_CHANGE);
	DateTime time;
	std::string sOperatorCommandType = "UnMute";

	result << GetName() << strUserID << uint16(AUTHORITY_PLAYER) << uint8(BanTypes::UNMUTE) << uint32(0) << sOperatorCommandType;
	g_pMain->AddDatabaseRequest(result, this);

	g_pMain->WriteBanListLogFile(string_format("%d:%d:%d || Sender=%s,Command=%s,User=%s,Period=Permanent,Zone=%d(%d,%d)\n",
		time.GetHour(), time.GetMinute(), time.GetSecond(), GetName().c_str(), sOperatorCommandType.c_str(), strUserID.c_str(), GetZoneID(), uint16(GetX()), uint16(GetZ())));
	return true;
}
#pragma endregion

#pragma region CUser::HandlePermanentCommand
COMMAND_HANDLER(CUser::HandlePermanentCommand)
{
	if (!isGM())
		return false;

	if (m_GameMastersBanPermit != 1)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Gerekli Yetki Yok. Lütfen Yetki Sýnýrlandýrmasý için Yönetici ile konuþunuz.");
		return true;
	}

	if (vargs.size() < 2)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +block CharacterName Description");
		return true;
	}

	std::string strUserID = vargs.front();
	vargs.pop_front();

	if (strUserID.empty() || strUserID.size() > MAX_ID_SIZE)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +block CharacterName is empty or too long!");
		return true;
	}

	uint16 vargsize = (uint16)vargs.size();

	if (vargsize >= (1 + 50))
	{
		// send description
		g_pMain->SendHelpDescription(this, "Error : long word!");
		return true;
	}

	std::string word[50];
	std::string guncelword = "";
	for (int x = 1; x <= vargsize; x++)
	{
		word[x] = vargs.front();
		vargs.pop_front();

		if (!word[x].empty())
		{
			if (word[x].size() > 75)
			{
				// send description
				g_pMain->SendHelpDescription(this, "Error : long word!");
				return true;
			}
		}
		else
		{
			word[x] = "";
		}
		guncelword += word[x] + ' ';
	}
	Packet result(WIZ_AUTHORITY_CHANGE);
	DateTime time;

	result << GetName() << strUserID << uint16(AUTHORITY_BANNED) << uint8(BanTypes::BANNED) << uint32(999) << guncelword;
	g_pMain->AddDatabaseRequest(result, this);

	g_pMain->WriteBanListLogFile(string_format("%d:%d:%d || Sender=%s,Command=%s,User=%s,Period=Permanent,Zone=%d(%d,%d)\n",
		time.GetHour(), time.GetMinute(), time.GetSecond(), GetName().c_str(), guncelword.c_str(), strUserID.c_str(), GetZoneID(), uint16(GetX()), uint16(GetZ())));

	return true;
}

#pragma region CUser::HandlePermitConnectCommand
COMMAND_HANDLER(CUser::HandlePermitConnectCommand)
{
	if (!isGM())
		return false;

	if (m_GameMastersUnBan != 1)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Gerekli Yetki Yok. Lütfen Yetki Sýnýrlandýrmasý için Yönetici ile konuþunuz.");
		return true;
	}

	// Char name
	if (vargs.size() < 1)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +unblock CharacterName");
		return true;
	}

	std::string strUserID = vargs.front();
	vargs.pop_front();

	if (strUserID.empty() || strUserID.size() > MAX_ID_SIZE)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +unblock CharacterName");
		return true;
	}

	Packet result(WIZ_AUTHORITY_CHANGE);
	DateTime time;
	std::string sOperatorCommandType = "Open Account Blocked";

	result << GetName() << strUserID << uint16(AUTHORITY_PLAYER) << uint8(BanTypes::UNBAN) << uint32(0) << sOperatorCommandType;
	g_pMain->AddDatabaseRequest(result, this);

	g_pMain->WriteBanListLogFile(string_format("%d:%d:%d || Sender=%s,Command=%s,User=%s,Period=Permanent,Zone=%d(%d,%d)\n",
		time.GetHour(), time.GetMinute(), time.GetSecond(), GetName().c_str(), sOperatorCommandType.c_str(), strUserID.c_str(), GetZoneID(), uint16(GetX()), uint16(GetZ())));
	return true;
}

#pragma endregion

#pragma region CUser::HandleAllowAttackCommand
COMMAND_HANDLER(CUser::HandleAllowAttackCommand)
{
	if (!isGM())
		return false;

	if (m_GameMastersAllowAttack != 1)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Gerekli Yetki Yok. Lütfen Yetki Sýnýrlandýrmasý için Yönetici ile konuþunuz.");
		return true;
	}

	// Char name
	if (vargs.size() < 1)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +allow CharacterName");
		return true;
	}

	std::string strUserID = vargs.front();
	vargs.pop_front();

	if (strUserID.empty() || strUserID.size() > MAX_ID_SIZE)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +allow CharacterName");
		return true;
	}

	Packet result(WIZ_AUTHORITY_CHANGE);
	DateTime time;
	std::string sNoticeMessage, sOperatorCommandType;

	sOperatorCommandType = "Allow Attack";

	result << GetName() << strUserID << uint16(AUTHORITY_PLAYER) << uint8(BanTypes::ALLOW_ATTACK) << uint32(0) << sOperatorCommandType;
	g_pMain->AddDatabaseRequest(result, this);
	g_pMain->WriteBanListLogFile(string_format("%d:%d:%d || Sender=%s,Command=%s,User=%s,Period=Permanent,Zone=%d(%d,%d)\n",
		time.GetHour(), time.GetMinute(), time.GetSecond(), GetName().c_str(), sOperatorCommandType.c_str(), strUserID.c_str(), GetZoneID(), uint16(GetX()), uint16(GetZ())));
	return true;
}
#pragma endregion

#pragma region CUser::HandleDisableCommand
COMMAND_HANDLER(CUser::HandleDisableCommand)
{
	if (!isGM())
		return false;

	if (m_GameMastersDisabledAttack != 1)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Gerekli Yetki Yok. Lütfen Yetki Sýnýrlandýrmasý için Yönetici ile konuþunuz.");
		return true;
	}

	// Char name
	if (vargs.size() < 1)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +disable CharacterName");
		return true;
	}

	std::string strUserID = vargs.front();
	vargs.pop_front();

	if (strUserID.empty() || strUserID.size() > MAX_ID_SIZE)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +disable CharacterName");
		return true;
	}

	Packet result(WIZ_AUTHORITY_CHANGE);
	DateTime time;
	std::string sNoticeMessage, sOperatorCommandType;

	sOperatorCommandType = "Disable Attack";

	result << GetName() << strUserID << uint16(AUTHORITY_ATTACK_DISABLED) << uint8(BanTypes::DIS_ATTACK) << uint32(0) << sOperatorCommandType;
	g_pMain->AddDatabaseRequest(result, this);
	g_pMain->WriteBanListLogFile(string_format("%d:%d:%d || Sender=%s,Command=%s,User=%s,Period=Permanent,Zone=%d(%d,%d)\n",
		time.GetHour(), time.GetMinute(), time.GetSecond(), GetName().c_str(), sOperatorCommandType.c_str(), strUserID.c_str(), GetZoneID(), uint16(GetX()), uint16(GetZ())));
	return true;
}
#pragma endregion

#pragma region CUser::HandleNPAddCommand
COMMAND_HANDLER(CUser::HandleNPAddCommand)
{
	if (!isGM())
		return false;

	if (m_GameMastersNpAdd != 1)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Gerekli Yetki Yok. Lütfen Yetki Sýnýrlandýrmasý için Yönetici ile konuþunuz.");
		return true;
	}

	// Expects the bonus np percent, e.g. '+np_add' for a +15 np boost.
	if (vargs.empty())
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +np_add Percent");
		return true;
	}

	g_pMain->m_byNPEventAmount = (uint8)atoi(vargs.front().c_str());

	// Don't send the announcement if we're turning the event off.
	if (g_pMain->m_byNPEventAmount == 0)
		return true;

	g_pMain->SendFormattedResource(IDS_NP_REPAY_EVENT, Nation::ALL, false, g_pMain->m_byNPEventAmount);
	return true;
}

#pragma endregion

#pragma region CUser::HandleExpAddCommand
// Starts/stops the server XP event & sets its server-wide bonus.
COMMAND_HANDLER(CUser::HandleExpAddCommand)
{
	if (!isGM())
		return false;

	if (m_GameMastersExpAdd != 1)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Gerekli Yetki Yok. Lütfen Yetki Sýnýrlandýrmasý için Yönetici ile konuþunuz.");
		return true;
	}

	// Expects the bonus XP percent, e.g. '+exp_add' for a +15 XP boost.
	if (vargs.empty())
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +exp_add Percent");
		return true;
	}

	g_pMain->m_byExpEventAmount = (uint8)atoi(vargs.front().c_str());

	// Don't send the announcement if we're turning the event off.
	if (g_pMain->m_byExpEventAmount == 0)
		return true;

	g_pMain->SendFormattedResource(IDS_EXP_REPAY_EVENT, Nation::ALL, false, g_pMain->m_byExpEventAmount);
	return true;
}

#pragma endregion

#pragma region CUser::HandleMoneyAddCommand
// Starts/stops the server coin event & sets its server-wide bonus.
COMMAND_HANDLER(CUser::HandleMoneyAddCommand)
{
	if (!isGM())
		return false;

	if (m_GameMastersMoneyAdd != 1)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Gerekli Yetki Yok. Lütfen Yetki Sýnýrlandýrmasý için Yönetici ile konuþunuz.");
		return true;
	}

	// Expects the bonus coin percent, e.g. '+money_add' for a +15 dropped coin boost.
	if (vargs.empty())
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +money_add Percent");
		return true;
	}

	g_pMain->m_byCoinEventAmount = (uint8)atoi(vargs.front().c_str());

	// Don't send the announcement if we're turning the event off.
	if (g_pMain->m_byCoinEventAmount == 0)
		return true;

	g_pMain->SendFormattedResource(IDS_MONEY_REPAY_EVENT, Nation::ALL, false, g_pMain->m_byCoinEventAmount);
	return true;
}
#pragma endregion

#pragma region CUser::HandleLoyaltyChangeCommand
COMMAND_HANDLER(CUser::HandleLoyaltyChangeCommand)
{
	if (!isGM())
		return false;

	if (m_GameMastersLoyaltyChange != 1)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Gerekli Yetki Yok. Lütfen Yetki Sýnýrlandýrmasý için Yönetici ile konuþunuz.");
		return true;
	}

	// Char name | loyalty
	if (vargs.size() < 2)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +np_change CharacterName Loyalty(+/-)");
		return true;
	}

	std::string strUserID = vargs.front();
	vargs.pop_front();
	if (strUserID.empty() || strUserID.size() > MAX_ID_SIZE)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +np_change CharacterName Loyalty(+/-)");
		return true;
	}

	CUser *pUser = g_pMain->GetUserPtr(strUserID, TYPE_CHARACTER);
	if (pUser == nullptr)
	{
		g_pMain->SendHelpDescription(this, "Error : User is not online");
		return true;
	}

	uint32 nLoyalty = atoi(vargs.front().c_str());

	if (nLoyalty != 0)
		pUser->SendLoyaltyChange(nLoyalty, false);

	return true;
}
#pragma endregion

#pragma region CUser::HandleExpChangeCommand
COMMAND_HANDLER(CUser::HandleExpChangeCommand)
{
	if (!isGM())
		return false;

	if (m_GameMastersExpChange != 1)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Gerekli Yetki Yok. Lütfen Yetki Sýnýrlandýrmasý için Yönetici ile konuþunuz.");
		return true;
	}

	// Char name | exp
	if (vargs.size() < 2)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +exp_change CharacterName Exp(+/-)");
		return true;
	}

	std::string strUserID = vargs.front();
	vargs.pop_front();

	CUser *pUser = g_pMain->GetUserPtr(strUserID, TYPE_CHARACTER);
	if (pUser == nullptr)
	{
		g_pMain->SendHelpDescription(this, "Error : User is not online");
		return true;
	}

	int64 nExp = atoi(vargs.front().c_str());

	if (nExp != 0)
		pUser->ExpChange(nExp, true);

	return true;
}

#pragma endregion

#pragma region CUser::HandleGoldChangeCommand
COMMAND_HANDLER(CUser::HandleGoldChangeCommand)
{
	if (!isGM())
		return false;

	if (m_GameMastersMoneyChange != 1)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Gerekli Yetki Yok. Lütfen Yetki Sýnýrlandýrmasý için Yönetici ile konuþunuz.");
		return true;
	}

	// Char name | coins
	if (vargs.size() < 2)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +gold_change CharacterName Gold(+/-)");
		return true;
	}

	std::string strUserID = vargs.front();
	vargs.pop_front();

	CUser *pUser = g_pMain->GetUserPtr(strUserID, TYPE_CHARACTER);
	if (pUser == nullptr)
	{
		g_pMain->SendHelpDescription(this, "Error : User is not online");
		return true;
	}

	uint32 nGold = atoi(vargs.front().c_str());

	if (nGold != 0)
	{
		if (nGold > 0)
		{
			pUser->GoldGain(nGold);
			g_pMain->SendHelpDescription(this, "User has received coins.");
		}
		else
		{
			pUser->GoldLose(nGold);
			g_pMain->SendHelpDescription(this, "Coins was taken from the user.");
		}
	}

	return true;
}
#pragma endregion

#pragma region CUser::HandleGiveItemCommand
COMMAND_HANDLER(CUser::HandleGiveItemCommand)
{
	if (!isGM())
		return false;

	if (m_GameMastersGiveItem != 1)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Gerekli Yetki Yok. Lütfen Yetki Sýnýrlandýrmasý için Yönetici ile konuþunuz.");
		return true;
	}

	// Char name | item ID | [stack size]
	if (vargs.size() < 2)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +give_item CharacterName ItemID StackSize");
		return true;
	}

	std::string strUserID = vargs.front();
	vargs.pop_front();

	CUser *pUser = g_pMain->GetUserPtr(strUserID, TYPE_CHARACTER);
	if (pUser == nullptr)
	{
		g_pMain->SendHelpDescription(this, "Error : User is not online");
		return true;
	}

	uint32 nItemID = atoi(vargs.front().c_str());
	vargs.pop_front();
	_ITEM_TABLE *pItem = g_pMain->GetItemPtr(nItemID);
	if (pItem == nullptr)
	{
		// send error message saying the item does not exist
		g_pMain->SendHelpDescription(this, "Error : Item does not exist");
		return true;
	}

	uint16 sCount = 1;
	if (!vargs.empty())
		sCount = atoi(vargs.front().c_str());


	if (!pUser->GiveItem(nItemID, sCount))
		g_pMain->SendHelpDescription(this, "Error : Item could not be added");
	else
		g_pMain->SendHelpDescription(this, string_format("Item added successfully to %s!", pUser->GetName().c_str()).c_str());
	return true;
}
#pragma endregion

#pragma region CUser::HandleGiveItemSelfCommand
COMMAND_HANDLER(CUser::HandleGiveItemSelfCommand)
{
	if (!isGM())
		return false;

	if (m_GameMastersGiveItemSelf != 1)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Gerekli Yetki Yok. Lütfen Yetki Sýnýrlandýrmasý için Yönetici ile konuþunuz.");
		return true;
	}

	// Char name | item ID | [stack size]
	if (vargs.size() < 1)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +item itemID stackSize");
		return true;
	}

	uint32 nItemID = atoi(vargs.front().c_str());
	vargs.pop_front();
	_ITEM_TABLE *pItem = g_pMain->GetItemPtr(nItemID);
	if (pItem == nullptr)
	{
		// send error message saying the item does not exist
		g_pMain->SendHelpDescription(this, "Error : Item does not exist");
		return true;
	}

	uint16 sCount = 1;
	if (!vargs.empty())
		sCount = atoi(vargs.front().c_str());

	if (!GiveItem(nItemID, sCount))
		g_pMain->SendHelpDescription(this, "Error : Item could not be added");

	return true;
}
#pragma endregion

#pragma region CUser::HandleSummonUserCommand
COMMAND_HANDLER(CUser::HandleSummonUserCommand)
{
	if (!isGM())
		return false;

	if (m_GameMastersSummonUser != 1)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Gerekli Yetki Yok. Lütfen Yetki Sýnýrlandýrmasý için Yönetici ile konuþunuz.");
		return true;
	}

	// Char name | item ID | [stack size]
	if (vargs.size() < 1)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +summonuser CharacterName ");
		return true;
	}

	std::string strUserID = vargs.front();
	vargs.pop_front();

	CUser *pUser = g_pMain->GetUserPtr(strUserID, TYPE_CHARACTER);
	if (pUser == nullptr)
	{
		g_pMain->SendHelpDescription(this, "Error : User is not online");
		return true;
	}

	pUser->ZoneChange(GetZoneID(), GetX(), GetZ());
	return true;
}
#pragma endregion

#pragma region CUser::HandleTpOnUserCommand
COMMAND_HANDLER(CUser::HandleTpOnUserCommand)
{
	if (!isGM())
		return false;

	if (m_GameMastersTpOnUser != 1)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Gerekli Yetki Yok. Lütfen Yetki Sýnýrlandýrmasý için Yönetici ile konuþunuz.");
		return true;
	}

	// Char name | item ID | [stack size]
	if (vargs.size() < 1)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +tpon CharacterName ");
		return true;
	}

	std::string strUserID = vargs.front();
	vargs.pop_front();

	CUser *pUser = g_pMain->GetUserPtr(strUserID, TYPE_CHARACTER);
	if (pUser == nullptr)
	{
		g_pMain->SendHelpDescription(this, "Error : User is not online");
		return true;
	}

	ZoneChange(pUser->GetZoneID(), pUser->GetX(), pUser->GetZ());
	return true;
}

#pragma endregion

#pragma region CUser::HandleZoneChangeCommand
COMMAND_HANDLER(CUser::HandleZoneChangeCommand)
{
	if (!isGM())
		return false;

	if (m_GameMastersZoneChange != 1)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Gerekli Yetki Yok. Lütfen Yetki Sýnýrlandýrmasý için Yönetici ile konuþunuz.");
		return true;
	}

	if (vargs.empty())
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +zonechange ZoneNumber");
		return true;
	}

	// Behave as in official (we'll fix this later)
	int nZoneID = atoi(vargs.front().c_str());

	_START_POSITION * pStartPosition = g_pMain->GetStartPosition(nZoneID);
	if (pStartPosition == nullptr)
		return false;

	m_bEventRoom = 0;

	ZoneChange(nZoneID,
		(float)(GetNation() == KARUS ? pStartPosition->sKarusX : pStartPosition->sElmoradX + myrand(0, pStartPosition->bRangeX)),
		(float)(GetNation() == KARUS ? pStartPosition->sKarusZ : pStartPosition->sElmoradZ + myrand(0, pStartPosition->bRangeZ)));

	return true;
}
#pragma endregion

#pragma region CUser::HandleLocationChange
COMMAND_HANDLER(CUser::HandleLocationChange)
{
	if (!isGM())
		return false;

	if (m_GameMastersLocationChange != 1)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Gerekli Yetki Yok. Lütfen Yetki Sýnýrlandýrmasý için Yönetici ile konuþunuz.");
		return true;
	}

	// Char name | item ID | [stack size]
	if (vargs.size() < 2)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +goto X Y");
		return true;
	}

	uint32 X = atoi(vargs.front().c_str());
	vargs.pop_front();

	uint32 Y = atoi(vargs.front().c_str());
	vargs.pop_front();

	if (X < 0 || Y < 0)
	{
		g_pMain->SendHelpDescription(this, "Error : Invalid coordinate.");
		return true;
	}

	ZoneChange(GetZoneID(), (float)X, (float)Y);
	return true;
}
#pragma endregion

#pragma region CUser::HandleMonsterSummonCommand
COMMAND_HANDLER(CUser::HandleMonsterSummonCommand)
{
	if (!isGM())
		return false;

	if (m_GameMastersMonsterSummon != 1)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Gerekli Yetki Yok. Lütfen Yetki Sýnýrlandýrmasý için Yönetici ile konuþunuz.");
		return true;
	}

	if (vargs.empty())
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +monsummon MonsterSID Count");
		return true;
	}

	int sSid = 0;
	uint16 sCount = 1;

	if (vargs.size() == 1)
		sSid = atoi(vargs.front().c_str());

	if (vargs.size() == 2)
	{
		sSid = atoi(vargs.front().c_str());
		vargs.pop_front();
		sCount = atoi(vargs.front().c_str());
	}

	g_pMain->SpawnEventNpc(sSid, true, GetZoneID(), GetX(), GetY(), GetZ(), sCount);

	return true;
}
#pragma endregion



#pragma region CUser::HandleNPCSummonCommand
COMMAND_HANDLER(CUser::HandleNPCSummonCommand)
{
	if (!isGM())
		return false;

	if (m_GameMastersNpcSummon != 1)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Gerekli Yetki Yok. Lütfen Yetki Sýnýrlandýrmasý için Yönetici ile konuþunuz.");
		return true;
	}

	if (vargs.empty())
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +npcsummon NPCSID");
		return true;
	}

	int sSid = atoi(vargs.front().c_str());
	g_pMain->SpawnEventNpc(sSid, false, GetZoneID(), GetX(), GetY(), GetZ());

	return true;
}

#pragma endregion

#pragma region CUser::HandleMonKillCommand
COMMAND_HANDLER(CUser::HandleMonKillCommand)
{
	if (!isGM())
		return false;

	if (m_GameMastersMonKilled != 1)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Gerekli Yetki Yok. Lütfen Yetki Sýnýrlandýrmasý için Yönetici ile konuþunuz.");
		return true;
	}

	if (GetTargetID() == 0 && GetTargetID() < NPC_BAND)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : Select a NPC or Monster than use +monkills");
		return false;
	}

	CNpc *pNpc = g_pMain->GetNpcPtr(GetTargetID(), GetZoneID());

	if (pNpc != nullptr)
		g_pMain->KillNpc(GetTargetID(), GetZoneID(), this);

	return true;
}

#pragma endregion

#pragma region CUser::HandleTeleportAllCommand
COMMAND_HANDLER(CUser::HandleTeleportAllCommand)
{
	if (!isGM())
		return false;

	if (m_GameMastersTeleportAllUser != 1)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Gerekli Yetki Yok. Lütfen Yetki Sýnýrlandýrmasý için Yönetici ile konuþunuz.");
		return true;
	}

	// Zone number
	if (vargs.size() < 1)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +tp_all ZoneNumber | +tp_all ZoneNumber TargetZoneNumber");
		return true;
	}

	int nZoneID = 0;
	int nTargetZoneID = 0;

	if (vargs.size() == 1)
		nZoneID = atoi(vargs.front().c_str());

	if (vargs.size() == 2)
	{
		nZoneID = atoi(vargs.front().c_str());
		vargs.pop_front();
		nTargetZoneID = atoi(vargs.front().c_str());
	}

	if (nZoneID > 0 || nTargetZoneID > 0)
		g_pMain->KickOutZoneUsers(nZoneID, nTargetZoneID);
	return true;
}
#pragma endregion

#pragma region CUser::HandleKnightsSummonCommand
COMMAND_HANDLER(CUser::HandleKnightsSummonCommand)
{
	if (!isGM())
		return false;

	if (m_GameMastersClanSummon != 1)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Gerekli Yetki Yok. Lütfen Yetki Sýnýrlandýrmasý için Yönetici ile konuþunuz.");
		return true;
	}

	// Clan name
	if (vargs.empty())
	{
		// Send description
		g_pMain->SendHelpDescription(this, "Using Sample : +summonknights ClanName");
		return true;
	}

	CKnights * pKnights;
	foreach_stlmap(itr, g_pMain->m_KnightsArray)
	{
		if (itr->second->GetName() == vargs.front().c_str())
		{
			pKnights = g_pMain->GetClanPtr(itr->first);
			break;
		}
	}

	if (pKnights == nullptr)
		return true;
	{
		foreach_stlmap(i, pKnights->m_arKnightsUser)
		{
			_KNIGHTS_USER *p = i->second;
			if (p == nullptr)
				continue;

			CUser* pTUser = g_pMain->GetUserPtr(p->strUserName, TYPE_CHARACTER);
			if (pTUser == nullptr
				|| !pTUser->isInGame()
				|| pTUser->GetName() == GetName())
				continue;

			pTUser->ZoneChange(GetZoneID(), m_curx, m_curz);
			string helpstring = string_format("[%s] %s is teleported.", pKnights->GetName().c_str(), pTUser->GetName().c_str());
			g_pMain->SendHelpDescription(this, helpstring);
		}
	}

	return true;
}
#pragma endregion

#pragma region CUser::HandleResetPlayerRankingCommand
COMMAND_HANDLER(CUser::HandleResetPlayerRankingCommand)
{
	if (!isGM())
		return false;

	if (m_GameMastersResetPlayerRanking != 1)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Gerekli Yetki Yok. Lütfen Yetki Sýnýrlandýrmasý için Yönetici ile konuþunuz.");
		return true;
	}

	// Zone ID
	if (vargs.empty())
	{
		// Send description
		g_pMain->SendHelpDescription(this, "Using Sample : +resetranking");
		return true;
	}

	g_pMain->ResetPlayerRankings();

	return true;
}
#pragma endregion

#pragma region CUser::HandleChangeRoom
COMMAND_HANDLER(CUser::HandleChangeRoom)
{
	if (!isGM())
		return false;

	if (m_GameMastersChangeEventRoom != 1)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Gerekli Yetki Yok. Lütfen Yetki Sýnýrlandýrmasý için Yönetici ile konuþunuz.");
		return true;
	}

	// Room number
	if (vargs.size() < 2)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +changeroom ZoneId RoomNumber");
		return true;
	}

	uint16 ZoneID;
	uint16 Room;

	ZoneID = atoi(vargs.front().c_str());
	vargs.pop_front();
	Room = atoi(vargs.front().c_str());
	vargs.pop_front();

	if (ZoneID < 0
		|| Room < 0
		|| ZoneID > 255
		|| Room > 255)
	{
		g_pMain->SendHelpDescription(this, "Using Sample : Invalid Zone Or Invalid Room");
		return true;
	}

	if (ZoneID != 84 && ZoneID != 85 && ZoneID != 87)
	{
		g_pMain->SendHelpDescription(this, "Using Sample : Invalid Zone");
		return true;
	}

	bool RoomNumber = (Room >= 1 && Room <= 60);

	if (!RoomNumber)
	{
		g_pMain->SendHelpDescription(this, "Using Sample : Invalid Room");
		return true;
	}

	m_bEventRoom = Room;
	ZoneChange(ZoneID, 0.0f, 0.0f);
	return true;
}
#pragma endregion

#pragma region CUser::HandleSummonPrison
COMMAND_HANDLER(CUser::HandleSummonPrison)
{
	if (!isGM())
		return false;

	if (m_GameMastersSendPrsion != 1)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Gerekli Yetki Yok. Lütfen Yetki Sýnýrlandýrmasý için Yönetici ile konuþunuz.");
		return true;
	}

	// Char name | item ID | [stack size]
	if (vargs.size() < 1)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +sendtorpsion CharacterName ");
		return true;
	}

	std::string strUserID = vargs.front();
	vargs.pop_front();

	CUser *pUser = g_pMain->GetUserPtr(strUserID, TYPE_CHARACTER);
	if (pUser == nullptr)
	{
		g_pMain->SendHelpDescription(this, "Error : User is not online");
		return true;
	}

	pUser->ZoneChange(ZONE_PRISON, 170, 146);
	return true;
}