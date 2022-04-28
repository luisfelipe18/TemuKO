#include "stdafx.h"

void CUser::RentalSystem(Packet & pkt)
{
	if (isDead() || !isInGame())
		return;

	uint8 opcode = pkt.read<uint8>();

	printf("Rental unhandled packets %d \n", opcode);
	TRACE("Rental unhandled packets %d \n", opcode);
}

