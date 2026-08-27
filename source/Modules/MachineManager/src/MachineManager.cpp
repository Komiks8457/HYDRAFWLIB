#include "MachineManager.h"
#include "AppLogger.h"
#include "HydraFramework.h"
#include "MemoryUtility.h"
#include "Classes/MainProcess.h"
#include "NetEngine/NetEngine.h"

MachineManager::CMachineManager *g_pMachineManager = NULL;

namespace MachineManager
{
    const char* ModuleName = "MachineManager";

    void SendMsgToServerBody(int nTargetID, CMsg *pMsg)
    {
        reinterpret_stdcall(0x01492890, char, nTargetID, pMsg);
    }

    BOOL WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, BOOL)
    {
        //dword_15AD5B8 = (int)&dword_158C008;
        MEMUTIL_WRITE_VALUE(uintptr_t, 0x015AD5B8, 0x0158C008);

        //assign our g_pGlobalManager global pointer
        g_pMachineManager = *reinterpret_cast<CMachineManager**>(0x015AD5B8); // or (CMachineManager*)0x0158C008;

        //return sub_14904E0(2, "MachineManager", hInstance, (int)lpCmdLine, 1, 1);
        return reinterpret_cdecl(0x014904E0, BOOL, 0, ModuleName, (BOOL)hInstance, (BOOL)lpCmdLine, 1, 0);
    }

    void CMachineManager::Initialize()
    {
        // Hook LogWriter first
        CAppLogger::SetupHook(0x0148E600);

        // Other hooks
        CMainProcess::SetupHook();

        // WinMain hook
        MEMUTIL_REPLACE_OFFSET(0x0152AE42, &WinMain);

        // CMachineManager vftable hooks
        MEMUTIL_VFTABLE_HOOK(0x01564B24,  1, &CMachineManager::init_module);
        MEMUTIL_VFTABLE_HOOK(0x01564B24, 11, &CMachineManager::init_localdata);

        // Disable ENDLESS_LOOP dump
        MEMUTIL_SETUP_HOOK(LongCall, 0x014C425C, HydraFramework::NO_ENDLESS_LOOP_DUMP);
    }

    BOOL CMachineManager::init_module()
    {
        CHydraProcess::CreateInstance(this, ModuleName);

        if (!g_pHFW) {
            PutLog(FATAL, "Failed to initialize HydraFramework");
            return FALSE;
        }

        return reinterpret_thiscall(0x0148B900, BOOL, this);
    }

    BOOL CMachineManager::init_localdata()
    {
        char oldTitle[MAX_PATH], newTitle[MAX_PATH];
        GetConsoleTitleA(oldTitle, sizeof(oldTitle));
        sprintf_s(newTitle, "[%d] %s", GetMyServerBodyID(), oldTitle);
        SetConsoleTitleA(newTitle);

        ShowWindow(GetConsoleWindow(), SW_FORCEMINIMIZE);

        return reinterpret_thiscall(0x01471130, BOOL, this);
    }

    BOOL CMachineManager::check_valid_msgid(int a2, WORD wMsgID, int a4)
    {
        if (wMsgID == SERVER_ONLY_ACK || wMsgID == SERVER_ONLY_REQ)
            return 0;

        return reinterpret_thiscall(0x014CEA90, BOOL, this, a2, wMsgID, a4);
    }

    BYTE CMachineManager::GetMyServerBodyID()
    {
        return *(BYTE*)MEMUTIL_ADD_PTR(*reinterpret_cast<uintptr_t*>(0x015AD9F8), 0);
    }

    HWND CMachineManager::GetWinHandle()
    {
        return *(HWND*)MEMUTIL_ADD_PTR(*reinterpret_cast<uintptr_t*>(0x015ADDBC), 0x14);
    }

    void CMachineManager::SendMsgToServerBody(E_MODULE::Type Type, CMsg* pMsg)
    {
        if (!pMsg) return;

        if (Type != E_MODULE::GlobalManager) {
            PutLog(FATAL, "%s(): pName(%s) not allowed", __FUNCTIONP__, E_MODULE::GetModuleName(Type));
            return;
        }

        std::vector<WORD> serverBodyList = g_pHFW->m_ServerInfo.GetServerBodyIDByModuleName(Type);

        if (serverBodyList.empty()) {
            PutLog(FATAL, "%s(): pName(%s) not found", __FUNCTIONP__, E_MODULE::GetModuleName(Type));
            return;
        }

        WORD myServerBodyID = static_cast<WORD>(GetMyServerBodyID());
        BYTE myDivisionID = g_pHFW->m_ServerInfo.ServerBodyMap[myServerBodyID].DivisionID;
        BYTE myFarmID = g_pHFW->m_ServerInfo.ServerBodyMap[myServerBodyID].FarmID;

        for (std::vector<WORD>::const_iterator it = serverBodyList.begin(); it != serverBodyList.end(); ++it)
        {
            WORD serverBodyID = *it;

            BYTE divisionID = g_pHFW->m_ServerInfo.ServerBodyMap[serverBodyID].DivisionID;
            BYTE farmID = g_pHFW->m_ServerInfo.ServerBodyMap[serverBodyID].FarmID;

            // check if same division and farm
            if (myDivisionID != divisionID || myFarmID != farmID)
                continue;

            // check if it does have actual cords connected between us and the target
            bool bHasCord = false;
            for (ServerCord::const_iterator cordIt = g_pHFW->m_ServerInfo.ServerCordMap.begin();
                 cordIt != g_pHFW->m_ServerInfo.ServerCordMap.end(); ++cordIt)
            {
                const sServerCord& cord = cordIt->second;
                if ((cord.OutletID == myServerBodyID && cord.InletID == serverBodyID) ||
                    (cord.OutletID == serverBodyID && cord.InletID == myServerBodyID))
                {
                    bHasCord = true;
                    break;
                }
            }

            if (!bHasCord)
                continue;

            if (!reinterpret_stdcall(0x01492890, char, (int)serverBodyID, pMsg))
                PutLog(FATAL, "Failed to send msg to body ID(%d), MsgID(0x%x)", serverBodyID, pMsg->GetMsgID());
        }
    }
}

