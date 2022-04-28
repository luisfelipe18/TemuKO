#include "DropListGroup.h"
#include "HDRReader.h"

CDropListGroup::CDropListGroup()
{
	btn_close = NULL;
	txt_droprate = NULL;
	progress_per = NULL;
}

bool CDropListGroup::Load(HANDLE hFile)
{
	if (CN3UIBase::Load(hFile) == false) return false;

	std::string find = xorstr("btn_close");
	btn_close = (CN3UIButton*)GetChildByID(find);

	float fUVAspect = (float)45.0f / (float)64.0f;

	for (int i = 1; i <= 6; i++)
	{
		find = string_format(xorstr("group_%d"), i);
		CN3UIBase* item_group = (CN3UIBase*)GetChildByID(find);
		for (int j = 1; j <= 5; j++)
		{
			find = string_format(xorstr("slot_%d"), j);

			CN3UIArea* area = (CN3UIArea*)item_group->GetChildByID(find);

			CN3UIIcon* item = new CN3UIIcon;
			item->Init(this);
			item->SetUVRect(0, 0, fUVAspect, fUVAspect);
			item->SetUIType(UI_TYPE_ICON);
			item->SetStyle(UISTYLE_ICON_ITEM | UISTYLE_ICON_CERTIFICATION_NEED);
			item->SetRegion(area->GetRegion());

			ItemInfo* tmp = new ItemInfo();
			tmp->icon = item;
			tmp->nItemID = 0;
			tmp->tbl = nullptr;
			items.push_back(tmp);
		}
	}

	SetPos(Engine->m_UiMgr->GetScreenCenter(this).x, Engine->m_UiMgr->GetScreenCenter(this).y);

	return true;
}

bool CDropListGroup::ReceiveMessage(CN3UIBase* pSender, uint32_t dwMsg)
{
	if (dwMsg == UIMSG_BUTTON_CLICK)
	{
		if (pSender == btn_close)
		{
			Close();
		}
	}
	return true;
}

bool CDropListGroup::OnKeyPress(int iKey)
{
	if (!IsVisible() || Engine->m_UiMgr->m_FocusedUI != this)
		return CN3UIBase::OnKeyPress(iKey);

	switch (iKey)
	{
	case DIK_ESCAPE:
		ReceiveMessage(btn_close, UIMSG_BUTTON_CLICK);
		return true;
	}

	return CN3UIBase::OnKeyPress(iKey);
}

uint32_t CDropListGroup::MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld)
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

	dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);
	return dwRet;
}

void CDropListGroup::Close()
{
	if (!Engine->m_UiMgr->uiDropList->IsVisible())
		Engine->m_UiMgr->uiDropList->SetVisible(true);

	SetVisible(false);
}

void CDropListGroup::Open()
{
	if (Engine->m_UiMgr->uiDropList->IsVisible())
		Engine->m_UiMgr->uiDropList->SetVisible(false);

	SetVisible(true);
	Engine->m_UiMgr->SetFocusedUI(this);
}

uint32 getItemOriginIDExG(uint32 sID)
{
	return std::atoi((std::to_string(sID).substr(0, 6) + xorstr("000")).c_str());
}

void CDropListGroup::Update(vector<uint32> drops)
{
	int i = 0;
	for (auto it : items)
	{
		it->nItemID = drops[i];
		TABLE_ITEM_BASIC* tbl = Engine->tblMgr->getItemData(getItemOriginIDExG(drops[i]));
		if (tbl == nullptr)
		{
			it->icon->SetTex(xorstr("UI\\itemicon_noimage.dxt"));
			it->tbl = nullptr;
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
			
				it->icon->SetTex(szIconFN);
			
			
			it->tbl = tbl;
		}

		i++;
	}
}

__IconItemSkill* CDropListGroup::GetItemStruct(CN3UIImage* img, std::string fileSz, __TABLE_ITEM_BASIC* tbl, uint32 realID)
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

	if (newItemExt == nullptr || newItemExt == NULL)
	{
		string ix = to_string(realID);
		uint8 itemGrade;
		itemGrade = std::atoi(ix.substr(ix.length() - 1, 1).c_str());

		newItemExt = new __TABLE_ITEM_EXT;
		newItemExt->extensionID = itemGrade;
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