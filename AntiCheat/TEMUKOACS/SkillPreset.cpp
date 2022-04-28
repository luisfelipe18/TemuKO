#include "SkillPreset.h"

CSkillPreset::CSkillPreset()
{
	posSet = false;
	selectedIndex = 0;
	points = 0;
	for (int i = 0; i < 4; i++) {
		point[i] = 0;
		ceilPoint[i] = 0;
	}
	for (int i = 0; i < 3; i++)
		skillText[i] = "";

	points = (Engine->m_PlayerBase->GetLevel() - 9) * 2;

	Engine->m_PlayerBase->UpdateFromMemory();
}

CSkillPreset::~CSkillPreset()
{

}

bool CSkillPreset::Load(HANDLE hFile)
{
	if (CN3UIBase::Load(hFile) == false) return false;

	Engine->m_PlayerBase->UpdateFromMemory();

	std::string find = xorstr("btn_close");
	btn_close = (CN3UIButton*)GetChildByID(find);

	CN3UIBase* base_slot = NULL;
	find = xorstr("base_slot");
	base_slot = (CN3UIBase*)GetChildByID(find);

	for (int i = 1; i <= 4; i++)
	{
		find = string_format(xorstr("btn_save%d"), i);
		btn_save[i - 1] = (CN3UIButton*)base_slot->GetChildByID(find);
		find = string_format(xorstr("txt_info%d"), i);
		txt_info[i - 1] = (CN3UIString*)base_slot->GetChildByID(find);
	}

	find = xorstr("txt_header");
	txt_header = (CN3UIString*)base_slot->GetChildByID(find);
	find = xorstr("btn_apply");
	btn_apply = (CN3UIButton*)base_slot->GetChildByID(find);
	find = xorstr("btn_edit");
	btn_edit = (CN3UIButton*)base_slot->GetChildByID(find);

	find = xorstr("base_stat");
	base_stat = (CN3UIBase*)GetChildByID(find);
	find = xorstr("txt_title");
	stat_txt_title = (CN3UIString*)base_stat->GetChildByID(find);
	find = xorstr("btn_save");
	stat_btn_save = (CN3UIButton*)base_stat->GetChildByID(find);
	find = xorstr("btn_cancel");
	stat_btn_cancel = (CN3UIButton*)base_stat->GetChildByID(find);

	for (int i = 0; i < 2; i++)
	{
		find = string_format(xorstr("btn_str_%d"), i);
		btn_str[i] = (CN3UIButton*)base_stat->GetChildByID(find);
		find = string_format(xorstr("btn_hp_%d"), i);
		btn_hp[i] = (CN3UIButton*)base_stat->GetChildByID(find);
		find = string_format(xorstr("btn_dex_%d"), i);
		btn_dex[i] = (CN3UIButton*)base_stat->GetChildByID(find);
		find = string_format(xorstr("btn_mp_%d"), i);
		btn_mp[i] = (CN3UIButton*)base_stat->GetChildByID(find);
		find = string_format(xorstr("btn_int_%d"), i);
		btn_int[i] = (CN3UIButton*)base_stat->GetChildByID(find);
	}

	for (int i = 0; i < 2; i++)
		btn_int[i]->SetState(UI_STATE_BUTTON_DISABLE);

	for (int i = 1; i <= 4; i++)
	{
		find = string_format(xorstr("txt_stat%d"), i);
		txt_stat[i - 1] = (CN3UIString*)base_stat->GetChildByID(find);
		find = string_format(xorstr("txt_point%d"), i);
		txt_point[i - 1] = (CN3UIString*)base_stat->GetChildByID(find);
	}
	find = xorstr("txt_stat5");
	CN3UIString* tmp = (CN3UIString*)base_stat->GetChildByID(find);
	tmp->SetString("");
	find = xorstr("txt_point5");
	tmp = (CN3UIString*)base_stat->GetChildByID(find);
	tmp->SetString("");

	find = xorstr("txt_points");
	txt_points = (CN3UIString*)base_stat->GetChildByID(find);

	base_stat->SetVisible(false);

	txt_header->SetString(xorstr("Save Skill Preset"));

	if (Engine->m_PlayerBase->isWarrior())
	{
		skillText[0] = xorstr("Attack");
		skillText[1] = xorstr("Defence");
		skillText[2] = xorstr("Passion");
		ceilPoint[0] = 80;
		ceilPoint[1] = 80;
		ceilPoint[2] = 80;
	} 
	else if (Engine->m_PlayerBase->isRogue())
	{
		skillText[0] = xorstr("Archery");
		skillText[1] = xorstr("Assassin");
		skillText[2] = xorstr("Explore");
		ceilPoint[0] = 80;
		ceilPoint[1] = 80;
		ceilPoint[2] = 80;
	}
	else if (Engine->m_PlayerBase->isMage())
	{
		skillText[0] = xorstr("Fire");
		skillText[1] = xorstr("Ice");
		skillText[2] = xorstr("Lightning");
		ceilPoint[0] = 80;
		ceilPoint[1] = 80;
		ceilPoint[2] = 80;
	}
	else if (Engine->m_PlayerBase->isPriest())
	{
		skillText[0] = xorstr("Heal");
		skillText[1] = xorstr("Aura");
		skillText[2] = xorstr("Spirit");
		ceilPoint[0] = 80;
		ceilPoint[1] = 80;
		ceilPoint[2] = 80;
	}
	else if (Engine->m_PlayerBase->isPortuKurian())
	{
		skillText[0] = xorstr("Attack");
		skillText[1] = xorstr("Defence");
		skillText[2] = xorstr("Devil");
		ceilPoint[0] = 80;
		ceilPoint[1] = 80;
		ceilPoint[2] = 80;
	} 
	if (Engine->m_PlayerBase->isMastered()) {
		ceilPoint[3] = 20;
		txt_stat[3]->SetString("Master");
	}
	else {
		ceilPoint[3] = 0;
		txt_stat[3]->SetString("-");
	}

	points = (Engine->m_PlayerBase->GetLevel() - 9) * 2;

	for (int i = 0; i < 3; i++)
		txt_stat[i]->SetString(skillText[i]);

	SetPos(Engine->m_UiMgr->GetScreenCenter(this).x, Engine->m_UiMgr->GetScreenCenter(this).y);

	return true;
}

