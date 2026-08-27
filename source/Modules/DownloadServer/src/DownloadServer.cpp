#include "DownloadServer.h"
#include "AppLogger.h"
#include "HydraFramework.h"
#include "MemoryUtility.h"
#include "Vanguard.h"
#include "Classes/MainProcess.h"
#include "NetEngine/NetEngine.h"

DownloadServer::CDownloadServer* g_pDownloadServer = NULL;

namespace DownloadServer
{
    const char* ModuleName = "DownloadServer";

    std::string CDownloadServer::m_DbUID;
    std::string CDownloadServer::m_DbPWD;

    BOOL WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, BOOL)
    {
        stdext::inifile iniFile;

        //dword_1561078 = (int)&unk_153FEC8;
        MEMUTIL_WRITE_VALUE(uintptr_t, 0x01561078, 0x0153FEC8);

        //assign our g_pDownloadServer global pointer
        g_pDownloadServer = *reinterpret_cast<CDownloadServer**>(0x01561078); // or (CDownloadServer*)0x0153FEC8;

        if (iniFile.load("hydrafwlib.ini"))
        {
            g_pDownloadServer->DbUID(iniFile.getvalue("DbUserInfo", "UID", "NULL").c_str());
            g_pDownloadServer->DbPWD(iniFile.getvalue("DbUserInfo", "PWD", "NULL").c_str());
        }

        if (g_pVanguard) {
            // load only if CVanguard was initilized in HydraDLL::CLibrary
            g_pVanguard->InitDll();
        }

        //return sub_14507F0(0, "DownloadServer", (int)hInstance, (int)lpCmdLine, 1, 0);
        return reinterpret_cdecl(0x014507F0, BOOL, 0, ModuleName, hInstance, (BOOL)lpCmdLine, 1, 0);
    }

    void CDownloadServer::Initialize()
    {
        // Hook LogWriter first
        CAppLogger::SetupHook(0x0144E910);

        // Other hooks
        CMainProcess::SetupHook();

        // WinMain hook
        MEMUTIL_REPLACE_OFFSET(0x014E2644, &WinMain);

        // CDownloadServer vftable hooks
        MEMUTIL_VFTABLE_HOOK(0x0151AE84,  1, &CDownloadServer::init_module);
        MEMUTIL_VFTABLE_HOOK(0x0151AE84, 11, &CDownloadServer::init_localdata);

        // ValidateMsg hook
        MEMUTIL_REPLACE_OFFSET(0x0144D7A7, &CDownloadServer::check_valid_msgid);

        // Disable ENDLESS_LOOP dump
        MEMUTIL_SETUP_HOOK(LongCall, 0x0147B6EC, HydraFramework::NO_ENDLESS_LOOP_DUMP);
    }

    BOOL CDownloadServer::init_module()
    {
        CHydraProcess::CreateInstance(this, ModuleName);

        if (!g_pHFW) {
            PutLog(FATAL, "Failed to initialize Hydra");
            return FALSE;
        }

        return reinterpret_thiscall(0x0144BC50, BOOL, this);
    }

    BOOL CDownloadServer::init_localdata()
    {
        char oldTitle[MAX_PATH], newTitle[MAX_PATH];
        GetConsoleTitleA(oldTitle, sizeof(oldTitle));
        sprintf_s(newTitle, "[%d] %s", GetMyServerBodyID(), oldTitle);
        SetConsoleTitleA(newTitle);

        ShowWindow(GetConsoleWindow(), SW_FORCEMINIMIZE);

        return reinterpret_thiscall(0x01435C80, BOOL, this);
    }

    BOOL CDownloadServer::check_valid_msgid(int a2, WORD wMsgID, int a4)
    {
        if (wMsgID == SERVER_ONLY_ACK || wMsgID == SERVER_ONLY_REQ)
            return 0;

        return reinterpret_thiscall(0x01485F20, BOOL, this, a2, wMsgID, a4);
    }

    BOOL CDownloadServer::GetMyServerBodyID()
    {
        return *(BYTE*)MEMUTIL_ADD_PTR(*reinterpret_cast<uintptr_t*>(0x01561574), 0);
    }

    HWND CDownloadServer::GetWinHandle()
    {
        return *(HWND*)MEMUTIL_ADD_PTR(*reinterpret_cast<uintptr_t*>(0x0156174C), 0x14);
    }

    void CDownloadServer::SendMsgToServerBody(E_MODULE::Type Type, CMsg* pMsg)
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

            if (!reinterpret_stdcall(0x01449330, char, (int)serverBodyID, pMsg))
                PutLog(FATAL, "Failed to send msg to body ID(%d), MsgID(0x%x)", serverBodyID, pMsg->GetMsgID());
        }
    }
}
