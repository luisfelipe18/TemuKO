#pragma once
class CSearchMonster;
#include "stdafx.h"
#include "TEMUKOACS Engine.h"
#include "N3BASE/N3UIBase.h"
#include "N3BASE/N3UIString.h"
#include "N3BASE/N3UIButton.h"
#include "N3BASE/N3UIScrollBar.h"

class CSearchMonster : public CN3UIBase
{
	CN3UIButton* btn_close, * btn_search, * btn_drops;
	CN3UIEdit* edit_search;

	CN3UIButton* items[5];
	CN3UIScrollBar* list_scroll;

	CN3UIString* txt_monster_name, * txt_level, * txt_hp, * txt_ap, * txt_ac;

	struct MONSTER_INFO
	{
		uint16 sID;
		std::string strName;
		MONSTER_INFO(uint16 id, std::string name)
		{
			sID = id;
			strName = name;
		}
	};

	vector<MONSTER_INFO> monsters;

	uint32 page;
	bool needToReset;
public:
	CSearchMonster();
	~CSearchMonster();
	bool Load(HANDLE hFile);
	bool ReceiveMessage(CN3UIBase* pSender, uint32_t dwMsg);
	uint32_t MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld);
	bool OnKeyPress(int iKey);
	void Close();
	void Open();
	void Update(Packet pkt);
	void ListOrder();
	void Tick();
	CN3UIString* GetItemText(CN3UIButton* item);
};