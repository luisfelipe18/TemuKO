#pragma once
class CSettingsManager;
#include "stdafx.h"
#include "TEMUKOACS Engine.h"

class CSettingsManager
{
public:
	int m_iVisibleGenie;
	int m_iCameraRange;
	std::string m_uID;

	int m_iWeapon;
	int m_iArmor;
	int m_iAccessory;

	int m_iNormal;
	int m_iUpgrade;
	int m_iUnique;
	int m_iRare;
	int m_iCraft;
	int m_iConsumable;
	int m_iMagic;
	int m_iPrice;

	int m_iRealFullScreen;
	int m_iVsync;

	int m_micDevice;
	int m_micState;
	int m_muteAll;
	int m_muteListening;
	int m_muteSpeaking;

public:
	CSettingsManager();
	~CSettingsManager();
	void Init();
	void Save();

private:

};