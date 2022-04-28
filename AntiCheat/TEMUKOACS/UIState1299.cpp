#include "UIState1299.h"

CUIStatePlug1299::CUIStatePlug1299()
{
	vector<int>offsets;
	offsets.push_back(0x1BC);
	offsets.push_back(0x124);
	offsets.push_back(0x130);
	offsets.push_back(0xBC);
	offsets.push_back(0);

	m_dVTableAddr = Engine->rdword(KO_DLG, offsets);

	m_txtCash = 0;
	m_btnResetStat = 0;
	m_txtDagger = 0;
	m_txtSword = 0;
	m_txtClub = 0;
	m_txtAxe = 0;
	m_txtSpear = 0;
	m_txtArrow = 0;

	m_iCash = 0;
	m_iDagger = 0;
	m_iSword = 0;
	m_iClub = 0;
	m_iAxe = 0;
	m_iSpear = 0;
	m_iArrow = 0;

	ParseUIElements();
	InitReceiveMessage();
}

CUIStatePlug1299::~CUIStatePlug1299()
{
}

void CUIStatePlug1299::ParseUIElements()
{
	std::string find = xorstr("btn_preset");
	Engine->GetChildByID(m_dVTableAddr, find, m_btnResetStat);
	find = xorstr("Text_Cash");
	Engine->GetChildByID(m_dVTableAddr, find, m_txtCash);
	find = xorstr("Text_DaggerAc");
	Engine->GetChildByID(m_dVTableAddr, find, m_txtDagger);
	find = xorstr("Text_SwordAc");
	Engine->GetChildByID(m_dVTableAddr, find, m_txtSword);
	find = xorstr("Text_MaceAc");
	Engine->GetChildByID(m_dVTableAddr, find, m_txtClub);
	find = xorstr("Text_AxeAc");
	Engine->GetChildByID(m_dVTableAddr, find, m_txtAxe);
	find = xorstr("Text_SpearAc");
	Engine->GetChildByID(m_dVTableAddr, find, m_txtSpear);
	find = xorstr("Text_BowAc");
	Engine->GetChildByID(m_dVTableAddr, find, m_txtArrow);
}

void CUIStatePlug1299::SetAntiDefInfo(Packet& pkt)
{
	short maxhp, maxmp, hp, mp;
	uint8 m_iJamadar = 0;
	pkt >> m_iDagger >> m_iSword >> m_iAxe >> m_iClub >> m_iSpear >> m_iArrow >> m_iJamadar >> Engine->moneyReq >> hp >> mp;;

	Engine->SetString(m_txtDagger, std::to_string(m_iDagger));
	Engine->SetString(m_txtSword, std::to_string(m_iSword));
	Engine->SetString(m_txtAxe, std::to_string(m_iAxe));
	Engine->SetString(m_txtClub, std::to_string(m_iClub));
	Engine->SetString(m_txtSpear, std::to_string(m_iSpear));
	Engine->SetString(m_txtArrow, std::to_string(m_iArrow));

	/*if (Engine->uiHPBarPlug != NULL)
	{
		Engine->uiHPBarPlug->HPChange(hp, maxhp);
		Engine->uiHPBarPlug->MPChange(mp, maxmp);
	}*/
}

void CUIStatePlug1299::UpdateKC(uint32 cash)
{
	Engine->SetString(m_txtCash, Engine->StringHelper->NumberFormat(cash));
}

bool CUIStatePlug1299::IsHaveFreePoints()
{
	return false;
}

void CUIStatePlug1299::UpdateUI()
{
	Engine->SetString(m_txtCash, Engine->StringHelper->NumberFormat(m_iCash));
	Engine->SetString(m_txtDagger, std::to_string(m_iDagger));
	Engine->SetString(m_txtSword, std::to_string(m_iSword));
	Engine->SetString(m_txtAxe, std::to_string(m_iAxe));
	Engine->SetString(m_txtClub, std::to_string(m_iClub));
	Engine->SetString(m_txtSpear, std::to_string(m_iSpear));
	Engine->SetString(m_txtArrow, std::to_string(m_iArrow));
}

DWORD uiState1299VTable;
bool CUIStatePlug1299::ReceiveMessage(DWORD* pSender, uint32_t dwMsg)
{
	uiState1299VTable = m_dVTableAddr;
	if (!pSender || pSender == 0 || dwMsg != UIMSG_BUTTON_CLICK)
		return false;
	
	if (pSender == (DWORD*)m_btnResetStat)
	{
		Engine->m_UiMgr->ShowMessageBox(xorstr("Reset Stat"), xorstr("You sure that you want to reset your stat points? It costs ") + Engine->StringHelper->NumberFormat(Engine->moneyReq) + xorstr(" noahs."), YesNo, PARENT_PAGE_STATE);
	}
	return true;
}

void __stdcall UiState1299ReceiveMessage_Hook(DWORD* pSender, uint32_t dwMsg)
{
	//Engine->uiState1299->ReceiveMessage(pSender, dwMsg);

	__asm
	{
		MOV ECX, uiState1299VTable
		PUSH dwMsg
		PUSH pSender
		MOV EAX, KO_UI_STATE_RECEIVE_MESSAGE_FUNC
		CALL EAX
	}
}

void CUIStatePlug1299::InitReceiveMessage()
{
	*(DWORD*)KO_UI_STATE_RECEIVE_MESSAGE_PTR = (DWORD)UiState1299ReceiveMessage_Hook;
}