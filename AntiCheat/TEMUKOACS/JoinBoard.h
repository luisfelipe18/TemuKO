#pragma once
class CJoinBoard;
#include "stdafx.h"
#include "TEMUKOACS Engine.h"
#include "N3BASE/N3UIBase.h"
#include "N3BASE/N3UIString.h"
#include "N3BASE/N3UIButton.h"
#include "N3BASE/N3UIArea.h"

class CJoinBoard : public CN3UIBase
{

	CN3UIBase* grp_minimum, * grp_maximum, * grp_rewards;
	CN3UIButton* btn_icon, * btn_maximum, * btn_enter, * btn_cancel, * btn_rewards_open, * btn_rewards_close;
	CN3UIString* txt_notice, * txt_time, *txt_content, * txt_content2, * txt_participant;
	CN3UIArea* slot_victory, * slot_defeat, * slot_victory_main, * slot_defeat_main;
	CTimer* m_Timer;
	uint16 m_iRemainingTime;

	RECT minimumRect, maximizeRect, rewardsRect;
public:
	CJoinBoard();
	~CJoinBoard();
	bool Load(HANDLE hFile);
	bool ReceiveMessage(CN3UIBase* pSender, uint32_t dwMsg);
	uint32_t MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld);
	bool OnKeyPress(int iKey);
	void Close();
	void Open();
	void Update(Packet pkt);
	void Tick();
};