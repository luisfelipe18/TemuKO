#include "DropList.h"
//#include <xtgmath.h>
#include "HDRReader.h"

CDropList::CDropList()
{
	btn_close = NULL;
	txt_mob = NULL;
	item1 = 0;
	item2 = 0;
	item3 = 0;
	item4 = 0;
	item5 = 0;
	item6 = 0;
}

CDropList::~CDropList()
{

}

bool CDropList::Load(HANDLE hFile)
{
	if (CN3UIBase::Load(hFile) == false) return false;

	string find = xorstr("str_head");
	txt_mob = (CN3UIString*)GetChildByID(find);

	// Buttons
	find = xorstr("btn_close");
	btn_close = (CN3UIButton*)GetChildByID(find);
	for (int i = 1; i <= 6; i++)
	{
		find = string_format(xorstr("btn_g%d"), i);
		btn_g[i-1] = (CN3UIButton*)GetChildByID(find);
	}

	// Progress
	for (int i = 1; i <= 6; i++)
	{
		find = string_format(xorstr("progress_g%d"), i);
		progress_g[i - 1] = (CN3UIProgress*)GetChildByID(find);	
		progress_g[i - 1]->SetRange(0, 100);
	}

	// Strings
	for (int i = 1; i <= 6; i++)
	{
		find = string_format(xorstr("txt_per%d"), i);
		txt_g[i - 1] = (CN3UIString*)GetChildByID(find);
	}

	float fUVAspect = (float)45.0f / (float)64.0f;

	for (int i = 1; i <= 6; i++)
	{
		find = string_format(xorstr("slot_%d"), i);
		CN3UIArea* area = (CN3UIArea*)GetChildByID(find);

		CN3UIIcon* icon = new CN3UIIcon();
		icon->Init(this);
		icon->SetUIType(UI_TYPE_ICON);
		icon->SetStyle(UISTYLE_ICON_ITEM | UISTYLE_ICON_CERTIFICATION_NEED);
		icon->SetUVRect(0, 0, fUVAspect, fUVAspect);
		icon->SetRegion(area->GetRegion());

		ItemInfo* inf = new ItemInfo();
		inf->icon = icon;
		inf->tbl = NULL;
		inf->nItemID = 0;

		items.push_back(inf);
	}

	SetMoveRect(txt_mob->GetRegion());
	SetPos(Engine->m_UiMgr->GetScreenCenter(this).x, Engine->m_UiMgr->GetScreenCenter(this).y);

	Close();

	if (Engine->m_PlayerBase == NULL) {
		Engine->m_PlayerBase = new CPlayerBase();
		Packet result(WIZ_HOOK_GUARD, uint8(WIZ_HOOK_USERDATA));
		Engine->Send(&result);
	}

	return true;
}

bool CDropList::ReceiveMessage(CN3UIBase* pSender, uint32_t dwMsg)
{
	if (dwMsg == UIMSG_BUTTON_CLICK)
	{
		
		if (pSender == btn_close)
		{
			Close();
		}
		for (int i = 0; i < 6; i++)
		{
			if (pSender == btn_g[i])
			{
				ItemInfo* inf = items[i];
				if (inf->tbl != nullptr)
					return true;

				if (inf->nItemID < 1 
					|| inf->nItemID > 100000)
					return true;

				Packet result(WIZ_HOOK_GUARD, uint8(WIZ_HOOK_DROP_REQUEST));
				result << uint8(2) << uint32(inf->nItemID);
				Engine->Send(&result);

				break;
			}
		}
	}

	return true;
}

uint32_t CDropList::MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld)
{

	uint32_t dwRet = UI_MOUSEPROC_NONE;

	if (Engine->m_UiMgr->m_FocusedUI == this)
	{
		bool showTooltip = false;

		for (auto it : items)
		{
			if (it->icon->IsIn(ptCur.x, ptCur.y) && it->tbl != nullptr)
			{
				Engine->m_UiMgr->m_pUITooltipDlg->DisplayTooltipsEnable(ptCur.x, ptCur.y, GetItemStruct(it->icon, it->icon->FileName(), it->tbl, it->nItemID), this, false, true, true);
				showTooltip = true;
				break;
			}
		}

		Engine->m_UiMgr->m_pUITooltipDlg->SetVisible(showTooltip);
	}

	dwRet = CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);
	return dwRet;
}

