#include "GMTools.h"

CGMTools::CGMTools()
{
	txt_nick = NULL;
	txt_detay = NULL;
	txt_durum = NULL;
	txt_letter = NULL;

	btn_close = NULL;
	btn_find = NULL;
	btn_select = NULL;
	btn_startgenie = NULL;
	btn_stopgenie = NULL;
	btn_scan = NULL;
	btn_tpon = NULL;
	btn_tpoff = NULL;
	btn_dc = NULL;
	btn_jail = NULL;
	btn_ban = NULL;
	btn_bdw = NULL;
	btn_chaos = NULL;
	btn_jr = NULL;
	btn_cr = NULL;
	btn_letter = NULL;
	btn_online = NULL;
}

CGMTools::~CGMTools()
{

}

bool CGMTools::Load(HANDLE hFile)
{
	if (CN3UIBase::Load(hFile) == false) return false;

	SetPos(Engine->m_UiMgr->GetScreenCenter(this).x, Engine->m_UiMgr->GetScreenCenter(this).y);

	return true;
}

bool CGMTools::ReceiveMessage(CN3UIBase* pSender, uint32_t dwMsg)
{
	if (dwMsg == UIMSG_BUTTON_CLICK)
	{

	}

	return true;
}

uint32_t CGMTools::MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld)
{
	uint32_t dwRet = UI_MOUSEPROC_NONE;
	dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);
	return dwRet;
}

bool CGMTools::OnKeyPress(int iKey)
{
	if (!IsVisible())
		return CN3UIBase::OnKeyPress(iKey);

	switch (iKey)
	{
	case DIK_ESCAPE:
		Close();
		return true;
	}

	return CN3UIBase::OnKeyPress(iKey);
}

void CGMTools::Open()
{
	SetVisible(true);
}

void CGMTools::Close()
{
	SetVisible(false);
}