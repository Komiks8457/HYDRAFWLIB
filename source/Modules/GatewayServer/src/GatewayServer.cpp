#include "GatewayServer.h"
#include "AppLogger.h"
#include "HydraFramework.h"
#include "MemoryUtility.h"
#include "Vanguard.h"
#include "Classes/MainProcess.h"
#include "NetEngine/NetEngine.h"

GatewayServer::CGatewayServer* g_pGatewayServer = NULL;

namespace GatewayServer
{
    const char* ModuleName = "GatewayServer";

    std::string CGatewayServer::m_DbUID;
    std::string CGatewayServer::m_DbPWD;

    BOOL WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, BOOL)
    {
        stdext::inifile iniFile;

        //dword_15D29F0 = (int)&unk_15B1BE0;
        MEMUTIL_WRITE_VALUE(uintptr_t, 0x015D29F0, 0x015B1BE0);

        //assign our g_pGatewayServer global pointer
        g_pGatewayServer = *reinterpret_cast<CGatewayServer**>(0x015D29F0); // or (CGatewayServer*)0x015B1BE0;

        if (iniFile.load("hydrafwlib.ini"))
        {
            g_pGatewayServer->DbUID(iniFile.getvalue("DbUserInfo", "UID", "NULL").c_str());
            g_pGatewayServer->DbPWD(iniFile.getvalue("DbUserInfo", "PWD", "NULL").c_str());
        }

        if (g_pVanguard) {
            // load only if CVanguard was initilized in HydraDLL::CLibrary
            g_pVanguard->InitDll();
        }

        //return sub_1495B30(0, "GatewayServer", (int)hInstance, (int)lpCmdLine, 1, 0);
        return reinterpret_cdecl(0x01495B30, BOOL, 0, ModuleName, hInstance, (BOOL)lpCmdLine, 1, 0);
    }

    void CGatewayServer::Initialize()
    {
        // Hook LogWriter first
        CAppLogger::SetupHook(0x01493C50);

        // MainProcess hook
        CMainProcess::SetupHook();

        // WinMain hook
        MEMUTIL_REPLACE_OFFSET(0x0154139C, &WinMain);

        // CFarmManager vftable hooks
        MEMUTIL_VFTABLE_HOOK(0x015815FC,  1, &CGatewayServer::init_module);
        MEMUTIL_VFTABLE_HOOK(0x015815FC, 11, &CGatewayServer::init_localdata);

        // ValidateMsgID hook
        MEMUTIL_REPLACE_OFFSET(0x014CD847, &CGatewayServer::check_valid_msgid);

        // Disable ENDLESS_LOOP dump
        MEMUTIL_SETUP_HOOK(LongCall, 0x014EAB4C, HydraFramework::NO_ENDLESS_LOOP_DUMP);
    }

    BOOL CGatewayServer::init_module()
    {
        CHydraProcess::CreateInstance(this, ModuleName);

        if (!g_pHFW) {
            PutLog(FATAL, "Failed to initialize Hydra");
            return FALSE;
        }

        return reinterpret_thiscall(0x014CBD00, BOOL, this);
    }

    BOOL CGatewayServer::init_localdata()
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

        return reinterpret_thiscall(0x01465A10, BOOL, this);
    }

    BOOL CGatewayServer::check_valid_msgid(int a2, WORD wMsgID, int a4)
    {
        if (wMsgID == SERVER_ONLY_ACK || wMsgID == SERVER_ONLY_REQ)
            return 0;

        return reinterpret_thiscall(0x014EB230, BOOL, this, a2, wMsgID, a4);
    }

    BOOL CGatewayServer::GetMyServerBodyID()
    {
        return *(BYTE*)MEMUTIL_ADD_PTR(*reinterpret_cast<uintptr_t*>(0x015D2DD8), 0);
    }

    HWND CGatewayServer::GetWinHandle()
    {
        return *(HWND*)MEMUTIL_ADD_PTR(*reinterpret_cast<uintptr_t*>(0x015D397C), 0x14);
    }

    BOOL CGatewayServer::SendMsgToServerBody(const char *pName, CMsg *pMsg)
    {
        if (!pName) {
            PutLog(FATAL, "%s(): pName is NULL", __FUNCTION__);
            return FALSE;
        }

        const char* allowed[] = { "GlobalManager" };

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
