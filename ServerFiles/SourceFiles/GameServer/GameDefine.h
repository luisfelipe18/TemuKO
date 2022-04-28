#pragma once

// Classes
#define KARUWARRIOR			101	// Beginner Karus Warrior
#define KARUROGUE			102	// Beginner Karus Rogue
#define KARUWIZARD			103	// Beginner Karus Magician
#define KARUPRIEST			104	// Beginner Karus Priest
#define BERSERKER			105	// Skilled (after first job change) Karus Warrior
#define GUARDIAN			106	// Mastered Karus Warrior
#define HUNTER				107	// Skilled (after first job change) Karus Rogue
#define PENETRATOR			108	// Mastered Karus Rogue
#define SORSERER			109	// Skilled (after first job change) Karus Magician
#define NECROMANCER			110	// Mastered Karus Magician
#define SHAMAN				111	// Skilled (after first job change) Karus Priest
#define DARKPRIEST			112	// Mastered Karus Priest
#define KURIANSTARTER		113	// Kurian 
#define KURIANNOVICE		114	// Novice Kurian 
#define KURIANMASTER		115	// Mastered Kurian

#define ELMORWARRRIOR		201	// Beginner El Morad Warrior
#define ELMOROGUE			202	// Beginner El Morad Rogue
#define ELMOWIZARD			203	// Beginner El Morad Magician
#define ELMOPRIEST			204	// Beginner El Morad Priest
#define BLADE				205	// Skilled (after first job change) El Morad Warrior
#define PROTECTOR			206	// Mastered El Morad Warrior
#define RANGER				207	// Skilled (after first job change) El Morad Rogue
#define ASSASSIN			208	// Mastered El Morad Rogue
#define MAGE				209	// Skilled (after first job change) El Morad Magician
#define ENCHANTER			210	// Mastered El Morad Magician
#define CLERIC				211	// Skilled (after first job change) El Morad Priest
#define DRUID				212	// Mastered El Morad Priest
#define PORUTUSTARTER		213	// Kurian 
#define PORUTUNOVICE		214	// Novice PORUTU 
#define PORUTUMASTER		215	// Mastered PORUTU

// Races
#define KARUS_BIG			1	// Arch Tuarek (Karus Warriors - only!)
#define KARUS_MIDDLE		2	// Tuarek (Karus Rogues & Priests)
#define KARUS_SMALL			3	// Wrinkle Tuarek (Karus Magicians)
#define KARUS_WOMAN			4	// Puri Tuarek (Karus Priests)
#define KURIAN				6	// Kurian

#define BABARIAN			11	// Barbarian (El Morad Warriors - only!)
#define ELMORAD_MAN			12	// El Morad Male (El Morad - ALL CLASSES)
#define ELMORAD_WOMAN		13	// El Morad Female (El Morad - ALL CLASSES)
#define PORUTU				14	// Porutu

// Ÿ�ݺ� ����� //
#define GREAT_SUCCESS			0X01		// �뼺��
#define SUCCESS					0X02		// ����
#define NORMAL					0X03		// ����
#define	FAIL					0X04		// ���� 

enum PetModes
{
	MODE_ATTACK = 3,
	MODE_DEFENCE = 4,
	MODE_LOOTING = 8,
	MODE_CHAT = 9,
	MODE_SATISFACTION_UPDATE = 0x0F,
	MODE_FOOD = 0x10
};

enum ItemMovementType
{
	ITEM_INVEN_SLOT			= 1,
	ITEM_SLOT_INVEN			= 2,
	ITEM_INVEN_INVEN		= 3,
	ITEM_SLOT_SLOT			= 4,
	ITEM_INVEN_ZONE			= 5,
	ITEM_ZONE_INVEN			= 6,
	ITEM_INVEN_TO_COSP		= 7,  // Inventory -> Cospre bag
	ITEM_COSP_TO_INVEN		= 8,  // Cospre bag -> Inventory
	ITEM_INVEN_TO_MBAG		= 9,  // Inventory -> Magic bag
	ITEM_MBAG_TO_INVEN		= 10, // Magic bag -> Inventory
	ITEM_MBAG_TO_MBAG		= 11,  // Magic bag -> Magic bag
	ITEM_INVEN_TO_PET		= 12, // Inventory -> Pet Inventory
	ITEM_PET_TO_INVEN		= 13  // Pet Inventory -> Inventory
};

enum SpawnEventType
{
	UnderTheCastleSummon = 1,
	ChaosStoneSummon = 2,
	StableSummonSummon = 3,
	DungeonDefencSummon = 4,
	DrakiTowerSummon = 5
};

enum ItemSlotType
{
	ItemSlot1HEitherHand		= 0,
	ItemSlot1HRightHand			= 1,
	ItemSlot1HLeftHand			= 2,
	ItemSlot2HRightHand			= 3,
	ItemSlot2HLeftHand			= 4,
	ItemSlotPauldron			= 5,
	ItemSlotPads				= 6,
	ItemSlotHelmet				= 7,
	ItemSlotGloves				= 8,
	ItemSlotBoots				= 9,
	ItemSlotEarring				= 10,
	ItemSlotNecklace			= 11,
	ItemSlotRing				= 12,
	ItemSlotShoulder			= 13,
	ItemSlotBelt				= 14,
	ItemSlotKaul				= 20,
	ItemSlotBag					= 25,
	ItemSlotCospreGloves		= 100,
	ItemSlotCosprePauldron		= 105,
	ItemSlotCospreHelmet		= 107,
	ItemSlotCospreWings			= 110,
	ItemSlotCospreFairy 		= 111,
	ItemSlotCospreTattoo		= 112
};

// Item Weapon Kind
#define WEAPON_KIND_DAGGER		11
#define WEAPON_KIND_1H_SWORD	21
#define WEAPON_KIND_2H_SWORD	22
#define WEAPON_KIND_1H_AXE		31
#define WEAPON_KIND_2H_AXE		32
#define WEAPON_KIND_1H_CLUP		41
#define WEAPON_KIND_2H_CLUP		42
#define WEAPON_KIND_1H_SPEAR	51
#define WEAPON_KIND_2H_SPEAR	52
#define WEAPON_SHIELD			60
#define WEAPON_PICKAXE			61	// Unlike the others, this is just the Kind field as-is (not / 10).
#define WEAPON_MATLOCK			62	// Unlike the others, this is just the Kind field as-is (not / 10). ZONE_PRISON
#define WEAPON_FISHING			63	// Unlike the others, this is just the Kind field as-is (not / 10).
#define WEAPON_KIND_BOW			70
#define WEAPON_KIND_CROSSBOW	71
#define ACCESSORY_EARRING		91
#define ACCESSORY_NECKLACE		92
#define ACCESSORY_RING			93
#define ACCESSORY_BELT			94
#define WEAPON_KIND_STAFF		110
#define WEAPON_KIND_JAMADHAR	140
#define WEAPON_KIND_MACE		181
#define WEAPON_KIND_PORTU_AC	200
#define WEAPON_KIND_WARRIOR_AC	210
#define WEAPON_KIND_ROGUE_AC	220
#define WEAPON_KIND_MAGE_AC		230
#define WEAPON_KIND_PRIEST_AC	240
#define ITEM_KIND_COSPRE		252
#define ITEM_KIND_PET_ITEM		151

////////////////////////////////////////////////////////////
// User Status //
#define USER_STANDING			0X01		// �� �ִ�.
#define USER_SITDOWN			0X02		// �ɾ� �ִ�.
#define USER_DEAD				0x03		// ��Ŷ�
#define USER_BLINKING			0x04		// ��� ��Ƴ���!!!
#define USER_MONUMENT			0x06		// MonumentSystem	
#define USER_MINING				0x07		// MINING System
#define USER_FLASHING			0x08		// FLASHING System
////////////////////////////////////////////////////////////
// Durability Type
#define ATTACK				0x01
#define DEFENCE				0x02
#define REPAIR_ALL			0x03
#define ACID_ALL			0x04
#define UTC_ATTACK			0x05
#define UTC_DEFENCE			0x06
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
// Knights Authority Type
/*
#define CHIEF				0x06
#define VICECHIEF			0x05*/
#define OFFICER				0x04
#define KNIGHT				0x03
//#define TRAINEE				0x02
#define PUNISH				0x01	

#define CHIEF				0x01	// ����
#define VICECHIEF			0x02	// �δ���
#define TRAINEE				0x05	// ���
#define COMMAND_CAPTAIN		100		// ���ֱ���
////////////////////////////////////////////////////////////

#define CLAN_COIN_REQUIREMENT	500000
#define CLAN_LEVEL_REQUIREMENT	35

#define ITEM_CHAT					900012000// Chat Quest
#define ITEM_GOLD					900000000// Coins
#define ITEM_EXP					900001000
#define ITEM_COUNT					900002000
#define ITEM_LADDERPOINT			900003000
#define ITEM_SKILL					900007000
#define ITEM_HUNT					900005000 // Hunt for Quests
#define ITEM_NO_TRADE_MIN			900000001 // Cannot be traded, sold or stored.
#define ITEM_NO_TRADE_MAX			999999999 // Cannot be traded, sold or stored.
#define ITEM_SCROLL_LOW				379221000 // Upgrade Scroll (Low Class item)
#define ITEM_SCROLL_MIDDLE			379205000 // Upgrade Scroll (Middle Class item)
#define ITEM_SCROLL_HIGH			379021000 // Blessed Upgrade Scroll
#define ITEM_SCROLL_REB				379256000 // Reverse Scroll
#define ITEM_SCROLL_REBSTR			379257000 // Reverse Item Strengthen Scroll
#define ITEM_TRINA					700002000 // Trina's Piece
#define ITEM_KARIVDIS				379258000 // Tears of Karivdis
#define ITEM_SHADOW_PIECE			700009000 // Shadow Piece
#define ITEM_DRAGON_SCALE			700043000 // Dragon Scale
#define ITEM_MAGE_EARRING			310310004
#define ITEM_WARRIOR_EARRING		310310005
#define ITEM_ROGUE_EARRING			310310006
#define ITEM_PRIEST_EARRING			310310007
#define SIEGEWARREWARD				914007000
#define VIP_VAULT_KEY				800442000

#define AUTOMATIC_MINING			501610000
#define AUTOMATIC_FISHING			501620000

#define SLAVE_MERCHANT				610002000

