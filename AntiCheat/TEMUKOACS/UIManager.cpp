#include "stdafx.h"
#include "UIManager.h"
#include "PearlGui.h"

CUIManager::CUIManager()
{
	s_bKeyPressed = false;
	s_bKeyPress = false;
	IgnoreMouseProc = false;
	localInput = new CLocalInput();
	uiBase = new CN3UIBase();
	m_FocusedUI = NULL;
	uiCollectionRace = NULL;
	uiCollectionRaceRewards = NULL;
	uiMsgBox = NULL;
	uiDropList = NULL;
	uiDropListGroup = NULL;
	m_pUITooltipDlg = NULL;
	uiTempItemList = NULL;
	uiPowerUpStore = NULL;
	uiGMTools = NULL;
	uiDropResult = NULL;
	uiSupport = NULL;
	uiTopLeft = NULL;
	uiStatPreset = NULL;
	uiSkillPreset = NULL;
	uiJobChange = NULL;
	uiMerchantList = NULL;
	uiLifeSkill = NULL;
	uiSearchMonster = NULL;
	uiJoinBoard = NULL;
	uiVoiceSettings = NULL;
}

CUIManager::~CUIManager()
{
	s_bKeyPressed = false;
	s_bKeyPress = false;
	IgnoreMouseProc = false;
	localInput = NULL;
	uiBase->Release();
	m_FocusedUI = NULL;
	uiCollectionRace = NULL;
	uiCollectionRaceRewards = NULL;
	uiMsgBox = NULL;
	uiDropList = NULL;
	uiDropListGroup = NULL;
	m_pUITooltipDlg = NULL;
	uiTempItemList = NULL;
	uiPowerUpStore = NULL;
	uiGMTools = NULL;
	uiDropResult = NULL;
	uiSupport = NULL;
	uiTopLeft = NULL;
	uiStatPreset = NULL;
	uiSkillPreset = NULL;
	uiJobChange = NULL;
	uiMerchantList = NULL;
	uiLifeSkill = NULL;
	uiSearchMonster = NULL;
	uiJoinBoard = NULL;
	uiVoiceSettings = NULL;
	Release();
}

void CUIManager::Init(LPDIRECT3DDEVICE9 lDevice)
{
	uiBase->s_lpD3DDev = lDevice;
	localInput->Init(GetModuleHandle(NULL), FindWindow(NULL, xorstr("Knight OnLine Client")), FALSE);
	InitUifHookVtables();

	m_pUITooltipDlg = new CUIImageTooltipDlg();
	m_pUITooltipDlg->Init(this);
	m_pUITooltipDlg->LoadFromFile(Engine->m_BasePath + xorstr("TEMUKOACS\\ui\\iteminfo.uif"), N3FORMAT_VER_1068);
	AddChild(m_pUITooltipDlg);
	m_pUITooltipDlg->SetVisible(false);
	SetChildFontsEx(m_pUITooltipDlg, xorstr("Verdana"), 9, FALSE, FALSE);
}

void CUIManager::AddChild(CN3UIBase* pChild, bool focused)
{
	SetChildFonts(pChild, xorstr("Verdana"), 2, FALSE, FALSE);
	m_Children.push_front(pChild);

	if (focused)
		SetFocusedUI(pChild);
}

void CUIManager::RemoveChild(CN3UIBase* pChild)
{
	if (NULL == pChild) return;
	m_DestroyQueue.push_front(pChild);
}

void CUIManager::SetChildFonts(CN3UIBase* pChild, std::string fontName, int minusFontSize, bool bold, bool italic)
{
	if (pChild->UIType() == UI_TYPE_STRING)
	{
		CN3UIString* strChild = (CN3UIString*)pChild;
		strChild->SetFont(fontName, strChild->GetFontHeight() - minusFontSize, bold, italic);
	}
	else if (pChild->UIType() == UI_TYPE_LIST)
	{
		CN3UIList* listChild = (CN3UIList*)pChild;
		listChild->SetFont(fontName, listChild->FontHeight() - minusFontSize, bold, italic);
	}

	UIList myChildren = pChild->GetChildren();
	for (UIListItor itor = myChildren.begin(); myChildren.end() != itor; ++itor)
	{
		CN3UIBase* myChild = (*itor);
		SetChildFonts(myChild, fontName, minusFontSize, bold, italic);
	}
}

