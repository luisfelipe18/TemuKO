#include "PowerUpStore.h"
#include "HDRReader.h"

bool pus_compare(const PUSItem& a, const PUSItem& b);

CPowerUpStore::CPowerUpStore()
{
	shopping_confirm = NULL;
	btn_close = NULL;
	btn_search = NULL;
	btn_reseller = NULL;
	btn_next = NULL;
	btn_previous = NULL;
	btn_useesn = NULL;
	txt_search = NULL;
	txt_page = NULL;
	txt_cash = NULL;
	txt_bonuscash = NULL;
	page = 1;
	pageCount = 1;
	cat = 0;
}

CPowerUpStore::~CPowerUpStore()
{

}

bool CPowerUpStore::Load(HANDLE hFile)
{
	if (CN3UIBase::Load(hFile) == false) return false;

	// Base
	std::string find = xorstr("shopping_confirm");
	shopping_confirm = (CN3UIBase*)GetChildByID(find);
	find = xorstr("btn_confirm");
	btn_confirm = (CN3UIButton*)shopping_confirm->GetChildByID(find);
	find = xorstr("btn_cancel");
	btn_cancel = (CN3UIButton*)shopping_confirm->GetChildByID(find);
	find = xorstr("shopping_icon");
	shopping_icon = (CN3UIImage*)shopping_confirm->GetChildByID(find);
	find = xorstr("shopping_name");
	shopping_name = (CN3UIString*)shopping_confirm->GetChildByID(find);
	find = xorstr("shopping_price");
	shopping_price = (CN3UIString*)shopping_confirm->GetChildByID(find);
	find = xorstr("shopping_quantity");
	shopping_quantity = (CN3UIString*)shopping_confirm->GetChildByID(find);
	find = xorstr("shopping_after");
	shopping_after = (CN3UIString*)shopping_confirm->GetChildByID(find);

	shopping_confirm->SetVisible(false);

	// Buttons
	find = xorstr("btn_close");
	btn_close = (CN3UIButton*)GetChildByID(find);
	find = xorstr("btn_search");
	btn_search = (CN3UIButton*)GetChildByID(find);
	find = xorstr("btn_reseller");
	btn_reseller = (CN3UIButton*)GetChildByID(find);
	find = xorstr("btn_next");
	btn_next = (CN3UIButton*)GetChildByID(find);
	find = xorstr("btn_previous");
	btn_previous = (CN3UIButton*)GetChildByID(find);
	find = xorstr("btn_useesn");
	btn_useesn = (CN3UIButton*)GetChildByID(find);

	for (int i = 1; i <= 7; i++)
	{
		find = string_format(xorstr("btn_tab%d"), i);
		CN3UIButton* tmp = (CN3UIButton*)GetChildByID(find);
		btn_tabs.push_back(tmp);
	}

	btn_tabs[0]->SetState(UI_STATE_BUTTON_DISABLE);

	// Texts
	find = xorstr("edit_search");
	CN3UIEdit* tmp = (CN3UIEdit*)GetChildByID(find);
	find = xorstr("txt_search");
	txt_search = (CN3UIString*)tmp->GetChildByID(find);
	find = xorstr("txt_page");
	txt_page = (CN3UIString*)GetChildByID(find);
	find = xorstr("txt_cash");
	txt_cash = (CN3UIString*)GetChildByID(find);
	find = xorstr("txt_bonuscash");
	txt_bonuscash = (CN3UIString*)GetChildByID(find);

	// Pus Items

	for (int i = 1; i <= 4; i++)
	{
		find = string_format(xorstr("items%d"), i);
		CN3UIBase* itemGroup = (CN3UIBase*)GetChildByID(find);
		for (int j = 1; j <= 5; j++)
		{
			find = string_format(xorstr("item%d"), j);
			CN3UIImage* item = (CN3UIImage*)itemGroup->GetChildByID(find);

			UI_PUSITEM pus_item;
			find = xorstr("item_icon");
			pus_item.icon = (CN3UIImage*)item->GetChildByID(find);
			find = xorstr("item_name");
			pus_item.name = (CN3UIString*)item->GetChildByID(find);
			find = xorstr("item_price");
			pus_item.price = (CN3UIString*)item->GetChildByID(find);
			find = xorstr("item_quantitiy");
			pus_item.quantitiy = (CN3UIString*)item->GetChildByID(find);
			find = xorstr("btn_purchase");
			pus_item.purchase = (CN3UIButton*)item->GetChildByID(find);

			pus_item.name->SetString(xorstr("-"));
			pus_item.price->SetString(xorstr("-"));
			pus_item.quantitiy->SetString(xorstr("-"));
			pus_item.icon->GetParent()->SetVisible(false);

			pus_items.push_back(pus_item);
		}
	}

	txt_page->SetString(to_string(page));

	SetPos(Engine->m_UiMgr->GetScreenCenter(this).x, Engine->m_UiMgr->GetScreenCenter(this).y);
	Engine->m_UiMgr->SetFocusedUI(this);

	return true;
}

