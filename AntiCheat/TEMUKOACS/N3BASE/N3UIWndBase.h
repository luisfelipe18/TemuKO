

#if !defined(AFX_N3UIWNDBASE_H__A30E8AD0_2EB8_4F27_8E0D_3E0979560761__INCLUDED_)
#define AFX_N3UIWNDBASE_H__A30E8AD0_2EB8_4F27_8E0D_3E0979560761__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "N3UIBase.h"
#include "N3UIArea.h"
#include "N3UIIcon.h"
#include "GameDef.h"

#include <string>

enum e_UIWND				{ 
								UIWND_INVENTORY = 0,		
								UIWND_TRANSACTION, 			
								UIWND_DROPITEM,				
								UIWND_PER_TRADE,			
								UIWND_SKILL_TREE,			
								UIWND_HOTKEY,				
								UIWND_PER_TRADE_EDIT,		
								UIWND_EXCHANGE_REPAIR,		
								UIWND_WARE_HOUSE,			
								UIWND_UNKNOWN,				
							};

enum e_UIWND_DISTRICT		{	
								UIWND_DISTRICT_INVENTORY_SLOT = 0,	
								UIWND_DISTRICT_INVENTORY_INV,		
								UIWND_DISTRICT_TRADE_NPC,			
								UIWND_DISTRICT_PER_TRADE_MY,		
								UIWND_DISTRICT_PER_TRADE_OTHER,		
								UIWND_DISTRICT_DROPITEM,			
								UIWND_DISTRICT_SKILL_TREE,			
								UIWND_DISTRICT_SKILL_HOTKEY,		
								UIWND_DISTRICT_EX_RE_NPC,			
								UIWND_DISTRICT_EX_RE_INV,			
								UIWND_DISTRICT_TRADE_MY,			
								UIWND_DISTRICT_PER_TRADE_INV,		
								UIWND_DISTRICT_UNKNOWN,				
							};

enum e_UIIconState			{
								UIICON_SELECTED = 0,				
								UIICON_NOT_SELECTED_BUT_HIGHLIGHT,	
							};

struct __UIWndIconInfo		{
								e_UIWND				UIWnd;
								e_UIWND_DISTRICT	UIWndDistrict;
								int					iOrder;
							};

enum e_UIIconType			{						
								UIICON_TYPE_ITEM = 0,				
								UIICON_TYPE_SKILL,					
							};

struct __InfoSelectedIcon	{
								__UIWndIconInfo		UIWndSelect;
								__IconItemSkill*	pItemSelect;					
								
								__InfoSelectedIcon() { memset(this, 0, sizeof(__InfoSelectedIcon)); }
							};								

struct __RecoveryJobInfo	{ 
								bool				m_bWaitFromServer;
								__IconItemSkill*	pItemSource;					
								__UIWndIconInfo		UIWndSourceStart;
								__UIWndIconInfo		UIWndSourceEnd;
								__IconItemSkill*	pItemTarget;
								__UIWndIconInfo		UIWndTargetStart;
								__UIWndIconInfo		UIWndTargetEnd;
								int					m_iPage;
							};

struct __SkillSelectInfo	{
								e_UIWND				UIWnd;
								int					iOrder;
								__IconItemSkill*	pSkillDoneInfo;
							};

const int UIITEM_TYPE_ONLYONE = 0;
const int UIITEM_TYPE_COUNTABLE = 1;
const int UITEIM_TYPE_GOLD = 2;
const int UIITEM_TYPE_COUNTABLE_SMALL = 3;
const int UIITEM_TYPE_SOMOONE = 4;

const int UIITEM_TYPE_SONGPYUN_ID_MIN = 490000;

const int UIITEM_COUNT_MANY = 9999;
const int UIITEM_COUNT_FEW = 500;

class CUIImageTooltipDlg;
class CCountableItemEditDlg;

class CN3UIWndBase  : public CN3UIBase		
											
{
	void				PlayItemEtcSound();
	void				PlayItemWeaponSound();
	void				PlayItemArmorSound();

public:
	static __InfoSelectedIcon		m_sSelectedIconInfo;
	static __RecoveryJobInfo		m_sRecoveryJobInfo;
	static __SkillSelectInfo		m_sSkillSelectInfo;
	static CN3UIImage*				m_pSelectionImage;
	static CCountableItemEditDlg*	m_pCountableItemEdit;

protected:
	e_UIWND						m_eUIWnd;

	static int					s_iRefCount; 
	static CN3SndObj*			s_pSnd_Item_Etc;
	static CN3SndObj*			s_pSnd_Item_Weapon;
	static CN3SndObj*			s_pSnd_Item_Armor;
	static CN3SndObj*			s_pSnd_Gold;
	static CN3SndObj*			s_pSnd_Repair;

protected:
	virtual void				InitIconWnd(e_UIWND eWnd);
	virtual void				InitIconUpdate() = 0;

public:
	CN3UIWndBase();
	virtual ~CN3UIWndBase();

	e_UIWND						GetUIWnd() { return m_eUIWnd; }

	virtual CN3UIArea*			GetChildAreaByiOrder(eUI_AREA_TYPE eUAT, int iOrder);
	virtual CN3UIString*		GetChildStringByiOrder(int iOrder);

	virtual uint32_t				MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld);
	virtual void				AllHighLightIconFree();

	virtual __IconItemSkill*	GetHighlightIconItem(CN3UIIcon* pUIIcon) = 0;
	virtual void				IconRestore() {}	
	virtual bool				CheckIconDropFromOtherWnd(__IconItemSkill* spItem) { return false; }
	virtual bool				ReceiveIconDrop(__IconItemSkill* spItem, POINT ptCur) { return false; }
	virtual void				CancelIconDrop(__IconItemSkill* spItem) {}
	virtual void				AcceptIconDrop(__IconItemSkill* spItem) {}

	virtual void				PlayItemSound(__TABLE_ITEM_BASIC* pBasic);
	virtual void				PlayGoldSound();
	virtual void				PlayRepairSound();
};

#endif 
