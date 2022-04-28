#pragma once
#include "stdafx.h"
#include "../shared/Condition.h"

#pragma region Enumerators

enum NpcThreadSignalType
{
	NpcHandle = 0,
	NpcAdd = 1,
	NpcRemove = 2,
	NpcAllLoad = 3,
	NpcAllRemove = 4,
	NpcAllReset = 5,
	NpcAllChangeAbility = 6,
	HandleJuraidMountainNpcKill = 7,
	HandleGateOpenFlag = 8,
	PetAdd = 9,
};

#pragma endregion

#pragma region Signal Subclasses

class AddNPCSignals
{
public:
	AddNPCSignals(uint16 sSid, bool bIsMonster, uint8 byZone, float fX, float fY, float fZ,
		uint16 sCount = 1, uint16 sRadius = 0, uint16 sDuration = 0,
		uint8 nation = 0, int16 socketID = -1, uint16 nEventRoom = 0,
		uint8 byDirection = 0, uint8 bMoveType = 0, uint8 bGateOpen = 0,
		uint16 nSummonSpecialType = 0, uint32 nSummonSpecialID = 0,
		bool bIsEvent = true)
	{
		m_sSid = sSid;
		m_bIsMonster = bIsMonster;
		m_byZone = byZone;
		m_fX = fX;
		m_fY = fY;
		m_fZ = fZ;
		m_sCount = sCount;
		m_sRadius = sRadius;
		m_sDuration = sDuration;
		m_nation = nation;
		m_socketID = socketID;
		m_nEventRoom = nEventRoom;
		m_byDirection = byDirection;
		m_bMoveType = bMoveType;
		m_bGateOpen = bGateOpen;
		m_nSummonSpecialType = nSummonSpecialType;
		m_nSummonSpecialID = nSummonSpecialID;
		m_bIsEvent = bIsEvent;
	}

public:
	bool m_bIsMonster;
	bool m_bIsEvent;
	uint8 m_byZone;
	float m_fX;
	float m_fY;
	float m_fZ;
	uint16 m_sSid;
	uint16 m_sCount;
	uint16 m_sRadius;
	uint16 m_sDuration;
	int16 m_socketID;
	uint16 m_nEventRoom;
	uint8 m_nation;
	uint8 m_byDirection;
	uint8 m_bMoveType;
	uint8 m_bGateOpen;
	uint16 m_nSummonSpecialType;
	uint32 m_nSummonSpecialID;
};

class AddPETSignals
{
public:
	AddPETSignals(uint16 sSid, bool bIsMonster, uint8 byZone, float fX, float fY, float fZ,
		uint16 sCount = 1, uint16 sRadius = 0, uint16 sDuration = 0,
		uint8 nation = 0, int16 socketID = -1, uint16 nEventRoom = 0,
		uint8 nType = 0, uint32 nSkillID = 0, bool bIsEvent = true)
	{
		m_sSid = sSid;
		m_bIsMonster = bIsMonster;
		m_byZone = byZone;
		m_fX = fX;
		m_fY = fY;
		m_fZ = fZ;
		m_sCount = sCount;
		m_sRadius = sRadius;
		m_sDuration = sDuration;
		m_nation = nation;
		m_socketID = socketID;
		m_nEventRoom = nEventRoom;
		m_Type = nType;
		m_SkillID = nSkillID;
		m_bIsEvent = bIsEvent;
	}

public:
	bool m_bIsMonster;
	bool m_bIsEvent;
	uint8 m_byZone;
	float m_fX;
	float m_fY;
	float m_fZ;
	uint16 m_sSid;
	uint16 m_sCount;
	uint16 m_sRadius;
	uint16 m_sDuration;
	uint8 m_nation;
	int16 m_socketID;
	uint16 m_nEventRoom;
	uint8 m_Type;
	uint32 m_SkillID;
};

class RemoveNPCSignals
{
public:
	RemoveNPCSignals(uint32 sNpcID)
	{
		m_sNpcID = sNpcID;
	}

public:
	uint32 m_sNpcID;
};

class HandleJuraidKillSignals
{
public:
	HandleJuraidKillSignals(uint32 sNpcID, uint32 sKillerID, uint8 bNation)
	{
		m_sNpcID = sNpcID;
		m_sKillerID = sKillerID;
		m_bNation = bNation;
	}

public:
	uint32 m_sNpcID;
	uint32 m_sKillerID;
	uint8 m_bNation;
};

class HandleGateOpenFlags
{
public:
	HandleGateOpenFlags(uint32 sNpcID, uint8 stType, uint32 sByGateOpen)
	{
		m_sNpcID = sNpcID;
		m_stType = stType;
		m_sByGateOpen = sByGateOpen;
	}

public:
	uint32	m_sNpcID;
	uint8	m_stType;
	uint32	m_sByGateOpen;
};

