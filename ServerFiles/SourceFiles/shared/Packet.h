#pragma once

#include "ByteBuffer.h"

class Packet : public ByteBuffer
{
public:
	INLINE Packet() : ByteBuffer(), m_opcode(0), m_sOwnerID(-1) { }
	INLINE Packet(uint8 opcode) : ByteBuffer(4096), m_opcode(opcode), m_sOwnerID(-1) {}
	INLINE Packet(uint8 opcode, size_t res) : ByteBuffer(res), m_opcode(opcode), m_sOwnerID(-1) {}
	INLINE Packet(const Packet &packet) : ByteBuffer(packet), m_opcode(packet.m_opcode), m_sOwnerID(-1) {}
	INLINE Packet(uint8 opcode, uint8 subOpcode) : m_opcode(opcode), m_sOwnerID(-1)
	{
		append(&subOpcode, 1);
	}

	//! Clear packet and set opcode all in one mighty blow
	INLINE void Initialize(uint8 opcode)
	{
		clear();
		m_opcode = opcode;
	}

	INLINE uint8 GetOpcode() const { return m_opcode; }
	INLINE int16 GetOwnerID() { return m_sOwnerID; }
	INLINE void SetOpcode(uint8 opcode) { m_opcode = opcode; }
	INLINE void SetOwnerID(int16 ownerid) { m_sOwnerID = ownerid; }
protected:
	uint8 m_opcode;
	int16 m_sOwnerID;
};
