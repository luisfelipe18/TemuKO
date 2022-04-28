#pragma once
#include "Types.h"
#include <string>

#include "N3UIIcon.h"

typedef struct __TABLE_ITEM_BASIC
{
	uint32_t Num;
	uint8_t extNum;
	std::string strName;
	std::string Description;
	uint32_t UNK0;
	uint8_t UNK1;
	uint32_t IconID;
	uint32_t dxtID;
	uint32_t UNK2;
	uint32_t UNK3;
	uint8_t Kind;
	uint8_t UNK4;
	uint8_t Slot;
	uint8_t UNK5;
	uint8_t Class;
	short Attack;
	short Delay;
	short Range;
	short Weight;
	short Duration;
	int repairPrice;
	int sellingPrice;
	short AC;
	uint8_t isCountable;
	uint32_t Effect1;
	uint32_t Effect2;
	byte ReqLevelMin;
	byte ReqLevelMax;
	uint8_t UNK6;
	uint8_t UNK7;
	uint8_t ReqStr;
	uint8_t ReqHp;
	uint8_t ReqDex;
	uint8_t ReqInt;
	uint8_t ReqMp;
	uint8_t SellingGroup;
	uint8_t ItemGrade;
	int UNK8;
	short UNK9;
	uint32_t UNK10;
	uint8_t UNK11;
} TABLE_ITEM_BASIC;
typedef struct __TABLE_ITEM_EXT
{
	uint32_t extensionID; 
	std::string szHeader; 
	uint32_t baseItemID; 
	std::string Description; 
	uint32_t EffectID; 
	uint32_t dxtID; 
	uint32_t iconID; 
	uint8_t byMagicOrRare; 
	short siDamage; 
	short siAttackIntervalPercentage; 
	short siHitRate; 
	short siEvationRate; 
	short siMaxDurability; 
	short siPriceMultiply; 
	short siDefense; 
	short siDefenseRateDagger; 
	short siDefenseRateSword; 
	short siDefenseRateBlow; 
	short siDefenseRateAxe; 
	short siDefenseRateSpear; 
	short siDefenseRateArrow; 
	short siDefenseRateJamadar; 
	uint8_t byDamageFire; 
	uint8_t byDamageIce; 
	uint8_t byDamageThuner; 
	uint8_t byDamagePoison; 
	uint8_t byStillHP; 
	uint8_t byDamageMP; 
	uint8_t byStillMP; 
	uint8_t byReturnPhysicalDamage; 
	uint8_t bySoulBind; 
	short siBonusStr; 
	short siBonusSta; 
	short siBonusDex; 
	short siBonusInt; 
	short siBonusMagicAttak; 
	short siBonusHP; 
	short siBonusMSP;  
	short siRegistFire; 
	short siRegistIce; 
	short siRegistElec; 
	short siRegistMagic; 
	short siRegistPoison; 
	short siRegistCurse; 
	short dwEffectID1; 
	uint32_t dwEffectID2; 
	uint32_t siNeedLevel; 
	short siNeedRank; 
	short siNeedTitle; 
	short siNeedStrength; 
	short siNeedStamina; 
	short siNeedDexterity; 
	short siNeedInteli; 
	short siNeedMagicAttack; 
	uint8_t UNK1; 
	short UNK2; 
} TABLE_ITEM_EXT;

typedef struct __TABLE_UPC_SKILL
{
	uint32_t		dwID;				
	std::string	szEngName;			
	std::string	szName;				
	std::string	szDesc;				
	int			iSelfAnimID1;		
	int			iSelfAnimID2;		

	int			idwTargetAnimID;	
	int			iSelfFX1;			
	int			iSelfPart1;			
	int			iSelfFX2;			
	int			iSelfPart2;			
	int			iFlyingFX;			
	int			iTargetFX;			

	int			iTargetPart;		
	int			iTarget;			
	int			iNeedLevel;			
	int			iNeedSkill;			
	int			iExhaustMSP;		

	int			iExhaustHP;			
	uint32_t		dwNeedItem;			
	uint32_t		dwExhaustItem;
	int			iCastTime;			
	int			iReCastTime;		

	float fIDK0; 
	float fIDK1; 

	int			iPercentSuccess;	
	uint32_t		dw1stTableType;		
	uint32_t		dw2ndTableType;		
	int			iValidDist;			

	int	iIDK2; 

} TABLE_UPC_ATTACK_B;

struct	__IconItemSkill
{
	CN3UIImage* pUIIcon;
	std::string				szIconFN;
	int						index;

	union
	{
		struct
		{
			__TABLE_ITEM_BASIC* pItemBasic;
			__TABLE_ITEM_EXT* pItemExt;
			int					iCount;
			int					iDurability;
		};

		__TABLE_UPC_SKILL* pSkill;
	};
};

enum e_PlayerType { PLAYER_BASE = 0, PLAYER_NPC = 1, PLAYER_OTHER = 2, PLAYER_MYSELF = 3 };

