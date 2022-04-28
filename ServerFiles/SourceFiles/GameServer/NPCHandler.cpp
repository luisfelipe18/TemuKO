#include "stdafx.h"
#include "Map.h"
#include "KnightsManager.h"
#include "KingSystem.h"
using namespace std;
using std::string;

struct ITEMS
{
	 int ITEMID;
	 uint8 IPOS;
	 uint16 _ICOUNT;
}
pItems;

void CUser::ItemRepair(Packet & pkt)
{
	if (isDead())
		return;

	Packet result(WIZ_ITEM_REPAIR);
	uint32 MoneyID, ItemID;
	uint16 durability, quantity, sNpcID;
	_ITEM_TABLE* pTable = nullptr;
	CNpc *pNpc = nullptr;
	uint8 sPos, sSlot;

	pkt >> sPos >> sSlot >> sNpcID >> ItemID;
	if (sPos == 1 ) 
	{	// SLOT
		if (sSlot >= SLOT_MAX) 
			goto fail_return;

		if (m_sItemArray[sSlot].nNum != ItemID)
			goto fail_return;
	}
	else if (sPos == 2 ) 
	{	// INVEN
		if (sSlot >= HAVE_MAX) 
			goto fail_return;

		if (m_sItemArray[SLOT_MAX + sSlot].nNum != ItemID)
			goto fail_return;
	}

	pNpc = g_pMain->GetNpcPtr(sNpcID, GetZoneID());
	if (pNpc == nullptr || !isInRange(pNpc, MAX_NPC_RANGE))
		return;

	if (pNpc->GetType() == NPC_TINKER || pNpc->GetType() == NPC_MERCHANT)
	{
		pTable = g_pMain->GetItemPtr(ItemID);
		if (pTable == nullptr
			|| pTable->m_iSellPrice == SellTypeNoRepairs) 
			goto fail_return;

		durability = pTable->m_sDuration;
		if(durability == 1) 
			goto fail_return;

		if(sPos == 1)
			quantity = pTable->m_sDuration - m_sItemArray[sSlot].sDuration;
		else if(sPos == 2) 
			quantity = pTable->m_sDuration - m_sItemArray[SLOT_MAX + sSlot].sDuration;

		MoneyID = (unsigned int)((((pTable->m_iBuyPrice-10) / 10000.0f) + pow((float)pTable->m_iBuyPrice, 0.75f)) * quantity / (double)durability);

		if (GetPremiumProperty(PremiumRepairDiscountPercent) > 0)
			MoneyID = MoneyID * GetPremiumProperty(PremiumRepairDiscountPercent) / 100;

		if (GetClanPremiumProperty(PremiumRepairDiscountPercent) > 0)
			MoneyID = MoneyID * GetClanPremiumProperty(PremiumRepairDiscountPercent) / 100;

		if (!GoldLose(MoneyID, false))
			goto fail_return;

		if (sPos == 1)
			m_sItemArray[sSlot].sDuration = durability;
		else if(sPos == 2)
			m_sItemArray[SLOT_MAX + sSlot].sDuration = durability;

		SetUserAbility(true);
		result << uint8(1) << GetCoins();
		Send(&result);
		return;
	}
	
fail_return:
	result << uint8(0) << GetCoins();
	Send(&result);
}

void CUser::ClientEvent(uint16 sNpcID)
{
	// Ensure AI's loaded
	if (isDead())
		return;

	int32 iEventID = 0;
	CNpc *pNpc = g_pMain->GetNpcPtr(sNpcID, GetZoneID());
	if (pNpc == nullptr
		|| !isInRange(pNpc, MAX_NPC_RANGE))
		return;

	m_sEventNid = sNpcID;
	m_sEventSid = pNpc->GetProtoID(); // For convenience purposes with Lua scripts.

	switch (pNpc->GetProtoID())
	{
	case SAW_BLADE_SSID:
	{
		switch (GetZoneID())
		{
		case ZONE_CHAOS_DUNGEON:
			HpChange(-5000 / 10, pNpc);
			break;
		case ZONE_KROWAZ_DOMINION:
			HpChange(-370, pNpc);
			break;
		default:
			break;
		}
		return;
	}break;
	case CHAOS_CUBE_SSID:
	{
		if (!pNpc->isDead())
		{
			uint32 nItemID = 0;
			srand((unsigned int)time(NULL));

			int dice_roll = rand() % 4 + 1;
			switch (dice_roll)
			{
			case 1:
				nItemID = ITEM_KILLING_BLADE;
				break;
			case 2:
				nItemID = ITEM_LIGHT_PIT;
				break;
			case 3:
				nItemID = ITEM_DRAIN_RESTORE;
				break;
			case 4:
				nItemID = ITEM_DRAIN_RESTORE;
				break;
			default:
				break;
			}
			GiveItem(nItemID);
			g_pMain->ShowNpcEffect(GetSocketID(), 251, GetZoneID(), GetEventRoom());
			g_pMain->KillNpc(sNpcID, GetZoneID());
			return;
		}
	}break;
	default:
		break;
	}

	switch (pNpc->GetType())
	{
	case NPC_KISS:
	{
		KissUser();
		return;
	}break;
	case NPC_ROLLINGSTONE:
	{
		HpChange(-GetMaxHealth(), pNpc);
		return;
	}break;
	case NPC_OBJECT_WOOD:
	{
		int32 testmaxhp = GetMaxHealth();
		if (testmaxhp != 0)
		{
			int32 RemainingHp = ((testmaxhp * 80) / 100);
			HpChange(-RemainingHp, pNpc);
		}
		return;
	}break;
	default:
		break;
	}

	Guard lock(g_pMain->m_questNpcLock);
	QuestNpcList::iterator itr = g_pMain->m_QuestNpcList.find(pNpc->GetProtoID());
	if (itr == g_pMain->m_QuestNpcList.end())
		return;

	QuestHelperList & pList = itr->second;
	_QUEST_HELPER * pHelper = nullptr;

	foreach(itr, pList)
	{
		if ((*itr) == nullptr
			|| (*itr)->sEventDataIndex
			|| (*itr)->bEventStatus
			|| ((*itr)->bNation != 3 && (*itr)->bNation != GetNation())
			|| ((isRogue() || isMage() || isPriest() || isWarrior()) && ((*itr)->bClass != 5 && !JobGroupCheck((*itr)->bClass)))
			|| (isPortuKurian() && !JobGroupCheck(ClassPortuKurian)))
			continue;

		pHelper = (*itr);
		break;
	}

	if (pHelper == nullptr)
		return;

	QuestV2RunEvent(pHelper, pHelper->nEventTriggerIndex, -1);
}

void CUser::KissUser()
{
	Packet result(WIZ_KISS);
	result << uint32(GetID()) << m_sEventNid;
	GiveItem(910014000); // aw, you got a 'Kiss'. How literal.
	
	if (GetEventRoom() > 0)
		SendToRegion(&result, nullptr, GetEventRoom());
	else
		SendToRegion(&result);
}