void CSkillPreset::TickPoints()
{
	for (int i = 0; i < 4; i++) {
		if (txt_point[i] == nullptr)
			return;
		txt_point[i]->SetString(string_format(xorstr("%d"), point[i]));
	}
	if (txt_points == nullptr)
		return;
	txt_points->SetString(string_format(xorstr("%d"), points));
}

void CSkillPreset::SetPoints()
{
	if (Engine->m_PlayerBase->isWarrior())
	{
		skillText[0] = xorstr("Attack");
		skillText[1] = xorstr("Defence");
		skillText[2] = xorstr("Passion");
		ceilPoint[0] = 80;
		ceilPoint[1] = 80;
		ceilPoint[2] = 80;
	}
	else if (Engine->m_PlayerBase->isRogue())
	{
		skillText[0] = xorstr("Archery");
		skillText[1] = xorstr("Assassin");
		skillText[2] = xorstr("Explore");
		ceilPoint[0] = 80;
		ceilPoint[1] = 80;
		ceilPoint[2] = 80;
	}
	else if (Engine->m_PlayerBase->isMage())
	{
		skillText[0] = xorstr("Fire");
		skillText[1] = xorstr("Ice");
		skillText[2] = xorstr("Lightning");
		ceilPoint[0] = 80;
		ceilPoint[1] = 80;
		ceilPoint[2] = 80;
	}
	else if (Engine->m_PlayerBase->isPriest())
	{
		skillText[0] = xorstr("Heal");
		skillText[1] = xorstr("Aura");
		skillText[2] = xorstr("Spirit");
		ceilPoint[0] = 80;
		ceilPoint[1] = 80;
		ceilPoint[2] = 80;
	}
	else if (Engine->m_PlayerBase->isPortuKurian())
	{
		skillText[0] = xorstr("Attack");
		skillText[1] = xorstr("Defence");
		skillText[2] = xorstr("Devil");
		ceilPoint[0] = 80;
		ceilPoint[1] = 80;
		ceilPoint[2] = 80;
	}
	if (Engine->m_PlayerBase->isMastered()) {
		ceilPoint[3] = 20;
		txt_stat[3]->SetString("Master");
	}
	else {
		ceilPoint[3] = 0;
		txt_stat[3]->SetString("-");
	}

	points = (Engine->m_PlayerBase->GetLevel() - 9) * 2;

	for (int i = 0; i < 3; i++) {
		if (txt_stat[i] == nullptr)
			return;
		txt_stat[i]->SetString(skillText[i]);
	}
	
	for (int i = 0; i < 4; i++) {
		if (txt_point[i] == nullptr)
			return;
		txt_point[i]->SetString(string_format(xorstr("%d"), point[i]));
	}
	if (txt_points == nullptr)
		return;
	txt_points->SetString(string_format(xorstr("%d"), points));
}

