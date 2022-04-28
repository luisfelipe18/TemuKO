#pragma once
class CDropResult;
#include "stdafx.h"
#include "TEMUKOACS Engine.h"
#include "N3BASE/N3UIBase.h"
#include "N3BASE/N3UIString.h"
#include "N3BASE/N3UIButton.h"
#include "N3BASE/N3UIImage.h"

class CDropResult : public CN3UIBase
{
	struct DropItems
	{
		uint32 nItemID;
		CN3UIImage* icon;
		TABLE_ITEM_BASIC* tbl;
	};

	CN3UIString* txt_title;
	CN3UIButton* btn_close;
	DropItems dropitems[5];

public:
	CDropResult();
	~CDropResult();
	bool Load(HANDLE hFile);
	bool ReceiveMessage(CN3UIBase* pSender, uint32_t dwMsg);
	uint32_t MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld);
	bool OnKeyPress(int iKey);
	void Close();
	void Open();
	void SetTitle(std::string text);
	void AddItem(uint32 itemID);
	__IconItemSkill* GetItemStruct(CN3UIImage* img, std::string fileSz, __TABLE_ITEM_BASIC* tbl, uint32 realID);
};