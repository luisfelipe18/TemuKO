#include "CRRewards.h"
#include "HDRReader.h"

CCollectionRaceRewards::CCollectionRaceRewards()
{
	for (int i = 0; i < 4; i++)
	{
		m_items[i].itemID = 0;
		m_items[i].tbl = nullptr;
	}
}

CCollectionRaceRewards::~CCollectionRaceRewards()
{

}

bool CCollectionRaceRewards::Load(HANDLE hFile)
{
	if (CN3UIBase::Load(hFile) == false) return false;

	std::string find = xorstr("txt_header");
	txt_header = (CN3UIString*)GetChildByID(find);
	find = xorstr("btn_close");
	btn_close = (CN3UIButton*)GetChildByID(find);
	// Slots
	for (int i = 1; i <= 2; i++)
	{
		find = string_format(xorstr("slot_i%d"), i);
		slot_i[i-1] = (CN3UIArea*)GetChildByID(find);
	}
	for (int i = 1; i <= 4; i++)
	{
		find = string_format(xorstr("slot_r%d"), i);
		slot_r[i - 1] = (CN3UIArea*)GetChildByID(find);
	}
	// Names
	for (int i = 1; i <= 2; i++)
	{
		find = string_format(xorstr("txt_name_i%d"), i);
		txt_name_i[i - 1] = (CN3UIString*)GetChildByID(find);
	}
	for (int i = 1; i <= 4; i++)
	{
		find = string_format(xorstr("txt_name_r%d"), i);
		txt_name_r[i - 1] = (CN3UIString*)GetChildByID(find);
	}
	// Counts
	for (int i = 1; i <= 2; i++)
	{
		find = string_format(xorstr("txt_count_i%d"), i);
		txt_count_i[i - 1] = (CN3UIString*)GetChildByID(find);
	}
	for (int i = 1; i <= 4; i++)
	{
		find = string_format(xorstr("txt_count_r%d"), i);
		txt_count_r[i - 1] = (CN3UIString*)GetChildByID(find);
	}
	// Icons
	for (int i = 0; i < 2; i++)
	{
		m_items[i].icon = new CN3UIIcon();
		m_items[i].icon->Init(this);
		m_items[i].icon->SetUIType(UI_TYPE_ICON);
		m_items[i].icon->SetStyle(UISTYLE_ICON_ITEM | UISTYLE_ICON_CERTIFICATION_NEED);
		m_items[i].icon->SetUVRect(0, 0, (float)45.0f / (float)64.0f, (float)45.0f / (float)64.0f);
		m_items[i].icon->SetRegion(slot_i[i]->GetRegion());
		m_items[i].icon->SetTex("UI\\itemicon_noimage.dxt");

	}
	for (int i = 0; i < 4; i++)
	{
		m_items[i+2].icon = new CN3UIIcon();
		m_items[i + 2].icon->Init(this);
		m_items[i + 2].icon->SetUIType(UI_TYPE_ICON);
		m_items[i + 2].icon->SetStyle(UISTYLE_ICON_ITEM | UISTYLE_ICON_CERTIFICATION_NEED);
		m_items[i + 2].icon->SetUVRect(0, 0, (float)45.0f / (float)64.0f, (float)45.0f / (float)64.0f);
		m_items[i + 2].icon->SetRegion(slot_r[i]->GetRegion());
		m_items[i + 2].icon->SetTex("UI\\itemicon_noimage.dxt");
	}

	SetMoveRect(txt_header->GetRegion());

	SetPos(Engine->m_UiMgr->GetScreenCenter(this).x, Engine->m_UiMgr->GetScreenCenter(this).y);
	return true;
}

bool CCollectionRaceRewards::ReceiveMessage(CN3UIBase* pSender, uint32_t dwMsg)
{
	if (dwMsg == UIMSG_BUTTON_CLICK)
	{
		if (pSender == btn_close)
			Close();
	}

	return true;
}

uint32_t CCollectionRaceRewards::MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld)
{
	uint32_t dwRet = UI_MOUSEPROC_NONE;
	if (Engine->m_UiMgr->m_FocusedUI == this)
	{
		bool showTooltip = false;

		for (auto it : m_items)
		{
			if (it.icon->IsIn(ptCur.x, ptCur.y) && it.tbl != nullptr)
			{
				Engine->m_UiMgr->m_pUITooltipDlg->DisplayTooltipsEnable(ptCur.x, ptCur.y, GetItemStruct(it.icon, it.icon->FileName(), it.tbl, it.itemID), this, true, true, true);
				showTooltip = true;
				break;
			}
		}

		Engine->m_UiMgr->m_pUITooltipDlg->SetVisible(showTooltip);
	}
	dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);
	return dwRet;
}

bool CCollectionRaceRewards::OnKeyPress(int iKey)
{
	if (Engine->m_UiMgr->uiSupport == NULL)
		return CN3UIBase::OnKeyPress(iKey);

	if (!IsVisible())
		return CN3UIBase::OnKeyPress(iKey);

	switch (iKey)
	{
	case DIK_ESCAPE:
		Close();
		return true;
		break;
	}

	return CN3UIBase::OnKeyPress(iKey);
}

void CCollectionRaceRewards::Open()
{
	SetVisible(true);
}

void CCollectionRaceRewards::Close()
{
	SetVisible(false);
}

void CCollectionRaceRewards::LoadItems(uint32 items[2], uint32 resources[4], uint32 counts[6])
{
	for (int i = 0; i < 2; i++)
	{
		m_items[i].itemID = items[i];

		TABLE_ITEM_BASIC* tbl = Engine->tblMgr->getItemData(items[i]);
		if (tbl != nullptr)
		{
			m_items[i].tbl = tbl;

			txt_name_i[i]->SetString(tbl->strName);
			txt_count_i[i]->SetString(Engine->StringHelper->NumberFormat(counts[i], ','));

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

			m_items[i].icon->SetTex(szIconFN);
		}
		else {
			m_items[i].itemID = 0;
			m_items[i].tbl = nullptr;
			txt_name_i[i]->SetString("None");
			txt_count_i[i]->SetString("0");
			m_items[i].icon->SetTex("UI\\itemicon_noimage.dxt");
		}
	}

	for (int i = 0; i < 4; i++)
	{
		m_items[i+2].itemID = resources[i];

		TABLE_ITEM_BASIC* tbl = Engine->tblMgr->getItemData(resources[i]);
		if (tbl != nullptr)
		{
			m_items[i+2].tbl = tbl;
			
			txt_name_r[i]->SetString(tbl->strName);
			txt_count_r[i]->SetString(Engine->StringHelper->NumberFormat(counts[i+2], ','));

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

			m_items[i + 2].icon->SetTex(szIconFN);
		}
		else {
			m_items[i+2].itemID = 0;
			m_items[i+2].tbl = nullptr;
			txt_name_r[i]->SetString("None");
			txt_count_r[i]->SetString("0");
			m_items[i + 2].icon->SetTex("UI\\itemicon_noimage.dxt");
		}
	}
}

__IconItemSkill* CCollectionRaceRewards::GetItemStruct(CN3UIImage* img, std::string fileSz, __TABLE_ITEM_BASIC* tbl, uint32 realID)
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