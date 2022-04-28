#pragma once
class CTempItemList;
#include "stdafx.h"
#include "TEMUKOACS Engine.h"
#include "N3BASE/N3UIBase.h"
#include "N3BASE/N3UIString.h"
#include "N3BASE/N3UIButton.h"
#include "N3BASE/N3UIImage.h"
#include "N3BASE/N3UIArea.h"

struct _SimpleItem
{
	uint32 nItemID;
	uint32 time;
	uint8 pos;
};

class CTempItemList : public CN3UIBase
{
	uint8 page;
	uint8 pageCount;

	CN3UIButton* btn_close;
	CN3UIButton* btn_next;
	CN3UIButton* btn_previous;
	CN3UIString* txt_page;

	struct _TempItem
	{
		uint32 nItemID;
		TABLE_ITEM_BASIC* tbl;
		CN3UIArea* area_icon;
		CN3UIIcon* item_icon;
		CN3UIString* item_name;
		CN3UIString* item_pos;
		CN3UIString* item_time;
	};

	_TempItem* TempItems[3];

public:
	CTempItemList();
	~CTempItemList();
	bool Load(HANDLE hFile);
	bool ReceiveMessage(CN3UIBase* pSender, uint32_t dwMsg);
	uint32_t MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld);
	bool OnKeyPress(int iKey);
	void SetItem(uint8 _p = 1);
	vector<_SimpleItem> itemList;
};