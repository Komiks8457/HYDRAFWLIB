#include "AgentServer.h"
#include "AppLogger.h"
#include "HydraFramework.h"
#include "MemoryUtility.h"
#include "Vanguard.h"
#include "Classes/MainProcess.h"
#include "NetEngine/NetEngine.h"

AgentServer::CAgentServer* g_pAgentServer = NULL;

namespace AgentServer
{
    const char* ModuleName = "AgentServer";

    std::string CAgentServer::m_DbUID;
    std::string CAgentServer::m_DbPWD;

    BOOL WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, BOOL)
    {
        stdext::inifile iniFile;

        //dword_5A30D8 = (int)&dword_5923F8;
        MEMUTIL_WRITE_VALUE(uintptr_t, 0x005A30D8, 0x005923F8);

        //assign our g_pAgentServer global pointer
        g_pAgentServer = *reinterpret_cast<CAgentServer**>(0x005A30D8); // or (CAgentServer*)0x005923F8;

        if (iniFile.load("hydrafwlib.ini"))
        {
            g_pAgentServer->DbUID(iniFile.getvalue("DbUserInfo", "UID", "NULL").c_str());
            g_pAgentServer->DbPWD(iniFile.getvalue("DbUserInfo", "PWD", "NULL").c_str());
        }

        if (g_pVanguard) {
            // load only if CVanguard was initilized in HydraDLL::CLibrary
            g_pVanguard->InitDll();
        }

        //return sub_466CB0(0, (CHAR *)"AgentServer", hInstance, (int)lpCmdLine, 1, 0);
        return reinterpret_cdecl(0x00466CB0, BOOL, 0, ModuleName, hInstance, (BOOL)lpCmdLine, 1, 0);
    }

    void CAgentServer::Initialize()
    {
        // Hook LogWriter first
        CAppLogger::SetupHook(0x00464DD0);

        // Other hooks
        CMainProcess::SetupHook();

        // WinMain hook
        MEMUTIL_REPLACE_OFFSET(0x004DE08B, &WinMain);

        // CAgentServer vftable hooks
        MEMUTIL_VFTABLE_HOOK(0x0051B684,  1, &CAgentServer::init_module);
        MEMUTIL_VFTABLE_HOOK(0x0051B684, 11, &CAgentServer::init_localdata);

        // ValidateMsgID hook
        MEMUTIL_REPLACE_OFFSET(0x0046F527, &CAgentServer::check_valid_msgid);

        // Disable ENDLESS_LOOP dump
        MEMUTIL_SETUP_HOOK(LongCall, 0x0048EDBC, HydraFramework::NO_ENDLESS_LOOP_DUMP);
    }

    BOOL CAgentServer::init_module()
    {
        CHydraProcess::CreateInstance(this, ModuleName);

        if (!g_pHFW) {
            PutLog(FATAL, "Failed to initialize Hydra");
            return FALSE;
        }

        return reinterpret_thiscall(0x0046DB40, BOOL, this);
    }

    BOOL CAgentServer::init_localdata()
    {
        char oldTitle[MAX_PATH], newTitle[MAX_PATH];
        GetConsoleTitleA(oldTitle, sizeof(oldTitle));
        sprintf_s(newTitle, "[%d] %s", GetMyServerBodyID(), oldTitle);
        SetConsoleTitleA(newTitle);

        ShowWindow(GetConsoleWindow(), SW_FORCEMINIMIZE);

        return reinterpret_thiscall(0x004291D0, BOOL, this);
    }

    BOOL CAgentServer::check_valid_msgid(int a2, WORD wMsgID, int a4)
    {
        if (wMsgID == SERVER_ONLY_ACK || wMsgID == SERVER_ONLY_REQ)
            return 0;

        return reinterpret_thiscall(0x0048FBE0, BOOL, this, a2, wMsgID, a4);
    }

    BYTE CAgentServer::GetMyServerBodyID()
    {
        return *(BYTE*)MEMUTIL_ADD_PTR(*reinterpret_cast<uintptr_t*>(0x005A2F7C), 0);
    }

    HWND CAgentServer::GetWinHandle()
    {
        return *(HWND*)MEMUTIL_ADD_PTR(*reinterpret_cast<uintptr_t*>(0x005A3E34), 0x14);
    }

    void CAgentServer::SendMsgToServerBody(E_MODULE::Type Type, CMsg* pMsg)
    {
        if (!pMsg) return;

        E_MODULE::Type allowed[] = { E_MODULE::FarmManager, E_MODULE::SR_ShardManager, E_MODULE::SR_GameServer };

        bool bAllowed = false;
        for (size_t i = 0; i < sizeof(allowed) / sizeof(allowed[0]); ++i)
        {
            if (Type == allowed[i])
            {
                bAllowed = true;
                break;
            }
        }

        if (!bAllowed) {
            PutLog(FATAL, "%s(): pName(%s) not allowed", __FUNCTIONP__, E_MODULE::GetModuleName(Type));
            return;
        }

        std::vector<WORD> serverBodyList = g_pHFW->m_ServerInfo.GetServerBodyIDByModuleName(Type);

        if (serverBodyList.empty()) {
            PutLog(FATAL, "%s(): pName(%s) not found", __FUNCTIONP__,  E_MODULE::GetModuleName(Type));
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

            if (!reinterpret_stdcall(0x00468B00, char, (int)serverBodyID, pMsg))
                PutLog(FATAL, "Failed to send msg to body ID(%d), MsgID(0x%x)", serverBodyID, pMsg->GetMsgID());
        }
    }
}
