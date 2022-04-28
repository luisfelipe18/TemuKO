#pragma once
class CLifeSkill;
#include "stdafx.h"
#include "TEMUKOACS Engine.h"
#include "N3BASE/N3UIBase.h"
#include "N3BASE/N3UIString.h"
#include "N3BASE/N3UIButton.h"

class CLifeSkill : public CN3UIBase
{
	CN3UIButton* btn_close;
	CN3UIButton* btn_troop;
	CN3UIString* txt_level[4];
	CN3UIString* txt_exp[4];
	CN3UIProgress* progress_exp[4];
public:
	CLifeSkill();
	~CLifeSkill();
	bool Load(HANDLE hFile);
	bool ReceiveMessage(CN3UIBase* pSender, uint32_t dwMsg);
	uint32_t MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld);
	bool OnKeyPress(int iKey);
	void Close();
	void Open();
	void Update(Packet pkt);
};