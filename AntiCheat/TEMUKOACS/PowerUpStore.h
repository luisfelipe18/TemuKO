#pragma once
class CPowerUpStore;
#include "stdafx.h"
#include "TEMUKOACS Engine.h"
#include "N3BASE/N3UIBase.h"
#include "N3BASE/N3UIString.h"
#include "N3BASE/N3UIButton.h"
#include "N3BASE/N3UIImage.h"
#include "N3BASE/N3UIArea.h"

class CPowerUpStore : public CN3UIBase
{
	int page;
	int pageCount;
	int cat;

	uint32 shopping_id;
	uint32 shopping_p;
	uint32 shopping_q;

	CN3UIBase* shopping_confirm;
	CN3UIButton* btn_confirm;
	CN3UIButton* btn_cancel;
	CN3UIImage* shopping_icon;
	CN3UIString* shopping_name;
	CN3UIString* shopping_price;
	CN3UIString* shopping_quantity;
	CN3UIString* shopping_after;

	CN3UIButton* btn_close;
	CN3UIButton* btn_search;
	CN3UIButton* btn_reseller;
	CN3UIButton* btn_next;
	CN3UIButton* btn_previous;
	CN3UIButton* btn_useesn;
	CN3UIString* txt_search;
	CN3UIString* txt_page;
	CN3UIString* txt_cash;
	CN3UIString* txt_bonuscash;
	vector<CN3UIButton*> btn_tabs;

	struct UI_PUSITEM
	{
		uint32 sItemID;
		CN3UIImage* icon;
		CN3UIString* name;
		CN3UIString* price;
		CN3UIString* quantitiy;
		CN3UIButton* purchase;
	};

	vector<UI_PUSITEM> pus_items;
	vector<PUSItem> item_list;
	
public:
	CPowerUpStore();
	~CPowerUpStore();
	bool Load(HANDLE hFile);
	bool ReceiveMessage(CN3UIBase* pSender, uint32_t dwMsg);
	uint32_t MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld);
	bool OnKeyPress(int iKey);
	void Close();
	void Open();
	void OpenShopping();
	void UpdateItemList(vector<PUSItem> items);
	void SetItems(int p = 1, int cat = 0, string q = "");
	void UpdateCash(uint32 kc, uint32 bonus);
	__IconItemSkill* GetItemStruct(CN3UIImage* img, std::string fileSz, __TABLE_ITEM_BASIC* tbl, uint32 realID);
};