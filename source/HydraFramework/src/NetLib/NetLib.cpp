#include "NetLib.h"
#include "Logger/Logger.h"
#include <process.h>

namespace NetLib
{
    // default configuration values
    static const DWORD DEFAULT_SESSION_TIMEOUT_MS = 2 * 60 * 1000; // 2 minutes
    static const DWORD DEFAULT_MAX_SEND_QUEUE_BYTES = 1024 * 1024; // 1 MB
    static const DWORD DEFAULT_HEARTBEAT_INTERVAL_MS = 30 * 1000; // 30 seconds
    static const DWORD DEFAULT_HEARTBEAT_TIMEOUT_MS = 30 * 1000; // 30 seconds

    CNetLib::CNetLib() : m_hIOCP(NULL), m_hAcceptThread(NULL), m_bIsRunning(false), m_bWinsockInit(false),
        m_dwSessionTimeoutMs(DEFAULT_SESSION_TIMEOUT_MS), m_dwMaxSendQueueBytes(DEFAULT_MAX_SEND_QUEUE_BYTES),
        m_dwHeartbeatIntervalMs(DEFAULT_HEARTBEAT_INTERVAL_MS), m_dwHeartbeatTimeoutMs(DEFAULT_HEARTBEAT_TIMEOUT_MS)
    {}

    CNetLib::~CNetLib()
    {
        Stop();
    }

    bool CNetLib::InitWinsock()
    {
        WSADATA wsaData;
        int nResult = ::WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (nResult != 0)
        {
            Logger::error("[CNetLib] WSAStartup failed with error: %d", nResult);
            return false;
        }

        m_bWinsockInit = true;
        Logger::success("[CNetLib] Winsock 2.2 initialized successfully.");
        return true;
    }

    void CNetLib::CleanupWinsock()
    {
        if (m_bWinsockInit)
        {
            ::WSACleanup();
            m_bWinsockInit = false;
        }
    }

    bool CNetLib::CreateIOCP()
    {
        m_hIOCP = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
        if (m_hIOCP == NULL)
        {
            Logger::error("[CNetLib] CreateIoCompletionPort failed: %d", ::GetLastError());
            return false;
        }
        return true;
    }

    void CNetLib::CloseIOCP()
    {
        if (m_hIOCP != NULL)
        {
            ::CloseHandle(m_hIOCP);
            m_hIOCP = NULL;
        }
    }

    bool CNetLib::Start(unsigned short usPort, DWORD dwWorkerThreadCount, DWORD dwSessionTimeoutMs,
                        DWORD dwMaxSendQueueBytes, DWORD dwHeartbeatIntervalMs, DWORD dwHeartbeatTimeoutMs)
    {
        if (m_bIsRunning)
        {
            Logger::warn("[CNetLib] Server is already running.");
            return true;
        }

        if (!InitWinsock())
            return false;

        if (!CreateIOCP())
        {
            CleanupWinsock();
            return false;
        }

        if (!m_listenSocket.StartServer(usPort))
        {
            CloseIOCP();
            CleanupWinsock();
            return false;
        }

        // store configuration
        if (dwSessionTimeoutMs != 0) m_dwSessionTimeoutMs = dwSessionTimeoutMs;
        if (dwMaxSendQueueBytes != 0) m_dwMaxSendQueueBytes = dwMaxSendQueueBytes;
        if (dwHeartbeatIntervalMs != 0) m_dwHeartbeatIntervalMs = dwHeartbeatIntervalMs;
        if (dwHeartbeatTimeoutMs != 0) m_dwHeartbeatTimeoutMs = dwHeartbeatTimeoutMs;

        m_threadMgr.SetIOCP(m_hIOCP);

        if (!m_threadMgr.SpawnWorkers(dwWorkerThreadCount))
        {
            Logger::error("[CNetLib] Failed to spawn worker threads.");
            Stop();
            return false;
        }

        m_bIsRunning = true;

        m_hAcceptThread = (HANDLE)_beginthreadex(NULL, 0, AcceptThreadProc, this, 0, NULL);
        if (m_hAcceptThread == NULL)
        {
            Logger::error("[CNetLib] Failed to spawn Accept Thread.");
            Stop();
            return false;
        }

        Logger::success("[CNetLib] Server Orchestrator started successfully on port %u.", usPort);
        return true;
    }

