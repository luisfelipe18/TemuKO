#pragma once

static float surround_fx[8] = {0.0f, -1.4142f, -2.0f, -1.4167f,  0.0f,  1.4117f,  2.0000f, 1.4167f};
static float surround_fz[8] = {2.0f,  1.4142f,  0.0f, -1.4167f, -2.0f, -1.4167f, -0.0035f, 1.4117f};


#define NO_ACTION				0	// the state of monster where it does no action
#define ATTACK_TO_TRACE			1	// the state of monster where it attacks or traces

#define LONG_ATTACK_RANGE		30	// the range of long attack
#define SHORT_ATTACK_RANGE		3	// the range of short attack

#define ARROW_MIN				391010000
#define ARROW_MAX				392010000

#define FAINTING_TIME			2 // in seconds


#define NPC_MAX_PATH_LIST		100
//
//	Defines About Communication
//
#define MAX_SOCKET				100
#define MAX_PATH_LINE			100

#define MAX_NPC_SIZE			30
#define MAX_WEAPON_NAME_SIZE	40
#define VIEW_DIST				48		// the distance that a monster can see forward
#define MAX_UPGRADE_WEAPON		12

// Npc InOut
#define NPC_IN					0X01
#define NPC_OUT					0X02

#define TILE_SIZE		4
#define CELL_SIZE		4

#define COMPARE(x,min,max) ((x>=min)&&(x<max))

struct _NpcPosition
{
	typedef struct { long x; long y; } Point;

	uint8	byType;			// type
	uint8	bySpeed;		// speed
	Point	pPoint;			// position
	float fXPos;
	float fZPos;
};

//
//	About USER
//
#define AI_USER_DEAD				0X00
#define AI_USER_LIVE				0X01

//
//	About NPC
//
#define NPC_NUM						20
#define MAX_DUNGEON_BOSS_MONSTER	20

#define NPC_PASSIVE					150
#define NPC_MAX_MOVE_RANGE			100
//
//	About Map Object
//
#define USER_BAND				0			// Map ID User
#define NPC_BAND				10000		// Map ID Npc 
#define INVALID_BAND			60000		// Invalid ID BAND 

//
//  Item
//
#define TYPE_MONEY				0
#define TYPE_ITEM				1

////////////////////////////////////////////////////////////
// Durability Type
#define ATTACK				0x01
#define DEFENCE				0x02
////////////////////////////////////////////////////////////

#define GREAT_SUCCESS			0X01		// 대성공
#define SUCCESS					0X02		// 성공
#define NORMAL					0X03		// 보통
#define	FAIL					0X04		// 실패

////////////////////////////////////////////////////////////
// DIRECTION DEFINITIONS
#define DIR_DOWN			0			
#define	DIR_DOWNLEFT		1
#define DIR_LEFT			2
#define	DIR_UPLEFT			3
#define DIR_UP				4
#define DIR_UPRIGHT			5
#define DIR_RIGHT			6
#define	DIR_DOWNRIGHT		7
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Npc Type
// Monster는 0부터 시작 10까지의 타입
#define NPCTYPE_MONSTER				0	// monster

// Attack Type
#define DIRECT_ATTACK		0
#define LONG_ATTACK			1
#define MAGIC_ATTACK		2
#define DURATION_ATTACK		3

#define NORMAL_OBJECT		0
#define SPECIAL_OBJECT		1

// Battlezone Announcement
#define BATTLEZONE_OPEN         0x00
#define BATTLEZONE_CLOSE        0x01           
#define DECLARE_WINNER          0x02

#define MAX_PATH_SIZE			100

#define NPC_MAX_USER_LIST		5

#define NPC_ATTACK_SHOUT		0
#define NPC_SUBTYPE_LONG_MON	1

#define NPC_TRACING_STEP		100

#define NPC_HAVE_USER_LIST		5

#define NPC_PATTEN_LIST			5
#define NPC_PATH_LIST			50
#define NPC_MAX_PATH_LIST		100
#define NPC_EXP_RANGE			50
#define NPC_EXP_PERSENT			50

#define NPC_SECFORMETER_MOVE	4
#define NPC_SECFORMETER_RUN		4
#define NPC_VIEW_RANGE			100

#define MAX_MAGIC_TYPE3			20
#define MAX_MAGIC_TYPE4			9
enum
{
	TENDER_ATTACK_TYPE		= 0, // The spawn is passive, i.e. won't attack until it's attacked, or it expects same-type spawns to help out.
	ATROCITY_ATTACK_TYPE	= 1  // The spawn is aggressive, i.e. it's just as happy to attack you first.
};

typedef std::map<uint16, uint32> DamagedUserList;

struct _K_NPC_POS
{
	uint8 bZoneID;
	uint16 sSid;	
	uint8 bActType;
	uint8 bRegenType;
	uint8 bDungeonFamily;
	uint8 bSpecialType;
	uint8 bTrapNumber;
	int32 iLeftX;
	int32 iTopZ;
	int32 iRightX;
	int32 iBottomZ;
	int32 iLimitMinX;
	int32 iLimitMinZ;
	int32 iLimitMaxX;
	int32 iLimitMaxZ;
	uint8 bNumNpc;
	uint16 sRegTime;
	uint8 bDirection;
	uint8 bDotCnt;
	char szPath[500];
	uint16 sRoom;

