#include "stdafx.h"
#include "MessageBoxUIF.h"

CUIMessageBox::CUIMessageBox(MsgBoxTypes type, ParentTypes parentType)
{
	m_Type = type;
	m_ParentType = parentType;

	m_txtTitle = NULL;
	m_txtMessage = NULL;

	m_editTextbox = NULL;

	m_btnNo = NULL;
	m_btnYes = NULL;
	m_btnOk = NULL;
}

CUIMessageBox::~CUIMessageBox()
{
}

bool CUIMessageBox::Load(HANDLE hFile)
{
	if (CN3UIBase::Load(hFile) == false) return false;

	std::string find = xorstr("Text_title");
	m_txtTitle = (CN3UIString*)GetChildByID(find);
	find = xorstr("Text_Message");
	m_txtMessage = (CN3UIString*)GetChildByID(find);

	find = xorstr("Edit_Common");
	m_editTextbox = (CN3UIEdit*)GetChildByID(find);

	find = xorstr("Btn_Yes");
	m_btnYes = (CN3UIButton*)GetChildByID(find);
	find = xorstr("Btn_No");
	m_btnNo = (CN3UIButton*)GetChildByID(find);
	find = xorstr("btn_ok");
	m_btnOk = (CN3UIButton*)GetChildByID(find);

	if (m_Type == Ok || m_Type == OkWithEdit)
	{
		m_btnYes->SetVisible(false);
		m_btnNo->SetVisible(false);
		m_btnOk->SetVisible(true);
		SetVisibleTextbox(m_Type == OkWithEdit);
	}
	else if (m_Type == YesNo || m_Type == YesNoWithEdit)
	{
		m_btnYes->SetVisible(true);
		m_btnNo->SetVisible(true);
		m_btnOk->SetVisible(false);
		SetVisibleTextbox(m_Type == YesNoWithEdit);
	}
	else if (m_Type == Cancel)
	{
		m_btnYes->SetVisible(false);
		m_btnNo->SetVisible(false);
		m_btnOk->SetVisible(false);
		SetVisibleTextbox(false);
	}

	SetPos(Engine->m_UiMgr->GetScreenCenter(this).x, Engine->m_UiMgr->GetScreenCenter(this).y);

	return true;
}

void CUIMessageBox::Update(MsgBoxTypes type, ParentTypes parentType)
{
	m_Type = type;
	m_ParentType = parentType;

	if (m_Type == Ok || m_Type == OkWithEdit)
	{
		m_btnYes->SetVisible(false);
		m_btnNo->SetVisible(false);
		m_btnOk->SetVisible(true);
		SetVisibleTextbox(m_Type == OkWithEdit);
	}
	else if (m_Type == YesNo || m_Type == YesNoWithEdit)
	{
		m_btnYes->SetVisible(true);
		m_btnNo->SetVisible(true);
		m_btnOk->SetVisible(false);
		SetVisibleTextbox(m_Type == YesNoWithEdit);
	}
	else if (m_Type == Cancel)
	{
		m_btnYes->SetVisible(false);
		m_btnNo->SetVisible(false);
		SetVisibleTextbox(false);
		m_btnOk->SetVisible(false);
	}

	SetPos(Engine->m_UiMgr->GetScreenCenter(this).x, Engine->m_UiMgr->GetScreenCenter(this).y);
}

void CUIMessageBox::SetTitle(std::string title)
{
	m_txtTitle->SetString(title);
}

void CUIMessageBox::SetMessage(std::string msg)
{
	m_txtMessage->SetString(msg);
}

void CUIMessageBox::SetVisibleTextbox(bool bVisible)
{
	if (NULL == m_editTextbox) return;

	m_editTextbox->SetString("");
	m_editTextbox->SetVisible(bVisible);
	if (bVisible) m_editTextbox->SetFocus();
	else m_editTextbox->KillFocus();
}

bool CUIMessageBox::ReceiveMessage(CN3UIBase* pSender, uint32_t dwMsg)
{
	if (dwMsg == UIMSG_BUTTON_CLICK)
	{
		if (pSender == m_btnYes)
		{
			if (m_ParentType == PARENT_PAGE_STATE)
			{
				Packet result(WIZ_HOOK_GUARD, uint8(WIZ_HOOK_RESET));
				result << uint8(2);
				Engine->Send(&result);
			}
			else if (m_ParentType == PARENT_SKILL_TREE)
			{
				Packet result(WIZ_HOOK_GUARD, uint8(WIZ_HOOK_RESET));
				result << uint8(1);
				Engine->Send(&result);
			}
		}
		else if (pSender == m_btnNo)
		{
			
		}
		else if (pSender == m_btnOk)
		{
			if (m_ParentType == PARENT_LOGIN)
			{
				exit(0);
				FreeLibrary(GetModuleHandle(NULL));
				TerminateProcess(GetCurrentProcess(), 0);
			}
		}

		Close();
	}

	return true;
}

bool CUIMessageBox::OnKeyPress(int iKey)
{
	if (!IsVisible())
		return CN3UIBase::OnKeyPress(iKey);

	if (m_Type == Ok || m_Type == OkWithEdit)
	{
		if (iKey == DIK_ESCAPE || iKey == DIK_RETURN)
		{
			ReceiveMessage(m_btnOk, UIMSG_BUTTON_CLICK);
			return true;
		}
	}
	else if (m_Type == YesNo || m_Type == YesNoWithEdit)
	{
		if (iKey == DIK_ESCAPE)
		{
			ReceiveMessage(m_btnNo, UIMSG_BUTTON_CLICK);
			return true;
		}
		else if (iKey == DIK_RETURN)
		{
			ReceiveMessage(m_btnYes, UIMSG_BUTTON_CLICK);
			return true;
		}
	}
	else if (m_Type == Cancel)
	{
		if (iKey == DIK_ESCAPE)
		{
			ReceiveMessage(m_btnNo, UIMSG_BUTTON_CLICK);
			return true;
		}
	}

	return CN3UIBase::OnKeyPress(iKey);
}

void CUIMessageBox::Close()
{
	m_Type = None;
	m_ParentType = PARENT_NONE;

	this->SetVisible(false);
	this->SetMessage("");
	this->SetTitle("");
	this->SetVisibleTextbox(false);

	Engine->m_UiMgr->RemoveChild(this);
	Engine->m_UiMgr->uiMsgBox->Release();
	Engine->m_UiMgr->uiMsgBox = NULL;
}