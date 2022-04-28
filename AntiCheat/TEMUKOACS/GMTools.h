#pragma once
class CGMTools;
#include "stdafx.h"
#include "TEMUKOACS Engine.h"
#include "N3BASE/N3UIBase.h"
#include "N3BASE/N3UIString.h"
#include "N3BASE/N3UIButton.h"
#include "N3BASE/N3UIImage.h"

class CGMTools : public CN3UIBase
{
	CN3UIString* txt_nick;
	CN3UIString* txt_detay;
	CN3UIString* txt_durum;
	CN3UIString* txt_letter;
	
	CN3UIButton* btn_close;
	CN3UIButton* btn_find;
	CN3UIButton* btn_select;
	CN3UIButton* btn_startgenie;
	CN3UIButton* btn_stopgenie;
	CN3UIButton* btn_scan;
	CN3UIButton* btn_tpon;
	CN3UIButton* btn_tpoff;
	CN3UIButton* btn_dc;
	CN3UIButton* btn_jail;
	CN3UIButton* btn_ban;
	CN3UIButton* btn_bdw;
	CN3UIButton* btn_chaos;
	CN3UIButton* btn_jr;
	CN3UIButton* btn_cr;
	CN3UIButton* btn_letter;
	CN3UIButton* btn_online;
public:
	CGMTools();
	~CGMTools();
	bool Load(HANDLE hFile);
	bool ReceiveMessage(CN3UIBase* pSender, uint32_t dwMsg);
	uint32_t MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld);
	bool OnKeyPress(int iKey);
	void Close();
	void Open();
};