class ChangeNPCAbilitySignals
{
public:
	ChangeNPCAbilitySignals(uint8 bType)
	{
		m_byType = bType;
	}

public:
	uint8 m_byType;
};

#pragma endregion

#pragma region Signal Class

class Signal
{
public:
	virtual ~Signal()
	{
		if (addNPCSignal)
			delete addNPCSignal;
		if (addPETSignal)
			delete addPETSignal;
		if (removeNPCSignal)
			delete removeNPCSignal;
		if (changeNPCAbilitySignal)
			delete changeNPCAbilitySignal;
		if (juraidKillSignal)
			delete juraidKillSignal;
		if (GateOpenSignal)
			delete GateOpenSignal;
	}

	Signal(AddNPCSignals* signal)
	{
		addNPCSignal = signal;
		addPETSignal = nullptr;
		removeNPCSignal = nullptr;
		changeNPCAbilitySignal = nullptr;
		juraidKillSignal = nullptr;
		GateOpenSignal = nullptr;
		strSignalString = string_format("<-- Signal, Add NPC ProtoID = %d", addNPCSignal->m_sSid);
		signal_type = NpcThreadSignalType::NpcAdd;
	}

	Signal(AddPETSignals* signal)
	{
		addNPCSignal = nullptr;
		addPETSignal = signal;
		removeNPCSignal = nullptr;
		changeNPCAbilitySignal = nullptr;
		juraidKillSignal = nullptr;
		GateOpenSignal = nullptr;
		strSignalString = string_format("<-- Signal, Add PET ProtoID = %d", addPETSignal->m_sSid);
		signal_type = NpcThreadSignalType::PetAdd;
	}

	Signal(RemoveNPCSignals* signal)
	{
		addNPCSignal = nullptr;
		addPETSignal = nullptr;
		removeNPCSignal = signal;
		changeNPCAbilitySignal = nullptr;
		juraidKillSignal = nullptr;
		GateOpenSignal = nullptr;
		strSignalString = string_format("<-- Signal, Remove NPC ID = %d", removeNPCSignal->m_sNpcID);
		signal_type = NpcThreadSignalType::NpcRemove;
	}

	Signal(ChangeNPCAbilitySignals* signal)
	{
		addNPCSignal = nullptr;
		addPETSignal = nullptr;
		removeNPCSignal = nullptr;
		changeNPCAbilitySignal = signal;
		juraidKillSignal = nullptr;
		GateOpenSignal = nullptr;
		strSignalString = "<-- Signal, Change Ability ALL ";
		signal_type = NpcThreadSignalType::NpcAllChangeAbility;
	}

	Signal(HandleJuraidKillSignals* signal)
	{
		addNPCSignal = nullptr;
		addPETSignal = nullptr;
		removeNPCSignal = nullptr;
		changeNPCAbilitySignal = nullptr;
		juraidKillSignal = signal;
		GateOpenSignal = nullptr;
		strSignalString = "<-- Signal, Handle Juraid Kill ";
		signal_type = NpcThreadSignalType::HandleJuraidMountainNpcKill;
	}

	Signal(HandleGateOpenFlags* signal)
	{
		addNPCSignal = nullptr;
		addPETSignal = nullptr;
		removeNPCSignal = nullptr;
		changeNPCAbilitySignal = nullptr;
		juraidKillSignal = nullptr;
		GateOpenSignal = signal;
		strSignalString = "<-- Signal, Handle Gate Open Flag ";
		signal_type = NpcThreadSignalType::HandleGateOpenFlag;
	}

	Signal(NpcThreadSignalType type)
	{
		removeNPCSignal = nullptr;
		addNPCSignal = nullptr;
		addPETSignal = nullptr;
		changeNPCAbilitySignal = nullptr;
		juraidKillSignal = nullptr;
		GateOpenSignal = nullptr;
		signal_type = type;
		switch (signal_type)
		{
		case NpcHandle:
			strSignalString = "<-- Signal, Handle All NPCs";
			break;
		case NpcAllLoad:
			strSignalString = "<-- Signal, Load All NPCs";
			break;
		case NpcAllRemove:
			strSignalString = "<-- Signal, Remove All NPCs";
			break;
		default:
			break;
		}
	}


public:
	AddNPCSignals * addNPCSignal;
	AddPETSignals * addPETSignal;
	RemoveNPCSignals* removeNPCSignal;
	ChangeNPCAbilitySignals* changeNPCAbilitySignal;
	HandleJuraidKillSignals* juraidKillSignal;
	HandleGateOpenFlags* GateOpenSignal;
	NpcThreadSignalType signal_type;
	std::string strSignalString;
};
#pragma endregion