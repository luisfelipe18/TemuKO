#pragma once
#include "stdafx.h"
#include "SocketDefines.h"
#include "CircularBuffer.h"
#include "atomic.h"

class SockEngine
{
	SOCKET server;
	CircularBuffer readBuffer, writeBuffer;
	OverlappedStruct m_readEvent, m_writeEvent;
	bool BurstSend(const uint8* Bytes, uint32 Size);
	void BurstPush();
	void WriteCallback();
public:
	SockEngine(SOCKET sv);
	bool SendPacket(Packet* pkt);
	INLINE CircularBuffer& GetReadBuffer() { return readBuffer; }
	INLINE CircularBuffer& GetWriteBuffer() { return writeBuffer; }
};