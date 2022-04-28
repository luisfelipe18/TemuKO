#include "stdafx.h"
#include "TEMUKOACS Engine.h"

void TEMUKOACSEngine::PointChange(Packet& pkt)
{
	uint8 subcode;
	pkt >> subcode;
	uint32 tmp32;
	uint16 maxhp, hp, maxmp, mp, tmp;
	pkt >> tmp >> maxhp >> maxmp >> tmp >> tmp32 >> hp >> mp;

	if (Engine->uiHPBarPlug != NULL) {
		Engine->uiHPBarPlug->HPChange(hp, maxhp);
		Engine->uiHPBarPlug->MPChange(mp, maxmp);
	}
}

void TEMUKOACSEngine::LevelChange(Packet& pkt)
{
	short maxhp, hp, maxmp, mp;
	uint16 sock;
	uint8 level, freeskill;
	int16 points;
	int64 maxexp, xp;
	pkt >> sock >> level >> points >> freeskill >> maxexp >> xp >> maxhp >> hp >> maxmp >> mp;

	if (Engine->m_PlayerBase != NULL){
		Engine->m_PlayerBase->m_iSocketID = sock;
		Engine->m_PlayerBase->m_iLevel = level;
	}

	if (Engine->uiHPBarPlug != NULL) {
		Engine->uiHPBarPlug->HPChange(hp, maxhp);
		Engine->uiHPBarPlug->MPChange(mp, maxmp);
	}
}

void TEMUKOACSEngine::HpChange(Packet& pkt)
{
	short maxhp, hp;
	pkt >> maxhp >> hp;
	if (Engine->uiHPBarPlug != NULL)
		Engine->uiHPBarPlug->HPChange(hp, maxhp);
}

void TEMUKOACSEngine::MpChange(Packet& pkt)
{
	short maxmp, mp;
	pkt >> maxmp >> mp;
	if (Engine->uiHPBarPlug != NULL)
		Engine->uiHPBarPlug->MPChange(mp, maxmp);
}

void TEMUKOACSEngine::TargetHpChange(Packet& pkt)
{
	if (Engine->uiTargetBar != NULL)
		Engine->uiTargetBar->SetTargetHp(pkt);
}

void TEMUKOACSEngine::SendMYInfo(Packet& pkt)
{
	// handle myinfo
	uint8 nation, face, rank, title, unk1, unk2;
	uint16 posX, posY, posZ;
	uint32 hair;
	pkt.SByte();
	pkt >> Engine->m_PlayerBase->m_iSocketID 
		>> Engine->m_PlayerBase->m_strCharacterName 
		>> posX 
		>> posZ 
		>> posY 
		>> nation
		>> Engine->m_PlayerBase->m_iRace 
		>> Engine->m_PlayerBase->m_sClass 
		>> face 
		>> hair
		>> rank 
		>> title 
		>> unk1 
		>> unk2
		>> Engine->m_PlayerBase->m_iLevel;

	if (!Engine->Player.isGameStat)
		Engine->Player.isGameStat = true;

	Packet result(WIZ_HOOK_GUARD, uint8(WIZ_HOOK_UIINFO));
	Engine->Send(&result);
}

void TEMUKOACSEngine::GameStart(Packet& pkt)
{
	Packet result(WIZ_HOOK_GUARD, uint8(WIZ_HOOK_UIINFO));
	Engine->Send(&result);
}

void TEMUKOACSEngine::ZoneChangeHandler(Packet& pkt)
{
	uint8 s_SubCode;
	pkt >> s_SubCode;

	switch (s_SubCode)
	{
	case 2:
		Engine->Player.isTeleporting = false;
		Engine->Loading = false;
		break;
	case 3:
	{
		Engine->Player.isTeleporting = true;
		uint16 newZone;
		pkt >> newZone;
		Engine->Player.zone = newZone;
		Engine->m_PlayerBase->UpdateZone(newZone);
	}break;
	default:
		break;
	}
}

