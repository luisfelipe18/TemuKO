#pragma once
class CDropList;
#include "stdafx.h"
#include "TEMUKOACS Engine.h"
#include "N3BASE/N3UIBase.h"
#include "N3BASE/N3UIString.h"
#include "N3BASE/N3UIButton.h"
#include "N3BASE/N3UIProgress.h"
#include "N3BASE/N3UIArea.h"

class CDropList : public CN3UIBase
{
	CN3UIButton* btn_close;
	CN3UIString* txt_mob;

	CN3UIButton* btn_g[6];
	CN3UIProgress* progress_g[6];
	CN3UIString* txt_g[6];

	vector<DropItem> m_Data;

	uint32 item1;
	uint32 item2;
	uint32 item3;
	uint32 item4;
	uint32 item5;
	uint32 item6;

	struct ItemInfo
	{
		uint32 nItemID;
		CN3UIIcon* icon;
		TABLE_ITEM_BASIC* tbl;
		ItemInfo()
		{
			nItemID = 0;
			icon = NULL;
			tbl = nullptr;
		}
	};

	vector<ItemInfo*> items;

public:
	CDropList();
	~CDropList();
	bool Load(HANDLE hFile);
	bool ReceiveMessage(CN3UIBase* pSender, uint32_t dwMsg);
	uint32_t MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld);
	bool OnKeyPress(int iKey);
	void Open();
	void Close();
	void Update(uint16 target, vector<DropItem> drops);
	void Clear();
	__IconItemSkill* GetItemStruct(CN3UIImage* img, std::string fileSz, __TABLE_ITEM_BASIC* tbl, uint32 realID);
};