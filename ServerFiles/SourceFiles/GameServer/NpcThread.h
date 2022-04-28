#pragma once
#include "NpcSignalling.h"

class CNpc;
class CNpcThread
{
public:
	CNpcThread(uint16 sZoneID);
	void AddRequest(Signal * pkt);
	virtual ~CNpcThread();
	void Shutdown();
private:
	void HandleJuraidKill(uint16 sNpcID, uint16 sKillerID, uint8 bNation);
	void HandleGateFlagOpen(uint16 sNpcID, uint8 m_stType, uint32 m_sByGateOpen);
	void HandleJuraidGateOpen(uint16 sNpcID, Nation bNation);

	bool LoadNPCs();
	void ResetAllNPCs();
	void RemoveAllNPCs();
	void ChangeAbilityAllNPCs(uint8 bType);
	void LoadAllObjects();
	void AddNPC(AddNPCSignals* addNPCSignal);
	void AddPET(AddPETSignals* addPETSignal);
	bool AddObjectEventNpc(_OBJECT_EVENT* pEvent, uint16 nEventRoom = 0, bool isEventObject = false);
	void RemoveNPC(uint32 sNpcID);
	void HandleNPC();
	void Engine();

public:
	typedef CSTLMap <CNpc>	NpcArray;
	NpcArray m_arNpcArray;
	std::thread m_thread;

private:
	uint16 m_sZoneID;
	bool _running;
	std::queue<Signal *> signal_queue;
	std::recursive_mutex signalqueue_lock;
	Condition s_hEvent;
	void State_Null(Signal & input);
};
