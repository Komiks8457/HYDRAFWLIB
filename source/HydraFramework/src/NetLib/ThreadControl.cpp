#include "ThreadControl.h"
#include <process.h>

namespace NetLib
{
    CThreadControl::CThreadControl() : m_hThread(NULL), m_bRunning(false)
    {}

    CThreadControl::~CThreadControl()
    {
        Stop();
    }

    bool CThreadControl::Start()
    {
        if (m_bRunning) return true;

        m_bRunning = true;
        m_hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadProc, this, 0, NULL);
        if (!m_hThread)
        {
            m_bRunning = false;
            return false;
        }
        return true;
    }

    void CThreadControl::Stop()
    {
        if (!m_bRunning) return;

        m_bRunning = false;
        if (m_hThread)
        {
            ::WaitForSingleObject(m_hThread, INFINITE);
            ::CloseHandle(m_hThread);
            m_hThread = NULL;
        }
    }

    unsigned int __stdcall CThreadControl::ThreadProc(void* pParam)
    {
        CThreadControl* pThread = static_cast<CThreadControl*>(pParam);
        if (pThread)
        {
            pThread->Run();
        }
        return 0;
    }
}