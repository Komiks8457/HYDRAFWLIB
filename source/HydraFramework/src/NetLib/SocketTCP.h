#pragma once

#ifndef SOCKETTCP_H
#define SOCKETTCP_H

#include "BaseSocket.h"

namespace NetLib
{
    enum ENetworkMode
    {
        MODE_NONE,
        MODE_SERVER,
        MODE_CLIENT
    };

    class CSocketTCP : public CBaseSocket {
    public:
        CSocketTCP();
        virtual ~CSocketTCP();

        bool StartServer(unsigned short usPort, int nBacklog = SOMAXCONN);
        bool Connect(const char* szIP, unsigned short usPort);

        ENetworkMode GetMode() const { return m_eMode; }

    private:
        ENetworkMode m_eMode;
    };
}

#endif