#pragma region CUser::ClassChange
void CUser::ClassChange(Packet & pkt, bool bFromClient /*= true */)
{
	Packet result(WIZ_CLASS_CHANGE);
	bool bSuccess = false;
	uint8 opcode = pkt.read<uint8>();

	if (opcode == CLASS_CHANGE_REQ)	
	{
		ClassChangeReq();
		return;
	}
	else if (opcode == ALL_POINT_CHANGE)	
	{
		AllPointChange(false);
		return;
	}
	else if (opcode == ALL_SKILLPT_CHANGE)	
	{
		AllSkillPointChange(false);
		return;
	}
	else if (opcode == CHANGE_MONEY_REQ)
	{
		uint8 sub_type = pkt.read<uint8>(); // type is irrelevant

		if (sub_type == 1)
		{
			if (GetPremium() == WAR_Premium || GetPremium() == Royal_Premium || CheckExistItem(RETURNTOKENS))
			{
				AllPointChange(true);
				return;
			}
			else
			{
				int money;

				money = (int)pow((GetLevel() * 2.0f), 3.4f);

				if (GetLevel() < 30)
					money = (int)(money * 0.4f);
				else if (GetLevel() >= 60)
					money = (int)(money * 1.5f);

				result << uint8(CHANGE_MONEY_REQ) << money;
				Send(&result);
				return;
			}
		}
		else if (sub_type == 2)
		{
			if (GetPremium() == WAR_Premium || GetPremium() == Royal_Premium || CheckExistItem(RETURNTOKENS))
			{
				AllSkillPointChange(true);
				return;
			}
			else
			{
				int money;

				money = (int)pow((GetLevel() * 2.0f), 3.4f);

				if (GetLevel() < 30)
					money = (int)(money * 0.4f);
				else if (GetLevel() >= 60)
					money = (int)(money * 1.5f);

				money = (int)(money * 1.5f);
				result << uint8(CHANGE_MONEY_REQ) << money;
				Send(&result);
				return;
			}
		}
	}
	else if (opcode == REB_STAT_CHANGE)
	{
		if (GetRebirthLevel() > 9)
			return;

		Packet result(WIZ_CLASS_CHANGE, uint8(REB_STAT_CHANGE));
		uint8 RecStr, RecSta, RecDex, RecInt, RecCha;

		uint8 rStr = GetRebStatBuff(STAT_STR),
			rSta = GetRebStatBuff(STAT_STA),
			rDex = GetRebStatBuff(STAT_DEX),
			rInt = GetRebStatBuff(STAT_INT),
			rCha = GetRebStatBuff(STAT_CHA);

		pkt >> RecStr >> RecSta >> RecDex >> RecInt >> RecCha;

		if (RecStr + RecSta + RecDex + RecInt + RecCha < 2
			|| RecStr + RecSta + RecDex + RecInt + RecCha > 2)
		{
			result << uint8(0) << uint32(0);
			Send(&result);
			return;
		}

		if (!CheckExistItem(QUALIFICATION_OF_REBITH, 1))
		{
			result << uint8(0) << uint32(0);
			Send(&result);
			return;
		}

		SetRebStatBuff(STAT_STR, rStr += RecStr);
		SetRebStatBuff(STAT_STA, rSta += RecSta);
		SetRebStatBuff(STAT_DEX, rDex += RecDex);
		SetRebStatBuff(STAT_INT, rInt += RecInt);
		SetRebStatBuff(STAT_CHA, rCha += RecCha);

		m_bRebirthLevel++;
		m_iExp = 0;

		if (m_bRebirthLevel < 9)
		{
			QuestV2RemoveEvent(1119);
			QuestV2RemoveEvent(1120);
			QuestV2RemoveEvent(1121);
			QuestV2RemoveEvent(1122);
		}

		result << uint8(1) << uint32(0);
		Send(&result);
		RobItem(QUALIFICATION_OF_REBITH, 1);
		Disconnect();
		return;
	}
	else if (opcode == REB_STAT_RESET)
	{
		if (GetRebirthLevel() == 0)
			return;

		bool isHaveWARPre = false;

		foreach_stlmap(itr, m_PremiumMap)
		{
			_PREMIUM_DATA * pPremium = itr->second;
			if (pPremium == nullptr
				|| pPremium->iPremiumTime == 0)
				continue;

			if (pPremium->bPremiumType == 12 || pPremium->bPremiumType == 13)
				isHaveWARPre = true;
		}

		if (!hasCoins(100000000) && !isHaveWARPre)
			return;

		Packet result(WIZ_CLASS_CHANGE, uint8(REB_STAT_RESET));
		uint8 RecStr, RecSta, RecDex, RecInt, RecCha;
		uint8 rStr = GetRebStatBuff(STAT_STR), rSta = GetRebStatBuff(STAT_STA), rDex = GetRebStatBuff(STAT_DEX), rInt = GetRebStatBuff(STAT_INT), rCha = GetRebStatBuff(STAT_CHA);
		pkt >> RecStr >> RecSta >> RecDex >> RecInt >> RecCha;
		uint8 bTotalPoint = rStr + rSta + rDex + rInt + rCha;
		uint8 reqTotalPoint = RecStr + RecSta + RecDex + RecInt + RecCha;

		if (bTotalPoint != reqTotalPoint || reqTotalPoint != GetRebirthLevel() * 2)
		{
			result << uint8(0) << uint32(0);
			Send(&result);
			return;
		}

		SetRebStatBuff(STAT_STR, RecStr);
		SetRebStatBuff(STAT_STA, RecSta);
		SetRebStatBuff(STAT_DEX, RecDex);
		SetRebStatBuff(STAT_INT, RecInt);
		SetRebStatBuff(STAT_CHA, RecCha);

		result << uint8(1) << uint32(0);
		Send(&result);

		if (isHaveWARPre == false)
			GoldLose(100000000, true);

		Disconnect();
		return;
	}

	// If this packet was sent from the client, ignore it.
	else if (bFromClient)
		return;

	uint8 classcode = pkt.read<uint8>();
	switch (m_sClass)
	{
	case KARUWARRIOR:
		if (classcode == BERSERKER || classcode == GUARDIAN)
			bSuccess = true;
		break;
	case KARUROGUE:
		if (classcode == HUNTER || classcode == PENETRATOR)
			bSuccess = true;
		break;
	case KARUWIZARD:
		if (classcode == SORSERER || classcode == NECROMANCER)
			bSuccess = true;
		break;
	case KARUPRIEST:
		if (classcode == SHAMAN || classcode == DARKPRIEST)
			bSuccess = true;
		break;
	case KURIANSTARTER:
		if (classcode == KURIANMASTER || classcode == KURIANNOVICE)
			bSuccess = true;
		break;
	case ELMORWARRRIOR:
		if (classcode == BLADE || classcode == PROTECTOR)
			bSuccess = true;
		break;
	case ELMOROGUE:
		if (classcode == RANGER || classcode == ASSASSIN)
			bSuccess = true;
		break;
	case ELMOWIZARD:
		if (classcode == MAGE || classcode == ENCHANTER)
			bSuccess = true;
		break;
	case ELMOPRIEST:
		if (classcode == CLERIC || classcode == DRUID)
			bSuccess = true;
		break;
	case BERSERKER:
		if (classcode == GUARDIAN)
			bSuccess = true;
		break;
	case HUNTER:
		if (classcode == PENETRATOR)
			bSuccess = true;
		break;
	case SORSERER:
		if (classcode == NECROMANCER)
			bSuccess = true;
		break;
	case SHAMAN:
		if (classcode == DARKPRIEST)
			bSuccess = true;
		break;
	case BLADE:
		if (classcode == PROTECTOR)
			bSuccess = true;
		break;
	case RANGER:
		if (classcode == ASSASSIN)
			bSuccess = true;
		break;
	case MAGE:
		if (classcode == ENCHANTER)
			bSuccess = true;
		break;
	case CLERIC:
		if (classcode == DRUID)
			bSuccess = true;
		break;
	case PORUTUSTARTER:
		if (classcode == PORUTUMASTER || classcode == PORUTUNOVICE)
			bSuccess = true;
		break;
	}

	// Not allowed this job change
	if (!bSuccess)
	{
		result << uint8(CLASS_CHANGE_RESULT) << uint8(0);
		Send(&result);
		return;
	}

	m_sClass = classcode;
	if (isInParty())
	{
		// TO-DO: Move this somewhere better.
		result.SetOpcode(WIZ_PARTY);
		result << uint8(PARTY_CLASSCHANGE) << GetSocketID() << uint16(classcode);
		g_pMain->Send_PartyMember(GetPartyID(), &result);
	}
}
#pragma endregion

void CUser::RecvMapEvent(Packet & pkt)
{
	uint8 bType = pkt.read<uint8>();
	
	Packet result;
	result.Initialize(WIZ_MAP_EVENT);

	if (g_pMain->isWarOpen() && GetZoneID() == ZONE_BATTLE4)
		result << bType << g_pMain->m_sKarusMonumentPoint << g_pMain->m_sElmoMonumentPoint;

	if (g_pMain->isWarOpen() && GetZoneID() == ZONE_BATTLE6)
		result << bType << g_pMain->m_sKarusDead << g_pMain->m_sElmoradDead;

	Send(&result);
}

void CUser::RecvSelectMsg(Packet & pkt)	// Receive menu reply from client.
{
	// 55 00 0F 31 36 30 34 37 5F 4D 6F 69 72 61 2E 6C 75 61 FF
	uint8 bMenuID = pkt.read<uint8>();
	string szLuaFilename;
	int8 bySelectedReward = -1;
	pkt.SByte();
	pkt >> szLuaFilename >> bySelectedReward;

	if (!AttemptSelectMsg(bMenuID, bySelectedReward))
		memset(&m_iSelMsgEvent, -1, sizeof(m_iSelMsgEvent));
}

bool CUser::AttemptSelectMsg(uint8 bMenuID, int8 bySelectedReward)
{
	_QUEST_HELPER * pHelper = nullptr;
	if (bMenuID >= MAX_MESSAGE_EVENT
		|| isDead()
		|| m_nQuestHelperID == 0)
		return false;

	if ((pHelper = g_pMain->m_QuestHelperArray.GetData(m_nQuestHelperID)) == nullptr)
		return false;

	_ITEM_EXCHANGE_EXP * pExchangeExp = g_pMain->m_ItemExchangeExpArray.GetData(pHelper->nExchangeIndex);
	if(pExchangeExp && bySelectedReward == -1)
	{
		bySelectedReward = bMenuID;
		bMenuID = 0;
	}
	else if(bySelectedReward == -1 && m_bSelectMsgFlag == 5)
	{
		bySelectedReward = bMenuID;
		bMenuID = 0;
	}

	// Get the event number that needs to be processed next.
	int32 selectedEvent = m_iSelMsgEvent[bMenuID];
	if (selectedEvent < 0)
		return false;

	if(!QuestV2RunEvent(pHelper, selectedEvent, bySelectedReward))
		return false;

	return true;
}

void CUser::SendSay(int32 nTextID[8])
{
	Packet result(WIZ_NPC_SAY);
	result << int32(-1) << int32(-1);
	foreach_array_n(i, nTextID, 8)
		result << nTextID[i];
	Send(&result);
}

void CUser::SelectMsg(uint8 bFlag, int32 nQuestID, int32 menuHeaderText, int32 menuButtonText[MAX_MESSAGE_EVENT], int32 menuButtonEvents[MAX_MESSAGE_EVENT])
{
	_QUEST_HELPER * pHelper = g_pMain->m_QuestHelperArray.GetData(m_nQuestHelperID);
	if (pHelper == nullptr)
		return;

	// Send the menu to the client
	Packet result(WIZ_SELECT_MSG);
	result.SByte();

	result << m_sEventSid << bFlag << nQuestID << menuHeaderText;
	foreach_array_n(i, menuButtonText, MAX_MESSAGE_EVENT)
	{
		//if(menuButtonText[i] >= 0)
		result << menuButtonText[i];
	}
	result << pHelper->strLuaFilename;
	Send(&result);

	// and store the corresponding event IDs.
	m_bSelectMsgFlag = bFlag;
	memcpy(&m_iSelMsgEvent, menuButtonEvents, sizeof(int32) * MAX_MESSAGE_EVENT);
}