bool CDropList::OnKeyPress(int iKey)
{
	if (!IsVisible())
		return CN3UIBase::OnKeyPress(iKey);

	switch (iKey)
	{
	case DIK_ESCAPE:
		ReceiveMessage(btn_close, UIMSG_BUTTON_CLICK);
		return true;
	}

	return CN3UIBase::OnKeyPress(iKey);
}

void CDropList::Open()
{
	SetVisible(true);
	SetPos(Engine->m_UiMgr->GetScreenCenter(this).x, Engine->m_UiMgr->GetScreenCenter(this).y);
	Engine->m_UiMgr->SetFocusedUI(this);
}

void CDropList::Close()
{
	SetVisible(false);
	SetPos(Engine->m_UiMgr->GetScreenCenter(this).x, Engine->m_UiMgr->GetScreenCenter(this).y);
}

void CDropList::Clear()
{
	for (int i = 0; i < items.size(); i++)
	{
		items[i]->icon->SetTex("");
		items[i]->nItemID = 0;
		items[i]->tbl = nullptr;
		btn_g[i]->SetState(UI_STATE_BUTTON_DISABLE);
		progress_g[i]->SetCurValue(0);
		txt_g[i]->SetString(xorstr("0%"));
	}
}

void CDropList::Update(uint16 target, vector<DropItem> drops)
{
	m_Data = drops;
	TABLE_MOB* mob = Engine->tblMgr->getMobData(target);
	if(mob != nullptr)
		txt_mob->SetString(mob->strName.c_str());
	else
		txt_mob->SetString(xorstr("<unknown>"));

	Clear();

	int slot = 0;
	for (auto drop : m_Data)
	{
		if (drop.nItemID == 0 || drop.sPercent == 0)
			continue;

		TABLE_ITEM_BASIC* tbl = Engine->tblMgr->getItemData(drop.nItemID);
		if (tbl == nullptr)
		{
			if (drop.nItemID > 0 && drop.nItemID < 100000000) // item group
			{
				items[slot]->icon->SetTex(xorstr("UI\\itemicon_group.dxt"));
				items[slot]->tbl = nullptr;
				btn_g[slot]->SetState(UI_STATE_BUTTON_NORMAL);
			}
			else continue;
		}
		else {
			std::vector<char> buffer(256, NULL);

			sprintf(&buffer[0], "ui\\itemicon_%.1d_%.4d_%.2d_%.1d.dxt",
				(tbl->dxtID / 10000000),
				(tbl->dxtID / 1000) % 10000,
				(tbl->dxtID / 10) % 100,
				tbl->dxtID % 10);

			std::string szIconFN = &buffer[0];

			SRC dxt = hdrReader->GetSRC(szIconFN);

			if (dxt.sizeInBytes == 0) {
				string iconID = to_string(tbl->Num);
				szIconFN = "itemicon_" + iconID.substr(0, 1) + "_" + iconID.substr(1, 4) + "_" + iconID.substr(5, 2) + "_" + iconID.substr(7, 1) + ".dxt";
			}

			items[slot]->icon->SetTex(szIconFN);
			items[slot]->tbl = tbl;
		}

		items[slot]->nItemID = drop.nItemID;

		float per = (float)((float)drop.sPercent / (float)100) > 100 ? 100 : (float)((float)drop.sPercent / (float)100);
		progress_g[slot]->SetCurValue((int)ceil(per), 0.5F, 40.0F);
		txt_g[slot]->SetString(string_format(xorstr("%.2lf%%"), per));

		slot++;
	}
}

__IconItemSkill* CDropList::GetItemStruct(CN3UIImage* img, std::string fileSz, __TABLE_ITEM_BASIC* tbl, uint32 realID)
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
	//newItemBasic->customStrings = itemOrg->customStrings;

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