void TEMUKOACSEngine::NoticeHandler(Packet& pkt)
{
	Packet result(WIZ_HOOK_GUARD, uint8(WIZ_HOOK_UIINFO));
	Engine->Send(&result);
}

void TEMUKOACSEngine::SendItemMove(Packet& pkt)
{
	uint16 a1, a2, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17;
	uint32 a3;
	uint8 a00, a0, a4;
	short maxhp, maxmp, hp, mp;
	pkt >> a00 >> a0 >> a1 >> a2 >> a3 >> a4 >> maxhp >> maxmp >> a7 >> a8 >> a9 >> a10 >> a11 >> a12 >> a13 >> a14 >> a15 >> a16 >> a17;

	if (a00 == 2)
		return;

	if (a0 != 0)
	{
		uint32 cash, bonuscash;
		pkt >> cash >> bonuscash;

		Engine->Player.KnightCash = cash;
		Engine->Player.KnightCashBonus = bonuscash;

		if (Engine->uiState != NULL)
			Engine->uiState->SetAntiDefInfoItemMove(pkt);

		if (Engine->uiState != NULL) {
			Engine->uiState->m_iCash = cash;
			Engine->uiState->m_iCashBonus = bonuscash;
			Engine->uiState->UpdateKC(cash, bonuscash);
		}
		
		pkt >> hp >> mp;

		if (Engine->uiTradeInventory != NULL) {
			Engine->uiTradeInventory->UpdateTotal(cash);
		}

		if (Engine->uiHPBarPlug != NULL)
		{
			Engine->uiHPBarPlug->HPChange(hp, maxhp);
			Engine->uiHPBarPlug->MPChange(mp, maxmp);
		}
	}
}

void TEMUKOACSEngine::MerchantHandler(Packet& pkt)
{
	uint8 s_SubCode;
	pkt >> s_SubCode;
	switch (s_SubCode)
	{
	case MERCHANT_ITEM_ADD:
		if (Engine->m_MerchantMgr == nullptr)
			return;
		Engine->m_MerchantMgr->UpdateRecentItemAddReq(pkt);
		break;
	case MERCHANT_SLOT_UPDATE:
		if (Engine->m_MerchantMgr == nullptr)
			return;
		Engine->m_MerchantMgr->UpdateItemDisplaySlots(pkt);
		break;
	case MERCHANT_ITEM_CANCEL:
		if (Engine->m_MerchantMgr == nullptr)
			return;
		Engine->m_MerchantMgr->RemoveItemFromSlot(pkt);
		break;
	case MERCHANT_INSERT:
		if (Engine->m_MerchantMgr == nullptr)
			return;
		Engine->m_MerchantMgr->MerchantCreated(pkt);
		break;
	case MERCHANT_ITEM_LIST:
		if (Engine->m_MerchantMgr == nullptr)
			return;
		Engine->m_MerchantMgr->SetTheirMerchantSlots(pkt);
		break;
	case MERCHANT_TRADE_CANCEL:
		if (Engine->m_MerchantMgr == nullptr)
			return;
		Engine->m_MerchantMgr->ResetMerchant();
		break;
	default:
		break;
	}
}

void TEMUKOACSEngine::ItemUpgradeHandler(Packet& pkt)
{
	uint8 subCode;
	pkt >> subCode;
	switch (subCode)
	{
	case ITEM_BIFROST_EXCHANGE:
	{
		uint8 resultOpCode;
		pkt >> resultOpCode;

		if (resultOpCode == Success && Engine->uiPieceChangePlug->m_bAuto)
		{
			POINT pt;
			Engine->GetUiPos(Engine->uiPieceChangePlug->m_btnStop, pt);
			pt.y -= 2;
			Engine->m_UiMgr->SendMouseProc(UI_MOUSE_LBCLICK, pt, pt);
		}
	}break;
	default:
		break;
	}
}