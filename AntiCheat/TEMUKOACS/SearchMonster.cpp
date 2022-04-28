#include "SearchMonster.h"
#include <cctype>

CSearchMonster::CSearchMonster()
{
	needToReset = true;
	page = 0;
	btn_close = NULL;
	int i = 0;

	auto mobs = Engine->tblMgr->GetMobTable();
	map<uint32, TABLE_MOB>::iterator it;
	for (it = mobs->begin(); it != mobs->end(); it++)
	{
		TABLE_MOB mob = it->second;
		monsters.push_back(MONSTER_INFO(mob.ID, mob.strName));
	}
}

CN3UIString* CSearchMonster::GetItemText(CN3UIButton* item)
{
	return (CN3UIString*)item->GetChildByID("txt_mob");
}

CSearchMonster::~CSearchMonster()
{
	btn_close = NULL;
}

void CSearchMonster::ListOrder()
{
	if (needToReset) {
		if(monsters.size() > 5)
			list_scroll->SetRange(0, monsters.size() - 5);
		else
			list_scroll->SetRange(0, 1);
		needToReset = false;
	}

	int itemIndex = 0;
	int range = monsters.size() - 1;
	for (int i = page; i < page + 5; i++)
	{
		if (i > range) {
			items[itemIndex]->SetVisible(false);
			itemIndex++;
			continue;
		}
		else {
			items[itemIndex]->SetVisible(true);
			GetItemText(items[itemIndex])->SetString(monsters[i].strName);
			itemIndex++;
		}
	}
}

void CSearchMonster::Tick()
{
	POINT currentCursorPos = Engine->m_UiMgr->localInput->MouseGetPos();

	if (Engine->m_UiMgr->m_FocusedUI != this) {
		Engine->disableCameraZoom = false;
		return;
	}

	if (IsIn(currentCursorPos.x, currentCursorPos.y))
	{
		Engine->disableCameraZoom = true;

		int delta = Engine->m_UiMgr->localInput->GetMouseWheelDelta();

		int tmp = page;

		if (delta > 0) // wheel to up
		{
			if (list_scroll->GetCurrentPos() != 0) {
				list_scroll->SetCurrentPos(list_scroll->GetCurrentPos() - 1);
				page--;
				ListOrder();
			}
		}
		else if (delta < 0) // whell to down
		{
			if (list_scroll->GetCurrentPos() != list_scroll->GetMaxPos()) {
				list_scroll->SetCurrentPos(list_scroll->GetCurrentPos() + 1);
				page++;
				ListOrder();
			}
		}

		if (tmp < page)
		{
			for (int i = 0; i < 5; i++)
			{
				if (items[i]->GetState() == UI_STATE_BUTTON_DOWN)
				{
					items[i]->SetState(UI_STATE_BUTTON_NORMAL);
					if (i != 0)
					{
						items[i-1]->SetState(UI_STATE_BUTTON_DOWN);
					}
					break;
				}
			}
		}
		else if (tmp > page) {
			for (int i = 0; i < 5; i++)
			{
				if (items[i]->GetState() == UI_STATE_BUTTON_DOWN)
				{
					items[i]->SetState(UI_STATE_BUTTON_NORMAL);
					if (i != 4)
					{
						items[i + 1]->SetState(UI_STATE_BUTTON_DOWN);
					}
					break;
				}
			}
		}
	} else Engine->disableCameraZoom = false;

	Engine->m_UiMgr->localInput->SetMouseWheelDelta(0);
}

bool CSearchMonster::Load(HANDLE hFile)
{
	if (CN3UIBase::Load(hFile) == false) return false;

	std::string find = xorstr("btn_close");
	btn_close = (CN3UIButton*)GetChildByID(find);
	find = xorstr("edit_search");
	edit_search = (CN3UIEdit*)GetChildByID(find);
	find = xorstr("btn_search");
	btn_search = (CN3UIButton*)GetChildByID(find);
	find = xorstr("btn_drops");
	btn_drops = (CN3UIButton*)GetChildByID(find);

	for (UIListReverseItor itor = m_Children.rbegin(); m_Children.rend() != itor; ++itor)
	{
		CN3UIScrollBar* pChild = (CN3UIScrollBar*)(*itor);
		if (pChild->GetMaxPos() == 10) {
			list_scroll = pChild;
			break;
		}
	}

	if (list_scroll != NULL)
		list_scroll->SetRange(0, 10);

	find = xorstr("txt_monster_name");
	txt_monster_name = (CN3UIString*)GetChildByID(find);
	find = xorstr("txt_level");
	txt_level = (CN3UIString*)GetChildByID(find);
	find = xorstr("txt_hp");
	txt_hp = (CN3UIString*)GetChildByID(find);
	find = xorstr("txt_ap");
	txt_ap = (CN3UIString*)GetChildByID(find);
	find = xorstr("txt_ac");
	txt_ac = (CN3UIString*)GetChildByID(find);

	for (int i = 0; i < 5; i++)
	{
		find = string_format(xorstr("item_%d"), i+1);
		items[i] = (CN3UIButton*)GetChildByID(find);
	}

	ListOrder();

	SetPos(Engine->m_UiMgr->GetScreenCenter(this).x, Engine->m_UiMgr->GetScreenCenter(this).y);

	return true;
}

