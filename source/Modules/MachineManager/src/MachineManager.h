#pragma once

#include "pch.h"

class CMsg;

namespace MachineManager
{
    extern const char* ModuleName;

    class CMachineManager {
    public:
        static void Initialize();
        BYTE GetMyServerBodyID();
        HWND GetWinHandle();

    private:
        BOOL init_module();
        BOOL init_localdata();
    };

    void SendMsgToServerBody(int nTargetID, CMsg *pMsg);
}

extern MachineManager::CMachineManager *g_pMachineManager;


