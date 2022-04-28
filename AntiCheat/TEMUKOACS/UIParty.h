#pragma once
class CUIPartyPlugin;
#include "stdafx.h"
#include "TEMUKOACS Engine.h"

struct PartySlot
{
	short uid;
	DWORD baseAddr;
	DWORD strName;
	DWORD strHP;
	DWORD strNoBuff;
	DWORD iconSkill[5];
	DWORD iconVoice;
	CTimer* voiceTimer;
	PartySlot()
	{
		uid = 0;
		baseAddr = 0;
		strName = 0;
		strHP = 0;
		strNoBuff = 0;
		for (int i = 0; i < 5; i++)
			iconSkill[i] = 0;
		iconVoice = 0;
		voiceTimer = NULL;
	}
};

class CUIPartyPlugin
{
public:
	DWORD m_dVTableAddr;

	PartySlot* slot[8];

public:
	CUIPartyPlugin();
	~CUIPartyPlugin();
	void ParseUIElements();
	bool ReceiveMessage(DWORD* pSender, uint32_t dwMsg);
	void InitReceiveMessage();
	void ReorderParty(Packet pkt);
	void ResetParty();
	void UserSpeaking(uint16 uid, bool speaking);
	bool isInParty();
	void Tick();
private:

};