void CUser::NpcEvent(Packet & pkt)
{
	// Ensure AI is loaded first
	if (isDead())
		return;

	if (isDead() 
		|| isTrading() 
		|| isMerchanting() 
		|| isStoreOpen() 
		|| isSellingMerchant() 
		|| isBuyingMerchant() 
		|| isMining()
		|| isFishing())
		return;

	Packet result;
	uint8 bUnknown = pkt.read<uint8>();
	uint16 sNpcID = pkt.read<uint16>();
	int32 nQuestID = pkt.read<int32>();

	CNpc *pNpc = g_pMain->GetNpcPtr(sNpcID, GetZoneID());
	if (pNpc == nullptr
		|| !isInRange(pNpc, MAX_NPC_RANGE))
		return;

	switch (pNpc->GetType())
	{
	case NPC_LOYALTY_MERCHANT:
		result.SetOpcode(WIZ_TRADE_NPC);
		result << pNpc->m_iSellingGroup;
		Send(&result);
		break;
	case NPC_MERCHANT:
	case NPC_TINKER:
		if (pNpc->m_iSellingGroup == 281000)
		{
			if (GetPremium() != 8)
			{
				std::string m_sAutosystem;
				m_sAutosystem = string_format("Press the character Royal Premium!");
				result.clear();
				result.Initialize(WIZ_HOOK_GUARD);
				result << uint8(WIZ_HOOK_INFOMESSAGE);
				result << m_sAutosystem;
				Send(&result);
				return;
			}
		}
		result.SetOpcode(pNpc->GetType() == NPC_MERCHANT ? WIZ_TRADE_NPC : WIZ_REPAIR_NPC);
		result << pNpc->m_iSellingGroup;
		Send(&result);
		break;

	case NPC_MARK:
		result.SetOpcode(WIZ_KNIGHTS_PROCESS);
		result << uint8(KNIGHTS_CAPE_NPC);
		Send(&result);
		break;

	case NPC_RENTAL:
		result.SetOpcode(WIZ_RENTAL);
		result	<< uint8(RENTAL_NPC) 
			<< uint16(1) // 1 = enabled, -1 = disabled 
			<< pNpc->m_iSellingGroup;
		Send(&result);
		break;

	case NPC_ELECTION:
	case NPC_TREASURY:
		{
			CKingSystem * pKingSystem = g_pMain->m_KingSystemArray.GetData(GetNation());
			result.SetOpcode(WIZ_KING);
			if (pNpc->GetType() == NPC_ELECTION)
			{
				// Ensure this still works as per official without a row in the table.
				string strKingName = (pKingSystem == nullptr ? "" : pKingSystem->m_strKingName);
				result.SByte();
				result	<< uint8(KING_NPC) << strKingName;
			}
			else
			{
				// Ensure this still works as per official without a row in the table.
				uint32 nTribute = (pKingSystem == nullptr ? 0 : pKingSystem->m_nTribute + pKingSystem->m_nTerritoryTax);
				uint32 nTreasury = (pKingSystem == nullptr ? 0 : pKingSystem->m_nNationalTreasury);
				if(isKing())
				{
					result	<< uint8(KING_TAX) << uint8(1) // success
						<< uint16(1) // 1 enables king-specific stuff (e.g. scepter), 2 is normal user stuff
						<< nTribute << nTreasury;
				}
				else
				{
					result	<< uint8(KING_TAX) << uint8(1) // success
						<< uint16(2) // 1 enables king-specific stuff (e.g. scepter), 2 is normal user stuff
						<< nTreasury << uint32(0);
				}
			}
			Send(&result);
		} break;

	case NPC_SIEGE:
		{
			result.SetOpcode(WIZ_SIEGE);
			result << uint8(3) << uint8(7);
			Send(&result);
		}break;

	case NPC_SIEGE_1:
		{
			_KNIGHTS_SIEGE_WARFARE *pKnightSiegeWarFare = g_pMain->GetSiegeMasterKnightsPtr(Aktive);

			if (pKnightSiegeWarFare == nullptr)
				return;

			if (isKing())
			{
				if (isKing() && pKnightSiegeWarFare->sMasterKnights == GetClanID())
					return;

				result.SetOpcode(WIZ_SIEGE);
				result << uint8(4) << uint8(1)
					<< pKnightSiegeWarFare->nMoradonTax + pKnightSiegeWarFare->nDellosTax;
				Send(&result);
			}
			else
			{
				if (isInClan())
				{
					if (pKnightSiegeWarFare->sMasterKnights != GetClanID())
						return;

					if (pKnightSiegeWarFare->sMasterKnights > NO_CLAN
						&& pKnightSiegeWarFare->sMasterKnights == GetClanID()
						&& isClanLeader())
					{
						result.SetOpcode(WIZ_SIEGE);
						result << uint8(4) << uint8(1)

							<< pKnightSiegeWarFare->nDungeonCharge
							<< pKnightSiegeWarFare->nMoradonTax
							<< pKnightSiegeWarFare->nDellosTax;
						Send(&result);
					}
				}
			}
		}break;

	case NPC_VICTORY_GATE:
		switch(GetWarVictory())
		{
		case KARUS:
			if(GetNation() == KARUS)
				ZoneChange(2,222,1846);
			   break;
		case ELMORAD:
			if(GetNation() == ELMORAD)
				ZoneChange(1,1865,168);
			   break;
		}break;

	case NPC_CAPTAIN:
		result.SetOpcode(WIZ_CLASS_CHANGE);
		result << uint8(CLASS_CHANGE_REQ);
		Send(&result);
		break;

	case NPC_CLAN_BANK:
	case NPC_WAREHOUSE:
		result.SetOpcode(WIZ_WAREHOUSE);
		result << uint8(WAREHOUSE_REQ);
		Send(&result);
		break;

	case NPC_CHAOTIC_GENERATOR:
	case NPC_CHAOTIC_GENERATOR2:
		SendAnvilRequest(sNpcID, ITEM_BIFROST_REQ);
		break;
#if 0
	case NPC_BORDER_MONUMENT:
		{
			_BDW_ROOM_INFO* pRoomInfo = g_pMain->m_TempleEventBDWRoomList.GetData(GetEventRoom());
			if (pRoomInfo == nullptr)
				return;

			if (pRoomInfo->m_bMonumentHolderNation == GetNation())
				return;

			result.Initialize(WIZ_QUEST);
			result << uint8(CAPURE_TIME) << uint32(9993);
			Send(&result);

			result.clear();

			result.Initialize(WIZ_CAPTURE);
			result << uint8(CAPURE_RIGHT_CLICK) << GetSocketID() << GetName();
			Send(&result);
		}break;
#endif
	case NPC_CLAN: // this HAS to go.
		result << uint16(0); // page 0
		CKnightsManager::KnightsAllList(this, result);
	default:
		ClientEvent(sNpcID);
	}
}

