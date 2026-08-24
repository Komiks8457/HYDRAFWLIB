#include "FarmManager.h"
#include "AppLogger.h"
#include "HydraFramework.h"
#include "MemoryUtility.h"
#include "Vanguard.h"
#include "Classes/MainProcess.h"
#include "NetEngine/NetEngine.h"

FarmManager::CFarmManager* g_pFarmManager = NULL;

namespace FarmManager
{
    const char* ModuleName = "FarmManager";

    std::string CFarmManager::m_DbUID;
    std::string CFarmManager::m_DbPWD;

    BOOL WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, BOOL)
    {
        stdext::inifile iniFile;

        //dword_156FF40 = (int)&dword_154EE60;
        MEMUTIL_WRITE_VALUE(uintptr_t, 0x0156FF40, 0x0154EE60);

        //assign our g_pFarmManager global pointer
        g_pFarmManager = *reinterpret_cast<CFarmManager**>(0x0156FF40); // or (CFarmManager*)0x0154EE60;

        if (iniFile.load("hydrafwlib.ini"))
        {
            g_pFarmManager->DbUID(iniFile.getvalue("DbUserInfo", "UID", "NULL").c_str());
            g_pFarmManager->DbPWD(iniFile.getvalue("DbUserInfo", "PWD", "NULL").c_str());
        }

        if (g_pVanguard) {
            // load only if CVanguard was initilized in HydraDLL::CLibrary
            g_pVanguard->InitDll();
        }

        //return sub_1459A50(0, "FarmManager", (int)hInstance, (int)lpCmdLine, 1, 0);
        return reinterpret_cdecl(0x01459A50, BOOL, 0, ModuleName, hInstance, (BOOL)lpCmdLine, 1, 0);
    }

    void CFarmManager::Initialize()
    {
        // 1. Hook LogWriter first
        CAppLogger::SetupHook(0x01457B70);

        // WinMain hook
        MEMUTIL_REPLACE_OFFSET(0x014F153D, &WinMain);

        // ValidateMsg Hook
        MEMUTIL_REPLACE_OFFSET(0x01455CF7, &check_valid_msgid);

        // CFarmManager vftable hooks
        MEMUTIL_VFTABLE_HOOK(0x01529BE4,  1, &CFarmManager::init_module);
        MEMUTIL_VFTABLE_HOOK(0x01529BE4, 11, &CFarmManager::init_localdata);

        // Disable ENDLESS_LOOP dump
        MEMUTIL_SETUP_HOOK(LongCall, 0x0148889C, HydraFramework::NO_ENDLESS_LOOP_DUMP);

        // Other hooks
        CMainProcess::SetupHook();
    }

    BOOL CFarmManager::init_module()
    {
        CHydraProcess::CreateInstance(this, ModuleName);

        if (!g_pHFW) {
            PutLog(FATAL, "Failed to initialize Hydra");
            return FALSE;
        }

        return reinterpret_thiscall(0x014540E0, BOOL, this);
    }

    BOOL CFarmManager::init_localdata()
    {
        char oldTitle[MAX_PATH], newTitle[MAX_PATH];
        GetConsoleTitleA(oldTitle, sizeof(oldTitle));
        sprintf_s(newTitle, "[%d] %s", GetMyServerBodyID(), oldTitle);
        SetConsoleTitleA(newTitle);

        ShowWindow(GetConsoleWindow(), SW_FORCEMINIMIZE);

        return reinterpret_thiscall(0x01441250, BOOL, this);
    }

    BOOL CFarmManager::check_valid_msgid(int a2, WORD wMsgID, int a4)
    {
        if (wMsgID == SERVER_ONLY_ACK || wMsgID == SERVER_ONLY_REQ)
            return 0;

        return reinterpret_thiscall(0x014930D0, BOOL, this, a2, wMsgID, a4);
    }

    BOOL CFarmManager::GetMyServerBodyID()
    {
        return *(BYTE*)MEMUTIL_ADD_PTR(*reinterpret_cast<uintptr_t*>(0x001570328), 0);
    }

    HWND CFarmManager::GetWinHandle()
    {
        return *(HWND*)MEMUTIL_ADD_PTR(*reinterpret_cast<uintptr_t*>(0x015706EC), 0x14);
    }

    BOOL CFarmManager::SendMsgToServerBody(const char *pName, CMsg* pMsg)
    {
        if (!pName) {
            PutLog(FATAL, "%s(): pName is NULL", __FUNCTION__);
            return FALSE;
        }

        const char* allowed[] = { "AgentServer", "SR_ShardManager", "SR_GameServer" };

        bool bAllowed = false;
        for (size_t i = 0; i < sizeof(allowed) / sizeof(allowed[0]); ++i)
        {
            if (strcmp(pName, allowed[i]) == 0)
            {
                bAllowed = true;
                break;
            }
        }

        if (!bAllowed) {
            PutLog(FATAL, "%s(): pName(%s) not allowed", __FUNCTION__, pName);
            return FALSE;
        }

        bool foundAny = false;

        ServerInfo::iterator it = g_pHFW->m_ServerInfo.begin();
        ServerInfo::iterator endIt = g_pHFW->m_ServerInfo.end();

        for (; it != endIt; ++it)
        {
            // Iterate through the ServerBody map inside sServerInfo
            ServerBody::iterator bodyIt = it->second.ServerBodyMap.begin();
            ServerBody::iterator bodyEnd = it->second.ServerBodyMap.end();

            for (; bodyIt != bodyEnd; ++bodyIt)
            {
                if (bodyIt->second.Name != pName)
                    continue;

                int bodyID = static_cast<int>(bodyIt->second.ID);

                if (!reinterpret_stdcall(0x01450EF0, char, bodyID, pMsg))
                {
                    PutLog(FATAL, "Failed to send msg to body ID(%d), MsgID(0x%x)", bodyID, pMsg->GetMsgID());
                    return FALSE;
                }

                foundAny = true;
            }
        }

        if (!foundAny)
        {
            PutLog(FATAL, "Unknown module %s", pName);
            return FALSE;
        }

        return TRUE;
    }
}