enum e_Race {
	RACE_ALL = 0,
	RACE_KA_ARKTUAREK = 1, RACE_KA_TUAREK = 2, RACE_KA_WRINKLETUAREK = 3, RACE_KA_PURITUAREK = 4,
	RACE_EL_BABARIAN = 11, RACE_EL_MAN = 12, RACE_EL_WOMEN = 13,
	
	RACE_NPC = 100,
	RACE_UNKNOWN = 0xffffffff
};

enum e_Class {
	CLASS_KINDOF_WARRIOR = 1, CLASS_KINDOF_ROGUE, CLASS_KINDOF_WIZARD, CLASS_KINDOF_PRIEST,
	CLASS_KINDOF_ATTACK_WARRIOR, CLASS_KINDOF_DEFEND_WARRIOR, CLASS_KINDOF_ARCHER, CLASS_KINDOF_ASSASSIN,
	CLASS_KINDOF_ATTACK_WIZARD, CLASS_KINDOF_PET_WIZARD, CLASS_KINDOF_HEAL_PRIEST, CLASS_KINDOF_CURSE_PRIEST,

	CLASS_KA_WARRIOR = 101, CLASS_KA_ROGUE, CLASS_KA_WIZARD, CLASS_KA_PRIEST, 
	CLASS_KA_BERSERKER = 105, CLASS_KA_GUARDIAN, CLASS_KA_HUNTER = 107, CLASS_KA_PENETRATOR,
	CLASS_KA_SORCERER = 109, CLASS_KA_NECROMANCER, CLASS_KA_SHAMAN = 111, CLASS_KA_DARKPRIEST,

	CLASS_EL_WARRIOR = 201, CLASS_EL_ROGUE, CLASS_EL_WIZARD, CLASS_EL_PRIEST, 
	CLASS_EL_BLADE = 205, CLASS_EL_PROTECTOR, CLASS_EL_RANGER = 207, CLASS_EL_ASSASIN,
	CLASS_EL_MAGE = 209, CLASS_EL_ENCHANTER, CLASS_EL_CLERIC = 211, CLASS_EL_DRUID,

	CLASS_UNKNOWN = 0xffffffff
};

enum e_Class_Represent { CLASS_REPRESENT_WARRIOR = 0, CLASS_REPRESENT_ROGUE, CLASS_REPRESENT_WIZARD, CLASS_REPRESENT_PRIEST, CLASS_REPRESENT_UNKNOWN = 0xffffffff };

enum e_ItemAttrib {
	ITEM_ATTRIB_GENERAL = 0,
	ITEM_ATTRIB_MAGIC = 1,
	ITEM_ATTRIB_LAIR = 2,
	ITEM_ATTRIB_CRAFT = 3,
	ITEM_ATTRIB_UNIQUE = 4,
	ITEM_ATTRIB_UPGRADE = 5,
	ITEM_ATTRIB_UNIQUE_REVERSE = 11,
	ITEM_ATTRIB_UPGRADE_REVERSE = 12,
	ITEM_ATTRIB_UNKNOWN = 0xffffffff
};

enum ItemAttrib
{
	ITEM_ATTR_NORMAL,
	ITEM_ATTR_MAGIC,
	ITEM_ATTR_RARE,
	ITEM_ATTR_CRAFT,
	ITEM_ATTR_UNIQUE,
	ITEM_ATTR_UPGRADE,
	ITEM_ATTR_EVENT,
	ITEM_ATTR_PET,
	ITEM_ATTR_COSPRE,
	ITEM_ATTR_MINERVA,
	ITEM_ATTR_REBIRTH_UPGRADE = 11,
	ITEM_ATTR_REBIRTH_UNIQUE
};

enum e_ItemClass {
	ITEM_CLASS_DAGGER = 11, 
	ITEM_CLASS_SWORD = 21, 
	ITEM_CLASS_SWORD_2H = 22, 
	ITEM_CLASS_AXE = 31, 
	ITEM_CLASS_AXE_2H = 32, 
	ITEM_CLASS_MACE = 41, 
	ITEM_CLASS_MACE_2H = 42, 
	ITEM_CLASS_SPEAR = 51, 
	ITEM_CLASS_POLEARM = 52, 

	ITEM_CLASS_SHIELD = 60, 

	ITEM_CLASS_BOW = 70, 
	ITEM_CLASS_BOW_CROSS = 71, 
	ITEM_CLASS_BOW_LONG = 80, 

	ITEM_CLASS_EARRING = 91, 
	ITEM_CLASS_AMULET = 92, 
	ITEM_CLASS_RING = 93, 
	ITEM_CLASS_BELT = 94, 
	ITEM_CLASS_CHARM = 95, 
	ITEM_CLASS_JEWEL = 96, 
	ITEM_CLASS_POTION = 97, 
	ITEM_CLASS_SCROLL = 98, 

	ITEM_CLASS_LAUNCHER = 100, 

	ITEM_CLASS_STAFF = 110, 
	ITEM_CLASS_ARROW = 120, 
	ITEM_CLASS_JAVELIN = 130, 

