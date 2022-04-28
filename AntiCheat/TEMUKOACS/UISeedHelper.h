#pragma once
class CUISeedHelperPlug;
#include "stdafx.h"
#include "TEMUKOACS Engine.h"

class CUISeedHelperPlug
{
public:
	DWORD m_dVTableAddr;
	DWORD m_dGroupOption;
	DWORD m_dGroupGame;
	DWORD m_dGroupEffect;
	DWORD m_dGroupLoot;

	DWORD m_CameraZoom;
	DWORD m_btnVisibleGenie;

	DWORD m_btnSaveSettings;

	DWORD m_btnWeapon;
	DWORD m_btnArmor;
	DWORD m_btnAccessory;

	DWORD m_btnNormal;
	DWORD m_btnUpgrade;
	DWORD m_btnUnique;
	DWORD m_btnRare;
	DWORD m_btnCraft;
	DWORD m_btnConsumable;
	DWORD m_btnMagic;

	DWORD m_editPrice;
	DWORD m_txtPrice;

	DWORD m_btnPriceSave;

	bool m_bNeedToSave;
	bool m_bNeedToSetLoot;
	bool m_bNeedToSetGame;
	bool m_bNeedToSetEffect;

public:
	CUISeedHelperPlug();
	~CUISeedHelperPlug();
	void ParseUIElements();
	bool ReceiveMessage(DWORD* pSender, uint32_t dwMsg);
	void InitReceiveMessage();
	void Tick();
	void InitSetVisible();
	void ApplyLootSettings();
	void ApplyGameSettings();
	void ApplyEffectSettings();
	void SetGenieSetting();
	void ApplySettings();
	void SetCameraRange(int val);
	int GetCameraRange();
private:

};