bool CPowerUpStore::ReceiveMessage(CN3UIBase* pSender, uint32_t dwMsg)
{
	if (dwMsg == UIMSG_BUTTON_CLICK)
	{
		if (pSender == btn_close)
			Close();
		else if (pSender == btn_next) {
			if (page < pageCount)
			{
				page++;
				SetItems(page, cat);
			}
		}
		else if (pSender == btn_previous) {
			if (page > 1)
			{
				page--;
				SetItems(page, cat);
			}
		}
		else if (pSender == btn_search) {
			SetItems(page, cat, txt_search->GetString());
		}
		else if (pSender == btn_confirm)
		{
			Packet result(WIZ_HOOK_GUARD, uint8(WIZ_HOOK_PUS));
			result << uint8(2) << uint32(shopping_id);
			Engine->Send(&result);
			shopping_confirm->SetVisible(false);
			btn_confirm->SetState(UI_STATE_BUTTON_NORMAL);
			btn_cancel->SetState(UI_STATE_BUTTON_NORMAL);
		}
		else if (pSender == btn_cancel) {
			shopping_confirm->SetVisible(false);
			btn_confirm->SetState(UI_STATE_BUTTON_NORMAL);
			btn_cancel->SetState(UI_STATE_BUTTON_NORMAL);
		}
		else if (pSender == btn_useesn)
		{
			if (Engine->uiTaskbarMain != NULL) {
				Engine->uiTaskbarMain->OpenPPCard();
				Close();
			}
		}
		else if (pSender == btn_reseller)
		{
			ShellExecute(NULL, xorstr("open"), Engine->EngineSettings->URL_KCbayi.c_str(), NULL, NULL, SW_SHOWNORMAL);
		}
		else {
			for (int i = 0; i < btn_tabs.size(); i++)
			{

				if (pSender == btn_tabs[i])
				{
					for (int j = 0; j < btn_tabs.size(); j++)
						btn_tabs[j]->SetState(UI_STATE_BUTTON_NORMAL);

					btn_tabs[i]->SetState(UI_STATE_BUTTON_DISABLE);
					page = 1;
					cat = i;
					SetItems(page, cat);
				}
			}
			for (int i = 0; i < 20; i++)
			{
				if (pus_items[i].purchase != NULL)
				{
					if (pSender == pus_items[i].purchase)
					{
						shopping_id = pus_items[i].sItemID;
						for (int i = 0; i < item_list.size(); i++)
						{
							if (item_list[i].sID == shopping_id)
							{
								shopping_p = item_list[i].Price;
								shopping_q = item_list[i].Quantitiy;
								break;
							}
						}
						OpenShopping();
					}
				}
			}
		}
	}

	return true;
}

