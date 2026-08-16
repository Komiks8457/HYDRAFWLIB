#pragma once

#ifndef SESSIONMGR_H
#define SESSIONMGR_H

#include <map>
#include <vector>
#include <functional>
#include "Session.h"

namespace NetLib
{
    class CNetLib;

    class CSessionMgr {
    public:
        CSessionMgr();
        virtual ~CSessionMgr();


        CSession* CreateSession(SOCKET hSocket, CNetLib* pNetLib);
        void RemoveSession(DWORD dwSessionID);
        CSession* GetSession(DWORD dwSessionID);

        void CloseAll();
        void GetAllSessions(std::vector<CSession*>& out);
        void RemoveTimedOutSessions(DWORD dwTimeoutMs);
        void ForEachSession(void (*fn)(CSession*));

    private:
        CCriticalSection            m_mgrLock;
        std::map<DWORD, CSession*>  m_sessionMap;
        DWORD                       m_dwNextSessionID;
    };
}

#endif