#include "Session.h"
#include "NetLib.h"
#include "Logger/Logger.h"

namespace NetLib
{
    CSession::CSession() : m_dwSessionID(0), m_lRefCnt(1), m_pNetLib(NULL), m_dwStreamSize(0), m_bSending(false)
    {
        ::ZeroMemory(&m_recvOverlapped, sizeof(m_recvOverlapped));
        ::ZeroMemory(&m_sendOverlapped, sizeof(m_sendOverlapped));
        ::ZeroMemory(m_recvBuffer, sizeof(m_recvBuffer));
        ::ZeroMemory(m_streamBuffer, sizeof(m_streamBuffer));
        m_dwLastActive = ::GetTickCount();
        m_nMaxSendQueueBytes = 0; // 0 = unlimited
        m_nCurSendQueueBytes = 0;
        m_bHeartbeatPending = false;
        m_dwHeartbeatSentAt = 0;
        m_dwHeartbeatInterval = 0;
        m_dwHeartbeatTimeout = 0;
        m_nMissedHeartbeats = 0;
    }

    CSession::~CSession()
    {
        Close();
    }

    LONG CSession::AddRef()
    {
        return ::InterlockedIncrement(&m_lRefCnt);
    }

    LONG CSession::Release()
    {
        LONG lRef = ::InterlockedDecrement(&m_lRefCnt);
        if (lRef == 0)
        {
            delete this;
        }
        return lRef;
    }

    void CSession::Init(DWORD dwSessionID, CNetLib* pNetLib)
    {
        m_dwSessionID = dwSessionID;
        m_pNetLib = pNetLib;
        m_dwStreamSize = 0;
        m_bSending = false;
        m_lRefCnt = 1;

        ::ZeroMemory(&m_recvOverlapped, sizeof(m_recvOverlapped));
        ::ZeroMemory(&m_sendOverlapped, sizeof(m_sendOverlapped));
        ::ZeroMemory(m_recvBuffer, sizeof(m_recvBuffer));
        ::ZeroMemory(m_streamBuffer, sizeof(m_streamBuffer));
        m_dwLastActive = ::GetTickCount();
        m_nCurSendQueueBytes = 0;
        m_bHeartbeatPending = false;
        m_dwHeartbeatSentAt = 0;
        m_nMissedHeartbeats = 0;
    }

    void CSession::Close()
    {
        ACS_SCOPED_LOCK(m_sendLock);

        if (m_hSocket != INVALID_SOCKET)
        {
            ::closesocket(m_hSocket);
            m_hSocket = INVALID_SOCKET;
        }

        while (!m_sendQueue.empty())
        {
            CMsg* pMsg = m_sendQueue.front();
            m_sendQueue.pop();
            if (pMsg != NULL)
            {
                delete pMsg;
            }
        }

        m_bSending = false;
        m_dwStreamSize = 0;
        m_dwLastActive = 0;
        Logger::info("[Session %u] Closed socket and cleared send queue.", m_dwSessionID);
    }

    bool CSession::PostRecv()
    {
        if (m_hSocket == INVALID_SOCKET)
            return false;

        ::ZeroMemory(&m_recvOverlapped.overlapped, sizeof(OVERLAPPED));
        m_recvOverlapped.operation = IO_OP_RECV;
        m_recvOverlapped.wsaBuf.buf = m_recvBuffer;
        m_recvOverlapped.wsaBuf.len = RECV_BUFFER_SIZE;

        DWORD dwBytes = 0;
        DWORD dwFlags = 0;

        AddRef();

        // update last activity
        m_dwLastActive = ::GetTickCount();

        int nRet = ::WSARecv(m_hSocket, &m_recvOverlapped.wsaBuf, 1, &dwBytes, &dwFlags, &m_recvOverlapped.overlapped, NULL);
        if (nRet == SOCKET_ERROR && ::WSAGetLastError() != WSA_IO_PENDING)
        {
            Release();
            return false;
        }

        return true;
    }

    bool CSession::PostSend(CMsg* pMsg)
    {
        if (pMsg == NULL || m_hSocket == INVALID_SOCKET)
        {
            if (pMsg) delete pMsg;
            return false;
        }

        ACS_SCOPED_LOCK(m_sendLock);

        // Account for send-queue size and apply policy
        DWORD dwMsgBytes = pMsg->GetMsgSize();
        if (m_nMaxSendQueueBytes > 0 && (m_nCurSendQueueBytes + dwMsgBytes) > m_nMaxSendQueueBytes)
        {
            Logger::warn("[Session %u] Send queue limit exceeded (%u bytes). Dropping outgoing message.", m_dwSessionID, m_nCurSendQueueBytes + dwMsgBytes);
            // If the queue is excessively large (e.g., > 2x configured max), drop the session to protect resources
            if ((m_nCurSendQueueBytes + dwMsgBytes) > (m_nMaxSendQueueBytes * 2))
            {
                Logger::error("[Session %u] Send queue far exceeds limit; disconnecting session to protect server.", m_dwSessionID);
                delete pMsg;
                // schedule graceful disconnect
                DisconnectGracefully();
                return false;
            }

            delete pMsg;
            return false;
        }

        m_sendQueue.push(pMsg);
        m_nCurSendQueueBytes += dwMsgBytes;

        if (!m_bSending)
        {
            m_bSending = true;

            CMsg* pNextMsg = m_sendQueue.front();
            ::ZeroMemory(&m_sendOverlapped.overlapped, sizeof(OVERLAPPED));
            m_sendOverlapped.operation = IO_OP_SEND;

            // Uses native CMsg buffer pointer and total written length
            m_sendOverlapped.wsaBuf.buf = (char*)pNextMsg->GetBufferAt(0);
            m_sendOverlapped.wsaBuf.len = pNextMsg->GetWrPos();

            DWORD dwSentBytes = 0;

            AddRef();

            // update last activity
            m_dwLastActive = ::GetTickCount();

            int nRet = ::WSASend(m_hSocket, &m_sendOverlapped.wsaBuf, 1, &dwSentBytes, 0, &m_sendOverlapped.overlapped, NULL);
            if (nRet == SOCKET_ERROR && ::WSAGetLastError() != WSA_IO_PENDING)
            {
                m_bSending = false;
                Release();
                return false;
            }
        }

        return true;
    }