    void CNetLib::Stop()
    {
        if (!m_bIsRunning)
            return;

        m_bIsRunning = false;

        m_listenSocket.CloseSocket();

        if (m_hAcceptThread != NULL)
        {
            ::WaitForSingleObject(m_hAcceptThread, INFINITE);
            ::CloseHandle(m_hAcceptThread);
            m_hAcceptThread = NULL;
        }

        m_threadMgr.StopAll();
        m_sessionMgr.CloseAll();
        CloseIOCP();
        CleanupWinsock();

        Logger::info("[CNetLib] Server Orchestrator stopped.");
    }

    void CNetLib::Update()
    {
        // First remove idle sessions based on configured timeout
        m_sessionMgr.RemoveTimedOutSessions(m_dwSessionTimeoutMs);

        // Heartbeat: probe sessions that have been idle longer than interval
        DWORD now = ::GetTickCount();
        // Collect sessions and iterate in C++98 style
        std::vector<CSession*> sessions;
        m_sessionMgr.GetAllSessions(sessions);
        for (size_t i = 0; i < sessions.size(); ++i)
        {
            CSession* pSession = sessions[i];
            if (!pSession) continue;

            DWORD last = pSession->GetLastActive();
            if (last == 0) continue;

            // If no heartbeat has been sent and inactivity exceeds heartbeat interval, send one
            if (!pSession->IsHeartbeatPending())
            {
                if ((now - last) >= m_dwHeartbeatIntervalMs)
                {
                    CMsg* pPing = new CMsg(0xFFFF); // internal heartbeat probe
                    if (pSession->PostSend(pPing))
                    {
                        pSession->MarkHeartbeatSent();
                    }
                    else
                    {
                        delete pPing;
                    }
                }
            }
            else
            {
                // Check heartbeat timeout
                DWORD sentAt = pSession->GetHeartbeatSentAt();
                if (sentAt != 0 && (now - sentAt) > m_dwHeartbeatTimeoutMs)
                {
                    Logger::warn("[CNetLib] Session %u missed heartbeat timeout. Disconnecting.", pSession->GetID());
                    pSession->DisconnectGracefully();
                }
            }
        }

        // Then process pending tasks
        m_taskMgr.Execute();
    }

    unsigned int __stdcall CNetLib::AcceptThreadProc(void* pParam)
    {
        CNetLib* pNetLib = static_cast<CNetLib*>(pParam);
        if (pNetLib != NULL)
        {
            pNetLib->AcceptLoop();
        }
        return 0;
    }

    void CNetLib::AcceptLoop()
    {
        while (m_bIsRunning)
        {
            SOCKADDR_IN clientAddr;
            int nAddrLen = sizeof(clientAddr);

            SOCKET hClientSocket = ::accept(m_listenSocket.GetSocket(), (SOCKADDR*)&clientAddr, &nAddrLen);
            if (hClientSocket == INVALID_SOCKET)
            {
                if (!m_bIsRunning)
                    break;

                Logger::error("[CNetLib] Accept failed: %d", ::WSAGetLastError());
                continue;
            }

            // Pass 'this' (CNetLib*) to session
            CSession* pSession = m_sessionMgr.CreateSession(hClientSocket, this);
            if (pSession == NULL)
            {
                ::closesocket(hClientSocket);
                continue;
            }

            HANDLE hBind = ::CreateIoCompletionPort((HANDLE)hClientSocket, m_hIOCP, (ULONG_PTR)pSession, 0);
            if (hBind == NULL)
            {
                Logger::error("[CNetLib] Failed to bind socket to IOCP: %d", ::GetLastError());
                m_sessionMgr.RemoveSession(pSession->GetID());
                continue;
            }

            OnClientConnected(pSession->GetID());

            // Apply configured session policies
            pSession->SetMaxSendQueueBytes(m_dwMaxSendQueueBytes);
            pSession->SetHeartbeatParams(m_dwHeartbeatIntervalMs, m_dwHeartbeatTimeoutMs);
            // Enable TCP keepalive on accepted sockets to detect half-open connections at TCP level
            pSession->SetKeepAlive(true);

            if (!pSession->PostRecv())
            {
                Logger::error("[CNetLib] Initial PostRecv failed for Session %u", pSession->GetID());
                m_sessionMgr.RemoveSession(pSession->GetID());
            }
        }
    }
}