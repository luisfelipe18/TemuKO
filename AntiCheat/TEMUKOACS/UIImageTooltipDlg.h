#pragma once
class CUIImageTooltipDlg;
#include "stdafx.h"
#include "N3BASE/N3UIBase.h"
#include "N3BASE/GameDef.h"
#include "TEMUKOACS Engine.h"

#define MAX_TOOLTIP_COUNT 30

class CUIImageTooltipDlg : public CN3UIBase
{
	const D3DCOLOR		m_CYellow;	// ·¹¾î...
	const D3DCOLOR		m_CBlue;	// ¸ÅÁ÷...	
	const D3DCOLOR		m_CGold;	// À¯´ÏÅ©...
	const D3DCOLOR		m_CIvory;	// ¾÷±×·¹ÀÌµå..
	const D3DCOLOR		m_CGreen;	// ¿É¼Ç...	ÀúÇ×·Â, ´É·ÂÄ¡ »ó½Â, ¸¶¹ý¼Ó¼º..
	const D3DCOLOR		m_CWhite;	// ÀÏ¹Ý...
	const D3DCOLOR		m_CRed;		// ¹º°¡ Á¦ÇÑ¿¡ °É¸±¶§..
	const D3DCOLOR		m_CCyan;
	const D3DCOLOR		m_CKrowazRed;
	const D3DCOLOR		m_CPink;
	const D3DCOLOR		m_COrange;

public:
	CN3UIString* m_pStr[MAX_TOOLTIP_COUNT];			// ½ºÆ®·ø.. ^^
	std::string			m_pstdstr[MAX_TOOLTIP_COUNT];
	CN3UIImage* m_pImg;

	int					m_iPosXBack, m_iPosYBack;
	__IconItemSkill* m_spItemBack;
	CN3UIBase* m_Parent;

protected:
	bool				SetTooltipTextColor(int iMyValue, int iTooltipValue);
	bool				SetTooltipTextColor(e_Race eMyValue, e_Race eTooltipValue);
	bool				SetTooltipTextColor(e_Class eMyValue, e_Class eTooltipValue);
	bool				GetTextByItemClass(e_ItemClass eItemClass, std::string& szText);
	bool				GetTextByRace(e_Race eRace, std::string& szText);
	bool				GetTextByClass(e_Class eClass, std::string& szText);
	int					CalcTooltipStringNumAndWrite(__IconItemSkill* spItem, bool bPrice, bool bBuy);
	void				SetPosSomething(int xpos, int ypos, int iNum);

public:
	CUIImageTooltipDlg();
	virtual ~CUIImageTooltipDlg();
	void	Release();
	bool	Load(HANDLE hFile);
	void	Tick();
	void	DisplayTooltipsEnable(int xpos, int ypos, __IconItemSkill* spItem, CN3UIBase* parent, bool tick, bool bPrice = false, bool bBuy = true);
	void	DisplayTooltipsDisable();
};