void CUIManager::SetChildFontsEx(CN3UIBase* pChild, std::string fontName, int fontSize, bool bold, bool italic)
{
	if (pChild->UIType() == UI_TYPE_STRING)
	{
		CN3UIString* strChild = (CN3UIString*)pChild;
		strChild->SetFont(fontName, fontSize, bold, italic);
	}
	else if (pChild->UIType() == UI_TYPE_LIST)
	{
		CN3UIList* listChild = (CN3UIList*)pChild;
		listChild->SetFont(fontName, fontSize, bold, italic);
	}

	UIList myChildren = pChild->GetChildren();
	for (UIListItor itor = myChildren.begin(); myChildren.end() != itor; ++itor)
	{
		CN3UIBase* myChild = (*itor);
		SetChildFontsEx(myChild, fontName, fontSize, bold, italic);
	}
}

bool isCursorInOurUI = false, m_bDoneSomething = false;
void CUIManager::Tick()
{
	localInput->Tick();
	POINT currentCursorPos = localInput->MouseGetPos();
	int mouseFlag = localInput->MouseGetFlag();
	IsCursorInOurUIs(currentCursorPos);
	ProcessUIKeyInput();

	DWORD dwRet = MouseProc(mouseFlag, currentCursorPos, localInput->MouseGetPosOld());
	m_bDoneSomething = false;
	if (dwRet != UI_MOUSEPROC_NONE)
		m_bDoneSomething = true;
	
	TickDestroyQueue();
	TickFocusedUI();

	for (UIListItor itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		if (!pChild->IsVisible())
			continue;

		pChild->Tick();
	}

	if (m_pUITooltipDlg != NULL && m_pUITooltipDlg->IsVisible())
		m_pUITooltipDlg->Tick();

	if (!isCursorInOurUI)
		Engine->disableCameraZoom = false;
}

uint32_t CUIManager::MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld)
{
	m_dwMouseFlagsCur = UI_MOUSEPROC_NONE;
	if (!m_bVisible || !m_bEnableOperation) return m_dwMouseFlagsCur;

	for (UIListItor itor = m_Children.begin(); m_Children.end() != itor; )
	{
		CN3UIBase* pChild = (*itor);

		if (pChild->m_pChildUI && pChild->m_pChildUI->IsVisible())
		{
			uint32_t dwRet = pChild->m_pChildUI->MouseProc(dwFlags, ptCur, ptOld);
			if (UI_MOUSEPROC_DONESOMETHING & dwRet)
			{
				pChild->MouseProc(0, ptCur, ptOld);
				m_dwMouseFlagsCur |= (UI_MOUSEPROC_DONESOMETHING | UI_MOUSEPROC_CHILDDONESOMETHING);

				SetFocusedUI(pChild);

				return m_dwMouseFlagsCur;
			}
			else if ((UI_MOUSE_LBCLICK & dwFlags) && (UI_MOUSEPROC_INREGION & dwRet))
			{
				pChild->MouseProc(0, ptCur, ptOld);
				m_dwMouseFlagsCur |= (UI_MOUSEPROC_DIALOGFOCUS);

				SetFocusedUI(pChild);

				return m_dwMouseFlagsCur;
			}
		}

		uint32_t dwChildRet = pChild->MouseProc(dwFlags, ptCur, ptOld);
		if (UI_MOUSEPROC_DONESOMETHING & dwChildRet)
		{
			m_dwMouseFlagsCur |= (UI_MOUSEPROC_DONESOMETHING | UI_MOUSEPROC_CHILDDONESOMETHING);

			SetFocusedUI(pChild);

			return m_dwMouseFlagsCur;
		}
		else if ((UI_MOUSE_LBCLICK & dwFlags) && (UI_MOUSEPROC_INREGION & dwChildRet))
		{
			m_dwMouseFlagsCur |= (UI_MOUSEPROC_DIALOGFOCUS);

			SetFocusedUI(pChild);

			return m_dwMouseFlagsCur;
		}
		else ++itor;

		m_dwMouseFlagsCur |= dwChildRet;
	}

	return m_dwMouseFlagsCur;
}

