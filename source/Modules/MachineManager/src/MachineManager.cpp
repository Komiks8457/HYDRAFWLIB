#include "MachineManager.h"
#include "AppLogger.h"
#include "HydraFramework.h"
#include "MemoryUtility.h"
#include "Classes/MainProcess.h"

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

    BYTE CMachineManager::GetMyServerBodyID()
    {
        return *(BYTE*)MEMUTIL_ADD_PTR(*reinterpret_cast<uintptr_t*>(0x015AD9F8), 0);
    }

    HWND CMachineManager::GetWinHandle()
    {
        return *(HWND*)MEMUTIL_ADD_PTR(*reinterpret_cast<uintptr_t*>(0x015ADDBC), 0x14);
    }
}

