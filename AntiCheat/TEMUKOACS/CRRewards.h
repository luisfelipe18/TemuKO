#pragma once
class CCollectionRaceRewards;
#include "stdafx.h"
#include "TEMUKOACS Engine.h"
#include "N3BASE/N3UIBase.h"
#include "N3BASE/N3UIString.h"
#include "N3BASE/N3UIButton.h"

class CCollectionRaceRewards : public CN3UIBase
{
	CN3UIString* txt_header;
	CN3UIButton* btn_close;
	CN3UIArea* slot_i[2];
	CN3UIArea* slot_r[4];
	CN3UIString* txt_name_i[2];
	CN3UIString* txt_name_r[4];
	CN3UIString* txt_count_i[2];
	CN3UIString* txt_count_r[4];

	struct CRItemData
	{
		uint32 itemID;
		TABLE_ITEM_BASIC* tbl;
		CN3UIIcon* icon;
	};

	CRItemData m_items[6];
public:
	CCollectionRaceRewards();
	~CCollectionRaceRewards();
	bool Load(HANDLE hFile);
	bool ReceiveMessage(CN3UIBase* pSender, uint32_t dwMsg);
	uint32_t MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld);
	bool OnKeyPress(int iKey);
	void Close();
	void Open();
	void LoadItems(uint32 items[2], uint32 resources[4], uint32 counts[6]);
	__IconItemSkill* GetItemStruct(CN3UIImage* img, std::string fileSz, __TABLE_ITEM_BASIC* tbl, uint32 realID);
};