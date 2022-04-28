#include "SockEngine.h"

SockEngine::SockEngine(SOCKET sv)
{
    server = sv;
}

bool SockEngine::SendPacket(Packet* pkt)
{
    uint8 opcode = pkt->GetOpcode();
    uint8* out_stream = nullptr;
    uint16 len = (uint16)(pkt->size() + 1);

    out_stream = new uint8[len];
    if (!out_stream)
        return false;
    out_stream[0] = pkt->GetOpcode();
    if (pkt->size() > 0)
        memcpy(&out_stream[1], pkt->contents(), pkt->size());

    int r = BurstSend((const uint8*)"\xaa\x55", 2);
    if (r) r = BurstSend((const uint8*)&len, 2);
    if (r) r = BurstSend((const uint8*)out_stream, len);
    if (r) r = BurstSend((const uint8*)"\x55\xaa", 2);
    if (r) BurstPush();

    delete[] out_stream;

    return true;
}

bool SockEngine::BurstSend(const uint8* Bytes, uint32 Size)
{
    return writeBuffer.Write(Bytes, Size);
}

void SockEngine::BurstPush()
{
    WriteCallback();
}

void SockEngine::WriteCallback()
{
    if (writeBuffer.GetContiguousBytes())
    {
        DWORD w_length = 0;
        DWORD flags = 0;

        WSABUF buf;
        buf.len = (ULONG)writeBuffer.GetContiguousBytes();
        buf.buf = (char*)writeBuffer.GetBufferStart();

        m_writeEvent.Mark();
        m_writeEvent.Reset(SOCKET_IO_EVENT_WRITE_END);

        int r = WSASend(server, &buf, 1, &w_length, flags, &m_writeEvent.m_overlap, 0);
        if (r == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
        {
            int a = WSAGetLastError();
            m_writeEvent.Unmark();
        }
    }
}