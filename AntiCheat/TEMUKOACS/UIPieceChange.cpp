#include "UIPieceChange.h"

CUIPieceChangePlug::CUIPieceChangePlug()
{
	vector<int>offsets;
	offsets.push_back(0x324);
	offsets.push_back(0);

	m_dVTableAddr = Engine->rdword(KO_DLG, offsets);

	m_bAuto = false;
	m_btnAuto = 0;
	m_bStock = false;
	m_btnStock = 0;
	m_bSell = false;
	m_btnSell = 0;
	m_btnStart = 0;
	m_btnStop = 0;
	m_exCount = 0;
	m_textExCount = 0;

	memset(m_slots, 0, sizeof(m_slots));

	m_bAutoExchangeStarted = false;
	m_nObjectID = 0;
	m_nExchangeItemID = 0;
	m_Timer = NULL;

	ParseUIElements();
	InitReceiveMessage();
}

CUIPieceChangePlug::~CUIPieceChangePlug()
{
}

void CUIPieceChangePlug::ParseUIElements()
{
	std::string find = xorstr("btn_auto");
	Engine->GetChildByID(m_dVTableAddr, find, m_btnAuto);
	find = xorstr("btn_stock");
	Engine->GetChildByID(m_dVTableAddr, find, m_btnStock);
	find = xorstr("btn_sell");
	Engine->GetChildByID(m_dVTableAddr, find, m_btnSell);
	find = xorstr("btn_start");
	Engine->GetChildByID(m_dVTableAddr, find, m_btnStart);
	find = xorstr("btn_stop");
	Engine->GetChildByID(m_dVTableAddr, find, m_btnStop);

	DWORD tmp;
	find = xorstr("edit_excount");
	Engine->GetChildByID(m_dVTableAddr, find, tmp);
	find = xorstr("text_excount");
	Engine->GetChildByID(tmp, find, m_textExCount);
}

DWORD pieceChangeVTable;
bool pieceChangeStopImmediate;
void __stdcall PieceChangeReceiveMessage_Hook(DWORD* pSender, uint32_t dwMsg)
{
	Engine->uiPieceChangePlug->ReceiveMessage(pSender, dwMsg);

	__asm
	{
		MOV ECX, pieceChangeVTable
		PUSH dwMsg
		PUSH pSender
		MOV EAX, KO_UI_PIECE_CHANGE_RECEIVE_MESSAGE_FUNC
		CALL EAX
	}

	if (pieceChangeStopImmediate)
	{
		pieceChangeStopImmediate = false;
		PieceChangeReceiveMessage_Hook((DWORD*)Engine->uiPieceChangePlug->m_btnStop, UIMSG_BUTTON_CLICK);
	}

}

void CUIPieceChangePlug::ClickStop()
{
	PieceChangeReceiveMessage_Hook((DWORD*)Engine->uiPieceChangePlug->m_btnStop, UIMSG_BUTTON_CLICK);
}

bool CUIPieceChangePlug::ReceiveMessage(DWORD* pSender, uint32_t dwMsg)
{
	pieceChangeVTable = m_dVTableAddr;
	if (!pSender || pSender == 0 || dwMsg != UIMSG_BUTTON_CLICK)
		return false;
	
	if (pSender == (DWORD*)m_btnAuto)
	{
		m_bAuto = !m_bAuto;
	}
	else if (pSender == (DWORD*)m_btnStock)
	{
		m_bStock = !m_bStock;
	}
	else if (pSender == (DWORD*)m_btnSell)
	{
		m_bSell = !m_bSell;
	}
	else if (pSender == (DWORD*)m_btnStart)
	{
		if (m_bAuto)
			pieceChangeStopImmediate = true;
		if (m_bAutoExchangeStarted)
		{
			m_bAutoExchangeStarted = false;
			m_Timer = NULL;
		}
	}
	else if (pSender == (DWORD*)m_btnStop)
	{
		m_bAutoExchangeStarted = false;
		m_Timer = NULL;
	}

	return true;
}

void CUIPieceChangePlug::InitReceiveMessage()
{
	*(DWORD*)KO_UI_PIECE_CHANGE_RECEIVE_MESSAGE_PTR = (DWORD)PieceChangeReceiveMessage_Hook;
}

void CUIPieceChangePlug::Tick()
{
	if (m_bAutoExchangeStarted)
	{
		if (Engine->IsVisible(m_dVTableAddr))
		{
			if (m_Timer == NULL)
				m_Timer = new CTimer(true, 500);

			if (m_Timer->IsElapsedSecond()) {

				int curCount = atoi(Engine->GetString(m_textExCount).c_str());
				if (m_exCount >= curCount -1)
				{
					Packet result(WIZ_HOOK_GUARD, uint8(WIZ_HOOK_CHAOTIC_EXCHANGE));
					result << m_nObjectID << m_nExchangeItemID << uint8(200) << uint8(0);
					Engine->Send(&result);

					m_bAutoExchangeStarted = false;
					m_Timer = NULL;
					m_exCount = 0;
					return;
				}
				Engine->SendChaoticExchange(m_nObjectID, m_nExchangeItemID);
				m_exCount++;
			}
		}
		else // ui kapatýldýysa kýrdýrmayý durdur.
		{
			m_bAutoExchangeStarted = false;
			m_Timer = NULL;
			m_exCount = 0;
		}
	}
}