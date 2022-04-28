#include "JoinBoard.h"

CJoinBoard::CJoinBoard()
{
	grp_minimum = NULL;
	grp_maximum = NULL;
	grp_rewards = NULL;
	btn_icon = NULL;
	btn_maximum = NULL;
	btn_enter = NULL;
	btn_cancel = NULL;
	btn_rewards_open = NULL;
	btn_rewards_close = NULL;
	txt_notice = NULL;
	txt_time = NULL;
	txt_participant = NULL;
	slot_victory = NULL;
	slot_defeat = NULL;
	slot_victory_main = NULL;
	slot_defeat_main = NULL;
	m_Timer = NULL;
	m_iRemainingTime = NULL;
}

CJoinBoard::~CJoinBoard()
{
	grp_minimum = NULL;
	grp_maximum = NULL;
	grp_rewards = NULL;
	btn_icon = NULL;
	btn_maximum = NULL;
	btn_enter = NULL;
	btn_cancel = NULL;
	btn_rewards_open = NULL;
	btn_rewards_close = NULL;
	txt_notice = NULL;
	txt_time = NULL;
	txt_participant = NULL;
	slot_victory = NULL;
	slot_defeat = NULL;
	slot_victory_main = NULL;
	slot_defeat_main = NULL;
	m_Timer = NULL;
	m_iRemainingTime = NULL;
}

bool CJoinBoard::Load(HANDLE hFile)
{
	if (CN3UIBase::Load(hFile) == false) return false;

	std::string find = xorstr("grp_minimum");
	grp_minimum = (CN3UIBase*)GetChildByID(find);
	find = xorstr("grp_maximum");
	grp_maximum = (CN3UIBase*)GetChildByID(find);
	find = xorstr("grp_rewards");
	grp_rewards = (CN3UIBase*)GetChildByID(find);

	// Group Minimum
	find = xorstr("btn_icon");
	btn_icon = (CN3UIButton*)grp_minimum->GetChildByID(find);
	find = xorstr("btn_maximum");
	btn_maximum = (CN3UIButton*)grp_minimum->GetChildByID(find);
	find = xorstr("txt_notice");
	txt_notice = (CN3UIString*)grp_minimum->GetChildByID(find);
	find = xorstr("txt_time");
	txt_time = (CN3UIString*)grp_minimum->GetChildByID(find);

	// Group Maximum
	find = xorstr("btn_enter");
	btn_enter = (CN3UIButton*)grp_maximum->GetChildByID(find);
	find = xorstr("btn_cancel");
	btn_cancel = (CN3UIButton*)grp_maximum->GetChildByID(find);
	find = xorstr("btn_rewards_open");
	btn_rewards_open = (CN3UIButton*)grp_maximum->GetChildByID(find);
	find = xorstr("btn_rewards_close");
	btn_rewards_close = (CN3UIButton*)grp_maximum->GetChildByID(find);
	find = xorstr("txt_content");
	txt_content = (CN3UIString*)grp_maximum->GetChildByID(find);
	find = xorstr("txt_content2");
	txt_content2 = (CN3UIString*)grp_maximum->GetChildByID(find);
	find = xorstr("txt_participant");
	txt_participant = (CN3UIString*)grp_maximum->GetChildByID(find);

	// Group Rewards
	find = xorstr("slot_victory");
	slot_victory = (CN3UIArea*)grp_rewards->GetChildByID(find);
	find = xorstr("slot_defeat");
	slot_defeat = (CN3UIArea*)grp_rewards->GetChildByID(find);
	find = xorstr("slot_victory_main");
	slot_victory_main = (CN3UIArea*)grp_rewards->GetChildByID(find);
	find = xorstr("slot_defeat_main");
	slot_defeat_main = (CN3UIArea*)grp_rewards->GetChildByID(find);

	txt_notice->m_bOutline = true;
	txt_time->m_bOutline = true;
	txt_content->m_bOutline = true;
	txt_content2->m_bOutline = true;
	txt_participant->m_bOutline = true;

	grp_maximum->SetVisible(false);
	grp_rewards->SetVisible(false);

	SetPos(Engine->m_UiMgr->GetScreenCenter(this).x, 0);

	SetRegion(grp_minimum->GetRegion());

	return true;
}

