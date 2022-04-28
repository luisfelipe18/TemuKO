#pragma once
class CUIPieceChangePlug;
#include "stdafx.h"
#include "TEMUKOACS Engine.h"
#include "Timer.h"

class CUIPieceChangePlug
{
public:
	DWORD m_dVTableAddr;

	bool m_bAuto;
	DWORD m_btnAuto;
	bool m_bStock;
	DWORD m_btnStock;
	bool m_bSell;
	DWORD m_btnSell;
	DWORD m_btnStart;
	DWORD m_btnStop;
	int m_exCount;
	DWORD m_textExCount;

	DWORD m_slots[28];

	bool m_bAutoExchangeStarted;
	uint16 m_nObjectID;
	uint32 m_nExchangeItemID;

	CTimer* m_Timer;

public:
	CUIPieceChangePlug();
	~CUIPieceChangePlug();
	void ParseUIElements();
	bool ReceiveMessage(DWORD* pSender, uint32_t dwMsg);
	void InitReceiveMessage();
	void ClickStop();
	void Tick();
private:

};