#pragma once

#ifndef THREADMGR_H
#define THREADMGR_H

#include <vector>
#include "ThreadControl.h"

namespace NetLib
{
    class CThreadMgr : public CThreadControl {
    public:
        CThreadMgr();
        virtual ~CThreadMgr();

        void SetIOCP(HANDLE hIOCP) { m_hIOCP = hIOCP; }
        bool SpawnWorkers(DWORD dwThreadCount = 0);
        void StopAll();

    protected:
        virtual void Run();

    private:
        HANDLE                      m_hIOCP;
        std::vector<HANDLE>         m_workerHandles;
    };
}

#endif