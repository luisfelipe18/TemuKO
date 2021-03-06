#pragma once
#include "stdafx.h"
#include "atomic.h"

enum SocketIOEvent
{
	SOCKET_IO_EVENT_READ_COMPLETE = 0,
	SOCKET_IO_EVENT_WRITE_END = 1,
	SOCKET_IO_THREAD_SHUTDOWN = 2,
	NUM_SOCKET_IO_EVENTS = 3,
};

class OverlappedStruct
{
public:
	OVERLAPPED m_overlap;
	SocketIOEvent m_event;

	Atomic<bool> m_inUse;

	OverlappedStruct(SocketIOEvent ev) : m_event(ev)
	{
		memset(&m_overlap, 0, sizeof(OVERLAPPED));
		m_inUse = false;
	};

	OverlappedStruct()
	{
		memset(&m_overlap, 0, sizeof(OVERLAPPED));
		m_inUse = false;
	}

	INLINE void Reset(SocketIOEvent ev)
	{
		memset(&m_overlap, 0, sizeof(OVERLAPPED));
		m_event = ev;
	}

	void Mark()
	{
		bool val = false;
	}

	void Unmark()
	{
		m_inUse = false;
	}
};
