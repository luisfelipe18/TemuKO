#include "stdafx.h"
#include "SettingsManager.h"

const std::string SETTINGS_INI = xorstr("TEMUKOACS\\settings.ini");
const std::string OPTIONS_INI = xorstr("Option.ini");

CSettingsManager::CSettingsManager()
{
	m_iVisibleGenie = 1;
	m_iCameraRange = 0;

	m_uID = "";

	m_iWeapon = 1;
	m_iArmor = 1;
	m_iAccessory = 1;

	m_iNormal = 1;
	m_iUpgrade = 1;
	m_iUnique = 1;
	m_iRare = 1;
	m_iCraft = 1;
	m_iConsumable = 1;
	m_iMagic = 1;
	m_iPrice = 0;

	m_iRealFullScreen = 0;
	m_iVsync = 0;

	m_micDevice = -1;
	m_micState = 1;
	m_muteAll = 0;
	m_muteListening = 0;
	m_muteSpeaking = 0;

	Init();
}

CSettingsManager::~CSettingsManager()
{
}

void CSettingsManager::Init()
{
	std::string section = xorstr("Misc");
	std::string key = xorstr("HideGenie");
	std::string path = Engine->m_BasePath + SETTINGS_INI;

	m_iVisibleGenie = GetPrivateProfileIntA(section.c_str(), key.c_str(), 1, path.c_str());
	key = xorstr("CameraRange");
	m_iCameraRange = GetPrivateProfileIntA(section.c_str(), key.c_str(), 10, path.c_str());

	section = xorstr("Graphic");

	key = xorstr("vsync");
	m_iVsync = GetPrivateProfileIntA(section.c_str(), key.c_str(), 0, path.c_str());
	key = xorstr("fullscreen");
	m_iRealFullScreen = GetPrivateProfileIntA(section.c_str(), key.c_str(), 0, path.c_str());

	section = xorstr("Login");

	key = xorstr("LoginUID");
	char tmp[50];
	GetPrivateProfileStringA(section.c_str(), key.c_str(), "", tmp, 50, path.c_str());
	m_uID = tmp;

	section = xorstr("Loot");

	key = xorstr("Weapon");
	m_iWeapon = GetPrivateProfileIntA(section.c_str(), key.c_str(), 1, path.c_str());
	key = xorstr("Armor");
	m_iArmor = GetPrivateProfileIntA(section.c_str(), key.c_str(), 1, path.c_str());
	key = xorstr("Accessory");
	m_iAccessory = GetPrivateProfileIntA(section.c_str(), key.c_str(), 1, path.c_str());

	key = xorstr("Normal");
	m_iNormal = GetPrivateProfileIntA(section.c_str(), key.c_str(), 1, path.c_str());
	key = xorstr("Upgrade");
	m_iUpgrade = GetPrivateProfileIntA(section.c_str(), key.c_str(), 1, path.c_str());
	key = xorstr("Unique");
	m_iUnique = GetPrivateProfileIntA(section.c_str(), key.c_str(), 1, path.c_str());
	key = xorstr("Rare");
	m_iRare = GetPrivateProfileIntA(section.c_str(), key.c_str(), 1, path.c_str());
	key = xorstr("Craft");
	m_iCraft = GetPrivateProfileIntA(section.c_str(), key.c_str(), 1, path.c_str());
	key = xorstr("Consumable");
	m_iConsumable = GetPrivateProfileIntA(section.c_str(), key.c_str(), 1, path.c_str());
	key = xorstr("Magic");
	m_iMagic = GetPrivateProfileIntA(section.c_str(), key.c_str(), 1, path.c_str());

	path = Engine->m_BasePath + OPTIONS_INI;
	section = xorstr("PetOption");
	key = xorstr("PetLootMoney");
	m_iPrice = GetPrivateProfileIntA(section.c_str(), key.c_str(), 0, path.c_str());

	section = xorstr("Voice");

	key = xorstr("MicDevice");
	m_micDevice = GetPrivateProfileIntA(section.c_str(), key.c_str(), -1, path.c_str());
	key = xorstr("MicState");
	m_micState = GetPrivateProfileIntA(section.c_str(), key.c_str(), 1, path.c_str());
	key = xorstr("MuteAll");
	m_muteAll = GetPrivateProfileIntA(section.c_str(), key.c_str(), 0, path.c_str());
	key = xorstr("MuteListening");
	m_muteListening = GetPrivateProfileIntA(section.c_str(), key.c_str(), 0, path.c_str());
	key = xorstr("MuteSpeaking");
	m_muteSpeaking = GetPrivateProfileIntA(section.c_str(), key.c_str(), 0, path.c_str());

	if (m_iVisibleGenie < 0) m_iVisibleGenie = 0;
	if (m_iVisibleGenie > 1) m_iVisibleGenie = 1;
	if (m_iCameraRange < 1) m_iCameraRange = 1;
	if (m_iCameraRange > 10) m_iCameraRange = 10;

	if (m_iWeapon < 0) m_iWeapon = 0;
	if (m_iWeapon > 1) m_iWeapon = 1;
	if (m_iArmor < 0) m_iArmor = 0;
	if (m_iArmor > 1) m_iArmor = 1;
	if (m_iAccessory < 0) m_iAccessory = 0;
	if (m_iAccessory > 1) m_iAccessory = 1;

	if (m_iUpgrade < 0) m_iUpgrade = 0;
	if (m_iUpgrade > 1) m_iUpgrade = 1;
	if (m_iUnique < 0) m_iUnique = 0;
	if (m_iUnique > 1) m_iUnique = 1;
	if (m_iRare < 0) m_iRare = 0;
	if (m_iRare > 1) m_iRare = 1;
	if (m_iCraft < 0) m_iCraft = 0;
	if (m_iCraft > 1) m_iCraft = 1;
	if (m_iConsumable < 0) m_iConsumable = 0;
	if (m_iConsumable > 1) m_iConsumable = 1;
	if (m_iMagic < 0) m_iMagic = 0;
	if (m_iMagic > 1) m_iMagic = 1;
	if (m_iPrice < 0) m_iPrice = 0;
	if (m_iPrice > INT_MAX) m_iPrice = INT_MAX;
}