#define AUTOMATIC_DROP_MINING		750680000//751690000
#define AUTOMATIC_DROP_FISHING		750690000//751710000 
////////////////////////////////////////////////////////////
// EVENT MISCELLANOUS DATA DEFINE
#define ZONE_TRAP_INTERVAL	   2		// Interval is one second (in seconds) right now.
#define ZONE_TRAP_DAMAGE	   500		// HP Damage is 10 for now :)

////////////////////////////////////////////////////////////

#define RIVALRY_DURATION		(300)	// 5 minutes
#define RIVALRY_NP_BONUS		(150)	// 150 additional NP on kill
#define MINIRIVALRY_NP_BONUS	(50)	// 150 additional NP on kill

#define MAX_ANGER_GAUGE			(5)		// Maximum of 5 deaths in a PVP zone to fill your gauge.

#define PVP_MONUMENT_NP_BONUS	(5)	// 5 additional NP on kill
#define EVENT_MONUMENT_NP_BONUS	(10)	// 10 additional NP on kill

#define MAX_ID_REPORT 15

#define PET_START_ITEM  610001000
#define PET_START_LEVEL	1

////////////////////////////////////////////////////////////
// SKILL POINT DEFINE
#define ORDER_SKILL			0x01
#define MANNER_SKILL		0X02
#define LANGUAGE_SKILL		0x03
#define BATTLE_SKILL		0x04
#define PRO_SKILL1			0x05
#define PRO_SKILL2			0x06
#define PRO_SKILL3			0x07
#define PRO_SKILL4			0x08

enum SkillPointCategory
{
	SkillPointFree		= 0,
	SkillPointCat1		= 5,
	SkillPointCat2		= 6,
	SkillPointCat3		= 7,
	SkillPointMaster	= 8
};

/////////////////////////////////////////////////////////////
// ITEM TYPE DEFINE
#define ITEM_TYPE_FIRE				0x01
#define ITEM_TYPE_COLD				0x02
#define ITEM_TYPE_LIGHTNING			0x03
#define ITEM_TYPE_POISON			0x04
#define ITEM_TYPE_HP_DRAIN			0x05
#define ITEM_TYPE_MP_DAMAGE			0x06
#define ITEM_TYPE_MP_DRAIN			0x07
#define ITEM_TYPE_MIRROR_DAMAGE		0x08

/////////////////////////////////////////////////////////////
// JOB GROUP TYPES
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

#define MAX_SELECTING_USER 5

//////////////////////////////////////////////////////////////
// USER ABNORMAL STATUS TYPES
enum AbnormalType
{
	ABNORMAL_INVISIBLE		= 0,	// Hides character completely (for GM visibility only).
	ABNORMAL_NORMAL			= 1,	// Shows character. This is the default for players.
	ABNORMAL_GIANT			= 2,	// Enlarges character.
	ABNORMAL_DWARF			= 3,	// Shrinks character.
	ABNORMAL_BLINKING		= 4,	// Forces character to start blinking.
	ABNORMAL_GIANT_TARGET	= 6,	// Enlarges character and shows "Hit!" effect.
	ABNORMAL_CHAOS_NORMAL   = 7,	// Returns the user to the Chaos nonblinkingin form.
};

//////////////////////////////////////////////////////////////
// Object Type
#define NORMAL_OBJECT		0
#define SPECIAL_OBJECT		1

//////////////////////////////////////////////////////////////
// REGENE TYPES
#define REGENE_NORMAL		0
#define REGENE_MAGIC		1
#define REGENE_ZONECHANGE	2
#define MAX_PARTY_USERS		8

struct _CLASS_COEFFICIENT
{
	uint16	sClassNum;
	float	ShortSword;
	float   Jamadar;
	float	Sword;
	float	Axe;
	float	Club;
	float	Spear;
	float	Pole;
	float	Staff;
	float	Bow;
	float	HP;
	float	MP;
	float	SP;
	float	AC;
	float	Hitrate;
	float	Evasionrate;
};

// Dropped loot/chest.
#define LOOT_ITEMS	6
struct _LOOT_ITEM
{
	uint32 nItemID;
	uint16 sCount;

	_LOOT_ITEM(uint32 nItemID, uint16 sCount)
	{
		this->nItemID = nItemID;
		this->sCount = sCount;
	}
};

struct _REPURCHASE_ITEM
{
	uint32 nItemID;
	uint32 nPrice;
	time_t nLastTime;

	uint32 iLastDay()
	{
		return (uint32)((((UNIXTIME-nLastTime)/60)/60)/24);
	}

	bool CheckRepurchase()
	{
		if(iLastDay() <= 3)
			return true;

		return false;
	}
};

struct _LOOT_BUNDLE
{
	uint32 nBundleID;
	std::vector<_LOOT_ITEM> Items;
	float x, z, y;
	time_t tDropTime;
};

struct	_EXCHANGE_ITEM
{
	uint32	nItemID;
	uint32	nCount;
	uint16	sDurability;
	uint8	bSrcPos;
	uint8	bDstPos;
	uint64	nSerialNum;
};

#define QUEST_MOB_GROUPS		4
struct _USER_QUEST_INFO
{
	uint8 QuestState;
	uint8 m_bKillCounts[QUEST_MOB_GROUPS];

	_USER_QUEST_INFO()
	{
		memset(&m_bKillCounts, 0, sizeof(m_bKillCounts));
	}
};

struct _USER_ACHIEVE_SUMMARY
{
	uint32 PlayTime;
	uint32 MonsterDefeatCount;
	uint32 UserDefeatCount;
	uint32 UserDeathCount;
	uint32 TotalMedal;
	uint16 RecentAchieve[3];
	uint16 NormalCount;
	uint16 QuestCount;
	uint16 WarCount;
	uint16 AdvantureCount;
	uint16 ChallangeCount;
	_USER_ACHIEVE_SUMMARY()
	{
		Initialize();
	}

	void Initialize()
	{
		PlayTime = 0;
		MonsterDefeatCount = 0;
		UserDefeatCount = 0;
		UserDeathCount = 0;
		TotalMedal = 0;
		NormalCount = 0;
		QuestCount = 0;
		WarCount = 0;
		AdvantureCount = 0;
		ChallangeCount = 0;
		memset(&RecentAchieve, 0, sizeof(RecentAchieve));
	}
};

struct _USER_ACHIEVE_INFO
{
	uint8 bStatus;
	uint32 iCount[2];
	_USER_ACHIEVE_INFO()
	{
		bStatus = 1;
		memset(iCount, 0, sizeof(iCount));
	}
};

struct _USER_ACHIEVE_TIMED_INFO
{
	uint32 iExpirationTime;
	_USER_ACHIEVE_TIMED_INFO()
	{
		iExpirationTime = 0;
	}
};

struct _COMMANDER
{
	std::string	strName;
	uint8	bNation;
	uint16  sWarzone;
};

enum ItemRace
{
	RACE_TRADEABLE_IN_48HR	= 19, // These items can't be traded until 48 hours from the time of creation
	RACE_UNTRADEABLE		= 20  // Cannot be traded or sold.
};

enum SellType
{
	SellTypeNormal		= 0, // sell price is 1/4 of the purchase price
	SellTypeFullPrice	= 1, // sell price is the same as the purchase price
	SellTypeNoRepairs	= 2  // sell price is 1/4 of the purchase price, item cannot be repaired.
};

struct _ITEM_TABLE
{
	uint32	m_iNum;
	int16	Extension;
	std::string	m_sName;
	std::string	Description;
	uint32	ItemIconID1;
	uint32	ItemIconID2;
	uint8	m_bKind;
	uint8	m_bSlot;
	uint8	m_bRace;
	uint8	m_bClass;
	uint16	m_sDamage;
	uint16	m_sDelay;
	uint16	m_sRange;
	uint16	m_sWeight;
	uint16	m_sDuration;
	uint32	m_iBuyPrice;
	uint32	m_iSellPrice;
	int16	m_sAc;
	uint8	m_bCountable;
	uint32	m_iEffect1;
	uint32	m_iEffect2;
	uint8	m_bReqLevel;
	uint8	m_bReqLevelMax;
	uint8	m_bReqRank;
	uint8	m_bReqTitle;
	uint8	m_bReqStr;
	uint8	m_bReqSta;
	uint8	m_bReqDex;
	uint8	m_bReqIntel;
	uint8	m_bReqCha;
	uint16	m_bSellingGroup;
	uint8	m_ItemType;
	uint16	m_sHitrate;
	uint16	m_sEvarate;
	uint16	m_sDaggerAc;
	uint16	m_JamadarAc;
	uint16	m_sSwordAc;
	uint16	m_sMaceAc;
	uint16	m_sAxeAc;
	uint16	m_sSpearAc;
	uint16	m_sBowAc;
	uint8	m_bFireDamage;
	uint8	m_bIceDamage;
	uint8	m_bLightningDamage;
	uint8	m_bPoisonDamage;
	uint8	m_bHPDrain;
	uint8	m_bMPDamage;
	uint8	m_bMPDrain;
	uint8	m_bMirrorDamage;
	uint8	m_DropRate;
	int16	m_sStrB;
	int16	m_sStaB;
	int16	m_sDexB;
	int16	m_sIntelB;
	int16	m_sChaB;
	int16	m_MaxHpB;
	int16	m_MaxMpB;
	int16	m_bFireR;
	int16	m_bColdR;
	int16	m_bLightningR;
	int16	m_bMagicR;
	int16	m_bPoisonR;
	int16	m_bCurseR;
	int16	ItemClass;

	uint32	m_iNPBuyPrice;
	int16	m_Bound;

	INLINE bool isStackable() { return m_bCountable != 0; }

	INLINE uint8 GetKind() { return m_bKind; }
	INLINE uint8 GetType() { return m_ItemType; }
	INLINE uint32 GetNum() { return m_iNum; }
	
	INLINE uint8 GetItemGroup() { return uint8(m_bKind); }
	INLINE bool isDagger() { return GetKind() == WEAPON_KIND_DAGGER; }
	INLINE bool isJamadar() { return GetKind() == WEAPON_KIND_JAMADHAR; }

	INLINE bool isSword() { return GetKind() == WEAPON_KIND_1H_SWORD || GetKind() == WEAPON_KIND_2H_SWORD; }
	INLINE bool is2HSword() { return GetKind() == WEAPON_KIND_2H_SWORD; }

