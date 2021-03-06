#pragma once

enum ResistanceTypes
{
	NONE_R		= 0,
	FIRE_R		= 1,
	COLD_R		= 2,
	LIGHTNING_R	= 3,
	MAGIC_R		= 4,
	DISEASE_R	= 5,
	POISON_R	= 6,
	LIGHT_R		= 7,
	DARKNESS_R	= 8
};

enum MagicDamageType
{
	FIRE_DAMAGE			= 5,
	ICE_DAMAGE			= 6,
	LIGHTNING_DAMAGE	= 7
};

enum SkillMoral
{
	MORAL_SELF				= 1,
	MORAL_FRIEND_WITHME		= 2,
	MORAL_FRIEND_EXCEPTME	= 3,
	MORAL_PARTY				= 4,
	MORAL_NPC				= 5,
	MORAL_PARTY_ALL			= 6,
	MORAL_ENEMY				= 7,
	MORAL_ALL				= 8,
	MORAL_AREA_ENEMY		= 10,
	MORAL_AREA_FRIEND		= 11,
	MORAL_AREA_ALL			= 12,
	MORAL_SELF_AREA			= 13,
	MORAL_CLAN				= 14,
	MORAL_CLAN_ALL			= 15,
	MORAL_UNDEAD			= 16, // I don't think a lot of these made it to KO.
	MORAL_PET_WITHME		= 17,
	MORAL_PET_ENEMY			= 18,
	MORAL_ANIMAL1			= 19,
	MORAL_ANIMAL2			= 20,
	MORAL_ANIMAL3			= 21,
	MORAL_ANGEL				= 22,
	MORAL_DRAGON			= 23,
	MORAL_DRAWBRIDGE		= 24,
	MORAL_CORPSE_FRIEND		= 25,
	MORAL_CORPSE_ENEMY		= 26,
	MORAL_SIEGE_WEAPON		= 31, // must be using a siege weapon
	MORAL_ENEMY_PARTY		= 37,
	MORAL_EXTEND_DURATION	= 240
};

#define WARP_RESURRECTION		1		// To the resurrection point.

#define REMOVE_TYPE3			1
#define REMOVE_TYPE4			2
#define RESURRECTION			3
#define	RESURRECTION_SELF		4
#define REMOVE_BLESS			5
#define LIFE_CRYSTAL			6

#define CLASS_STONE_BASE_ID	 379058000

enum SkillUseResult
{
	Successful,
	Failed,
	NoValue,
	NoFunction
};

class Unit;
struct _MAGIC_TABLE;
class MagicInstance
{
public:
	uint8	bOpcode;
	uint32	nSkillID;
	_MAGIC_TABLE * pSkill;
	uint16 sSkillCasterZoneID;
	int16	sCasterID, sTargetID; 
	Unit	*pSkillCaster, *pSkillTarget;
	int16	sData[7];
	bool	bSendFail;	// When enabled (enabled by default), sends fail packets to the client.
	// This is not preferable in cases like scripted casts, as the script should handle the failure.
	bool	bIsRecastingSavedMagic;
	bool	bIsItemProc;
	bool	bIsRunProc;
	bool	bInstantCast;

	INLINE bool BuffType3() { 
		return nSkillID == 112575 
			|| nSkillID == 212575
			|| nSkillID == 112570
			|| nSkillID == 212570; }

	INLINE bool LightStunSkills() { return nSkillID == 109742 ||
		nSkillID == 110742 || 
		nSkillID == 209742 ||
		nSkillID == 210742 ||
		nSkillID == 110772 ||
		nSkillID == 210772; }

	INLINE bool LightStunSkillsNot() { return nSkillID == 189742 ||
		nSkillID == 190742 || 
		nSkillID == 289742 ||
		nSkillID == 290742 ||
		nSkillID == 190772 ||
		nSkillID == 290772; }

	INLINE bool ColdSkills() { return nSkillID == 109642 ||
		nSkillID == 110642 || 
		nSkillID == 209642 ||
		nSkillID == 210642 ||
		nSkillID == 110672 ||
		nSkillID == 210672; }

	INLINE bool ColdSkillsNot() { return nSkillID == 189642 ||
		nSkillID == 190642 || 
		nSkillID == 289642 ||
		nSkillID == 290642 ||
		nSkillID == 190672 ||
		nSkillID == 290672; }

	bool	bSkillSuccessful;

	uint32	nConsumeItem;

	MagicInstance() : bOpcode(MAGIC_EFFECTING), nSkillID(0), pSkill(nullptr), 
		sCasterID(-1), sTargetID(-1), pSkillCaster(nullptr), pSkillTarget(nullptr),
		bSendFail(true), bIsRecastingSavedMagic(false), bIsItemProc(false), bIsRunProc(false), bInstantCast(false),
		bSkillSuccessful(true), nConsumeItem(0)
	{
		memset(&sData, 0, sizeof(sData));
	}

	void Run();

	bool IsAvailable();
	SkillUseResult UserCanCast();
	SkillUseResult CheckSkillPrerequisites();

	bool CheckType3Prerequisites();
	bool CheckType4Prerequisites();
	bool CheckType6Prerequisites();

	bool ExecuteSkill(uint8 bType);
	bool ExecuteType1();	
	bool ExecuteType2();
	bool ExecuteType3();
	bool ExecuteType4();
	bool ExecuteType5();
	bool ExecuteType6();
	bool ExecuteType7();
	bool ExecuteType8();
	bool ExecuteType9();

	void Type3Cancel();
	void Type4Cancel();
	void Type6Cancel(bool bForceRemoval = false);
	void Type9Cancel(bool bRemoveFromMap = true);
	void Type4Extend();

	short GetMagicDamage(Unit *pTarget, int total_hit, int attribute);
	int32 GetWeatherDamage(int32 damage, int attribute);
	void ReflectDamage(int32 damage, Unit * pTarget);
	bool isStaffSkill(bool isExtended = false);
	bool isRushSkill(bool isExtended = false);
	bool isExtendedArcherSkill(bool isExtended = false);
	bool isUnderTheCastleDisableSkill();
	void ConsumeItem();

	bool isDKMToMonsterDamageSkills();
	bool isDKMToUserDamageSkills();

	void SendSkillToAI();
	void BuildSkillPacket(Packet & result, int16 sSkillCaster, int16 sSkillTarget, int8 opcode, uint32 nSkillID, int16 sData[7]);
	void BuildAndSendSkillPacket(Unit * pUnit, bool bSendToRegion, int16 sSkillCaster, int16 sSkillTarget, int8 opcode, uint32 nSkillID, int16 sData[7]);
	void SendSkill(bool bSendToRegion = true, Unit * pUnit = nullptr);
	void SendSkillFailed(int16 sTargetID = -1);
	void SendSkillNoFunction(int16 sTargetID = -1);
	void SendSkillNotEffect();
	void SendTransformationList();
};