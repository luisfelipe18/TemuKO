#pragma once
class CTopLeft;
#include "stdafx.h"
#include "TEMUKOACS Engine.h"
#include "N3BASE/N3UIBase.h"
#include "N3BASE/N3UIString.h"
#include "N3BASE/N3UIButton.h"

class CTopLeft : public CN3UIBase
{
	bool posSet;
	CN3UIButton* btn_facebook;
	CN3UIButton* btn_discord;
	CN3UIButton* btn_live;
	CN3UIButton* btn_bug;
	CN3UIButton* btn_dropresult;
public:
	CTopLeft();
	~CTopLeft();
	bool Load(HANDLE hFile);
	bool ReceiveMessage(CN3UIBase* pSender, uint32_t dwMsg);
	uint32_t MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld);
	bool OnKeyPress(int iKey);
	void Close();
	void Open();
	void UpdatePosition();
	void DropResultStatus(bool status);
};