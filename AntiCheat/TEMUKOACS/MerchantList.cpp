#include "MerchantList.h"

CMerchantList::CMerchantList()
{

}

CMerchantList::~CMerchantList()
{

}

bool CMerchantList::Load(HANDLE hFile)
{
	if (CN3UIBase::Load(hFile) == false) return false;

	string find = xorstr("btn_close");
	btn_close = (CN3UIButton*)GetChildByID(find);
	find = xorstr("btn_search");
	btn_search = (CN3UIButton*)GetChildByID(find);
	find = xorstr("btn_sort_price");
	btn_sort_price = (CN3UIButton*)GetChildByID(find);
	find = xorstr("edit_itemname");
	edit_itemname = (CN3UIEdit*)GetChildByID(find);
	find = xorstr("txt_itemname");
	txt_itemname = (CN3UIString*)edit_itemname->GetChildByID(find);
	find = xorstr("btn_previous");
	btn_previous = (CN3UIButton*)GetChildByID(find);
	find = xorstr("btn_next");
	btn_next = (CN3UIButton*)GetChildByID(find);
	find = xorstr("txt_page");
	txt_page = (CN3UIString*)GetChildByID(find);

	for (int i = 0; i < 14; i++)
	{
		find = string_format(xorstr("row_%d"), i+1);
		mData[i].row = (CN3UIBase*)GetChildByID(find);

		// Row Parts
		find = xorstr("txt_seller");
		mData[i].txt_seller = (CN3UIString*)mData[i].row->GetChildByID(find);
		find = xorstr("txt_item");
		mData[i].txt_item = (CN3UIString*)mData[i].row->GetChildByID(find);
		find = xorstr("txt_grade");
		mData[i].txt_grade = (CN3UIString*)mData[i].row->GetChildByID(find);
		find = xorstr("txt_type");
		mData[i].txt_type = (CN3UIString*)mData[i].row->GetChildByID(find);
		find = xorstr("txt_count");
		mData[i].txt_count = (CN3UIString*)mData[i].row->GetChildByID(find);
		find = xorstr("txt_price");
		mData[i].txt_price = (CN3UIString*)mData[i].row->GetChildByID(find);
		find = xorstr("btn_go");
		mData[i].btn_go = (CN3UIButton*)mData[i].row->GetChildByID(find);
		find = xorstr("btn_pm");
		mData[i].btn_pm = (CN3UIButton*)mData[i].row->GetChildByID(find);
	}

	page = 1;
	
	txt_page->SetString(to_string(page));

	ClearList();

	SetPos(Engine->m_UiMgr->GetScreenCenter(this).x, Engine->m_UiMgr->GetScreenCenter(this).y);

	return true;
}

void CMerchantList::ClearList()
{
	for (int i = 0; i < 14; i++)
	{
		mData[i].txt_seller->SetString("");
		mData[i].txt_item->SetString("");
		mData[i].txt_grade->SetString("");
		mData[i].txt_type->SetString("");
		mData[i].txt_count->SetString("");
		mData[i].txt_price->SetString("");
		mData[i].btn_go->SetVisible(false);
		mData[i].btn_pm->SetVisible(false);
	}
}

void CMerchantList::SetItem(uint8 _p)
{
	page = _p;
	pageCount = abs(ceil((double)merchantList.size() / (double)14));

	ClearList();

	for (uint32 i = 0; i < 14; i++)
	{
		if (14 * (page - 1) + i > merchantList.size() - 1 || merchantList.size() < i + 1)
			break;

		int j = i + 14 * (page - 1);
		TABLE_ITEM_BASIC* tbl = Engine->tblMgr->getItemData(merchantList[j].nItemID);
		if (tbl != nullptr)
		{
			merchantList[j].tbl = tbl;
			mData[i].txt_seller->SetString(merchantList[j].seller);
			mData[i].txt_item->SetString(tbl->strName);

			TABLE_ITEM_EXT* ext = Engine->tblMgr->getExtData(tbl->extNum, merchantList[j].nItemID);

			if (ext != nullptr) 
			{
				if (ext->extensionID % 10 != 0)
				{
					char szExtID[20];
					sprintf(szExtID, "+%d", ext->extensionID % 10);
					mData[i].txt_grade->SetString(szExtID);
				}
				else
				{
					mData[i].txt_grade->SetString("-");
				}
			}

			mData[i].txt_type->SetString(merchantList[j].type == 0 ? "Sell" : "Buy");
			mData[i].txt_count->SetString(to_string(merchantList[j].count));

			if(merchantList[j].isKC)
				mData[i].txt_price->SetString(string_format(xorstr("%s kniht cash"), Engine->StringHelper->NumberFormat(merchantList[j].price).c_str()));
			else
				mData[i].txt_price->SetString(string_format(xorstr("%s coins"), Engine->StringHelper->NumberFormat(merchantList[j].price).c_str()));

			mData[i].btn_go->SetVisible(true);
			mData[i].btn_pm->SetVisible(true);
		}
	}

	txt_page->SetString(to_string(page));
}

bool CMerchantList::ReceiveMessage(CN3UIBase* pSender, uint32_t dwMsg)
{
	if (dwMsg == UIMSG_BUTTON_CLICK)
	{
		if (pSender == btn_close)
			Close();

		for (int i = 0; i < 14; i++)
		{
			if (pSender == mData[i].btn_go)
				GO(merchantList[i*page].seller);
			else if (pSender == mData[i].btn_pm)
				PM(merchantList[i*page].seller);
		}
	}

	return true;
}

void CMerchantList::GO(string name) {
	Packet result(WIZ_HOOK_GUARD, uint8(WIZ_HOOK_MERCHANTLIST));
	result << uint8(1) << name;
	Engine->Send(&result);
}
void CMerchantList::PM(string name) {
	Packet result(WIZ_HOOK_GUARD, uint8(WIZ_HOOK_MERCHANTLIST));
	result << uint8(2) << name;
	Engine->Send(&result);
}

uint32_t CMerchantList::MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld)
{
	uint32_t dwRet = UI_MOUSEPROC_NONE;
	dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);
	return dwRet;
}

bool CMerchantList::OnKeyPress(int iKey)
{
	if (!IsVisible())
		return CN3UIBase::OnKeyPress(iKey);

	return true;
}

void CMerchantList::Open()
{
	SetVisible(true);
}

void CMerchantList::Close()
{
	SetVisible(false);
}