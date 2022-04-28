#pragma once

#define MONSTER_SPEED	1500

class CNpcTable
{
public:
	uint16	m_sSid;				// MONSTER(NPC) Serial ID
	std::string	m_strName;		// MONSTER(NPC) Name
	uint16	m_sPid;				// MONSTER(NPC) Picture ID
	uint16	m_sSize;			// size of NPC (100 by default)
	uint32	m_iWeapon_1;		// Weapon 2
	uint32	m_iWeapon_2;		// Weapon 2
	uint8	m_byGroup;			// Group ID
	uint8	m_byActType;		// Acting Type (AGGRESSIVE or FRIENDLY ETC)
	uint8	m_tNpcType;			// NPC Type  0 : Monster  1 : Normal NPC
	uint8	m_byFamilyType;		// family type
	uint8	m_byRank;			// the rank (not used)
	uint8	m_byTitle;			// the title (not used)
	uint32	m_iSellingGroup;	// selling group of the NPC
	uint16	m_sLevel;			// level
	uint32	m_iExp;				// the Exp Amount
	uint32	m_iLoyalty;			// the Loyalty Amount

	uint32	m_MaxHP;			// Max HP
	uint16	m_MaxMP;			// Max MP

	uint16	m_sAttack;			//
	uint16	m_sDefense;			//
	uint16	m_sHitRate;			//
	uint16	m_sEvadeRate;		//
	uint16	m_sDamage;			//
	uint16	m_sAttackDelay;		//
	uint16	m_sSpeed;			//
	uint8	m_bySpeed_1;		//
	uint8	m_bySpeed_2;		// 
	uint16	m_sStandTime;		// 
	uint32	m_iMagic1;			// 
	uint32	m_iMagic2;			// 
	uint32	m_iMagic3;			// 

	uint16	m_byFireR;			// 
	uint16	m_byColdR;			// 
	uint16	m_byLightningR;		// 
	uint16	m_byMagicR;			// 
	uint16	m_byDiseaseR;		// 
	uint16	m_byPoisonR;		// 

	float	m_fBulk;

	uint8	m_bySearchRange;	//
	uint8	m_byAttackRange;	// 
	uint8	m_byTracingRange;	//

	uint32	m_iMoney;			//
	uint16	m_iItem;			//
	uint8	m_byDirectAttack;	// 
	uint8	m_byMagicAttack;	//
	uint8	m_byGroupSpecial;

	CNpcTable() : m_sSpeed(MONSTER_SPEED)
	{
	}
};