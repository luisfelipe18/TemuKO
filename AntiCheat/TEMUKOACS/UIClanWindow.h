#pragma once
class CUIClanWindowPlug;
#include "stdafx.h"
#include "TEMUKOACS Engine.h"

class CUIClanWindowPlug
{
public:
	DWORD m_dVTableAddr;
	DWORD m_btnClanBank;

public:
	CUIClanWindowPlug();
	~CUIClanWindowPlug();
	void ParseUIElements();
	bool ReceiveMessage(DWORD* pSender, uint32_t dwMsg);
	void InitReceiveMessage();

private:

};