#include "StatPreset.h"

CStatPreset::CStatPreset()
{
	posSet = false;
	selectedIndex = 0;
	points = 0;
	for (int i = 0; i < 5; i++) {
		point[i] = 0;
		basePoint[i] = 0;
	}

	if (Engine->m_PlayerBase != NULL) {
		Engine->m_PlayerBase->UpdateFromMemory();
		uint8 level = Engine->m_PlayerBase->GetLevel();

		basePoint[0] = 50;
		basePoint[1] = 60;
		basePoint[2] = 60;
		basePoint[3] = 50;
		basePoint[4] = 50;

		if (Engine->m_PlayerBase->isPriest())
			basePoint[4] += 20;
		else if (Engine->m_PlayerBase->isWarrior())
		{
			basePoint[0] += 15;
			basePoint[1] += 5;
		}
		else if (Engine->m_PlayerBase->isMage())
		{
			basePoint[2] += 10;
			basePoint[4] += 20;
			basePoint[1] -= 10;
		}
		else if (Engine->m_PlayerBase->isRogue())
		{
			basePoint[0] += 10;
			basePoint[2] += 10;
		}

		points = 10 + (level - 1) * 3;

		if (level > 60)
			points += 2 * (level - 60);

		for (int i = 0; i < 5; i++)
			point[i] = basePoint[i];
	}
}

CStatPreset::~CStatPreset()
{

}

