#pragma once
class CVoteBoard;
#include "stdafx.h"
#include "TEMUKOACS Engine.h"
#include "N3BASE/N3UIBase.h"
#include "N3BASE/N3UIString.h"
#include "N3BASE/N3UIButton.h"
#include "Timer.h"

class CVoteBoard : public CN3UIBase
{
	CN3UIString* txt_question;
	CN3UIString* txt_answer[3];
	CN3UIButton* btn_answer[3];
	CN3UIButton* btn_vote;
	CN3UIBase* group_result;
	CN3UIString* txt_result;

	CTimer* Timer;

public:
	CVoteBoard();
	~CVoteBoard();
	bool Load(HANDLE hFile);
	bool ReceiveMessage(CN3UIBase* pSender, uint32_t dwMsg);
	uint32_t MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld);
	bool OnKeyPress(int iKey);
	void Close();
	void Open();
};