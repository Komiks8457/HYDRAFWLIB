#pragma once

#ifndef BASESOCKET_H
#define BASESOCKET_H

#include "pch.h"

namespace NetLib
{
    class CBaseSocket {
    public:
        CBaseSocket();
        virtual ~CBaseSocket();

        bool CreateSocket(int nType = SOCK_STREAM);
        void CloseSocket();
        SOCKET GetSocket() const { return m_hSocket; }
        bool IsValid() const { return m_hSocket != INVALID_SOCKET; }

        void AttachSocket(SOCKET hSocket);

        bool SetNonBlocking(bool bEnable = true);
        bool SetReuseAddr(bool bEnable = true);
        bool SetNoDelay(bool bEnable = true);
        bool SetKeepAlive(bool bEnable = true);

    protected:
        SOCKET m_hSocket;
    };
}

#endif