bool CSkillPreset::ReceiveMessage(CN3UIBase* pSender, uint32_t dwMsg)
{
	if (dwMsg == UIMSG_BUTTON_CLICK)
	{
		if (btn_close == nullptr)
			return true;

		if (pSender == btn_close)
			Close();

		for (int i = 0; i < 4; i++)
		{
			if (btn_save[i] == nullptr)
				return true;

			if (pSender == btn_save[i])
			{
				UncheckAll();
				btn_save[i]->SetState(UI_STATE_BUTTON_DOWN);
				selectedIndex = i;
			}
		}

		for (int i = 0; i < 2; i++)
		{
			if (btn_str[i] == nullptr 
				|| btn_hp[i] == nullptr 
				|| btn_dex[i] == nullptr 
				|| btn_mp[i] == nullptr 
				|| btn_int[i] == nullptr)
				return true;

			if (pSender == btn_str[i])
			{
				if ((points <= 0 && i == 1) 
					|| (points >= 302 && i == 0))
					return true;

				if (i == 0) {
					if (point[0] > 0) {
						point[0]--;
						points++;
					}
				}
				else {
					if (point[0] < ceilPoint[0]) {
						point[0]++;
						points--;
					}
				}
			}
			if (pSender == btn_hp[i])
			{
				if ((points <= 0 && i == 1) 
					|| (points >= 302 && i == 0))
					return true;

				if (i == 0) {
					if (point[1] > 0) {
						point[1]--;
						points++;
					}
				}
				else {
					if (point[1] < ceilPoint[1]) {
						point[1]++;
						points--;
					}
				}
			}
			if (pSender == btn_dex[i])
			{
				if ((points <= 0 && i == 1) 
					|| (points >= 302 && i == 0))
					return true;

				if (i == 0) {
					if (point[2] > 0) {
						point[2]--;
						points++;
					}
				}
				else {
					if (point[2] < ceilPoint[2]) {
						point[2]++;
						points--;
					}
				}
			}
			if (pSender == btn_mp[i])
			{
				if ((points <= 0 && i == 1) 
					|| (points >= 302 && i == 0))
					return true;

				if (i == 0) {
					if (point[3] > 0) {
						point[3]--;
						points++;
					}
				}
				else {
					if (point[3] < ceilPoint[3]) {
						point[3]++;
						points--;
					}
				}
			}
			if (pSender == btn_int[i])
			{
				if ((points <= 0 && i == 1) 
					|| (points >= 302 && i == 0))
					return true;

				if (i == 0) {
					if (point[4] > 0) {
						point[4]--;
						points++;
					}
				}
				else {
					if (point[4] < ceilPoint[4]) {
						point[4]++;
						points--;
					}
				}
			}
		}

		if (btn_edit != nullptr && pSender == btn_edit)
			OpenStatSettings(GetCheckedIndex());

		if (stat_btn_save != nullptr && pSender == stat_btn_save)
		{
			string path = Engine->m_BasePath + xorstr("preset.ini");
			if (Engine->m_PlayerBase->m_strCharacterName.empty() || Engine->m_PlayerBase->m_strCharacterName == "")
				return true;

			string group = string_format(xorstr("%s"), Engine->m_PlayerBase->m_strCharacterName.c_str());
			string key = "";
			for (int i = 0; i < 4; i++)
			{
				if (!Engine->m_PlayerBase->isMastered() && i == 3)
					break;
				key = string_format(xorstr("skillslot%d_%d"), GetCheckedIndex(), i);
				WritePrivateProfileStringA(group.c_str(), key.c_str(), to_string(point[i]).c_str(), path.c_str());
			}

			for (auto itr : skillText)
				if (txt_info == nullptr)
					return true;

			if(Engine->m_PlayerBase->isMastered())
				txt_info[GetCheckedIndex()]->SetString(string_format(xorstr("%s:%d %s:%d %s:%d Master:%d"), skillText[0].c_str(), point[0], skillText[1].c_str(), point[1], skillText[2].c_str(), point[2], point[3]));
			else
				txt_info[GetCheckedIndex()]->SetString(string_format(xorstr("%s:%d %s:%d %s:%d"), skillText[0].c_str(), point[0], skillText[1].c_str(), point[1], skillText[2].c_str(), point[2]));

			if (base_stat == nullptr)
				return true;

			base_stat->SetVisible(false);
		}

		if (stat_btn_cancel != nullptr && pSender == stat_btn_cancel)
			base_stat->SetVisible(false);

		if (btn_apply != nullptr && pSender == btn_apply)
		{
			GetSavedData(GetCheckedIndex());

			Packet result(WIZ_HOOK_GUARD, uint8(WIZ_HOOK_RESET));
			result << uint8(1);

			for (int i = 0; i < 3; i++)
				result << point[i];

			if (Engine->m_PlayerBase->isMastered())
				result << point[3];
			else
				result << uint8(0);

			Engine->Send(&result);
		}
	}

	TickPoints();

	return true;
}

