#include "GlobalManager.h"
#include "AppLogger.h"
#include "HydraFramework.h"
#include "MemoryUtility.h"
#include "Vanguard.h"
#include "Classes/MainProcess.h"
#include "Logger/Logger.h"
#include "NetEngine/NetEngine.h"
#include "../../../HydraFramework/src/ServerInfo.h"

GlobalManager::CGlobalManager* g_pGlobalManager = NULL;

namespace GlobalManager
{
    const char* ModuleName = "GlobalManager";

    std::string CGlobalManager::m_DbUID;
    std::string CGlobalManager::m_DbPWD;

    BOOL WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, BOOL)
    {
        stdext::inifile iniFile;

        //dword_1868950 = (int)&dword_1856948;
        MEMUTIL_WRITE_VALUE(uintptr_t, 0x01868950, 0x01856948);

        //assign our g_pGlobalManager global pointer
        g_pGlobalManager = *reinterpret_cast<CGlobalManager**>(0x01868950); // or (CGlobalManager*)0x01856948;

        if (iniFile.load("hydrafwlib.ini"))
        {
            g_pGlobalManager->DbUID(iniFile.getvalue("DbUserInfo", "UID", "NULL").c_str());
            g_pGlobalManager->DbPWD(iniFile.getvalue("DbUserInfo", "PWD", "NULL").c_str());
        }

        if (g_pVanguard) {
            // load only if CVanguard was initilized in HydraDLL::CLibrary
            g_pVanguard->InitDll();
        }

        //return sub_170C6F0(0, "GlobalManager", hInstance, (int)lpCmdLine, 1, 0);
        return reinterpret_cdecl(0x0170C6F0, BOOL, 0, ModuleName, hInstance, lpCmdLine, 1, 0);
    }

    void CGlobalManager::Initialize()
    {
        // Hook LogWriter first
        CAppLogger::SetupHook(0x0170B690);

        // WinMain hook
        MEMUTIL_REPLACE_OFFSET(0x017B274D, &WinMain);

        // CGlobalManager vftable hooks
        MEMUTIL_VFTABLE_HOOK(0x01802BAC,  1, &CGlobalManager::init_module);
        MEMUTIL_VFTABLE_HOOK(0x01802BAC, 11, &CGlobalManager::init_localdata);

        // ValidateMsgID hook
        MEMUTIL_REPLACE_OFFSET(0x0172C017, &CGlobalManager::check_valid_msgid);

        // Disable ENDLESS_LOOP dump
        MEMUTIL_SETUP_HOOK(LongCall, 0x0175C3AC, HydraFramework::NO_ENDLESS_LOOP_DUMP);

        // Other hooks
        CMainProcess::SetupHook();
    }

    BOOL CGlobalManager::init_module()
    {
        CHydraProcess::CreateInstance(this, ModuleName);

        if (!g_pHFW) {
            PutLog(FATAL, "Failed to initialize HydraFramework");
            return FALSE;
        }

        return reinterpret_thiscall(0x0172A540, BOOL, this);
    }

    BOOL CGlobalManager::init_localdata()
    {
        char oldTitle[MAX_PATH], newTitle[MAX_PATH];
        GetConsoleTitleA(oldTitle, sizeof(oldTitle));
        sprintf_s(newTitle, "[%d] %s", GetMyServerBodyID(), oldTitle);
        SetConsoleTitleA(newTitle);

        WORD wServerID = static_cast<WORD>(GetMyServerBodyID());
        g_pHFW->m_ServerInfo.IniDbConfig(wServerID, g_pHFW->m_dbConfig, m_DbUID, m_DbPWD);

        if (!g_pHFW->InitDbMgr(5, 10))
            return FALSE;

        ShowWindow(GetConsoleWindow(), SW_FORCEMINIMIZE);

        return reinterpret_thiscall(0x01674DF0, BOOL, this);
    }

    BOOL CGlobalManager::check_valid_msgid(int a2, WORD wMsgID, int a4)
    {
        if (wMsgID == SERVER_ONLY_ACK || wMsgID == SERVER_ONLY_REQ)
            return 0;

        return reinterpret_thiscall(0x0175CBF0, BOOL, this, a2, wMsgID, a4);
    }

    BOOL CGlobalManager::GetMyServerBodyID()
    {
        return *(BYTE*)MEMUTIL_ADD_PTR(*reinterpret_cast<uintptr_t*>(0x01868D38), 0);
    }

    HWND CGlobalManager::GetWinHandle()
    {
        return *(HWND*)MEMUTIL_ADD_PTR(*reinterpret_cast<uintptr_t*>(0x0186981C), 0x14);
    }

    BOOL CGlobalManager::SendMsgToServerBody(const char *pName, CMsg* pMsg)
    {
        if (!pName) {
            PutLog(FATAL, "%s(): pName is NULL", __FUNCTION__);
            return FALSE;
        }

        const char* allowed[] = { "Certification", "MachineManager", "DownloadServer", "GatewayServer", "FarmManager" };

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

        std::vector<WORD> serverBodyList = g_pHFW->m_ServerInfo.GetServerBodyIDByModuleName(pName);

        if (serverBodyList.empty()) {
            PutLog(FATAL, "%s(): pName(%s) not found", __FUNCTION__, pName);
            return FALSE;
        }

        for (std::vector<WORD>::const_iterator it = serverBodyList.begin(); it != serverBodyList.end(); ++it)
        {
            int serverBodyID = static_cast<int>(*it);
            if (!reinterpret_stdcall(0x0173C4F0, char, serverBodyID, pMsg))
            {
                PutLog(FATAL, "Failed to send msg to body ID(%d), MsgID(0x%x)", serverBodyID, pMsg->GetMsgID());
                return FALSE;
            }
        }

        return TRUE;
    }
}
