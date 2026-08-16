#pragma once

#ifndef TASK_TASKMGR_H
#define TASK_TASKMGR_H

#include <queue>
#include "CriticalSection.h"

namespace NetLib
{
    class CTask {
    public:
        CTask() {}
        virtual ~CTask() {}

        virtual void Execute() = 0;
    };

    class CTaskMgr {
    public:
        CTaskMgr();
        virtual ~CTaskMgr();

        void Execute();
        void PushTask(CTask* pTask);
        CTask* PopTask();
        bool IsEmpty();

    private:
        CCriticalSection    m_taskLock;
        std::queue<CTask*>  m_taskQueue;
    };
}

#endif