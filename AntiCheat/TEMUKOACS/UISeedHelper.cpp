#include "stdafx.h"
#include "UISeedHelper.h"

CUISeedHelperPlug::CUISeedHelperPlug()
{
	vector<int>offsets;
	offsets.push_back(0x338);
	offsets.push_back(0);

	m_dVTableAddr = Engine->rdword(KO_DLG, offsets);

	m_btnVisibleGenie = 0;
	m_CameraZoom = 0;

	m_btnSaveSettings = 0;

	m_btnWeapon = 0;
	m_btnArmor = 0;
	m_btnAccessory = 0;

	m_btnNormal = 0;
	m_btnUpgrade = 0;
	m_btnUnique = 0;
	m_btnRare = 0;
	m_btnCraft = 0;
	m_btnConsumable = 0;
	m_btnMagic = 0;

	m_editPrice = 0;
	m_txtPrice = 0;

	m_btnPriceSave = 0;

	m_bNeedToSave = false;
	m_bNeedToSetGame = true;
	m_bNeedToSetLoot = true;
	m_bNeedToSetEffect = true;

	m_dGroupOption = 0;
	m_dGroupGame = 0;
	m_dGroupLoot = 0;

	ParseUIElements();
	InitReceiveMessage();
	InitSetVisible();
}

CUISeedHelperPlug::~CUISeedHelperPlug()
{
}

void CUISeedHelperPlug::ParseUIElements()
{
	std::string find = xorstr("group_option");
	Engine->GetChildByID(m_dVTableAddr, find, m_dGroupOption); __ASSERT(m_dGroupOption, "NULL Pointer");

	find = xorstr("group_option_game");
	Engine->GetChildByID(m_dGroupOption, find, m_dGroupGame); __ASSERT(m_dGroupGame, "NULL Pointer");

	find = xorstr("btn_state_genie");
	Engine->GetChildByID(m_dGroupGame, find, m_btnVisibleGenie); __ASSERT(m_btnVisibleGenie, "NULL Pointer");

	find = xorstr("group_option_effect");
	Engine->GetChildByID(m_dGroupOption, find, m_dGroupEffect); __ASSERT(m_dGroupEffect, "NULL Pointer");

	find = xorstr("scroll_camera");
	Engine->GetChildByID(m_dGroupEffect, find, m_CameraZoom); __ASSERT(m_CameraZoom, "NULL Pointer");

	find = xorstr("group_option_Looting");
	Engine->GetChildByID(m_dGroupOption, find, m_dGroupLoot); __ASSERT(m_dGroupLoot, "NULL Pointer");

	find = xorstr("btn_weapon");
	Engine->GetChildByID(m_dGroupLoot, find, m_btnWeapon); __ASSERT(m_btnWeapon, "NULL Pointer");
	find = xorstr("btn_armor");
	Engine->GetChildByID(m_dGroupLoot, find, m_btnArmor); __ASSERT(m_btnArmor, "NULL Pointer");
	find = xorstr("btn_accessory");
	Engine->GetChildByID(m_dGroupLoot, find, m_btnAccessory); __ASSERT(m_btnAccessory, "NULL Pointer");

	find = xorstr("btn_normal");
	Engine->GetChildByID(m_dGroupLoot, find, m_btnNormal); __ASSERT(m_btnNormal, "NULL Pointer");
	find = xorstr("btn_upgrade");
	Engine->GetChildByID(m_dGroupLoot, find, m_btnUpgrade); __ASSERT(m_btnUpgrade, "NULL Pointer");
	find = xorstr("btn_unique");
	Engine->GetChildByID(m_dGroupLoot, find, m_btnUnique); __ASSERT(m_btnUnique, "NULL Pointer");
	find = xorstr("btn_rare");
	Engine->GetChildByID(m_dGroupLoot, find, m_btnRare); __ASSERT(m_btnRare, "NULL Pointer");
	find = xorstr("btn_craft");
	Engine->GetChildByID(m_dGroupLoot, find, m_btnCraft); __ASSERT(m_btnCraft, "NULL Pointer");
	find = xorstr("btn_pet_count_loot");
	Engine->GetChildByID(m_dGroupLoot, find, m_btnConsumable); __ASSERT(m_btnConsumable, "NULL Pointer");
	find = xorstr("btn_magic");
	Engine->GetChildByID(m_dGroupLoot, find, m_btnMagic); __ASSERT(m_btnMagic, "NULL Pointer");

	find = xorstr("edit_pet_loot_money");
	Engine->GetChildByID(m_dGroupLoot, find, m_editPrice); __ASSERT(m_editPrice, "NULL Pointer");

	/*find = xorstr("text_pet_loot_money");
	Engine->GetChildByID(m_editPrice, find, m_txtPrice);*/

	find = xorstr("btn_pet_loot_money");
	Engine->GetChildByID(m_dGroupLoot, find, m_btnPriceSave); __ASSERT(m_btnPriceSave, "NULL Pointer");

	find = xorstr("btn_save");
	Engine->GetChildByID(m_dGroupOption, find, m_btnSaveSettings); __ASSERT(m_btnSaveSettings, "NULL Pointer");

	ApplyGameSettings();
	ApplySettings();
}