void CPowerUpStore::OpenShopping() {
	shopping_confirm->SetState(UI_STATE_BUTTON_NORMAL);
	if (Engine->Player.KnightCash < shopping_p)
		if (Engine->Player.KnightCashBonus < shopping_p)
			shopping_confirm->SetState(UI_STATE_BUTTON_DISABLE);

	TABLE_ITEM_BASIC* tbl = Engine->tblMgr->getItemData(shopping_id);
	if (tbl != nullptr)
	{
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

		shopping_icon->SetTex(szIconFN);
		shopping_name->SetString(tbl->strName);
		shopping_price->SetString(string_format(xorstr("Price: %s"), Engine->StringHelper->NumberFormat(shopping_p).c_str()));
		shopping_quantity->SetString(string_format(xorstr("Quantity: %s"), Engine->StringHelper->NumberFormat(shopping_q).c_str()));
		shopping_after->SetString(string_format(xorstr("%d-%d = %s"), Engine->Player.KnightCash, shopping_p, Engine->StringHelper->NumberFormat(Engine->Player.KnightCash - shopping_p).c_str()));
	}
	shopping_confirm->SetVisible(true);
}

uint32_t CPowerUpStore::MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld)
{
	uint32_t dwRet = UI_MOUSEPROC_NONE;
	if (Engine->m_UiMgr->m_FocusedUI == this)
	{
		bool showTooltip = false;
		bool pass = false;

		if (shopping_icon != NULL)
		{
			if (shopping_icon->IsIn(ptCur.x, ptCur.y) && shopping_id > 0)
			{
				TABLE_ITEM_BASIC* tbl = Engine->tblMgr->getItemData(shopping_id);
				if (tbl != nullptr) {
					Engine->m_UiMgr->m_pUITooltipDlg->DisplayTooltipsEnable(ptCur.x, ptCur.y, GetItemStruct(shopping_icon, shopping_icon->FileName(), tbl, shopping_id), this, false, false, false);
					showTooltip = true;
					pass = true;
				}
			}
		}

		if (shopping_confirm->IsIn(ptCur.x, ptCur.y) && shopping_confirm->IsVisible())
			pass = true;

		if (!pass) {
			for (auto it : pus_items)
			{
				if (it.icon != NULL && it.sItemID > 0) {
					if (it.icon->IsIn(ptCur.x, ptCur.y) && it.icon->GetParent()->IsVisible())
					{
						TABLE_ITEM_BASIC* tbl = Engine->tblMgr->getItemData(it.sItemID);
						if (tbl != nullptr) {
							Engine->m_UiMgr->m_pUITooltipDlg->DisplayTooltipsEnable(ptCur.x, ptCur.y, GetItemStruct(it.icon, it.icon->FileName(), tbl, it.sItemID), this, false, false, false);
							showTooltip = true;
							break;
						}
					}
				}
			}
		}
		Engine->m_UiMgr->m_pUITooltipDlg->SetVisible(showTooltip);
	}
	dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);
	return dwRet;
}

bool CPowerUpStore::OnKeyPress(int iKey)
{
	if (!IsVisible())
		return CN3UIBase::OnKeyPress(iKey);

	switch (iKey)
	{
	case DIK_ESCAPE:
		Close();
		return true;
		break;
	case DIK_RETURN:
		ReceiveMessage(btn_search, UIMSG_BUTTON_CLICK);
		return true;
		break;
	}

	return CN3UIBase::OnKeyPress(iKey);
}

void CPowerUpStore::Open()
{
	txt_cash->SetString(Engine->StringHelper->NumberFormat(Engine->Player.KnightCash).c_str());
	txt_bonuscash->SetString(Engine->StringHelper->NumberFormat(Engine->Player.KnightCashBonus).c_str());
	SetItems();
	SetVisible(true);
}

void CPowerUpStore::Close()
{
	SetVisible(false);
}

void CPowerUpStore::UpdateItemList(vector<PUSItem> items)
{
	item_list = items;
	page = 1;
	pageCount = abs(ceil((double)item_list.size() / (double)20));
}

