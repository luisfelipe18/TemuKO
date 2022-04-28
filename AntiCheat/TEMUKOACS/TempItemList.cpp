#include "TempItemList.h"
#include "HDRReader.h"

CTempItemList::CTempItemList()
{
	page = 1;
	btn_close = NULL;
	btn_next = NULL;
	btn_previous = NULL;
	
	for (int i = 0; i < 3; i++)
	{
		TempItems[i] = new _TempItem();
		TempItems[i]->nItemID = 0;
		TempItems[i]->item_icon = NULL;
		TempItems[i]->tbl = nullptr;
		TempItems[i]->area_icon = NULL;
		TempItems[i]->item_name = NULL;
		TempItems[i]->item_time = NULL;
		TempItems[i]->item_pos = NULL;
	}
}

CTempItemList::~CTempItemList()
{

}

bool CTempItemList::Load(HANDLE hFile)
{
	if (CN3UIBase::Load(hFile) == false) return false;

	string find = xorstr("btn_close");
	btn_close = (CN3UIButton*)GetChildByID(find);
	find = xorstr("btn_right");
	btn_next = (CN3UIButton*)GetChildByID(find);
	find = xorstr("btn_Left");
	btn_previous = (CN3UIButton*)GetChildByID(find);
	find = xorstr("str_page");
	txt_page = (CN3UIString*)GetChildByID(find);

	for (int i = 0; i < 3; i++)
	{
		find = string_format(xorstr("area_icon_%d"), i);
		TempItems[i]->area_icon = (CN3UIArea*)GetChildByID(find);
		find = string_format(xorstr("str_name_%d"), i);
		TempItems[i]->item_name = (CN3UIString*)GetChildByID(find);
		find = string_format(xorstr("str_Time_%d"), i);
		TempItems[i]->item_time = (CN3UIString*)GetChildByID(find);
		find = string_format(xorstr("str_pos_%d"), i);
		TempItems[i]->item_pos = (CN3UIString*)GetChildByID(find);

		TempItems[i]->item_name->SetString(xorstr("-"));
		TempItems[i]->item_time->SetString(xorstr("-"));
		TempItems[i]->item_pos->SetString(xorstr("-"));
	}

	txt_page->SetString(to_string(page));

	float fUVAspect = (float)45.0f / (float)64.0f;

	for (int i = 0; i < 3; i++)
	{
		TempItems[i]->item_icon = new CN3UIIcon;
		TempItems[i]->item_icon->Init(this);
		TempItems[i]->item_icon->SetUVRect(0, 0, fUVAspect, fUVAspect);
		TempItems[i]->item_icon->SetUIType(UI_TYPE_ICON);
		TempItems[i]->item_icon->SetStyle(UISTYLE_ICON_ITEM | UISTYLE_ICON_CERTIFICATION_NEED);
	}

	Engine->m_UiMgr->SetFocusedUI(this);

	SetPos(Engine->m_UiMgr->GetScreenCenter(this).x, Engine->m_UiMgr->GetScreenCenter(this).y);

	return true;
}