DWORD uiSeedHelperVTable;
void __stdcall UISeedHelperReceiveMessage_Hook(DWORD* pSender, uint32_t dwMsg)
{
	Engine->uiSeedHelperPlug->ReceiveMessage(pSender, dwMsg);

	__asm
	{
		MOV ECX, uiSeedHelperVTable
		PUSH dwMsg
		PUSH pSender
		MOV EAX, KO_UI_SEED_HELPER_RECEIVE_MESSAGE_FUNC
		CALL EAX
	}
}

bool CUISeedHelperPlug::ReceiveMessage(DWORD* pSender, uint32_t dwMsg)
{
	uiSeedHelperVTable = m_dVTableAddr;
	if (!pSender || pSender == 0 || dwMsg != UIMSG_BUTTON_CLICK)
		return false;
	
	if (m_btnVisibleGenie != NULL && pSender == (DWORD*)m_btnVisibleGenie)
	{
		m_bNeedToSave = true;
		Engine->m_SettingsMgr->m_iVisibleGenie = Engine->m_SettingsMgr->m_iVisibleGenie == 1 ? 0 : 1;
	}
	else if (m_btnWeapon != NULL && pSender == (DWORD*)m_btnWeapon)
	{
		m_bNeedToSave = true;
		Engine->m_SettingsMgr->m_iWeapon = Engine->m_SettingsMgr->m_iWeapon == 1 ? 0 : 1;
	}
	else if (m_btnArmor != NULL && pSender == (DWORD*)m_btnArmor)
	{
		m_bNeedToSave = true;
		Engine->m_SettingsMgr->m_iArmor = Engine->m_SettingsMgr->m_iArmor == 1 ? 0 : 1;
	}
	else if (m_btnAccessory != NULL && pSender == (DWORD*)m_btnAccessory)
	{
		m_bNeedToSave = true;
		Engine->m_SettingsMgr->m_iAccessory = Engine->m_SettingsMgr->m_iAccessory == 1 ? 0 : 1;
	}
	else if (m_btnNormal != NULL && pSender == (DWORD*)m_btnNormal)
	{
		m_bNeedToSave = true;
		Engine->m_SettingsMgr->m_iNormal = Engine->m_SettingsMgr->m_iNormal == 1 ? 0 : 1;
	}
	else if (m_btnUpgrade != NULL && pSender == (DWORD*)m_btnUpgrade)
	{
		m_bNeedToSave = true;
		Engine->m_SettingsMgr->m_iUpgrade = Engine->m_SettingsMgr->m_iUpgrade == 1 ? 0 : 1;
	}
	else if (m_btnUnique != NULL && pSender == (DWORD*)m_btnUnique)
	{
		m_bNeedToSave = true;
		Engine->m_SettingsMgr->m_iUnique = Engine->m_SettingsMgr->m_iUnique == 1 ? 0 : 1;
	}
	else if (m_btnRare != NULL && pSender == (DWORD*)m_btnRare)
	{
		m_bNeedToSave = true;
		Engine->m_SettingsMgr->m_iRare = Engine->m_SettingsMgr->m_iRare == 1 ? 0 : 1;
	}
	else if (m_btnCraft != NULL && pSender == (DWORD*)m_btnCraft)
	{
		m_bNeedToSave = true;
		Engine->m_SettingsMgr->m_iCraft = Engine->m_SettingsMgr->m_iCraft == 1 ? 0 : 1;
	}
	else if (m_btnConsumable != NULL && pSender == (DWORD*)m_btnConsumable)
	{
		m_bNeedToSave = true;
		Engine->m_SettingsMgr->m_iConsumable = Engine->m_SettingsMgr->m_iConsumable == 1 ? 0 : 1;
	}
	else if (m_btnMagic != NULL && pSender == (DWORD*)m_btnMagic)
	{
		m_bNeedToSave = true;
		Engine->m_SettingsMgr->m_iMagic = Engine->m_SettingsMgr->m_iMagic == 1 ? 0 : 1;
	}
	else if (m_btnSaveSettings != NULL && pSender == (DWORD*)m_btnSaveSettings)
	{
		/*std::string inputText = Engine->GetString(m_txtPrice);
		int iPrice = 0;
		try { iPrice = std::stoi(inputText); }
		catch (std::invalid_argument const& e) { iPrice = 0; }
		catch (std::out_of_range const& e) { iPrice = 999999999; }

		if (iPrice != Engine->m_SettingsMgr->m_iPrice)
		{
			m_bNeedToSave = true;
			Engine->m_SettingsMgr->m_iPrice = iPrice;
		}*/

		if (GetCameraRange() != Engine->m_SettingsMgr->m_iCameraRange) {
			Engine->m_SettingsMgr->m_iCameraRange = GetCameraRange();
			m_bNeedToSave = true;
		}

		if (m_bNeedToSave)
		{
			if(m_btnPriceSave != NULL)
				UISeedHelperReceiveMessage_Hook((DWORD*)m_btnPriceSave, UIMSG_BUTTON_CLICK);

			ApplySettings();
			Engine->m_SettingsMgr->Save();
	
			m_bNeedToSave = false;
		}
	}

	return true;
}

