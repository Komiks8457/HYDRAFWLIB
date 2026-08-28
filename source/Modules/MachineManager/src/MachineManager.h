#pragma once

#include "pch.h"

class CMsg;

namespace MachineManager
{
    extern const char* ModuleName;

    class CMachineManager {
    public:
        static void Initialize();
        WORD GetMyServerBodyID();
        HWND GetWinHandle();
        void SendMsgToServerBody(E_MODULE::Type Type, CMsg* pMsg);

    private:
        BOOL init_module();
        BOOL init_localdata();
        BOOL check_valid_msgid(int a2, WORD wMsgID, int a4);
    };
}

extern MachineManager::CMachineManager *g_pMachineManager;


