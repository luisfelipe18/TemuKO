// N3UIWndBase.cpp: implementation of the CN3UIWndBase class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
#include "../TEMUKOACS Engine.h"
#include "N3UIWndBase.h"
#include "N3UITooltip.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

__InfoSelectedIcon		CN3UIWndBase::m_sSelectedIconInfo;
__RecoveryJobInfo		CN3UIWndBase::m_sRecoveryJobInfo;
__SkillSelectInfo		CN3UIWndBase::m_sSkillSelectInfo;

CN3UIImage* CN3UIWndBase::m_pSelectionImage = NULL;
CCountableItemEditDlg* CN3UIWndBase::m_pCountableItemEdit = NULL;

CN3SndObj* CN3UIWndBase::s_pSnd_Item_Etc = NULL;
CN3SndObj* CN3UIWndBase::s_pSnd_Item_Weapon = NULL;
CN3SndObj* CN3UIWndBase::s_pSnd_Item_Armor = NULL;
CN3SndObj* CN3UIWndBase::s_pSnd_Gold = NULL;
CN3SndObj* CN3UIWndBase::s_pSnd_Repair = NULL;
int CN3UIWndBase::s_iRefCount = 0;

CN3UIWndBase::CN3UIWndBase()
{
	m_pSelectionImage = NULL;
	m_pSelectionImage = new CN3UIImage;
	m_pSelectionImage->Init(Engine->m_UiMgr);
	m_pSelectionImage->SetUVRect(0.0f, 0.0f, 1.0f, 1.0f);
	s_iRefCount++; // ÂüÁ¶ Ä«¿îÆ®
}

CN3UIWndBase::~CN3UIWndBase()
{

}

void CN3UIWndBase::InitIconWnd(e_UIWND eWnd)
{
	m_eUIWnd = eWnd;
}

CN3UIArea* CN3UIWndBase::GetChildAreaByiOrder(eUI_AREA_TYPE eUAT, int iOrder)
{
	char pszID[32];
	sprintf(pszID, "%d", iOrder);

	for (UIListItor itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		CN3UIArea* pChild = (CN3UIArea*)(*itor);
		if ((pChild->UIType() == UI_TYPE_AREA) && (pChild->m_eAreaType == eUAT))
		{
			if (pChild->m_szID == pszID) return pChild;
		}
	}

	return NULL;
}

CN3UIString* CN3UIWndBase::GetChildStringByiOrder(int iOrder)
{
	char pszID[32];
	sprintf(pszID, "%d", iOrder);

	for (UIListItor itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		CN3UIString* pChild = (CN3UIString*)(*itor);
		if (pChild->UIType() == UI_TYPE_STRING)
		{
			if (pChild->m_szID == pszID) return pChild;
		}
	}

	return NULL;

}

void CN3UIWndBase::AllHighLightIconFree()
{
	for (UIListReverseItor itor = m_Children.rbegin(); m_Children.rend() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		if (pChild->UIType() == UI_TYPE_ICON)
			pChild->SetStyle(pChild->GetStyle() & (~UISTYLE_ICON_HIGHLIGHT));
	}
}

uint32_t CN3UIWndBase::MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld)
{
	uint32_t dwRet = UI_MOUSEPROC_NONE;
	if (!m_bVisible) return dwRet;

	// ¾ÆÀÌÄÜÀ» °¡Áø À©µµ¿ì´Â ÀÚ±â À©µµ¿ì ¿µ¿ªÀ» ¹þ¾î ³µÀ»¶§µµ ÀÚ½ÄÀÇ ¸Þ½ÃÁö¸¦ ¹ÞÀ»¼ö ÀÖ¾î¾ß ÇÑ´Ù.. ^^

	// UI ¿òÁ÷ÀÌ´Â ÄÚµå
	if (UI_STATE_COMMON_MOVE == m_eState)
	{
		if (dwFlags & UI_MOUSE_LBCLICKED)
		{
			SetState(UI_STATE_COMMON_NONE);
		}
		else
		{
			MoveOffset(ptCur.x - ptOld.x, ptCur.y - ptOld.y);
		}
		dwRet |= UI_MOUSEPROC_DONESOMETHING;
		return dwRet;
	}

	if (false == IsIn(ptCur.x, ptCur.y))	// ¿µ¿ª ¹ÛÀÌ¸é
	{
		if (false == IsIn(ptOld.x, ptOld.y))
		{
			if (GetState() != UI_STATE_ICON_MOVING)
				return dwRet;// ÀÌÀü ÁÂÇ¥µµ ¿µ¿ª ¹ÛÀÌ¸é 
		}
		dwRet |= UI_MOUSEPROC_PREVINREGION;	// ÀÌÀü ÁÂÇ¥´Â ¿µ¿ª ¾ÈÀÌ¾ú´Ù.
	}
	else
	{
		// tool tip °ü·Ã
		if (s_pTooltipCtrl) s_pTooltipCtrl->SetText(m_szToolTip);
	}
	dwRet |= UI_MOUSEPROC_INREGION;	// ÀÌ¹ø ÁÂÇ¥´Â ¿µ¿ª ¾ÈÀÌ´Ù.

	// child¿¡°Ô ¸Þ¼¼Áö Àü´Þ
	for (UIListItor itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		uint32_t dwChildRet = pChild->MouseProc(dwFlags, ptCur, ptOld);
		if (UI_MOUSEPROC_DONESOMETHING & dwChildRet)
		{
			// ÀÌ°æ¿ì¿¡´Â ¸Õ°¡ Æ÷Ä¿½º¸¦ ¹ÞÀº °æ¿ìÀÌ´Ù.
			dwRet |= (UI_MOUSEPROC_CHILDDONESOMETHING | UI_MOUSEPROC_DONESOMETHING);
			return dwRet;
		}
	}

	// UI ¿òÁ÷ÀÌ´Â ÄÚµå
	if (UI_STATE_COMMON_MOVE != m_eState &&
		PtInRect(&m_rcMovable, ptCur) && (dwFlags & UI_MOUSE_LBCLICK))
	{
		// ÀÎº¥Åä¸® À©µµ¿ìÀÌ°í »ó°Å·¡ ÁßÀÌ¸é..
		if ((UIType() == UI_TYPE_ICON_MANAGER) && (m_eUIWnd == UIWND_INVENTORY))
			return dwRet;
		SetState(UI_STATE_COMMON_MOVE);
		dwRet |= UI_MOUSEPROC_DONESOMETHING;
		return dwRet;
	}

	return dwRet;
}

void CN3UIWndBase::PlayItemEtcSound()
{
	//if (s_pSnd_Item_Etc)	s_pSnd_Item_Etc->Play();
}

void CN3UIWndBase::PlayItemWeaponSound()
{
	//if (s_pSnd_Item_Weapon)	s_pSnd_Item_Weapon->Play();
}

void CN3UIWndBase::PlayItemArmorSound()
{
	//if (s_pSnd_Item_Armor)	s_pSnd_Item_Armor->Play();
}

void CN3UIWndBase::PlayGoldSound()
{
	//if (s_pSnd_Gold)	s_pSnd_Gold->Play();
}

void CN3UIWndBase::PlayRepairSound()
{//
	//if (s_pSnd_Repair)	s_pSnd_Repair->Play();
}

void CN3UIWndBase::PlayItemSound(__TABLE_ITEM_BASIC* pBasic)
{
	/*if (!pBasic) return;
	switch (pBasic->dwID / 100000000)
	{
	case 1:
		PlayItemWeaponSound();
		break;

	case 2:
		PlayItemArmorSound();
		break;

	default:
		PlayItemEtcSound();
		break;
	}*/
}