void CUIManager::TickDestroyQueue()
{
	bool isThisFocused = false;
	for (UIListItor ito = m_DestroyQueue.begin(); m_DestroyQueue.end() != ito; ++ito)
	{
		for (UIListItor itor = m_Children.begin(); m_Children.end() != itor;)
		{
			if ((*itor) == (*ito))
			{
				if ((*itor) == m_FocusedUI)
					isThisFocused = true;

				CN3UIBase* pChild = (*itor);
				m_Children.remove(pChild);
				delete pChild;
				break;
			}
			else ++itor;
		}
	}

	m_DestroyQueue.clear();

	if (isThisFocused)
		SetFocusedUI(GetTopUI(true));
}

void CUIManager::TickFocusedUI()
{
	if (m_FocusedUI != NULL && !m_FocusedUI->IsVisible())
		SetFocusedUI(GetTopUI(true));
}

void CUIManager::Render()
{
	if (Engine->Loading)
		return;

	PrepareRenderState();

	for (UIListReverseItor itor = m_Children.rbegin(); m_Children.rend() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		if (!pChild->IsVisible())
			continue;

		pChild->Render();
	}

	if (m_pUITooltipDlg != NULL && m_pUITooltipDlg->IsVisible())
		m_pUITooltipDlg->Render();

	RestoreRenderState();
}

void CUIManager::IsCursorInOurUIs(POINT currentCursorPos)
{
	isCursorInOurUI = false;

	for (UIListItor itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		if (!pChild->IsVisible())
			continue;

		if (pChild->IsIn(currentCursorPos.x, currentCursorPos.y))
		{
			isCursorInOurUI = true;
			break;
		}
	}
}

void CUIManager::SetFocusedUI(CN3UIBase* pUI)
{
	if (NULL == pUI)
	{
		m_FocusedUI = NULL;
		return;
	}

	UIListItor it = m_Children.begin(), itEnd = m_Children.end();
	it = m_Children.begin();
	bool isChild = false;
	for (; it != itEnd; it++)
	{
		if (pUI == *it)
		{
			isChild = true;
			break;
		}
	}

	if (it == itEnd || !isChild)
		return;

	it = m_Children.erase(it);
	m_Children.push_front(pUI);
	ReorderChildList();

	m_FocusedUI = GetTopUI(true);
}

void CUIManager::ReorderChildList()
{
	int iChildCount = m_Children.size();
	if (iChildCount <= 0) return;
	CN3UIBase** ppBuffer = new CN3UIBase * [iChildCount];
	int iAlwaysTopChildCount = 0;

	for (UIListItor itor = m_Children.begin(); m_Children.end() != itor; )
	{
		CN3UIBase* pChild = (*itor);
		if (pChild->GetStyle() & UISTYLE_ALWAYSTOP)
		{
			itor = m_Children.erase(itor);
			ppBuffer[iAlwaysTopChildCount++] = pChild;
		}
		else ++itor;
	}
	int i;
	for (i = iAlwaysTopChildCount - 1; i >= 0; --i)
	{
		m_Children.push_front(ppBuffer[i]);
	}
	delete[] ppBuffer;
}