void CSkillPreset::GetSavedData(uint8 id)
{
	Engine->m_PlayerBase->UpdateFromMemory();

	SetPoints();

	if (Engine->m_PlayerBase->m_strCharacterName.empty() || Engine->m_PlayerBase->m_strCharacterName == "")
		return;
	
	string path = Engine->m_BasePath + xorstr("preset.ini");
	string group = string_format(xorstr("%s"), Engine->m_PlayerBase->m_strCharacterName.c_str());
	string key = "";

	uint16 totalMinus = 0;
	for (int i = 0; i < 4; i++)
	{
		if (!Engine->m_PlayerBase->isMastered() && i == 3)
			break;
		key = string_format(xorstr("skillslot%d_%d"), id, i);
		point[i] = GetPrivateProfileIntA(group.c_str(), key.c_str(), 0, path.c_str());
		if (point[i] > ceilPoint[i]) point[i] = 0;
		totalMinus += point[i];
	}

	if (totalMinus > points)
	{
		SetPoints();
		return;
	}

	points -= totalMinus;

	TickPoints();
}

void CSkillPreset::LoadSavedData()
{
	points = (Engine->m_PlayerBase->GetLevel() - 9) * 2;

	if (Engine->m_PlayerBase->m_strCharacterName.empty() || Engine->m_PlayerBase->m_strCharacterName == "")
		return;

	string path = Engine->m_BasePath + xorstr("preset.ini");
	string group = string_format(xorstr("%s"), Engine->m_PlayerBase->m_strCharacterName.c_str());
	string key = "";

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (!Engine->m_PlayerBase->isMastered() && i == 3)
			{
				point[j] = 0;
				break;
			}
			key = string_format(xorstr("skillslot%d_%d"), i, j);
			point[j] = GetPrivateProfileIntA(group.c_str(), key.c_str(), 0, path.c_str());
			if (point[j] > ceilPoint[j]) point[j] = 0;
		}
		bool allDefault = true;
		for (int j = 0; j < 4; j++)
			if (point[j] != 0)
				allDefault = false;

		if (!allDefault) {
			if (Engine->m_PlayerBase->isMastered())
				txt_info[i]->SetString(string_format(xorstr("%s:%d %s:%d %s:%d Master:%d"), skillText[0].c_str(), point[0], skillText[1].c_str(), point[1], skillText[2].c_str(), point[2], point[3]));
			else
				txt_info[i]->SetString(string_format(xorstr("%s:%d %s:%d %s:%d"), skillText[0].c_str(), point[0], skillText[1].c_str(), point[1], skillText[2].c_str(), point[2]));
		}
		else
			txt_info[i]->SetString(xorstr("No Info"));
	}
}

