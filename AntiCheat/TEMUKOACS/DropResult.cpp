#include "DropResult.h"
#include "HDRReader.h"

CDropResult::CDropResult()
{
	txt_title = NULL;
	btn_close = NULL;
	for (int i = 0; i < 5; i++) {
		dropitems[i].icon = NULL;
		dropitems[i].nItemID = 0;
		dropitems[i].tbl = nullptr;
	}
}

CDropResult::~CDropResult()
{

}

bool CDropResult::Load(HANDLE hFile)
{
	if (CN3UIBase::Load(hFile) == false) return false;

	string find = xorstr("txt_title");
	txt_title = (CN3UIString*)GetChildByID(find);
	find = xorstr("btn_close");
	btn_close = (CN3UIButton*)GetChildByID(find);

	for (int i = 1; i <= 5; i++)
	{
		find = string_format(xorstr("itemicon%d"), i);
		dropitems[i - 1].icon = (CN3UIImage*)GetChildByID(find);
	}

	SetMoveRect(txt_title->GetRegion());
	SetPos(Engine->m_UiMgr->GetScreenCenter(this).x, Engine->m_UiMgr->GetScreenCenter(this).y / 2 / 2);

	return true;
}

bool CDropResult::ReceiveMessage(CN3UIBase* pSender, uint32_t dwMsg)
{
	if (dwMsg == UIMSG_BUTTON_CLICK)
	{
		if (pSender == btn_close)
			Close();
	}

	return true;
}

uint32_t CDropResult::MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld)
{
	uint32_t dwRet = UI_MOUSEPROC_NONE;
	if (Engine->m_UiMgr->m_FocusedUI == this && Engine->m_UiMgr->m_pUITooltipDlg != NULL)
	{
		bool showTooltip = false;

		for(int i = 0; i < 5; i++)
		{
			if (dropitems[i].icon != NULL) {
				if (dropitems[i].icon->IsIn(ptCur.x, ptCur.y) && dropitems[i].tbl != nullptr && dropitems[i].nItemID != 0)
				{
					Engine->m_UiMgr->m_pUITooltipDlg->DisplayTooltipsEnable(ptCur.x, ptCur.y, GetItemStruct(dropitems[i].icon, dropitems[i].icon->FileName(), dropitems[i].tbl, dropitems[i].nItemID), this, false, true, true);
					showTooltip = true;
					break;
				}
			}
		}

		Engine->m_UiMgr->m_pUITooltipDlg->SetVisible(showTooltip);
	}
	dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);
	return dwRet;
}

bool CDropResult::OnKeyPress(int iKey)
{
	if (!IsVisible())
		return CN3UIBase::OnKeyPress(iKey);

	switch (iKey)
	{
	case DIK_ESCAPE:
		Close();
		return true;
	}

	return CN3UIBase::OnKeyPress(iKey);
}

void CDropResult::Open()
{
	SetVisible(true);
}

void CDropResult::Close()
{
	SetVisible(false);
}

void CDropResult::SetTitle(std::string text)
{
	txt_title->SetString(text);
}

void CDropResult::AddItem(uint32 itemID)
{
	TABLE_ITEM_BASIC* tbl = Engine->tblMgr->getItemData(itemID);
	if (tbl == nullptr)
		return;

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

	for (int i = 0; i < 5; i++)
	{
		if (dropitems[i].nItemID == 0)
		{
			dropitems[i].nItemID = itemID;
			dropitems[i].tbl = tbl;
			dropitems[i].icon->SetTex(szIconFN);
			dropitems[i].icon->FileNameSet(szIconFN);
			return;
		}
	}

	for (int i = 4; i >= 1; i--)
	{
		dropitems[i].nItemID = dropitems[i - 1].nItemID;
		dropitems[i].icon->SetTex(dropitems[i - 1].icon->FileName());
		dropitems[i].icon->FileNameSet(dropitems[i - 1].icon->FileName());
		dropitems[i].tbl = dropitems[i - 1].tbl;
	}

	dropitems[0].nItemID = itemID;
	dropitems[0].tbl = tbl;
	dropitems[0].icon->SetTex(szIconFN);
	dropitems[0].icon->FileNameSet(szIconFN);
}

__IconItemSkill* CDropResult::GetItemStruct(CN3UIImage* img, std::string fileSz, __TABLE_ITEM_BASIC* tbl, uint32 realID)
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