CN3UIBase* CUIManager::GetTopUI(bool bVisible)
{
	if (!bVisible)
	{
		if (m_Children.empty())
			return NULL;
		return *(m_Children.begin());
	}

	for (UIListItor itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		CN3UIBase* pUI = (*itor);

		if (pUI->IsVisible())
			return pUI;
	}

	return NULL;
}

void CUIManager::HideAllUI()
{
	if (m_FocusedUI != NULL)
	{
		m_FocusedUI->m_bIsThisFocused = true;
		SetFocusedUI(NULL);
	}

	for (UIListItor itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		if (!pChild->IsVisible())
			continue;

		pChild->m_bNeedToRestore = true;
		pChild->SetVisible(false);
	}
}

void CUIManager::ShowAllUI()
{
	for (UIListItor itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);

		if (pChild->m_bNeedToRestore)
		{
			pChild->SetVisible(true);
			pChild->m_bNeedToRestore = false;
		}
	}

	for (UIListItor itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);

		if (pChild->m_bIsThisFocused)
		{
			SetFocusedUI(pChild);
			break;
		}
	}
}

void CUIManager::PrepareRenderState()
{
	uiBase->s_lpD3DDev->GetRenderState(D3DRS_ZENABLE, &dwZEnable);
	uiBase->s_lpD3DDev->GetRenderState(D3DRS_ALPHABLENDENABLE, &dwAlphaBlend);
	uiBase->s_lpD3DDev->GetRenderState(D3DRS_SRCBLEND, &dwSrcBlend);
	uiBase->s_lpD3DDev->GetRenderState(D3DRS_DESTBLEND, &dwDestBlend);
	uiBase->s_lpD3DDev->GetRenderState(D3DRS_FOGENABLE, &dwFog);

	uiBase->s_lpD3DDev->GetSamplerState(0, D3DSAMP_MAGFILTER, &dwMagFilter);
	uiBase->s_lpD3DDev->GetSamplerState(0, D3DSAMP_MINFILTER, &dwMinFilter);
	uiBase->s_lpD3DDev->GetSamplerState(0, D3DSAMP_MIPFILTER, &dwMipFilter);

	if (D3DZB_FALSE != dwZEnable) uiBase->s_lpD3DDev->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	if (TRUE != dwAlphaBlend) uiBase->s_lpD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	if (D3DBLEND_SRCALPHA != dwSrcBlend) uiBase->s_lpD3DDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	if (D3DBLEND_INVSRCALPHA != dwDestBlend) uiBase->s_lpD3DDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	if (FALSE != dwFog) uiBase->s_lpD3DDev->SetRenderState(D3DRS_FOGENABLE, FALSE);
	if (D3DTEXF_POINT != dwMagFilter) uiBase->s_lpD3DDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	if (D3DTEXF_POINT != dwMinFilter) uiBase->s_lpD3DDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	if (D3DTEXF_NONE != dwMipFilter) uiBase->s_lpD3DDev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
}

void CUIManager::RestoreRenderState()
{
	if (D3DZB_FALSE != dwZEnable) uiBase->s_lpD3DDev->SetRenderState(D3DRS_ZENABLE, dwZEnable);
	if (TRUE != dwAlphaBlend) uiBase->s_lpD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, dwAlphaBlend);
	if (D3DBLEND_SRCALPHA != dwSrcBlend) uiBase->s_lpD3DDev->SetRenderState(D3DRS_SRCBLEND, dwSrcBlend);
	if (D3DBLEND_INVSRCALPHA != dwDestBlend) uiBase->s_lpD3DDev->SetRenderState(D3DRS_DESTBLEND, dwDestBlend);
	if (FALSE != dwFog) uiBase->s_lpD3DDev->SetRenderState(D3DRS_FOGENABLE, dwFog);
	if (D3DTEXF_POINT != dwMagFilter) uiBase->s_lpD3DDev->SetSamplerState(0, D3DSAMP_MAGFILTER, dwMagFilter);
	if (D3DTEXF_POINT != dwMinFilter) uiBase->s_lpD3DDev->SetSamplerState(0, D3DSAMP_MINFILTER, dwMinFilter);
	if (D3DTEXF_NONE != dwMipFilter) uiBase->s_lpD3DDev->SetSamplerState(0, D3DSAMP_MIPFILTER, dwMipFilter);
}

