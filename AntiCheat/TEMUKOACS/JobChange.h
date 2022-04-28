#pragma once
class CJobChange;
#include "stdafx.h"
#include "TEMUKOACS Engine.h"
#include "N3BASE/N3UIBase.h"
#include "N3BASE/N3UIString.h"
#include "N3BASE/N3UIButton.h"

class CJobChange : public CN3UIBase
{
	CN3UIButton* btn_warrior;
	CN3UIButton* btn_rogue;
	CN3UIButton* btn_mage;
	CN3UIButton* btn_priest;
public:
	CJobChange();
	~CJobChange();
	bool Load(HANDLE hFile);
	bool ReceiveMessage(CN3UIBase* pSender, uint32_t dwMsg);
	uint32_t MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld);
	bool OnKeyPress(int iKey);
	void Close();
	void Open();
};