void CUISeedHelperPlug::InitReceiveMessage()
{
	*(DWORD*)KO_UI_SEED_HELPER_RECEIVE_MESSAGE_PTR = (DWORD)UISeedHelperReceiveMessage_Hook;
}

void CUISeedHelperPlug::Tick()
{
	if (m_dGroupLoot == 0 
		|| m_dGroupGame == 0 
		|| m_dGroupEffect == 0)
		return;

	if (Engine->IsVisible(m_dGroupLoot))
		ApplyLootSettings();
	else 
		m_bNeedToSetLoot = true;

	if (Engine->IsVisible(m_dGroupGame))
		ApplyGameSettings();
	else
		m_bNeedToSetGame = true;

	if (Engine->IsVisible(m_dGroupEffect))
		ApplyEffectSettings();
	else
		m_bNeedToSetEffect = true;
}

void CUISeedHelperPlug::ApplyEffectSettings()
{
	if (!m_bNeedToSetEffect)
		return;

	m_bNeedToSetEffect = false;
	SetCameraRange(Engine->m_SettingsMgr->m_iCameraRange);
}

void CUISeedHelperPlug::ApplyGameSettings()
{
	if (!m_bNeedToSetGame)
		return;

	SetGenieSetting();

	Engine->SetState(m_btnVisibleGenie, Engine->m_SettingsMgr->m_iVisibleGenie == 1 ? UI_STATE_BUTTON_DOWN : UI_STATE_BUTTON_NORMAL);

	m_bNeedToSetGame = false;
}

void CUISeedHelperPlug::ApplyLootSettings()
{
	if (!m_bNeedToSetLoot)
		return;

	Engine->SetState(m_btnArmor, Engine->m_SettingsMgr->m_iArmor == 1 ? UI_STATE_BUTTON_DOWN : UI_STATE_BUTTON_NORMAL);
	Engine->SetState(m_btnWeapon, Engine->m_SettingsMgr->m_iWeapon == 1 ? UI_STATE_BUTTON_DOWN : UI_STATE_BUTTON_NORMAL);
	Engine->SetState(m_btnAccessory, Engine->m_SettingsMgr->m_iAccessory == 1 ? UI_STATE_BUTTON_DOWN : UI_STATE_BUTTON_NORMAL);

	Engine->SetState(m_btnNormal, Engine->m_SettingsMgr->m_iNormal == 1 ? UI_STATE_BUTTON_DOWN : UI_STATE_BUTTON_NORMAL);
	Engine->SetState(m_btnUpgrade, Engine->m_SettingsMgr->m_iUpgrade == 1 ? UI_STATE_BUTTON_DOWN : UI_STATE_BUTTON_NORMAL);
	Engine->SetState(m_btnCraft, Engine->m_SettingsMgr->m_iCraft == 1 ? UI_STATE_BUTTON_DOWN : UI_STATE_BUTTON_NORMAL);
	Engine->SetState(m_btnRare, Engine->m_SettingsMgr->m_iRare == 1 ? UI_STATE_BUTTON_DOWN : UI_STATE_BUTTON_NORMAL);
	Engine->SetState(m_btnMagic, Engine->m_SettingsMgr->m_iMagic == 1 ? UI_STATE_BUTTON_DOWN : UI_STATE_BUTTON_NORMAL);
	Engine->SetState(m_btnUnique, Engine->m_SettingsMgr->m_iUnique == 1 ? UI_STATE_BUTTON_DOWN : UI_STATE_BUTTON_NORMAL);

	Engine->SetState(m_btnConsumable, Engine->m_SettingsMgr->m_iConsumable == 1 ? UI_STATE_BUTTON_DOWN : UI_STATE_BUTTON_NORMAL);
	
	m_bNeedToSetLoot = false;
}

