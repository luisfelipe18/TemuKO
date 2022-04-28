#pragma once
class CMerchantList;
#include "stdafx.h"
#include "TEMUKOACS Engine.h"
#include "N3BASE/N3UIBase.h"
#include "N3BASE/N3UIString.h"
#include "N3BASE/N3UIButton.h"

struct MerchantData
{
	uint8 type;
	int16 socketID;
	uint32 merchantID;
	string seller;
	uint32 nItemID, count, price;
	bool isKC;
	float x, y, z;
	TABLE_ITEM_BASIC* tbl;
};

class CMerchantList : public CN3UIBase
{
	int page, pageCount;

	CN3UIButton *btn_close, *btn_search, *btn_sort_price, *btn_previous, *btn_next;
	CN3UIEdit* edit_itemname;
	CN3UIString* txt_itemname, *txt_page;

	

	struct MerchantUI
	{
		CN3UIBase* row;
		CN3UIString *txt_seller, *txt_item, *txt_grade, *txt_type, *txt_count, *txt_price;
		CN3UIButton *btn_go, *btn_pm;
	};

	
	MerchantUI mData[14];

public:
	CMerchantList();
	~CMerchantList();
	bool Load(HANDLE hFile);
	bool ReceiveMessage(CN3UIBase* pSender, uint32_t dwMsg);
	uint32_t MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld);
	bool OnKeyPress(int iKey);
	void Close();
	void Open();
	void ClearList();
	void GO(string name);
	void PM(string name);
	void SetItem(uint8 _p);
	vector<MerchantData> merchantList;
};