#pragma once
class CSkillPreset;
#include "stdafx.h"
#include "TEMUKOACS Engine.h"
#include "N3BASE/N3UIBase.h"
#include "N3BASE/N3UIString.h"
#include "N3BASE/N3UIButton.h"

class CSkillPreset : public CN3UIBase
{
	bool posSet;
	uint8 selectedIndex;
	CN3UIString* txt_header;
	CN3UIButton* btn_close;
	CN3UIButton* btn_save[4] = { NULL,NULL,NULL,NULL };
	CN3UIString* txt_info[4] = { NULL,NULL,NULL,NULL };
	CN3UIButton* btn_apply;
	CN3UIButton* btn_edit;

	// Stat
	CN3UIBase* base_stat;
	CN3UIString* stat_txt_title;
	CN3UIButton* stat_btn_save;
	CN3UIButton* stat_btn_cancel;

	CN3UIButton* btn_str[2] = { NULL, NULL };
	CN3UIButton* btn_hp[2] = { NULL, NULL };
	CN3UIButton* btn_dex[2] = { NULL, NULL };
	CN3UIButton* btn_mp[2] = { NULL, NULL };
	CN3UIButton* btn_int[2] = { NULL, NULL };

	CN3UIString* txt_stat[4] = { NULL,NULL,NULL,NULL };
	CN3UIString* txt_point[4] = { NULL,NULL,NULL,NULL };
	CN3UIString* txt_points;

	uint16 points = 0;
	uint8 ceilPoint[4] = { 80,80,80,23 };
	uint8 point[4] = { 0,0,0,0 };

	string skillText[3] = { "","","" };

public:
	CSkillPreset();
	~CSkillPreset();
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