	ITEM_CLASS_ARMOR_WARRIOR = 210, 
	ITEM_CLASS_ARMOR_ROGUE = 220, 
	ITEM_CLASS_ARMOR_MAGE = 230, 
	ITEM_CLASS_ARMOR_PRIEST = 240, 

	ITEM_CLASS_ETC = 251, 

	ITEM_CLASS_UNKNOWN = 0xffffffff
}; 

struct __TABLE_ITEM_BASIC;
struct __TABLE_ITEM_EXT;
struct __TABLE_PLAYER;

enum e_Authority { AUTHORITY_MANAGER = 0, AUTHORITY_USER, AUTHORITY_NOCHAT, AUTHORITY_NPC, AUTHORITY_BLOCK_USER = 0xff };

struct __InfoPlayerOther
{
	int			iFace;			
	int			iHair;			

	int			iCity;			
	int			iKnightsID;		
	std::string szKnights;		
	int			iKnightsGrade;	
	int			iKnightsRank;	

	int			iRank;			
	int			iTitle;			

	void Init()
	{
		iFace = 0;			
		iHair = 0;			
		iCity;				
		iKnightsID = 0;		
		szKnights = "";		
		iKnightsGrade = 0;		
		iKnightsRank = 0;			
		iTitle = 0;			
	}
};

enum e_KnightsDuty {
	KNIGHTS_DUTY_UNKNOWN = 0,		
	KNIGHTS_DUTY_CHIEF = 1,			
	KNIGHTS_DUTY_VICECHIEF = 2,		
	KNIGHTS_DUTY_PUNISH = 3,		
	KNIGHTS_DUTY_TRAINEE = 4,		
	KNIGHTS_DUTY_KNIGHT = 5,		
	KNIGHTS_DUTY_OFFICER = 6		
};

struct __InfoPlayerMySelf : public __InfoPlayerOther
{
	int					iBonusPointRemain; 
	int					iLevelPrev; 

	int					iMSPMax;
	int					iMSP;

	int					iTargetHPPercent;
	int					iGold;
	Uint64				iExpNext;
	Uint64				iExp;
	int					iRealmPoint;		
	int					iRealmPointMonthly;		
	e_KnightsDuty		eKnightsDuty;		
	int					iWeightMax;			
	int					iWeight;			
	int					iStrength;			
	int					iStrength_Delta;		
	int					iStamina;			
	int					iStamina_Delta;		
	int					iDexterity;			
	int					iDexterity_Delta;	
	int					iIntelligence;		
	int					iIntelligence_Delta; 
	int 				iMagicAttak;		
	int 				iMagicAttak_Delta;	

	int 				iAttack;		
	int 				iAttack_Delta;	
	int 				iGuard;			
	int 				iGuard_Delta;	

	int 				iRegistFire;			
	int 				iRegistFire_Delta;		
	int 				iRegistCold;			
	int 				iRegistCold_Delta;		
	int 				iRegistLight;			
	int 				iRegistLight_Delta;		
	int 				iRegistMagic;			
	int 				iRegistMagic_Delta;		
	int 				iRegistCurse;			
	int 				iRegistCurse_Delta;		
	int 				iRegistPoison;			
	int 				iRegistPoison_Delta;	

	int					iZoneInit;				
	int					iZoneCur;				
	int					iVictoryNation;			

	void Init()
	{
		__InfoPlayerOther::Init();

		iBonusPointRemain = 0; 
		iLevelPrev = 0; 

		iMSPMax = 0;
		iMSP = 0;

		iTargetHPPercent = 0;
		iGold = 0;
		iExpNext = 0;
		iExp = 0;
		iRealmPoint = 0;		
		iRealmPointMonthly = 0; 
		eKnightsDuty = KNIGHTS_DUTY_UNKNOWN;		
		iWeightMax = 0;			
		iWeight = 0;			
		iStrength = 0;			
		iStrength_Delta = 0;	
		iStamina = 0;			
		iStamina_Delta = 0;		
		iDexterity = 0;			
		iDexterity_Delta = 0;	
		iIntelligence = 0;		
		iIntelligence_Delta = 0; 
		iMagicAttak = 0;		
		iMagicAttak_Delta = 0;	

		iAttack = 0;		
		iAttack_Delta = 0;	
		iGuard = 0;			
		iGuard_Delta = 0;	

		iRegistFire = 0;			
		iRegistFire_Delta = 0;		
		iRegistCold = 0;			
		iRegistCold_Delta = 0;		
		iRegistLight = 0;			
		iRegistLight_Delta = 0;		
		iRegistMagic = 0;			
		iRegistMagic_Delta = 0;		
		iRegistCurse = 0;			
		iRegistCurse_Delta = 0;		
		iRegistPoison = 0;			
		iRegistPoison_Delta = 0;	

		iZoneInit = 0x01;			
		iZoneCur = 0;				
		iVictoryNation = -1;		
	}
};
