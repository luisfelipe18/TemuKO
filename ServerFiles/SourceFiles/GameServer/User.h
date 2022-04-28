#pragma once

#include "LuaEngine.h"
#include "../shared/KOSocket.h"

#include "Unit.h"
#include "ChatHandler.h"

struct _KNIGHTS_USER;
struct _EXCHANGE_ITEM;
struct _USER_SEAL_ITEM;

typedef std::map<uint64, _USER_SEAL_ITEM*>	UserItemSealMap;
typedef	std::list<_EXCHANGE_ITEM*>			ItemList;
typedef CSTLMap <_USER_QUEST_INFO, uint16>	QuestMap;
typedef	std::map<uint32, ULONGLONG>			SkillCooldownList;
typedef	std::map<uint32, ULONGLONG>			SkillCastList;
typedef	std::map<uint8, ULONGLONG>			MagicTypeCooldownList;
typedef	std::map<uint16, ULONGLONG>			RHitRepeatList; 
typedef	std::map<uint32, ULONGLONG>			UserSavedMagicMap;

// Time (in seconds) between each save request (10 min).
#define PLAYER_SAVE_INTERVAL			(10 * 60)
// Time (in milliseconds) between each skill request (-1 sec).
#define PLAYER_SKILL_REQUEST_INTERVAL	800 // milliseconds (ms)
// Time (in milliseconds) between each r hit request (-1 sec).
#define PLAYER_R_HIT_REQUEST_INTERVAL	900 // milliseconds (ms)
// Time (in milliseconds) between each skill request (-1 sec).
#define PLAYER_POTION_REQUEST_INTERVAL	2000 // milliseconds (ms)
// Time (in minute) for daily operations
#define DAILY_OPERATIONS_MINUTE			1440
// Time (in seconds) for nation monuments
#define NATION_MONUMENT_REWARD_SECOND	60

// Time (in seconds) for stamina change
#define PLAYER_STAMINA_INTERVAL 2
// Time (in seconds) for training exp change
#define PLAYER_TRAINING_INTERVAL 15
// time genie updated
#define PLAYER_GENIE_INTERVAL	(1 * MINUTE)
//time monster stone updated
#define PLAYER_MONSTER_STONE_INTERVAL	(30 * MINUTE)
#define PLAYER_MONSTER_STONE_KILL_INTERVAL	20

// Time for Check PPCard Serial Time
#define PPCARD_TIME (1 * MINUTE)
enum GameState
{
	GAME_STATE_CONNECTED,
	GAME_STATE_INGAME
};

enum MerchantState
{
	MERCHANT_STATE_NONE		= -1,
	MERCHANT_STATE_SELLING	= 0,
	MERCHANT_STATE_BUYING	= 1
};

enum ClassType
{
	ClassWarrior			= 1,
	ClassRogue				= 2,
	ClassMage				= 3,
	ClassPriest				= 4,
	ClassWarriorNovice		= 5,
	ClassWarriorMaster		= 6,
	ClassRogueNovice		= 7,
	ClassRogueMaster		= 8,
	ClassMageNovice			= 9,
	ClassMageMaster			= 10,
	ClassPriestNovice		= 11,
	ClassPriestMaster		= 12,
	ClassPortuKurian		= 13,
	ClassPortuKurianNovice  = 14,
	ClassPortuKurianMaster	= 15
};

enum WarpListResponse
{
	WarpListGenericError		= 0,
	WarpListSuccess				= 1,  // "You've arrived at."
	WarpListMinLevel			= 2,  // "You need to be at least level <level>."
	WarpListNotDuringCSW		= 3,  // "You cannot enter during the Castle Siege War."
	WarpListNotDuringWar		= 4,  // "You cannot enter during the Lunar War."
	WarpListNeedNP				= 5,  // "You cannot enter when you have 0 national points."
	WarpListWrongLevelDLW		= 6,  // "Only characters with level 30~50 can enter." (dialog) 
	WarpListDoNotQualify		= 7,  // "You can not enter because you do not qualify." (dialog) 
	WarpListRecentlyTraded		= 8,  // "You can't teleport for 2 minutes after trading." (dialog) 
	WarpListArenaFull			= 9,  // "Arena Server is full to capacity. Please try again later." (dialog) 
	WarpListFinished7KeysQuest	= 10, // "You can't enter because you completed Guardian of 7 Keys quest." (dialog) 
};

enum TeamColour
{
	TeamColourNone = 0,
	TeamColourBlue,
	TeamColourRed,
	TeamColourOutside,
	TeamColourMap
};

#define ARROW_EXPIRATION_TIME (2500) // 2.5 seconds

struct Arrow
{
	uint32 nSkillID;
	ULONGLONG tFlyingTime;

	Arrow(uint32 nSkillID, ULONGLONG tFlyingTime) 
	{
		this->nSkillID = nSkillID;
		this->tFlyingTime = tFlyingTime;
	}
};

typedef std::vector<Arrow> ArrowList;
typedef CSTLMap <_USER_ACHIEVE_INFO, uint16>	AchieveMap;
typedef CSTLMap <_PREMIUM_DATA>	PremiumMap;
typedef CSTLMap <_USER_ACHIEVE_TIMED_INFO>	AchieveTimedMap;
typedef CSTLMap <_DELETED_ITEM>	DeletedItemMap;

#include "GameDefine.h"

class CGameServerDlg;
class CUser : public Unit, public KOSocket
{
public:
	virtual uint16 GetID() { return GetSocketID(); }

	ULONGLONG m_tLastSurroundingUpdate;
	ULONGLONG m_iLastExchangeTime;
	bool	m_bCharacterDataLoaded;
	bool m_bIsLoggingOut;
	std::string & GetAccountName() { return m_strAccountID; }
	virtual std::string & GetName() { return m_strUserID; }

	std::string	m_strAccountID, m_strUserID, m_strMemo;

	uint8 floodcounter;
	DWORD m_lastflood;
	DWORD m_mutetime;

	time_t	LastUsePPCard;

	int		m_iDrakiTime;
	uint8	m_bDrakiStage;
	uint8	m_bDrakiEnteranceLimit;
	uint8	m_bRace;
	uint16	m_sClass;
	uint32	m_nHair;
	uint8	m_bRank;
	uint8	m_bTitle;
	int64	m_iExp;	
	uint32	m_iLoyalty,m_iLoyaltyMonthly;
	uint32	m_iMannerPoint;
	uint8	m_bFace;
	uint8	m_bCity;
	int16	m_bKnights;	
	int16	m_bKnightsReq;	
	uint8	m_bFame;
	int16	m_sHp, m_sMp, m_sSp;
	uint8	m_bStats[STAT_COUNT];
	uint8	m_bRebStats[STAT_COUNT];
	uint8	m_bAuthority;
	int16	m_sPoints; // this is just to shut the compiler up
	uint32	m_iGold, m_iBank;
	int16	m_sBind;
	int8    m_sUserUpgradeCount;
	uint8    m_bstrSkill[10];	

	_ITEM_DATA m_sItemArray[INVENTORY_TOTAL];
	_ITEM_DATA m_sItemKnightRoyalArray[INVENTORY_TOTAL];
	_ITEM_DATA m_sWarehouseArray[WAREHOUSE_MAX];
	_ITEM_DATA m_sVIPWarehouseArray[VIPWAREHOUSE_MAX];

	std::string m_bVIPStoragePassword;
	uint8 m_bWIPStorePassowrdRequest;
	uint32 m_bVIPStorageVaultExpiration;

	uint8	m_bLogout;
	uint32	m_dwTime;
	time_t	m_lastSaveTime;
	time_t	m_LastCashTimeCheck;
	time_t	m_LastCashTimeCheck2;
	time_t	m_lastBonusTime;
	time_t	m_TimeOnline;
	time_t	m_tGenieTimeNormal;

	uint8	m_bAccountStatus;
	uint8	m_bPremiumType;
	uint16	m_sPremiumTime;

	bool	m_bWaitingOrder;
	time_t	m_tOrderRemain;

	uint32  m_nKnightCash;
	uint32  m_nKnightCashBonus;

	uint16	m_GenieTime;
	uint8   m_sFirstUsingGenie;
	char	m_GenieOptions[100];
	bool	m_bGenieStatus;

	int16   m_TowerOwnerID;

	/* Vanguard System Start */
	void VanGuard(Packet & pkt);
	void VanGuardZone();
	bool	VanGuardStatus;
	time_t	WantedSystemTime;
	/* Vanguard System End */

	// Ban-Mute Related
	uint32 m_iTimePunishment;

	// Achieve System
	_USER_ACHIEVE_SUMMARY m_AchieveInfo;
	uint16 m_sCoverID, m_sCoverTitle, m_sSkillID, m_sSkillTitle;
	AchieveMap m_AchieveMap;
	AchieveTimedMap m_AchieveTimedMap;

	time_t	m_lastStaminaTime;
	time_t	m_lastTrainingTime;
	uint32  m_iTotalTrainingExp;
	uint32  m_iTotalTrainingTime;

	time_t	m_lastPetSatisfaction, m_AutoMiningFishingSystem, m_OfflineSystemTime;
	PET_DATA* m_PettingOn;

	bool	m_bSelectedCharacter;
	bool	m_bStoreOpen;

	int8	m_bMerchantState;
	bool	m_bSellingMerchantPreparing;
	bool	m_bBuyingMerchantPreparing;
	bool	m_bMerchantOpen;
	uint16	m_bMerchantViewer;
	int16	m_sMerchantsSocketID;
	std::list<uint16>	m_arMerchantLookers;
	_MERCH_DATA	m_arMerchantItems[MAX_MERCH_ITEMS]; //What is this person selling? Stored in "_MERCH_DATA" structure.
	bool	m_bPremiumMerchant;
	UserItemSealMap m_sealedItemMap;
	uint8	m_bRequestingChallenge, // opcode of challenge request being sent by challenger
		m_bChallengeRequested;  // opcode of challenge request received by challengee
	int16	m_sChallengeUser;

	// Event System
	uint32 m_iEventJoinOrder;
	int16 m_sJoinedEvent;

	uint32 m_KnightRoyaleJoinRequest;
	int16 m_KnightRoyaleJoinEvent;

	// Rival system
	int16	m_sRivalID;			// rival's session ID
	time_t	m_tRivalExpiryTime;	// time when the rivalry ends

	// Anger gauge system 
	uint8	m_byAngerGauge; // values range from 0-5

	// Training
	uint32 m_iTrainingTime[2];

	// Magic System Cooldown checks
	SkillCooldownList	m_CoolDownList;
	SkillCastList		m_SkillCastList;
	std::recursive_mutex	m_SkillCastListLock;
	ULONGLONG				t_timeLastPotionUse;

	// Magic System Same time magic type checks
	MagicTypeCooldownList  m_MagicTypeCooldownList;

	// Attack System Cooldown checks
	RHitRepeatList	m_RHitRepeatList;

	ArrowList m_flyingArrows;
	ArrowList m_flyingArrowsSuccess;
	std::recursive_mutex m_arrowLock;
	DeletedItemMap m_sDeletedItemMap;

	bool    m_bOfflineSystemType;

	bool	m_bIsChicken; // Is the character taking the beginner/chicken quest?
	bool	m_bIsHidingHelmet;
	bool	m_bIsHidingCospre;
	bool	m_bIsHidingWings;
	bool	KnightRoyaleStatiMaxHp;
	bool	KnightRoyaleStatiMaxMp;

	uint32  ReturnSymbolisOK;
	int64   ReturnisLogOutTime;
	int64   ReturnSymbolTime;
	uint8	m_bPremiumInUse;
	uint8	m_bClanPremiumInUse;
	PremiumMap	m_PremiumMap;
	uint32   m_iSealedExp;
	uint8	 bExpSealStatus;
	uint8	m_sUserPartyType;

	bool	m_bMining;
	bool	m_bFishing;
	time_t	m_tLastMiningAttempt;
	time_t	m_tLastFishingAttempt;

	bool	m_bSlaveMerchant;
	int16	m_bSlaveUserID;

	int8	m_bPersonalRank;
	int8	m_bKnightsRank;

	float	m_oldx, m_oldy, m_oldz;
	uint16	m_oldwillx, m_oldwillz;
	uint16  curX1, curZ1;
	int16	m_sDirection;

	int64	m_iMaxExp;

	uint32	m_sMaxWeight;
	uint16	m_sMaxWeightBonus;

	int16   m_sSpeed;

	uint8	m_bPlayerAttackAmount;
	uint8	m_bAddWeaponDamage;
	uint16	m_sAddArmourAc; 
	uint8	m_bPctArmourAc;

	int16	m_sItemMaxHp;
	int16	m_sItemMaxMp;
	uint32	m_sItemWeight;
	short	m_sItemAc;
	short	m_sItemHitrate;
	short	m_sItemEvasionrate;

	uint8	m_byAPBonusAmount;
	uint8	m_byAPClassBonusAmount[4]; // one for each of the 4 class types
	uint8	m_byAcClassBonusAmount[4]; // one for each of the 4 class types

	int16	m_sStatItemBonuses[STAT_COUNT];
	int16	m_sStatAchieveBonuses[ACHIEVE_STAT_COUNT];
	bool	AchieveChalengeCount;
	int8	m_bStatBuffs[STAT_COUNT];

	uint8	m_sExpGainAmount;
	uint8	m_bItemExpGainAmount;
	uint8	m_bNPGainAmount, m_bItemNPBonus, m_bSkillNPBonus;
	uint8	m_bNoahGainAmount, m_bItemNoahGainAmount;
	uint8	m_bMaxWeightAmount;
		
	uint8   m_FlashExpBonus;
	uint8   m_FlashDcBonus;
	uint8   m_FlashWarBonus;

	short	m_MaxHp, m_MaxMp, m_MaxSp;

	uint8	m_bResHpType;
	bool	m_bWarp;
	uint8	m_bNeedParty;

	uint16	m_sPartyIndex;
	bool	m_bInParty;
	bool    m_bInEnterParty;
	bool	m_bPartyLeader;

	bool	m_bCanSeeStealth;
	uint8	m_bInvisibilityType;

	short	m_sExchangeUser;
	uint8	m_bExchangeOK;
	bool	m_isRequestSender;
	ItemList	m_ExchangeItemList;

	bool	m_bBlockPrivateChat;
	std::string	m_sPrivateChatUser;