	INLINE bool isAxe() { return GetKind() == WEAPON_KIND_1H_AXE || GetKind() == WEAPON_KIND_2H_AXE; }
	INLINE bool is2HAxe() { return GetKind() == WEAPON_KIND_2H_AXE; }

	INLINE bool isClub() { return GetKind() == WEAPON_KIND_1H_CLUP || GetKind() == WEAPON_KIND_2H_CLUP; }
	INLINE bool is2HClub() { return GetKind() == WEAPON_KIND_2H_CLUP; }

	INLINE bool isSpear() { return GetKind() == WEAPON_KIND_1H_SPEAR || GetKind() == WEAPON_KIND_2H_SPEAR; }
	INLINE bool is2HSpear() { return GetKind() == WEAPON_KIND_2H_SPEAR; }

	INLINE bool isShield() { return GetKind() == WEAPON_SHIELD; }
	INLINE bool isStaff() { return GetKind() == WEAPON_KIND_STAFF; }
	INLINE bool isBow() { return GetKind() == WEAPON_KIND_BOW || GetKind() == WEAPON_KIND_CROSSBOW; }
	INLINE bool isCrossBow() { return GetKind() == WEAPON_KIND_CROSSBOW; }
	INLINE bool isMace() { return GetKind() == WEAPON_KIND_MACE; }




	INLINE bool isPickaxe() { return GetKind() == WEAPON_PICKAXE; }
	INLINE bool isPetItem() { return GetKind() == ITEM_KIND_PET_ITEM; }
	INLINE bool isPunishmentStick() { return GetNum() == 900356000; }

	INLINE bool isTimingFlowTyon() { return GetNum() == 900335523; }
	INLINE bool isTimingFlowMeganthereon() { return GetNum() == 900336524; }
	INLINE bool isTimingFlowHellHound() { return GetNum() == 900337525; }

	INLINE bool isTimingDelay() {
		return
			GetNum() == 900335523 ||
			GetNum() == 900336524 ||
			GetNum() == 900337525;
	}

	INLINE bool isWirinomUniqDelay() {
		return
			(GetNum() >= 127410731 && GetNum() <= 127410740) ||
			(GetNum() >= 127420741 && GetNum() <= 127420750) ||
			(GetNum() >= 127430751 && GetNum() <= 127430760) ||
			(GetNum() >= 127440761 && GetNum() <= 127440770) ||
			GetNum() == 127410284 ||
			GetNum() == 127420285 ||
			GetNum() == 127430286 ||
			GetNum() == 127440287;
	}

	INLINE bool isWirinomRebDelay() {
		return
			(GetNum() >= 127411181 && GetNum() <= 127411210) ||
			(GetNum() >= 127421211 && GetNum() <= 127421240) ||
			(GetNum() >= 127431241 && GetNum() <= 127431270) ||
			(GetNum() >= 127441271 && GetNum() <= 127441300);
	}

	INLINE bool isDarkKnightMace() {
		return
			(GetNum() >= 1111471741 && GetNum() <= 1111471750) ||
			(GetNum() >= 1111479901 && GetNum() <= 1111479930);
	}

	INLINE bool isGargesSwordDelay() {
		return
			(GetNum() >= 1110582731 && GetNum() <= 1110582740) ||
			GetNum() == 1110582451;
	}
	
	INLINE bool isFishing() { return GetKind() == WEAPON_FISHING; }
	INLINE bool isRON() { return GetNum() == 189401287 || GetNum() == 189401288 || GetNum() == 189401289; }
	INLINE bool isLigh() { return GetNum() == 189301277 || GetNum() == 189301278 || GetNum() == 189301279; }
	INLINE bool isNormal() { return GetType() == 4 || GetType() == 5; }
	INLINE bool isRebirth1() { return GetType() == 11 || GetType() == 12; }
	INLINE bool isAll() { return GetKind() > 0; }
	INLINE bool is2Handed() { return m_bSlot == ItemSlot2HLeftHand || m_bSlot == ItemSlot2HRightHand; }
	INLINE bool isAccessory() { return GetKind() == ACCESSORY_EARRING || GetKind() == ACCESSORY_NECKLACE || GetKind() == ACCESSORY_RING || GetKind() == ACCESSORY_BELT; }
	INLINE bool isEarring() { return GetKind() == ACCESSORY_EARRING; }
	INLINE bool isNecklace() { return GetKind() == ACCESSORY_NECKLACE; }
	INLINE bool isRing() { return GetKind() == ACCESSORY_RING; }
	INLINE bool isBelt() { return GetKind() == ACCESSORY_BELT; }
};

struct _ZONE_SERVERINFO
{
	short		sServerNo;
	std::string	strServerIP;
};

struct _KNIGHTS_CAPE
{
	uint16	sCapeIndex;
	uint32	nReqCoins;
	uint32	nReqClanPoints;	// clan point requirement
	uint8	byGrade;		// clan grade requirement
	uint8	byRanking;		// clan rank requirement (e.g. royal, accredited, etc)
	std::string	Description;
	uint8 Type;
	uint8 Ticket;
};

struct _KNIGHTS_SIEGE_WARFARE
{
	uint16	sCastleIndex;
	uint16	sMasterKnights;
	uint8	bySiegeType;
	uint8	byWarDay;
	uint8	byWarTime;
	uint8	byWarMinute;
	uint16	sChallengeList_1;
	uint16	sChallengeList_2;
	uint16	sChallengeList_3;
	uint16	sChallengeList_4;
	uint16	sChallengeList_5;
	uint16	sChallengeList_6;
	uint16	sChallengeList_7;
	uint16	sChallengeList_8;
	uint16	sChallengeList_9;
	uint16	sChallengeList_10;
	uint8	byWarRequestDay;
	uint8	byWarRequestTime;
	uint8	byWarRequestMinute;
	uint8	byGuerrillaWarDay;
	uint8	byGuerrillaWarTime;
	uint8	byGuerrillaWarMinute;
	std::string	strChallengeList;
	uint16	sMoradonTariff;
	uint16	sDellosTariff;
	int32	nDungeonCharge;
	int32	nMoradonTax;
	int32	nDellosTax;
	uint16	sRequestList_1;
	uint16	sRequestList_2;
	uint16	sRequestList_3;
	uint16	sRequestList_4;
	uint16	sRequestList_5;
	uint16	sRequestList_6;
	uint16	sRequestList_7;
	uint16	sRequestList_8;
	uint16	sRequestList_9;
	uint16	sRequestList_10;
};

struct _KNIGHTS_ALLIANCE
{
	uint16	sMainAllianceKnights;
	uint16	sSubAllianceKnights;
	uint16	sMercenaryClan_1;
	uint16	sMercenaryClan_2;
	std::string strAllianceNotice;
	_KNIGHTS_ALLIANCE()
	{
		strAllianceNotice = "";
		sMainAllianceKnights = 0;
		sSubAllianceKnights = 0;
		sMercenaryClan_1 = 0;
		sMercenaryClan_2 = 0;
	}
};

struct _START_POSITION
{
	uint16	ZoneID;
	uint16	sKarusX;
	uint16	sKarusZ;
	uint16	sElmoradX;
	uint16	sElmoradZ;
	uint16	sKarusGateX;
	uint16	sKarusGateZ;
	uint16	sElmoradGateX;
	uint16	sElmoradGateZ;
	uint8	bRangeX;
	uint8	bRangeZ;
};

struct _KNIGHTS_RATING
{
	uint32 nRank;
	uint16 sClanID;
	uint32 nPoints;
};

struct _USER_RANK
{
	uint16	nRank;  // shIndex for USER_KNIGHTS_RANK
	uint16	sKnights;
	std::string strUserName;
	std::string strClanName;
	std::string strRankName;
	uint32	nSalary; // nMoney for USER_KNIGHTS_RANK
	uint32	nLoyalty; // nKarusLoyaltyMonthly/nElmoLoyaltyMonthly for USER_PERSONAL_RANK
};

struct _PREMIUM_DATA
{
	uint8	bPremiumType;
	uint32	iPremiumTime;

	_PREMIUM_DATA()
	{
		bPremiumType = 0;
		iPremiumTime = 0;
	}
};

struct _DRAKI_TOWER_ROOM_USER_LIST
{
	uint16 m_DrakiRoomID;
	std::string strUserID;
};

struct _DRAKI_TOWER_INFO
{
	CSTLMap <_DRAKI_TOWER_ROOM_USER_LIST, uint16> m_UserList;
	uint16  m_tDrakiRoomID;
	time_t  m_tDrakiTimer;
	time_t	m_tDrakiSubTimer;
	time_t	m_tDrakiOutTimer;
	time_t  m_tDrakiTownOutTimer;
	bool	m_bOutTimer;
	bool	m_bTownOutTimer;
	bool	m_isDrakiStageChange;
	bool    m_tDrakiTowerStart;
	uint8	m_bSavedDrakiMaxStage;
	uint8	m_bSavedDrakiStage;
	bool    m_bTownRequest;
	uint32	m_bSavedDrakiTime;
	uint8	m_bSavedDrakiLimit;
	uint16	m_sDrakiStage, m_sDrakiSubStage, m_sDrakiTempStage, m_sDrakiTempSubStage;
	uint16  m_bDrakiMonsterKill;
	uint32	m_tDrakiSpareTimer;
	uint32  m_tDrakiRoomCloseTimer;

	_DRAKI_TOWER_INFO()
	{
		Initialize();
	}

	uint8 GetRoomuserCount()
	{
		return m_UserList.GetSize();
	}

	void Initialize()
	{
		m_tDrakiRoomID = 0;
		m_tDrakiTimer = UNIXTIME;
		m_tDrakiSubTimer = UNIXTIME;
		m_tDrakiOutTimer = UNIXTIME;
		m_tDrakiTownOutTimer = UNIXTIME;
		m_bSavedDrakiMaxStage = 0;
		m_bSavedDrakiStage = 0;
		m_bTownRequest = false;
		m_bSavedDrakiTime = 0;
		m_bSavedDrakiLimit = 0;
		m_sDrakiStage = 0;
		m_sDrakiSubStage = 0;
		m_sDrakiTempStage = 0;
		m_sDrakiTempSubStage = 0;
		m_bDrakiMonsterKill = 0;
		m_tDrakiSpareTimer = 0;
		m_tDrakiRoomCloseTimer = 7200;
		m_tDrakiTowerStart = false;
		m_bOutTimer = false;
		m_bTownOutTimer = false;
		m_isDrakiStageChange = false;
	}
};

