#include "stdafx.h"
#include "DBAgent.h"

void CUser::PPCardSystem(Packet & pkt)
{
	Packet result(WIZ_EDIT_BOX);
	uint8 opcode = pkt.read<uint8>();

	switch (opcode)
	{
	case 4:
		PPCard(pkt);
		break;

	default:
		TRACE("Unhandled PPCardSystem opcode: %X\n", opcode);
	}
}

void CUser::PPCard(Packet& pkt)// çözüm buldum
{
	Packet result(WIZ_EDIT_BOX, uint8(0x04));
	uint8 bResult = 1;
	uint32 Box1 = pkt.read<uint32>();
	std::string Code;
	pkt.SByte();
	pkt >> Code;
	Guard lock(g_pMain->m_PPCardArray.m_lock);
	_PPCARD* pCard = g_pMain->m_PPCardArray.GetData(Box1); //null olayý çözüldümü  evet

	if (LastUsePPCard > 0 && LastUsePPCard > UNIXTIME)
	{
		g_pMain->SendSpecialNotice("[PPCARD]", string_format("Try agin afer %d seconds(s).", char16_t(LastUsePPCard - UNIXTIME)), 2, this);
		return;
	}

	if (pCard == nullptr)
	{
		LastUsePPCard = UNIXTIME + 30;
		g_pMain->SendSpecialNotice("[PPCARD]", string_format("The KESN Code you entered is wrong ! [Hatalý Kesn kodu]"), 2, this);
		return;
	}

	if (pCard->Codes != Code || pCard->FirstBox != Box1)
	{
		LastUsePPCard = UNIXTIME + 30;
		g_pMain->SendSpecialNotice("[PPCARD]", string_format("The KESN Code you entered is wrong ! [Hatalý Kesn kodu]"), 2, this);
		return;
	}

	if (pCard->isUsed)
	{
		LastUsePPCard = UNIXTIME + 30;
		g_pMain->SendSpecialNotice("[PPCARD]", string_format("The KESN Code you entered was used ! [KESN kodu zaten kullanýlmýþ]"), 2, this);
		return;
	}

	GiveKnightCash(pCard->CashPoint);
	result << int8(bResult);
	g_pMain->SendSpecialNotice("[PPCARD]", string_format("%d Knight cash given to your account (%s)", pCard->CashPoint, GetAccountName().c_str()), 2, this);
	LastUsePPCard = UNIXTIME + 60;
	pCard->isUsed = 1;
	g_DBAgent.UpdatePPCard(pCard->FirstBox, 1, GetAccountName());

	Send(&result);
	return;
fail_return:
	/*
		0,2,3 -> Wrong
		4 -> Expired
		5 -> Registered/Used
		6 -> Use Later
		7 -> After 5Minute
	*/
	result << uint8(bResult);
	Send(&result);
}