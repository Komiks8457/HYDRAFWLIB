#pragma once

#include "pch.h"

class CMsg;
class CMassiveMsg;

namespace AgentServer
{
    class CMainProcess {
    public:
        static void SetupHook();

    private:
        BOOL PrepareMsgHandlers();
        BOOL RegisterMsgHandler(WORD wMsgID, uintptr_t fnPtr);
        BOOL HandleMsg(CMsg *pMsg, DWORD a3, LPVOID a4, CMassiveMsg *pMassiveMsg);
        BOOL CustomMsg(CMsg *pMsg, DWORD, LPVOID, CMassiveMsg*);
    };
}