struct COLLECTIONRACE_SETTINGS
{
	uint32  ID;
	uint16  CREventMaxUser;
	uint32  CREventItemID[2];
	uint32	CREventWinEXP;
	uint32	CREventWinCoin;
	uint32	CREventWinNP;
	uint32 CREventWinKC;
	std::string CREventstrName[2];
	uint32  CREventItemCount[2];
	uint32  CREventItemTimed[2];
	uint8   CREventMaxLevel;
	uint8	CREventMinLevel;
};

struct COLLECTIONRACE_HUNT_LIST
{
	uint8	QuestID;
	uint8	QuestZone;
	uint16	MonsterNum1;
	uint16	MonsterNum2;
	uint16	MonsterNum3;
	uint16	MonsterNum4;
	uint16	MonsterKillCount1;
	uint16	MonsterKillCount2;
	uint16	MonsterKillCount3;
	uint16	MonsterKillCount4;
	std::string	MonsterName1;
	std::string MonsterName2;
	std::string MonsterName3;
	std::string MonsterName4;
};

struct USER_HACKTOOL_LIST
{
	uint32 nIndex;
	std::string nChecking;
};

struct DRAKI_MONSTER_LIST
{
	uint32 nIndex;
	uint8  bDrakiStage;
	uint16 sMonsterID;
	uint16 sPosX;
	uint16 sPosZ;
	uint16 sDirection;
	uint8  bMonster;
};

struct DRAKI_ROOM_LIST
{
	uint8 bIndex;
	uint8 bDrakiStage;
	uint8 bDrakiSubStage;
	uint8 bDrakiNpcStage;
	uint8 MonsterKillCount;

	void Initialize()
	{
		MonsterKillCount = 0;
	}
};

struct DRAKI_ROOM_RANK
{
	uint32 bIndex;
	std::string strUserID;
	uint8 bStage;
	uint32 FinishTime;
	uint8 MaxStage;
	uint8 EnteranceLimit;
};

// Achiement System
struct _ACHIEVE_TITLE
{
	uint32	sIndex;
	std::string strName;
	uint16 Str;
	uint16 Hp;
	uint16 Dex;
	uint16 Int;
	uint16 Mp;
	uint16 Attack;
	uint16 Defence;
	uint16 sLoyaltyBonus;
	uint16 sExpBonus;
	uint16 sShortSwordAC;
	uint16 sJamadarAC;
	uint16 sSwordAC;
	uint16 sBlowAC;
	uint16 sAxeAC;
	uint16 sSpearAC;
	uint16 sArrowAC;
	uint16 sFireBonus;
	uint16 sIceBonus;
	uint16 sLightBonus;
	uint16 sFireResist;
	uint16 sIceResist;
	uint16 sLightResist;
	uint16 sMagicResist;
	uint16 sCurseResist;
	uint16 sPoisonResist;
};

struct _ACHIEVE_MAIN
{
	uint32	sIndex;
	uint8	Type;
	uint16 TitleID;
	uint16 Point;
	uint32 ItemNum;
	uint32 iCount;
	uint8	ZoneID;
	uint8	unknown2;
	uint8	AchieveType;
	uint16	ReqTime;
	std::string strName;
	std::string strDescription;
	uint8 byte1;
	uint8 byte2;
};

#define ACHIEVE_MOB_GROUPS		2
#define  ACHIEVE_MOBS_PER_GROUP	4
struct _ACHIEVE_MONSTER
{
	uint32	sIndex;
	uint8 Type;
	uint8 byte;
	uint32	sNum[ACHIEVE_MOB_GROUPS][ACHIEVE_MOBS_PER_GROUP];
	uint32	sCount[ACHIEVE_MOB_GROUPS];

	_ACHIEVE_MONSTER()
	{
		memset(&sCount, 0, sizeof(sCount));
		memset(&sNum, 0, sizeof(sNum));
	}
};

struct _ACHIEVE_WAR
{
	uint32	sIndex;
	uint8	Type;
	uint32 Count;
};

struct _ACHIEVE_NORMAL
{
	uint32	sIndex;
	uint16	Type;
	uint32 Count;
};

struct _ACHIEVE_COM
{
	uint32	sIndex;
	uint8	Type;
	uint16  Required1;
	uint16  Required2;
};
// Finish Achiement System

struct _DUNGEON_DEFENCE_MONSTER_LIST
{
	uint32	ID;
	uint8   Diffuculty;
	uint16  MonsterID;
	uint16  sCount;
	uint16  PosX;
	uint16  PosZ;
	uint16  sDirection;
	uint8   isMonster;
	uint16  sRadiusRange;
};

struct _DUNGEON_DEFENCE_STAGE_LIST
{
	uint32 ID;
	uint8 Difficulty;
	std::string DifficultyName;
	uint8 sStageID;
};

struct _DUNGEON_DEFENCE_ROOM_USER_LIST
{
	uint16 m_DefenceRoomID;
	std::string strUserID[MAX_PARTY_USERS];
};

struct _DUNGEON_DEFENCE_ROOM_INFO
{
	CSTLMap <_DUNGEON_DEFENCE_ROOM_USER_LIST, uint16> m_UserList;
	uint16 m_DefenceRoomID;
	uint16 m_DefenceKillCount;
	uint32 m_DefenceSpawnTime;
	uint32 m_DefenceRoomClose;
	uint32 m_DefenceOutTime;
	uint8  m_DefenceStageID;
	uint8  m_DefenceDifficulty;
	bool   m_DefenceisStarted;
	bool   m_DefenceMonsterBeginnerSpawned;
	bool   m_DefenceMonsterSpawned;
	bool   m_DefenceisFinished;


	_DUNGEON_DEFENCE_ROOM_INFO()
	{
		Initialize();
	}

	uint8 GetRoomuserCount()
	{
		return m_UserList.GetSize();
	}

	void Initialize()
	{
		m_UserList.DeleteAllData();
		m_DefenceRoomID = 0;
		m_DefenceKillCount = 0;
		m_DefenceSpawnTime = 60;
		m_DefenceRoomClose = 7200;
		m_DefenceOutTime = 0;
		m_DefenceStageID = 0;
		m_DefenceDifficulty = 0;
		m_DefenceisStarted = false;
		m_DefenceMonsterSpawned = false;
		m_DefenceMonsterBeginnerSpawned = false;
		m_DefenceisFinished = false;
	}
};

// TODO: Rewrite this system to be less script dependent for exchange logic.
// Coin requirements should be in the database, and exchanges should be grouped.
#define ITEMS_IN_ORIGIN_GROUP 5
#define ITEMS_IN_EXCHANGE_GROUP 5

#define ITEMS_IN_ROB_ITEM_GROUP_LUA 10
#define ITEMS_IN_GIVE_ITEM_GROUP_LUA 10

#define ITEMS_SPECIAL_EXCHANGE_GROUP 10
#define EVENT_START_TIMES 5

struct _MINING_EXCHANGE
{
	uint16  nIndex;
	uint16  sNpcID;
	uint8   GiveEffect;
	uint8   OreType;
	std::string nOriginItemName;
	uint32 nOriginItemNum;
	std::string nGiveItemName;
	uint32 nGiveItemNum;
	uint16 nGiveItemCount;
	uint32 SuccesRate;
};

enum EventType
{
	LunarWar = 1,
	VirtualRoom = 2,
	SingleRoom = 3
};

enum EventLocalID
{
	CastleSiegeWar = 1,
	NapiesGorge = 2,
	AlseidsPrairie = 3,
	NiedsTriangle = 4,
	NereidsIsland = 5,
	Zipang = 6,
	Oreads = 7,
	TestZone = 8,
	SnowWar = 9,
	BorderDefenceWar = 10,
	ChaosExpansion = 11,
	JuraidMountain = 12,
	KnightRoyale = 13,
	UnderTheCastle = 14,
	ForgettenTempleLow = 15,
	ForgettenTempleHigh = 16
};

struct _ROOM_EVENT_PLAY_TIMER
{
	uint8 TableEventLocalID;
	uint16 EventZoneID;
	std::string EventName;
	uint32 EventSignTime;
	uint32 EventPlayTime;
	uint32 EventAttackOpenTime;
	uint32 EventAttackCloseTime;
	uint32 EventFinishTime;
};

struct _ROYALE_EVENT_PLAY_TIMER
{
	uint8 TableEventLocalID;
	uint16 EventZoneID;
	std::string EventName;
	uint32 EventSignTime;
	uint32 EventManuelCloseTimer;
	uint32 EventGasStepTimer1;
	uint32 EventGasStepTimer2;
	uint32 EventGasStepTimer3;
	uint32 EventGasStepTimer4;
	uint32 EventGasStepTimer5;
};

struct _CSW_EVENT_PLAY_TIMER
{
	uint8 TableEventLocalID;
	uint16 EventZoneID;
	std::string EventName;
	int32 BarrackCreatedTime;
	int32 DeathMatchTime;
	int32 PreparingTime;
	int32 CastellanTime;
	int32 OwnerDelosMonumentFinish;
	int32 MonumentFinishTime;
};

struct _EVENT_SCHEDULE
{
	uint8 EventLocalID;
	uint8 EventType;
	uint16 EventZoneID;
	std::string EventName;
	std::string StartDays;
	uint8 EventStatus;
	uint32 EventStartHour[5];
	uint32 EventStartMinutes[5];
	uint8 TimeActive[5];
};

struct _GIVE_LUA_ITEM_EXCHANGE
{
	uint32	nExchangeID;

	uint32	nRobItemID[10];
	uint32	nRobItemCount[10];

	uint32	nGiveItemID[10];
	uint32	nGiveItemCount[10];

	uint32  nGiveItemTime[10];
};

struct _ITEM_EXCHANGE
{
	uint32	nIndex;
	uint8	bRandomFlag;

	uint32	nOriginItemNum[ITEMS_IN_ORIGIN_GROUP];
	uint32	sOriginItemCount[ITEMS_IN_ORIGIN_GROUP];

	uint32	nExchangeItemNum[ITEMS_IN_EXCHANGE_GROUP];
	uint32	sExchangeItemCount[ITEMS_IN_EXCHANGE_GROUP];
};

