#pragma once

#include "pch.h"

class CMsg;
class CMassiveMsg;

namespace MachineManager
{
    class CMainProcess {
    public:
        static void SetupHook();

    private:
        uint32_t HandleMsg(CMsg *pMsg, DWORD dwOverlappedID, void *lpParam, CMassiveMsg *pMassiveMsg);
    };
}