	time_t	m_tHPLastTimeNormal;					// For Automatic HP recovery. 
	time_t	m_tHPStartTimeNormal;
	time_t	m_tSpLastTimeNormal;					// For Automatic SP recovery. 
	time_t	m_tSpStartTimeNormal;
	time_t	m_tPPCardTimeNormal;

	short	m_bHPAmountNormal;
	uint8	m_bHPDurationNormal;
	uint8	m_bHPIntervalNormal;
	uint8	m_bSpIntervalNormal;

	time_t m_tGameStartTimeSavedMagic;

	uint32	m_fSpeedHackClientTime, m_fSpeedHackServerTime;
	uint8	m_bSpeedHackCheck;

	time_t	m_tBlinkExpiryTime;			// When you should stop blinking.

	ULONGLONG   m_tLastArrowUse;

	uint32	m_bAbnormalType;			// Is the player normal, a giant, or a dwarf?
	uint32	m_nOldAbnormalType;

	int16	m_sWhoKilledMe;				// ID of the unit that killed you.
	int64	m_iLostExp;					// Experience points that were lost when you died.

	time_t	m_tLastTrapAreaTime;		// The last moment you were in the trap area.

	bool	m_bZoneChangeFlag;
	bool	m_bZoneChangeControl;

	bool	m_bSuperiorCapeFlag;

	uint8	m_bRegeneType;				// Did you die and go home or did you type '/town'?

	time_t	m_tLastRegeneTime;			// The last moment you got resurrected.

	bool	m_bZoneChangeSameZone;		// Did the server change when you warped?

	bool	m_bHasAlterOptained;
	char	s_FlashNotice[128];

	int		m_iSelMsgEvent[MAX_MESSAGE_EVENT];
	uint8	m_bSelectMsgFlag;
	short	m_sEventNid, m_sEventSid;
	uint32	m_nQuestHelperID;

	bool	m_bWeaponsDisabled;

	TeamColour	m_teamColour;

	bool   m_isRoyalPoison;
	uint8  m_RoyaleForUserRegeneStartedCount;
	uint8  m_RoyaleForUserRegeneRemainingCount;
	uint8  m_RoyaleForUserKillRewarCount;
	uint16 m_RoyaleForUserRankRewarCount;

	uint32 m_BorderDefenceWarUserPoint;
	uint32 m_ChaosExpansionKillCount;
	uint32 m_ChaosExpansionDeadCount;
	uint32 m_PlayerKillingLoyaltyDaily;
	uint16 m_PlayerKillingLoyaltyPremiumBonus;

	uint16    m_ChatRoomIndex;

	uint8 m_GameMastersMute;
	uint8 m_GameMastersUnMute;
	uint8 m_GameMastersUnBan;
	uint8 m_GameMastersBanPermit;
	uint8 m_GameMastersAllowAttack;
	uint8 m_GameMastersDisabledAttack;
	uint8 m_GameMastersNpAdd;
	uint8 m_GameMastersExpAdd;
	uint8 m_GameMastersMoneyAdd;
	uint8 m_GameMastersLoyaltyChange;
	uint8 m_GameMastersExpChange;
	uint8 m_GameMastersMoneyChange;
	uint8 m_GameMastersGiveItem;
	uint8 m_GameMastersGiveItemSelf;
	uint8 m_GameMastersSummonUser;
	uint8 m_GameMastersTpOnUser;
	uint8 m_GameMastersZoneChange;
	uint8 m_GameMastersLocationChange;
	uint8 m_GameMastersMonsterSummon;
	uint8 m_GameMastersNpcSummon;
	uint8 m_GameMastersMonKilled;
	uint8 m_GameMastersTeleportAllUser;
	uint8 m_GameMastersClanSummon;
	uint8 m_GameMastersResetPlayerRanking;
	uint8 m_GameMastersChangeEventRoom;
	uint8 m_GameMastersWarOpen1;
	uint8 m_GameMastersWarOpen2;
	uint8 m_GameMastersWarOpen3;
	uint8 m_GameMastersWarOpen4;
	uint8 m_GameMastersWarOpen5;
	uint8 m_GameMastersWarOpen6;
	uint8 m_GameMastersWarOpen9;
	uint8 m_GameMastersWarSiegeOpen;
	uint8 m_GameMastersWarClose;
	uint8 m_GameMastersCaptainElection;
	uint8 m_GameMastersCastleSiegeWarClose;
	uint8 m_GameMastersSendPrsion;

	bool OpenGmBotSystem;

	float    m_LastX;
	float    m_LastZ;
	int			m_SpeedHackTrial;
	ULONGLONG	m_tLastChatUseTime;
	ULONGLONG	m_tLastType4SkillCheck;


	uint8 m_bRebirthLevel;
public:
	INLINE bool isMeChatRoom(uint16 Room) { return m_ChatRoomIndex == Room; };
	INLINE bool  isGenieActive(){ return m_bGenieStatus;}
	INLINE uint16 GetGenieTime(){ return m_GenieTime; }
	
	INLINE bool isBanned() { return GetAuthority() == AUTHORITY_BANNED; }
	INLINE bool isMuted() { return GetAuthority() == AUTHORITY_MUTED; }
	INLINE bool isAttackDisabled() { return GetAuthority() == AUTHORITY_ATTACK_DISABLED; }
	INLINE bool isGM() { return GetAuthority() == AUTHORITY_GAME_MASTER; }
	INLINE bool isLimitedGM() { return GetAuthority() == AUTHORITY_LIMITED_GAME_MASTER; }

	virtual bool isDead() { return m_bResHpType == USER_DEAD || m_sHp <= 0; }
	virtual bool isBlinking() { return m_bAbnormalType == ABNORMAL_BLINKING; }

	INLINE bool isInGame() { return GetState() == GAME_STATE_INGAME; }

	INLINE bool isInParty() { return m_bInParty && m_bInEnterParty; }
	INLINE bool isInApprovedParty() { return m_bInParty; }
	INLINE bool isPartyLeader() { return isInParty() && m_bPartyLeader; }

	INLINE bool isInClan() { return GetClanID() > 0; }
	INLINE bool isKing() { return m_bRank == 1; }
	INLINE bool isClanLeader() { return GetFame() == CHIEF; }
	INLINE bool isClanAssistant() { return GetFame() == VICECHIEF; }

	INLINE bool isSlaveMerchant() { return m_bSlaveMerchant; }
	INLINE int16 GetSlaveGetID() { return m_bSlaveUserID; }

	INLINE bool isWarrior() { return JobGroupCheck(ClassWarrior); }
	INLINE bool isRogue() { return JobGroupCheck(ClassRogue); }
	INLINE bool isMage() { return JobGroupCheck(ClassMage); }
	INLINE bool isPriest() { return JobGroupCheck(ClassPriest); }
	INLINE bool isPortuKurian() { return JobGroupCheck(ClassPortuKurian); }
	INLINE bool isBeginner() 
	{
		uint16 sClass = GetClassType();
		return (sClass <= ClassPriest 
			|| sClass == ClassPortuKurian);
	}

	INLINE bool isBeginnerWarrior() { return GetClassType() == ClassWarrior; }
	INLINE bool isBeginnerRogue()   { return GetClassType() == ClassRogue; }
	INLINE bool isBeginnerMage()    { return GetClassType() == ClassMage; }
	INLINE bool isBeginnerPriest()  { return GetClassType() == ClassPriest; }
	INLINE bool isBeginnerKurianPortu() { return GetClassType() == ClassPortuKurian; }

	INLINE bool isNovice() 
	{
		uint16 sClass = GetClassType();
		return (sClass == ClassWarriorNovice 
			|| sClass == ClassRogueNovice
			|| sClass == ClassMageNovice 
			|| sClass == ClassPriestNovice 
			|| sClass == ClassPortuKurianNovice);
	}

	INLINE bool isNoviceWarrior() { return GetClassType() == ClassWarriorNovice; }
	INLINE bool isNoviceRogue()   { return GetClassType() == ClassRogueNovice; }
	INLINE bool isNoviceMage()    { return GetClassType() == ClassMageNovice; }
	INLINE bool isNovicePriest()  { return GetClassType() == ClassPriestNovice; }
	INLINE bool isNoviceKurianPortu()  { return GetClassType() == ClassPortuKurianNovice; }

	INLINE bool isMastered() 
	{
		uint16 sClass = GetClassType();
		return (sClass == ClassWarriorMaster 
			|| sClass == ClassRogueMaster 
			|| sClass == ClassMageMaster 
			|| sClass == ClassPriestMaster 
			|| sClass == ClassPortuKurianMaster);
	}

	INLINE bool isMasteredWarrior() { return GetClassType() == ClassWarriorMaster; }
	INLINE bool isMasteredRogue()   { return GetClassType() == ClassRogueMaster; }
	INLINE bool isMasteredMage()    { return GetClassType() == ClassMageMaster; }
	INLINE bool isMasteredPriest()  { return GetClassType() == ClassPriestMaster; }
	INLINE bool isMasteredKurianPortu() { return GetClassType() == ClassPortuKurianMaster; }

	INLINE bool isTrading() { return m_sExchangeUser != -1; }
	INLINE bool isStoreOpen() { return m_bStoreOpen; }
	INLINE bool isMerchanting() { return GetMerchantState() != MERCHANT_STATE_NONE; }
	INLINE bool isSellingMerchantingPreparing() { return m_bSellingMerchantPreparing; }
	INLINE bool isBuyingMerchantingPreparing() { return m_bBuyingMerchantPreparing; }
	INLINE bool isSellingMerchant() { return GetMerchantState() == MERCHANT_STATE_SELLING; }
	INLINE bool isBuyingMerchant() { return GetMerchantState() == MERCHANT_STATE_BUYING; }
	INLINE bool isMining() { return m_bMining; }
	INLINE bool isFishing() { return m_bFishing; }

	INLINE bool isBlockingPrivateChat() { return m_bBlockPrivateChat; }
	INLINE bool isWeaponsDisabled() { return m_bWeaponsDisabled; }
	INLINE bool isTowerOwner() { return GetTowerID() > -1; }
	INLINE bool isOfflineSystem() { return m_bOfflineSystemType; };
	INLINE bool iszonevanguar() { return GetZoneID() == ZONE_RONARK_LAND; }

	INLINE bool isInPKZone() { 
		return GetZoneID() == ZONE_ARDREAM 
			|| GetZoneID() == ZONE_RONARK_LAND 
			|| GetZoneID() == ZONE_RONARK_LAND_BASE; }

	INLINE bool isInEventZone() {  
		return GetZoneID() == ZONE_BORDER_DEFENSE_WAR 
			|| GetZoneID() == ZONE_JURAID_MOUNTAIN 
			|| GetZoneID() == ZONE_CHAOS_DUNGEON; }

	INLINE bool isInEventNoTownZone() {
		return GetZoneID() == ZONE_BORDER_DEFENSE_WAR
			|| GetZoneID() == ZONE_CHAOS_DUNGEON;
	}

	INLINE int8 GetMerchantState() { return m_bMerchantState; }
	INLINE uint8 GetAuthority() { return m_bAuthority; }
	INLINE uint8 GetFame() { return m_bFame; }
	INLINE uint16 GetClass() { return m_sClass; }
	INLINE uint8 GetPremium() { return m_bPremiumInUse; }
	INLINE uint8 GetClanPremium() { return m_bClanPremiumInUse; }

	INLINE bool isLockableScroll(uint8 buffType) { 
		return (buffType == BUFF_TYPE_HP_MP 
			|| buffType == BUFF_TYPE_AC 
			|| buffType == BUFF_TYPE_FISHING 
			|| buffType == BUFF_TYPE_DAMAGE 
			|| buffType == BUFF_TYPE_SPEED 
			|| buffType == BUFF_TYPE_STATS 
			|| buffType == BUFF_TYPE_BATTLE_CRY); }