DWORD uiMgrVTable;
void CUIManager::InitUifHookVtables()
{
	CalcVtable();

	InitProcessLocalInput();
	InitMouseProc();
	InitUIHideAll();
	InitUIShowAll();

	StaticMemberInit();
}

void CUIManager::CalcVtable()
{
	// patlarsa : 388, 2e4, 0 | 388, 734, 0
	vector<int>offsets;
	offsets.push_back(0x388);
	offsets.push_back(0x2E4);
	offsets.push_back(0);

	m_dVTableAddr = Engine->rdword(KO_DLG, offsets);

	uiMgrVTable = m_dVTableAddr;
}

void CUIManager::StaticMemberInit()
{
	D3DDEVICE_CREATION_PARAMETERS cparams;
	CN3UIBase::s_lpD3DDev->GetCreationParameters(&cparams);
	CN3Base::s_hWndBase = cparams.hFocusWindow;

	RECT rc;
	::GetClientRect(CN3Base::s_hWndBase, &rc);
	RECT rcTmp = rc; rcTmp.left = (rc.right - rc.left) / 2; rcTmp.bottom = rcTmp.top + 30;
	CN3UIEdit::CreateEditWindow(s_hWndBase, rcTmp);
	localInput->CreateMouseWheelWindow(s_hWndBase, rcTmp);
}

void CUIManager::ProcessUIKeyInput()
{
	s_bKeyPressed = false;

	if (m_FocusedUI != NULL && m_FocusedUI && m_FocusedUI->IsVisible())
	{
		for (int i = 0; i < NUMDIKEYS; i++)
		{
			if (localInput->IsKeyPress(i))
			{
				if (m_FocusedUI->m_pChildUI && m_FocusedUI->m_pChildUI->IsVisible())
					s_bKeyPress |= m_FocusedUI->m_pChildUI->OnKeyPress(i);
				else s_bKeyPress |= m_FocusedUI->OnKeyPress(i);
			}

			if (localInput->IsKeyPressed(i))
			{
				if (m_FocusedUI->m_pChildUI && m_FocusedUI->m_pChildUI->IsVisible())
					s_bKeyPressed |= m_FocusedUI->m_pChildUI->OnKeyPressed(i);
				else s_bKeyPressed |= m_FocusedUI->OnKeyPressed(i);
			}
		}
	}

	if (s_bKeyPress)
	{
		for (int i = 0; i < NUMDIKEYS; i++)
		{
			if (localInput->IsKeyPressed(i))
			{
				if (!s_bKeyPressed) s_bKeyPress = false;
				break;
			}
		}
	}
}

RECT CUIManager::GetScreenRect()
{
	D3DDEVICE_CREATION_PARAMETERS cparams;
	RECT rect;
	CN3UIBase::s_lpD3DDev->GetCreationParameters(&cparams);
	GetWindowRect(cparams.hFocusWindow, &rect);

	return rect;
}

POINT CUIManager::GetScreenCenter(CN3UIBase* ui)
{
	D3DDEVICE_CREATION_PARAMETERS cparams;
	RECT rect;
	CN3UIBase::s_lpD3DDev->GetCreationParameters(&cparams);
	GetWindowRect(cparams.hFocusWindow, &rect);

	POINT ret;
	ret.x = (rect.right / 2) - (ui->GetWidth() / 2);
	ret.y = (rect.bottom / 2) - (ui->GetHeight() / 2);

	return ret;
}

const	DWORD	KO_PROCESS_LOCAL_INPUT_CALL_ADDR = 0x00535439;
const	DWORD	KO_PROCESS_LOCAL_INPUT_FUNC = 0x00527800;

