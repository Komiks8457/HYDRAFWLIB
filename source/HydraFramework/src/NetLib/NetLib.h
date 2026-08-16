#pragma once

#ifndef NETLIB_H
#define NETLIB_H

#include "pch.h"
#include "SocketTCP.h"
#include "SessionMgr.h"
#include "ThreadMgr.h"
#include "Task.h"

class CMsg;

namespace NetLib
{
    class CNetLib {
    public:
        CNetLib();
        virtual ~CNetLib();

        bool Start(unsigned short usPort, DWORD dwWorkerThreadCount = 0,
                   DWORD dwSessionTimeoutMs = 2 * 60 * 1000,
                   DWORD dwMaxSendQueueBytes = 1024 * 1024,
                   DWORD dwHeartbeatIntervalMs = 30 * 1000,
                   DWORD dwHeartbeatTimeoutMs = 30 * 1000);
        void SetSessionTimeout(DWORD dwMs) { m_dwSessionTimeoutMs = dwMs; }
        void SetMaxSendQueueBytes(DWORD dwBytes) { m_dwMaxSendQueueBytes = dwBytes; }
        void SetHeartbeatParams(DWORD dwIntervalMs, DWORD dwTimeoutMs)
        {
            m_dwHeartbeatIntervalMs = dwIntervalMs;
            m_dwHeartbeatTimeoutMs = dwTimeoutMs;
        }
        void Stop();
        void Update();

        bool IsRunning() const { return m_bIsRunning; }

        CSessionMgr& GetSessionMgr() { return m_sessionMgr; }
        CTaskMgr& GetTaskMgr()       { return m_taskMgr; }

        // --- Virtual event callbacks for derived servers (e.g. CExServer) ---
        virtual void OnClientConnected(DWORD dwSessionID) {}
        virtual void OnClientDisconnected(DWORD dwSessionID) {}
        virtual void OnPacketReceived(DWORD dwSessionID, CMsg* pMsg) {}

    private:
        bool InitWinsock();
        void CleanupWinsock();
        bool CreateIOCP();
        void CloseIOCP();

        static unsigned int __stdcall AcceptThreadProc(void* pParam);
        void AcceptLoop();

    private:
        HANDLE          m_hIOCP;
        HANDLE          m_hAcceptThread;
        bool            m_bIsRunning;
        bool            m_bWinsockInit;

        CSocketTCP      m_listenSocket;
        CSessionMgr     m_sessionMgr;
        CThreadMgr      m_threadMgr;
        CTaskMgr        m_taskMgr;
        // configuration
        DWORD           m_dwSessionTimeoutMs;
        DWORD           m_dwMaxSendQueueBytes;
        DWORD           m_dwHeartbeatIntervalMs;
        DWORD           m_dwHeartbeatTimeoutMs;
    };
}

using namespace NetLib;

#endif