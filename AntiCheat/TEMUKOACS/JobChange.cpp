#include "JobChange.h"

CJobChange::CJobChange()
{
	
}

CJobChange::~CJobChange()
{

}

bool CJobChange::Load(HANDLE hFile)
{
	if (CN3UIBase::Load(hFile) == false) return false;

	std::string find = xorstr("btn_warrior");
	btn_warrior = (CN3UIButton*)GetChildByID(find);
	find = xorstr("btn_rogue");
	btn_rogue = (CN3UIButton*)GetChildByID(find);
	find = xorstr("btn_mage");
	btn_mage = (CN3UIButton*)GetChildByID(find);
	find = xorstr("btn_priest");
	btn_priest = (CN3UIButton*)GetChildByID(find);

	if (Engine->m_PlayerBase != NULL) {
		if (Engine->m_PlayerBase->isWarrior())
			btn_warrior->SetState(UI_STATE_BUTTON_DISABLE);
		else if (Engine->m_PlayerBase->isRogue())
			btn_rogue->SetState(UI_STATE_BUTTON_DISABLE);
		else if (Engine->m_PlayerBase->isMage())
			btn_mage->SetState(UI_STATE_BUTTON_DISABLE);
		else if (Engine->m_PlayerBase->isPriest())
			btn_priest->SetState(UI_STATE_BUTTON_DISABLE);
	}

	SetPos(Engine->m_UiMgr->GetScreenCenter(this).x, Engine->m_UiMgr->GetScreenCenter(this).y);

	return true;
}

bool CJobChange::ReceiveMessage(CN3UIBase* pSender, uint32_t dwMsg)
{
	if (dwMsg == UIMSG_BUTTON_CLICK)
	{
		
	}

	return true;
}

uint32_t CJobChange::MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld)
{
	uint32_t dwRet = UI_MOUSEPROC_NONE;
	dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);
	return dwRet;
}

bool CJobChange::OnKeyPress(int iKey)
{
	if (!IsVisible())
		return CN3UIBase::OnKeyPress(iKey);

	return true;
}

void CJobChange::Open()
{
	SetVisible(true);
}

void CJobChange::Close()
{
	SetVisible(false);
}