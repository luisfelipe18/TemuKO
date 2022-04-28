

#if !defined(AFX_N3UIAREA_H__895A2972_7C58_4264_92AA_B740D40B0C22__INCLUDED_)
#define AFX_N3UIAREA_H__895A2972_7C58_4264_92AA_B740D40B0C22__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "N3UIBase.h"

enum eUI_AREA_TYPE	{ 
						UI_AREA_TYPE_NONE = 0, 
						UI_AREA_TYPE_SLOT,
						UI_AREA_TYPE_INV,
						UI_AREA_TYPE_TRADE_NPC,
						UI_AREA_TYPE_PER_TRADE_MY,
						UI_AREA_TYPE_PER_TRADE_OTHER,
						UI_AREA_TYPE_DROP_ITEM,
						UI_AREA_TYPE_SKILL_TREE,
						UI_AREA_TYPE_SKILL_HOTKEY,
						UI_AREA_TYPE_REPAIR_INV,
						UI_AREA_TYPE_REPAIR_NPC,
						UI_AREA_TYPE_TRADE_MY,
						UI_AREA_TYPE_PER_TRADE_INV,
					};

class CN3UIArea : public CN3UIBase  
{
public:
	CN3UIArea();
	virtual ~CN3UIArea();

public:
	eUI_AREA_TYPE	m_eAreaType;

public:
	virtual void	Release();
	virtual bool	Load(HANDLE);
	virtual void	SetRegion(const RECT& Rect);

	virtual uint32_t	MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld);
#ifndef _REPENT
#ifdef _N3GAME
	virtual	bool	ReceiveMessage(CN3UIBase* pSender, uint32_t dwMsg);
#endif
#endif

};

#endif 