	INLINE uint8 GetRace() { return m_bRace; }
	INLINE uint8 GetRebirthLevel() { return m_bRebirthLevel; }
	INLINE int16 GetTowerID() { return m_TowerOwnerID; }
	/**
	* @brief	Gets the player's base class type, independent of nation.
	*
	* @return	The class type.
	*/
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
			Disconnect();
			return ClassType(0);
		}

		return classTypes[classType - 1];
	}

	/**
	* @brief	Gets class type, independent of nation.
	*
	* @return	The class type.
	*/
	INLINE uint8 GetClassType(){
		return GetClass() % 100;
	}

	INLINE uint16 GetPartyID() { return m_sPartyIndex; }

	INLINE int16 GetClanID() { return m_bKnights; }
	INLINE void SetClanID(int16 val) { m_bKnights = val; }

	INLINE uint32 GetCoins() { return m_iGold; }
	INLINE uint32 GetInnCoins() { return m_iBank; }
	INLINE uint32 GetLoyalty() { return m_iLoyalty; }
	INLINE uint32 GetMonthlyLoyalty() { return m_iLoyaltyMonthly; }
	INLINE uint32 GetManner() { return m_iMannerPoint; }

	virtual int32 GetHealth() { return m_sHp; }
	virtual int32 GetMaxHealth() { return m_MaxHp; }
	virtual int32 GetMana() { return m_sMp; }
	virtual int32 GetMaxMana() { return m_MaxMp; }
	virtual int32 GetStamina() { return m_sSp; }
	virtual int32 GetMaxStamina() { return m_MaxSp; }

	// Shortcuts for lazy people
	uint8 GetQuestStatus(uint16 sQuestID);
	INLINE bool hasCoins(uint32 amount) { return (GetCoins() >= amount); }
	INLINE bool hasInnCoins(uint32 amount) { return (GetInnCoins() >= amount); }
	INLINE bool hasLoyalty(uint32 amount) { return (GetLoyalty() >= amount); }
	INLINE bool hasMonthlyLoyalty(uint32 amount) { return (GetMonthlyLoyalty() >= amount); }
	INLINE bool hasManner(uint32 amount) { return (GetManner() >= amount); }
	INLINE uint8 GetAngerGauge() { return m_byAngerGauge; }
	INLINE bool hasFullAngerGauge() { return GetAngerGauge() >= MAX_ANGER_GAUGE; }
	INLINE bool hasRival() { return GetRivalID() >= 0; }
	INLINE bool hasRivalryExpired() { return UNIXTIME >= m_tRivalExpiryTime; }
	INLINE int16 GetRivalID() { return m_sRivalID; }
	INLINE GameState GetState() { return m_state; }
	INLINE uint16 GetActiveQuestID() { return 0; }

	uint8 GetBeefRoastVictory();

	uint8 GetClanGrade();
	uint8 GetClanRank();
	uint32 GetClanPoint();
	void SendClanPointChange(int32 nChangeAmount = 0);
	uint8 GetRankReward(bool isMonthly);
	uint8 GetWarVictory();
	uint8 CheckMiddleStatueCapture();
	void MoveMiddleStatue();
	uint8 GetPVPMonumentNation();
	uint8 GetMonsterChallengeTime();
	uint8 GetMonsterChallengeUserCount();
	bool GetUnderTheCastleOpen();
	uint16 GetUnderTheCastleUserCount();
	bool GetJuraidMountainTime();

	bool isBridgeStage1();
	bool isBridgeStage2();
	bool isDevaStage();

	INLINE int16 GetJoinedEvent() { return m_sJoinedEvent; }

	INLINE uint8 GetStat(StatType type)
	{
		if (type >= STAT_COUNT)
			return 0;

		return m_bStats[type];
	}

	INLINE void SetStat(StatType type, uint8 val)
	{
		if (type >= STAT_COUNT)
			return;

		m_bStats[type] = val;
	}

	INLINE int32 GetStatTotal() // NOTE: Shares name with another, but lack-of args should be self-explanatory
	{
		int32 total = 0; // NOTE: this loop should be unrolled by the compiler
		foreach_array (i, m_bStats)
			total += m_bStats[i];
		return total;
	}

	INLINE int16 GetStatAchieveBonus(UserAchieveStatTypes type)
	{
		if (type >= ACHIEVE_STAT_COUNT)
			return 0;

		return m_sStatAchieveBonuses[type];
	}

	INLINE int16 GetStatItemBonus(StatType type)
	{
		ASSERT(type < STAT_COUNT);
		return m_sStatItemBonuses[type];
	}

	INLINE int16 GetStatWithItemBonus(StatType type)
	{
		return GetStat(type) + GetStatItemBonus(type);
	}

	INLINE int32 GetStatItemBonusTotal()
	{
		int32 total = 0; // NOTE: this loop should be unrolled by the compiler
		foreach_array (i, m_sStatItemBonuses)
			total += m_sStatItemBonuses[i];
		return total;
	}

	INLINE int16 GetStatBonusTotal(StatType type)
	{return GetStatBuff(type) + GetRebStatBuff(type) + GetStatItemBonus(type) + GetStatAchieveBonus((UserAchieveStatTypes)type);}

	INLINE uint8 GetRebStatBuff(StatType type)
	{
		if (type >= STAT_COUNT)
		{
			return 0;
		}
		return m_bRebStats[type];
	}

	INLINE void SetRebStatBuff(StatType type, uint8 val)
	{
		if (type >= STAT_COUNT)
			return;

		m_bRebStats[type] = val;
	}

	INLINE int8 GetStatBuff(StatType type)
	{
		ASSERT(type < STAT_COUNT);
		return m_bStatBuffs[type];
	}

	INLINE void SetStatBuffAdd(StatType type, uint8 val)
	{
		ASSERT(type < STAT_COUNT);
		m_bStatBuffs[type] += val;
	}

	INLINE void SetStatBuffRemove(StatType type, uint8 val)
	{
		ASSERT(type < STAT_COUNT);
		m_bStatBuffs[type] -= val;
	}

	INLINE uint32 GetStatBuffTotal()
	{
		uint32 total = 0; // NOTE: this loop should be unrolled by the compiler
		foreach_array (i, m_bStatBuffs)
			total += m_bStatBuffs[i];
		return total;
	}

	INLINE int16 getStatTotal(StatType type){
		return GetStat(type) + GetStatBonusTotal(type);
	}

	INLINE uint16 GetTotalSkillPoints()
	{
		return m_bstrSkill[SkillPointFree] 
			+ m_bstrSkill[SkillPointCat1]
			+ m_bstrSkill[SkillPointCat2] 
			+ m_bstrSkill[SkillPointCat3] 
			+ m_bstrSkill[SkillPointMaster];
	}

	INLINE uint8 GetSkillPoints(SkillPointCategory category)
	{
		if (category < SkillPointFree 
			|| category > SkillPointMaster)
			return 0;

		return m_bstrSkill[category];
	}

	INLINE _ITEM_DATA * GetItem(uint8 pos) 
	{
		if (pos > INVENTORY_TOTAL)
			return false;

		ASSERT(pos < INVENTORY_TOTAL);
		return &m_sItemArray[pos]; 
	}

	INLINE _ITEM_DATA* GetWerehouseItem(uint8 pos)
	{
		if (pos >= WAREHOUSE_MAX)
			return nullptr;

		return &m_sWarehouseArray[pos];
	}

	INLINE _ITEM_DATA * GetKnightRoyaleItem(uint8 pos)
	{
		if (pos > INVENTORY_TOTAL)
			return false;

		ASSERT(pos < INVENTORY_TOTAL);
		return &m_sItemKnightRoyalArray[pos];

	}

	INLINE _ITEM_TABLE * GetItemPrototype(uint8 pos) 
	{
		_ITEM_DATA * pItem;
		ASSERT(pos < INVENTORY_TOTAL);
		return GetItemPrototype(pos, pItem);
	}

	INLINE void ResetTempleEventData()
	{
		m_bEventRoom = 0;
		m_iEventJoinOrder = 0;
		m_sJoinedEvent = -1;
	}

	INLINE void ResetKnightRoyaleEventData() {
		m_KnightRoyaleJoinRequest = 0;
		m_KnightRoyaleJoinEvent = -1;
	}

	INLINE void ResetKnightRoyaleUserRegeneData() {
		m_RoyaleForUserRegeneStartedCount = 0;
		m_RoyaleForUserRegeneRemainingCount = 0;
		m_RoyaleForUserKillRewarCount = 0;
		m_RoyaleForUserRankRewarCount = 0;
		m_isRoyalPoison = false;
	}

	_ITEM_TABLE * GetItemPrototype(uint8 pos, _ITEM_DATA *& pItem);

	INLINE KOMap * GetMap() { return m_pMap; }

	CUser(uint16 socketID, SocketMgr *mgr); 

	virtual void OnConnect();
	virtual void OnDisconnect();
	virtual bool HandlePacket(Packet & pkt);

	void Update();

	virtual void AddToRegion(int16 new_region_x, int16 new_region_z);
	void ExpFlash();
	void DcFlash();
	void WarFlash();
	void HandleBDWCapture(Packet & pkt);
	void HandleTowerPackets(Packet & pkt);
	void TowerExitsFunciton(bool dead = false);
	void HandleGenie(Packet & pkt);
	void GenieNonAttackProgress(Packet & pkt);
	void UpdateGenieTime(uint16 m_sGenieTime);
	void GenieAttackProgress(Packet & pkt);
	void GenieStart();
	void GenieStop();
	void GenieUseGenieSpirint(Packet & pkt);
	void HandleGenieLoadOptions();
	void HandleGenieSaveOptions(Packet & pkt);

	// Surrounding Users Systems
	void HandleSurroundingUserInfo(Packet & pkt);
	void HandleSurroundingUserInfoDetail(Packet & pkt);
	void HandleSurroundingAllUserInfo(uint8 type);
	void HandleSurroundingUserRegionUpdate();
	void MonsterDeathCount();
	void SetRival(CUser * pRival);
	void RemoveRival();
	void SendLoyaltyChange(int32 nChangeAmount = 0, bool bIsKillReward = false, bool bIsBonusTime = false, bool bIsAddLoyaltyMonthly = true);
	void SendKnightCash(uint32 nCashPoint = 0);
	void NativeZoneReturn();
	void KickOutZoneUser(bool home = false, uint8 nZoneID = 21);
	void TrapProcess();
	bool JobGroupCheck(short jobgroupid);
	uint8 GetBaseClass();
	void SendSay(int32 nTextID[8]);
	void SelectMsg(uint8 bFlag, int32 nQuestID, int32 menuHeaderText, int32 menuButtonText[MAX_MESSAGE_EVENT], int32 menuButtonEvents[MAX_MESSAGE_EVENT]);
	bool CheckClass(short class1, short class2 = -1, short class3 = -1, short class4 = -1, short class5 = -1, short class6 = -1);
	bool GiveItem(uint32 nItemID, uint16 sCount = 1, bool send_packet = true, uint32 Time = 0);
	bool GiveWerehouseItem(uint32 nItemID, uint16 sCount = 1, bool mining = true, bool fishing = true, uint32 Time = 0);
	bool GiveItemKnightRoyalReward(uint32 nItemID, uint16 sCount = 1, uint32 Time = 0);
	bool RobItem(uint32 nItemID, uint16 sCount = 1);
	bool RobItem(uint8 bPos, _ITEM_TABLE * pTable, uint16 sCount = 1);
	bool RobAllItemParty(uint32 nItemID, uint16 sCount = 1);
	bool CheckExistItem(int itemid, short count = 1);
	bool CheckExistItemAnd(int32 nItemID1, int16 sCount1, int32 nItemID2, int16 sCount2, int32 nItemID3, int16 sCount3, int32 nItemID4, int16 sCount4, int32 nItemID5, int16 sCount5);
	bool CheckExistSpacialItemAnd(int32 nItemID1, int16 sCount1, int32 nItemID2, int16 sCount2, int32 nItemID3, int16 sCount3, int32 nItemID4, int16 sCount4, int32 nItemID5, int16 sCount5, int32 nItemID6, int16 sCount6, int32 nItemID7, int16 sCount7, int32 nItemID8, int16 sCount8, int32 nItemID9, int16 sCount9, int32 nItemID10, int16 sCount10);
	
	bool GiveItemLua(
		uint32 nGiveItemID1 = 0,
		uint32 nGiveItemCount1 = 1,
		uint32 nGiveItemTime1 = 0,
		uint32 nGiveItemID2 = 0,
		uint32 nGiveItemCount2 = 1,
		uint32 nGiveItemTime2 = 0,
		uint32 nGiveItemID3 = 0,
		uint32 nGiveItemCount3 = 1,
		uint32 nGiveItemTime3 = 0,
		uint32 nGiveItemID4 = 0,
		uint32 nGiveItemCount4 = 1,
		uint32 nGiveItemTime4 = 0,
		uint32 nGiveItemID5 = 0,
		uint32 nGiveItemCount5 = 1,
		uint32 nGiveItemTime5 = 0,
		uint32 nGiveItemID6 = 0,
		uint32 nGiveItemCount6 = 1,
		uint32 nGiveItemTime6 = 0,
		uint32 nGiveItemID7 = 0,
		uint32 nGiveItemCount7 = 1,
		uint32 nGiveItemTime7 = 0,
		uint32 nGiveItemID8 = 0,
		uint32 nGiveItemCount8 = 1,
		uint32 nGiveItemTime8 = 0,
		uint32 nRobItemID1 = 0,
		uint32 nRobItemCount1 = 0,
		uint32 nRobItemID2 = 0,
		uint32 nRobItemCount2 = 0,
		uint32 nRobItemID3 = 0,
		uint32 nRobItemCount3 = 0,
		uint32 nRobItemID4 = 0,
		uint32 nRobItemCount4 = 0,
		uint32 nRobItemID5 = 0,
		uint32 nRobItemCount5 = 0);

	bool CheckExistItemLua(uint32 itemid, uint32 count = 1);
	bool CheckExistItemAndLua(uint32 nItemID1, uint32 sCount1, uint32 nItemID2, uint32 sCount2, uint32 nItemID3, uint32 sCount3, uint32 nItemID4, uint32 sCount4, uint32 nItemID5, uint32 sCount5);
	bool RobItemLua(uint8 bPos, _ITEM_TABLE * pTable, uint16 sCount = 1);
	bool RobItemLua(uint32 nItemID1, uint32 sCount1, uint32 nItemID2, uint32 sCount2, uint32 nItemID3, uint32 sCount3, uint32 nItemID4, uint32 sCount4, uint32 nItemID5, uint32 sCount5);

	bool ItemSmashSystemRobItemCheck(uint8 bPos, uint32 nItemID, uint16 sCount = 1);
	bool BifrostPieceRobItem(uint8 bPos, uint32 nItemID, uint16 sCount = 1);
	bool CraftingShadowPieceRobItem(uint8 bPos, uint32 nItemID, uint16 sCount = 1);
	bool BifrostPieceSmashGiveItem(uint32 nItemID, uint16 sCount = 1);
	bool CraftingRobItem(uint8 ItemPos, uint32 nItemID, uint16 sCount = 1);
	bool CraftingRobItem(uint8 bPos, _ITEM_TABLE * pTable, uint16 sCount = 1);
	int GetBifrostPieceSmashEmptySlot(uint32 nItemID, uint16 sCount);

	uint16 GetItemCount(uint32 nItemID);
	void CheckRespawnScroll();
	bool CheckGiveSlot(uint8 sSlot);
	bool LuaCheckGiveSlot(uint8 sSlot);
	void MonsterStoneTimerScreen(uint16 Time);
	void MonsterStoneQuestTimerScreen(uint16 Time);
	bool CheckWeight(uint32 nItemID, uint16 sCount);
	bool CheckWeight(_ITEM_TABLE * pTable, uint32 nItemID, uint16 sCount);
	bool CheckSkillPoint(uint8 skillnum, uint8 min, uint8 max);
	bool GoldLose(uint32 gold, bool bSendPacket = true);
	bool LoyaltyLose(uint32 loyalty);
	void GoldGain(uint32 gold, bool bSendPacket = true, bool bApplyBonus = false);
	void LuaGoldGain(uint32 gold, bool bSendPacket = true);
	void JackPotNoah(uint32 gold);
	void JackPotExp(int64 iExp);

	void SendChat(uint8 chattype, std::string msg, std::string Sender = ""); //tekli pm

	void SendFlashDCNotice(bool isRemove = false);
	void SendFlashEXPNotice(bool isRemove = false);
	void SendFlashCountNotice(bool isRemove = false);

	void ShowBulletinBoard();
	void ShowKarusRankBoard();
	void ShowElmoradRankBoard();

	//�eriff
	void KingsInspectorList();
	void SheriffVote(Packet & pkt);

	void SendItemWeight();
	void UpdateVisibility(InvisibilityType bNewType);
	void ResetGMVisibility();
	void BlinkStart(int exBlinkTime = 0);
	void BlinkTimeCheck();
	void GoldChange(short tid, int gold);
	CUser * GetItemRoutingUser(uint32 nItemID, uint16 sCount);
	CUser* GetItemRoutingUserLooting(uint32 nItemID, uint16 sCount);
	bool GetStartPosition(short & x, short & y, uint8 bZone = 0);
	bool GetStartPositionRandom(short & x, short & z, uint8 bZone = 0);
	bool GetStartKnightRoyalePositionRandom(short & x, short & z);
	int FindSlotForItem(uint32 nItemID, uint16 sCount = 1);
	int FindWerehouseSlotForItem(uint32 nItemID, uint16 sCount = 1);
	int GetWerehouseEmptySlot();
	int GetEmptySlot();
	void SendAllKnightsID();
	void SendStackChange(uint32 nItemID, uint32 nCount /* needs to be 4 bytes, not a bug */, uint16 sDurability, 
		uint8 bPos, bool bNewItem = false, uint32 Time = 0, uint8 bSlotSection = 1);
	void SpawnEventSystem(uint16 sSid, uint8 bIsmonster, uint8 byZone, float fX, float fY, float fZ);
	void Type4Duration();
	void HPTimeChange();
	void SpTimeChange();
	void HPTimeChangeType3();
	void TrainingProcess();

	short GetDamage(Unit *pTarget, _MAGIC_TABLE *pSkill = nullptr, bool bPreviewOnly = false);
	void OnAttack(Unit * pTarget, AttackType attackType);
	void OnDefend(Unit * pAttacker, AttackType attackType);
	bool TriggerProcItem(uint8 bSlot, Unit * pTarget, ItemTriggerType triggerType);

	void SendDurability(uint8 slot, uint16 durability);
	void SendItemMove(uint8 command, uint8 subcommand);
	void ItemWoreOut( int type, int damage );
	void Dead();
	void GetUserInfoForAI(Packet & result);
	bool ItemEquipAvailable( _ITEM_TABLE* pTable );
	bool ItemClassAvailable( _ITEM_TABLE* pTable );
	
	virtual void HpChange(int amount, Unit *pAttacker = nullptr, bool isDOT = false);
	virtual void SiegeTransformHpChange(int amount);
	virtual void MSpChange(int amount);
	virtual void SpChange(int amount);

	void SendPartyHPUpdate();
	void ShowEffect(uint32 nSkillID);
	void ShowNpcEffect(uint32 nEffectID, bool bSendToRegion = false);
	void SendAnvilRequest(uint16 sNpcID, uint8 bType = ITEM_UPGRADE_REQ);
	void RecastSavedMagic(uint8 buffType = 0);
	void RecastLockableScrolls(uint8 buffType);
	void SetMaxSp();
	void HealMagic();
	void HealAreaCheck(C3DMap * pMap, int rx, int rz);
	// packet handlers start here
	void VersionCheck(Packet & pkt);
	void LoginProcess(Packet & pkt);

	void LoadingLogin(Packet & pkt);
	void AllCharInfoToAgent();
	void AllCharNameChangeInfo(Packet & pkt);
	void AllCharInfo(Packet & pkt);
	void AllCharLocationSendToAgent(Packet & pkt);
	void AllCharLocationRecvToAgent(Packet & pkt);

	void SelNationToAgent(Packet & pkt);
	void ChangeHair(Packet & pkt);
	void NewCharToAgent(Packet & pkt);
	bool NewCharValid(uint8 bRace, uint16 bClass);
	bool NewCharClassVaid(uint16 bClass);
	bool NewCharRaceVaid(uint16 bRace);
	void DelCharToAgent(Packet & pkt);
	void SelCharToAgent(Packet & pkt);
	void SelectCharacter(Packet & pkt); // from the database
	void SetLogInInfoToDB(uint8 bInit);
	void RecvLoginInfo(Packet & pkt); // from the database

	void SpeedHackTime(Packet & pkt);
	void TempleProcess(Packet & pkt );
	bool TempleJoinEvent(int16 sEvent);
	void TempleDisbandEvent(int16 sEvent);

	void MonsterStoneProcess(Packet & pkt);
	void MonsterStoneQuestProcess(uint16 sQuestNum = 0);
	void MonsterStoneExitProcess();
	
	void DrakiTowerList();
	void DrakiTowerTempleEnter(Packet & pkt);
	void SendDrakiTempleDetail(bool MonsterSummon);
	void DrakiRiftChange(uint16 DrakiStage, uint16 DrakiSubStage);
	uint8 SelectDrakiRoom();
	uint8 SelectNpcDrakiRoom();
	void SummonDrakiMonsters(uint8 RoomIndex);
	void DrakiTowerNpcOut();
	void ChangeDrakiMode();
	void DrakiTowerSavedUserInfo();
	void UpdateDrakiRank(uint32 Time, uint8 bRoom);
	void DrakiTowerKickOuts();
	void DrakiTowerKickTimer();
	void DrakiTowerTown();
	void TournamentSendTimer();
	void HackToolList(bool bSender);
	void GameStart(Packet & pkt);
	void RentalSystem(Packet & pkt);
	void SkillDataProcess(Packet & pkt);
	void SkillDataSave(Packet & pkt);
	void SkillDataLoad();
	void MoveProcess(Packet & pkt);
	void Rotate(Packet & pkt);
	void Attack(Packet & pkt);

	void PPCardSystem(Packet& pkt);
	void PPCard(Packet& pkt);

	void ProgramCheckProcess(Packet& pkt);

	static void InitChatCommands();
	static void CleanupChatCommands();

	void Chat(Packet & pkt);
	void ChatTargetSelect(Packet & pkt);
	void SendDeathNotice(Unit * pKiller, DeathNoticeType noticeType, bool isToZone = true);
	bool ProcessChatCommand(std::string & message);

	uint8 GetUserDailyOp(uint8 type = 0);
	void SetUserDailyOp(uint8 type = 0, bool isInsert = false);

	uint32 GetEventTrigger();

	void RemoveStealth();

	void GiveKnightCash(uint32 nKnightCash);
	void RobChaosSkillItems();

	// Nation Transfer, Gender Change and Job Change (in game)
	void HandleNationChange(Packet & pkt);
	void ReqHandleNationChange(Packet & pkt);
	void SendNationTransfer();
	void ReqSendNationTransfer();
	uint8	NationChange();
	uint8	GetNewRace();
	bool GenderChange(uint8 nRace = 0);
	uint8 JobChange(uint8 NewJob = 0);
	void GenderChangeV2(Packet & pkt);

	/* System Entegrasyon*/
	void LifeSkillHuntProcess(uint16 MonsterNum);
	void HOOK_Main();
	void HOOK_SendLifeSkills();
	void HOOK_HpInfo();
	void HOOK_PowerUpStore();
	void HOOK_ItemNotice(uint8 type, uint32 nItemID);
	void HOOK_SendMessageBox(std::string title, std::string message);
	bool HOOK_CashLose(uint32 cash);
	void HOOK_CashGain(uint32 cash);
	void HOOK_SendPartyProcess();
	void CollectionRaceLoadProcess();
	void CollectionRaceReward();
	void CollectionRaceEndProcess();
	void CollectionRaceProcess(uint16 MonsterNum);

	//bool CollectionRaceRegister;
	uint16 CollectionRaceEventMonsterID[CR_EVENT_COUNT];
	uint16 CollectionRaceEventMonsterKillCount[CR_EVENT_COUNT];
	uint16 CollectionRaceEventCount[CR_EVENT_COUNT];
	uint8 CollectionRaceEventZone[CR_EVENT_COUNT];
	std::string CollectionRaceEventMonsterName[CR_EVENT_COUNT];

	struct _LIFE_SKILLS {
		uint64 WarExp;
		uint64 HuntingExp;
		uint64 SmitheryExp;
		uint64 KarmaExp;
	};

	enum LIFE_SKILL {
		WAR = 1,
		HUNTING,
		SMITHERY,
		KARMA
	};

	_LIFE_SKILLS m_sLifeSkills;

	// Life Skills
	struct LF_VEC2
	{
		uint16 LVL;
		float PER;
		LF_VEC2(uint16 _LVL, float _PER)
		{
			LVL = _LVL;
			PER = _PER;
		}
	};

	void LifeSkillControl()
	{
		if (m_sLifeSkills.WarExp > 500000000) 
			m_sLifeSkills.WarExp = 500000000;

		if (m_sLifeSkills.HuntingExp > 500000000) 
			m_sLifeSkills.HuntingExp = 500000000;

		if (m_sLifeSkills.SmitheryExp > 500000000)
			m_sLifeSkills.SmitheryExp = 500000000;

		if (m_sLifeSkills.KarmaExp > 500000000) 
			m_sLifeSkills.KarmaExp = 500000000;

		if (m_sLifeSkills.WarExp < 100)
			m_sLifeSkills.WarExp = 100;

		if (m_sLifeSkills.HuntingExp < 100)
			m_sLifeSkills.HuntingExp = 100;

		if (m_sLifeSkills.SmitheryExp < 100)
			m_sLifeSkills.SmitheryExp = 100;

		if (m_sLifeSkills.KarmaExp < 100)
			m_sLifeSkills.KarmaExp = 100;
	}

	INLINE LF_VEC2 GetLifeSkillLevelFromEXP(int64 EXP)
	{
		double multipier = 1.15;

		if (EXP < 100)
			return LF_VEC2(1, 0);

		// logaritma form�l

		double order = log(EXP / 100) / log(multipier);
		uint16 level = uint16(floor(order));
		float percent = float(order - level) * 100;

		return LF_VEC2(level, percent);
	}

	INLINE LF_VEC2 GetLifeSkillLevel(LIFE_SKILL skill)
	{
		LifeSkillControl();
		switch (skill)
		{
		case LIFE_SKILL::WAR:
			return GetLifeSkillLevelFromEXP(m_sLifeSkills.WarExp);
			break;
		case LIFE_SKILL::HUNTING:
			return GetLifeSkillLevelFromEXP(m_sLifeSkills.HuntingExp);
			break;
		case LIFE_SKILL::SMITHERY:
			return GetLifeSkillLevelFromEXP(m_sLifeSkills.SmitheryExp);
			break;
		case LIFE_SKILL::KARMA:
			return GetLifeSkillLevelFromEXP(m_sLifeSkills.KarmaExp);
			break;
		default:
			return LF_VEC2(1, 0);
			break;
		}
	}

	time_t HOOK_LASTCHECK;
	uint64 HOOK_TICKCOUNT;
	time_t HOOK_LASTSUPPORT;

	/* Packet System*/
	void Handle(Packet & pkt);
	void MonsterInfoHandler(Packet& pkt);
	void HOOK_AuthInfo(Packet& pkt);
	void HOOK_General(Packet& pkt);
	void HOOK_UIRequest(Packet& pkt);
	void HOOK_ReqUserData(Packet& pkt);
	void HOOK_StayAlive(Packet& pkt);
	void HOOK_HandleLifeSkill(Packet& pkt);
	void HOOK_ReqUserInfo(Packet& pkt);
	void HOOK_Support(Packet& pkt);
	void HOOK_ReqMerchantList(Packet& pkt);
	void HOOK_SendTempItems(Packet& pkt);
	void HOOK_DropRequest(Packet& pkt);
	void HOOK_PowerUpStoreRequest(Packet& pkt);
	void HOOK_PowerUpStorePurchase(Packet& pkt);
	void HOOK_AllSkillStatRestard(Packet& pkt);
	void HOOK_ChaoticExchange(Packet& pkt);
	void HOOK_MerchantHandler(Packet& pkt);
	void HOOK_MerchantItemAdd(Packet& pkt);
	void HOOK_VoiceHandler(Packet& pkt);
	void HOOK_LogHandler(Packet& pkt);

	COMMAND_HANDLER(HandleHelpCommand);
	COMMAND_HANDLER(HandleGiveItemCommand);
	COMMAND_HANDLER(HandleZoneChangeCommand);
	COMMAND_HANDLER(HandleMonsterSummonCommand);
	COMMAND_HANDLER(HandleNPCSummonCommand);
	COMMAND_HANDLER(HandleMonKillCommand);
	COMMAND_HANDLER(HandleWar1OpenCommand);
	COMMAND_HANDLER(HandleWar2OpenCommand);
	COMMAND_HANDLER(HandleWar3OpenCommand);
	COMMAND_HANDLER(HandleWar4OpenCommand);
	COMMAND_HANDLER(HandleWar5OpenCommand);
	COMMAND_HANDLER(HandleWar6OpenCommand);
	COMMAND_HANDLER(HandleWarMOpenCommand);
	COMMAND_HANDLER(HandleCaptainCommand);
	COMMAND_HANDLER(HandleSnowWarOpenCommand);
	COMMAND_HANDLER(HandleSiegeWarOpenCommand);
	COMMAND_HANDLER(HandleWarCloseCommand);
	COMMAND_HANDLER(HandleLoyaltyChangeCommand);
	COMMAND_HANDLER(HandleExpChangeCommand);
	COMMAND_HANDLER(HandleGoldChangeCommand);
	COMMAND_HANDLER(HandleExpAddCommand); /* for the server XP event */
	COMMAND_HANDLER(HandleNPAddCommand); /* for the server XP event */
	COMMAND_HANDLER(HandleMoneyAddCommand); /* for the server coin event */
	COMMAND_HANDLER(HandlePermitConnectCommand);
	COMMAND_HANDLER(HandleTeleportAllCommand);
	COMMAND_HANDLER(HandleKnightsSummonCommand);
	COMMAND_HANDLER(HandleWarResultCommand);
	COMMAND_HANDLER(HandleResetPlayerRankingCommand);

	COMMAND_HANDLER(HandleGiveItemSelfCommand);
	COMMAND_HANDLER(HandleBanUnderCommand);
	COMMAND_HANDLER(HandleBanDaysCommand);
	COMMAND_HANDLER(HandleBanScammingCommand);
	COMMAND_HANDLER(HandleBanCheatingCommand);
	COMMAND_HANDLER(HandleSummonUserCommand);
	COMMAND_HANDLER(HandleTpOnUserCommand);
	COMMAND_HANDLER(HandleLocationChange);

	COMMAND_HANDLER(HandleMuteCommand);
	COMMAND_HANDLER(HandleUnMuteCommand);
	COMMAND_HANDLER(HandlePermanentCommand);
	COMMAND_HANDLER(HandleAllowAttackCommand);
	COMMAND_HANDLER(HandleDisableCommand);
	COMMAND_HANDLER(HandleChangeRoom);
	COMMAND_HANDLER(HandleCastleSiegeWarClose);
	COMMAND_HANDLER(HandleSummonPrison);
	COMMAND_HANDLER(HandleServerGameTestCommand);
	COMMAND_HANDLER(HandleServerBotCommand);

	COMMAND_HANDLER(HandleChaosExpansionOpen);
	COMMAND_HANDLER(HandleBorderDefenceWarOpen);
	COMMAND_HANDLER(HandleJuraidMountainOpen);
	COMMAND_HANDLER(HandleKnightRoyaleOpen);

	COMMAND_HANDLER(HandlePmallCommand);

	COMMAND_HANDLER(RemoveAllBots);
	COMMAND_HANDLER(HandleFishingDropTester);
	COMMAND_HANDLER(HandleMiningDropTester);
	COMMAND_HANDLER(HandleMonsterDropTester);

	COMMAND_HANDLER(HandleBossEvent);

	COMMAND_HANDLER(HandleCollectionOpen);
	COMMAND_HANDLER(HandleCollectionInfoMessage);
	COMMAND_HANDLER(HandleCollectionClose);

	COMMAND_HANDLER(HandleOpenArdreamCommand);
	COMMAND_HANDLER(HandleOpenCZCommand);
	COMMAND_HANDLER(HandleCloseCZEventCommand);
	COMMAND_HANDLER(HandleCloseArdreamEventCommand);
	COMMAND_HANDLER(HandleBaseEvent);
	COMMAND_HANDLER(HandleBaseLandCloseCommand);

	COMMAND_HANDLER(HandleReload);
	COMMAND_HANDLER(GmBotSystemOpen);
	COMMAND_HANDLER(HandleAllUpCommand);
	COMMAND_HANDLER(HandleOnlineItemCommand);

	COMMAND_HANDLER(HandleBeefEventOpen);
	COMMAND_HANDLER(HandleBeefEventClose);

	COMMAND_HANDLER(ClearInvCommand);

	void RebirthSystemActivity();

	void Regene(uint8 regene_type, uint32 magicid = 0);
	void RequestUserIn(Packet & pkt);
	void RequestNpcIn(Packet & pkt);
	void RecvWarp(Packet & pkt);
	void Warp(uint16 sPosX, uint16 sPosZ);
	void ItemMove(Packet & pkt);
	void NpcEvent(Packet & pkt);

	void InventorySystem(Packet & pkt);
	void InventorySystemReflesh(Packet & pkt);
	void InventorySystemKnightRoyal(Packet & pkt);
	bool IsValidSlotPosKnightRoyal(_ITEM_TABLE* pTable, int destpos);

	void ItemTrade(Packet & pkt);
	void ItemTradeRepurchase(Packet & pkt);
	void BundleOpenReq(Packet & pkt);
	void ItemGet(Packet & pkt);
	CUser * GetLootUser(_LOOT_BUNDLE * pBundle, _LOOT_ITEM * pItem);
	void ItemGetLooting(uint32 nBundleID, uint32 nItemID, uint16 bBoxSlot);
	CUser* GetLootUserLooting(_LOOT_BUNDLE* pBundle, _LOOT_ITEM* pItem);

	void RecvZoneChange(Packet & pkt);
	void PointChange(Packet & pkt);

	void StateChange(Packet & pkt);
	virtual void StateChangeServerDirect(uint8 bType, uint32 nBuff);



	void PartySystemProcess(Packet & pkt);
	void PartyisDelete();
	void PartyLeaderPromote(uint16 GetLeaderID);
	void PartyNemberRemove(uint16 UserID);
	void AgreeToJoinTheParty();
	void DoNotAcceptJoiningTheParty();
	void PartyCreateRequest(Packet & pkt);
	void PartyInvitationRequest(Packet & pkt);
	void PartyInvitationCheck(uint16 UserSocketID, uint8 PartyType = 0);
	void PartyCreateCheck(uint16 UserSocketID, uint8 PartyType = 0);
	void PartyInsertOrCancel(Packet & pkt);
	void PartyAlert(Packet & pkt);
	void PartyCommand(Packet & pkt);
	void PartyTargetNumber(Packet & pkt);

	void EventPartyCreate();
	void EventPartyInvitationCheck(uint16 EnterPartyGetID);
	void EventPartyInvitation();

	// Trade system
	void ExchangeProcess(Packet & pkt);
	void ExchangeReq(Packet & pkt);
	void ExchangeAgree(Packet & pkt);
	void ExchangeAdd(Packet & pkt);
	void ExchangeDecide();
	void ExchangeCancel(bool bIsOnDeath = false);
	void ExchangeGiveItemsBack();
	void ExchangeFinish();

	
	bool CheckExchange();
	void ExecuteExchange();
	bool CheckExecuteExchange();

	// Merchant system (both types)
	void MerchantProcess(Packet & pkt);
	void GiveMerchantItems();
	void GiveSlaveMerchantItems();
	void MerchantSlaveClose();
	// regular merchants
	void MerchantOpen();
	void MerchantClose();
	void MerchantItemAdd(Packet & pkt);
	void MerchantItemCancel(Packet & pkt);
	void MerchantItemList(Packet & pkt);
	void MerchantItemBuy(Packet & pkt);
	void MerchantInsert(Packet & pkt);
	void CancelMerchant();

	// buying merchants
	void BuyingMerchantOpen(Packet & pkt);
	void BuyingMerchantClose();
	void BuyingMerchantInsert(Packet & pkt);
	void BuyingMerchantInsertRegion();
	void BuyingMerchantList(Packet & pkt);
	void BuyingMerchantBuy(Packet & pkt);

	//Merchant Official List
	void MerchantOfficialList(Packet & pkt);
	void MerchantListSend(Packet & pkt);
	void MerchantListMoveProcess(Packet & pkt);

	void RemoveFromMerchantLookers();

	void SkillPointChange(Packet & pkt);

	void ObjectEvent(Packet & pkt);
	bool BindObjectEvent(_OBJECT_EVENT *pEvent);
	bool GateLeverObjectEvent(_OBJECT_EVENT *pEvent, int nid);
	bool LogLeverBuringLog(_OBJECT_EVENT *pEvent, int nid);
	bool FlagObjectEvent(_OBJECT_EVENT *pEvent, int nid);
	bool WarpListObjectEvent(_OBJECT_EVENT *pEvent);
	bool KrowazGateEvent(_OBJECT_EVENT *pEvent, int nid);

	void UpdateGameWeather(Packet & pkt);

	void ClassChange(Packet & pkt, bool bFromClient = true);
	void ClassChangeReq();
	void SendStatSkillDistribute();
	void AllPointChange(bool bIsFree = false);
	void AllSkillPointChange(bool bIsFree = false);

	void AllPointChange2(bool bIsFree = false);
	void AllSkillPointChange2(bool bIsFree = false);

	void CountConcurrentUser();
	void UserDataSaveToAgent();

	void ItemRepair(Packet & pkt);
	void ItemRemove(Packet & pkt);
	void SendRepurchaseMsg(bool isRefreshed = false);
	void OperatorCommand(Packet & pkt);

	void WarehouseSystem(Packet & pkt);
	void WarehouseProcess(Packet& pkt);
	void ClanWarehouseProcess(Packet& pkt);
	void HShieldClanWarehouseProcess(Packet& pkt);

	void VIPhouseProcess(Packet & pkt);
	void ReqVipStorageProcess(Packet & pkt);
	void ReqConcurrentProcess(Packet & pkt);
	void DailyOpProcess(Packet & pkt);
	void Home();

	void FriendProcess(Packet & pkt);
	void FriendRequest();
	void FriendModify(Packet & pkt, uint8 opcode);
	void RecvFriendModify(Packet & pkt, uint8 opcode);
	void FriendReport(Packet & pkt);
	uint8 GetFriendStatus(std::string & charName, int16 & sid);

	void SelectWarpList(Packet & pkt);
	bool GetWarpList( int warp_group );

	void ServerChangeOk(Packet & pkt);

	void PartyBBS(Packet & pkt);
	void PartyBBSRegister(Packet & pkt);
	void PartyBBSDelete(Packet & pkt);
	void PartyBBSNeeded(Packet & pkt);
	void PartyBBSWanted(Packet & pkt);
	uint8 GetPartyMemberAmount(_PARTY_GROUP *pParty = nullptr);
	void SendPartyBBSNeeded(Packet & pkt);
	void PartyBBSUserLoqOut();
	void PartyBBSPartyChange(Packet & pkt);
	void PartyBBSPartyDelete(Packet & pkt);

	void DungeonDefencePartyBBSRegister(Packet & pkt);
	void DungeonDefencePartyBBSDelete(Packet & pkt);
	void DungeonDefencePartyBBSNeeded(Packet & pkt);
	void DungeonDefencePartyBBSWanted(Packet & pkt);
	void DungeonDefenceSendPartyBBSNeeded(Packet & pkt);
	void DungeonDefencePartyBBSPartyChange(Packet & pkt);
	void DungeonDefencePartyBBSPartyDelete(Packet & pkt);

	void DungeonDefenceSign();
	void ChangeDungeonDefenceStage();
	void DungeonDefenceSendFinishTimer();
	void DungeonDefenceRobItemSkills();

	void ClientEvent(uint16 sNpcID);
	void KissUser();

	void RecvMapEvent(Packet & pkt);
	void RecvSelectMsg(Packet & pkt);
	bool AttemptSelectMsg(uint8 bMenuID, int8 bySelectedReward);

	void BifrostProcess(CUser * pUser);

	// from the client
	void ExchangeSystemProcess(Packet & pkt);
	void ItemUpgrade(Packet & pkt, uint8 nUpgradeType = ITEM_UPGRADE);
	void ItemUpgradeNotice(_ITEM_TABLE * pItem, uint8 UpgradeResult);
	void ItemUpgradeAccessories(Packet & pkt);
	void BifrostPieceProcess(Packet & pkt);
	void SpecialItemExchange(Packet & pkt);
	void ItemSmashExchange(Packet & pkt);
	void ItemUpgradeRebirth(Packet & pkt);
	void ItemSealProcess(Packet & pkt);
	void SealItem(uint8 bSealType, uint8 bSrcPos);
	void HactchingTransformExchange(Packet &pkt);
	void HatchingImageTransformExchange(Packet &pkt);
	void OtherExchangeSystem();
	
	// Character Seal Process Related
	void CharacterSealProcess(Packet & pkt);
	void CharacterSealShowList(Packet &pkt);
	void CharacterSealUseScroll(Packet &pkt);
	void CharacterSealUseRing(Packet &pkt);
	void CharacterSealPreview(Packet &pkt);
	void CharacterSealAchieveList(Packet &pkt);
	void ShowCyperRingItemInfo(Packet & pkt, uint64 nSerialNum);

	void ReqCharacterSealProcess(Packet & pkt);
	void ReqCharacterSealShowList(Packet &pkt);
	void ReqCharacterSealUseScroll(Packet &pkt);
	void ReqCharacterSealUseRing(Packet &pkt);

	void ShoppingMall(Packet & pkt);
	void HandleStoreOpen(Packet & pkt);
	void HandleStoreClose();
	void LetterSystem(Packet & pkt);

	void ReqLetterSystem(Packet & pkt);
	void ReqLetterUnread();
	void ReqLetterList(bool bNewLettersOnly = true);
	void ReqLetterRead(Packet & pkt);
	void ReqLetterSend(Packet & pkt);
	void ReqLetterGetItem(Packet & pkt);
	void ReqLetterDelete(Packet & pkt);
	void ReqLetterGivePremiumItem(Packet & pkt);
	void ReqLetterGiveBeginnerItem(Packet & pkt);

	void HandleNameChange(Packet & pkt);
	void HandlePlayerNameChange(Packet & pkt);
	void HandleSelectCharacterNameChange(Packet & pkt);
	void HandlePlayerClanNameChange(Packet & pkt);
	void SendNameChange();
	void SendClanNameChange();

	void HandleHelmet(Packet & pkt);
	void HandleCapeChange(Packet & pkt);

	void HandleChallenge(Packet & pkt);
	void HandleChallengeRequestPVP(Packet & pkt);
	void HandleChallengeRequestCVC(Packet & pkt);
	void HandleChallengeAcceptPVP(Packet & pkt);
	void HandleChallengeAcceptCVC(Packet & pkt);
	void HandleChallengeCancelled(uint8 opcode);
	void HandleChallengeRejected(uint8 opcode);

	// Ranking System
	// Ranking System
	void BorderDefenceAddPlayerRank();
	void ChaosExpansionAddPlayerRank();
	void PlayerKillingAddPlayerRank();
	void BorderDefenceRemovePlayerRank();
	void ChaosExpansionRemovePlayerRank();
	void PlayerKillingRemovePlayerRank();
	void HandlePlayerRankings(Packet& pkt);
	void HandleRankingPKZone(Packet& pkt);
	void HandleRankingBDW(Packet& pkt);
	void HandleRankingChaosDungeon(Packet& pkt);
	void UpdateChaosExpansionRank();
	void UpdateBorderDefenceWarRank();
	void UpdatePlayerKillingRank();
	uint16 GetPlayerRank(uint8 nRankType);

	void HandleMiningSystem(Packet & pkt);
	void HandleMiningStart(Packet & pkt);
	void HandleMiningAttempt(Packet & pkt);
	void HandleMiningStop(Packet & pkt);
	void HandleFishingStart(Packet & pkt);
	void HandleFishingAttempt(Packet & pkt);
	void HandleFishingStop(Packet & pkt);
	void HandleBettingGame(Packet & pkt);

	void HandleFishingDropTester(uint8 sType, uint8 sHours);
	void HandleMiningDropTester(uint8 sType, uint8 sHours);
	void HandleMonsterDropTester(uint16 sGetID, uint8 sHours);

	// Achievement System
	void HandleUserAchieve(Packet & pkt);
	void HandleUserAchieveGiftRequest(Packet & pkt);
	void HandleUserAchieveUserDetail(Packet & pkt);
	void HandleUserAchieveSummary(Packet & pkt);
	void HandleUserAchieveStart(Packet & pkt);
	void HandleUserAchieveStop(Packet & pkt);
	void HandleUserAchieveCoverTitle(Packet & pkt);
	void HandleUserAchieveSkillTitle(Packet & pkt);
	void HandleUserAchieveCoverTitleReset(Packet & pkt);
	void HandleUserAchieveSkillTitleReset(Packet & pkt);

	void UpdateAchieve(uint16 sIndex, uint8 Status);
	bool CheckAchieveStatus(uint16 sIndex, uint16 Status);
	void AchieveMonsterCountAdd(uint16 sNpcID);
	void AchieveWarCountAdd(UserAchieveWarTypes type, uint16 sNpcID = 0, CUser *pTarget = nullptr);
	void AchieveNormalCountAdd(UserAchieveNormalTypes type, uint16 sNpcID = 0, CUser *pTarget = nullptr);
	void AchieveComCountAdd(UserAchieveComTypes type, uint16 sNpcID = 0, CUser *pTarget = nullptr);

	void AchieveWarCheck(_USER_ACHIEVE_INFO *pAchieveData, _ACHIEVE_WAR *pAchieveWar, _ACHIEVE_MAIN *pAchieveMain, uint16 sNpcID = 0, CUser *pTarget = nullptr);
	void AchieveNormalCheck(_USER_ACHIEVE_INFO *pAchieveData, _ACHIEVE_NORMAL *pAchieveNormal, _ACHIEVE_MAIN *pAchieveMain, uint16 sNpcID = 0, CUser *pTarget = nullptr);
	bool AchieveComCheck(_USER_ACHIEVE_INFO *pAchieveData, _ACHIEVE_COM *pAchieveCom, _ACHIEVE_MAIN *pAchieveMain, uint16 sNpcID = 0, CUser *pTarget = nullptr);
	void AchieveMonsterCheck(_USER_ACHIEVE_INFO *pAchieveData, _ACHIEVE_MONSTER *pAchieveMonster, _ACHIEVE_MAIN *pAchieveMain, uint16 sNpcID);

	void SendGenieStart(bool isToRegion = false);
	void SendGenieStop(bool isToRegion = false);
	void HandleGenieStart();
	void HandleGenieStop();

	void HandleSoccer(Packet & pkt);

	void SendNotice();
	void SendFlashNotice(bool isRemove = false);
	void TopSendNotice();
	void AppendNoticeEntry(Packet & pkt, uint8 & elementCount, const char * message, const char * title);
	void AppendNoticeEntryOld(Packet & pkt, uint8 & elementCount, const char * message);
	void AppendExtraNoticeData(Packet & pkt, uint8 & elementCount);
	void UserLookChange( int pos, int itemid, int durability );
	void SpeedHackUser();
	void LoyaltyChange(int16 tid, uint16 bonusNP = 0);
	void LoyaltyDivide(int16 tid, uint16 bonusNP = 0);
	int16 GetLoyaltyDivideSource(uint8 totalmember = 0);
	int16 GetLoyaltyDivideTarget();
	void GrantChickenManner();
	void SendMannerChange(int32 iMannerPoints);
	void UserRegionChat(Packet & pkt);
	bool CanLevelQualify(uint8 sLevel);
	bool CanChangeZone(C3DMap * pTargetMap, WarpListResponse & errorReason);
	void ZoneChange(uint16 sNewZone, float x, float z);
	void ZoneChangeParty(uint16 sNewZone, float x, float z);
	void ZoneChangeClan(uint16 sNewZone, float x, float z);

	bool isEventUser();
	bool isSoccerEventUser();
	bool isQuestEventUser();
	bool isInSoccerEvent();
	void isEventSoccerMember(uint8 TeamColours, float x, float z);
	void isEventSoccerStard();
	void isEventSoccerEnd();

	void wallhack();
	void BannedWallHack();
	void BannedSpeedHack();

	// Premium Related Methods
	void HandlePremium(Packet& pkt);
	void SendPremiumInfo();
	void SendClanPremiumInfo();
	void GivePremium(uint8 bPremiumType, uint16 sPremiumTime);
	void GiveClanPremium(uint8 bPremiumType, uint16 sPremiumTime);
	void GivePremiumItem(uint8 bPremiumType);

	// Pet System
	void MainPetProcess(Packet& pkt);
	void SelectingModeFunction(Packet& pkt);
	void HandlePetUseSkill(Packet& pkt);
	void ShowPetItemInfo(Packet& pkt, uint64 nSerialNum);
	void PetSpawnProcess(bool LevelUp = false);
	void LootingPet(float x, float z);
	void SendPetHP(int tid, int damage = 0);
	void SendPetHpChange(int tid, int damage = 0);
	void SendPetExpChange(int32 iExp, int tid = -1);
	void SendPetMSpChange(int tid, int damage = 0);
	void SetPetInfo(PET_DATA * pPet, int tid = -1);
	void PetFeeding(Packet& pkt, uint8 bType);
	void PetSatisFactionUpdate(int16 amount);
	void PetOnDeath();
	void PetHome(uint16 x, uint16 y, uint16 z);

	//HackTool
	void HandleHackTool(Packet &pkt);

	void HandleTargetHP(Packet & pkt);
	void SendTargetHP( uint8 echo, int tid, int damage = 0 );
	bool IsValidSlotPos( _ITEM_TABLE* pTable, int destpos );
	void SetUserAbility(bool bSendPacket = true);
	float SetCoefficient();
	void LevelChange(uint8 level, bool bLevelUp = true);
	void SetSlotItemValue();
	void ApplySetItemBonuses(_SET_ITEM * pItem);
	void SendTime();
	void SendWeather();
	void SetZoneAbilityChange(uint16 sNewZone);
	void SetMaxMp();
	void SetMaxHp(int iFlag = 0);
	void RecvUserExp(CNpc* pNpc, uint32 iDamage, uint32 iTotalDamage);
	void ExpChange(int64 iExp, bool bIsBonusReward = false);
	void LogOut();
	void SendMyInfo();
	void SendInfo();
	void SendServerChange(std::string & ip, uint8 bInit);
	uint16 GetPremiumProperty(PremiumPropertyOpCodes type);
	uint16 GetClanPremiumProperty(PremiumPropertyOpCodes type);
	void CastleSiegeWarProcess(CUser * 
	
	);
	void SiegeWarFareProcess(Packet & pkt);
	void LogosShout(Packet & pkt);
	void EventTrapProcess(float x, float z, CUser * pUser);
	virtual void GetInOut(Packet & result, uint8 bType);
	void UserInOut(uint8 bType);
	void GmGetInOut(Packet& result, uint8 bType);
	void GmInOut(uint8 bType);
	uint8 CastleSiegeWarDeathmacthRegister();
	uint8 CastleSiegeWarDeathmacthCancelRegister();
	void CastleSiegeWarRegisterClanShow();
	void CastleSiegeWarClanRankShow();
	void UpateCSWTotalKillCounts();
	void UpateCSWTotalBarracksKillCounts();
	void CastleSiegeWarBaseCreate();
	void CswUserToolUserNotice(uint8 NoticeType = -1);
	void CswUserToolUserFlag();
	void CswUserToolKickOutUser();
	void CswUserToolTownUser();
	void CastleSiegeWarFlag();

	void GetUserInfo(Packet & pkt);
	void SendUserStatusUpdate(UserStatus type, UserStatusBehaviour status);
	virtual void Initialize();

	//Bdw Jr Monument
	void BDWMonumentPointProcess();
	void BDWAltarScreenAndPlayerPointChange();
	void BDWDeath();
	void BDWUserHasObtainedGameLoqOut();
	void BDWUpdateRoomKillCount();
	void JRUpdateRoomKillCount();
	void ExpSealHandler(Packet & pkt);
	void ExpSealUpdateStatus(uint8 status);
	void ExpSealChangeExp(uint64 amount);
	void ExpSealSealedPotion(Packet & pkt);
	void KnightRoyaleProcess(Packet &pkt);
	void KnightRoyaleLoginSendPacket();
	void KnightRoyaleSurvivorLogOut();
	void KnightRoyaleisDeathProcess();
	void KnightRoyaleRegeneProcess();
	void KnightRoyaleisKillerProcess();
	void KnightRoyaleisWinner();
	void KnightRoyaleExpChange(uint16 iExp);
	void KnightRoyaleLevelChange(uint8 level, bool bLevelUp = true);
	void KnightRoyaleLoqOutPacket();
	void KnightRoyaleDeathChest();
	void KnightRoyaleGasinDamage();
	void KnightRoyaleSendGas(bool Gas = false);
	void KnightRoyaleAreaControl();
	void KnightRoyaleHealtMagicChange();
	void ItemGetKnightRoyal(Packet &pkt);
	void ItemRemoveKnightRoyal(Packet &pkt);
	void KnightReturnGiveLetterItem();
	bool KnightRoyalAlan1_1();
	bool KnightRoyalAlan2_1();
	bool KnightRoyalAlan3_1();
	bool KnightRoyalAlan4_1();
	bool KnightRoyalAlan5_1();
	void KnightReturnLetterItemDeleted();
	void OreadsZoneTerrainEvent();
	void UserWallCheatCheckRegion();
	void ChangeFame(uint8 bFame);
	void SendServerIndex();

	void SendToRegion(Packet *pkt, CUser *pExceptUser = nullptr, uint16 nEventRoom = 0);
	void SendToZone(Packet *pkt, CUser *pExceptUser = nullptr, uint16 nEventRoom = 0, float fRange = 0.0f);

	virtual void OnDeath(Unit *pKiller);
	void OnDeathitDoesNotMatter();
	void OnDeathKilledPlayer(CUser* pKiller);
	void OnDeathKilledNpc(CNpc* pKiller);
	void InitOnDeath(Unit *pKiller);
	void UpdateAngerGauge(uint8 byAngerGauge);
	void InitializeStealth();

	// Exchange system
	bool BifrostCheckExchange(int nExchangeID);
	bool CheckExchange(int nExchangeID);
	bool CheckExchangeExp(int nExchangeID, int32 selectedAward = -1, uint8 giveAllExp = 0);
	bool RunExchange(int nExchangeID, uint16 count = 0 /* No random flag */,bool isQuest = false, int32 SelectedAward = -1, uint16 giveCount = 1);
	bool RunQuestExchange(int nExchangeID, int32 selectedAward = -1, uint8 giveAllExp = 0);
	uint16 GetMaxExchange(int nExchangeID);	

	bool RunCountExchange(int nExchangeID);
	bool RunCountCheckExchange(int nExchangeID);

	bool RunGiveItemExchange(int nExchangeID);
	bool RunGiveItemCheckExchange(int nExchangeID);
	bool RunGiveCheckExistItem(int itemid, short count = 1);
	bool RunGiveCheckExistItemAnd(
		int32 nItemID1, int16 sCount1, int32 nItemID2, int16 sCount2, int32 nItemID3, int16 sCount3, int32 nItemID4, int16 sCount4, int32 nItemID5, int16 sCount5,
		int32 nItemID6, int16 sCount6, int32 nItemID7, int16 sCount7, int32 nItemID8, int16 sCount8, int32 nItemID9, int16 sCount9, int32 nItemID10, int16 sCount10);

	bool LuaCheckExchange(int nLuaExchangeID);
	void MiningExchange(uint16 ID);

	// Clan system
	void SendClanUserStatusUpdate(bool bToRegion = true);
	
	// Party system
	void SendPartyStatusUpdate(uint8 bStatus, uint8 bResult = 0);

	//Vaccuni system
	bool VaccuniBoxExp(uint16 MonsterType = 0);
	bool VaccuniAttack();
	int GetExpPercent();
	bool CanUseItem(uint32 nItemID, uint16 sCount = 1);

	void CheckSavedMagic();
	virtual void InsertSavedMagic(uint32 nSkillID, uint16 sDuration);
	virtual void RemoveSavedMagic(uint32 nSkillID);
	virtual bool HasSavedMagic(uint32 nSkillID);
	virtual int16 GetSavedMagicDuration(uint32 nSkillID);

	void SaveEvent(uint16 sQuestID, uint8 bQuestState);
	void DeleteEvent(uint16 sQuestID);
	bool CheckExistEvent(uint16 sQuestID, uint8 bQuestState);

	void QuestV2MonsterCountAdd(uint16 sNpcID);
	uint8 QuestV2CheckQuestFinished(uint16 sQuestID);
	uint8 QuestV2CheckMonsterCount(uint16 sQuestID, uint8 sRate);

	void QuestV2SaveEvent(uint16 sQuestID);
	void QuestV2SendNpcMsg(uint32 nQuestID, uint16 sNpcID);
	void QuestV2ShowGiveItem(uint32 nUnk1, uint32 sUnk1, 
		uint32 nUnk2, uint32 sUnk2,
		uint32 nUnk3, uint32 sUnk3,
		uint32 nUnk4, uint32 sUnk4,
		uint32 nUnk5 = 0, uint32 sUnk5 = 0);
	uint16 QuestV2SearchEligibleQuest(uint16 sNpcID);
	void QuestV2ShowMap(uint32 nQuestHelperID);

	// Sends the quest completion statuses
	void QuestDataRequest(bool gamestarted = false);
	void QuestOpenSkillRequest();

	// Handles new quest packets
	void QuestV2PacketProcess(Packet & pkt);
	void QuestV2RemoveEvent(uint16 sQuestID);
	void QuestV2MonsterDataRequest(uint16 sQuestID);
	void QuestV2ExecuteHelper(_QUEST_HELPER * pQuestHelper);
	void QuestV2CheckFulfill(_QUEST_HELPER * pQuestHelper);
	bool QuestV2RunEvent(_QUEST_HELPER * pQuestHelper, uint32 nEventID, int8 bSelectedReward = -1);

	bool PromoteUserNovice();
	bool PromoteUser();
	void PromoteClan(ClanTypeFlag byFlag);

	// CheatRoom
	void ChatRoomHandle(Packet & pkt);
	void ChatRoomCreate(Packet & pkt);
	void ChatRoomList(Packet & pkt);
	void ChatRoomJoin(Packet & pkt);
	void ChatRoomLeave(Packet & pkt);
	void ChatRoomKickOut(Packet & pkt);
	void ChatRoomKickOut(uint16 userID);
	void ChatRoomChat(std::string * strMessage, std::string strSender);
	void ChatRoomAdmin(Packet & pkt);
	void ChatRoomMemberoption(Packet & pkt);
	void ChatRoomChangeAdmin(Packet & pkt);
	void SendChatRoom(Packet & pkt);

	// Attack/zone checks
	bool isHostileTo(Unit * pTarget);
	bool isMoral2Checking(Unit* pTarget, _MAGIC_TABLE* pSkill);
	bool isMoral7Checking(Unit* pTarget, _MAGIC_TABLE* pSkill);

	bool isInArena();
	bool isInPVPZone();
	bool isInEnemySafetyArea();
	bool isInOwnSafetyArea();

	bool isInClanArena();

	bool isInMeleeArena();
	bool isInPartyArena();

	bool SendPrisonZoneArea();

	void ResetWindows();

	void CloseProcess();
	virtual ~CUser() {}

	/* Database requests */
	void ReqAccountLogIn(Packet & pkt);
	void ReqSelectNation(Packet & pkt);
	void ReqAllCharInfo(Packet & pkt);
	void ReqChangeHair(Packet & pkt);
	void ReqCreateNewChar(Packet & pkt);
	void ReqDeleteChar(Packet & pkt);
	void ReqSelectCharacter(Packet & pkt);
	void ReqSaveCharacter();
	void ReqUserLogOut();
	void ReqNationTransfer(Packet & pkt);
	void ReqRegisterClanSymbol(Packet & pkt);
	void ReqSetLogInInfo(Packet & pkt);
	void ReqRemoveCurrentUser(Packet & pkt);
	void ReqUserKickOut(Packet & pkt);
	void BattleEventResult(Packet & pkt);
	void ReqShoppingMall(Packet & pkt);
	void ReqLoadWebItemMall();
	void ReqSkillDataProcess(Packet & pkt);
	void ReqSkillDataSave(Packet & pkt);
	void ReqSkillDataLoad(Packet & pkt);
	void ReqFriendProcess(Packet & pkt);
	void ReqRequestFriendList(Packet & pkt);
	void ReqAddFriend(Packet & pkt);
	void ReqRemoveFriend(Packet & pkt);
	void NameChangeSystem(Packet & pkt);
	void ReqChangeCape(Packet & pkt);
	void ReqSealItem(Packet & pkt);
	void ReqItemUpgrade(Packet &pkt);

	// NPoint Related
	void GiveNPoints(uint16 sNPointAmount);
	void ReqHandleNPoints(Packet & pkt);

	//npc kill
	void KillNpcEvent();
	void NpcEventSystem(uint32 m_iSellingGroup);

	void DelosCasttellanZoneOut();
	bool isCswWinnerNembers();
	bool CanEnterDelos();

	void ChangePosition();

	//private:
	static ChatCommandTable s_commandTable;
	GameState m_state;

	// users quest map. this should hold all the info needed. 
	// the quest kill count for each quest, 
	QuestMap m_questMap;

	UserSavedMagicMap m_savedMagicMap;
	std::recursive_mutex m_savedMagicLock;

	_KNIGHTS_USER * m_pKnightsUser;