bool CStatPreset::Load(HANDLE hFile)
{
	if (CN3UIBase::Load(hFile) == false) return false;

	std::string find = xorstr("btn_close");
	btn_close = (CN3UIButton*)GetChildByID(find);

	CN3UIBase* base_slot = NULL;
	find = xorstr("base_slot");
	base_slot = (CN3UIBase*)GetChildByID(find);
	
	for (int i = 1; i <= 4; i++)
	{
		find = string_format(xorstr("btn_save%d"), i);
		btn_save[i-1] = (CN3UIButton*)base_slot->GetChildByID(find);
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

	for (int i = 1; i <= 5; i++)
	{
		find = string_format(xorstr("txt_stat%d"), i);
		txt_stat[i-1] = (CN3UIString*)base_stat->GetChildByID(find);
		find = string_format(xorstr("txt_point%d"), i);
		txt_point[i - 1] = (CN3UIString*)base_stat->GetChildByID(find);
	}

	find =xorstr("txt_points");
	txt_points = (CN3UIString*)base_stat->GetChildByID(find);

	base_stat->SetVisible(false);

	LoadSavedData();

	SetPos(Engine->m_UiMgr->GetScreenCenter(this).x, Engine->m_UiMgr->GetScreenCenter(this).y);

	return true;
}

void CStatPreset::TickPoints()
{
	for (int i = 0; i < 5; i++)
		txt_point[i]->SetString(string_format(xorstr("%d"), point[i]));

	txt_points->SetString(string_format(xorstr("%d"), points));
}

void CStatPreset::SetPoints()
{
	uint8 level = Engine->m_PlayerBase->GetLevel();

	basePoint[0] = 50;
	basePoint[1] = 60;
	basePoint[2] = 60;
	basePoint[3] = 50;
	basePoint[4] = 50;

	if (Engine->m_PlayerBase->isPriest())
		basePoint[4] += 20;
	else if (Engine->m_PlayerBase->isWarrior())
	{
		basePoint[0] += 15;
		basePoint[1] += 5;
	}
	else if (Engine->m_PlayerBase->isMage())
	{
		basePoint[2] += 10;
		basePoint[4] += 20;
		basePoint[1] -= 10;
	}
	else if (Engine->m_PlayerBase->isRogue())
	{
		basePoint[0] += 10;
		basePoint[2] += 10;
	}

	points = 10 + (level - 1) * 3;

	if (level > 60)
		points += 2 * (level - 60);

	for (int i = 0; i < 5; i++)
		point[i] = basePoint[i];

	for(int i = 0; i < 5; i++)
		txt_point[i]->SetString(string_format(xorstr("%d"), point[i]));

	txt_points->SetString(string_format(xorstr("%d"), points));
}

bool CStatPreset::ReceiveMessage(CN3UIBase* pSender, uint32_t dwMsg)
{
	if (dwMsg == UIMSG_BUTTON_CLICK)
	{
		if (pSender == btn_close)
			Close();

		for (int i = 0; i < 4; i++)
		{
			if (pSender == btn_save[i])
			{
				UncheckAll();
				btn_save[i]->SetState(UI_STATE_BUTTON_DOWN);
				selectedIndex = i;
			}
		}

		for (int i = 0; i < 2; i++)
		{
			if (pSender == btn_str[i])
			{
				if ((points <= 0 && i == 1) || (points >= 302 && i == 0))
					return true;

				if (i == 0) {
					if (point[0] > basePoint[0]) {
						point[0]--;
						points++;
					}
				}
				else {
					if (point[0] < 255) {
						point[0]++;
						points--;
					}
				}
			}
			if (pSender == btn_hp[i])
			{
				if ((points <= 0 && i == 1) || (points >= 302 && i == 0))
					return true;

				if (i == 0) {
					if (point[1] > basePoint[1]) {
						point[1]--;
						points++;
					}
				}
				else {
					if (point[1] < 255) {
						point[1]++;
						points--;
					}
				}
			}
			if (pSender == btn_dex[i])
			{
				if ((points <= 0 && i == 1) || (points >= 302 && i == 0))
					return true;

				if (i == 0) {
					if (point[2] > basePoint[2]) {
						point[2]--;
						points++;
					}
				}
				else {
					if (point[2] < 255) {
						point[2]++;
						points--;
					}
				}
			}
			if (pSender == btn_mp[i])
			{
				if ((points <= 0 && i == 1) || (points >= 302 && i == 0))
					return true;

				if (i == 0) {
					if (point[3] > basePoint[3]) {
						point[3]--;
						points++;
					}
				}
				else {
					if (point[3] < 255) {
						point[3]++;
						points--;
					}
				}
			}
			if (pSender == btn_int[i])
			{
				if ((points <= 0 && i == 1) || (points >= 302 && i == 0))
					return true;

				if (i == 0) {
					if (point[4] > basePoint[4]) {
						point[4]--;
						points++;
					}
				}
				else {
					if (point[4] < 255) {
						point[4]++;
						points--;
					}
				}
			}
		}

		if (pSender == btn_edit)
			OpenStatSettings(GetCheckedIndex());

		if (pSender == stat_btn_save)
		{
			string path = Engine->m_BasePath + xorstr("preset.ini");
			string group = string_format(xorstr("%s"), Engine->m_PlayerBase->m_strCharacterName.c_str());
			string key = "";
			for (int i = 0; i < 5; i++)
			{
				key = string_format(xorstr("statslot%d_%d"), GetCheckedIndex(), i);
				WritePrivateProfileStringA(group.c_str(), key.c_str(), to_string(point[i]).c_str(), path.c_str());
			}
			txt_info[GetCheckedIndex()]->SetString(string_format(xorstr("STR:%d HP:%d DEX:%d MP:%d INT:%d"), point[0], point[1], point[2], point[3], point[4]));
			base_stat->SetVisible(false);
		}

		if (pSender == stat_btn_cancel)
			base_stat->SetVisible(false);

		if (pSender == btn_apply)
		{
			GetSavedData(GetCheckedIndex());

			Packet result(WIZ_HOOK_GUARD, uint8(WIZ_HOOK_RESET));
			result << uint8(2);

			for (int i = 0; i < 5; i++)
				result << point[i];

			Engine->Send(&result);
		}
	}

	TickPoints();

	return true;
}

void CStatPreset::GetSavedData(uint8 id)
{
	SetPoints();

	string path = Engine->m_BasePath + xorstr("preset.ini");
	string group = string_format(xorstr("%s"), Engine->m_PlayerBase->m_strCharacterName.c_str());
	string key = "";

	uint16 totalMinus = 0;
	for (int i = 0; i < 5; i++)
	{
		key = string_format(xorstr("statslot%d_%d"), id, i);
		point[i] = GetPrivateProfileIntA(group.c_str(), key.c_str(), basePoint[i], path.c_str());
		if (point[i] < basePoint[i]) point[i] = basePoint[i];
		totalMinus += point[i] - basePoint[i];
	}

	if (totalMinus > points)
	{
		SetPoints();
		return;
	}

	points -= totalMinus;

	TickPoints();
}

void CStatPreset::LoadSavedData()
{
	Engine->m_PlayerBase->UpdateFromMemory();

	string path = Engine->m_BasePath + xorstr("preset.ini");
	string group = string_format(xorstr("%s"), Engine->m_PlayerBase->m_strCharacterName.c_str());
	string key = "";

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			key = string_format(xorstr("statslot%d_%d"), i, j);
			point[j] = GetPrivateProfileIntA(group.c_str(), key.c_str(), basePoint[j], path.c_str());
			if (point[j] < basePoint[j]) point[j] = basePoint[j];
		}
		bool allDefault = true;
		for (int j = 0; j < 5; j++)
			if (point[j] != basePoint[j])
				allDefault = false;

		if(!allDefault)
			txt_info[i]->SetString(string_format(xorstr("STR:%d HP:%d DEX:%d MP:%d INT:%d"), point[0], point[1], point[2], point[3], point[4]));
		else
			txt_info[i]->SetString(xorstr("No Info"));
	}
}

