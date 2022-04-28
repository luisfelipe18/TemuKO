#pragma once

#include "../N3BASE/N3ShapeMgr.h"
#include "Region.h"
#include "GameEvent.h"
#include "../shared/STLMap.h"

class CGameEvent;
typedef CSTLMap <CGameEvent>		EventArray;

class CUser;
class CBot;
class CNpc;
class CGameServerDlg;
class SMDFile;

// temporary
struct CSize
{
	CSize() : cx(0), cy(0) {}
	CSize(int cx, int cy) : cx(cx), cy(cy) {}
	int cx, cy;
};

class C3DMap
{
public:
	// Passthru methods
	int GetXRegionMax();
	int GetZRegionMax(); 
	int GetMapSize();
	float GetUnitDistance();
	bool IsValidPosition(float x, float z, float y);
	_OBJECT_EVENT * GetObjectEvent(int objectindex);
	_REGENE_EVENT * GetRegeneEvent(int objectindex);
	_WARP_INFO * GetWarp(int warpID);
	void GetWarpList(int warpGroup, std::set<_WARP_INFO *> & warpEntries);

	C3DMap();
	bool Initialize(_ZONE_INFO *pZone);
	CRegion * GetRegion(uint16 regionX, uint16 regionZ);
	bool CheckEvent( float x, float z, CUser* pUser = nullptr );
	void RegionItemRemove(CRegion * pRegion, _LOOT_BUNDLE * pBundle, _LOOT_ITEM * pItem);
	bool RegionItemAdd(uint16 rx, uint16 rz, _LOOT_BUNDLE * pBundle);
	virtual ~C3DMap();

	EventArray	m_EventArray;

	int	m_nServerNo, m_nZoneNumber;
	std::string m_nZoneName;
	uint8 m_Status,
		m_MinLevel,
		m_MaxLevel,
		m_ZoneType,
		m_kTradeOtherNation,
		m_kTalkOtherNation,
		m_kAttackOtherNation,
		m_kAttackSameNation,
		m_kFriendlyNpc,
		m_kWarZone,
		m_kClanUpdates,
		m_bTeleport,
		m_bGate,
		m_bEscape,
		m_bCallingFriend,
		m_bTeleportFriend,
		m_bBlink,
		m_bPetSpawn,
		m_bExpLost,
		m_bGiveLoyalty,
		m_bGuardSummon,
		m_bBlinkZone;
	float m_fInitX, m_fInitZ, m_fInitY;
	short	m_sMaxUser;

	CRegion**	m_ppRegion;

	uint32	m_wBundle;	// Zone Item Max Count

	SMDFile *m_smdFile;
	std::recursive_mutex m_lock;

	INLINE uint16 GetID() { return m_nZoneNumber; }

	/* the following should all be duplicated to AI server's map class for now */

	INLINE bool canTradeWithOtherNation() { return (m_zoneFlags & ZF_TRADE_OTHER_NATION) != 0; }
	INLINE bool canTalkToOtherNation() { return (m_zoneFlags & ZF_TALK_OTHER_NATION) != 0; }
	INLINE bool canAttackOtherNation() { return (m_zoneFlags & ZF_ATTACK_OTHER_NATION) != 0; } 
	INLINE bool canAttackSameNation() { return (m_zoneFlags & ZF_ATTACK_SAME_NATION) != 0; } 
	INLINE bool isWarZone() { return (m_zoneFlags & ZF_WAR_ZONE) != 0; }
	INLINE bool isNationPVPZone() { return canAttackOtherNation() && !canAttackSameNation(); }
	INLINE bool areNPCsFriendly() { return (m_zoneFlags & ZF_FRIENDLY_NPCS) != 0; }
	INLINE bool canUpdateClan() { return (m_zoneFlags & ZF_CLAN_UPDATE) != 0; }

	INLINE uint8 GetZoneType() { return m_zoneType; }
	INLINE uint8 GetTariff() { return m_byTariff; }
	INLINE void SetTariff(uint8 tariff) { m_byTariff = tariff; }

	INLINE uint8 GetMinLevelReq() { return m_byMinLevel; }
	INLINE uint8 GetMaxLevelReq() { return m_byMaxLevel; }

	INLINE uint8 isBlinkZone() { return m_bBlink; }
	INLINE uint8 isTeleportZone() { return m_bTeleport; }
	INLINE uint8 isGateZone() { return m_bGate; }
	INLINE uint8 isEscapeZone() { return m_bEscape; }
	INLINE uint8 isCallingFriendZone() { return m_bCallingFriend; }
	INLINE uint8 isTeleportFriendZone() { return m_bTeleportFriend; }
	INLINE uint8 isGuardSummonZone() { return m_bGuardSummon; }

	void UpdateDelosDuringCSW(bool Open = true, ZoneAbilityType type = ZoneAbilityPVP);
	bool IsMovable(int dest_x, int dest_y);

protected:
	void SetZoneAttributes(_ZONE_INFO *pZone);

	ZoneAbilityType m_zoneType;
	uint16 m_zoneFlags;
	uint8 m_byTariff;
	uint8 m_byMinLevel, m_byMaxLevel;
};