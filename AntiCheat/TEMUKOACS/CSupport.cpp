#include "CSupport.h"

#define PL_SUPPORT_CHECK 10
time_t PL_SUPPORT_LAST = 0;
time_t t = time(nullptr);

CSupport::CSupport()
{
	bug = true;
	txt_subject = NULL;
	txt_message = NULL;
	btn_bug = NULL;
	btn_koxp = NULL;
	btn_report = NULL;
	btn_close = NULL;
	spamTimer = NULL;
}

CSupport::~CSupport()
{

}

bool CSupport::Load(HANDLE hFile)
{
	if (CN3UIBase::Load(hFile) == false) return false;

	std::string find = xorstr("edit_subject");
	CN3UIEdit* tmp = (CN3UIEdit*)GetChildByID(find);
	find = xorstr("txt_subject");
	txt_subject = (CN3UIString*)tmp->GetChildByID(find);

	find = xorstr("edit_message");
	tmp = (CN3UIEdit*)GetChildByID(find);
	find = xorstr("txt_message");
	txt_message = (CN3UIString*)tmp->GetChildByID(find);

	find = xorstr("btn_bug");
	btn_bug = (CN3UIButton*)GetChildByID(find);
	find = xorstr("btn_koxp");
	btn_koxp = (CN3UIButton*)GetChildByID(find);
	find = xorstr("btn_report");
	btn_report = (CN3UIButton*)GetChildByID(find);
	find = xorstr("btn_close");
	btn_close = (CN3UIButton*)GetChildByID(find);

	btn_bug->SetState(UI_STATE_BUTTON_DOWN);

	SetPos(Engine->m_UiMgr->GetScreenCenter(this).x, Engine->m_UiMgr->GetScreenCenter(this).y);

	return true;
}

bool CSupport::ReceiveMessage(CN3UIBase* pSender, uint32_t dwMsg)
{
	if (dwMsg == UIMSG_BUTTON_CLICK)
	{
		if (pSender == btn_bug || pSender == btn_koxp)
		{
			bug = !bug;
			if (bug) {
				btn_bug->SetState(UI_STATE_BUTTON_DOWN);
				btn_koxp->SetState(UI_STATE_BUTTON_NORMAL);
			}
			else {
				btn_bug->SetState(UI_STATE_BUTTON_NORMAL);
				btn_koxp->SetState(UI_STATE_BUTTON_DOWN);
			}
		}
		else if (pSender == btn_close)
			Close();
		else if (pSender == btn_report)
		{
			string subject = txt_subject->GetString();
			string message = txt_message->GetString();
			if (subject.size() > 25)
			{
				Engine->m_UiMgr->ShowMessageBox(xorstr("Support Failed"), xorstr("The subject cannot be longer than 25 characters."), Ok);
				Close();
				return true;
			}
			if (message.size() > 40)
			{
				Engine->m_UiMgr->ShowMessageBox(xorstr("Support Failed"), xorstr("The message cannot be longer than 40 characters."), Ok);
				Close();
				return true;
			}
			if (subject.size() < 3)
			{
				Engine->m_UiMgr->ShowMessageBox(xorstr("Support Failed"), xorstr("The subject must be at least 3 characters."), Ok);
				Close();
				return true;
			}
			if (message.size() < 10)
			{
				Engine->m_UiMgr->ShowMessageBox(xorstr("Support Failed"), xorstr("The message must be at least 10 characters."), Ok);
				Close();
				return true;
			}

			Packet result(WIZ_HOOK_GUARD, uint8(WIZ_HOOK_SUPPORT));
			result << uint8(bug ? 0 : 1) << subject << message;
			Engine->Send(&result);

			Engine->m_UiMgr->ShowMessageBox(xorstr("Support"), xorstr("Your message has been sent."), Ok);
			Close();
		}
	}

	return true;
}

uint32_t CSupport::MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld)
{
	uint32_t dwRet = UI_MOUSEPROC_NONE;
	dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);
	return dwRet;
}

bool CSupport::OnKeyPress(int iKey)
{
	if(Engine->m_UiMgr->uiSupport == NULL)
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

void CSupport::Open()
{
	*localtime(&t);
	if (PL_SUPPORT_LAST == 0)
		PL_SUPPORT_LAST = t;

	if (t - PL_SUPPORT_LAST < PL_SUPPORT_CHECK)
	{
		Engine->m_UiMgr->ShowMessageBox(xorstr("Support"), xorstr("You need to wait 10 seconds to report a new bug."), Ok);
		return;
	}

	PL_SUPPORT_LAST = t;

	SetVisible(true);
}

void CSupport::Close()
{
	Engine->m_UiMgr->RemoveChild(this);
	Engine->m_UiMgr->uiSupport->Release();
	Engine->m_UiMgr->uiSupport = NULL;
}