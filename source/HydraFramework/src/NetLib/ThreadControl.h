#pragma once

#ifndef THREADCONTROL_H
#define THREADCONTROL_H

#include "pch.h"

namespace NetLib
{
    class CThreadControl {
    public:
        CThreadControl();
        virtual ~CThreadControl();

        bool Start();
        void Stop();
        bool IsRunning() const { return m_bRunning; }

    protected:
        virtual void Run() = 0;
        static unsigned int __stdcall ThreadProc(void* pParam);

        HANDLE  m_hThread;
        bool    m_bRunning;
    };
}

#endif