#pragma once

#include "LuaEngine.h"
#include "../shared/KOSocket.h"

#include "User.h"
#include "ChatHandler.h"


#include "GameDefine.h"

class CGameServerDlg;

class CBot
{
public:


	virtual uint16 GetID() { return m_sSid; }

	std::string& GetAccountName() { return m_strAccountID; }
	virtual std::string& GetName() { return m_strUserID; }

	std::string	m_strAccountID, m_strUserID, m_strMemo;
	GameState m_state;
	float	m_curx, m_curz, m_cury;
	uint8	m_bNation, m_bLevel, m_bZone;
	std::string MerchantChat;
	KOMap* m_pMap;
	CRegion* m_pRegion;
	uint8	m_bResHpType;
	uint32  m_pktcount;
	uint8	m_bRace;
	uint16	m_sClass;
	time_t	LastWarpTime;
	uint32	m_nHair;
	time_t  m_tLastKillTime;
	uint16	m_sChallangeAchieveID;
	uint16	m_sSid;
	uint8	m_bRank;
	uint8	m_bTitle;
	int64	m_iExp;
	uint32	m_iLoyalty, m_iLoyaltyMonthly;
	uint32	m_iMannerPoint;
	uint8	m_bFace;
	uint8	m_bCity;
	int16	m_bKnights;
	uint8	m_bFame;
	int16	m_sHp, m_sMp, m_sSp;
	uint8	m_bStats[STAT_COUNT];
	uint8	m_bRebStats[STAT_COUNT];
	uint8	m_reblvl;
	uint8	m_bAuthority;
	int16	m_sPoints; // this is just to shut the compiler up
	uint32	m_iGold, m_iBank;
	int16	m_sBind;
	_ITEM_DATA m_sItemArray[INVENTORY_TOTAL];

	int8	m_bMerchantState;
	bool	m_bSellingMerchantPreparing;
	bool	m_bBuyingMerchantPreparing;
	bool	m_bMerchantOpen;
	int16	m_bMerchantViewer;
	int16	m_sMerchantsSocketID;
	_MERCH_DATA	m_arMerchantItems[MAX_MERCH_ITEMS]; //What is this person selling? Stored in "_MERCH_DATA" structure.
	bool	m_bPremiumMerchant;

	uint8    m_bstrSkill[10];

	INLINE _ITEM_DATA* GetItem(uint8 pos)
	{
		return &m_sItemArray[pos];
	}
	INLINE uint8 GetNation() { return m_bNation; }
	INLINE uint8 GetRace() { return m_bRace; }
	INLINE uint8 GetLevel() { return m_bLevel; }
	INLINE uint16 GetClass() { return m_sClass; }

	INLINE uint8 GetStat(StatType type)
	{
		if (type >= STAT_COUNT)
			return 0;

		return m_bStats[type];
	}

	uint16		m_sAchieveCoverTitle;

	float	m_oldx, m_oldy, m_oldz;
	int16	m_sDirection;

	uint8	m_bNeedParty;

	bool	m_bInParty;
	bool	m_bPartyLeader;

	bool	m_bSlaveMerchant;
	int16	m_bSlaveUserID;

	bool	m_bIsChicken; // Is the character taking the beginner/chicken quest?
	bool	m_bIsHidingHelmet;
	bool	m_bIsHidingCospre;

	short	m_MaxHP, m_MaxMp;
	uint8	m_MaxSp;
	uint8	m_bInvisibilityType;

	uint32	m_bAbnormalType;			// Is the player normal, a giant, or a dwarf?
	uint32	m_nOldAbnormalType, ReturnSymbolisOK;
	bool	m_bBlockPrivateChat;
	short	m_sPrivateChatUser;

	uint16	m_sRegionX, m_sRegionZ; // this is probably redundant

	TeamColour	m_teamColour;
public:
	INLINE uint8 GetUniqueLevel() { return m_bRebStats[STAT_STR]; } // Char Rebirth mi 
	INLINE uint8 GetRebLevel() { return m_reblvl; }
	INLINE uint16 GetSPosX() { return uint16(GetX() * 10); };
	INLINE uint16 GetSPosY() { return uint16(GetY() * 10); };
	INLINE uint16 GetSPosZ() { return uint16(GetZ() * 10); };
	INLINE CRegion* GetRegion() { return m_pRegion; }
	void SetRegion(uint16 x = -1, uint16 z = -1);
	void StateChangeServerDirect(uint8 bType, uint32 nBuff);
	INLINE KOMap* GetMap() { return m_pMap; }
	INLINE float GetX() { return m_curx; }
	INLINE void SetPosition(float fx, float fy, float fz)
	{
		m_curx = fx;
		m_curz = fz;
		m_cury = fy;
	}