	_K_NPC_POS()
	{
		memset(&szPath, 0, sizeof(szPath));
	}
};

#define LOOT_GROUP_ITEMS	7
#define LOOT_DROP_ITEMS		7

struct _MONSTER_DROP_MAIN
{
	uint16 MonsterID;
	uint8  MonsterOpenDrop;
	uint32 MonsterMoney;
	uint32 ItemListID[LOOT_GROUP_ITEMS];
	uint32 ItemListPersent[LOOT_GROUP_ITEMS];
	uint32 ItemListCount[LOOT_GROUP_ITEMS];
	uint32 ItemSingleID[LOOT_GROUP_ITEMS];
	uint32 ItemSinglePersent[LOOT_GROUP_ITEMS];
	uint32 ItemSingleCount[LOOT_GROUP_ITEMS];

	_MONSTER_DROP_MAIN()
	{
		memset(&ItemListID, 0, sizeof(ItemListID));
		memset(&ItemListPersent, 0, sizeof(ItemListPersent));
		memset(&ItemListCount, 0, sizeof(ItemListCount));
		memset(&ItemSingleID, 0, sizeof(ItemSingleID));
		memset(&ItemSinglePersent, 0, sizeof(ItemSinglePersent));
		memset(&ItemSingleCount, 0, sizeof(ItemSingleCount));
	}
};

struct _MONSTER_GROUP_ITEM_LIST
{
	uint32	GroupNumber;
	std::vector<uint32> ItemID;
};

struct _K_MONSTER_ITEM
{
	uint16 sIndex;
	uint32 iItem[LOOT_DROP_ITEMS];
	uint16 sPercent[LOOT_DROP_ITEMS];

	_K_MONSTER_ITEM()
	{
		memset(&iItem, 0, sizeof(iItem));
		memset(&sPercent, 0, sizeof(sPercent));
	}
};

struct _K_NPC_ITEM
{
	uint16 sIndex;
	uint32 iItem[LOOT_DROP_ITEMS];
	uint16 sPercent[LOOT_DROP_ITEMS];

	_K_NPC_ITEM()
	{
		memset(&iItem, 0, sizeof(iItem));
		memset(&sPercent, 0, sizeof(sPercent));
	}
};

struct _MAKE_ITEM_GROUP
{
	uint32	iItemGroupNum;
	std::vector<uint32> iItems;
};

struct _MAKE_WEAPON
{
	uint8	byIndex;
	uint16	sClass[MAX_UPGRADE_WEAPON];
	_MAKE_WEAPON() { memset(&sClass, 0, sizeof(sClass)); }
};

struct _MAKE_ITEM_GRADE_CODE
{
	uint8	byItemIndex;		// item grade
	uint16	sGrade[9];
};	

struct _MAKE_ITEM_LARE_CODE
{
	uint8	byItemLevel;
	uint16	sLareItem;
	uint16	sMagicItem;
	uint16	sGeneralItem;
};

enum NpcSpecialType
{
	NpcSpecialTypeNone				= 0,
	NpcSpecialTypeCycleSpawn		= 7,
	NpcSpecialTypeKarusWarder1		= 90,
	NpcSpecialTypeKarusWarder2		= 91,
	NpcSpecialTypeElmoradWarder1	= 92,
	NpcSpecialTypeElmoradWarder2	= 93,
	NpcSpecialTypeKarusKeeper		= 98,
	NpcSpecialTypeElmoradKeeper		= 99
};


enum MonSearchType
{
	MonSearchSameFamily,	// find any available mobs of the same family
	MonSearchAny,			// find any available mob
	MonSearchNeedsHealing	// find any mob that needs healing
};

enum CloseTargetResult
{
	CloseTargetInvalid,
	CloseTargetNotInRange,
	CloseTargetInGeneralRange,
	CloseTargetInAttackRange
};

struct _Target
{
	uint16	id;						// Target User uid
	bool bSet;
	float x;						// User-target x pos
	float y;						// User-target y pos
	float z;						// User-target z pos
	ULONGLONG m_tLastDamageTime;
};

struct _PattenPos
{
	short x;
	short z;
};

struct _PathList
{
	_PattenPos pPatternPos[NPC_MAX_PATH_LIST];
};

struct _TargetHealer
{
	short	sNID;				// npc nid
	short	sValue;				// 점수
};

struct  _NpcSkillList
{
	short	sSid;
	uint8	tLevel;
	uint8	tOnOff;
};

struct  _NpcGiveItem
{
	int 	sSid;			// item serial number
	short	count;			// item count
};

struct _NPC_LIVE_TIME
{
	uint16 nIndex;
	int16 SocketID;
	uint16 Nid;
	uint16 Duration;
	int32 SpawnedTime;

};

struct _RANDOM_ITEM
{
	uint16 Number;
	std::string strName;
	uint32 ItemID;
	uint32 Percent;
	uint32 Days;
	uint32 Coin;
	uint32 Count;
};
