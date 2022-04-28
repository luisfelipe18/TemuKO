#pragma once
class CUIManager;
#include "stdafx.h"
#include "TEMUKOACS Engine.h"
#include "N3BASE/N3Base.h"
#include "N3BASE/N3BaseFileAccess.h"
#include "N3BASE/N3UIBase.h"
#include "N3BASE/N3UIButton.h"
#include "N3BASE/N3UIString.h"
#include "N3BASE/N3UIList.h"
#include "N3BASE/N3UIEdit.h"
#include "N3BASE/N3UIProgress.h"
#include "N3BASE/N3UIImage.h"
#include "UIImageTooltipDlg.h"
#include "DropList.h"
#include "DropListGroup.h"
#include "N3BASE/LogWriter.h"
#include "LocalInput.h"
#include "CREvent.h"
#include "CRRewards.h"
#include "MessageBoxUIF.h"
#include "TempItemList.h"
#include "PowerUpStore.h"
#include "GMTools.h"
#include "DropResult.h"
#include "CSupport.h"
#include "CTopLeft.h"
#include "StatPreset.h"
#include "SkillPreset.h"
#include "JobChange.h"
#include "MerchantList.h"
#include "CLifeSkill.h"
#include "SearchMonster.h"
#include "JoinBoard.h"
#include "CVoiceSettings.h"

typedef std::list<CN3UIBase*>			UIList;
typedef UIList::iterator				UIListItor;
typedef UIList::const_iterator			UIListItorConst;
typedef UIList::reverse_iterator		UIListReverseItor;

class CUIManager : public CN3UIBase
{
public:
	DWORD m_dVTableAddr;
	CN3UIBase* uiBase;
	CLocalInput* localInput;
	CN3UIBase* m_FocusedUI;

	CUIImageTooltipDlg* m_pUITooltipDlg;

	CUIMessageBox* uiMsgBox;
	CCollectionRace* uiCollectionRace;
	CCollectionRaceRewards* uiCollectionRaceRewards;
	CDropList* uiDropList;
	CDropListGroup* uiDropListGroup;
	CTempItemList* uiTempItemList;
	CPowerUpStore* uiPowerUpStore;
	CGMTools* uiGMTools;
	CDropResult* uiDropResult;
	CSupport* uiSupport;
	CTopLeft* uiTopLeft;
	CStatPreset* uiStatPreset;
	CSkillPreset* uiSkillPreset;
	CJobChange* uiJobChange;
	CMerchantList* uiMerchantList;
	CLifeSkill* uiLifeSkill;
	CSearchMonster* uiSearchMonster;
	CJoinBoard* uiJoinBoard;
	CVoiceSettings* uiVoiceSettings;

	vector<PUSItem> item_list;

	UIList		m_Children;
	UIList		m_DestroyQueue;

	bool s_bKeyPressed;
	bool s_bKeyPress;
	uint32_t m_dwMouseFlagsCur;
	bool m_bEnableOperation;

public:
	CUIManager();
	~CUIManager();
	void Init(LPDIRECT3DDEVICE9 lDevice);
	void AddChild(CN3UIBase* pChild, bool focused = false);
	void RemoveChild(CN3UIBase* pChild);
	void SetChildFonts(CN3UIBase* pChild, std::string fontName, int minusFontSize, bool bold, bool italic);
	void SetChildFontsEx(CN3UIBase* pChild, std::string fontName, int fontSize, bool bold, bool italic);
	void Tick();
	uint32_t MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld);
	void TickDestroyQueue();
	void TickFocusedUI();
	void Render();
	void IsCursorInOurUIs(POINT currentCursorPos);
	void SetFocusedUI(CN3UIBase* pUI);
	void ReorderChildList();
	CN3UIBase* GetTopUI(bool bVisible);
	void HideAllUI();
	void ShowAllUI();
	void PrepareRenderState();
	void RestoreRenderState();
	void InitUifHookVtables();
	void CalcVtable();
	void StaticMemberInit();
	void ProcessUIKeyInput();
	RECT GetScreenRect();
	POINT GetScreenCenter(CN3UIBase* ui);
	void InitProcessLocalInput();
	const POINT MouseGetPos(const POINT& ptCur);
	void SendMouseProc(uint32 dwFlags, const POINT& ptCur, const POINT& ptOld);
	void InitMouseProc();
	void InitUIHideAll();
	void InitUIShowAll();
	void ShowMessageBox(string title, string text, MsgBoxTypes type = MsgBoxTypes::YesNo, ParentTypes parent = ParentTypes::PARENT_NONE);
	void ShowDropList(Packet& pkt);
	void ProccessGM(Packet& pkt);
	void ShowMerchantList(Packet& pkt);
	void ShowDropResult();
	void OpenStatPreset();
	void OpenSkillPreset();
	void OpenJobChange();
	void OpenSearchMonster();
	void StartJoinBoard();
	void OpenVoiceSettings();
	bool IgnoreMouseProc;
private:
	DWORD dwZEnable, dwAlphaBlend, dwSrcBlend, dwDestBlend, dwFog;
	DWORD dwMagFilter, dwMinFilter, dwMipFilter;
};