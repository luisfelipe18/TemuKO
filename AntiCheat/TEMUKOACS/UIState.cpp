#include "UIState.h"

CUIStatePlug::CUIStatePlug()
{
	vector<int>offsets;
	offsets.push_back(0x1BC);
	offsets.push_back(0x124);
	offsets.push_back(0x130);
	offsets.push_back(0xBC);
	offsets.push_back(0);

	m_dVTableAddr = Engine->rdword(KO_DLG, offsets);

	m_txtCash = 0;
	m_txtCashBonus = 0;
	m_btnResetStat = 0;
	m_txtDagger = 0;
	m_txtSword = 0;
	m_txtClub = 0;
	m_txtAxe = 0;
	m_txtSpear = 0;
	m_txtArrow = 0;

	m_iCash = 0;
	m_iCashBonus = 0;
	m_iDagger = 0;
	m_iSword = 0;
	m_iClub = 0;
	m_iAxe = 0;
	m_iSpear = 0;
	m_iArrow = 0;
	m_iJamadar = 0;

	ParseUIElements();
	InitReceiveMessage();
}

CUIStatePlug::~CUIStatePlug()
{
}

void CUIStatePlug::ParseUIElements()
{
	std::string find = xorstr("btn_preset");
	Engine->GetChildByID(m_dVTableAddr, find, m_btnResetStat);
	find = xorstr("Text_Cash");
	Engine->GetChildByID(m_dVTableAddr, find, m_txtCash);
	find = xorstr("Text_CashBonus");
	Engine->GetChildByID(m_dVTableAddr, find, m_txtCashBonus);
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
	find = xorstr("Text_JamadarAc");
	Engine->GetChildByID(m_dVTableAddr, find, m_txtJamadar);
}

void CUIStatePlug::SetAntiDefInfo(Packet& pkt)
{
	pkt >> m_iDagger >> m_iSword >> m_iAxe >> m_iClub >> m_iSpear >> m_iArrow >> m_iJamadar >> Engine->moneyReq;

	Engine->SetString(m_txtDagger, std::to_string(m_iDagger));
	Engine->SetString(m_txtSword, std::to_string(m_iSword));
	Engine->SetString(m_txtAxe, std::to_string(m_iAxe));
	Engine->SetString(m_txtClub, std::to_string(m_iClub));
	Engine->SetString(m_txtSpear, std::to_string(m_iSpear));
	Engine->SetString(m_txtArrow, std::to_string(m_iArrow));
	Engine->SetString(m_txtJamadar, std::to_string(m_iJamadar));
}

void CUIStatePlug::SetAntiDefInfoItemMove(Packet& pkt)
{
	pkt >> m_iDagger >> m_iSword >> m_iAxe >> m_iClub >> m_iSpear >> m_iArrow >> m_iJamadar >> Engine->moneyReq;

	Engine->SetString(m_txtDagger, std::to_string(m_iDagger));
	Engine->SetString(m_txtSword, std::to_string(m_iSword));
	Engine->SetString(m_txtAxe, std::to_string(m_iAxe));
	Engine->SetString(m_txtClub, std::to_string(m_iClub));
	Engine->SetString(m_txtSpear, std::to_string(m_iSpear));
	Engine->SetString(m_txtArrow, std::to_string(m_iArrow));
	Engine->SetString(m_txtJamadar, std::to_string(m_iJamadar));
}
void CUIStatePlug::UpdateKC(uint32 cash, uint32 bonuscash)
{
	Engine->SetString(m_txtCash, Engine->StringHelper->NumberFormat(cash));
	Engine->SetString(m_txtCashBonus, Engine->StringHelper->NumberFormat(bonuscash));
}

bool CUIStatePlug::IsHaveFreePoints(){
	return false;
}

void CUIStatePlug::UpdateUI()
{
	Engine->SetString(m_txtCash, Engine->StringHelper->NumberFormat(m_iCash));
	Engine->SetString(m_txtCashBonus, Engine->StringHelper->NumberFormat(m_iCashBonus));
	Engine->SetString(m_txtDagger, std::to_string(m_iDagger));
	Engine->SetString(m_txtSword, std::to_string(m_iSword));
	Engine->SetString(m_txtAxe, std::to_string(m_iAxe));
	Engine->SetString(m_txtClub, std::to_string(m_iClub));
	Engine->SetString(m_txtSpear, std::to_string(m_iSpear));
	Engine->SetString(m_txtArrow, std::to_string(m_iArrow));
	Engine->SetString(m_txtJamadar, std::to_string(m_iJamadar));
}

DWORD uiStateVTable;
bool CUIStatePlug::ReceiveMessage(DWORD* pSender, uint32_t dwMsg)
{
	uiStateVTable = m_dVTableAddr;
	if (!pSender || pSender == 0 || dwMsg != UIMSG_BUTTON_CLICK)
		return false;
	
	if (pSender == (DWORD*)m_btnResetStat)
	{
		if (Engine->EngineSettings->State_StatReset != 0)
			Engine->m_UiMgr->OpenStatPreset();
		else
			Engine->m_UiMgr->ShowMessageBox("Failed", "Stat Preset feature is currently disabled.");
	}
	return true;
}

void __stdcall UiStateReceiveMessage_Hook(DWORD* pSender, uint32_t dwMsg)
{
	Engine->uiState->ReceiveMessage(pSender, dwMsg);

	__asm
	{
		MOV ECX, uiStateVTable
		PUSH dwMsg
		PUSH pSender
		MOV EAX, KO_UI_STATE_RECEIVE_MESSAGE_FUNC
		CALL EAX
	}
}

void CUIStatePlug::InitReceiveMessage()
{
	*(DWORD*)KO_UI_STATE_RECEIVE_MESSAGE_PTR = (DWORD)UiStateReceiveMessage_Hook;
}