struct _DKM_MONSTER_DEAD_GIFT
{
	uint8  iNum;
	uint32 nGiveItemNum;
	std::string nGiveItemName;
	uint16 nGiveItemCount;
	uint32 nGiveItemPercent;
};

struct _ITEM_EXCHANGE_EXP
{
	uint32	nIndex;
	uint8	bRandomFlag;
	uint32	nExchangeItemNum[ITEMS_IN_EXCHANGE_GROUP];
	uint32	sExchangeItemCount[ITEMS_IN_EXCHANGE_GROUP];
};

struct SPECIAL_PART_SEWING_EXCHANGE
{
	uint32	nIndex;
	uint16	sNpcNum;
	uint32	nReqItemID[ITEMS_SPECIAL_EXCHANGE_GROUP];
	uint32	nReqItemCount[ITEMS_SPECIAL_EXCHANGE_GROUP];
	uint32	nGiveItemID;
	uint32	nGiveItemCount;
	uint32  iSuccessRate;
	uint8	isNotice;
	uint8   isShadowSucces;
};

struct _ITEM_EXCHANGE_CRASH {
	uint32 nIndex;
	uint32 nItemID;
	uint8  nCount;
	uint16 sRate;
};

struct _ITEM_PREMIUM_GIFT
{
	uint32	m_iItemNum;
	uint16	sCount;
	std::string strMessage;
	std::string strSubject;
	std::string strSender;
};

struct _ITEM_SELLTABLE
{
	int iSellingGroup;
	std::vector<int> ItemIDs;
	int  nIndex;

	_ITEM_SELLTABLE()
	{
		ItemIDs.clear();
	}
};

struct _ITEM_EXPIRATION_TABLE {
	uint32 m_iNum;
	std::string m_sName;
	uint32 nExpiration;
};

#define MAX_ITEMS_REQ_FOR_UPGRADE 8
struct _ITEM_UPGRADE
{
	uint32	nIndex;
	uint16	sNpcNum;
	int8	bOriginType;
	uint32	sOriginItem;
	uint32	nReqItem[MAX_ITEMS_REQ_FOR_UPGRADE];
	uint32	nReqNoah;
	uint8	bRateType;
	uint16	sGenRate;
	int32	nGiveItem;

	INLINE uint32 Getscroll() { return nReqItem[MAX_ITEMS_REQ_FOR_UPGRADE]; }
	INLINE bool isReverse() { return Getscroll() == 379257000; }
	INLINE bool isTransform() { return Getscroll() == 379256000; }
};

enum ItemTriggerType
{
	TriggerTypeAttack = 3,
	TriggerTypeDefend = 13
};

struct _ITEM_OP
{
	uint32	nItemID;
	uint8	bTriggerType;
	uint32	nSkillID;
	uint8	bTriggerRate;
};

struct _SET_ITEM
{
	uint32 SetIndex;

	uint16 HPBonus;
	uint16 MPBonus;
	uint16 StrengthBonus;
	uint16 StaminaBonus;
	uint16 DexterityBonus;
	uint16 IntelBonus;
	uint16 CharismaBonus;
	uint16 FlameResistance;
	uint16 GlacierResistance;
	uint16 LightningResistance;
	uint16 PoisonResistance;
	uint16 MagicResistance;
	uint16 CurseResistance;

	uint16 XPBonusPercent;
	uint16 CoinBonusPercent;

	uint16 APBonusPercent;		// +AP% for all classes
	uint16 APBonusClassType;	// defines a specific class for +APBonusClassPercent% to be used against
	uint16 APBonusClassPercent;	// +AP% for APBonusClassType only

	uint16 ACBonus;				// +AC amount for all classes
	uint16 ACBonusClassType;	// defines a specific class for +ACBonusClassPercent% to be used against
	uint16 ACBonusClassPercent;	// +AC% for ACBonusClassType only

	uint16 MaxWeightBonus;
	uint8 NPBonus;
};

struct _QUEST_HELPER
{
	uint32	nIndex;
	uint8	bMessageType;
	uint8	bLevel;
	uint32	nExp;
	uint8	bClass;
	uint8	bNation;
	uint8	bQuestType;
	uint8	bZone;
	uint16	sNpcId;
	uint16	sEventDataIndex;
	int8	bEventStatus;
	uint32	nEventTriggerIndex;
	uint32	nEventCompleteIndex;
	uint32	nExchangeIndex;
	uint32	nEventTalkIndex;
	std::string strLuaFilename;
	uint32	sQuestMenu;
	uint32	sNpcMain;
	uint8	sQuestSolo;
};

struct _QUEST_HELPER_NATION
{
	uint8	bNation;
	uint16	sEventDataIndex;
};

struct _USER_SEAL_ITEM
{
	uint64 nSerialNum;
	uint32 nItemID;
	uint8 bSealType;
};

#define QUEST_MOB_GROUPS		4
#define QUEST_MOBS_PER_GROUP	4
struct _QUEST_MONSTER
{
	uint16	sQuestNum;
	uint16	sNum[QUEST_MOB_GROUPS][QUEST_MOBS_PER_GROUP]; 
	uint16	sCount[QUEST_MOB_GROUPS];

	_QUEST_MONSTER()
	{
		memset(&sCount, 0, sizeof(sCount));
		memset(&sNum, 0, sizeof(sNum));
	}
};

enum SpecialQuestIDs
{
	QUEST_KILL_GROUP1	= 32001,
	QUEST_KILL_GROUP2	= 32002,
	QUEST_KILL_GROUP3	= 32003,
	QUEST_KILL_GROUP4	= 32004,
	QUEST_KILL_GROUP5	= 32005,
	QUEST_KILL_GROUP6	= 32006,
	QUEST_KILL_GROUP7	= 32007,
};

struct _RENTAL_ITEM
{
	uint32	nRentalIndex;
	uint32	nItemID;
	uint16	sDurability;
	uint64	nSerialNum;
	uint8	byRegType;
	uint8	byItemType;
	uint8	byClass;
	uint16	sRentalTime;
	uint32	nRentalMoney;
	std::string strLenderCharID;
	std::string strBorrowerCharID;
};

struct _PREMIUM_ITEM
{
	uint8	Type;
	uint16	ExpRestorePercent;
	uint16	NoahPercent;
	uint16	DropPercent;
	uint32	BonusLoyalty;
	uint16	RepairDiscountPercent;
	uint16	ItemSellPercent;
};

struct _CHAOS_EXPANSION_RANKING
{
	uint16 c_SocketID;
	int16  c_EventRoom;
	uint16 c_KillCount;
	uint16 c_DeathCount;
};

struct _BORDER_DEFENCE_WAR_RANKING
{
	uint16 b_SocketID;
	int16  b_EventRoom;
	uint8  b_Nation;
	uint32 b_UserPoint;
};

struct _PLAYER_KILLING_ZONE_RANKING
{
	uint16 p_SocketID;
	uint16 p_Zone;
	uint8  P_Nation;
	uint32 P_LoyaltyDaily;
	uint16 P_LoyaltyPremiumBonus;
};

struct _PREMIUM_ITEM_EXP
{
	uint16	nIndex;
	uint8	Type;
	uint8	MinLevel;
	uint8	MaxLevel;
	uint16	sPercent;
};

struct _SEEKING_PARTY_USER
{
	uint16 m_sGetID;
	uint16 m_sClass;
	uint8	isPartyLeader;
	int16	m_sLevel;
	uint8	m_bZone;
	std::string	m_strSeekingNote;
	std::string	m_strUserID;
	uint8	m_bNation;
	uint16 m_sPartyID;
	uint8 m_bSeekType;
	uint8 m_bloginType;
};

struct _TEMPLE_SOCCER_EVENT_USER
{
	uint16 m_socketID;
	uint8 m_teamColour;
};

struct _KNIGHT_RETURN_GIFT_ITEM
{
	uint8 ID;
	uint32	m_iItemNum;
	uint16	sCount;
	std::string strMessage;
	std::string strSubject;
	std::string strSender;
};

struct _KNIGHT_ROYALE_TREASURE_CHEST_LIST
{
	uint8 TreasureChestID;
	uint16 TreasureChestSsid;
	uint16 TreasureChestPosX;
	uint16 TreasureChestPosZ;
	uint32 TreasureSpawnTimer;
	uint32 TreasureDeadSpawnTimer;
	bool TresureisDead;
};

struct _KNIGHT_ROYALE_STATS_LIST
{
	uint8  KnightRoyaleStatsLevel;
	uint32 KnightRoyaleStatsRequiredExperience;
	uint16 KnightRoyaleStatsTotalHit;
	uint16 KnightRoyaleStatsTotalAc;
	uint16 KnightRoyaleStatiMaxHp;
	uint16 KnightRoyaleStatiMaxMp;
	uint16 KnightRoyaleStatsFireR;
	uint16 KnightRoyaleStatsColdR;
	uint16 KnightRoyaleStatsLightningR;
	uint16 KnightRoyaleStatsMagicR;
	uint16 KnightRoyaleStatsDiseaseR;
	uint16 KnightRoyaleStatsPoisonR;
};

struct _KNIGHT_ROYALE_BEGINNER_ITEM
{
	uint8 ID;
	uint8 ItemPos;
	uint32 nItemID;
	uint8 nCount;
	uint16 nDurability;
	uint8 nFlag;
};


struct _KNIGHT_ROYALE_EVENT_STATUS
{
	int16	ActiveEvent;
	uint8	AllUserCount;
	uint32  StartTime;
	uint32  CloseTime;
	bool	isActive;
	bool	AllowJoin;
	bool    EventTimerStartControl;
	bool    EventCloseMainControl;
};

struct _KNIGHT_ROYALE_EVENT_USER
{
	uint32 m_KnightRoyaleJoinRequest;
	std::string KnightRoyaleStrUserID;
};

struct _KNIGHT_ROYALE_STARTED_USER
{
	std::string KnightRoyaleStrUserID;
	bool  KnightRoyaleisDead;
	bool  KnightRoyaleisExit;
	bool  KnightRoyaleinWinner;
	uint8 KnightRoyaleLevel;
	uint16 KnightRoyaleTotalExp;
	uint16 KnightRoyaleUserExp;
	uint8 KnightRoyaleKillCount;
};

