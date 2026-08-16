#pragma once

#ifndef SESSION_H
#define SESSION_H

#include "BaseSocket.h"
#include "NetEngine/Msg.h"
#include "CriticalSection.h"
#include <queue>

#define RECV_BUFFER_SIZE 8192

namespace NetLib
{
    class CNetLib;

    enum EIOOperation
    {
        IO_OP_RECV = 1,
        IO_OP_SEND = 2
    };

    struct OVERLAPPED_EX
    {
        OVERLAPPED   overlapped; // Must remain at offset 0
        EIOOperation operation;
        WSABUF       wsaBuf;
    };

    class CSession : public CBaseSocket {
    public:
        CSession();
        virtual ~CSession();

        void Init(DWORD dwSessionID, CNetLib* pNetLib);
        void Close();

        DWORD GetID() const { return m_dwSessionID; }

        LONG AddRef();
        LONG Release();

        bool PostRecv();
        bool PostSend(CMsg* pMsg);

        void OnRecvCompleted(DWORD dwBytesTransferred);
        void OnSendCompleted(DWORD dwBytesTransferred);

        void ProcessIncomingStream();
        void DisconnectGracefully();
        DWORD GetLastActive() const { return m_dwLastActive; }

        // Send-queue limits
        void SetMaxSendQueueBytes(DWORD dwMax) { m_nMaxSendQueueBytes = dwMax; }
        DWORD GetCurrentSendQueueBytes() const { return m_nCurSendQueueBytes; }

        // Heartbeat management
        void SetHeartbeatParams(DWORD dwIntervalMs, DWORD dwTimeoutMs)
        { m_dwHeartbeatInterval = dwIntervalMs; m_dwHeartbeatTimeout = dwTimeoutMs; }
        bool IsHeartbeatPending() const { return m_bHeartbeatPending; }
        DWORD GetHeartbeatSentAt() const { return m_dwHeartbeatSentAt; }
        void MarkHeartbeatSent() { m_bHeartbeatPending = true; m_dwHeartbeatSentAt = ::GetTickCount(); }
        void ClearHeartbeatPending() { m_bHeartbeatPending = false; m_dwHeartbeatSentAt = 0; m_nMissedHeartbeats = 0; }

    protected:
        DWORD               m_dwSessionID;
        LONG                m_lRefCnt;
        CNetLib*            m_pNetLib;

        OVERLAPPED_EX       m_recvOverlapped;
        OVERLAPPED_EX       m_sendOverlapped;

        char                m_recvBuffer[RECV_BUFFER_SIZE];
        char                m_streamBuffer[RECV_BUFFER_SIZE * 2];
        DWORD               m_dwStreamSize;
        DWORD               m_dwLastActive;
        // send queue accounting
        DWORD               m_nMaxSendQueueBytes;
        DWORD               m_nCurSendQueueBytes;

        // heartbeat
        bool                m_bHeartbeatPending;
        DWORD               m_dwHeartbeatSentAt;
        DWORD               m_dwHeartbeatInterval;
        DWORD               m_dwHeartbeatTimeout;
        int                 m_nMissedHeartbeats;

        std::queue<CMsg*>   m_sendQueue;
        CCriticalSection    m_sendLock;
        bool                m_bSending;
    };
}

#endif