void CPowerUpStore::SetItems(int p, int cat, string q)
{
	vector<PUSItem> tmpList;
	for (int i = 0; i < item_list.size(); i++)
	{
		if (cat > 0 && q == "") {
			if (item_list[i].Category == cat)
				tmpList.push_back(item_list[i]);
		}
		else if (cat == 0 && q != "")
		{
			TABLE_ITEM_BASIC* tbl = Engine->tblMgr->getItemData(item_list[i].sID);
			if (tbl != nullptr)
			{
				if (Engine->StringHelper->IsContains(tbl->strName, q))
					tmpList.push_back(item_list[i]);
			}
		}
		else if (cat > 0 && q != "")
		{
			if (item_list[i].Category == cat) {
				TABLE_ITEM_BASIC* tbl = Engine->tblMgr->getItemData(item_list[i].sID);
				if (tbl != nullptr)
				{
					if (Engine->StringHelper->IsContains(tbl->strName, q))
						tmpList.push_back(item_list[i]);
				}
			}
		}
		else tmpList.push_back(item_list[i]);
	}

	if (tmpList.size() == 0)
	{
		for(int i = 0; i < 20; i++)
			pus_items[i].icon->GetParent()->SetVisible(false);
		txt_page->SetString(to_string(page));
		pageCount = 1;
		return;
	}

	// Fiyati buyukten kucuge
	std::sort(tmpList.begin(), tmpList.end(), pus_compare);
	
	pageCount = abs(ceil((double)tmpList.size() / (double)20));

	int begin = (p - 1) * 20;
	int j = -1;
	for (int i = begin; i < begin+20; i++)
	{
		j++;
		if (j > 19) break;
		if (i > tmpList.size() - 1)
		{
			pus_items[j].icon->GetParent()->SetVisible(false);
			continue;
		}

		TABLE_ITEM_BASIC* tbl = Engine->tblMgr->getItemData(tmpList[i].sID);
		if (tbl == nullptr)
		{
			pus_items[j].icon->GetParent()->SetVisible(false);
			continue;
		}

		std::vector<char> buffer(256, NULL);

		sprintf(&buffer[0], "ui\\itemicon_%.1d_%.4d_%.2d_%.1d.dxt",
			(tbl->dxtID / 10000000),
			(tbl->dxtID / 1000) % 10000,
			(tbl->dxtID / 10) % 100,
			tbl->dxtID % 10);

		std::string szIconFN = &buffer[0];

		SRC dxt = hdrReader->GetSRC(szIconFN);

		if (dxt.sizeInBytes > 0) {
			pus_items[j].icon->SetTex(szIconFN);
		} else pus_items[j].icon->SetTex("");

		pus_items[j].sItemID = tmpList[i].sID;
		
		pus_items[j].name->SetString(tbl->strName);
		pus_items[j].price->SetString(string_format(xorstr("Price: %s"), Engine->StringHelper->NumberFormat(tmpList[i].Price).c_str()));
		pus_items[j].quantitiy->SetString(string_format(xorstr("Quantity: %s"), Engine->StringHelper->NumberFormat(tmpList[i].Quantitiy).c_str()));

		bool enoughtMoney = true;

		if (Engine->Player.KnightCash < tmpList[i].Price)
			if (Engine->Player.KnightCashBonus < tmpList[i].Price)
				enoughtMoney = false;

		if (enoughtMoney) {
			pus_items[j].price->SetColor(D3DCOLOR_RGBA(146, 252, 175, 255));
			pus_items[j].purchase->SetState(UI_STATE_BUTTON_NORMAL);
		}
		else {
			pus_items[j].price->SetColor(D3DCOLOR_RGBA(247, 141, 173, 255));
			pus_items[j].purchase->SetState(UI_STATE_BUTTON_DISABLE);
		}

		pus_items[j].icon->GetParent()->SetVisible(true);

	}
	txt_page->SetString(to_string(page));
}

void CPowerUpStore::UpdateCash(uint32 kc, uint32 bonus)
{
	txt_cash->SetString(Engine->StringHelper->NumberFormat(kc).c_str());
	txt_bonuscash->SetString(Engine->StringHelper->NumberFormat(bonus).c_str());
}

