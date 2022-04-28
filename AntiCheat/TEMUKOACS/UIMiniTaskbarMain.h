#pragma once
class CUIMiniTaskbarMainPlug;
#include "stdafx.h"
#include "TEMUKOACS Engine.h"

class CUIMiniTaskbarMainPlug
{
public:
	DWORD m_dVTableAddr;
	DWORD m_btnPowerUP;
public:
	CUIMiniTaskbarMainPlug();
	~CUIMiniTaskbarMainPlug();
	void ParseUIElements();
	bool ReceiveMessage(DWORD* pSender, uint32_t dwMsg);
	void InitReceiveMessage();
private:

};