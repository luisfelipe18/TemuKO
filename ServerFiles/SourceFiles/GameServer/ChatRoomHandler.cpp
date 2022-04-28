#include "stdafx.h"

enum Chatroomopcodes
{
	CHATROOM_LIST = 0x04,
	CHATROOM_CREATE = 0x05,
	CHATROOM_JOIN = 0x06,
	CHATROOM_LEAVE = 0x07,
	CHATROOM_SEARCH = 0x08,
	CHATROOM_UPDATECONFIG = 0x09,
	CHATROOM_MEMBEROPTION = 0x0B,
	CHATROOM_ADMIN = 0x0C,

	CHATROOM_MANUEL = 0x0B
};

void CUser::ChatRoomHandle(Packet & pkt )
{
	uint8 chatroomManuel = pkt.read<uint8>();

	if(chatroomManuel != CHATROOM_MANUEL)
		return;

	uint8 opcode = pkt.read<uint8>();

	switch(opcode)
	{
	case CHATROOM_LIST:
		ChatRoomList(pkt);
		break;
	case CHATROOM_CREATE:
		ChatRoomCreate(pkt);
		break;
	case CHATROOM_JOIN:
		ChatRoomJoin(pkt);
		break;
	case CHATROOM_LEAVE:
		ChatRoomLeave(pkt);
		break;
	case CHATROOM_ADMIN:
		ChatRoomAdmin(pkt);
		break;
	case CHATROOM_MEMBEROPTION:
		ChatRoomMemberoption(pkt);
		break;
	default:
		TRACE("Chatroom Handle Unknow : %d\n",opcode);
		printf("Chatroom Handle Unknow : %d\n", opcode);
		break;
	}
}

void CUser::ChatRoomList(Packet & pkt)
{
	Packet result(WIZ_NATION_CHAT,uint8(0x0B));

	uint16 count = 0;
	result << uint8(CHATROOM_LIST) << uint16(count);
	result.DByte();
	//190B04 01 0002 00 0500 726573756C 00 01 00 0100 0A00
	foreach_stlmap(itr, g_pMain->m_ChatRoomArray)
	{
		_CHAT_ROOM* pChatRoom = itr->second;

		if(pChatRoom == nullptr)
			continue;

		result << uint16(++count) 
			<< pChatRoom->strRoomName
			<< uint8(pChatRoom->isPassword())
			<< pChatRoom->m_bRoomNation
			<< uint8(0)
			<< pChatRoom->m_sCurrentUser
			<< pChatRoom->m_sMaxUser;
	}

	result.put(2,count);

	Send(&result);
}

void CUser::ChatRoomCreate(Packet & pkt )
{
	if(m_ChatRoomIndex < 1 )
		return;
	
	Packet result(WIZ_NATION_CHAT,uint8(0x0B));

	uint8 isPassword,unk,m_bResult; 
	uint16 RoomSize;
	std::string strRoomName , strPassword;

	//strPassword.empty();
	strPassword = strPassword.empty();

	pkt.DByte();
	pkt >> strRoomName >> isPassword ;

	m_bResult = 0;

	if(isPassword != 0)
		pkt >> strPassword;
	else 
		pkt >> unk;

	pkt >> RoomSize;

	_CHAT_ROOM* m_pRoom = new _CHAT_ROOM();

	m_pRoom->strRoomName = strRoomName;
	m_pRoom->m_bRoomNation = GetNation();
	m_pRoom->strAdministrator = m_strUserID;
	m_pRoom->m_sMaxUser = 10;
	m_pRoom->strPassword = strPassword;
	m_pRoom->nIndex = g_pMain->m_ChatRoomArray.GetSize() +1;
	m_pRoom->m_sCurrentUser = 0;

	if(!m_pRoom->AddUser(m_strUserID))
	{
		TRACE("Room creation fail 1\n");
		delete m_pRoom;
		goto return_fail;
	}

	bool check = false;
	g_pMain->m_ChatRoomArray.m_lock.lock();
	foreach_stlmap_nolock(itr,g_pMain->m_ChatRoomArray)
	{
		if(itr->second != nullptr && itr->second->strRoomName == strRoomName)
		{
			check = true;
			break;
		}
	}
	g_pMain->m_ChatRoomArray.m_lock.unlock();
	if(!g_pMain->m_ChatRoomArray.PutData(m_pRoom->nIndex,m_pRoom) || check)
	{
		TRACE("Room creation fail 2\n");
		delete m_pRoom;
		goto return_fail;
	}

	m_ChatRoomIndex = m_pRoom->nIndex;

	//190B 05 01 0200 0500 72 65 73 75 6C 00 0100 0A00 00000000
	
	result << uint8(0x05) << uint8(1) << uint16(m_pRoom->nIndex) << m_pRoom->strRoomName << uint8(isMeChatRoom(m_pRoom->nIndex)) << m_pRoom->m_sCurrentUser << m_pRoom->m_sMaxUser << uint32(0);

	Send(&result);
	return;

return_fail:
	result << uint8(0x05) << uint8(0);
	Send(&result);
}

