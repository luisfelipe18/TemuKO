#pragma once
class CPlayerBase;
#include "stdafx.h"
#include "TEMUKOACS Engine.h"

enum ClassType
{
	ClassWarrior = 1,
	ClassRogue = 2,
	ClassMage = 3,
	ClassPriest = 4,
	ClassWarriorNovice = 5,
	ClassWarriorMaster = 6,
	ClassRogueNovice = 7,
	ClassRogueMaster = 8,
	ClassMageNovice = 9,
	ClassMageMaster = 10,
	ClassPriestNovice = 11,
	ClassPriestMaster = 12,
	ClassPortuKurian = 13,
	ClassPortuKurianNovice = 14,
	ClassPortuKurianMaster = 15
};

#define GROUP_WARRIOR				1
#define GROUP_ROGUE					2
#define GROUP_MAGE					3
#define GROUP_CLERIC				4
#define GROUP_ATTACK_WARRIOR		5
#define GROUP_DEFENSE_WARRIOR		6
#define GROUP_ARCHERER				7
#define GROUP_ASSASSIN				8
#define GROUP_ATTACK_MAGE			9
#define GROUP_PET_MAGE				10
#define GROUP_HEAL_CLERIC			11
#define GROUP_CURSE_CLERIC			12
#define GROUP_PORTU_KURIAN			13

class CPlayerBase
{
public:
	uint16 m_iSocketID;

	std::string m_strCharacterName;
	short m_sClass;
	uint8 m_iRace;
	uint8 m_iLevel;
	uint8 m_bNation;
	uint32 m_iGold;

	uint16 m_iStr;
	uint16 m_iHp;
	uint16 m_iDex;
	uint16 m_iInt;
	uint16 m_iMp;

	uint16 m_iRebStr;
	uint16 m_iRebHp;
	uint16 m_iRebDex;
	uint16 m_iRebInt;
	uint16 m_iRebMp;

	int m_iKC;

	uint8 m_iRebFreeStatPoints;
	uint16 m_iFreeStatPoints;
	uint8 m_iFreeSkillPoints;

	uint8 m_iZoneID;

public:
	CPlayerBase();
	~CPlayerBase();
	void UpdateFromMemory();
	void TEMUKOACSUserUpdate(Packet& pkt);
	void PlayerUpdate(Packet& pkt);
	void UpdateKC(Packet& pkt);
	void UpdateZone(uint8 zoneID);
	void UpdateFreeSkillPoints(uint8 freePoints);
	void UpdateStats(Packet& pkt);
	void ResetStats(Packet& pkt);
	void UpdateFreeStatPoints(uint16 freePoints);
	void UpdateRebStats(Packet& pkt);
	void SetRebStats();
	void UpdateGold();
	uint16 GetTargetID();
	INLINE uint16 GetSocketID() { return m_iSocketID; }
	INLINE uint8 GetZoneID() { return m_iZoneID; }
	INLINE uint8 GetLevel() { UpdateFromMemory(); return m_iLevel; }
	INLINE uint16 GetClass() { UpdateFromMemory(); return m_sClass; }
	bool JobGroupCheck(short jobgroupid);
	INLINE bool isWarrior() { return JobGroupCheck(ClassWarrior); }
	INLINE bool isRogue() { return JobGroupCheck(ClassRogue); }
	INLINE bool isMage() { return JobGroupCheck(ClassMage); }
	INLINE bool isPriest() { return JobGroupCheck(ClassPriest); }
	INLINE bool isPortuKurian() { return JobGroupCheck(ClassPortuKurian); }

	INLINE ClassType GetBaseClassType()
	{
		static const ClassType classTypes[] =
		{
			ClassWarrior, ClassRogue, ClassMage, ClassPriest,
			ClassWarriorNovice, ClassWarriorMaster,		// job changed / mastered
			ClassRogueNovice, ClassRogueMaster,			// job changed / mastered
			ClassMageNovice, ClassMageMaster,			// job changed / mastered
			ClassPriestNovice, ClassPriestMaster,		// job changed / mastered
			ClassPortuKurian,ClassPortuKurianNovice,	// job changed / mastered 
			ClassPortuKurianMaster,						// job changed / mastered
		};

		uint8 classType = GetClassType();

		if (classType < 1 && classType > 19){
			return ClassType(0);
		}

		return classTypes[classType - 1];
	}
	INLINE uint8 GetClassType()
	{
		return GetClass() % 100;
	}
	INLINE bool isMastered()
	{
		UpdateFromMemory();
		uint16 sClass = GetClassType();
		return (sClass == ClassWarriorMaster
			|| sClass == ClassRogueMaster
			|| sClass == ClassMageMaster
			|| sClass == ClassPriestMaster
			|| sClass == ClassPortuKurianMaster);
	}
private:

};