// NPC Shop
void CUser::ItemTrade(Packet & pkt)
{
	Packet result(WIZ_ITEM_TRADE);
	uint32 transactionPrice;
	int group = 0;
	uint16 npcid;
	uint16 line;
	_ITEM_TABLE* pTable = nullptr;
	_ITEM_SELLTABLE *pSellTable = nullptr;	
	CNpc* pNpc = nullptr;
	uint8 type, errorCode = 1,purchased_item_count;
	bool bSuccess = true;
	std::vector<ITEMS> pItems(0);
	DateTime time;

	if (isDead()
		|| isTrading()
		|| isMining()
		|| isFishing()
		|| isMerchanting()
		|| isSellingMerchantingPreparing()
		|| isBuyingMerchantingPreparing())
	{
		errorCode = 1;
		goto fail_return;
	}

	pkt >> type;	
	// Buy == 1, Sell == 2
	if (type == 1 || type == 2)
	{
		pkt >> group >> npcid;
		if ((pNpc = g_pMain->GetNpcPtr(npcid, GetZoneID())) == nullptr
			|| (pNpc->GetType() != NPC_MERCHANT
				&& pNpc->GetType() != NPC_TINKER
				&& pNpc->GetType() != NPC_LOYALTY_MERCHANT
				&& pNpc->GetType() != NPC_PET_TRADE)
			|| pNpc->m_iSellingGroup != group
			|| !isInRange(pNpc, MAX_NPC_RANGE))
			goto fail_return;
	}
	else if (type == 5)
	{
		ItemTradeRepurchase(pkt);
		return;
	}
	else 
		goto fail_return;

	pkt >> purchased_item_count; 
	if (type == 1)
	{
		for (int i = 0; i < purchased_item_count; i++)
		{
			ITEMS iItems;
			pkt >> iItems.ITEMID;
			pkt >> iItems.IPOS;
			pkt >> iItems._ICOUNT;
			pkt >> line;  // WtF ?
			pItems.resize(i+1,iItems);
		}
	}
	else
	{
		for (int i = 0; i < purchased_item_count; i++)
		{
			ITEMS iItems;
			pkt >> iItems.ITEMID;
			pkt >> iItems.IPOS;
			pkt >> iItems._ICOUNT;
			pItems.resize(i+1,iItems);
		}
	}
	uint32 real_price = 0;
	uint32 total_price = 0;
	// Buying from an NPC Gold
	if (type == 1 && pNpc->m_iSellingGroup != 249000)
	{	
		for (int i = 0; i < purchased_item_count; i++)
		{	
			transactionPrice = 0;
			if(pItems[i].ITEMID != 0)
			{
				if (isTrading()
					|| (pTable = g_pMain->GetItemPtr(pItems[i].ITEMID)) == nullptr
					|| (pSellTable = g_pMain->m_ItemSellTableArray.GetData(pNpc->m_iSellingGroup)) == nullptr
					|| (type == 2 
					&& (pItems[i].ITEMID >= ITEM_NO_TRADE_MIN && pItems[i].ITEMID <= ITEM_NO_TRADE_MAX // Cannot be traded, sold or stored.
					|| pTable->m_bRace == RACE_UNTRADEABLE))) 
					goto fail_return;

				if (pItems[i].IPOS >= HAVE_MAX
					|| pItems[i]._ICOUNT <= 0 || pItems[i]._ICOUNT > MAX_ITEM_COUNT)
				{
					errorCode = 2;
					goto fail_return;
				}

				if (i == 0) 
				{
					_ITEM_TABLE *prTable;
					for (int j = 0; j < purchased_item_count; j++)
					{
						prTable = nullptr;
						prTable = g_pMain->GetItemPtr(pItems[j].ITEMID);

						if(prTable == nullptr)
							continue;

						total_price += ((uint32)prTable->m_iBuyPrice * pItems[j]._ICOUNT);
					}

					if(!hasCoins(total_price)) 
					{
						errorCode = 3;
						total_price = 0;
						goto fail_return;
					}
				}

				if (pItems[i].ITEMID != 0)
				{
					if (m_sItemArray[SLOT_MAX+pItems[i].IPOS].nNum != 0)
					{
						if (m_sItemArray[SLOT_MAX+pItems[i].IPOS].nNum != pItems[i].ITEMID)
						{
							errorCode = 2;
							goto fail_return;
						}

						if (!pTable->m_bCountable || pItems[i]._ICOUNT <= 0)
						{
							errorCode = 2;
							goto fail_return;
						}

						if (pTable->m_bCountable 
							&& (pItems[i]._ICOUNT + m_sItemArray[SLOT_MAX+pItems[i].IPOS].sCount) > MAX_ITEM_COUNT)
						{
							errorCode = 4;
							goto fail_return;				
						}
					}

					switch (pItems[i].ITEMID)
					{
					case 379068000:
					case 379107000:
					case 379109000:
					case 379110000:
					case 379067000:
						transactionPrice = ((uint32)pTable->m_iBuyPrice * pItems[i]._ICOUNT); 
						break;
					default:
						{
							transactionPrice = ((uint32)pTable->m_iBuyPrice * pItems[i]._ICOUNT); 

							C3DMap * pMap = g_pMain->GetZoneByID(GetZoneID());
							if (pMap != nullptr)
							{
								_KNIGHTS_SIEGE_WARFARE *pSiegeWarFare = g_pMain->GetSiegeMasterKnightsPtr(Aktive);
								CKingSystem * pKingSystem = g_pMain->m_KingSystemArray.GetData(GetNation());

								uint32 tax = 0, tax0 = 0, tax1 = 0, tax2 = 0, tax3 = 0, tax4 = 0, tax5 = 0, tax6 = 0, tax7 = 0, tax8 = 0, tax9 = 0, tax10 = 0;

								switch (GetZoneID())
								{
									case ZONE_KARUS:
									case ZONE_KARUS2:
									case ZONE_KARUS3:
									case ZONE_ELMORAD:
									case ZONE_ELMORAD2:
									case ZONE_ELMORAD3:
									case ZONE_KARUS_ESLANT:
									case ZONE_KARUS_ESLANT2:
									case ZONE_KARUS_ESLANT3:
									case ZONE_ELMORAD_ESLANT:
									case ZONE_ELMORAD_ESLANT2:
									case ZONE_ELMORAD_ESLANT3:
									case ZONE_BIFROST:
									case ZONE_BATTLE:
									case ZONE_BATTLE2:
									case ZONE_BATTLE3:
									case ZONE_BATTLE4:
									case ZONE_BATTLE5:
									case ZONE_BATTLE6:
									case ZONE_SNOW_BATTLE:
									case ZONE_RONARK_LAND:
									case ZONE_ARDREAM:
									case ZONE_RONARK_LAND_BASE:
									case ZONE_KROWAZ_DOMINION:
									case ZONE_STONE1:
									case ZONE_STONE2:
									case ZONE_STONE3:
									case ZONE_BORDER_DEFENSE_WAR:
									case ZONE_UNDER_CASTLE:
									case ZONE_JURAID_MOUNTAIN:
									case ZONE_PARTY_VS_1:
									case ZONE_PARTY_VS_2:
									case ZONE_PARTY_VS_3:
									case ZONE_PARTY_VS_4:
									case ZONE_DRAKI_TOWER:
									case ZONE_DUNGEON_DEFENCE:
									case ZONE_OLD_MORADON:
									case ZONE_CLAN_WAR_ARDREAM:
									case ZONE_CLAN_WAR_RONARK:
									case ZONE_OLD_KARUS:
									case ZONE_OLD_HUMAN:
									case ZONE_NEW_BATTLE_TEST:

										if (pKingSystem != nullptr)
										{
											tax = (transactionPrice * pKingSystem->m_nTerritoryTariff) / 100;
											transactionPrice += tax;
										}
										break;
									case ZONE_MORADON:
									case ZONE_MORADON2:
									case ZONE_MORADON3:
									case ZONE_MORADON4:
									case ZONE_MORADON5:
									case ZONE_ARENA:
										{
											tax0 =(transactionPrice * 0) / 100; 
											tax1 =(transactionPrice * 10) / 100; 
											tax2 =(transactionPrice * 9) / 100; 
											tax3 =(transactionPrice * 8) / 100; 
											tax4 =(transactionPrice * 7) / 100; 
											tax5 =(transactionPrice * 6) / 100; 
											tax6 =(transactionPrice * 5) / 100; 
											tax7 =(transactionPrice * 4) / 100; 
											tax8 =(transactionPrice * 3) / 100; 
											tax9 =(transactionPrice * 2) / 100; 
											tax10 =(transactionPrice * 1) / 100; 
											if (pSiegeWarFare != nullptr)
											{
												if (pSiegeWarFare->sMoradonTariff == 0)
													transactionPrice -= tax1;
												else if (pSiegeWarFare->sMoradonTariff == 1)
													transactionPrice -= tax2;
												else if (pSiegeWarFare->sMoradonTariff == 2)
													transactionPrice -= tax3;
												else if (pSiegeWarFare->sMoradonTariff == 3)
													transactionPrice -= tax4;
												else if (pSiegeWarFare->sMoradonTariff == 4)
													transactionPrice -= tax5;
												else if (pSiegeWarFare->sMoradonTariff == 5)
													transactionPrice -= tax6;
												else if (pSiegeWarFare->sMoradonTariff == 6)
													transactionPrice -= tax7;
												else if (pSiegeWarFare->sMoradonTariff == 7)
													transactionPrice -= tax8;
												else if (pSiegeWarFare->sMoradonTariff == 8)
													transactionPrice -= tax9;
												else if (pSiegeWarFare->sMoradonTariff == 9)
													transactionPrice -= tax10;
												else if (pSiegeWarFare->sMoradonTariff == 10)
													transactionPrice -= tax0;
												else if (pSiegeWarFare->sMoradonTariff == 11)
													transactionPrice += tax10;
												else if (pSiegeWarFare->sMoradonTariff == 12)
													transactionPrice += tax9;
												else if (pSiegeWarFare->sMoradonTariff == 13)
													transactionPrice += tax8;
												else if (pSiegeWarFare->sMoradonTariff == 14)
													transactionPrice += tax7;
												else if (pSiegeWarFare->sMoradonTariff == 15)
													transactionPrice += tax6;
												else if (pSiegeWarFare->sMoradonTariff == 16)
													transactionPrice += tax5;
												else if (pSiegeWarFare->sMoradonTariff == 17)
													transactionPrice += tax4;
												else if (pSiegeWarFare->sMoradonTariff == 18)
													transactionPrice += tax3;
												else if (pSiegeWarFare->sMoradonTariff == 19)
													transactionPrice += tax2;
												else if (pSiegeWarFare->sMoradonTariff == 20)
													transactionPrice += tax1;
											}
										}
										break;
									case ZONE_DELOS:
									case ZONE_DESPERATION_ABYSS:
									case ZONE_HELL_ABYSS:
									case ZONE_DELOS_CASTELLAN:
										{
											tax0 =(transactionPrice * 0) / 100; 
											tax1 =(transactionPrice * 10) / 100; 
											tax2 =(transactionPrice * 9) / 100; 
											tax3 =(transactionPrice * 8) / 100; 
											tax4 =(transactionPrice * 7) / 100; 
											tax5 =(transactionPrice * 6) / 100; 
											tax6 =(transactionPrice * 5) / 100; 
											tax7 =(transactionPrice * 4) / 100; 
											tax8 =(transactionPrice * 3) / 100; 
											tax9 =(transactionPrice * 2) / 100; 
											tax10 =(transactionPrice * 1) / 100; 
											if (pSiegeWarFare != nullptr)
											{
												if (pSiegeWarFare->sDellosTariff == 0)
													transactionPrice -= tax1;
												else if (pSiegeWarFare->sDellosTariff == 1)
													transactionPrice -= tax2;
												else if (pSiegeWarFare->sDellosTariff == 2)
													transactionPrice -= tax3;
												else if (pSiegeWarFare->sDellosTariff == 3)
													transactionPrice -= tax4;
												else if (pSiegeWarFare->sDellosTariff == 4)
													transactionPrice -= tax5;
												else if (pSiegeWarFare->sDellosTariff == 5)
													transactionPrice -= tax6;
												else if (pSiegeWarFare->sDellosTariff == 6)
													transactionPrice -= tax7;
												else if (pSiegeWarFare->sDellosTariff == 7)
													transactionPrice -= tax8;
												else if (pSiegeWarFare->sDellosTariff == 8)
													transactionPrice -= tax9;
												else if (pSiegeWarFare->sDellosTariff == 9)
													transactionPrice -= tax10;
												else if (pSiegeWarFare->sDellosTariff == 10)
													transactionPrice -= tax0;
												else if (pSiegeWarFare->sDellosTariff == 11)
													transactionPrice += tax10;
												else if (pSiegeWarFare->sDellosTariff == 12)
													transactionPrice += tax9;
												else if (pSiegeWarFare->sDellosTariff == 13)
													transactionPrice += tax8;
												else if (pSiegeWarFare->sDellosTariff == 14)
													transactionPrice += tax7;
												else if (pSiegeWarFare->sDellosTariff == 15)
													transactionPrice += tax6;
												else if (pSiegeWarFare->sDellosTariff == 16)
													transactionPrice += tax5;
												else if (pSiegeWarFare->sDellosTariff == 17)
													transactionPrice += tax4;
												else if (pSiegeWarFare->sDellosTariff == 18)
													transactionPrice += tax3;
												else if (pSiegeWarFare->sDellosTariff == 19)
													transactionPrice += tax2;
												else if (pSiegeWarFare->sDellosTariff == 20)
													transactionPrice += tax1;
											}
										}
										break;
									default:
										transactionPrice = ((uint32)pTable->m_iBuyPrice * pItems[i]._ICOUNT);
									break;
								}

								if (pSiegeWarFare != nullptr && pSiegeWarFare->sMasterKnights > 0)
								{
									uint32  tax00 = 0,
										tax01 = 0,
										tax02 = 0,
										tax03 = 0,
										tax04 = 0,
										tax05 = 0,
										tax06 = 0,
										tax07 = 0,
										tax08 = 0,
										tax09 = 0,
										tax010 = 0;

									uint32 transactionPrice2;
									transactionPrice2 = ((uint32)pTable->m_iBuyPrice * pItems[i]._ICOUNT);

									if (isInMoradon())
									{
										tax00 = (transactionPrice2 * 0) / 100;
										tax01 = (transactionPrice2 * 10) / 100;
										tax02 = (transactionPrice2 * 9) / 100;
										tax03 = (transactionPrice2 * 8) / 100;
										tax04 = (transactionPrice2 * 7) / 100;
										tax05 = (transactionPrice2 * 6) / 100;
										tax06 = (transactionPrice2 * 5) / 100;
										tax07 = (transactionPrice2 * 4) / 100;
										tax08 = (transactionPrice2 * 3) / 100;
										tax09 = (transactionPrice2 * 2) / 100;
										tax010 = (transactionPrice2 * 1) / 100;

										if (pSiegeWarFare->sMoradonTariff == 11)
										{
											uint32 nDungeonChargeTax = (tax010 * 80) / 100;
											uint32 nMaradonTax = (tax010 * 10) / 100;

											if (pSiegeWarFare->nDungeonCharge + nDungeonChargeTax > COIN_MAX)
												pSiegeWarFare->nDungeonCharge = COIN_MAX;
											else
												pSiegeWarFare->nDungeonCharge += nDungeonChargeTax;

											if (pSiegeWarFare->nMoradonTax + nMaradonTax > COIN_MAX)
												pSiegeWarFare->nMoradonTax = COIN_MAX;
											else
												pSiegeWarFare->nMoradonTax += nMaradonTax;
										}
										else if (pSiegeWarFare->sMoradonTariff == 12)
										{
											uint32 nDungeonChargeTax = (tax09 * 80) / 100;
											uint32 nMaradonTax = (tax09 * 10) / 100;

											if (pSiegeWarFare->nDungeonCharge + nDungeonChargeTax > COIN_MAX)
												pSiegeWarFare->nDungeonCharge = COIN_MAX;
											else
												pSiegeWarFare->nDungeonCharge += nDungeonChargeTax;

											if (pSiegeWarFare->nMoradonTax + nMaradonTax > COIN_MAX)
												pSiegeWarFare->nMoradonTax = COIN_MAX;
											else
												pSiegeWarFare->nMoradonTax += nMaradonTax;
										}
										else if (pSiegeWarFare->sMoradonTariff == 13)
										{
											uint32 nDungeonChargeTax = (tax08 * 80) / 100;
											uint32 nMaradonTax = (tax08 * 10) / 100;

											if (pSiegeWarFare->nDungeonCharge + nDungeonChargeTax > COIN_MAX)
												pSiegeWarFare->nDungeonCharge = COIN_MAX;
											else
												pSiegeWarFare->nDungeonCharge += nDungeonChargeTax;

											if (pSiegeWarFare->nMoradonTax + nMaradonTax > COIN_MAX)
												pSiegeWarFare->nMoradonTax = COIN_MAX;
											else
												pSiegeWarFare->nMoradonTax += nMaradonTax;
										}
										else if (pSiegeWarFare->sMoradonTariff == 14)
										{
											uint32 nDungeonChargeTax = (tax07 * 80) / 100;
											uint32 nMaradonTax = (tax07 * 10) / 100;

											if (pSiegeWarFare->nDungeonCharge + nDungeonChargeTax > COIN_MAX)
												pSiegeWarFare->nDungeonCharge = COIN_MAX;
											else
												pSiegeWarFare->nDungeonCharge += nDungeonChargeTax;

											if (pSiegeWarFare->nMoradonTax + nMaradonTax > COIN_MAX)
												pSiegeWarFare->nMoradonTax = COIN_MAX;
											else
												pSiegeWarFare->nMoradonTax += nMaradonTax;
										}
										else if (pSiegeWarFare->sMoradonTariff == 15)
										{
											uint32 nDungeonChargeTax = (tax06 * 80) / 100;
											uint32 nMaradonTax = (tax06 * 10) / 100;

											if (pSiegeWarFare->nDungeonCharge + nDungeonChargeTax > COIN_MAX)
												pSiegeWarFare->nDungeonCharge = COIN_MAX;
											else
												pSiegeWarFare->nDungeonCharge += nDungeonChargeTax;

											if (pSiegeWarFare->nMoradonTax + nMaradonTax > COIN_MAX)
												pSiegeWarFare->nMoradonTax = COIN_MAX;
											else
												pSiegeWarFare->nMoradonTax += nMaradonTax;
										}
										else if (pSiegeWarFare->sMoradonTariff == 16)
										{
											uint32 nDungeonChargeTax = (tax05 * 80) / 100;
											uint32 nMaradonTax = (tax05 * 10) / 100;

											if (pSiegeWarFare->nDungeonCharge + nDungeonChargeTax > COIN_MAX)
												pSiegeWarFare->nDungeonCharge = COIN_MAX;
											else
												pSiegeWarFare->nDungeonCharge += nDungeonChargeTax;

											if (pSiegeWarFare->nMoradonTax + nMaradonTax > COIN_MAX)
												pSiegeWarFare->nMoradonTax = COIN_MAX;
											else
												pSiegeWarFare->nMoradonTax += nMaradonTax;
										}
										else if (pSiegeWarFare->sMoradonTariff == 17)
										{
											uint32 nDungeonChargeTax = (tax04 * 80) / 100;
											uint32 nMaradonTax = (tax04 * 10) / 100;

											if (pSiegeWarFare->nDungeonCharge + nDungeonChargeTax > COIN_MAX)
												pSiegeWarFare->nDungeonCharge = COIN_MAX;
											else
												pSiegeWarFare->nDungeonCharge += nDungeonChargeTax;

											if (pSiegeWarFare->nMoradonTax + nMaradonTax > COIN_MAX)
												pSiegeWarFare->nMoradonTax = COIN_MAX;
											else
												pSiegeWarFare->nMoradonTax += nMaradonTax;
										}
										else if (pSiegeWarFare->sMoradonTariff == 18)
										{
											uint32 nDungeonChargeTax = (tax03 * 80) / 100;
											uint32 nMaradonTax = (tax03 * 10) / 100;

											if (pSiegeWarFare->nDungeonCharge + nDungeonChargeTax > COIN_MAX)
												pSiegeWarFare->nDungeonCharge = COIN_MAX;
											else
												pSiegeWarFare->nDungeonCharge += nDungeonChargeTax;

											if (pSiegeWarFare->nMoradonTax + nMaradonTax > COIN_MAX)
												pSiegeWarFare->nMoradonTax = COIN_MAX;
											else
												pSiegeWarFare->nMoradonTax += nMaradonTax;
										}
										else if (pSiegeWarFare->sMoradonTariff == 19)
										{
											uint32 nDungeonChargeTax = (tax02 * 80) / 100;
											uint32 nMaradonTax = (tax02 * 10) / 100;

											if (pSiegeWarFare->nDungeonCharge + nDungeonChargeTax > COIN_MAX)
												pSiegeWarFare->nDungeonCharge = COIN_MAX;
											else
												pSiegeWarFare->nDungeonCharge += nDungeonChargeTax;

											if (pSiegeWarFare->nMoradonTax + nMaradonTax > COIN_MAX)
												pSiegeWarFare->nMoradonTax = COIN_MAX;
											else
												pSiegeWarFare->nMoradonTax += nMaradonTax;
										}
										else if (pSiegeWarFare->sMoradonTariff == 20)
										{
											uint32 nDungeonChargeTax = (tax01 * 80) / 100;
											uint32 nMaradonTax = (tax01 * 10) / 100;

											if (pSiegeWarFare->nDungeonCharge + nDungeonChargeTax > COIN_MAX)
												pSiegeWarFare->nDungeonCharge = COIN_MAX;
											else
												pSiegeWarFare->nDungeonCharge += nDungeonChargeTax;

											if (pSiegeWarFare->nMoradonTax + nMaradonTax > COIN_MAX)
												pSiegeWarFare->nMoradonTax = COIN_MAX;
											else
												pSiegeWarFare->nMoradonTax += nMaradonTax;
										}

									}
									else if (GetZoneID() == ZONE_DELOS || GetZoneID() == ZONE_DESPERATION_ABYSS || GetZoneID() == ZONE_HELL_ABYSS)
									{
										tax00 = (transactionPrice2 * 0) / 100;
										tax01 = (transactionPrice2 * 10) / 100;
										tax02 = (transactionPrice2 * 9) / 100;
										tax03 = (transactionPrice2 * 8) / 100;
										tax04 = (transactionPrice2 * 7) / 100;
										tax05 = (transactionPrice2 * 6) / 100;
										tax06 = (transactionPrice2 * 5) / 100;
										tax07 = (transactionPrice2 * 4) / 100;
										tax08 = (transactionPrice2 * 3) / 100;
										tax09 = (transactionPrice2 * 2) / 100;
										tax010 = (transactionPrice2 * 1) / 100;

										if (pSiegeWarFare->sDellosTariff == 11)
										{
											uint32 nDungeonChargeTax = (tax010 * 80) / 100;
											uint32 nDelosTax = (tax010 * 10) / 100;

											if (pSiegeWarFare->nDungeonCharge + nDungeonChargeTax > COIN_MAX)
												pSiegeWarFare->nDungeonCharge = COIN_MAX;
											else
												pSiegeWarFare->nDungeonCharge += nDungeonChargeTax;

											if (pSiegeWarFare->nDellosTax + nDelosTax > COIN_MAX)
												pSiegeWarFare->nDellosTax = COIN_MAX;
											else
												pSiegeWarFare->nDellosTax += nDelosTax;
										}
										else if (pSiegeWarFare->sDellosTariff == 12)
										{
											uint32 nDungeonChargeTax = (tax09 * 80) / 100;
											uint32 nDelosTax = (tax09 * 10) / 100;

											if (pSiegeWarFare->nDungeonCharge + nDungeonChargeTax > COIN_MAX)
												pSiegeWarFare->nDungeonCharge = COIN_MAX;
											else
												pSiegeWarFare->nDungeonCharge += nDungeonChargeTax;

											if (pSiegeWarFare->nDellosTax + nDelosTax > COIN_MAX)
												pSiegeWarFare->nDellosTax = COIN_MAX;
											else
												pSiegeWarFare->nDellosTax += nDelosTax;
										}
										else if (pSiegeWarFare->sDellosTariff == 13)
										{
											uint32 nDungeonChargeTax = (tax08 * 80) / 100;
											uint32 nDelosTax = (tax08 * 10) / 100;

											if (pSiegeWarFare->nDungeonCharge + nDungeonChargeTax > COIN_MAX)
												pSiegeWarFare->nDungeonCharge = COIN_MAX;
											else
												pSiegeWarFare->nDungeonCharge += nDungeonChargeTax;

											if (pSiegeWarFare->nDellosTax + nDelosTax > COIN_MAX)
												pSiegeWarFare->nDellosTax = COIN_MAX;
											else
												pSiegeWarFare->nDellosTax += nDelosTax;
										}
										else if (pSiegeWarFare->sDellosTariff == 14)
										{
											uint32 nDungeonChargeTax = (tax07 * 80) / 100;
											uint32 nDelosTax = (tax07 * 10) / 100;

											if (pSiegeWarFare->nDungeonCharge + nDungeonChargeTax > COIN_MAX)
												pSiegeWarFare->nDungeonCharge = COIN_MAX;
											else
												pSiegeWarFare->nDungeonCharge += nDungeonChargeTax;

											if (pSiegeWarFare->nDellosTax + nDelosTax > COIN_MAX)
												pSiegeWarFare->nDellosTax = COIN_MAX;
											else
												pSiegeWarFare->nDellosTax += nDelosTax;
										}
										else if (pSiegeWarFare->sDellosTariff == 15)
										{
											uint32 nDungeonChargeTax = (tax06 * 80) / 100;
											uint32 nDelosTax = (tax06 * 10) / 100;

											if (pSiegeWarFare->nDungeonCharge + nDungeonChargeTax > COIN_MAX)
												pSiegeWarFare->nDungeonCharge = COIN_MAX;
											else
												pSiegeWarFare->nDungeonCharge += nDungeonChargeTax;

											if (pSiegeWarFare->nDellosTax + nDelosTax > COIN_MAX)
												pSiegeWarFare->nDellosTax = COIN_MAX;
											else
												pSiegeWarFare->nDellosTax += nDelosTax;
										}
										else if (pSiegeWarFare->sDellosTariff == 16)
										{
											uint32 nDungeonChargeTax = (tax05 * 80) / 100;
											uint32 nDelosTax = (tax05 * 10) / 100;

											if (pSiegeWarFare->nDungeonCharge + nDungeonChargeTax > COIN_MAX)
												pSiegeWarFare->nDungeonCharge = COIN_MAX;
											else
												pSiegeWarFare->nDungeonCharge += nDungeonChargeTax;

											if (pSiegeWarFare->nDellosTax + nDelosTax > COIN_MAX)
												pSiegeWarFare->nDellosTax = COIN_MAX;
											else
												pSiegeWarFare->nDellosTax += nDelosTax;
										}
										else if (pSiegeWarFare->sDellosTariff == 17)
										{
											uint32 nDungeonChargeTax = (tax04 * 80) / 100;
											uint32 nDelosTax = (tax04 * 10) / 100;

											if (pSiegeWarFare->nDungeonCharge + nDungeonChargeTax > COIN_MAX)
												pSiegeWarFare->nDungeonCharge = COIN_MAX;
											else
												pSiegeWarFare->nDungeonCharge += nDungeonChargeTax;

											if (pSiegeWarFare->nDellosTax + nDelosTax > COIN_MAX)
												pSiegeWarFare->nDellosTax = COIN_MAX;
											else
												pSiegeWarFare->nDellosTax += nDelosTax;
										}
										else if (pSiegeWarFare->sDellosTariff == 18)
										{
											uint32 nDungeonChargeTax = (tax03 * 80) / 100;
											uint32 nDelosTax = (tax03 * 10) / 100;

											if (pSiegeWarFare->nDungeonCharge + nDungeonChargeTax > COIN_MAX)
												pSiegeWarFare->nDungeonCharge = COIN_MAX;
											else
												pSiegeWarFare->nDungeonCharge += nDungeonChargeTax;

											if (pSiegeWarFare->nDellosTax + nDelosTax > COIN_MAX)
												pSiegeWarFare->nDellosTax = COIN_MAX;
											else
												pSiegeWarFare->nDellosTax += nDelosTax;
										}
										else if (pSiegeWarFare->sDellosTariff == 19)
										{
											uint32 nDungeonChargeTax = (tax02 * 80) / 100;
											uint32 nDelosTax = (tax02 * 10) / 100;

											if (pSiegeWarFare->nDungeonCharge + nDungeonChargeTax > COIN_MAX)
												pSiegeWarFare->nDungeonCharge = COIN_MAX;
											else
												pSiegeWarFare->nDungeonCharge += nDungeonChargeTax;

											if (pSiegeWarFare->nDellosTax + nDelosTax > COIN_MAX)
												pSiegeWarFare->nDellosTax = COIN_MAX;
											else
												pSiegeWarFare->nDellosTax += nDelosTax;
										}
										else if (pSiegeWarFare->sDellosTariff == 20)
										{
											uint32 nDungeonChargeTax = (tax01 * 80) / 100;
											uint32 nDelosTax = (tax01 * 10) / 100;

											if (pSiegeWarFare->nDungeonCharge + nDungeonChargeTax > COIN_MAX)
												pSiegeWarFare->nDungeonCharge = COIN_MAX;
											else
												pSiegeWarFare->nDungeonCharge += nDungeonChargeTax;

											if (pSiegeWarFare->nDellosTax + nDelosTax > COIN_MAX)
												pSiegeWarFare->nDellosTax = COIN_MAX;
											else
												pSiegeWarFare->nDellosTax += nDelosTax;
										}
									}
								}
								if (pKingSystem != nullptr && !pKingSystem->m_strKingName.empty())
								{
									CKingSystem * pKingSystem = g_pMain->m_KingSystemArray.GetData(GetNation());

									if (pKingSystem->m_nTerritoryTariff > 0)
									{
										switch (GetZoneID())
										{
										case ZONE_KARUS:
										case ZONE_KARUS2:
										case ZONE_KARUS3:
										case ZONE_ELMORAD:
										case ZONE_ELMORAD2:
										case ZONE_ELMORAD3:
										case ZONE_KARUS_ESLANT:
										case ZONE_KARUS_ESLANT2:
										case ZONE_KARUS_ESLANT3:
										case ZONE_ELMORAD_ESLANT:
										case ZONE_ELMORAD_ESLANT2:
										case ZONE_ELMORAD_ESLANT3:
										case ZONE_BIFROST:
										case ZONE_BATTLE:
										case ZONE_BATTLE2:
										case ZONE_BATTLE3:
										case ZONE_BATTLE4:
										case ZONE_BATTLE5:
										case ZONE_BATTLE6:
										case ZONE_SNOW_BATTLE:
										case ZONE_RONARK_LAND:
										case ZONE_ARDREAM:
										case ZONE_RONARK_LAND_BASE:
										case ZONE_KROWAZ_DOMINION:
										case ZONE_STONE1:
										case ZONE_STONE2:
										case ZONE_STONE3:
										case ZONE_BORDER_DEFENSE_WAR:
										case ZONE_UNDER_CASTLE:
										case ZONE_JURAID_MOUNTAIN:
										case ZONE_PARTY_VS_1:
										case ZONE_PARTY_VS_2:
										case ZONE_PARTY_VS_3:
										case ZONE_PARTY_VS_4:
										case ZONE_DRAKI_TOWER:
										case ZONE_DUNGEON_DEFENCE:
										case ZONE_OLD_MORADON:
										case ZONE_CLAN_WAR_ARDREAM:
										case ZONE_CLAN_WAR_RONARK:
										case ZONE_OLD_KARUS:
										case ZONE_OLD_HUMAN:
										case ZONE_NEW_BATTLE_TEST:
										{
											uint32 nNationalTreasuryTax = (tax * 80) / 100;
											uint32 nTerritoryTax = (tax * 20) / 100;

											if (pKingSystem->m_nNationalTreasury + nNationalTreasuryTax > COIN_MAX)
												pKingSystem->m_nNationalTreasury = COIN_MAX;
											else
												pKingSystem->m_nNationalTreasury += nNationalTreasuryTax;

											if (pKingSystem->m_nTerritoryTax + nTerritoryTax > COIN_MAX)
												pKingSystem->m_nTerritoryTax = COIN_MAX;
											else
												pKingSystem->m_nTerritoryTax += nTerritoryTax;
										}
										break;
										default:

											break;
										}
									}
								}
							}
						}
						break;
					}

					if (std::find(pSellTable->ItemIDs.begin(), pSellTable->ItemIDs.end(), pItems[i].ITEMID) == pSellTable->ItemIDs.end())
					{
						errorCode = 2;
						goto fail_return;
					}

					if(transactionPrice > COIN_MAX)
					{
						errorCode = 2;
						goto fail_return;
					}

					if (((pTable->m_sWeight * pItems[i]._ICOUNT) + m_sItemWeight) > m_sMaxWeight)
					{
						errorCode = 4;
						goto fail_return;
					}	

					if (!hasCoins(transactionPrice))
					{
						errorCode = 3;
						goto fail_return;
					}

					m_sItemArray[SLOT_MAX+pItems[i].IPOS].nNum = pItems[i].ITEMID;
					m_sItemArray[SLOT_MAX+pItems[i].IPOS].sDuration = pTable->m_sDuration;
					m_sItemArray[SLOT_MAX+pItems[i].IPOS].sCount += pItems[i]._ICOUNT;					

					g_pMain->WriteItemTransactionLogFile(string_format("%d:%d:%d || ZoneID = %d(%d,%d),UserID=%s,Task=BuyItem NPC,ItemName=%s,ItemID=%d,m_iBuyPrice=%d,count_array[%d]=%d\n",
					time.GetHour(),time.GetMinute(),time.GetSecond(),GetZoneID(),uint16(GetX()),uint16(GetZ()),GetName().c_str(),pTable->m_sName.c_str(),pTable->m_iNum,pTable->m_iBuyPrice, i, pItems[i]._ICOUNT));
				
					m_iGold -= transactionPrice;

					if (!pTable->m_bCountable)
						m_sItemArray[SLOT_MAX+pItems[i].IPOS].nSerialNum = g_pMain->GenerateItemSerial();

					SetUserAbility(false);
					SendItemWeight();
				}

			}
			real_price += transactionPrice; 

			g_pMain->WriteItemTransactionLogFile(string_format("%d:%d:%d || ZoneID = %d(%d,%d),UserID=%s,Task=BuyItem NPC,transactionPrice=%d\n",
			time.GetHour(),time.GetMinute(),time.GetSecond(),GetZoneID(),uint16(GetX()),uint16(GetZ()),GetName().c_str(),transactionPrice));
		}
	}
	// Buying from an NPC National Point
	else if (type == 1 && pNpc->m_iSellingGroup == 249000)
	{	
		for (int i = 0; i < purchased_item_count; i++)
		{	
			transactionPrice = 0;
			if(pItems[i].ITEMID != 0)
			{
				if (isTrading()
					|| (pTable = g_pMain->GetItemPtr(pItems[i].ITEMID)) == nullptr
					|| (pSellTable = g_pMain->m_ItemSellTableArray.GetData(pNpc->m_iSellingGroup)) == nullptr
					|| (type == 2 
					&& (pItems[i].ITEMID >= ITEM_NO_TRADE_MIN && pItems[i].ITEMID <= ITEM_NO_TRADE_MAX
					|| pTable->m_bRace == RACE_UNTRADEABLE))) 
					goto fail_return;

				if (pItems[i].IPOS >= HAVE_MAX
					|| pItems[i]._ICOUNT <= 0 || pItems[i]._ICOUNT > MAX_ITEM_COUNT)
				{
					errorCode = 2;
					goto fail_return;
				}

				if (i == 0) 
				{
					_ITEM_TABLE *prTable;
					for (int j = 0; j < purchased_item_count; j++)
					{
						prTable = nullptr;
						prTable = g_pMain->GetItemPtr(pItems[j].ITEMID);

						if(prTable == nullptr)
							continue;

						total_price += ((uint32)prTable->m_iNPBuyPrice * pItems[j]._ICOUNT);
					}

					if(!hasCoins(total_price)) 
					{
						errorCode = 3;
						total_price = 0;
						goto fail_return;
					}
				}

				if (pItems[i].ITEMID != 0)
				{
					if (m_sItemArray[SLOT_MAX+pItems[i].IPOS].nNum != 0)
					{
						if (m_sItemArray[SLOT_MAX+pItems[i].IPOS].nNum != pItems[i].ITEMID)
						{
							errorCode = 2;
							goto fail_return;
						}

						if (!pTable->m_bCountable || pItems[i]._ICOUNT <= 0)
						{
							errorCode = 2;
							goto fail_return;
						}

						if (pTable->m_bCountable 
							&& (pItems[i]._ICOUNT + m_sItemArray[SLOT_MAX+pItems[i].IPOS].sCount) > MAX_ITEM_COUNT)
						{
							errorCode = 4;
							goto fail_return;				
						}
					}

					transactionPrice = ((uint32)pTable->m_iNPBuyPrice * pItems[i]._ICOUNT); 

					if (std::find(pSellTable->ItemIDs.begin(), pSellTable->ItemIDs.end(), pItems[i].ITEMID) == pSellTable->ItemIDs.end())
					{
						errorCode = 2;
						goto fail_return;
					}
					
					if(transactionPrice > LOYALTY_MAX)
					{
						errorCode = 2;
						goto fail_return;
					}

					if (((pTable->m_sWeight * pItems[i]._ICOUNT) + m_sItemWeight) > m_sMaxWeight)
					{
						errorCode = 4;
						goto fail_return;
					}

					if (!hasLoyalty(transactionPrice))
					{
						errorCode = 3;
						goto fail_return;
					}

					m_sItemArray[SLOT_MAX+pItems[i].IPOS].nNum = pItems[i].ITEMID;
					m_sItemArray[SLOT_MAX+pItems[i].IPOS].sDuration = pTable->m_sDuration;
					m_sItemArray[SLOT_MAX+pItems[i].IPOS].sCount += pItems[i]._ICOUNT;

					g_pMain->WriteItemTransactionLogFile(string_format("%d:%d:%d || ZoneID = %d(%d,%d),UserID=%s,Task=BuyItem NPC,ItemName=%s,ItemID=%d,m_iBuyPrice=%d,count_array[%d]=%d\n",
					time.GetHour(),time.GetMinute(),time.GetSecond(),GetZoneID(),uint16(GetX()),uint16(GetZ()),GetName().c_str(),pTable->m_sName.c_str(),pTable->m_iNum,pTable->m_iBuyPrice, i, pItems[i]._ICOUNT));
				
					m_iLoyalty -= transactionPrice;

					if (!pTable->m_bCountable)
						m_sItemArray[SLOT_MAX+pItems[i].IPOS].nSerialNum = g_pMain->GenerateItemSerial();

					SetUserAbility(false);
					SendItemWeight();
				}

			}
			real_price += transactionPrice; 

			g_pMain->WriteItemTransactionLogFile(string_format("%d:%d:%d || ZoneID = %d(%d,%d),UserID=%s,Task=BuyItem NPC,transactionPrice=%d\n",
			time.GetHour(),time.GetMinute(),time.GetSecond(),GetZoneID(),uint16(GetX()),uint16(GetZ()),GetName().c_str(),transactionPrice));
		}
	}
	// Selling an item to an NPC Gold
	else if (type == 2 && pNpc->m_iSellingGroup != 249000)
	{
		for (int i = 0; i < purchased_item_count; i++)
		{
			if (isTrading()
				|| (pTable = g_pMain->GetItemPtr(pItems[i].ITEMID)) == nullptr
				|| (type == 2 
				&& (pItems[i].ITEMID >= ITEM_NO_TRADE_MIN && pItems[i].ITEMID <= ITEM_NO_TRADE_MAX
				|| pTable->m_bRace == RACE_UNTRADEABLE))) 
				goto fail_return;

			_ITEM_DATA *pItem = &m_sItemArray[SLOT_MAX+pItems[i].IPOS];
			if (pItem->nNum != pItems[i].ITEMID
				|| pItem->isSealed() 
				|| pItem->isRented()
				|| pItem->isExpirationTime())
			{
				errorCode = 2;
				goto fail_return;
			}

			if (pItem->sCount < pItems[i]._ICOUNT)
			{
				errorCode = 3;
				goto fail_return;
			}

			// Added: (GetPremiumProperty(PremiumItemSellPercent) > 0 ? 4 : 6)   Diff For premium (PREMIUM_ITEM/ItemSellPercent)   yy Old: 6
			if (pTable->m_iSellPrice != SellTypeFullPrice)
				transactionPrice = ((pTable->m_iBuyPrice / (GetPremiumProperty(PremiumItemSellPercent) > 0 ? 4 : 6)) * pItems[i]._ICOUNT); // /6 is normal, /4 for prem/discount
			else
				transactionPrice = (pTable->m_iBuyPrice * pItems[i]._ICOUNT);

			if (GetCoins() + transactionPrice > COIN_MAX)
			{
				errorCode = 3;
				goto fail_return;
			}

			GoldGain(transactionPrice, false);

			g_pMain->WriteItemTransactionLogFile(string_format("%d:%d:%d || ZoneID = %d(%d,%d),UserID=%s,Task=SellItem NPC,ItemName=%s,ItemID=%d,m_iBuyPrice=%d,count_array[%d]=%d\n",
			time.GetHour(),time.GetMinute(),time.GetSecond(),GetZoneID(),uint16(GetX()),uint16(GetZ()),GetName().c_str(),pTable->m_sName.c_str(),pTable->m_iNum,pTable->m_iBuyPrice, i, pItems[i]._ICOUNT));

			if (pItems[i]._ICOUNT >= pItem->sCount)
				memset(pItem, 0, sizeof(_ITEM_DATA));
			else
				pItem->sCount -= pItems[i]._ICOUNT;

			SetUserAbility(false);
			SendItemWeight();
			real_price += transactionPrice;

			g_pMain->WriteItemTransactionLogFile(string_format("%d:%d:%d || ZoneID = %d(%d,%d),UserID=%s,Task=SellItem NPC,transactionPrice=%d\n",
			time.GetHour(),time.GetMinute(),time.GetSecond(),GetZoneID(),uint16(GetX()),uint16(GetZ()),GetName().c_str(),transactionPrice));
		}  
	}
	// Selling an item to an NPC NationalPoint
	else if (type == 2 && pNpc->m_iSellingGroup == 249000)
	{
		for (int i = 0; i < purchased_item_count; i++)
		{
			if (isTrading()
				|| (pTable = g_pMain->GetItemPtr(pItems[i].ITEMID)) == nullptr
				|| (type == 2 
				&& (pItems[i].ITEMID >= ITEM_NO_TRADE_MIN 
					&& pItems[i].ITEMID <= ITEM_NO_TRADE_MAX
				|| pTable->m_bRace == RACE_UNTRADEABLE))) 
				goto fail_return;

			_ITEM_DATA *pItem = &m_sItemArray[SLOT_MAX+pItems[i].IPOS];
			if (pItem->nNum != pItems[i].ITEMID
				|| pItem->isSealed() 
				|| pItem->isRented()
				|| pItem->isExpirationTime())
			{
				errorCode = 2;
				goto fail_return;
			}

			if (pItem->sCount < pItems[i]._ICOUNT)
			{
				errorCode = 3;
				goto fail_return;
			}

			// Added: (GetPremiumProperty(PremiumItemSellPercent) > 0 ? 4 : 6)   Diff For premium (PREMIUM_ITEM/ItemSellPercent)   yy Old: 6
			if (pTable->m_iSellPrice != SellTypeFullPrice)
				transactionPrice = ((pTable->m_iBuyPrice / (GetPremiumProperty(PremiumItemSellPercent) > 0 ? 4 : 6)) * pItems[i]._ICOUNT); // /6 is normal, /4 for prem/discount
			else
				transactionPrice = (pTable->m_iBuyPrice * pItems[i]._ICOUNT);

			if (GetLoyalty() + transactionPrice > LOYALTY_MAX)
			{
				errorCode = 3;
				goto fail_return;
			}

			SendLoyaltyChange(transactionPrice, false);
			
			g_pMain->WriteItemTransactionLogFile(string_format("%d:%d:%d || ZoneID = %d(%d,%d),UserID=%s,Task=SellItem NPC,ItemName=%s,ItemID=%d,m_iBuyPrice=%d,count_array[%d]=%d\n",
			time.GetHour(),time.GetMinute(),time.GetSecond(),GetZoneID(),uint16(GetX()),uint16(GetZ()),GetName().c_str(),pTable->m_sName.c_str(),pTable->m_iNum,pTable->m_iBuyPrice, i, pItems[i]._ICOUNT));

			if (pItems[i]._ICOUNT >= pItem->sCount)
				memset(pItem, 0, sizeof(_ITEM_DATA));
			else
				pItem->sCount -= pItems[i]._ICOUNT;

			SetUserAbility(false);
			SendItemWeight();
			real_price += transactionPrice;
			
			g_pMain->WriteItemTransactionLogFile(string_format("%d:%d:%d || ZoneID = %d(%d,%d),UserID=%s,Task=SellItem NPC,transactionPrice=%d\n",
			time.GetHour(),time.GetMinute(),time.GetSecond(),GetZoneID(),uint16(GetX()),uint16(GetZ()),GetName().c_str(),transactionPrice));
		}  
	}
	goto send_packet;

fail_return:
	bSuccess = false;
send_packet:
	result << bSuccess;
	if (!bSuccess)
		result << errorCode;
	else if (pNpc->m_iSellingGroup == 249000 && bSuccess)
	{
		if (purchased_item_count < 1)
			return;

		result << m_iLoyalty << real_price << (uint8)pTable->m_bSellingGroup;
	}
	else if (pNpc->m_iSellingGroup != 249000 && bSuccess)
	{
		if (purchased_item_count < 1)
			return;

		result << m_iGold << real_price << (uint8)pTable->m_bSellingGroup;
	}
	Send(&result);
}

