#include "stdafx.h"
#include "PlayerBase.h"

CPlayerBase::CPlayerBase()
{
	m_iSocketID = 0;

	m_strCharacterName = "";
	m_sClass = 0;
	m_iRace = 0;
	m_iLevel = 0;
	m_iGold = 0;

	m_iStr = 0;
	m_iHp = 0;
	m_iDex = 0;
	m_iInt = 0;
	m_iMp = 0;

	m_iRebStr = 0;
	m_iRebHp = 0;
	m_iRebDex = 0;
	m_iRebInt = 0;
	m_iRebMp = 0;

	m_iKC = 0;

	m_iFreeStatPoints = 0;
	m_iFreeSkillPoints = 0;

	m_iZoneID = 0;
}

CPlayerBase::~CPlayerBase()
{
}

DWORD chr = 0;

void CPlayerBase::UpdateFromMemory()
{
	if (chr == 0) 
		ReadProcessMemory(GetCurrentProcess(), (LPVOID)KO_PTR_CHR, &chr, sizeof(chr), 0);

	ReadProcessMemory(GetCurrentProcess(), (LPVOID)(chr + 0xB9C), &m_iGold, sizeof(m_iGold), 0);
	ReadProcessMemory(GetCurrentProcess(), (LPVOID)(chr + KO_OFF_CLASS), &m_sClass, sizeof(m_sClass), 0);
	ReadProcessMemory(GetCurrentProcess(), (LPVOID)(chr + 0x6B4), &m_iLevel, sizeof(m_iLevel), 0);

	uint8 nameLen = 0;
	if (nameLen <= 0)
		return;

	ReadProcessMemory(GetCurrentProcess(), (LPVOID)(chr + KO_OFF_NAMELEN), &nameLen, sizeof(nameLen), 0);
	char* buff;
	buff = (char*)malloc(nameLen);
	ReadProcessMemory(GetCurrentProcess(), (void*)(chr + KO_OFF_NAME), (LPVOID)buff, nameLen, 0);
	m_strCharacterName = buff;
	m_strCharacterName = m_strCharacterName.substr(0, nameLen);
}

void CPlayerBase::PlayerUpdate(Packet& pkt)
{
	pkt >> m_iSocketID;
	pkt.DByte();
	pkt >> m_strCharacterName
		>> m_sClass 
		>> m_iRace
		>> m_iLevel
		>> m_bNation
		>> m_iStr 
		>> m_iHp 
		>> m_iDex 
		>> m_iInt 
		>> m_iMp;
}

void CPlayerBase::TEMUKOACSUserUpdate(Packet& pkt)
{
	pkt >> m_iSocketID;
	pkt.DByte();
	pkt >> m_strCharacterName 
		>> m_sClass
		>> m_iRace 
		>> m_iLevel 
		>> m_bNation
		>> m_iStr 
		>> m_iHp 
		>> m_iDex
		>> m_iInt
		>> m_iMp
		// Engine Settings
		>> Engine->EngineSettings->ACS_Validation
		>> Engine->EngineSettings->Button_Facebook
		>> Engine->EngineSettings->Button_Discord
		>> Engine->EngineSettings->Button_Live
		>> Engine->EngineSettings->Button_Support
		>> Engine->EngineSettings->URL_Facebook
		>> Engine->EngineSettings->URL_Discord
		>> Engine->EngineSettings->URL_Live
		>> Engine->EngineSettings->URL_KCbayi
		>> Engine->EngineSettings->KCMerchant_MinPrice
		>> Engine->EngineSettings->KCMerchant_MaxPrice
		>> Engine->EngineSettings->State_StatReset
		>> Engine->EngineSettings->State_SkillReset
		>> Engine->EngineSettings->State_TempItemList
		>> Engine->EngineSettings->State_PUS;
	UpdateFromMemory();
}

void CPlayerBase::UpdateKC(Packet& pkt)
{
	pkt >> m_iKC;

	if (Engine->uiState != NULL)
		Engine->uiState->UpdateKC(m_iKC, 0);

	if (Engine->uiTradeInventory != NULL)
		Engine->uiTradeInventory->UpdateTotal(m_iKC);

	UpdateFromMemory();
}

void CPlayerBase::UpdateZone(uint8 zoneID)
{
	m_iZoneID = zoneID;
	UpdateFromMemory();
}

void CPlayerBase::UpdateGold()
{
	UpdateFromMemory();
}

void CPlayerBase::UpdateFreeSkillPoints(uint8 freePoints)
{
	m_iFreeSkillPoints = freePoints;
}

void CPlayerBase::UpdateStats(Packet& pkt)
{
	uint8 type, diff;
	uint16 newStat;

	pkt >> type >> newStat;
	type--;
	switch (type)
	{
	case 0: // str
		diff = newStat - m_iStr;
		m_iStr = newStat;
		break;
	case 1: // hp
		diff = newStat - m_iHp;
		m_iHp = newStat;
		break;
	case 2: // dex
		diff = newStat - m_iDex;
		m_iDex = newStat;
		break;
	case 3: // int
		diff = newStat - m_iInt;
		m_iInt = newStat;
		break;
	case 4: // mp
		diff = newStat - m_iMp;
		m_iMp = newStat;
		break;
	}

	m_iFreeStatPoints -= diff;
	//Engine->m_PlayerBase->UpdateFreeStatPoints(m_iFreeStatPoints);
}

void CPlayerBase::ResetStats(Packet& pkt)
{
	uint32 param32;
	uint16 param16, str, hp, dex, intt, mp, free;

	pkt >> param32 >> str >> hp >> dex >> intt >> mp
		>> param16 >> param16 >> param16 >> param32 >> free;

	m_iStr = str;
	m_iHp = hp;
	m_iDex = dex;
	m_iInt = intt;
	m_iMp = mp;

	//Engine->m_PlayerBase->UpdateFreeStatPoints(free);
}

void CPlayerBase::UpdateFreeStatPoints(uint16 freePoints)
{
	m_iFreeStatPoints = freePoints;
	//Engine->SetString(Engine->uiState->m_strFreeStatPoint, std::to_string(m_iFreeStatPoints));
}

void CPlayerBase::UpdateRebStats(Packet& pkt)
{
	pkt >> m_iRebStr >> m_iRebHp >> m_iRebDex >> m_iRebInt >> m_iRebMp >> m_iRebFreeStatPoints;
	SetRebStats();
}

void CPlayerBase::SetRebStats()
{

}

uint16 CPlayerBase::GetTargetID() { return Engine->m_zMob; }

bool CPlayerBase::JobGroupCheck(short jobgroupid)
{
	if (jobgroupid > 100)
		return GetClass() == jobgroupid;

	ClassType subClass = GetBaseClassType();
	switch (jobgroupid)
	{
	case GROUP_WARRIOR:
		return (subClass == ClassWarrior || subClass == ClassWarriorNovice || subClass == ClassWarriorMaster);

	case GROUP_ROGUE:
		return (subClass == ClassRogue || subClass == ClassRogueNovice || subClass == ClassRogueMaster);

	case GROUP_MAGE:
		return (subClass == ClassMage || subClass == ClassMageNovice || subClass == ClassMageMaster);

	case GROUP_CLERIC:
		return (subClass == ClassPriest || subClass == ClassPriestNovice || subClass == ClassPriestMaster);

	case GROUP_PORTU_KURIAN:
		return (subClass == ClassPortuKurian || subClass == ClassPortuKurianNovice || subClass == ClassPortuKurianMaster);
	}

	return (subClass == jobgroupid);
}