DWORD uiGenieSubVTable;
DWORD genieSubSetVisibleParam;
void __stdcall GenieSubSetVisible_Hook(bool visible)
{
	DWORD thisPtr;
	__asm
	{
		MOV thisPtr, ECX
	}

	genieSubSetVisibleParam = visible ? 1 : 0;

	if (Engine->m_SettingsMgr->m_iVisibleGenie)
		genieSubSetVisibleParam = 1;

	_asm
	{
		MOV ECX, thisPtr
		MOV EAX, genieSubSetVisibleParam
		PUSH EAX
		CALL KO_SET_VISIBLE_GENIE_SUB_FUNC
	}
}

void CUISeedHelperPlug::InitSetVisible()
{
	*(DWORD*)KO_SET_VISIBLE_GENIE_SUB_PTR = (DWORD)GenieSubSetVisible_Hook;
}

void __stdcall GenieSubSetVisibleViaOGFuncAsm()
{
	_asm
	{
		MOV ECX, uiGenieSubVTable
		MOV EAX, genieSubSetVisibleParam
		PUSH EAX
		CALL KO_SET_VISIBLE_GENIE_SUB_FUNC
	}
}

void CUISeedHelperPlug::SetGenieSetting()
{
	uiGenieSubVTable = Engine->uiGenieSubPlug->m_dVTableAddr;
	genieSubSetVisibleParam = Engine->m_SettingsMgr->m_iVisibleGenie == 1;
	GenieSubSetVisibleViaOGFuncAsm();
}

void CUISeedHelperPlug::ApplySettings()
{
	SetGenieSetting();

	char ar[2];
	switch (Engine->m_SettingsMgr->m_iCameraRange)
	{
	case 0:ar[0] = 0x10; ar[1] = 0x41; break;
	case 1:ar[0] = 0x20; ar[1] = 0x41; break;
	case 2:ar[0] = 0x30; ar[1] = 0x41; break;
	case 3:ar[0] = 0x40; ar[1] = 0x41; break;
	case 4:ar[0] = 0x50; ar[1] = 0x41; break;
	case 5:ar[0] = 0x60; ar[1] = 0x41; break;
	case 6:ar[0] = 0x70; ar[1] = 0x41; break;
	case 7:ar[0] = 0x80; ar[1] = 0x41; break;
	case 8:ar[0] = 0x88; ar[1] = 0x41; break;
	case 9:ar[0] = 0x90; ar[1] = 0x41; break;
	case 10:ar[0] = 0x98; ar[1] = 0x41; break;
	default:
		ar[0] = 0x50; ar[1] = 0x41;
		break;
	}
	Engine->WriteString(0x00B8FBBA, ar);

	Packet result(WIZ_HOOK_GUARD, uint8(WIZ_HOOK_LOOT_SETTINS));
	result << uint8(1) << uint8(1)
		<< uint8(1) << uint8(1)
		<< uint8(Engine->m_SettingsMgr->m_iWeapon) << uint8(Engine->m_SettingsMgr->m_iArmor)
		<< uint8(Engine->m_SettingsMgr->m_iAccessory) << uint8(Engine->m_SettingsMgr->m_iNormal)
		<< uint8(Engine->m_SettingsMgr->m_iUpgrade) << uint8(Engine->m_SettingsMgr->m_iCraft)
		<< uint8(Engine->m_SettingsMgr->m_iRare) << uint8(Engine->m_SettingsMgr->m_iMagic)
		<< uint8(Engine->m_SettingsMgr->m_iUnique) << uint8(Engine->m_SettingsMgr->m_iConsumable)
		<< uint32(Engine->m_SettingsMgr->m_iPrice);

	Engine->Send(&result);
}

void CUISeedHelperPlug::SetCameraRange(int val)
{
	Engine->SetScrollValue(m_CameraZoom, val);
}

int CUISeedHelperPlug::GetCameraRange()
{
	return Engine->GetScrollValue(m_CameraZoom);
}