#include "ThreadMgr.h"
#include "Session.h"
#include "Logger/Logger.h"
#include <process.h>

namespace NetLib
{
    CThreadMgr::CThreadMgr() : m_hIOCP(NULL)
    {}

    CThreadMgr::~CThreadMgr()
    {
        StopAll();
    }

    void CThreadMgr::Run()
    {
        DWORD dwBytesTransferred = 0;
        ULONG_PTR ulCompletionKey = 0;
        LPOVERLAPPED pOverlapped = NULL;

        while (m_bRunning)
        {
            BOOL bSuccess = ::GetQueuedCompletionStatus(
                m_hIOCP,
                &dwBytesTransferred,
                &ulCompletionKey,
                &pOverlapped,
                1000
            );

            if (!bSuccess && pOverlapped == NULL)
                continue;

            CSession* pSession = reinterpret_cast<CSession*>(ulCompletionKey);
            OVERLAPPED_EX* pOverlapEx = reinterpret_cast<OVERLAPPED_EX*>(pOverlapped);

            if (pSession && pOverlapEx)
            {
                if (bSuccess && dwBytesTransferred > 0)
                {
                    if (pOverlapEx->operation == IO_OP_RECV)
                    {
                        pSession->OnRecvCompleted(dwBytesTransferred);
                    }
                    else if (pOverlapEx->operation == IO_OP_SEND)
                    {
                        pSession->OnSendCompleted(dwBytesTransferred);
                    }
                }
                else
                {
                    pSession->DisconnectGracefully();
                }
            }
        }
    }

    bool CThreadMgr::SpawnWorkers(DWORD dwThreadCount)
    {
        if (dwThreadCount == 0)
        {
            SYSTEM_INFO sysInfo;
            ::GetSystemInfo(&sysInfo);
            dwThreadCount = sysInfo.dwNumberOfProcessors * 2;
        }

        m_bRunning = true;

        for (DWORD i = 0; i < dwThreadCount; ++i)
        {
            HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadProc, this, 0, NULL);
            if (hThread)
            {
                m_workerHandles.push_back(hThread);
            }
        }

        return !m_workerHandles.empty();
    }

    void CThreadMgr::StopAll()
    {
        if (!m_bRunning) return;

        m_bRunning = false;

        for (size_t i = 0; i < m_workerHandles.size(); ++i)
        {
            if (m_hIOCP != NULL)
            {
                ::PostQueuedCompletionStatus(m_hIOCP, 0, 0, NULL);
            }
        }

        for (size_t i = 0; i < m_workerHandles.size(); ++i)
        {
            ::WaitForSingleObject(m_workerHandles[i], INFINITE);
            ::CloseHandle(m_workerHandles[i]);
        }
        m_workerHandles.clear();
    }
}