	bool RegisterRegion();
	void AddToRegion(int16 new_region_x, int16 new_region_z);
	void RemoveRegion(int16 del_x, int16 del_z);
	void InsertRegion(int16 insert_x, int16 insert_z);
	bool	m_bGenieStatus, m_bIsDevil, m_bIsHidingWings;

	int8	m_bPersonalRank;
	int8	m_bKnightsRank;

	INLINE uint16 GetNewRegionX() { return (uint16)(GetX()) / VIEW_DISTANCE; }
	INLINE uint16 GetNewRegionZ() { return (uint16)(GetZ()) / VIEW_DISTANCE; }
	INLINE float GetY() { return m_cury; }
	INLINE float GetZ() { return m_curz; }
	INLINE int16 GetClanID() { return m_bKnights; }
	INLINE void SetClanID(int16 val) { m_bKnights = val; }
	INLINE uint16 GetRegionX() { return m_sRegionX; }
	INLINE uint16 GetRegionZ() { return m_sRegionZ; }
	INLINE uint32 GetCoins() { return m_iGold; }
	INLINE uint32 GetInnCoins() { return m_iBank; }
	INLINE uint32 GetLoyalty() { return m_iLoyalty; }
	INLINE uint32 GetMonthlyLoyalty() { return m_iLoyaltyMonthly; }
	INLINE uint32 GetManner() { return m_iMannerPoint; }
	INLINE bool isInGame() { return m_state == GAME_STATE_INGAME; }
	INLINE bool isMerchanting() { return (GetMerchantState() != MERCHANT_STATE_NONE); }
	INLINE bool isSellingMerchant() { return GetMerchantState() == MERCHANT_STATE_SELLING; }
	INLINE bool isBuyingMerchant() { return GetMerchantState() == MERCHANT_STATE_BUYING; }
	INLINE int8 GetMerchantState() { return m_bMerchantState; }

	INLINE bool isSlaveMerchant() { return m_bSlaveMerchant; }
	INLINE int16 GetSlaveGetID() { return m_bSlaveUserID; }

	virtual int32 GetHealth() { return m_sHp; }
	virtual int32 GetMaxHealth() { return m_MaxHP; }
	virtual int32 GetMana() { return m_sMp; }
	virtual int32 GetMaxMana() { return m_MaxMp; }
	INLINE bool  isGenieActive() { return m_bGenieStatus; }
	INLINE uint8 GetFame() { return m_bFame; }
	INLINE bool isKing() { return m_bRank == 1; }
	INLINE uint8 GetZoneID() { return m_bZone; }

	INLINE bool isWarrior() { return JobGroupCheck(ClassWarrior); }
	INLINE bool isRogue() { return JobGroupCheck(ClassRogue); }
	INLINE bool isMage() { return JobGroupCheck(ClassMage); }
	INLINE bool isPriest() { return JobGroupCheck(ClassPriest); }
#if __VERSION > 2024
	INLINE bool isPortuKurian() { return JobGroupCheck(ClassPortuKurian); }
#endif

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

		if (classType < 1 && classType > 19)
		{
			return ClassType(0);
		}

		return classTypes[classType - 1];
	}

	INLINE uint8 GetClassType()
	{
		return GetClass() % 100;
	}

	INLINE uint8 GetRebirthLevel() { return m_reblvl; }

	bool JobGroupCheck(short jobgroupid);
	void Home();
	void SetMaxMp();
	void SetMaxSp();
	void SetMaxHp(int iFlag = 0);
	void LogOut();

	void Add();
	void Remove();

	void SendToRegion(Packet* pkt);
	virtual void GetInOut(Packet& result, uint8 bType);
	void UserInOut(uint8 bType);
	void HandleSurroundingUserRegionUpdate();
	void GetUserInfo(Packet& pkt);
	virtual void Initialize();
	~CBot() {}
	CBot();

};