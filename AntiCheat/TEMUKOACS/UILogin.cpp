#include "UILogin.h"

uint32 idLastState, pwLastState;

CUILogin::CUILogin()
{
	if (Engine->m_SettingsMgr == NULL) {
		Engine->m_SettingsMgr = new CSettingsManager();
		Engine->m_SettingsMgr->Init();
	}

	vector<int>offsets;
	offsets.push_back(0x28);
	offsets.push_back(0);

	m_dVTableAddr = Engine->rdword(0xE47868, offsets);

	m_bGroupLogin = 0;
	m_btnRememberID = 0;
	m_TextUID = 0;
	m_EditPW = 0;
	m_EditUID = 0;

	ParseUIElements();
	
}

CUILogin::~CUILogin()
{
}

void CUILogin::ParseUIElements()
{
	std::string find = xorstr("Group_Login");
	Engine->GetChildByID(m_dVTableAddr, find, m_bGroupLogin);

	find = xorstr("btn_remember_id");
	Engine->GetChildByID(m_bGroupLogin, find, m_btnRememberID);
	idLastState = Engine->GetState(m_btnRememberID);

	find = xorstr("btn_ok");
	Engine->GetChildByID(m_bGroupLogin, find, m_btnLogin);

	find = xorstr("Edit_ID");
	Engine->GetChildByID(m_bGroupLogin, find, m_EditUID);
	find = xorstr("Text_UID");
	Engine->GetChildByID(m_EditUID, find, m_TextUID);

	find = xorstr("Edit_PW");
	Engine->GetChildByID(m_bGroupLogin, find, m_EditPW);
	find = xorstr("Text_UPW");
	Engine->GetChildByID(m_EditPW, find, m_TextPW);

	Engine->SetString(m_TextUID, Engine->m_SettingsMgr->m_uID);

	if (Engine->m_SettingsMgr->m_uID.length() > 0)
		Engine->SetState(m_btnRememberID, UI_STATE_BUTTON_DOWN);

	InitReceiveMessage();
}

DWORD uiLoginVTable;

bool CUILogin::ReceiveMessage(DWORD* pSender, uint32_t dwMsg)
{
	uiLoginVTable = m_dVTableAddr;
	if (!pSender || pSender == 0 || dwMsg != UIMSG_BUTTON_CLICK)
		return false;
	
	if (pSender == (DWORD*)m_btnLogin)
	{
		if (Engine->GetState(m_btnRememberID) == UI_STATE_BUTTON_DOWN)
		{
			Engine->m_SettingsMgr->m_uID = Engine->GetString(m_TextUID);
			Engine->m_SettingsMgr->Save();
		}
		else {
			Engine->m_SettingsMgr->m_uID = "";
			Engine->m_SettingsMgr->Save();
		}
	}
	return true;
}

void __stdcall UiLoginReceiveMessage_Hook(DWORD* pSender, uint32_t dwMsg)
{
	Engine->uiLogin->ReceiveMessage(pSender, dwMsg);
	__asm
	{
		MOV ECX, uiLoginVTable
		PUSH dwMsg
		PUSH pSender
		MOV EAX, KO_UI_LOGIN_RECEIVE_MESSAGE_FUNC
		CALL EAX
	}
}

void CUILogin::Login(string account, string password)
{
	char* pw = new char[password.length()];
	for (int i = 0; i < password.length(); i++)
		pw[i] = '*';
	Engine->SetString(m_TextUID, account);
	Engine->SetString(m_TextPW, string(pw).substr(0, password.length()));
	Engine->WriteString(m_EditUID + 0x15C, (char*)account.c_str());
	Engine->WriteString(m_EditPW + 0x140, (char*)password.c_str());
	Engine->SetState(m_EditUID, UI_STATE_EDTCTRL_ACTIVE);
	Engine->SetState(m_EditPW, UI_STATE_EDTCTRL_ACTIVE);
	UiLoginReceiveMessage_Hook((DWORD*)m_btnLogin, UIMSG_BUTTON_CLICK);
}

void CUILogin::InitReceiveMessage()
{
	*(DWORD*)KO_UI_LOGIN_RECEIVE_MESSAGE_PTR = (DWORD)UiLoginReceiveMessage_Hook;
}