void __stdcall ProcessLocalInput_Hook(uint32_t dwMouseFlags)
{
	DWORD thisPtr;
	__asm
	{
		MOV thisPtr, ECX
	}

	CN3UIEdit* focusedEdit = CN3UIEdit::GetFocusedEdit();
	if (focusedEdit != NULL)
	{
		if (!m_bDoneSomething)
			focusedEdit->KillFocus();
		return;
	}

	if (dwMouseFlags != 0x0 && m_bDoneSomething)
		dwMouseFlags = 0x0;

	__asm
	{
		MOV ECX, thisPtr
		PUSH dwMouseFlags
		MOV EAX, KO_PROCESS_LOCAL_INPUT_FUNC
		CALL EAX
	}
}

void CUIManager::InitProcessLocalInput()
{
	Engine->InitCallHook(KO_PROCESS_LOCAL_INPUT_CALL_ADDR, (DWORD)ProcessLocalInput_Hook);
}

const POINT CUIManager::MouseGetPos(const POINT& ptCur)
{
	POINT cur;
	if (isCursorInOurUI)
		cur.x = cur.y = 0;
	else cur = ptCur;

	return cur;
}

void __stdcall MouseProc_Hook(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld)
{
	if ((dwFlags != 0x0 && isCursorInOurUI) 
		|| Engine->m_UiMgr->uiMsgBox != NULL)
		dwFlags = 0x0;

	const POINT& cur = Engine->m_UiMgr->MouseGetPos(ptCur);
	__asm
	{
		MOV ECX, uiMgrVTable
		PUSH ptOld
		PUSH ptCur
		PUSH dwFlags
		MOV EAX, KO_IU_MGR_MOUSE_PROC_FUNC
		CALL EAX
	}
}

void CUIManager::SendMouseProc(uint32 dwFlags, const POINT& ptCur, const POINT& ptOld)
{
	MouseProc_Hook(dwFlags, ptCur, ptOld);
}

void CUIManager::InitMouseProc()
{
	*(DWORD*)KO_IU_MGR_MOUSE_PROC_PTR = (DWORD)MouseProc_Hook;
}

void __stdcall UIHideAll_Hook()
{
	DWORD thisPtr;
	__asm
	{
		MOV thisPtr, ECX
	}

	Engine->m_UiMgr->HideAllUI();

	__asm
	{
		MOV ECX, thisPtr
		MOV EAX, KO_UI_HIDE_ALL_FUNC
		CALL EAX
	}
}

void CUIManager::InitUIHideAll()
{
	Engine->InitCallHook(KO_UI_HIDE_ALL_CALL_ADDR, (DWORD)UIHideAll_Hook);
}

void __stdcall UIShowAll_Hook()
{
	DWORD thisPtr;
	__asm
	{
		MOV thisPtr, ECX
	}

	Engine->m_UiMgr->ShowAllUI();

	__asm
	{
		MOV ECX, thisPtr
		MOV EAX, KO_UI_SHOW_ALL_FUNC
		CALL EAX
	}
}

void CUIManager::InitUIShowAll()
{
	Engine->InitCallHook(KO_UI_SHOW_ALL_CALL_ADDR, (DWORD)UIShowAll_Hook);
}

void CUIManager::ShowMessageBox(string title, string text, MsgBoxTypes type, ParentTypes parent)
{
	if (uiMsgBox == NULL) {
		uiMsgBox = new CUIMessageBox(type, parent);
		uiMsgBox->Init(this);
		uiMsgBox->LoadFromFile(Engine->m_BasePath + xorstr("TEMUKOACS\\ui\\messagebox.uif"));
		AddChild(uiMsgBox);
	} else uiMsgBox->Update(type, parent);
	
	uiMsgBox->SetTitle(title);
	uiMsgBox->SetMessage(text);
	uiMsgBox->SetVisible(true);
	SetFocusedUI(uiMsgBox);
}

