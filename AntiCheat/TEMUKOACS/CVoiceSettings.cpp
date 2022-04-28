#include "CVoiceSettings.h"

CVoiceSettings::CVoiceSettings()
{
	btn_close = NULL;
	btn_micstate = NULL;
	btn_mute_all = NULL;
	btn_mute_listening = NULL;
	btn_mute_speaking = NULL;
	edit_hotkey = NULL;
}

CVoiceSettings::~CVoiceSettings()
{
	btn_close = NULL;
	btn_micstate = NULL;
	btn_mute_all = NULL;
	btn_mute_listening = NULL;
	btn_mute_speaking = NULL;
	edit_hotkey = NULL;
}

bool CVoiceSettings::Load(HANDLE hFile)
{
	if (CN3UIBase::Load(hFile) == false) return false;

	std::string find = xorstr("btn_close");
	btn_close = (CN3UIButton*)GetChildByID(find);
	find = xorstr("btn_micstate");
	btn_micstate = (CN3UIButton*)GetChildByID(find);
	find = xorstr("btn_mute_all");
	btn_mute_all = (CN3UIButton*)GetChildByID(find);
	find = xorstr("btn_mute_listening");
	btn_mute_listening = (CN3UIButton*)GetChildByID(find);
	find = xorstr("btn_mute_speaking");
	btn_mute_speaking = (CN3UIButton*)GetChildByID(find);

	find = xorstr("edit_hotkey");
	edit_hotkey = (CN3UIEdit*)GetChildByID(find);

	SetPos(Engine->m_UiMgr->GetScreenCenter(this).x, Engine->m_UiMgr->GetScreenCenter(this).y);

	LoadSettings();

	return true;
}

bool CVoiceSettings::ReceiveMessage(CN3UIBase* pSender, uint32_t dwMsg)
{
	if (dwMsg == UIMSG_BUTTON_CLICK)
	{
		if (pSender == btn_close)
			Close();
		else
			SaveSettings();
	}

	return true;
}

uint32_t CVoiceSettings::MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld)
{
	uint32_t dwRet = UI_MOUSEPROC_NONE;
	dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);
	return dwRet;
}

bool CVoiceSettings::OnKeyPress(int iKey)
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

void CVoiceSettings::Open()
{
	LoadSettings();
	SetVisible(true);
}

void CVoiceSettings::Close()
{
	SetVisible(false);
}

void CVoiceSettings::LoadSettings()
{
	if (Engine->m_SettingsMgr == NULL)
	{
		Engine->m_SettingsMgr = new CSettingsManager();
		Engine->m_SettingsMgr->Init();
	}

	btn_micstate->SetState(Engine->m_SettingsMgr->m_micState == 1 ? UI_STATE_BUTTON_DOWN : UI_STATE_BUTTON_NORMAL);
	btn_mute_all->SetState(Engine->m_SettingsMgr->m_muteAll == 1 ? UI_STATE_BUTTON_DOWN : UI_STATE_BUTTON_NORMAL);
	btn_mute_listening->SetState(Engine->m_SettingsMgr->m_muteListening == 1 ? UI_STATE_BUTTON_DOWN : UI_STATE_BUTTON_NORMAL);
	btn_mute_speaking->SetState(Engine->m_SettingsMgr->m_muteSpeaking == 1 ? UI_STATE_BUTTON_DOWN : UI_STATE_BUTTON_NORMAL);
}

void CVoiceSettings::SaveSettings()
{
	if (Engine->m_SettingsMgr == NULL)
	{
		Engine->m_SettingsMgr = new CSettingsManager();
		Engine->m_SettingsMgr->Init();
	}

	Engine->m_SettingsMgr->m_micState = btn_micstate->GetState() == UI_STATE_BUTTON_DOWN ? 1 : 0;
	Engine->m_SettingsMgr->m_muteAll = btn_mute_all->GetState() == UI_STATE_BUTTON_DOWN ? 1 : 0;
	Engine->m_SettingsMgr->m_muteListening = btn_mute_listening->GetState() == UI_STATE_BUTTON_DOWN ? 1 : 0;
	Engine->m_SettingsMgr->m_muteSpeaking = btn_mute_speaking->GetState() == UI_STATE_BUTTON_DOWN ? 1 : 0;
}