void CSettingsManager::Save()
{
	std::string section = xorstr("Misc");
	std::string key = xorstr("VisibleGenie");
	std::string path = Engine->m_BasePath + SETTINGS_INI;

	WritePrivateProfileStringA(section.c_str(), key.c_str(), std::to_string(m_iVisibleGenie).c_str(), path.c_str());
	key = xorstr("CameraRange");
	WritePrivateProfileStringA(section.c_str(), key.c_str(), std::to_string(m_iCameraRange).c_str(), path.c_str());

	section = xorstr("Graphic");

	key = xorstr("vsync");
	WritePrivateProfileStringA(section.c_str(), key.c_str(), std::to_string(m_iVsync).c_str(), path.c_str());
	key = xorstr("fullscreen");
	WritePrivateProfileStringA(section.c_str(), key.c_str(), std::to_string(m_iRealFullScreen).c_str(), path.c_str());

	section = xorstr("Login");

	key = xorstr("LoginUID");
	WritePrivateProfileStringA(section.c_str(), key.c_str(), m_uID.c_str(), path.c_str());

	section = xorstr("Loot");

	key = xorstr("Weapon");
	WritePrivateProfileStringA(section.c_str(), key.c_str(), std::to_string(m_iWeapon).c_str(), path.c_str());
	key = xorstr("Armor");
	WritePrivateProfileStringA(section.c_str(), key.c_str(), std::to_string(m_iArmor).c_str(), path.c_str());
	key = xorstr("Accessory");
	WritePrivateProfileStringA(section.c_str(), key.c_str(), std::to_string(m_iAccessory).c_str(), path.c_str());

	key = xorstr("Normal");
	WritePrivateProfileStringA(section.c_str(), key.c_str(), std::to_string(m_iNormal).c_str(), path.c_str());
	key = xorstr("Upgrade");
	WritePrivateProfileStringA(section.c_str(), key.c_str(), std::to_string(m_iUpgrade).c_str(), path.c_str());
	key = xorstr("Unique");
	WritePrivateProfileStringA(section.c_str(), key.c_str(), std::to_string(m_iUnique).c_str(), path.c_str());
	key = xorstr("Rare");
	WritePrivateProfileStringA(section.c_str(), key.c_str(), std::to_string(m_iRare).c_str(), path.c_str());
	key = xorstr("Craft");
	WritePrivateProfileStringA(section.c_str(), key.c_str(), std::to_string(m_iCraft).c_str(), path.c_str());
	key = xorstr("Consumable");
	WritePrivateProfileStringA(section.c_str(), key.c_str(), std::to_string(m_iConsumable).c_str(), path.c_str());
	key = xorstr("Magic");
	WritePrivateProfileStringA(section.c_str(), key.c_str(), std::to_string(m_iMagic).c_str(), path.c_str());

	path = Engine->m_BasePath + OPTIONS_INI;
	section = xorstr("PetOption");
	key = xorstr("PetLootMoney");
	WritePrivateProfileStringA(section.c_str(), key.c_str(), std::to_string(m_iPrice).c_str(), path.c_str());

	section = xorstr("Voice");

	key = xorstr("MicDevice");
	WritePrivateProfileStringA(section.c_str(), key.c_str(), std::to_string(m_micDevice).c_str(), path.c_str());
	key = xorstr("MicState");
	WritePrivateProfileStringA(section.c_str(), key.c_str(), std::to_string(m_micState).c_str(), path.c_str());
	key = xorstr("MuteAll");
	WritePrivateProfileStringA(section.c_str(), key.c_str(), std::to_string(m_muteAll).c_str(), path.c_str());
	key = xorstr("MuteListening");
	WritePrivateProfileStringA(section.c_str(), key.c_str(), std::to_string(m_muteListening).c_str(), path.c_str());
	key = xorstr("MuteSpeaking");
	WritePrivateProfileStringA(section.c_str(), key.c_str(), std::to_string(m_muteSpeaking).c_str(), path.c_str());
}