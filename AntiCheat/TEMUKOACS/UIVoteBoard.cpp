#include "UIVoteBoard.h"

CVoteBoard::CVoteBoard()
{

}

CVoteBoard::~CVoteBoard()
{

}

bool CVoteBoard::Load(HANDLE hFile)
{
	if (CN3UIBase::Load(hFile) == false) return false;

	std::string find = xorstr("edit_subject");

	SetPos(Engine->m_UiMgr->GetScreenCenter(this).x, Engine->m_UiMgr->GetScreenCenter(this).y);

	return true;
}

bool CVoteBoard::ReceiveMessage(CN3UIBase* pSender, uint32_t dwMsg)
{
	if (dwMsg == UIMSG_BUTTON_CLICK)
	{
		
	}

	return true;
}

uint32_t CVoteBoard::MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld)
{
	uint32_t dwRet = UI_MOUSEPROC_NONE;
	dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);
	return dwRet;
}

bool CVoteBoard::OnKeyPress(int iKey)
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

void CVoteBoard::Open()
{

	SetVisible(true);
}

void CVoteBoard::Close()
{
	Engine->m_UiMgr->RemoveChild(this);
}