void CUser::ItemTradeRepurchase(Packet & pkt)
{
	uint8 bSubOpcode = 0;
	bSubOpcode = pkt.read<uint8>();	 
	if(bSubOpcode == 4) // Refreshing the screen
	{
		SendRepurchaseMsg(true);
		return;
	}
	else if(bSubOpcode == 3) // closing the shop
		return;

	if(bSubOpcode != 2) // Repurchasing an item
		return;

	Packet result(WIZ_ITEM_TRADE, uint8(5));
	uint8 bItemCount;
	uint32 nItemID = 0;
	bItemCount = pkt.read<uint8>();
	nItemID = pkt.read<uint32>();

	_ITEM_TABLE * pItem = g_pMain->m_ItemtableArray.GetData(nItemID);
	if(pItem == nullptr)
		goto fail_return;

	uint32 sIndex = 0;
	_DELETED_ITEM* pDeletedItem = nullptr;
	{
		foreach_stlmap(itr, m_sDeletedItemMap)
		{
			if(itr->second == nullptr)
				continue;

			if (itr->second->nNum != nItemID)
				continue;

			if(UNIXTIME - itr->second->iDeleteTime > 60 * 60 * 24 * 3)
				continue;

			pDeletedItem = itr->second;
			sIndex = itr->first;
			break;
		}
	}

	if(pDeletedItem == nullptr)
		goto fail_return;

	uint32 nPrice = pItem->m_iBuyPrice * pDeletedItem->sCount * 3;
	if (GetCoins() < nPrice)
		goto fail_return;

	GoldLose(nPrice);
	GiveItem(pDeletedItem->nNum, pDeletedItem->sCount);

	m_sDeletedItemMap.DeleteData(sIndex);

	result << uint8(2) << uint8(1)  // 1 Success,  2 Failed, press button again
		<< uint16(0) << nItemID;

	Send(&result);
	return;

fail_return:
	result << uint8(2) << uint8(2);
	Send(&result);
}

void CUser::SendRepurchaseMsg(bool isRefreshed /*= false*/)
{
	uint32 nPrice;
	uint16 sCount = uint16(m_sDeletedItemMap.GetSize());

	Packet result(WIZ_ITEM_TRADE, uint8(5));
	result << uint8(isRefreshed == true ? 4 : 1) // bSubOpcode
		<< uint8(1)
		<< sCount;

	foreach_stlmap(itr, m_sDeletedItemMap)
	{
		_DELETED_ITEM *pItemDeleted = itr->second;
		if(pItemDeleted == nullptr)
			continue; 

		if(UNIXTIME - pItemDeleted->iDeleteTime > 60 * 60 * 24 * 3)
			continue;

		_ITEM_TABLE * pItem = g_pMain->m_ItemtableArray.GetData(pItemDeleted->nNum);
		if (pItem == nullptr)
			continue;

		nPrice = pItem->m_iBuyPrice * pItemDeleted->sCount * 3;

		if (nPrice > COIN_MAX)
			nPrice = COIN_MAX;

		result << pItemDeleted->nNum
			<< nPrice
			<< uint32(pItemDeleted->iDeleteTime)
			<< uint8(0);
	}

	Send(&result);
}