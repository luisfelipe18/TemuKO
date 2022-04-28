#include "CTopLeft.h"

CTopLeft::CTopLeft()
{
	posSet = false;
}

CTopLeft::~CTopLeft()
{

}

bool CTopLeft::Load(HANDLE hFile)
{
	if (CN3UIBase::Load(hFile) == false) return false;

	std::string find = xorstr("btn_facebook");
	btn_facebook = (CN3UIButton*)GetChildByID(find);
	find = xorstr("btn_discord");
	btn_discord = (CN3UIButton*)GetChildByID(find);
	find = xorstr("btn_live");
	btn_live = (CN3UIButton*)GetChildByID(find);
	find = xorstr("btn_bug");
	btn_bug = (CN3UIButton*)GetChildByID(find);
	find = xorstr("btn_dropresult");
	btn_dropresult = (CN3UIButton*)GetChildByID(find);

	btn_dropresult->SetVisible(false);
	btn_dropresult->SetState(UI_STATE_BUTTON_DISABLE);

	if(Engine->EngineSettings->URL_Live.length() > 1)
		btn_live->SetState(UI_STATE_BUTTON_NORMAL);

	int xOrder = btn_facebook->GetPos().x;
	int xDiff = btn_discord->GetPos().x - xOrder;

	if (Engine->EngineSettings->Button_Facebook == 0)
		btn_facebook->SetVisible(false);
	else {
		btn_facebook->SetPos(xOrder, btn_facebook->GetPos().y);
		xOrder += xDiff;
	}

	if (Engine->EngineSettings->Button_Discord == 0)
		btn_discord->SetVisible(false);
	else {
		btn_discord->SetPos(xOrder, btn_discord->GetPos().y);
		xOrder += xDiff;
	}

	if (Engine->EngineSettings->Button_Live == 0)
		btn_live->SetVisible(false);
	else {
		btn_live->SetPos(xOrder, btn_live->GetPos().y);
		xOrder += xDiff;
	}

	if (Engine->EngineSettings->Button_Support == 0)
		btn_bug->SetVisible(false);
	else {
		btn_bug->SetPos(xOrder, btn_bug->GetPos().y);
		xOrder += xDiff;
	}

	btn_dropresult->SetPos(xOrder, btn_dropresult->GetPos().y);
	xOrder += xDiff;

	SetPos(Engine->m_UiMgr->GetScreenCenter(this).y, 5);

	return true;
}

void CTopLeft::DropResultStatus(bool status)
{
	if (this) 
	{
		btn_dropresult->SetVisible(status);
		btn_dropresult->SetState(status ? UI_STATE_BUTTON_NORMAL : UI_STATE_BUTTON_DISABLE);
	}
}

bool CTopLeft::ReceiveMessage(CN3UIBase* pSender, uint32_t dwMsg)
{
	if (dwMsg == UIMSG_BUTTON_CLICK)
	{
		if (pSender == btn_facebook)
			ShellExecute(NULL, "open", Engine->EngineSettings->URL_Facebook.c_str(), NULL, NULL, SW_SHOWNORMAL);
		else if(pSender == btn_discord)
			ShellExecute(NULL, "open", Engine->EngineSettings->URL_Discord.c_str(), NULL, NULL, SW_SHOWNORMAL);
		else if(pSender == btn_live)
			ShellExecute(NULL, "open", Engine->EngineSettings->URL_Live.c_str(), NULL, NULL, SW_SHOWNORMAL);
		else if (pSender == btn_bug)
		{
			if (Engine->uiState == NULL) 
			{
				Engine->m_UiMgr->ShowMessageBox(xorstr("Error"), xorstr("Support is available only after the game has started."), Ok);
				return true;
			}

			if (Engine->m_UiMgr->uiSupport != NULL)
				Engine->m_UiMgr->uiSupport->Release();

			Engine->m_UiMgr->uiSupport = new CSupport();
			Engine->m_UiMgr->uiSupport->LoadFromFile(Engine->m_BasePath + xorstr("TEMUKOACS\\ui\\support.uif"), N3FORMAT_VER_1068);
			Engine->m_UiMgr->AddChild(Engine->m_UiMgr->uiSupport);
		}
		else if (pSender == btn_dropresult)
		{
			if (Engine->m_UiMgr->uiDropResult == NULL)
				Engine->m_UiMgr->ShowDropResult();
			else 
			{
				if (Engine->m_UiMgr->uiDropResult->IsVisible())
					Engine->m_UiMgr->uiDropResult->Close();
				else
					Engine->m_UiMgr->uiDropResult->Open();
			}
		}
	}

	return true;
}

uint32_t CTopLeft::MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld)
{
	uint32_t dwRet = UI_MOUSEPROC_NONE;
	if (Engine->uiTargetBar != NULL && !posSet)
	{
		POINT eventNoticePos;
		Engine->GetUiPos(Engine->uiTargetBar->m_dVTableAddr, eventNoticePos);
		posSet = true;
		SetPos(eventNoticePos.y + Engine->GetUiWidth(Engine->uiTargetBar->m_dVTableAddr) + 5, 5);
	}
	dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);
	return dwRet;
}

bool CTopLeft::OnKeyPress(int iKey)
{
	if (!IsVisible())
		return CN3UIBase::OnKeyPress(iKey);

	return true;
}

void CTopLeft::Open()
{
	SetVisible(true);
}

void CTopLeft::Close()
{
	SetVisible(false);
}

void CTopLeft::UpdatePosition()
{
	posSet = false;
	if (Engine->uiTargetBar != NULL && !posSet)
	{
		POINT eventNoticePos;
		Engine->GetUiPos(Engine->uiTargetBar->m_dVTableAddr, eventNoticePos);
		posSet = true;
		SetPos(eventNoticePos.y + Engine->GetUiWidth(Engine->uiTargetBar->m_dVTableAddr) + 5, 5);
	}
}