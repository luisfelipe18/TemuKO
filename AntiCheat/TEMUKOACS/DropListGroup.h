#pragma once
class CDropListGroup;
#include "stdafx.h"
#include "TEMUKOACS Engine.h"
#include "N3BASE/N3UIBase.h"
#include "N3BASE/N3UIString.h"
#include "N3BASE/N3UIButton.h"
#include "N3BASE/N3UIProgress.h"
#include "N3BASE/N3UIImage.h"

class CDropListGroup : public CN3UIBase
{
	CN3UIButton* btn_close;
	CN3UIString* txt_droprate;
	CN3UIProgress* progress_per;

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
	CDropListGroup();
	bool Load(HANDLE hFile);
	bool ReceiveMessage(CN3UIBase* pSender, uint32_t dwMsg);
	uint32_t MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld);
	bool OnKeyPress(int iKey);
	void Close();
	void Open();
	void Update(vector<uint32> items);
	__IconItemSkill* GetItemStruct(CN3UIImage* img, std::string fileSz, __TABLE_ITEM_BASIC* tbl, uint32 realID);
};