#include "stdafx.h"

#pragma region WIZ_KNIGHTS_PROCESS
#if 0
WIZ_KNIGHTS_PROCESS: Opcode = 22 : The clan battle has begun.
Packet x(WIZ_KNIGHTS_PROCESS, uint8(22));
Send(&x);

WIZ_KNIGHTS_PROCESS: Opcode = 23 : The clan battle Records
Packet x(WIZ_KNIGHTS_PROCESS, uint8(23));
x << uint8(Type) << uint16(Win) << uint16(Loss);
Send(&x);

WIZ_KNIGHTS_PROCESS: Opcode = 24 : The clan battle Records Chat Notice.
Packet x(WIZ_KNIGHTS_PROCESS, uint8(23));
x << uint8(Type) << uint16(Win) << uint16(Loss);
Send(&x);

WIZ_KNIGHTS_PROCESS: Opcode = 81 :
1.You don't have the authorization
2.This command cannot be used.
3.Please confirm the exact character ID.
Packet x(WIZ_KNIGHTS_PROCESS, uint8(81));
x << uint16(3);;
Send(&x);

WIZ_KNIGHTS_PROCESS: Opcode = 84 : Deleted succesfuly
Packet x(WIZ_KNIGHTS_PROCESS, uint8(84));
x << uint8(1);;
Send(&x);

WIZ_KNIGHTS_PROCESS: Opcode = 85 : Deleted succesfuly
Packet x(WIZ_KNIGHTS_PROCESS, uint8(84));
x << uint8(1);;
Send(&x);

WIZ_KNIGHTS_PROCESS: Opcode = 86 :
1.Notice
2.Clan not found
3.Selected character is not the commander.
4.Less than 5 minutes have elapsed since your last advertisement.
5.There is no registered advertisement.
6.Not registered on the forum.
7.Clan advertisement cannot be announced during Lunar War hours.
Packet x(WIZ_KNIGHTS_PROCESS, uint8(86));
x.SByte();
x << uint16(1) << "ClanName" << "LeaderName";
Send(&x);

Packet x(WIZ_KNIGHTS_PROCESS, uint8(87));
x.DByte();
x << uint8(1) << "ClanName1" << "ClanName1";
Send(&x);

#endif
#pragma endregion

#pragma region WIZ_MAP_EVENT
#if 0

WIZ_MAP_EVENT: Opcode = 3 :
1.Karus
2.El Morad
Packet x(WIZ_MAP_EVENT, uint8(3));
x << uint8(1);;
Send(&x);

WIZ_MAP_EVENT: Opcode = 4 : You have 0 National Point.You can recharge your national point by visiting the Knight Clerk in the main village.
Packet x(WIZ_MAP_EVENT, uint8(4));
x << uint16(1);
Send(&x);

WIZ_MAP_EVENT: Opcode = 4 : *Called* Karus %d : El Morad %d       %s has succeeded in occupying Nereidth.
Packet x(WIZ_MAP_EVENT, uint8(5));
x << uint16(1);
Send(&x);

WIZ_MAP_EVENT: Opcode = 7 : Number of people infiltrated main castle
Packet x(WIZ_MAP_EVENT, uint8(7));
x << uint16(1);
Send(&x);

Packet x(WIZ_MAP_EVENT, uint8(10));
x.SByte();
x << uint16(1);//"asdas" << "sdasdas";
Send(&x);

#endif // 0
#pragma endregion

#pragma region WIZ_EVENT
#if 0
result.Initialize(WIZ_SELECT_MSG); // kule savaþý
result << uint16(0)
<< uint8(7)
<< uint64(0)
<< uint32(0x0A)
<< uint8(233)
<< uint16(6)
<< uint16(UNIXTIME - 50);
pUser->Send(&result);

result.Initialize(WIZ_EVENT);
result << uint8(60)
<< uint8(233)
<< uint8(3)
<< uint16(0)
<< uint16(0)
<< uint32(10)
<< uint32(UNIXTIME - 50);
pUser->Send(&result);

result.Initialize(WIZ_BIFROST);
result << uint8(5)
<< uint16(10);
pUser->Send(&result);

result.Initialize(WIZ_EVENT);//kule savaþý.
result << uint8(59)
<< uint8(233)
<< uint8(3)
<< uint16(0)
<< uint16(0)
<< uint32(10)
<< uint32(UNIXTIME - 50);
pUser->Send(&result);
#endif // 0
#pragma endregion
