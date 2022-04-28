#pragma once
class CStatPreset;
#include "stdafx.h"
#include "TEMUKOACS Engine.h"
#include "N3BASE/N3UIBase.h"
#include "N3BASE/N3UIString.h"
#include "N3BASE/N3UIButton.h"

class CStatPreset : public CN3UIBase
{
	bool posSet;
	uint8 selectedIndex;
	CN3UIString* txt_header;
	CN3UIButton* btn_close;
	CN3UIButton* btn_save[4];
	CN3UIString* txt_info[4];
	CN3UIButton* btn_apply;
	CN3UIButton* btn_edit;

	// Stat
	CN3UIBase* base_stat;
	CN3UIString* stat_txt_title;
	CN3UIButton* stat_btn_save;
	CN3UIButton* stat_btn_cancel;

	CN3UIButton* btn_str[2];
	CN3UIButton* btn_hp[2];
	CN3UIButton* btn_dex[2];
	CN3UIButton* btn_mp[2];
	CN3UIButton* btn_int[2];

	CN3UIString* txt_stat[5];
	CN3UIString* txt_point[5];
	CN3UIString* txt_points;

	uint16 points;
	uint8 basePoint[5];
	uint8 point[5];

public:
	CStatPreset();
	~CStatPreset();
	bool Load(HANDLE hFile);
	bool ReceiveMessage(CN3UIBase* pSender, uint32_t dwMsg);
	uint32_t MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld);
	bool OnKeyPress(int iKey);
	void Tick();
	void Close();
	void Open();
	void UpdatePosition();
	void UncheckAll();
	uint8 GetCheckedIndex();
	void OpenStatSettings(uint8 id = 0);
	uint16 GetTotalPoints();
	void SetPoints();
	void TickPoints();
	void LoadSavedData();
	void GetSavedData(uint8 id = 0);
};