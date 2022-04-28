#include "stdafx.h"

/**
* @brief	Packet handler for skillbar requests.
*
* @param	pkt	The packet.
*/
void CUser::SkillDataProcess(Packet & pkt)
{
	uint8 opcode = pkt.read<uint8>();
	switch (opcode)
	{
	case SKILL_DATA_SAVE:
		SkillDataSave(pkt);
		break;

	case SKILL_DATA_LOAD:
		SkillDataLoad();
		break;
	default:
		printf("Skill Data unhandled packets %d \n",opcode);
		TRACE("Skill Data unhandled packets %d \n", opcode);
		break;
	}
}

/**
* @brief	Packet handler for saving a skillbar.
*
* @param	pkt	The packet.
*/
void CUser::SkillDataSave(Packet & pkt)
{
	Packet result(WIZ_SKILLDATA, uint8(SKILL_DATA_SAVE));
	uint16 sCount = pkt.read<uint16>();
	if (sCount == 0 || sCount > 64)
		return;

	result << sCount;
	for (int i = 0; i < sCount; i++)
		result << pkt.read<uint32>();

	g_pMain->AddDatabaseRequest(result, this);
}

/**
* @brief	Packet handler for loading a skillbar.
*/
void CUser::SkillDataLoad()
{
	Packet result(WIZ_SKILLDATA, uint8(SKILL_DATA_LOAD));
	g_pMain->AddDatabaseRequest(result, this);
}