__IconItemSkill* CPowerUpStore::GetItemStruct(CN3UIImage* img, std::string fileSz, __TABLE_ITEM_BASIC* tbl, uint32 realID)
{
	__IconItemSkill* newStruct = new __IconItemSkill;
	newStruct->pUIIcon = img;
	newStruct->szIconFN = fileSz;
	newStruct->index = 0;

	__TABLE_ITEM_BASIC* newItemBasic = new __TABLE_ITEM_BASIC;
	__TABLE_ITEM_BASIC* itemOrg = tbl;
	newItemBasic->Num = itemOrg->Num;
	newItemBasic->extNum = itemOrg->extNum;
	newItemBasic->strName = itemOrg->strName;
	newItemBasic->Description = itemOrg->Description;

	newItemBasic->Class = itemOrg->Class;

	newItemBasic->Attack = itemOrg->Attack;
	newItemBasic->Delay = itemOrg->Delay;
	newItemBasic->Range = itemOrg->Range;
	newItemBasic->Weight = itemOrg->Weight;
	newItemBasic->Duration = itemOrg->Duration;
	newItemBasic->repairPrice = itemOrg->repairPrice;
	newItemBasic->sellingPrice = itemOrg->sellingPrice;
	newItemBasic->AC = itemOrg->AC;
	newItemBasic->isCountable = itemOrg->isCountable;
	newItemBasic->ReqLevelMin = itemOrg->ReqLevelMin;

	newItemBasic->ReqStr = itemOrg->ReqStr;
	newItemBasic->ReqHp = itemOrg->ReqHp;
	newItemBasic->ReqDex = itemOrg->ReqDex;
	newItemBasic->ReqInt = itemOrg->ReqInt;
	newItemBasic->ReqMp = itemOrg->ReqMp;

	newItemBasic->SellingGroup = itemOrg->SellingGroup;

	newStruct->pItemBasic = newItemBasic;

	__TABLE_ITEM_EXT* newItemExt = Engine->tblMgr->getExtData(itemOrg->extNum, realID);

	if (newItemExt == nullptr)
	{
		newItemExt = new __TABLE_ITEM_EXT;
		newItemExt->extensionID = itemOrg->extNum;
		newItemExt->szHeader = itemOrg->strName;
		newItemExt->byMagicOrRare = itemOrg->ItemGrade;
		newItemExt->siDamage = 0;
		newItemExt->siAttackIntervalPercentage = 100;
		newItemExt->siHitRate = 0;
		newItemExt->siEvationRate = 0;
		newItemExt->siMaxDurability = 0;
		newItemExt->siDefense = 0;
		newItemExt->siDefenseRateDagger = 0;
		newItemExt->siDefenseRateSword = 0;
		newItemExt->siDefenseRateBlow = 0;
		newItemExt->siDefenseRateAxe = 0;
		newItemExt->siDefenseRateSpear = 0;
		newItemExt->siDefenseRateArrow = 0;
		newItemExt->byDamageFire = 0;
		newItemExt->byDamageIce = 0;
		newItemExt->byDamageThuner = 0;
		newItemExt->byDamagePoison = 0;
		newItemExt->byStillHP = 0;
		newItemExt->byDamageMP = 0;
		newItemExt->byStillMP = 0;
		newItemExt->siBonusStr = 0;
		newItemExt->siBonusSta = 0;
		newItemExt->siBonusHP = 0;
		newItemExt->siBonusDex = 0;
		newItemExt->siBonusMSP = 0;
		newItemExt->siBonusInt = 0;
		newItemExt->siBonusMagicAttak = 0;
		newItemExt->siRegistFire = 0;
		newItemExt->siRegistIce = 0;
		newItemExt->siRegistElec = 0;
		newItemExt->siRegistMagic = 0;
		newItemExt->siRegistPoison = 0;
		newItemExt->siRegistCurse = 0;
		newItemExt->siNeedLevel = 0;
	}

	newStruct->pItemExt = newItemExt;

	return newStruct;
}

bool pus_compare(const PUSItem& a, const PUSItem& b)
{
	if (a.Price != b.Price)
		return a.Price > b.Price;
	else
		return a.sID > b.sID;
}