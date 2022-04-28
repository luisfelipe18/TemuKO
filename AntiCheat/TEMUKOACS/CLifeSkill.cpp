#include "CLifeSkill.h"

CLifeSkill::CLifeSkill()
{
	btn_close = NULL;
	for (int i = 0; i < 4; i++)
	{
		txt_level[i] = NULL;
		txt_exp[i] = NULL;
		progress_exp[i] = NULL;
	}
}

CLifeSkill::~CLifeSkill()
{

}

bool CLifeSkill::Load(HANDLE hFile)
{
	if (CN3UIBase::Load(hFile) == false) return false;

	std::string find = xorstr("btn_close");
	btn_close = (CN3UIButton*)GetChildByID(find);
	find = xorstr("btn_troop");
	btn_troop = (CN3UIButton*)GetChildByID(find);

	for (int i = 0; i < 4; i++)
	{
		find = string_format(xorstr("txt_level_%d"), i + 1);
		txt_level[i] = (CN3UIString*)GetChildByID(find);
		find = string_format(xorstr("txt_exp_%d"), i + 1);
		txt_exp[i] = (CN3UIString*)GetChildByID(find);
		find = string_format(xorstr("progress_exp_%d"), i + 1);
		progress_exp[i] = (CN3UIProgress*)GetChildByID(find);

		// Reset All
		txt_level[i]->SetString(xorstr("LvL. 1"));
		txt_exp[i]->SetString(xorstr("0%"));
		progress_exp[i]->SetRange(0, 100);
		progress_exp[i]->SetCurValue(0);
	}

	btn_troop->SetState(UI_STATE_BUTTON_DOWN);

	SetPos(Engine->m_UiMgr->GetScreenCenter(this).x, Engine->m_UiMgr->GetScreenCenter(this).y);

	return true;
}

void CLifeSkill::Update(Packet pkt)
{
	for (int i = 0; i < 4; i++)
	{
		uint16 level;
		float percent;
		pkt >> level >> percent;

		txt_level[i]->SetString(string_format(xorstr("LvL. %d"), level));
		txt_exp[i]->SetString(string_format(xorstr("%.2lf%%"), percent));
		progress_exp[i]->SetRange(0, 100);
		if(percent < progress_exp[i]->GetCurValue())
			progress_exp[i]->SetCurValue((int)percent);
		else
			progress_exp[i]->SetCurValue((int)percent, 0.5F, 40.0F);
	}
}

bool CLifeSkill::ReceiveMessage(CN3UIBase* pSender, uint32_t dwMsg)
{
	if (dwMsg == UIMSG_BUTTON_CLICK)
	{
		if (pSender == btn_close)
			Close();
	}

	return true;
}

uint32_t CLifeSkill::MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld)
{
	uint32_t dwRet = UI_MOUSEPROC_NONE;
	dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);
	return dwRet;
}

bool CLifeSkill::OnKeyPress(int iKey)
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

void CLifeSkill::Open()
{
	SetVisible(true);
}

void CLifeSkill::Close()
{
	SetVisible(false);
}