struct _KNIGHT_ROYALE_INFO
{
	CSTLMap <_KNIGHT_ROYALE_STARTED_USER, std::string> m_KnightRoyaleUserList;
	uint32   m_KnightRoyaleisCloseTimer;
	bool   m_KnightRoyaleisFinished;
	bool   m_KnightRoyaleSendGasArea;
	uint8  m_KnightRoyaleStartedUserCount;
	uint8  m_KnightRoyaleRemaingUserCount;
	uint32 m_KnightRoyaleGasTimer;
	uint8  m_KnightRoyaleGasStepCount;
	uint8  m_KnightRoyaleGasStepTimer;

	_KNIGHT_ROYALE_INFO()
	{
		Initialize();
	}

	uint8 GetRoomTotalUserCount()
	{
		return m_KnightRoyaleUserList.GetSize();
	}

	void Initialize()
	{
		m_KnightRoyaleUserList.DeleteAllData();
		m_KnightRoyaleisFinished = false;
		m_KnightRoyaleSendGasArea = false;
		m_KnightRoyaleisCloseTimer = 0;
		m_KnightRoyaleStartedUserCount = 0;
		m_KnightRoyaleRemaingUserCount = 0;
		m_KnightRoyaleGasTimer = 0;
		m_KnightRoyaleGasStepCount = 0;
		m_KnightRoyaleGasStepTimer = 3;
	}
};

struct _CSW_STATUS
{
	bool   Started;
	uint8  Status;

	int32 CswTime;
	int32 MonumentTime;
	uint8 MonumentType;

	bool BarrackCreateTimerControl;
	bool DeathMatchTimerControl;
	bool PreparationTimerControl;
	bool CastellanWarTimerControl;

	bool isMonumentFinish;
	bool isMonumentActive;

	uint8 BarrackNoticeTime;
	uint8 DeathMatchNoticeTime;
	uint8 PreparationNoticeTime;
	uint8 CastellanWarNoticeTime;
	uint8 MonumentNoticeTime;
};

struct _EVENT_STATUS
{
	int16	ActiveEvent;
	int8	ZoneID;
	uint8	LastEventRoom;
	uint32	StartTime;
	uint32  ClosedTime;
	uint16	AllUserCount;
	uint16	ElMoradUserCount;
	uint16	KarusUserCount;
	uint32  m_tLastWinnerCheck;
	int8	EventStartTimeNumber;
	bool	isAttackable;
	bool	isActive;
	bool	bAllowJoin;
	bool    AutomaticEventOpening;
	bool    EventTimerStartControl;
	bool    EventTimerAttackOpenControl;
	bool    EventTimerAttackCloseControl;
	bool    EventTimerFinishControl;
	bool    EventTimerResetControl;
	bool    EventCloseMainControl;
};

struct _EVENT_BRIDGE
{
	bool   isBridgeSystemActive;
	bool   isBridgeTimerControl1;
	bool   isBridgeTimerControl2;
	bool   isBridgeTimerControl3;
	uint32 isBridgeSystemStartMinutes;
};

struct _TEMPLE_EVENT_USER
{
	uint32 m_iJoinOrder;
	std::string strUserID;
};

struct _TEMPLE_QUEST_USER
{
	uint16	m_socketID;
	uint16	m_zoneID;
	int32	m_teletortTIME;
};

struct _TEMPLE_STARTED_EVENT_USER
{
	std::string strUserID;
	bool isPrizeGiven;
	bool isLoqOut;
};

struct _JURAID_ROOM_INFO
{
	CSTLMap <_TEMPLE_STARTED_EVENT_USER, std::string> m_KarusUserList;
	CSTLMap <_TEMPLE_STARTED_EVENT_USER, std::string> m_ElmoradUserList;
	uint32 m_iKarusDeathCount;	// number of death of karus nation in this room
	uint32 m_iKarusKillCount;	// number of kills of karus nation in this room
	uint32 m_iElmoradDeathCount; // number of kills of elmorad nation in this room
	uint32 m_iElmoradKillCount;  // number of kills of elmorad nation in this room
	uint8  m_iElmoMainMonsterKill;
	uint8  m_iKarusMainMonsterKill;
	uint8  m_iElmoSubMonsterKill;
	uint8  m_iKarusSubMonsterKill;
	uint8  m_bWinnerNation;
	bool   m_bFinished;
	uint32  m_tFinishTimeCounter;
	bool   m_FinishPacketControl;
	uint16 m_sKarusBridges[3];
	uint16 m_sElmoBridges[3];
	bool   m_sElmoBridgesTimerOpen[3];
	bool   m_sKarusBridgesTimerOpen[3];
	bool   m_sElmoBridgeSummonControl[3];
	bool   m_sKarusBridgeSummonControl[3];
	uint16 m_sDevaID;

	_JURAID_ROOM_INFO()
	{
		Initialize();
	}

	uint8 GetRoomKarusUserCount()
	{
		return m_KarusUserList.GetSize();
	}

	uint8 GetRoomElmoradUserCount()
	{
		return m_ElmoradUserList.GetSize();
	}

	uint8 GetRoomTotalUserCount()
	{
		return GetRoomKarusUserCount() + GetRoomElmoradUserCount();
	}

	void Initialize()
	{
		m_KarusUserList.DeleteAllData();
		m_ElmoradUserList.DeleteAllData();
		m_bWinnerNation = 0;
		m_iKarusDeathCount = 0;
		m_iKarusKillCount = 0;
		m_iElmoradDeathCount = 0;
		m_iElmoradKillCount = 0;
		m_iElmoMainMonsterKill = 0;
		m_iKarusMainMonsterKill = 0;
		m_iElmoSubMonsterKill = 0;
		m_iKarusSubMonsterKill = 0;
		m_bFinished = false;
		m_tFinishTimeCounter = 0;
		m_sElmoBridgesTimerOpen[0] = false;
		m_sElmoBridgesTimerOpen[1] = false;
		m_sElmoBridgesTimerOpen[2] = false;
		m_sKarusBridgesTimerOpen[0] = false;
		m_sKarusBridgesTimerOpen[1] = false;
		m_sKarusBridgesTimerOpen[2] = false;
		m_sElmoBridgeSummonControl[0] = false;
		m_sElmoBridgeSummonControl[1] = false;
		m_sElmoBridgeSummonControl[2] = false;
		m_sKarusBridgeSummonControl[0] = false;
		m_sKarusBridgeSummonControl[1] = false;
		m_sKarusBridgeSummonControl[2] = false;
		m_FinishPacketControl = false;
	}
};

struct _BDW_ROOM_INFO
{
	CSTLMap <_TEMPLE_STARTED_EVENT_USER, std::string> m_KarusUserList;
	CSTLMap <_TEMPLE_STARTED_EVENT_USER, std::string> m_ElmoradUserList;

	uint32 m_iKarusKillCount;
	uint32 m_iElmoradKillCount;
	uint32 m_iKarusMonuCount;
	uint32 m_iElmoMonuCount;
	uint32 m_ElmoScoreBoard;
	uint32 m_KarusScoreBoard;
	uint8  m_bWinnerNation;
	bool   m_bFinished;
	bool   m_FinishPacketControl;
	uint32  m_tFinishTimeCounter;
	time_t m_tAltarSpawnTimed;
	bool   m_tAltarSpawn;

	_BDW_ROOM_INFO()
	{
		Initialize();
	}

	uint8 GetRoomKarusUserCount()
	{
		return m_KarusUserList.GetSize();
	}

	uint8 GetRoomElmoradUserCount()
	{
		return m_ElmoradUserList.GetSize();
	}

	uint8 GetRoomTotalUserCount()
	{
		return GetRoomKarusUserCount() + GetRoomElmoradUserCount();
	}

	void Initialize()
	{
		m_KarusUserList.DeleteAllData();
		m_ElmoradUserList.DeleteAllData();
		m_bWinnerNation = 0;
		m_iKarusKillCount = 0;
		m_iElmoradKillCount = 0;
		m_iKarusMonuCount = 0;
		m_iElmoMonuCount = 0;
		m_bFinished = false;
		m_tFinishTimeCounter = 0;
		m_tAltarSpawnTimed = UNIXTIME;
		m_tAltarSpawn = false;
		m_FinishPacketControl = false;
		m_ElmoScoreBoard = 0;
		m_KarusScoreBoard = 0;
	}
};

struct _CHAOS_ROOM_INFO
{
	CSTLMap <_TEMPLE_STARTED_EVENT_USER, std::string> m_UserList;
	bool   m_bFinished;
	int32  m_tFinishTimeCounter;
	bool   m_FinishPacketControl;

	_CHAOS_ROOM_INFO()
	{
		Initialize();
	}

	uint8 GetRoomTotalUserCount()
	{
		return m_UserList.GetSize();
	}

	void Initialize()
	{
		m_UserList.DeleteAllData();
		m_bFinished = false;
		m_tFinishTimeCounter = 0;
		m_FinishPacketControl = false;
	}
};

struct _UNDER_THE_CASTLE_INFO
{
	int16 m_sUtcGateID[3];

	_UNDER_THE_CASTLE_INFO()
	{
		Initialize();
	}

	void Initialize()
	{
		m_sUtcGateID[0] = -1;
		m_sUtcGateID[1] = -1;
		m_sUtcGateID[2] = -1;
	}
};

struct _TOURNAMENT_DATA
{
	uint8 aTournamentZoneID;
	uint16 aTournamentClanNum[2];
	uint16 aTournamentScoreBoard[2];
	uint32 aTournamentTimer;
	uint8 aTournamentMonumentKilled;
	time_t aTournamentOutTimer;
	bool aTournamentisAttackable;
	bool aTournamentisStarted;
	bool aTournamentisFinished;

	_TOURNAMENT_DATA()
	{
		Initialize();
	}

	void Initialize()
	{
		aTournamentZoneID = 0;
		aTournamentClanNum[0] = 0;
		aTournamentClanNum[1] = 0;
		aTournamentScoreBoard[0] = 0;
		aTournamentScoreBoard[1] = 0;
		aTournamentTimer = 0;
		aTournamentMonumentKilled = 0;
		aTournamentOutTimer = UNIXTIME;
		aTournamentisAttackable = false;
		aTournamentisStarted = false;
		aTournamentisFinished = false;
	}
};

struct _DEATHMATCH_BARRACK_INFO
{
	uint16  m_tBaseClanID;
	uint16  m_tBaseX;
	uint16  m_tBaseZ;

