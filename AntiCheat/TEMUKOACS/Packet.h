#pragma once

#include "ByteBuffer.h"

using namespace std;

class Packet : public ByteBuffer
{
public:
	INLINE Packet() : ByteBuffer(), m_opcode(0) { }
	INLINE Packet(uint8 opcode) : ByteBuffer(4096), m_opcode(opcode) {}
	INLINE Packet(uint8 opcode, size_t res) : ByteBuffer(res), m_opcode(opcode) {}
	INLINE Packet(const Packet& packet) : ByteBuffer(packet), m_opcode(packet.m_opcode) {}
	INLINE Packet(uint8 opcode, uint8 subOpcode) : m_opcode(opcode)
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
	INLINE void SetOpcode(uint8 opcode) { m_opcode = opcode; }

protected:
	uint8 m_opcode;
};

static void _string_format(const std::string fmt, std::string * result, va_list args)
{
	char buffer[1024];
	_vsnprintf(buffer, sizeof(buffer), fmt.c_str(), args);
	*result = buffer;
}

static string string_format(const std::string fmt, ...)
{
	std::string result;
	va_list ap;

	va_start(ap, fmt);
	_string_format(fmt, &result, ap);
	va_end(ap);

	return result;
}

static void tstring_format(const std::string fmt, std::string* result, va_list args)
{
	char buffer[1024];
	_vsnprintf_s(buffer, sizeof(buffer), fmt.c_str(), args);
	*result = buffer;
}