void CUser::ChatRoomJoin(Packet & pkt)
{
	/*Result opcedes 
	 * 0 = Successfully
	 * 1 = Already in room
	 * 2 = Room does not exit ! 
	 * 3 = Password lenght is correct
	 * 4 = Password does not match
	 * 5 = Nation do not match */

	uint16 RoomID ;
	uint8 isPassword ;
	std::string strPassword ;

	pkt >> RoomID >> isPassword >> strPassword;

	_CHAT_ROOM* pRoom = g_pMain->m_ChatRoomArray.GetData(RoomID);

	uint8 nResult = 0;

	if(pRoom == nullptr ||
		pRoom->m_sMaxUser < pRoom->m_sCurrentUser+1)
		nResult = 2;
	else if(pRoom->isPassword() &&
		STRCASECMP(strPassword.c_str(), pRoom->strPassword.c_str()) != 0)
		nResult = 4;
	else if(!pRoom->AddUser(GetName()))
		nResult = 2;

	Packet result(WIZ_NATION_CHAT, uint8(CHATROOM_MANUEL));

	result << uint8(CHATROOM_JOIN) << nResult << RoomID;

	Send(&result);

	if(nResult == 0)
		m_ChatRoomIndex = pRoom->nIndex;
}                    

void CUser::ChatRoomLeave(Packet & pkt)
{
	uint16 RoomID  =  pkt.read<uint16>();
	_CHAT_ROOM* pRoom = g_pMain->m_ChatRoomArray.GetData(RoomID);

	if(pRoom == nullptr)
		return;

	// Kullanıcıları odadan atma 

	if(pRoom->isAdministrator(GetName()) == 2 )
		g_pMain->m_ChatRoomArray.DeleteData(RoomID);

	Packet result(WIZ_NATION_CHAT, uint8(CHATROOM_MANUEL));

	result << uint8(CHATROOM_LEAVE) << uint8(0);
	
	Send(&result);
}

void CUser::ChatRoomChat(std::string * strMessage, std::string strSender)
{
	CUser* pUser = g_pMain->GetUserPtr(strSender, TYPE_CHARACTER);

	if(pUser == nullptr) 
		return;

	Packet result(WIZ_CHAT,uint8(CHATROM_CHAT));
			result.DByte();
			result << uint8(0) << pUser->GetSocketID() << pUser->GetName() << *strMessage << pUser->GetZoneID();
	//10 21 00 0701 0B00 7370656369616C6C697374 0600 74656B726172 15

	SendChatRoom(result);
}

void CUser::ChatRoomAdmin(Packet & pkt)
{
	//19 0B 0C  00 0300 0500 726573756C 1802 0A00 0100 1802 0B00 4D617844616D616765336B

	Packet result(WIZ_NATION_CHAT,uint8(CHATROOM_MANUEL));
		
	uint16 count = 0 , subpkt = pkt.read<uint8>();

	result << uint8(CHATROOM_ADMIN) << uint8(0) << subpkt;

	_CHAT_ROOM* pRoom = g_pMain->m_ChatRoomArray.GetData(m_ChatRoomIndex);

	if(pRoom == nullptr)
		return;

	result.DByte();

	result << pRoom->strRoomName << uint8(0x18) << uint8(0x02) << pRoom->m_sMaxUser << pRoom->m_sCurrentUser  ;

	foreach(itr,pRoom->m_UserList)
	{
		CUser* pSendUser = g_pMain->GetUserPtr(itr->second,TYPE_CHARACTER);

		if(pSendUser == nullptr)
			continue;

		result << uint8(0x18) << uint8(pRoom->isAdministrator(pSendUser->GetName())) << pSendUser->GetName();		
	}
	Send(&result);
}

void CUser::ChatRoomMemberoption(Packet & pkt)
{
	uint8 subPkt = pkt.read<uint8>();

	if(subPkt == 1)
		ChatRoomKickOut(pkt.read<uint16>());
}

void CUser::ChatRoomKickOut(uint16 userID)
{
	_CHAT_ROOM* pRoom = g_pMain->m_ChatRoomArray.GetData(m_ChatRoomIndex);

	if(pRoom == nullptr)
		return;

	Packet result(WIZ_NATION_CHAT,uint8(CHATROOM_MANUEL));

	result << uint8(CHATROOM_MEMBEROPTION);

	foreach(itr,pRoom->m_UserList)
	{
		if(itr->first == userID)
		{
			result << uint8(2);
			Send(&result);
			return;
		}
	}

	pRoom->m_UserList.erase(userID);
	pRoom->m_sCurrentUser--;
	result << uint8(1) << userID;

	SendChatRoom(result);
}

void CUser::SendChatRoom(Packet & result)
{
	_CHAT_ROOM* pRoom = g_pMain->m_ChatRoomArray.GetData(m_ChatRoomIndex);

	if(pRoom == nullptr)
		return;

	foreach(itr,pRoom->m_UserList)
	{
		CUser* pSendUser = g_pMain->GetUserPtr(itr->second,TYPE_CHARACTER);

		if(pSendUser == nullptr)
			continue;

		pSendUser->Send(&result);
	}
}