void CUIManager::ShowDropList(Packet& pkt)
{
	uint16 mob;
	uint8 subcode;
	pkt >> subcode;
	switch (subcode)
	{
	case 1:
	{
		vector<DropItem> drops;
		uint32 item1, item2, item3, item4, item5, item6;
		uint16 per1, per2, per3, per4, per5, per6;
		pkt >> mob >> item1 >> per1 >> item2 >> per2 >> item3 >> per3 >> item4 >> per4 >> item5 >> per5 >> item6 >> per6;
		drops.push_back(DropItem(item1, per1));
		drops.push_back(DropItem(item2, per2));
		drops.push_back(DropItem(item3, per3));
		drops.push_back(DropItem(item4, per4));
		drops.push_back(DropItem(item5, per5));
		drops.push_back(DropItem(item6, per6));

		if (uiDropList == NULL)
		{
			uiDropList = new CDropList();
			uiDropList->Init(this);
			uiDropList->LoadFromFile(Engine->m_BasePath + xorstr("TEMUKOACS\\ui\\droplist.uif"), N3FORMAT_VER_1068);
			AddChild(uiDropList);
		}
		uiDropList->Update(mob, drops);
		uiDropList->Open();
	}break;
	case 2:
	{
		vector<uint32> items;
		uint8 size;
		pkt >> size;
		pkt.SByte();
		for (int i = 0; i < size; i++)
		{
			uint32 item;
			pkt >> item;
			items.push_back(item);
		}
		if (uiDropListGroup == NULL)
		{
			uiDropListGroup = new CDropListGroup();
			uiDropListGroup->Init(this);
			uiDropListGroup->LoadFromFile(Engine->m_BasePath + xorstr("TEMUKOACS\\ui\\itemgroup.uif"), N3FORMAT_VER_1068);
			AddChild(uiDropListGroup);
		}
		uiDropListGroup->Update(items);
		uiDropListGroup->Open();
	}break;
	default:
		break;
	}
	
}

void CUIManager::ProccessGM(Packet& pkt)
{
	uint8 subCode;
	pkt >> subCode;
	switch (subCode)
	{
	case 1:
	{
		if (uiGMTools == NULL)
		{
			uiGMTools = new CGMTools();
			uiGMTools->Init(this);
			uiGMTools->LoadFromFile(Engine->m_BasePath + xorstr("TEMUKOACS\\ui\\gm.uif"), N3FORMAT_VER_1068);
			AddChild(uiGMTools);
		}
		uiGMTools->Open();
	}
	break;
	}
}

void CUIManager::ShowDropResult()
{
	if (uiDropResult == NULL)
	{
		uiDropResult = new CDropResult();
		uiDropResult->Init(this);
		uiDropResult->LoadFromFile(Engine->m_BasePath + xorstr("TEMUKOACS\\ui\\dropresult.uif"), N3FORMAT_VER_1068);
		AddChild(uiDropResult);
	}
	uiDropResult->Open();
}

void CUIManager::OpenStatPreset()
{
	if (Engine->m_PlayerBase == NULL)
		return;

	if (Engine->m_PlayerBase->GetLevel() < 10)
	{
		ShowMessageBox("Failed", "Your level is too low to use stat preset", Ok);
		return;
	}

	if (uiStatPreset == NULL)
	{
		uiStatPreset = new CStatPreset();
		uiStatPreset->Init(this);
		uiStatPreset->LoadFromFile(Engine->m_BasePath + xorstr("TEMUKOACS\\ui\\re_stat_preset.uif"), N3FORMAT_VER_1068);
		AddChild(uiStatPreset);
	}
	uiStatPreset->Open();
}