public:
	DECLARE_LUA_CLASS(CUser);

	// Standard getters
	DECLARE_LUA_GETTER(GetName)
	DECLARE_LUA_GETTER(GetAccountName)
	DECLARE_LUA_GETTER(GetZoneID)
	DECLARE_LUA_GETTER(GetX)
	DECLARE_LUA_GETTER(GetY)
	DECLARE_LUA_GETTER(GetZ)
	DECLARE_LUA_GETTER(GetNation)
	DECLARE_LUA_GETTER(GetLevel)
	DECLARE_LUA_GETTER(GetRebirthLevel)
	DECLARE_LUA_GETTER(GetClass)
	DECLARE_LUA_GETTER(GetCoins)
	DECLARE_LUA_GETTER(GetInnCoins)
	DECLARE_LUA_GETTER(GetLoyalty)
	DECLARE_LUA_GETTER(GetMonthlyLoyalty)
	DECLARE_LUA_GETTER(GetManner)
	DECLARE_LUA_GETTER(GetActiveQuestID)
	DECLARE_LUA_GETTER(GetClanGrade)
	DECLARE_LUA_GETTER(GetClanPoint)
	DECLARE_LUA_GETTER(GetClanRank)
	DECLARE_LUA_GETTER(isWarrior)
	DECLARE_LUA_GETTER(isRogue)
	DECLARE_LUA_GETTER(isMage)
	DECLARE_LUA_GETTER(isPriest)
	DECLARE_LUA_GETTER(isPortuKurian)
	DECLARE_LUA_GETTER(isBeginner)
	DECLARE_LUA_GETTER(isBeginnerWarrior)
	DECLARE_LUA_GETTER(isBeginnerRogue)
	DECLARE_LUA_GETTER(isBeginnerMage)
	DECLARE_LUA_GETTER(isBeginnerPriest)
	DECLARE_LUA_GETTER(isBeginnerKurianPortu)
	DECLARE_LUA_GETTER(isNovice)
	DECLARE_LUA_GETTER(isNoviceWarrior)
	DECLARE_LUA_GETTER(isNoviceRogue)
	DECLARE_LUA_GETTER(isNoviceMage)
	DECLARE_LUA_GETTER(isNovicePriest)
	DECLARE_LUA_GETTER(isNoviceKurianPortu)
	DECLARE_LUA_GETTER(isMastered)
	DECLARE_LUA_GETTER(isMasteredWarrior)
	DECLARE_LUA_GETTER(isMasteredRogue)
	DECLARE_LUA_GETTER(isMasteredMage)
	DECLARE_LUA_GETTER(isMasteredPriest)
	DECLARE_LUA_GETTER(isMasteredKurianPortu)
	DECLARE_LUA_GETTER(isInClan)
	DECLARE_LUA_GETTER(isClanLeader)
	DECLARE_LUA_GETTER(isInParty)
	DECLARE_LUA_GETTER(isPartyLeader)
	DECLARE_LUA_GETTER(isKing)
	DECLARE_LUA_GETTER(GetBeefRoastVictory)
	DECLARE_LUA_GETTER(GetPartyMemberAmount)
	DECLARE_LUA_GETTER(GetPremium)
	DECLARE_LUA_GETTER(GetWarVictory)
	DECLARE_LUA_GETTER(GetMonsterChallengeTime)
	DECLARE_LUA_GETTER(GetMonsterChallengeUserCount)
	DECLARE_LUA_GETTER(GetUnderTheCastleOpen)
	DECLARE_LUA_GETTER(GetUnderTheCastleUserCount)
	DECLARE_LUA_GETTER(GetJuraidMountainTime)
	DECLARE_LUA_GETTER(GetRace)
	DECLARE_LUA_GETTER(CastleSiegeWarDeathmacthRegister)
	DECLARE_LUA_GETTER(CastleSiegeWarDeathmacthCancelRegister)

	// Shortcuts for lazy people
	DECLARE_LUA_FUNCTION(GetQuestStatus) {
		LUA_RETURN(LUA_GET_INSTANCE()->GetQuestStatus(LUA_ARG(uint16, 2)));
	}

	DECLARE_LUA_FUNCTION(hasCoins)  {
		LUA_RETURN(LUA_GET_INSTANCE()->hasCoins(LUA_ARG(uint32, 2))); 
	}

	DECLARE_LUA_FUNCTION(hasInnCoins) {
		LUA_RETURN(LUA_GET_INSTANCE()->hasInnCoins(LUA_ARG(uint32, 2))); 
	}

	DECLARE_LUA_FUNCTION(hasLoyalty) {
		LUA_RETURN(LUA_GET_INSTANCE()->hasLoyalty(LUA_ARG(uint32, 2))); 
	}

	DECLARE_LUA_FUNCTION(hasMonthlyLoyalty) {
		LUA_RETURN(LUA_GET_INSTANCE()->hasMonthlyLoyalty(LUA_ARG(uint32, 2)));
	}

	DECLARE_LUA_FUNCTION(hasManner) {
		LUA_RETURN(LUA_GET_INSTANCE()->hasManner(LUA_ARG(uint32, 2))); 
	}

	// The useful method wrappers
	DECLARE_LUA_FUNCTION(GiveItem) {
		LUA_RETURN(LUA_GET_INSTANCE()->GiveItem(
			LUA_ARG(uint32, 2), 
			LUA_ARG_OPTIONAL(uint16, 1, 3), 
			true,
			LUA_ARG_OPTIONAL(uint32, 0, 4)));
	}

	DECLARE_LUA_FUNCTION(GiveItemLua) {
		LUA_RETURN(LUA_GET_INSTANCE()->GiveItemLua(
			LUA_ARG(uint32, 2),
			LUA_ARG(uint32, 3),
			LUA_ARG(uint32, 4),

			LUA_ARG(uint32, 5),
			LUA_ARG(uint32, 6),
			LUA_ARG(uint32, 7),

			LUA_ARG(uint32, 8),
			LUA_ARG(uint32, 9),
			LUA_ARG(uint32, 10),

			LUA_ARG(uint32, 11),
			LUA_ARG(uint32, 12),
			LUA_ARG(uint32, 13),

			LUA_ARG(uint32, 14),
			LUA_ARG(uint32, 15),
			LUA_ARG(uint32, 16),

			LUA_ARG(uint32, 17),
			LUA_ARG(uint32, 18),

			LUA_ARG(uint32, 19),
			LUA_ARG(uint32, 20),

			LUA_ARG(uint32, 21),
			LUA_ARG(uint32, 22),

			LUA_ARG(uint32, 23),
			LUA_ARG(uint32, 24),

			LUA_ARG(uint32, 25),
			LUA_ARG(uint32, 26)));
	}

	DECLARE_LUA_FUNCTION(RobItem) {
		LUA_RETURN(LUA_GET_INSTANCE()->RobItem(
			LUA_ARG(uint32, 2), 
			LUA_ARG_OPTIONAL(uint16, 1, 3)));
	}

	DECLARE_LUA_FUNCTION(RobAllItemParty) {
		LUA_RETURN(LUA_GET_INSTANCE()->RobItem(
			LUA_ARG(uint32, 2), 
			LUA_ARG_OPTIONAL(uint16, 1, 3)));
	}

	DECLARE_LUA_FUNCTION(CheckExistItem) {
		LUA_RETURN(LUA_GET_INSTANCE()->CheckExistItem(
			LUA_ARG(uint32, 2), 
			LUA_ARG_OPTIONAL(uint16, 1, 3)));
	}

	DECLARE_LUA_FUNCTION(GoldGain) {
		LUA_NO_RETURN(LUA_GET_INSTANCE()->LuaGoldGain(LUA_ARG(int32, 2)));
	}

	DECLARE_LUA_FUNCTION(GoldLose) {
		LUA_NO_RETURN(LUA_GET_INSTANCE()->GoldLose(LUA_ARG(uint32, 2)));	
	}

	DECLARE_LUA_FUNCTION(RunGiveItemExchange) {
		LUA_NO_RETURN(LUA_GET_INSTANCE()->RunGiveItemExchange(LUA_ARG(uint32, 2)));
	}

	DECLARE_LUA_FUNCTION(ExpChange) {
		LUA_NO_RETURN(LUA_GET_INSTANCE()->ExpChange(LUA_ARG(int32, 2)));	
	}

	DECLARE_LUA_FUNCTION(SaveEvent) {
		LUA_NO_RETURN(LUA_GET_INSTANCE()->QuestV2SaveEvent(
			LUA_ARG(uint16, 2)));  // quest ID
	}

	DECLARE_LUA_FUNCTION(SearchQuest) {
		CUser * pUser = LUA_GET_INSTANCE();
		LUA_RETURN(pUser->QuestV2SearchEligibleQuest(LUA_ARG_OPTIONAL(uint16, pUser->m_sEventSid, 2))); // NPC ID
	}

	DECLARE_LUA_FUNCTION(ShowMap) {
		CUser * pUser = LUA_GET_INSTANCE();
		LUA_NO_RETURN(pUser->QuestV2ShowMap(LUA_ARG_OPTIONAL(uint32, pUser->m_nQuestHelperID, 2))); // quest helper ID
	}

	DECLARE_LUA_FUNCTION(CountMonsterQuestSub) {
		LUA_RETURN(LUA_GET_INSTANCE()->QuestV2CheckMonsterCount(LUA_ARG(uint16, 2), LUA_ARG(uint8, 3)));
	}

	DECLARE_LUA_FUNCTION(QuestCheckQuestFinished) {
		LUA_RETURN(LUA_GET_INSTANCE()->QuestV2CheckQuestFinished((LUA_ARG(uint16, 2))));
	}

	DECLARE_LUA_FUNCTION(QuestCheckExistEvent) { // arg1:eventDataIndex arg2:eventStatus 
		LUA_RETURN(LUA_GET_INSTANCE()->CheckExistEvent((LUA_ARG(uint16, 2)), (LUA_ARG(uint8, 3))));
	}

	DECLARE_LUA_FUNCTION(CountMonsterQuestMain) {
		LUA_NO_RETURN(LUA_GET_INSTANCE()->QuestV2MonsterCountAdd((LUA_ARG(uint16, 2))));
	} 

	DECLARE_LUA_FUNCTION(NpcSay) {
		CUser * pUser = LUA_GET_INSTANCE();
		uint32 arg = 2; // start from after the user instance.
		int32 nTextID[8]; 

		foreach_array(i, nTextID)
			nTextID[i] = LUA_ARG_OPTIONAL(int32, -1, arg++);

		LUA_NO_RETURN(pUser->SendSay(nTextID));
	}

	// This is probably going to be cleaned up, as the methodology behind these menus is kind of awful.
	// For now, we'll just copy existing behaviour: that is, pass along a set of text IDs & button IDs.
	DECLARE_LUA_FUNCTION(SelectMsg) {
		CUser * pUser = LUA_GET_INSTANCE();
		uint32 arg = 2; // start from after the user instance.
		int32 menuButtonText[MAX_MESSAGE_EVENT], 
			menuButtonEvents[MAX_MESSAGE_EVENT];
		uint8 bFlag = LUA_ARG(uint8, arg++);
		int32 nQuestID = LUA_ARG_OPTIONAL(int32, -1, arg++);
		int32 menuHeaderText = LUA_ARG(int32, arg++);

		foreach_array(i, menuButtonText)
		{
			menuButtonText[i] = LUA_ARG_OPTIONAL(int32, -1, arg++);
			menuButtonEvents[i] = LUA_ARG_OPTIONAL(int32, -1, arg++);
		}

		LUA_NO_RETURN(pUser->SelectMsg(bFlag, nQuestID, menuHeaderText, menuButtonText, menuButtonEvents));
	}

	DECLARE_LUA_FUNCTION(NpcMsg) {
		CUser * pUser = LUA_GET_INSTANCE();
		LUA_NO_RETURN(pUser->QuestV2SendNpcMsg(
			LUA_ARG(uint32, 2),
			LUA_ARG_OPTIONAL(uint16, pUser->m_sEventSid, 3)));
	}

	DECLARE_LUA_FUNCTION(CheckWeight) {
		LUA_RETURN(LUA_GET_INSTANCE()->CheckWeight(
			LUA_ARG(uint32, 2),		// item ID
			LUA_ARG_OPTIONAL(uint16, 1, 3)));	// stack size
	}

	DECLARE_LUA_FUNCTION(CheckSkillPoint) {
		LUA_RETURN(LUA_GET_INSTANCE()->CheckSkillPoint(
			LUA_ARG(uint8, 2),		// skill point category
			LUA_ARG(uint8, 3),		// min
			LUA_ARG(uint8, 4)));	// max
	}

	DECLARE_LUA_FUNCTION(isRoomForItem) {
		LUA_RETURN(LUA_GET_INSTANCE()->FindSlotForItem(
			LUA_ARG(uint32, 2),					// item ID
			LUA_ARG_OPTIONAL(uint16, 1, 3)));	// stack size
	}

	DECLARE_LUA_FUNCTION(CheckGiveSlot) {
		LUA_RETURN(LUA_GET_INSTANCE()->LuaCheckGiveSlot(LUA_ARG(uint8, 2)));	// Slot Control
	}

	DECLARE_LUA_FUNCTION(CheckExchange) {
		LUA_RETURN(LUA_GET_INSTANCE()->CheckExchange(LUA_ARG(uint32, 2)));	// exchange ID
	}

	DECLARE_LUA_FUNCTION(RunQuestExchange) {
		LUA_RETURN(LUA_GET_INSTANCE()->RunQuestExchange(
			LUA_ARG(uint32, 2),		// exchange ID
			LUA_ARG_OPTIONAL(int32, -1, 3),	  // step
			LUA_ARG_OPTIONAL(uint8, 0, 4)));	  // giveall
	}

	DECLARE_LUA_FUNCTION(RunMiningExchange) {
		LUA_NO_RETURN(LUA_GET_INSTANCE()->MiningExchange(LUA_ARG(uint16, 2)));
	}
	DECLARE_LUA_FUNCTION(KissUser) {
		LUA_NO_RETURN(LUA_GET_INSTANCE()->KissUser());
	}

	DECLARE_LUA_FUNCTION(PromoteUserNovice) {
		LUA_RETURN(LUA_GET_INSTANCE()->PromoteUserNovice());
	}

	DECLARE_LUA_FUNCTION(PromoteUser) {
		LUA_RETURN(LUA_GET_INSTANCE()->PromoteUser());
	}

	DECLARE_LUA_FUNCTION(ShowEffect) {
		LUA_NO_RETURN(LUA_GET_INSTANCE()->ShowEffect(LUA_ARG(uint32, 2))); // effect ID
	}

	DECLARE_LUA_FUNCTION(ShowNpcEffect) {
		LUA_NO_RETURN(LUA_GET_INSTANCE()->ShowNpcEffect(LUA_ARG(uint32, 2))); // effect ID
	}

	DECLARE_LUA_FUNCTION(ZoneChange) {
		LUA_NO_RETURN(LUA_GET_INSTANCE()->ZoneChange(
			LUA_ARG(uint16, 2),		// zone ID
			LUA_ARG(float, 3),		// x
			LUA_ARG(float, 4)));	// z
	}

	DECLARE_LUA_FUNCTION(ZoneChangeParty) {
		LUA_NO_RETURN(LUA_GET_INSTANCE()->ZoneChangeParty(
			LUA_ARG(uint16, 2),		// zone ID
			LUA_ARG(float, 3),		// x
			LUA_ARG(float, 4)));	// z
	}

	DECLARE_LUA_FUNCTION(ZoneChangeClan) {
		LUA_NO_RETURN(LUA_GET_INSTANCE()->ZoneChangeClan(
			LUA_ARG(uint16, 2),		// zone ID
			LUA_ARG(float, 3),		// x
			LUA_ARG(float, 4)));	// z
	}

	DECLARE_LUA_FUNCTION(SendNameChange) {
		LUA_NO_RETURN(LUA_GET_INSTANCE()->SendNameChange());
	}

	DECLARE_LUA_FUNCTION(SendClanNameChange) {
		LUA_NO_RETURN(LUA_GET_INSTANCE()->SendClanNameChange());
	}

	DECLARE_LUA_FUNCTION(SendStatSkillDistribute) {
		LUA_NO_RETURN(LUA_GET_INSTANCE()->SendStatSkillDistribute());
	}

	DECLARE_LUA_FUNCTION(ResetStatPoints) {
		LUA_NO_RETURN(LUA_GET_INSTANCE()->AllPointChange2(false));
	}

	DECLARE_LUA_FUNCTION(ResetSkillPoints) {
		LUA_NO_RETURN(LUA_GET_INSTANCE()->AllSkillPointChange2(false));
	}

	DECLARE_LUA_FUNCTION(GiveLoyalty) {
		LUA_NO_RETURN(LUA_GET_INSTANCE()->SendLoyaltyChange(LUA_ARG(int32, 2), false, false, false));
	}

	DECLARE_LUA_FUNCTION(RobLoyalty) {
		LUA_NO_RETURN(LUA_GET_INSTANCE()->SendLoyaltyChange(-(LUA_ARG(int32, 2)), false, false, false));
	}

	DECLARE_LUA_FUNCTION(GiveCash) {
		LUA_NO_RETURN(LUA_GET_INSTANCE()->SendKnightCash(LUA_ARG(uint32, 2)));	
	}

	DECLARE_LUA_FUNCTION(ShowBulletinBoard) {
		LUA_NO_RETURN(LUA_GET_INSTANCE()->ShowBulletinBoard());	
	}

	DECLARE_LUA_FUNCTION(ChangeManner) {
		LUA_NO_RETURN(LUA_GET_INSTANCE()->SendMannerChange(LUA_ARG(int32, 2)));	
	}

	DECLARE_LUA_FUNCTION(PromoteClan) {
		LUA_NO_RETURN(LUA_GET_INSTANCE()->PromoteClan((ClanTypeFlag) LUA_ARG_OPTIONAL(uint8, ClanTypePromoted, 2)));	
	}

	DECLARE_LUA_FUNCTION(GetStat) {
		LUA_RETURN(LUA_GET_INSTANCE()->GetStat((StatType)(LUA_ARG(uint8, 2) + 1)));	
	}

	DECLARE_LUA_FUNCTION(RobClanPoint) {
		LUA_NO_RETURN(LUA_GET_INSTANCE()->SendClanPointChange(-(LUA_ARG(int32, 2))));	
	}

	DECLARE_LUA_FUNCTION(RequestPersonalRankReward) {
		LUA_RETURN(LUA_GET_INSTANCE()->GetRankReward(true));	
	}

	DECLARE_LUA_FUNCTION(RequestReward) {
		LUA_RETURN(LUA_GET_INSTANCE()->GetRankReward(false));	
	}

	DECLARE_LUA_FUNCTION(RunExchange) {
		LUA_NO_RETURN(LUA_GET_INSTANCE()->RunExchange(
			LUA_ARG(int, 2),		
			LUA_ARG(uint16, 3)));
	}

	DECLARE_LUA_FUNCTION(GetMaxExchange) {
		LUA_RETURN(LUA_GET_INSTANCE()->GetMaxExchange((LUA_ARG(int, 2))));
	}

	DECLARE_LUA_FUNCTION(RunCountExchange) {
		LUA_RETURN(LUA_GET_INSTANCE()->RunCountExchange((LUA_ARG(int, 2))));
	}

	DECLARE_LUA_FUNCTION(GetUserDailyOp) {
		LUA_RETURN(LUA_GET_INSTANCE()->GetUserDailyOp((LUA_ARG(uint8, 2))));
	}

	DECLARE_LUA_FUNCTION(GetEventTrigger) {
		LUA_RETURN(LUA_GET_INSTANCE()->GetEventTrigger());
	}

	DECLARE_LUA_FUNCTION(CheckMiddleStatueCapture) {
		LUA_RETURN(LUA_GET_INSTANCE()->CheckMiddleStatueCapture());
	}

	DECLARE_LUA_FUNCTION(MoveMiddleStatue) {
		LUA_NO_RETURN(LUA_GET_INSTANCE()->MoveMiddleStatue());
	}

	DECLARE_LUA_FUNCTION(LevelChange) {
		LUA_NO_RETURN(LUA_GET_INSTANCE()->LevelChange(LUA_ARG(uint8, 2), false));
	}

	DECLARE_LUA_FUNCTION(GivePremium) {
		LUA_NO_RETURN(LUA_GET_INSTANCE()->GivePremium(LUA_ARG(uint8, 2), LUA_ARG_OPTIONAL(uint8, 1, 3)));
	}

	DECLARE_LUA_FUNCTION(GiveClanPremium) {
		LUA_NO_RETURN(LUA_GET_INSTANCE()->GiveClanPremium(LUA_ARG(uint8, 2), LUA_ARG_OPTIONAL(uint8, 1, 3)));
	}

	DECLARE_LUA_FUNCTION(GivePremiumItem) {
		LUA_NO_RETURN(LUA_GET_INSTANCE()->GivePremiumItem(LUA_ARG(uint8, 2)));
	}

	DECLARE_LUA_FUNCTION(GiveKnightCash){
		LUA_NO_RETURN(LUA_GET_INSTANCE()->GiveKnightCash(LUA_ARG(uint32,2)));
	}

	DECLARE_LUA_FUNCTION(GetPVPMonumentNation) {
		LUA_RETURN(LUA_GET_INSTANCE()->GetPVPMonumentNation());
	}

	DECLARE_LUA_FUNCTION(NationChange) {
		LUA_RETURN(LUA_GET_INSTANCE()->NationChange());
	}

	DECLARE_LUA_FUNCTION(GenderChange) {
		LUA_RETURN(LUA_GET_INSTANCE()->GenderChange((LUA_ARG(uint8, 2))));
	}

	DECLARE_LUA_FUNCTION(JobChange) {
		LUA_RETURN(LUA_GET_INSTANCE()->JobChange((LUA_ARG(uint8, 2))));
	}

	DECLARE_LUA_FUNCTION(EventSoccerMember) {
		LUA_NO_RETURN(LUA_GET_INSTANCE()->isEventSoccerMember(
			LUA_ARG(uint8, 2),		// Soccer Team
			LUA_ARG(float, 3),		// x
			LUA_ARG(float, 4)));	// z
	}

	DECLARE_LUA_FUNCTION(EventSoccerStard) {
		LUA_NO_RETURN(LUA_GET_INSTANCE()->isEventSoccerStard());
	}

	DECLARE_LUA_FUNCTION(SpawnEventSystem) {
		LUA_NO_RETURN(LUA_GET_INSTANCE()->SpawnEventSystem(
			LUA_ARG(uint16, 2),		//	sSid
			LUA_ARG(uint8, 3),		//	(NPC 0 Monster 1)
			LUA_ARG(uint8, 4),		//	ZONEID
			LUA_ARG(float, 5),		//	x
			LUA_ARG(float, 6),		//	y
			LUA_ARG(float, 7)));	//	z
	}

	DECLARE_LUA_FUNCTION(SendNationTransfer) {
		LUA_NO_RETURN(LUA_GET_INSTANCE()->SendNationTransfer());
	}

	DECLARE_LUA_FUNCTION(SendRepurchaseMsg) {
		LUA_NO_RETURN(LUA_GET_INSTANCE()->SendRepurchaseMsg());
	}

	DECLARE_LUA_FUNCTION(JoinEvent) {
		LUA_RETURN(LUA_GET_INSTANCE()->TempleJoinEvent(TEMPLE_EVENT_JURAD_MOUNTAIN)); // nIndex or quest ID
	}

	DECLARE_LUA_FUNCTION(GiveNPoints) {
		LUA_NO_RETURN(LUA_GET_INSTANCE()->GiveNPoints((LUA_ARG(uint16, 2)))); // NPoints Amount
	}

	DECLARE_LUA_FUNCTION(KillNpcEvent) {
		LUA_NO_RETURN(LUA_GET_INSTANCE()->KillNpcEvent());
	}

	DECLARE_LUA_FUNCTION(NpcEventSystem) {
		LUA_NO_RETURN(LUA_GET_INSTANCE()->NpcEventSystem((LUA_ARG(uint32, 2)))); // NPoints Amount
	}

	DECLARE_LUA_FUNCTION(KingsInspectorList) {
		LUA_NO_RETURN(LUA_GET_INSTANCE()->KingsInspectorList());
	}

	DECLARE_LUA_FUNCTION(isCswWinnerNembers) {
		LUA_NO_RETURN(LUA_GET_INSTANCE()->isCswWinnerNembers());
	}

	DECLARE_LUA_FUNCTION(DelosCasttellanZoneOut) {
		LUA_NO_RETURN(LUA_GET_INSTANCE()->DelosCasttellanZoneOut());
	}

	DECLARE_LUA_FUNCTION(CycleSpawn) {
		LUA_NO_RETURN(LUA_GET_INSTANCE()->ChangePosition());
	}

	DECLARE_LUA_FUNCTION(MonsterStoneQuestJoin) {
		LUA_NO_RETURN(LUA_GET_INSTANCE()->MonsterStoneQuestProcess(
			LUA_ARG(uint16, 2)));		//sQuestNum
	}
	DECLARE_LUA_FUNCTION(DrakiRiftChange) {
		LUA_NO_RETURN(LUA_GET_INSTANCE()->DrakiRiftChange(
			LUA_ARG(uint16, 2),
			LUA_ARG(uint16, 3)));
	}

	DECLARE_LUA_FUNCTION(DrakiOutZone) {
		LUA_NO_RETURN(LUA_GET_INSTANCE()->DrakiTowerKickOuts());
	}

	DECLARE_LUA_FUNCTION(DrakiTowerNpcOut) {
		LUA_NO_RETURN(LUA_GET_INSTANCE()->DrakiTowerNpcOut());
	}

	DECLARE_LUA_FUNCTION(GetExpPercent)
	{
		CUser * pUser = LUA_GET_INSTANCE();
		if (!pUser) {
			LUA_RETURN(0);
		}
		LUA_RETURN(pUser->GetExpPercent());
	}
};