	_DEATHMATCH_BARRACK_INFO()
	{
		Initialize();
	}

	void Initialize()
	{
		m_tBaseClanID = 0;
		m_tBaseX = 0;
		m_tBaseZ = 0;
	}
};

struct _CASTELLAN_OLD_CSW_WINNER
{
	uint16  m_oldClanID;

	_CASTELLAN_OLD_CSW_WINNER()
	{
		Initialize();
	}

	void Initialize()
	{
		m_oldClanID = 0;
	}
};

struct _CASTELLAN_WAR_INFO
{
	uint16  m_tClanID;
	uint16  m_tTotalKills;
	uint8	m_tBarracksKills;
	uint8	m_tRemainBarracks;

	_CASTELLAN_WAR_INFO()
	{
		Initialize();
	}

	void Initialize()
	{
		m_tClanID = 0;
		m_tTotalKills = 0,
		m_tBarracksKills = 0;
		m_tRemainBarracks = 0;
	}
};

struct _DEATHMATCH_WAR_INFO
{
	uint16  m_tClanID;
	uint16  m_tTotalKills;
	uint8	m_tBarracksKills;
	uint8	m_tRemainBarracks;
	int16   m_tMark;
	std::string  m_tClanName;

	_DEATHMATCH_WAR_INFO()
	{
		Initialize();
	}

	void Initialize()
	{
		m_tClanName.clear();
		m_tClanID = 0;
		m_tTotalKills = 0,
		m_tBarracksKills = 0;
		m_tRemainBarracks = 0;
		m_tMark = 0;
	}
};

struct _CHAOS_STONE_INFO
{
	uint8  sChaosIndex;
	uint16 sChaosID;
	uint16 sZoneID;
	uint8  sRank;
	uint32 sSpawnTime;
	uint8  sMonsterFamily;
	uint16 sBoosID[10];
	uint8  sBoosKilledCount;
	bool isChaosStoneKilled;
	bool isTotalKilledMonster;
	bool isOnResTimer;
	bool ChaosStoneON;

	_CHAOS_STONE_INFO()
	{
		Initialize();
	}

	void Initialize()
	{
		memset(&sBoosID, 0, sizeof(sBoosID));
		sChaosIndex = 0;
		sChaosID = 0;
		sZoneID = 0;
		sRank = 1;
		sSpawnTime = 30;
		sMonsterFamily = 0;
		sBoosKilledCount = 0;
		isChaosStoneKilled = false;
		isTotalKilledMonster = false;
		isOnResTimer = false;
		ChaosStoneON = false;
	}
};

struct _CHAOS_STONE_SUMMON_LIST
{
	uint32	nIndex;
	uint16	ZoneID;
	uint16	sSid;
	uint8	MonsterSpawnFamily;
};

struct _CHAOS_STONE_RESPAWN
{
	uint8	sIndex;
	uint16  sZoneID;
	uint8   isOpen;
	uint8	sRank;
	uint16  sChaosID;
	uint16  sCount;
	uint16  sSpawnX;
	uint16  sSpawnZ;
	uint8   sSpawnTime;
	uint16  sDirection;
	uint16  sRadiusRange;
};

struct _CHAOS_STONE_STAGE
{
	uint8 nIndex;
	uint16 ZoneID;
	uint8 nIndexFamily;
};

struct _SERVER_SETTINGS
{
	uint32 nServerNo;
	uint8 MaximumLevelChange;
	uint8 DropNotice;
	uint8 UpgradeNotice;
	uint8 UserMaxUpgradeCount;
	uint8 OpenQuestSkill;
};

struct _LOAD_QUEST_SKILL
{
	uint32 nIndex;
	uint16 nEventDataIndex;
};

struct _EVENT_TRIGGER
{
	uint32 nIndex;
	uint16 bNpcType;
	uint32 sNpcID;
	uint32 nTriggerNum;
};

struct _USER_DAILY_OP
{
	std::string strUserId;
	int32 ChaosMapTime;
	int32 UserRankRewardTime;
	int32 PersonalRankRewardTime;
	int32 KingWingTime;
	int32 WarderKillerTime1;
	int32 WarderKillerTime2;
	int32 KeeperKillerTime;
	int32 UserLoyaltyWingRewardTime;
};

struct _MONUMENT_INFORMATION
{
	uint16	sSid;
	uint16	sNid;
	int32	RepawnedTime;
	uint8	bLevel;
};

struct _MONSTER_CHALLENGE
{
	uint16 sIndex;
	uint8 bStartTime1;
	uint8 bStartTime2;
	uint8 bStartTime3;
	uint8 bLevelMin;
	uint8 bLevelMax;
};

struct _MINING_FISHING_ITEM
{
	uint32 nIndex;
	uint32 nTableType;
	uint32 nWarStatus;
	uint8 UseItemType;
	std::string nGiveItemName;
	uint32 nGiveItemID;
	uint32 nGiveItemCount;
	uint32 SuccessRate;
};

struct _MONSTER_CHALLENGE_SUMMON_LIST
{
	uint16 sIndex;
	uint8 bLevel;
	uint8 bStage;
	uint8 bStageLevel;
	uint16 sTime;
	uint16 sSid;
	uint16 sCount;
	uint16 sPosX;
	uint16 sPosZ;
	uint8 bRange;
};

struct _SOCCER_STATUS
{
	bool	m_SoccerActive;
	bool	m_SoccerTimer;
	int16	m_SoccerSocketID;
	uint16	m_SoccerTimers;
	uint16	m_SoccerTime;
	uint8	m_SoccerRedColour;
	uint8	m_SoccerBlueColour;
	uint8	m_SoccerBlueGool;
	uint8	m_SoccerRedGool;

	bool	isSoccerAktive() {return m_SoccerActive;}
	bool	isSoccerTime() {return m_SoccerTimer;}
};

struct _START_POSITION_RANDOM
{
	uint16 sIndex;
	uint8 ZoneID;
	uint16 PosX;
	uint16 PosZ;
	uint8 Radius;
};

struct _KN_START_POSITION_RANDOM
{
	uint16 sIndex;
	short PosX;
	short PosZ;
	uint8 GasStep;
};


struct _USER_ITEM
{
	uint32 nItemID;
	std::vector<uint64> nItemSerial;
};

struct _NATION_DATA
{	
	std::string	bCharName;
	uint8		bRace;
	uint8		bFace;
	uint16		sClass;
	uint16		nNum;
	uint32		nHair;
	uint16		sClanID;

	_NATION_DATA()
	{
		bRace = 0;
		bFace = 0;
		sClass = 0;
		nNum = 0;
		nHair = 0;
		sClanID = 0;
	}
};

struct _DELETED_ITEM
{
	uint32 nNum;
	uint32 sCount;
	uint32 iDeleteTime;

	_DELETED_ITEM()
	{
		nNum = 0;
		sCount = 0;
		iDeleteTime = 0;
	}
};

struct _BANISH_OF_WINNER
{
	uint32 sira;
	uint16 sSid;
	uint8  sNationID;
	uint8  sZoneID;
	uint16 sPosX;
	uint16 sPosZ;
	uint16 sCount;
	uint16 sRadius;
	uint16 sDeadTime;
};

struct _KNIGHTS_CASTELLAN_CAPE
{
	uint16 sClanID;
	uint16 sCape;
	uint8 sR;
	uint8 sG;
	uint8 sB;
	uint8 sActive;
	uint64 sRemainingTime;
};

enum BuffType
{
	BUFF_TYPE_NONE					= 0,
	BUFF_TYPE_HP_MP					= 1,
	BUFF_TYPE_AC					= 2,
	BUFF_TYPE_SIZE					= 3,
	BUFF_TYPE_DAMAGE				= 4,
	BUFF_TYPE_ATTACK_SPEED			= 5,
	BUFF_TYPE_SPEED					= 6,
	BUFF_TYPE_STATS					= 7,
	BUFF_TYPE_RESISTANCES			= 8,
	BUFF_TYPE_ACCURACY				= 9,
	BUFF_TYPE_MAGIC_POWER			= 10,
	BUFF_TYPE_EXPERIENCE			= 11,
	BUFF_TYPE_WEIGHT				= 12,
	BUFF_TYPE_WEAPON_DAMAGE			= 13,
	BUFF_TYPE_WEAPON_AC				= 14,
	BUFF_TYPE_LOYALTY				= 15,
	BUFF_TYPE_NOAH_BONUS			= 16,
	BUFF_TYPE_PREMIUM_MERCHANT		= 17,
	BUFF_TYPE_ATTACK_SPEED_ARMOR	= 18,  // Berserker
	BUFF_TYPE_DAMAGE_DOUBLE			= 19,  // Critical Point
	BUFF_TYPE_DISABLE_TARGETING		= 20,  // Smoke Screen / Light Shock
	BUFF_TYPE_BLIND					= 21,  // Blinding (Strafe)
	BUFF_TYPE_FREEZE				= 22,  // Freezing Distance
	BUFF_TYPE_INSTANT_MAGIC			= 23,  // Instantly Magic
	BUFF_TYPE_DECREASE_RESIST		= 24,  // Minor resist
	BUFF_TYPE_MAGE_ARMOR			= 25,  // Fire / Ice / Lightning Armor
	BUFF_TYPE_PROHIBIT_INVIS		= 26,  // Source Marking
	BUFF_TYPE_RESIS_AND_MAGIC_DMG	= 27,  // Elysian Web
	BUFF_TYPE_TRIPLEAC_HALFSPEED	= 28,  // Wall of Iron
	BUFF_TYPE_BLOCK_CURSE			= 29,  // Counter Curse
	BUFF_TYPE_BLOCK_CURSE_REFLECT	= 30,  // Curse Refraction
	BUFF_TYPE_MANA_ABSORB			= 31,  // Outrage / Frenzy
	BUFF_TYPE_IGNORE_WEAPON			= 32,  // Weapon cancellation
	BUFF_TYPE_VARIOUS_EFFECTS		= 33,  // ... whatever the event item grants.
	BUFF_TYPE_PASSION_OF_SOUL		= 35,  // Passion of the Soul
	BUFF_TYPE_FIRM_DETERMINATION	= 36,  // Firm Determination
	BUFF_TYPE_SPEED2				= 40,  // Cold Wave
	BUFF_TYPE_ARMORED				= 41,  // Armored Skin
	BUFF_TYPE_UNK_EXPERIENCE		= 42,  // unknown buff type, used for something relating to XP.
	BUFF_TYPE_ATTACK_RANGE_ARMOR	= 43,  // Inevitable Murderous
	BUFF_TYPE_MIRROR_DAMAGE_PARTY	= 44,  // Minak's Thorn
	BUFF_TYPE_DAGGER_BOW_DEFENSE	= 45,  // Eskrima
	BUFF_TYPE_STUN                  = 47,  // Lighting Skill 
	BUFF_TYPE_FISHING				= 48,  // Fishing Skill
	BUFF_TYPE_LOYALTY_AMOUNT		= 55,  // Santa's Present
	BUFF_TYPE_NO_RECALL				= 150, // "Cannot use against the ones to be summoned"
	BUFF_TYPE_REDUCE_TARGET			= 151, // "Reduction" (reduces target's stats, but enlarges their character to make them easier to attack)
	BUFF_TYPE_SILENCE_TARGET		= 152, // Silences the target to prevent them from using any skills (or potions)
	BUFF_TYPE_NO_POTIONS			= 153, // "No Potion" prevents target from using potions.
	BUFF_TYPE_KAUL_TRANSFORMATION	= 154, // Transforms the target into a Kaul (a pig thing), preventing you from /town'ing or attacking, but increases defense.
	BUFF_TYPE_UNDEAD				= 155, // User becomes undead, increasing defense but preventing the use of potions and converting all health received into damage.
	BUFF_TYPE_UNSIGHT				= 156, // Blocks the caster's sight (not the target's).
	BUFF_TYPE_BLOCK_PHYSICAL_DAMAGE	= 157, // Blocks all physical damage.
	BUFF_TYPE_BLOCK_MAGICAL_DAMAGE	= 158, // Blocks all magical/skill damage.
	BUFF_TYPE_UNK_POTION			= 159, // unknown potion, "Return of the Warrior", "Comeback potion", perhaps some sort of revive?
	BUFF_TYPE_SLEEP					= 160, // Zantman (Sandman)
	BUFF_TYPE_INVISIBILITY_POTION	= 163, // "Unidentified potion"
	BUFF_TYPE_GODS_BLESSING			= 164, // Increases your defense/max HP 
	BUFF_TYPE_HELP_COMPENSATION		= 165, // Compensation for using the help system (to help, ask for help, both?)
	BUFF_TYPE_BATTLE_CRY			= 171,

