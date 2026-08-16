#include "AgentServer.h"
#include "AppLogger.h"
#include "HydraFramework.h"
#include "MemoryUtility.h"
#include "Vanguard.h"
#include "Classes/MainProcess.h"

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
            LoadLibraryW(g_pVanguard->m_dllPath.c_str());
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

    BYTE CAgentServer::SendMsgToServerBody(const char *pName, CMsg* pMsg)
    {
        if (!pName) {
            PutLog(FATAL, "%s(): pName is NULL", __FUNCTION__);
            return FALSE;
        }

        const char* allowed[] = { "FarmManager", "SR_ShardManager", "SR_GameServer" };

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

                if (!reinterpret_stdcall(0x00468B00, char, bodyID, pMsg))
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
