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
        BOOL PrepareMsgHandlers();
        BOOL RegisterMsgHandler(WORD wMsgID, uintptr_t fnPtr);
        BOOL HandleMsg(CMsg *pMsg, DWORD dwOverlappedID, void *lpParam, CMassiveMsg *pMassiveMsg);
        BOOL CustomMsg(CMsg *pMsg, DWORD, LPVOID, CMassiveMsg*);
    };
}