uint16 CSkillPreset::GetTotalPoints()
{
	uint16 total = 0;
	for (int i = 0; i < 4; i++)
		total += point[i];
	return total;
}

void CSkillPreset::OpenStatSettings(uint8 id)
{
	SetPoints();
	selectedIndex = id;
	stat_txt_title->SetString(string_format("Skill Preset %d", selectedIndex + 1));
	GetSavedData(selectedIndex);
	base_stat->SetVisible(true);
}

uint8 CSkillPreset::GetCheckedIndex()
{
	for (int i = 0; i < 4; i++)
		if(btn_save[i] != nullptr)
			if (btn_save[i]->GetState() == UI_STATE_BUTTON_DOWN)
				return i;
	return 0;
}

void CSkillPreset::UncheckAll()
{
	for (int i = 0; i < 4; i++)
		if (btn_save[i] != nullptr)
			btn_save[i]->SetState(UI_STATE_BUTTON_NORMAL);
}

uint32_t CSkillPreset::MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld)
{
	uint32_t dwRet = UI_MOUSEPROC_NONE;
	if (IsVisible()) {
		if (Engine->uiSkillTreePlug != NULL && !posSet)
		{
			POINT uiSkillTreePos;
			POINT uiStatePos;
			Engine->GetUiPos(Engine->uiSkillTreePlug->m_dVTableAddr, uiSkillTreePos);
			Engine->GetUiPos(Engine->uiState->m_dVTableAddr, uiStatePos);
			posSet = true;
			SetPos(uiSkillTreePos.x - Engine->GetUiWidth(Engine->uiSkillTreePlug->m_dVTableAddr) + 80, uiStatePos.y + GetHeight() / 2 + 10);
		}
	}
	dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);
	return dwRet;
}

bool CSkillPreset::OnKeyPress(int iKey)
{
	if (Engine->m_UiMgr->uiSkillPreset == NULL)
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

	return true;
}

void CSkillPreset::Open()
{
	Engine->m_PlayerBase->UpdateFromMemory();
	LoadSavedData();
	SetVisible(true);
}

void CSkillPreset::Close()
{
	SetVisible(false);
}

void CSkillPreset::UpdatePosition()
{
	posSet = false;
	if (Engine->uiSkillTreePlug != NULL && !posSet)
	{
		POINT uiSkillTreePos;
		POINT uiStatePos;
		Engine->GetUiPos(Engine->uiSkillTreePlug->m_dVTableAddr, uiSkillTreePos);
		Engine->GetUiPos(Engine->uiState->m_dVTableAddr, uiStatePos);
		posSet = true;
		SetPos(uiSkillTreePos.x - Engine->GetUiWidth(Engine->uiSkillTreePlug->m_dVTableAddr) + 80, uiStatePos.y + GetHeight() / 2 + 10);
	}
}

void CSkillPreset::Tick()
{
	if (Engine->uiSkillTreePlug != NULL)
	{
		if (!Engine->IsVisible(Engine->uiSkillTreePlug->m_dVTableAddr))
			SetVisible(false);
	}
}