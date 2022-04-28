#include "stdafx.h"
#include "TEMUKOACS Engine.h"

bool s_TeminitionTýmeItemList = false;

void TEMUKOACSEngine::TEMUKOACSHandler(Packet& pkt)
{
	uint8 s_SubCode;
	pkt >> s_SubCode;

	switch (s_SubCode)
	{
	case WIZ_HOOK_VOICE:
		Engine->StartNoviceHandler(pkt);
		break;
	case WIZ_HOOK_EVENT:
	{
		Packet original = pkt;
		uint8 subcode;
		pkt >> subcode;
		if (subcode >= 7 && subcode <= 16)
		{
			if (subcode == TEMPLE_EVENT)
				Engine->m_UiMgr->StartJoinBoard();

			if (Engine->m_UiMgr->uiJoinBoard != NULL)
				Engine->m_UiMgr->uiJoinBoard->Update(original);
		}
	}break;
	case WIZ_HOOK_MONSTER_INFO:
		if (Engine->m_UiMgr->uiSearchMonster != NULL)
			Engine->m_UiMgr->uiSearchMonster->Update(pkt);
		break;
	case WIZ_HOOK_LIFESKILL:
		if (Engine->m_UiMgr->uiLifeSkill == NULL)
		{
			Engine->m_UiMgr->uiLifeSkill = new CLifeSkill();
			Engine->m_UiMgr->uiLifeSkill->Init(Engine->m_UiMgr);
			Engine->m_UiMgr->uiLifeSkill->LoadFromFile(Engine->m_BasePath + xorstr("TEMUKOACS\\ui\\lifeskill.uif"), N3FORMAT_VER_1068);
			Engine->m_UiMgr->AddChild(Engine->m_UiMgr->uiLifeSkill);
			Engine->m_UiMgr->uiLifeSkill->Close();
		}
		Engine->m_UiMgr->uiLifeSkill->Update(pkt);
		break;
	case WIZ_HOOK_MESSAGE:
	{
		string title, message;
		pkt >> title >> message;
		if (Engine->m_UiMgr != NULL
			&& !title.empty()
			&& !message.empty()
			&& title.length() <= 30
			&& message.length() <= 255)
			Engine->m_UiMgr->ShowMessageBox(title, message, MsgBoxTypes::Ok);
	}break;
	case WIZ_HOOK_MERCHANTLIST:
		Engine->m_UiMgr->ShowMerchantList(pkt);
		break;
	case WIZ_HOOK_HPINFO:
	{
		uint16 maxHP, maxMP;
		int16 hp, mp;

		pkt >> maxHP >> hp >> maxMP >> mp;

		if (Engine->uiHPBarPlug != NULL) {
			Engine->uiHPBarPlug->HPChange(hp, maxHP);
			Engine->uiHPBarPlug->MPChange(mp, maxMP);
		}
	}break;
	case WIZ_HOOK_PARTY_PROCESS:
	{
		if (Engine->uiPartyPlug == NULL
			|| Engine->uiPartyPlug->m_dVTableAddr == 0
			|| Engine->uiPartyPlug->slot[0] == NULL
			|| Engine->uiPartyPlug->slot[0]->strHP == NULL)
			return;

		Packet original = pkt;
		uint8 subcode;
		pkt >> subcode;
		switch (subcode)
		{
		case 0:
			Engine->uiPartyPlug->ResetParty();
			break;
		case 1:
			Engine->uiPartyPlug->ReorderParty(original);
			break;
		default:
			break;
		}
	}break;
	case WIZ_HOOK_AUTODROP:
	{
		uint8 type;
		uint32 nItemID;
		pkt >> type >> nItemID;
		switch (type)
		{
		case 0:
		{
			if (Engine->m_UiMgr->uiDropResult == NULL)
				Engine->m_UiMgr->ShowDropResult();

			Engine->m_UiMgr->uiDropResult->SetTitle("Auto Mining");
			Engine->m_UiMgr->uiDropResult->AddItem(nItemID);

			TABLE_ITEM_BASIC* item = Engine->tblMgr->getItemData(nItemID);
			if (item != nullptr)
				Engine->WriteInfoMessageExt((char*)string_format(xorstr("[Mining] you received %s"), item->strName.c_str()).c_str(), 0xFFF2AB);

			Engine->m_UiMgr->uiTopLeft->DropResultStatus(true);
		}break;
		case 1:
		{
			if (Engine->m_UiMgr->uiDropResult == NULL)
				Engine->m_UiMgr->ShowDropResult();

			Engine->m_UiMgr->uiDropResult->SetTitle("Auto Fishing");
			Engine->m_UiMgr->uiDropResult->AddItem(nItemID);

			TABLE_ITEM_BASIC* item = Engine->tblMgr->getItemData(nItemID);
			if (item != nullptr)
				Engine->WriteInfoMessageExt((char*)string_format(xorstr("[Fishing] you received %s"), item->strName.c_str()).c_str(), 0xFFF2AB);

			Engine->m_UiMgr->uiTopLeft->DropResultStatus(true);
		}break;
		default:
			if (Engine->m_UiMgr->uiDropResult != NULL) {
				Engine->m_UiMgr->uiDropResult->Close();
				Engine->m_UiMgr->uiTopLeft->DropResultStatus(false);
			}
			Engine->WriteInfoMessageExt((char*)xorstr("[Mining/Fishing] stopped"), 0xFFF2AB);
			break;
		}
	}break;
	case WIZ_HOOK_GMTOOLS:
	{
		if (Engine->m_UiMgr != NULL)
			Engine->m_UiMgr->ProccessGM(pkt);
	}break;
	case WIZ_HOOK_TEMPITEMS:
	{
		uint8 s_sCount;
		pkt >> s_sCount;

		if (s_TeminitionTýmeItemList)
			return;

		s_TeminitionTýmeItemList = true;

		if (Engine->m_UiMgr->uiTopLeft == NULL) {
			Engine->m_UiMgr->uiTopLeft = new CTopLeft();
			Engine->m_UiMgr->uiTopLeft->Init(Engine->m_UiMgr);
			Engine->m_UiMgr->uiTopLeft->LoadFromFile(Engine->m_BasePath + xorstr("TEMUKOACS\\ui\\topleft.uif"), N3FORMAT_VER_1068);
			Engine->m_UiMgr->AddChild(Engine->m_UiMgr->uiTopLeft);
		}

		if (s_sCount > 0)
		{
			if (Engine->m_UiMgr->uiTempItemList == NULL) {
				Engine->m_UiMgr->uiTempItemList = new CTempItemList();
				Engine->m_UiMgr->uiTempItemList->Init(Engine->m_UiMgr);
				Engine->m_UiMgr->uiTempItemList->LoadFromFile(Engine->m_BasePath + xorstr("TEMUKOACS\\ui\\re_itemlist.uif"), N3FORMAT_VER_1068);
				Engine->m_UiMgr->AddChild(Engine->m_UiMgr->uiTempItemList);
			}

			for (int i = 0; i < s_sCount; i++) {
				_SimpleItem item;
				pkt >> item.nItemID >> item.pos >> item.time;
				Engine->m_UiMgr->uiTempItemList->itemList.push_back(item);
			}
			Engine->m_UiMgr->uiTempItemList->SetItem();
		}
		Packet result(WIZ_HOOK_GUARD, uint8(WIZ_HOOK_USERDATA));
		Engine->Send(&result);
	}break;
	case WIZ_HOOK_USERDATA:
	{
		if (Engine->m_PlayerBase == NULL)
			Engine->m_PlayerBase = new CPlayerBase();

		Engine->m_PlayerBase->TEMUKOACSUserUpdate(pkt);
	}break;
	case WIZ_HOOK_CR:
	{
		uint8 subCode;
		pkt >> subCode;

		switch (subCode)
		{
		case 0:
		{
			if (Engine->IsCRActive == true)
				return;

			Engine->IsCRActive = true;

			for (int i = 0; i < 4; i++)
			{
				pkt >> Engine->crEvent.mobs[i]
					>> Engine->crEvent.killTarget[i]
					>> Engine->crEvent.killCount[i];

				Engine->crEvent.killCount[i] = 0;
			}

			for (int i = 0; i < 2; i++)
				pkt >> Engine->crEvent.rewards[i]
				>> Engine->crEvent.rewardsCount[i];

			pkt >> Engine->crEvent.rewardNoah
				>> Engine->crEvent.rewardExp
				>> Engine->crEvent.rewardNP
				>> Engine->crEvent.rewardKC
				>> Engine->crEvent.endTime;

			if (Engine->m_UiMgr->uiCollectionRace == NULL)
			{
				Engine->m_UiMgr->uiCollectionRace = new CCollectionRace();
				Engine->m_UiMgr->uiCollectionRace->Init(Engine->m_UiMgr);
				Engine->m_UiMgr->uiCollectionRace->LoadFromFile(Engine->m_BasePath + xorstr("TEMUKOACS\\ui\\cr.uif"), N3FORMAT_VER_1068);
				Engine->m_UiMgr->AddChild(Engine->m_UiMgr->uiCollectionRace);
			}
			Engine->m_UiMgr->uiCollectionRace->Open();
			Engine->m_UiMgr->uiCollectionRace->Start();
			Engine->WriteInfoMessageExt((char*)xorstr("[CR] Event has begun."), 0xFFF2AB);
		}break;
		case 1:
		{
			if (Engine->IsCRActive == false)
				return;

			for (int i = 0; i < 4; i++) {
				Engine->crEvent.mobs[i] = 0;
				Engine->crEvent.killTarget[i] = 0;
				Engine->crEvent.killCount[i] = 0;
			}
			for (int i = 0; i < 2; i++)
			{
				Engine->crEvent.rewards[i] = 0;
				Engine->crEvent.rewardsCount[i] = 0;
			}

			Engine->crEvent.rewardNoah = 0;
			Engine->crEvent.rewardExp = 0;
			Engine->crEvent.rewardNP = 0;
			Engine->crEvent.rewardKC = 0;
			Engine->crEvent.endTime = 0;

			Engine->m_UiMgr->uiCollectionRace->SetVisible(false);
			if (Engine->m_UiMgr->uiCollectionRaceRewards != NULL)
				Engine->m_UiMgr->uiCollectionRaceRewards->SetVisible(false);

			Engine->IsCRActive = false;
			Engine->WriteInfoMessageExt((char*)xorstr("[CR] Event has ended."), 0xFFF2AB);
		}break;
		case 2:
		{
			uint16 protoID;
			pkt >> protoID;

			if (Engine->IsCRActive)
			{
				bool updateRequired = false;
				for (int i = 0; i < 4; i++) {
					if (Engine->crEvent.mobs[i] == protoID && Engine->crEvent.killCount[i] < Engine->crEvent.killTarget[i])
					{
						Engine->crEvent.killCount[i]++;
						updateRequired = true;
					}
				}
				if (updateRequired)
					Engine->m_UiMgr->uiCollectionRace->Update();
			}
		}break;
		default:
			break;
		}
	}break;
	case WIZ_HOOK_DROP_REQUEST:
		Engine->m_UiMgr->ShowDropList(pkt);
		break;
	case WIZ_HOOK_CASHCHANGE:
	{
		uint32 cash, bonuscash;
		pkt >> cash >> bonuscash;

		if (Engine->uiState != NULL) {
			if (cash > Engine->uiState->m_iCash)
				Engine->WriteInfoMessageExt((char*)string_format(xorstr("Received %d knight cash."), cash - Engine->uiState->m_iCash).c_str(), 0xFFF2AB);
			else
				Engine->WriteInfoMessageExt((char*)string_format(xorstr("Lost %d knight cash."), Engine->uiState->m_iCash - cash).c_str(), 0xFF0000);
		}

		Engine->Player.KnightCash = cash;
		Engine->Player.KnightCashBonus = bonuscash;

		if (Engine->uiState != NULL){
			Engine->uiState->m_iCash = cash;
			Engine->uiState->m_iCashBonus = bonuscash;
			Engine->uiState->UpdateKC(cash, bonuscash);
		}

		if (Engine->uiTradeInventory != NULL) {
			Engine->uiTradeInventory->UpdateTotal(cash);
		}

		if (Engine->m_UiMgr->uiPowerUpStore != NULL) {
			Engine->m_UiMgr->uiPowerUpStore->UpdateCash(cash, bonuscash);
		}
	}break;
	case WIZ_HOOK_INFOMESSAGE:
	{
		string message;
		pkt >> message;
		Engine->WriteInfoMessageExt((char*)message.c_str(), 0xFFF2AB);
	}break;
	case WIZ_HOOK_PUS:
	{
		uint32 itemCount;
		vector<PUSItem> items;
		pkt >> itemCount;

		for (uint32 i = 0; i < itemCount; i++) {
			uint32 sid, price, quantitiy;
			uint8 cat;
			pkt >> sid >> price >> cat >> quantitiy;
			items.push_back(PUSItem(sid, price, (PUS_CATEGORY)cat, quantitiy));
		}
		Engine->m_UiMgr->item_list = items;
	}break;
	case WIZ_HOOK_PROCINFO:
	{
		uint16 sProcessID = 0;
		pkt >> sProcessID;
		Engine->SendProcess(sProcessID);
	}break;
	case WIZ_HOOK_ALIVE:
		StayAlive();
		break;
	case WIZ_HOOK_OPEN:
	{
		string address;
		pkt >> address;
		ShellExecuteA(NULL, xorstr("open"), address.c_str(), NULL, NULL, SW_SHOWNORMAL);
	}break;
	case WIZ_HOOK_UIINFO:
	{
		uint32 cash, bonuscash;
		pkt >> cash >> bonuscash;

		Engine->Player.KnightCash = cash;
		Engine->Player.KnightCashBonus = bonuscash;

		if (Engine->m_PlayerBase == NULL)
			Engine->m_PlayerBase = new CPlayerBase();

		if (Engine->uiState != NULL)
			Engine->uiState->SetAntiDefInfo(pkt);

		if (Engine->m_PlayerBase != NULL)
			Engine->m_PlayerBase->PlayerUpdate(pkt);

		if (Engine->uiState != NULL) {
			Engine->uiState->m_iCash = cash;
			Engine->uiState->m_iCashBonus = bonuscash;
			Engine->uiState->UpdateKC(cash, bonuscash);
		}
		if (Engine->uiTradeInventory != NULL) {
			Engine->uiTradeInventory->UpdateTotal(cash);
		}
	}break;
	case WIZ_HOOK_CHAOTIC_RESULT:
		Engine->uiPieceChangePlug->m_bAutoExchangeStarted = false;
		Engine->uiPieceChangePlug->m_Timer = NULL;
		break;
	default:
		printf("Hook packet (%X)\n", s_SubCode);
		break;
	}
}