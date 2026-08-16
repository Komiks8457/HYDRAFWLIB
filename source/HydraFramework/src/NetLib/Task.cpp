#include "Task.h"

namespace NetLib
{
    CTaskMgr::CTaskMgr()
    {}

    CTaskMgr::~CTaskMgr()
    {
        ACS_SCOPED_LOCK(m_taskLock);
        while (!m_taskQueue.empty())
        {
            delete m_taskQueue.front();
            m_taskQueue.pop();
        }
    }

    void CTaskMgr::Execute()
    {
        while (!IsEmpty())
        {
            CTask* pTask = PopTask();
            if (pTask)
            {
                pTask->Execute();
                delete pTask;
            }
        }
    }

    void CTaskMgr::PushTask(CTask* pTask)
    {
        if (!pTask) return;
        ACS_SCOPED_LOCK(m_taskLock);
        m_taskQueue.push(pTask);
    }

    CTask* CTaskMgr::PopTask()
    {
        ACS_SCOPED_LOCK(m_taskLock);
        if (m_taskQueue.empty()) return NULL;

        CTask* pTask = m_taskQueue.front();
        m_taskQueue.pop();
        return pTask;
    }

    bool CTaskMgr::IsEmpty()
    {
        ACS_SCOPED_LOCK(m_taskLock);
        return m_taskQueue.empty();
    }
}