void CUIManager::OpenSkillPreset()
{
	if (Engine->m_PlayerBase == NULL)
		return;

	if (Engine->m_PlayerBase->GetLevel() < 10)
	{
		ShowMessageBox("Failed", "Your level is too low to use skill preset", Ok);
		return;
	}

	if (uiSkillPreset == NULL)
	{
		uiSkillPreset = new CSkillPreset();
		uiSkillPreset->Init(this);
		uiSkillPreset->LoadFromFile(Engine->m_BasePath + xorstr("TEMUKOACS\\ui\\re_stat_preset.uif"), N3FORMAT_VER_1068);
		AddChild(uiSkillPreset);
	}
	uiSkillPreset->Open();
}

void CUIManager::OpenJobChange()
{
	if (Engine->m_PlayerBase == NULL)
		return;

	if (uiJobChange == NULL)
	{
		uiJobChange = new CJobChange();
		uiJobChange->Init(this);
		uiJobChange->LoadFromFile(Engine->m_BasePath + xorstr("TEMUKOACS\\ui\\jobchange.uif"), N3FORMAT_VER_1068);
		AddChild(uiJobChange);
	}
	uiJobChange->Open();
}

void CUIManager::ShowMerchantList(Packet& pkt)
{
	if (Engine->m_PlayerBase == NULL)
		return;

	if (uiMerchantList == NULL)
	{
		uiMerchantList = new CMerchantList();
		uiMerchantList->Init(this);
		uiMerchantList->LoadFromFile(Engine->m_BasePath + xorstr("TEMUKOACS\\ui\\merchantlist.uif"), N3FORMAT_VER_1068);
		AddChild(uiMerchantList);
	}

	vector<MerchantData> merchantList;

	uint32 merchantCount;
	pkt >> merchantCount;
	for (int i = 0; i < merchantCount; i++)
	{
		MerchantData data;
		uint32 merchantID;
		uint16 socket;
		string seller;
		uint32 itemID, price;
		uint16 count;
		uint8 isKC;
		uint8 type;
		float x, y, z;
		pkt >> socket >> merchantID >> seller >> type >> itemID >> count >> price >> isKC >> x >> y >> z;
		data.type = type;
		data.socketID = socket;
		data.merchantID = merchantID;
		data.seller = seller;
		data.nItemID = itemID;
		data.price = price;
		data.count = count;
		data.isKC = isKC == 1 ? true : false;
		data.x = x;
		data.y = y;
		data.z = z;
		data.tbl = nullptr;
		merchantList.push_back(data);
	}
	uiMerchantList->merchantList = merchantList;
	uiMerchantList->SetItem(1);
	uiMerchantList->Open();
}

void CUIManager::OpenSearchMonster()
{
	if (uiSearchMonster == NULL)
	{
		uiSearchMonster = new CSearchMonster();
		uiSearchMonster->Init(this);
		uiSearchMonster->LoadFromFile(Engine->m_BasePath + xorstr("TEMUKOACS\\ui\\re_search_monster.uif"), N3FORMAT_VER_1068);
		AddChild(uiSearchMonster);
	}
	uiSearchMonster->Open();
}

void CUIManager::StartJoinBoard()
{
	if (uiJoinBoard == NULL)
	{
		uiJoinBoard = new CJoinBoard();
		uiJoinBoard->Init(this);
		uiJoinBoard->LoadFromFile(Engine->m_BasePath + xorstr("TEMUKOACS\\ui\\re_joinboard.uif"), N3FORMAT_VER_1068);
		AddChild(uiJoinBoard);
	}
	uiJoinBoard->Open();
}

void CUIManager::OpenVoiceSettings()
{
	if (uiVoiceSettings == NULL)
	{
		uiVoiceSettings = new CVoiceSettings();
		uiVoiceSettings->Init(this);
		uiVoiceSettings->LoadFromFile(Engine->m_BasePath + xorstr("TEMUKOACS\\ui\\re_voice_settings.uif"), N3FORMAT_VER_1068);
		AddChild(uiVoiceSettings);
	}
	uiVoiceSettings->Open();
}
