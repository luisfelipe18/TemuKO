#pragma once
class CUIInventoryPlug;
#include "stdafx.h"
#include "TEMUKOACS Engine.h"

class CUIInventoryPlug
{
public:
	DWORD m_dVTableAddr;
	DWORD slot[24];
public:
	CUIInventoryPlug();
	~CUIInventoryPlug();
	void ParseUIElements();
	bool ReceiveMessage(DWORD* pSender, uint32_t dwMsg);
	void InitReceiveMessage();
private:

};