#pragma once
class CUIMiniMainPlug;
#include "stdafx.h"
#include "HShield Engine.h"

class CUIMiniMainPlug
{
public:
	DWORD m_dVTableAddr;
	DWORD m_btnPowerUP;
public:
	CUIMiniMainPlug();
	~CUIMiniMainPlug();
	void ParseUIElements();
	bool ReceiveMessage(DWORD* pSender, uint32_t dwMsg);
	void InitReceiveMessage();
private:
};