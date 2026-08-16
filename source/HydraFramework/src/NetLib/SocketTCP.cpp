#include "SocketTCP.h"
#include "Logger/Logger.h"

namespace NetLib
{
    CSocketTCP::CSocketTCP() : m_eMode(MODE_NONE)
    {}

    CSocketTCP::~CSocketTCP()
    {}

    bool CSocketTCP::StartServer(unsigned short usPort, int nBacklog)
    {
        if (!CreateSocket()) return false;

        SetReuseAddr(true);
        SetNoDelay(true);

        SOCKADDR_IN addr;
        ::ZeroMemory(&addr, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = ::htonl(INADDR_ANY);
        addr.sin_port = ::htons(usPort);

        if (::bind(m_hSocket, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR)
        {
            Logger::error("[CSocketTCP] Bind error on port %d: %d", usPort, ::WSAGetLastError());
            CloseSocket();
            return false;
        }

        if (::listen(m_hSocket, nBacklog) == SOCKET_ERROR)
        {
            Logger::error("[CSocketTCP] Listen error: %d", ::WSAGetLastError());
            CloseSocket();
            return false;
        }

        m_eMode = MODE_SERVER;
        Logger::success("[CSocketTCP] Listening on port %d...", usPort);
        return true;
    }

    bool CSocketTCP::Connect(const char* szIP, unsigned short usPort)
    {
        if (!CreateSocket()) return false;

        SetNoDelay(true);

        SOCKADDR_IN addr;
        ::ZeroMemory(&addr, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = ::inet_addr(szIP);
        addr.sin_port = ::htons(usPort);

        if (::connect(m_hSocket, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR)
        {
            //Logger::error("[CSocketTCP] Connection failed to %s:%d (Error %d)", szIP, usPort, ::WSAGetLastError());
            CloseSocket();
            return false;
        }

        m_eMode = MODE_CLIENT;
        //Logger::success("[CSocketTCP] Connected to %s:%d", szIP, usPort);
        return true;
    }
}