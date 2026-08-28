#pragma once

#include "pch.h"

class CMsg;

namespace FarmManager
{
    extern const char* ModuleName;

    class CFarmManager {
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

extern FarmManager::CFarmManager* g_pFarmManager;