uint16 CStatPreset::GetTotalPoints()
{
	uint16 total = 0;
	for (int i = 0; i < 5; i++)
		total += point[i];
	return total;
}

void CStatPreset::OpenStatSettings(uint8 id)
{
	SetPoints();
	selectedIndex = id;
	stat_txt_title->SetString(string_format("Stat Preset %d", selectedIndex + 1));
	GetSavedData(selectedIndex);
	base_stat->SetVisible(true);
}

uint8 CStatPreset::GetCheckedIndex()
{
	for (int i = 0; i < 4; i++)
		if (btn_save[i]->GetState() == UI_STATE_BUTTON_DOWN)
			return i;
	return 0;
}

void CStatPreset::UncheckAll()
{
	for (int i = 0; i < 4; i++)
		btn_save[i]->SetState(UI_STATE_BUTTON_NORMAL);
}

uint32_t CStatPreset::MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld)
{
	uint32_t dwRet = UI_MOUSEPROC_NONE;
	if (IsVisible()) {
		if (Engine->uiState != NULL && !posSet)
		{
			POINT uiStatePos;
			Engine->GetUiPos(Engine->uiState->m_dVTableAddr, uiStatePos);
			posSet = true;
			SetPos(uiStatePos.x + Engine->GetUiWidth(Engine->uiState->m_dVTableAddr), uiStatePos.y + GetHeight() / 2 + 10);
		}
	}
	dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);
	return dwRet;
}

bool CStatPreset::OnKeyPress(int iKey)
{
	if (Engine->m_UiMgr->uiStatPreset == NULL)
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

void CStatPreset::Open()
{
	LoadSavedData();
	SetVisible(true);
}

void CStatPreset::Close()
{
	SetVisible(false);
}

void CStatPreset::UpdatePosition()
{
	posSet = false;
	if (Engine->uiState != NULL && !posSet)
	{
		POINT uiStatePos;
		Engine->GetUiPos(Engine->uiState->m_dVTableAddr, uiStatePos);
		posSet = true;
		SetPos(uiStatePos.x + Engine->GetUiWidth(Engine->uiState->m_dVTableAddr), uiStatePos.y + GetHeight() / 2 + 10);
	}
}

void CStatPreset::Tick()
{
	if (Engine->uiState != NULL)
	{
		if (!Engine->IsVisible(Engine->uiState->m_dVTableAddr))
			SetVisible(false);
	}
}