	BUFF_TYPE_IMIR_ROARS			= 167, // Imirs scream-  Physical damage-free zone is created for 20 seconds within a range of 10m from the user.
	BUFF_TYPE_LOGOS_HORNS			= 168, // Logos horns- Magical damage-free zone is created for 20 seconds within a range of 10m from the user.
	BUFF_TYPE_NP_DROP_NOAH			= 169, // Increase rates by some value ( NP, Drop, Noah etc.)
	BUFF_TYPE_SNOWMAN_TITI			= 170, // SnowMan titi
	BUFF_TYPE_INCREASE_ATTACK		= 172, // Attack increased by 5%
	BUFF_TYPE_REWARD_MASK			= 180,
	BUFF_TYPE_INCREASE_NP			= 162, // Np Increase Event Items
	BUFF_TYPE_DRAKEY				= 50,  // Drakey's Nemesis 
	BUFF_TYPE_DEVIL_TRANSFORM		= 49,  // Kurian/Porutu Devil Transformation
	BUFF_TYPE_GM_BUFF				= 46,  // 30 minutes 10 all stats rise
	BUFF_TYPE_BLESS_OF_TEMPLE		= 39,  // Bless of Temple (Attack Increase).
	BUFF_TYPE_FRAGMENT_OF_MANES		= 52,  // Lost Speed
	BUFF_TYPE_HELL_FIRE_DRAGON		= 37,  // Advent of Hellfire Dragon
	BUFF_TYPE_DIVIDE_ARMOR			= 53,

};

enum FlyingSantaOrAngel
{
	FLYING_NONE		= 0,
	FLYING_SANTA	= 1,
	FLYING_ANGEL	= 2
};

enum SurroundingUserOpCode
{
	UserInfo = 1, 
	UserInfoDetail = 2,
	UserInfoAll = 3,
	UserInfoShow = 4
};

// Start Achieve System

enum UserAchieveOpcodes
{
	AchieveError = 0,
	AchieveSuccess = 1,
	AchieveGiftRequest = 2,
	AchieveDetailShow = 3,
	AchieveSummary = 4,
	AchieveFailed = 5,
	AchieveStart = 6,
	AchieveStop = 7,
	AchieveChalengeFailed = 8,
	AchieveCountScreen = 9,
	AchieveCoverTitle = 16,
	AchieveSkillTitle = 17,
	AchieveCoverTitleReset = 18,
	AchieveSkillTitleReset = 19

};

enum UserAchieveStatTypes
{
	ACHIEVE_STAT_STR = 0,
	ACHIEVE_STAT_STA = 1,
	ACHIEVE_STAT_DEX = 2,
	ACHIEVE_STAT_INT = 3,
	ACHIEVE_STAT_CHA = 4,
	ACHIEVE_STAT_ATTACK = 5,
	ACHIEVE_STAT_DEFENCE = 6,
	ACHIEVE_STAT_COUNT = 7
};

enum UserAchieveMainTypes
{
	AchieveWar = 1,
	AchieveMonster = 2,
	AchieveCom = 3,
	AchieveNormal = 4
};

enum UserAchieveNormalTypes
{
	AchieveBecomeKing = 1,
	AchieveReachNP = 2,
	AchieveReachLevel = 3,
	AchieveBecomeSpecial = 4,
	AchieveKnightContribution = 5,
	AchieveDefeatMons = 8,
	AchieveBecomeKnightMember = 10,
};

enum UserAchieveComTypes
{
	AchieveRequireQuest = 1,
	AchieveRequireAchieve = 2
};

enum UserAchieveMonsterTypes
{
	AchieveDefeatMonster = 1,
	AchieveDefeatNpc = 2
};

enum UserAchieveWarTypes
{
	AchieveDefeatEnemy = 1,
	AchieveTakeRevange = 3,
	AchieveWinWar = 4,
	AchieveWinNationGuardBattle = 5,
	AchieveWinJuraid = 6,
	AchieveWinChaos1 = 7,
	AchieveWinChaos2 = 8,
	AchieveWinChaos3 = 9,
	AchieveWinCSW = 10,
	AchieveKillCountChaos = 11,
	AchieveDefeatMonsterRonarkL = 20,
	AchieveDrakiTowerFinished = 21
};

enum UserAchieveStatus
{
	AchieveChalenngeInComplete = 0,
	AchieveIncomplete = 1,
	AchieveFinished = 4,
	AchieveCompleted = 5
};

// Finish Achieve System

enum ExpSealOpcode
{
	SealExp_ON = 1,
	SealExp_OFF = 2
};

enum NationTransferOpcode
{
	NationWarStatus			= 1,
	NationOpenBox			= 2,
	NationSuccessTransfer	= 3
};

enum SheriffOpcode
{
	ReportFailed		= 0,
	SendPrison			= 2,
	Question			= 4,
	QuestionAnswer		= 5,
	QuestionNotAnswered = 6,
	Macro				= 7,
	ReportSuccess		= 9,
	VotingFailed		= 10,
	ReportAgree			= 12,
	ReportDisagree		= 13,
	ListOpen			= 14,
	Unknown				= 15,
	GiveAccolade		= 16,
	Unknown2			= 17,
	KingsInspector		= 18,
	RemoveAccolade		= 20
};

enum CharAllInfoOpcodes
{
	AllCharInfoOpcode = 1,
	AlreadyCharName = 2,
	ArrangeOpen = 3,
	ArrangeRecvSend = 4
};

struct _SHERIFF_STUFF
{
	std::string reportedName, ReportSheriffName, crime, SheriffYesA, SheriffYesB, SheriffYesC, SheriffNoA, SheriffNoB, SheriffNoC, m_Date;
	uint8  m_VoteYes;
	uint8  m_VoteNo;

	_SHERIFF_STUFF()
	{
		reportedName.clear();
		ReportSheriffName.clear();
		crime.clear();
		SheriffYesA.clear();
		SheriffYesB.clear();
		SheriffYesC.clear();
		SheriffNoA.clear();
		SheriffNoB.clear();
		SheriffNoC.clear();
		m_Date.clear();		
		m_VoteYes = m_VoteNo = 0;
	}
};

enum ClanBattle
{
	MatchStarted	 = 1,
	MatchLose		 = 2
};

enum GenieStatus
{
	GenieStatusInactive		= 0,
	GenieStatusActive		= 1
};

enum GenieSystemInfo
{
	GenieInfoRequest		 = 1,
	GenieUpdateRequest		 = 2
};

enum GenieNonAttackType
{
	GenieUseSpiringPotion	= 1,
	GenieLoadOptions		= 2,
	GenielSaveOptions		= 3,
	GenieStartHandle		= 4,
	GenieStopHandle			= 5,
	GenieRemainingTime		= 6,
	GenieActivated			= 7,
};

enum GenieAttackHandle
{
	GenieMove = 1,
	GenieRotate = 2,
	GenieMainAttack = 3,
	GenieMagic = 4
};
#define GAMESERVER_CONSOLE_PASSWORD "123"
#define TOURNAMENT_MIN_CLAN_COUNT 30
#define TOURNAMENT_MAX_CLAN_COUNT 50
#define TOURNAMENT_MIN_PARTY_COUNT 8
#define TOURNAMENT_MAX_PARTY_COUNT 8

struct _HOOK_SETTINGS
{
	uint8 ACS_Validation;
	uint8 Button_Facebook;
	uint8 Button_Discord;
	uint8 Button_Live;
	uint8 Button_Support;
	std::string URL_Facebook;
	std::string URL_Discord;
	std::string URL_Live;
	std::string URL_KCBayi;
	uint32 KCMerchant_MinPrice;
	uint32 KCMerchant_MaxPrice;
	uint8 State_TempItemList;
	uint8 State_StatReset;
	uint8 State_SkillReset;
	uint8 State_PUS;
};

struct _HOOK_PUS_ITEM
{
	uint32 sItemID;
	uint32 sPrice;
	uint32 sBuyCount;
	uint8 sType;
};

#include "../shared/database/structs.h"