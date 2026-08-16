#include "BaseSocket.h"
#include "Logger/Logger.h"

namespace NetLib
{
    CBaseSocket::CBaseSocket() : m_hSocket(INVALID_SOCKET)
    {}

    CBaseSocket::~CBaseSocket()
    {
        CloseSocket();
    }

    void CBaseSocket::AttachSocket(SOCKET hSocket)
    {
        CloseSocket();
        m_hSocket = hSocket;
    }

    bool CBaseSocket::CreateSocket(int nType)
    {
        CloseSocket();
        m_hSocket = ::WSASocket(AF_INET, nType, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
        if (m_hSocket == INVALID_SOCKET)
        {
            Logger::error("[CBaseSocket] WSASocket failed: %d", ::WSAGetLastError());
            return false;
        }
        return true;
    }

    void CBaseSocket::CloseSocket()
    {
        if (m_hSocket != INVALID_SOCKET)
        {
            ::closesocket(m_hSocket);
            m_hSocket = INVALID_SOCKET;
        }
    }

    bool CBaseSocket::SetNonBlocking(bool bEnable)
    {
        u_long arg = bEnable ? 1 : 0;
        return (::ioctlsocket(m_hSocket, FIONBIO, &arg) != SOCKET_ERROR);
    }

    bool CBaseSocket::SetReuseAddr(bool bEnable)
    {
        int optval = bEnable ? 1 : 0;
        return (::setsockopt(m_hSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&optval, sizeof(optval)) != SOCKET_ERROR);
    }

    bool CBaseSocket::SetNoDelay(bool bEnable)
    {
        int optval = bEnable ? 1 : 0;
        return (::setsockopt(m_hSocket, IPPROTO_TCP, TCP_NODELAY, (const char*)&optval, sizeof(optval)) != SOCKET_ERROR);
    }

    bool CBaseSocket::SetKeepAlive(bool bEnable)
    {
        int optval = bEnable ? 1 : 0;
        return (::setsockopt(m_hSocket, SOL_SOCKET, SO_KEEPALIVE, (const char*)&optval, sizeof(optval)) != SOCKET_ERROR);
    }
}