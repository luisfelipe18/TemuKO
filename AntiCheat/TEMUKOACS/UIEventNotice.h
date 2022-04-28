#pragma once
#include "stdafx.h"
class CUIEventNoticePlug;
#include "TEMUKOACS Engine.h"

class CUIEventNoticePlug
{
public:
	DWORD m_dVTableAddr;

public:
	CUIEventNoticePlug();
	~CUIEventNoticePlug();
	POINT GetPos();
};