    void CSession::OnRecvCompleted(DWORD dwBytesTransferred)
    {
        if (dwBytesTransferred == 0)
        {
            Close();
            Release();
            return;
        }

        if (m_dwStreamSize + dwBytesTransferred <= sizeof(m_streamBuffer))
        {
            ::memcpy(m_streamBuffer + m_dwStreamSize, m_recvBuffer, dwBytesTransferred);
            m_dwStreamSize += dwBytesTransferred;

            // Update last activity timestamp on successful receive
            m_dwLastActive = ::GetTickCount();
            // Receiving any data from client clears heartbeat pending state
            ClearHeartbeatPending();

            ProcessIncomingStream();
        }
        else
        {
            Logger::error("[Session %u] Stream buffer overflow! Disconnecting session.", m_dwSessionID);
            Close();
            Release();
            return;
        }

        PostRecv();
        Release();
    }

    void CSession::OnSendCompleted(DWORD dwBytesTransferred)
    {
        ACS_SCOPED_LOCK(m_sendLock);

        // Update last activity timestamp on successful send
        m_dwLastActive = ::GetTickCount();

        if (!m_sendQueue.empty())
        {
            CMsg* pSent = m_sendQueue.front();
            m_sendQueue.pop();
            if (pSent)
            {
                // adjust accounting
                DWORD dwSz = pSent->GetMsgSize();
                if (m_nCurSendQueueBytes >= dwSz) m_nCurSendQueueBytes -= dwSz; else m_nCurSendQueueBytes = 0;
                delete pSent;
            }
        }

        if (!m_sendQueue.empty())
        {
            CMsg* pNextMsg = m_sendQueue.front();
            ::ZeroMemory(&m_sendOverlapped.overlapped, sizeof(OVERLAPPED));
            m_sendOverlapped.operation = IO_OP_SEND;
            m_sendOverlapped.wsaBuf.buf = (char*)pNextMsg->GetBufferAt(0);
            m_sendOverlapped.wsaBuf.len = pNextMsg->GetWrPos();

            DWORD dwSentBytes = 0;
            // We are starting an overlapped send for the next queued message.
            // Increment ref count to ensure the session remains alive until completion.
            AddRef();
            int nRet = ::WSASend(m_hSocket, &m_sendOverlapped.wsaBuf, 1, &dwSentBytes, 0, &m_sendOverlapped.overlapped, NULL);
            if (nRet == SOCKET_ERROR && ::WSAGetLastError() != WSA_IO_PENDING)
            {
                m_bSending = false;
                Release();
            }
        }
        else
        {
            m_bSending = false;
            Release();
        }
    }

    void CSession::DisconnectGracefully()
    {
        if (m_pNetLib != NULL)
        {
            // Notify server and ask SessionMgr to remove this session from its map.
            m_pNetLib->OnClientDisconnected(m_dwSessionID);
            m_pNetLib->GetSessionMgr().RemoveSession(m_dwSessionID);
        }
        else
        {
            // Fallback if no owning NetLib: just close and release
            Close();
            Release();
        }
    }

    void CSession::ProcessIncomingStream()
    {
        while (m_dwStreamSize >= MSG_HEADER_SIZE)
        {
            WORD wDataSize = (*(WORD*)&m_streamBuffer[0]) & MSG_MASK_SIZE_ONLY;
            DWORD dwTotalPacketSize = wDataSize + MSG_HEADER_SIZE;

            if (m_dwStreamSize < dwTotalPacketSize)
                break;

            WORD wMsgID = *(WORD*)&m_streamBuffer[2];

            CMsg* pMsg = new CMsg(wMsgID);
            // Ensure the message's internal buffer can hold the entire packet
            if (dwTotalPacketSize > pMsg->GetCapacity())
            {
                // Resize will allocate a larger buffer and preserve existing header/data
                pMsg->Resize(dwTotalPacketSize);
            }

            // Copy the full packet (header + payload) directly into the message buffer.
            // Avoid using WriteBytes which appends after the header; overwrite starting at offset 0.
            ::memcpy(pMsg->GetBufferAt(0), m_streamBuffer, dwTotalPacketSize);

            // Set write/read pointers: write = total packet size, read = start of payload
            pMsg->SetWrPos(dwTotalPacketSize, FALSE);
            pMsg->SetRdPos(MSG_HEADER_SIZE, FALSE);

            if (m_pNetLib != NULL)
            {
                m_pNetLib->OnPacketReceived(m_dwSessionID, pMsg);
            }
            else
            {
                delete pMsg;
            }

            m_dwStreamSize -= dwTotalPacketSize;
            if (m_dwStreamSize > 0)
            {
                ::memmove(m_streamBuffer, m_streamBuffer + dwTotalPacketSize, m_dwStreamSize);
            }
        }
    }
}