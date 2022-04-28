#pragma once
class CUIMiniMenuPlug;
#include "stdafx.h"
#include "TEMUKOACS Engine.h"

class CUIMiniMenuPlug
{
public:
	DWORD m_dVTableAddr;
	DWORD m_btnViewInfo;

public:
	CUIMiniMenuPlug();
	~CUIMiniMenuPlug();
	void ParseUIElements();
	bool ReceiveMessage(DWORD* pSender, uint32_t dwMsg);
	void InitReceiveMessage();

private:

};