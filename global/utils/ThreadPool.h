#pragma once

#include <Windows.h>
#include <process.h>
#include <vector>
#include <queue>

class CThreadPool {
public:
    CThreadPool(size_t numThreads) : m_isShutdown(false)
    {
        InitializeCriticalSection(&m_queueLock);

        m_taskSemaphore = CreateSemaphore(NULL, 0, 0x7FFFFFFF, NULL);

        for (size_t i = 0; i < numThreads; ++i)
        {
            HANDLE h = (HANDLE)_beginthreadex(NULL, 0, WorkerThread, this, 0, NULL);
            if (h) m_workerThreads.push_back(h);
        }
    }

    ~CThreadPool()
    {
        m_isShutdown = true;

        ReleaseSemaphore(m_taskSemaphore, (LONG)m_workerThreads.size(), NULL);

        for (size_t i = 0; i < m_workerThreads.size(); ++i) {
            WaitForSingleObject(m_workerThreads[i], INFINITE);
            CloseHandle(m_workerThreads[i]);
        }

        DeleteCriticalSection(&m_queueLock);
        CloseHandle(m_taskSemaphore);
    }

    void EnqueueTask(void (*function)(void *), void *arg)
    {
        EnterCriticalSection(&m_queueLock);
        m_taskQueue.push(std::make_pair(function, arg));
        LeaveCriticalSection(&m_queueLock);

        ReleaseSemaphore(m_taskSemaphore, 1, NULL);
    }

private:
    static unsigned int __stdcall WorkerThread(void *arg)
    {
        CThreadPool *pool = (CThreadPool *)arg;

        while (true)
        {
            WaitForSingleObject(pool->m_taskSemaphore, INFINITE);

            if (pool->m_isShutdown && pool->IsEmpty())
                break;

            std::pair<void (*)(void *), void *> task(NULL, NULL);

            EnterCriticalSection(&pool->m_queueLock);
            if (!pool->m_taskQueue.empty())
            {
                task = pool->m_taskQueue.front();
                pool->m_taskQueue.pop();
            }
            LeaveCriticalSection(&pool->m_queueLock);

            if (task.first != NULL)
                task.first(task.second);
        }

        return 0;
    }

    bool IsEmpty()
    {
        EnterCriticalSection(&m_queueLock);
        bool empty = m_taskQueue.empty();
        LeaveCriticalSection(&m_queueLock);
        return empty;
    }

    std::vector<HANDLE> m_workerThreads;
    std::queue<std::pair<void (*)(void *), void *> > m_taskQueue;
    CRITICAL_SECTION m_queueLock;
    HANDLE m_taskSemaphore;
    volatile bool m_isShutdown;
};
