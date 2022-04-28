#pragma once
class CSupport;
#include "stdafx.h"
#include "TEMUKOACS Engine.h"
#include "N3BASE/N3UIBase.h"
#include "N3BASE/N3UIString.h"
#include "N3BASE/N3UIButton.h"
#include "Timer.h"

class CSupport : public CN3UIBase
{
	bool bug;
	CN3UIString* txt_subject;
	CN3UIString* txt_message;
	CN3UIButton* btn_bug;
	CN3UIButton* btn_koxp;
	CN3UIButton* btn_report;
	CN3UIButton* btn_close;

	CTimer* spamTimer;
public:
	CSupport();
	~CSupport();
	bool Load(HANDLE hFile);
	bool ReceiveMessage(CN3UIBase* pSender, uint32_t dwMsg);
	uint32_t MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld);
	bool OnKeyPress(int iKey);
	void Close();
	void Open();
};