void CSearchMonster::Update(Packet pkt)
{
	uint16 level, damage, defense;
	uint32 maxhp;
	pkt >> level >> maxhp >> damage >> defense;

	txt_level->SetString(string_format(xorstr("Level: %d"), level));
	txt_hp->SetString(string_format(xorstr("Health: %s"), Engine->StringHelper->NumberFormat(maxhp).c_str()));
	txt_ap->SetString(string_format(xorstr("Attack: %s"), Engine->StringHelper->NumberFormat(damage).c_str()));
	txt_ac->SetString(string_format(xorstr("Defense: %s"), Engine->StringHelper->NumberFormat(defense).c_str()));
}

bool findStringIC(const std::string& strHaystack, const std::string& strNeedle)
{
	auto it = std::search(
		strHaystack.begin(), strHaystack.end(),
		strNeedle.begin(), strNeedle.end(),
		[](char ch1, char ch2) { return std::toupper(ch1) == std::toupper(ch2); }
	);
	return (it != strHaystack.end());
}

bool CSearchMonster::ReceiveMessage(CN3UIBase* pSender, uint32_t dwMsg)
{
	if (dwMsg == UIMSG_BUTTON_CLICK)
	{
		if (pSender == btn_close)
			Close();
		else if (pSender == btn_search)
		{
			std::string query = edit_search->GetString();
			monsters.clear();
			auto mobs = Engine->tblMgr->GetMobTable();
			map<uint32, TABLE_MOB>::iterator it;
			for (it = mobs->begin(); it != mobs->end(); it++)
			{
				TABLE_MOB mob = it->second;
				if(findStringIC(mob.strName, query))
					monsters.push_back(MONSTER_INFO(mob.ID, mob.strName));
			}
			needToReset = true;
			ListOrder();
		}
		else if (pSender == btn_drops)
		{
			for (int i = 0; i < 5; i++)
			{
				if (monsters.size() == 0 
					|| i + page > monsters.size() - 1)
					break;

				if (items[i]->GetState() == UI_STATE_BUTTON_DOWN) {
					Packet result(WIZ_HOOK_GUARD, uint8(WIZ_HOOK_DROP_REQUEST));
					result << uint8(3) << uint16(monsters[i + page].sID);
					Engine->Send(&result);
				}
			}
		}
		else {
			for (int i = 0; i < 5; i++)
			{
				if (monsters.size() == 0 || i + page > monsters.size() - 1)
					break;

				if (pSender != items[i] && pSender != btn_drops && pSender != btn_search && pSender != btn_close)
					items[i]->SetState(UI_STATE_BUTTON_NORMAL);
				else if (pSender == items[i]) {
					Packet result(WIZ_HOOK_GUARD, uint8(WIZ_HOOK_MONSTER_INFO));
					result << uint32(monsters[i + page].sID);
					txt_monster_name->SetString(monsters[i + page].strName);
					Engine->Send(&result);
				}
			}
		}
	}
	else if (dwMsg == UIMSG_SCROLLBAR_POS)
	{
		page = list_scroll->GetCurrentPos();
		ListOrder();
	}

	return true;
}

uint32_t CSearchMonster::MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld)
{
	uint32_t dwRet = UI_MOUSEPROC_NONE;
	dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);
	return dwRet;
}

bool CSearchMonster::OnKeyPress(int iKey)
{
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

void CSearchMonster::Open()
{
	SetVisible(true);
}

void CSearchMonster::Close()
{
	SetVisible(false);
}