void CTempItemList::SetItem(uint8 _p)
{
	page = _p;
	pageCount = abs(ceil((double)itemList.size() / (double)3));

	time_t UNIXTIME;
	tm g_localTime;
	UNIXTIME = time(nullptr);
	g_localTime = *localtime(&UNIXTIME);

	if (btn_previous == NULL || btn_next == NULL)
		return;

	if (page <= 1)
		btn_previous->SetState(UI_STATE_BUTTON_DISABLE);
	else
		btn_previous->SetState(UI_STATE_BUTTON_NORMAL);

	if(page == pageCount)
		btn_next->SetState(UI_STATE_BUTTON_DISABLE);
	else
		btn_next->SetState(UI_STATE_BUTTON_NORMAL);

	for (int i = 0; i < 3; i++)
	{
		if (TempItems[i]->item_icon == NULL 
			|| TempItems[i]->item_name == NULL 
			|| TempItems[i]->item_pos == NULL 
			|| TempItems[i]->item_time == NULL)
			return;

		TempItems[i]->item_icon->SetTex(xorstr(""));
		TempItems[i]->item_name->SetString(xorstr("-"));
		TempItems[i]->item_pos->SetString(xorstr("-"));
		TempItems[i]->item_time->SetString(xorstr("-"));
	}

	for (uint32 i = 0; i < 3; i++)
	{
		if (3 * (page - 1) + i > itemList.size() - 1 
			|| itemList.size() < i + 1)
			break;
		
		int j = i + 3 * (page - 1);
		TABLE_ITEM_BASIC* tbl = Engine->tblMgr->getItemData(itemList[j].nItemID);
		if (tbl != nullptr)
		{
			TempItems[i]->tbl = tbl;
			TempItems[i]->nItemID = tbl->Num;
			TempItems[i]->item_name->SetString(tbl->strName);

			if(itemList[j].pos == 0)
				TempItems[i]->item_pos->SetString(xorstr("Inventory"));
			else if(itemList[j].pos == 1)
				TempItems[i]->item_pos->SetString(xorstr("Magic Bag"));
			else if(itemList[j].pos == 2)
				TempItems[i]->item_pos->SetString(xorstr("Warehouse"));
			else
				TempItems[i]->item_pos->SetString(xorstr("VIP"));
			
			TempItems[i]->item_time->SetString(to_string((int)ceil((itemList[j].time - UNIXTIME) / 86400)));
			 
			std::vector<char> buffer(256, NULL);

			sprintf(&buffer[0], "ui\\itemicon_%.1d_%.4d_%.2d_%.1d.dxt",
				(tbl->IconID / 10000000),
				(tbl->IconID / 1000) % 10000,
				(tbl->IconID / 10) % 100,
				tbl->IconID % 10);

			std::string szIconFN = &buffer[0];

			SRC dxt = hdrReader->GetSRC(szIconFN);

			if (dxt.sizeInBytes == 0) {
				string iconID = to_string(tbl->Num);
				szIconFN = "itemicon_" + iconID.substr(0, 1) + "_" + iconID.substr(1, 4) + "_" + iconID.substr(5, 2) + "_" + iconID.substr(7, 1) + ".dxt";
			}
			
			
			TempItems[i]->item_icon->SetTex(szIconFN);

			TempItems[i]->item_icon->SetRegion(TempItems[i]->area_icon->GetRegion());
			TempItems[i]->item_icon->SetMoveRect(TempItems[i]->area_icon->GetRegion());
		}
		else {
			TempItems[i]->item_icon->SetTex("");
			TempItems[i]->item_name->SetString(xorstr("-"));
			TempItems[i]->item_pos->SetString(xorstr("-"));
			TempItems[i]->item_time->SetString(xorstr("-"));
			continue;
		}
	}

	txt_page->SetString(to_string(page));
}

bool CTempItemList::ReceiveMessage(CN3UIBase* pSender, uint32_t dwMsg)
{
	if (dwMsg == UIMSG_BUTTON_CLICK)
	{
		if (btn_close == NULL || btn_next == NULL || btn_previous == NULL)
			return true;

		if (pSender == btn_close)
			Release();
		if (pSender == btn_next)
		{
			if (pageCount >= page)
				SetItem(++page);
		}
		else if (pSender == btn_previous)
		{
			if (page > 1)
				SetItem(--page);
		}
	}
	return true;
}

uint32_t CTempItemList::MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld)
{
	uint32_t dwRet = UI_MOUSEPROC_NONE;
	dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);
	return dwRet;
}

bool CTempItemList::OnKeyPress(int iKey)
{
	if (!IsVisible())
		return CN3UIBase::OnKeyPress(iKey);

	switch (iKey)
	{
	case DIK_ESCAPE:
		if(btn_close != NULL) ReceiveMessage(btn_close, UIMSG_BUTTON_CLICK);
		return true;
		break;
	}

	return CN3UIBase::OnKeyPress(iKey);
}
