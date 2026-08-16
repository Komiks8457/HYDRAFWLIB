#include "SessionMgr.h"
#include "Logger/Logger.h"
#include <vector>

namespace NetLib
{
    CSessionMgr::CSessionMgr() : m_dwNextSessionID(1)
    {}

    CSessionMgr::~CSessionMgr()
    {
        CloseAll();
    }

    void CSessionMgr::ForEachSession(void (*fn)(CSession*))
    {
        std::vector<CSession*> copy;
        {
            ACS_SCOPED_LOCK(m_mgrLock);
            std::map<DWORD, CSession*>::iterator it;
            for (it = m_sessionMap.begin(); it != m_sessionMap.end(); ++it)
            {
                if (it->second) copy.push_back(it->second);
            }
        }

        for (size_t i = 0; i < copy.size(); ++i)
        {
            fn(copy[i]);
        }
    }

    void CSessionMgr::GetAllSessions(std::vector<CSession*>& out)
    {
        ACS_SCOPED_LOCK(m_mgrLock);
        out.clear();
        std::map<DWORD, CSession*>::iterator it;
        for (it = m_sessionMap.begin(); it != m_sessionMap.end(); ++it)
        {
            if (it->second) out.push_back(it->second);
        }
    }

    void CSessionMgr::RemoveTimedOutSessions(DWORD dwTimeoutMs)
    {
        std::vector<DWORD> toRemove;
        DWORD now = ::GetTickCount();

        // Collect candidates under lock
        {
            ACS_SCOPED_LOCK(m_mgrLock);
            std::map<DWORD, CSession*>::iterator it;
            for (it = m_sessionMap.begin(); it != m_sessionMap.end(); ++it)
            {
                CSession* pSession = it->second;
                if (pSession == NULL) continue;
                DWORD last = pSession->GetLastActive();
                if (last == 0) continue;
                if ((now - last) > dwTimeoutMs)
                {
                    toRemove.push_back(it->first);
                }
            }
        }

        // Remove outside lock using existing RemoveSession() which acquires the lock
        for (size_t i = 0; i < toRemove.size(); ++i)
        {
            DWORD id = toRemove[i];
            Logger::info("[CSessionMgr] Removing timed-out session %u", id);
            RemoveSession(id);
        }
    }

    CSession* CSessionMgr::CreateSession(SOCKET hSocket, CNetLib* pNetLib)
    {
        ACS_SCOPED_LOCK(m_mgrLock);

        CSession* pSession = new CSession();
        DWORD id = m_dwNextSessionID++;
        pSession->Init(id, pNetLib);
        pSession->AttachSocket(hSocket);

        m_sessionMap[id] = pSession;
        Logger::info("[CSessionMgr] Session Created: ID %u", id);
        return pSession;
    }

    void CSessionMgr::RemoveSession(DWORD dwSessionID)
    {
        ACS_SCOPED_LOCK(m_mgrLock);

        std::map<DWORD, CSession*>::iterator it = m_sessionMap.find(dwSessionID);
        if (it != m_sessionMap.end())
        {
            CSession* pSession = it->second;
            m_sessionMap.erase(it);

            pSession->Close();
            pSession->Release();
            Logger::warn("[CSessionMgr] Session Removed: ID %u", dwSessionID);
        }
    }

    CSession* CSessionMgr::GetSession(DWORD dwSessionID)
    {
        ACS_SCOPED_LOCK(m_mgrLock);

        std::map<DWORD, CSession*>::iterator it = m_sessionMap.find(dwSessionID);
        if (it != m_sessionMap.end())
        {
            return it->second;
        }
        return NULL;
    }

    void CSessionMgr::CloseAll()
    {
        ACS_SCOPED_LOCK(m_mgrLock);

        std::map<DWORD, CSession*>::iterator it;
        for (it = m_sessionMap.begin(); it != m_sessionMap.end(); ++it)
        {
            CSession* pSession = it->second;
            if (pSession)
            {
                pSession->Close();
                pSession->Release();
            }
        }
        m_sessionMap.clear();
    }
}