void CJoinBoard::Update(Packet pkt)
{
	uint8 subcode;
	pkt >> subcode;
	switch (subcode)
	{
		case TEMPLE_EVENT:
		{
			uint16 activeEvent, rem;
			pkt >> activeEvent >> rem;

			if (activeEvent < TEMPLE_EVENT_BORDER_DEFENCE_WAR || activeEvent > TEMPLE_EVENT_UNDER_THE_CASTLE)
			{
				Release();
				Engine->m_UiMgr->uiJoinBoard = NULL;
				return;
			}

			txt_content->SetString(xorstr("Karus"));
			txt_content2->SetString(xorstr("Elmo"));
			txt_participant->SetString(xorstr("0 : 0"));
			txt_content->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNLEFT);
			txt_content->SetColor(D3DCOLOR_RGBA(59, 134, 255, 255));
			txt_participant->SetColor(D3DCOLOR_RGBA(255, 255, 255, 255));
			txt_content->SetFont("Verdana", txt_participant->GetFontHeight() - 1, false, false);
			txt_content2->SetFont("Verdana", txt_participant->GetFontHeight() - 1, false, false);

			if (activeEvent == TEMPLE_EVENT_BORDER_DEFENCE_WAR)
				txt_notice->SetString(xorstr("BDW"));
			else if (activeEvent == TEMPLE_EVENT_CHAOS) {
				txt_notice->SetString(xorstr("Chaos"));
				txt_content->SetString(xorstr("     Entry:"));
				txt_content2->SetString("");
				txt_participant->SetString(xorstr("0"));
				txt_content->SetColor(D3DCOLOR_RGBA(255, 255, 255, 255));
				txt_participant->SetColor(D3DCOLOR_RGBA(126, 225, 254, 255));
				txt_content->SetFont("Verdana", txt_participant->GetFontHeight(), false, false);
				txt_content2->SetFont("Verdana", txt_participant->GetFontHeight(), false, false);
			}
			else if (activeEvent == TEMPLE_EVENT_JURAD_MOUNTAIN)
				txt_notice->SetString(xorstr("Juraid M."));
			else if (activeEvent == TEMPLE_EVENT_UNDER_THE_CASTLE)
				txt_notice->SetString(xorstr("UTC."));

			m_iRemainingTime = rem;
			m_Timer = new CTimer();
		}
		break;
		case TEMPLE_EVENT_COUNTER:
		{
			uint16 activeEvent, userCount, karusCount, elmoradCount;
			pkt >> activeEvent;
			if (activeEvent == TEMPLE_EVENT_BORDER_DEFENCE_WAR || activeEvent == TEMPLE_EVENT_JURAD_MOUNTAIN) {
				pkt >> karusCount >> elmoradCount;
				txt_participant->SetString(string_format(xorstr("%s : %s"), Engine->StringHelper->NumberFormat(karusCount).c_str(), Engine->StringHelper->NumberFormat(elmoradCount).c_str()));
			}
			else if (activeEvent == TEMPLE_EVENT_CHAOS) {
				pkt >> userCount;
				txt_participant->SetString(string_format(xorstr("%s"), Engine->StringHelper->NumberFormat(userCount).c_str()));
			}
		}
		break;
		case TEMPLE_EVENT_JOIN:
		{
			uint8 bResult;
			int16 activeEvent;
			pkt >> bResult >> activeEvent;
			if (bResult == 1)
			{
				btn_enter->SetVisible(false);
				btn_cancel->SetVisible(true);
			}
			else if (activeEvent == TEMPLE_EVENT_CHAOS && bResult == 3)
				Engine->m_UiMgr->ShowMessageBox("Failed Registration", "You need to have the map of chaos.", Ok);
			else if (bResult == 4)
				Engine->m_UiMgr->ShowMessageBox("Failed Registration", "You cannot attend the event while a pickaxe or fishingrod is equipped.", Ok);
		}
		break;
		case TEMPLE_EVENT_DISBAND:
		{
			uint8 bResult;
			pkt >> bResult;
			if (bResult == 1)
			{
				btn_enter->SetVisible(true);
				btn_cancel->SetVisible(false);
			}
		}
		break;
	}
}

bool CJoinBoard::ReceiveMessage(CN3UIBase* pSender, uint32_t dwMsg)
{
	if (dwMsg == UIMSG_BUTTON_CLICK)
	{
		if (pSender == btn_icon)
			Close();
		else if (pSender == btn_enter)
		{
			Packet result(WIZ_HOOK_GUARD, uint8(WIZ_HOOK_EVENT));
			result << uint8(TEMPLE_EVENT_JOIN);
			Engine->Send(&result);
		}
		else if (pSender == btn_cancel)
		{
			Packet result(WIZ_HOOK_GUARD, uint8(WIZ_HOOK_EVENT));
			result << uint8(TEMPLE_EVENT_DISBAND);
			Engine->Send(&result);
		}
		else if (pSender == btn_maximum)
		{
			if (grp_maximum->IsVisible())
			{
				grp_maximum->SetVisible(false);
				SetRegion(grp_minimum->GetRegion());
			}
			else {
				grp_maximum->SetVisible(true);
				SetRegion(grp_maximum->GetRegion());
			}
		}
		else if (pSender == btn_rewards_open)
		{
			grp_rewards->SetVisible(true);
			SetRegion(grp_rewards->GetRegion());
			btn_rewards_open->SetVisible(false);
			btn_rewards_close->SetVisible(true);
		}
		else if (pSender == btn_rewards_close)
		{
			grp_rewards->SetVisible(false);
			SetRegion(grp_maximum->GetRegion());
			btn_rewards_open->SetVisible(true);
			btn_rewards_close->SetVisible(false);
		}
	}

	return true;
}

uint32_t CJoinBoard::MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld)
{
	uint32_t dwRet = UI_MOUSEPROC_NONE;
	dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);
	return dwRet;
}

bool CJoinBoard::OnKeyPress(int iKey)
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

void CJoinBoard::Open()
{
	SetVisible(true);
}

void CJoinBoard::Close()
{
	SetVisible(false);
}

void CJoinBoard::Tick()
{
	if (!m_Timer || m_iRemainingTime == NULL)
		return;
	
	if (m_Timer->IsElapsedSecond())
		m_iRemainingTime--;

	if (m_iRemainingTime <= 0)
	{
		Release();
		Engine->m_UiMgr->uiJoinBoard = NULL;
		return;
	}

	uint16_t remainingMinutes = (uint16_t)ceil(m_iRemainingTime / 60);
	uint16_t remainingSeconds = m_iRemainingTime - (remainingMinutes * 60);

	std::string remainingTime;
	if (remainingMinutes < 10 && remainingSeconds < 10)
		remainingTime = string_format(xorstr("0%d:0%d"), remainingMinutes, remainingSeconds);
	else if (remainingMinutes < 10)
		remainingTime = string_format(xorstr("0%d:%d"), remainingMinutes, remainingSeconds);
	else if (remainingSeconds < 10)
		remainingTime = string_format(xorstr("%d:0%d"), remainingMinutes, remainingSeconds);
	else remainingTime = string_format(xorstr("%d:%d"), remainingMinutes, remainingSeconds);

	txt_time->SetString(string_format(xorstr("%s"), remainingTime.c_str()));
}