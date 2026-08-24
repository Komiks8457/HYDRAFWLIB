#include "GameServer.h"
#include "AppLogger.h"
#include "HydraFramework.h"
#include "MemoryUtility.h"
#include "Vanguard.h"
#include "Classes/MainProcess.h"
#include "Logger/Logger.h"
#include "NetEngine/NetEngine.h"

GameServer::CGameServer *g_pGameServer = NULL;

namespace GameServer
{
    const char* ModuleName = "SR_GameServer";

    std::string CGameServer::m_DbUID;
    std::string CGameServer::m_DbPWD;

    BOOL WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, BOOL)
    {
        stdext::inifile iniFile;

        // dword_10F6910 = (int)&dword_101EFF0
        MEMUTIL_WRITE_VALUE(uintptr_t, 0x010F6910, 0x0101EFF0);

        // assign our g_pGlobalManager global pointer
        g_pGameServer = *reinterpret_cast<CGameServer**>(0x010F6910); // or (CGameServer*)0x0101EFF0;

        if (iniFile.load("hydrafwlib.ini"))
        {
            g_pGameServer->DbUID(iniFile.getvalue("DbUserInfo", "UID", "NULL").c_str());
            g_pGameServer->DbPWD(iniFile.getvalue("DbUserInfo", "PWD", "NULL").c_str());
        }

        if (g_pVanguard) {
            // load only if CVanguard was initilized in HydraDLL::CLibrary
            g_pVanguard->InitDll();
        }

        // return sub_BCB7A0(0, "SR_GameServer", (int)hInstance, (int)lpCmdLine, 1, 0);
        return reinterpret_cdecl(0x00BCB7A0, BOOL, 0, ModuleName, (BOOL)hInstance, (BOOL)lpCmdLine, 1, 0);
    }

    void CGameServer::Initialize()
    {
        // Hook LogWriter first
        CAppLogger::SetupHook(0x00BCA8A0);

        // MainProcess hook
        CMainProcess::SetupHook();

        // WinMain hook
        MEMUTIL_REPLACE_OFFSET(0x00C764C7, &WinMain);

        // CGameServer vftable hooks
        MEMUTIL_VFTABLE_HOOK(0x00DAFA04,  1, &CGameServer::init_module);
        MEMUTIL_VFTABLE_HOOK(0x00DAFA04, 11, &CGameServer::init_localdata);

        // ValidateMsg hook
        MEMUTIL_REPLACE_OFFSET(0x00BC9F47, &CGameServer::check_valid_msgid);

        // Disable ENDLESS_LOOP dump
        MEMUTIL_SETUP_HOOK(LongCall, 0x00BF830C, HydraFramework::NO_ENDLESS_LOOP_DUMP);

        // Static Patches
        MEMUTIL_WRITE_VALUE(BYTE, 0x00902673 + 1, (BYTE)0xC8); //MOB_THIEF
        MEMUTIL_WRITE_VALUE(BYTE, 0x00902787 + 2, (BYTE)0xC7); //MOB_THIEF

        MEMUTIL_WRITE_VALUE(BYTE, 0x009028A3 + 1, (BYTE)0xC8); //MOB_HUNTER
        MEMUTIL_WRITE_VALUE(BYTE, 0x009029B7 + 2, (BYTE)0xC7); //MOB_HUNTER

        MEMUTIL_WRITE_VALUE(BYTE, 0x008FDC70 + 1, (BYTE)0xC8); //UNK
        MEMUTIL_WRITE_VALUE(BYTE, 0x008FDD95 + 2, (BYTE)0xC8); //UNK

        MEMUTIL_WRITE_VALUE(BYTE, 0x008D4C56 + 1, (BYTE)0xC8); //UNK

        MEMUTIL_WRITE_VALUE(BYTE, 0x008E77F1 + 1, (BYTE)0xC8); //MERCENARY
        MEMUTIL_WRITE_VALUE(BYTE, 0x008E79D4 + 1, (BYTE)0xC8); //SEIGEGUARD

        std::srand(static_cast<unsigned>(std::time(0)));
    }

    BOOL CGameServer::init_module()
    {
        CHydraProcess::CreateInstance(this, ModuleName);

        if (!g_pHFW) {
            PutLog(FATAL, "Failed to initialize HydraFramework");
            return FALSE;
        }

        return reinterpret_thiscall(0x00BC87E0, BOOL, this);
    }

    BOOL CGameServer::init_localdata()
    {
        char oldTitle[MAX_PATH], newTitle[MAX_PATH];
        GetConsoleTitleA(oldTitle, sizeof(oldTitle));
        sprintf_s(newTitle, "[%d] %s", GetMyServerBodyID(), oldTitle);
        SetConsoleTitleA(newTitle);

        ServerInfo::iterator it = g_pHFW->m_ServerInfo.begin();
        ServerInfo::iterator endIt = g_pHFW->m_ServerInfo.end();

        for (; it != endIt; ++it)
        {
            // Access ServerBody map inside sServerInfo to check the name
            ServerBody::iterator bodyIt = it->second.ServerBodyMap.begin();
            if (bodyIt == it->second.ServerBodyMap.end() || bodyIt->second.Name != "SR_ShardManager")
                continue;

            WORD divisionID = bodyIt->second.DivisionID;
            WORD shardID = bodyIt->second.ShardID;

            // Access DivisionInfo map nested inside sServerInfo
            Division::iterator divIt = it->second.DivisionMap.find(divisionID);
            if (divIt != it->second.DivisionMap.end())
            {
                stdext::replacesqlinfo(divIt->second.DbConfigAccount,
                    DbUID().c_str(),
                    DbPWD().c_str());

                g_pHFW->m_dbConn.ACCOUNT = divIt->second.DbConfigAccount.c_str();
            }

            // Access ShardInfo map nested inside sServerInfo
            Shard::iterator shardIt = it->second.ShardMap.find(shardID);
            if (shardIt != it->second.ShardMap.end())
            {
                stdext::replacesqlinfo(shardIt->second.DbConfigShard,
                    DbUID().c_str(),
                    DbPWD().c_str());

                g_pHFW->m_dbConn.SHARD = shardIt->second.DbConfigShard.c_str();

                stdext::replacesqlinfo(shardIt->second.DbConfigLog,
                    DbUID().c_str(),
                    DbPWD().c_str());

                g_pHFW->m_dbConn.LOG = shardIt->second.DbConfigLog.c_str();
            }
        }

        if (!g_pHFW->InitDbMgr(5, 10))
            return FALSE;

        ShowWindow(GetConsoleWindow(), SW_FORCEMINIMIZE);

        return reinterpret_thiscall(0x00481680, BOOL, this);
    }

    BOOL CGameServer::check_valid_msgid(int a2, WORD wMsgID, int a4)
    {
        if (wMsgID == SERVER_ONLY_ACK || wMsgID == SERVER_ONLY_REQ)
            return 0;

        return reinterpret_thiscall(0x00BF9110, BOOL, this, a2, wMsgID, a4);
    }

    BYTE CGameServer::GetMyServerBodyID()
    {
        return *(BYTE*)MEMUTIL_ADD_PTR(*reinterpret_cast<uintptr_t*>(0x010F6E54), 0);
    }

    HWND CGameServer::GetWinHandle()
    {
        return *(HWND*)MEMUTIL_ADD_PTR(*reinterpret_cast<uintptr_t*>(0x010F6E48), 0x14);
    }

    BYTE CGameServer::SendMsgToServerBody(const char *pName, CMsg* pMsg)
    {
        if (!pName) {
            PutLog(FATAL, "%s(): pName is NULL", __FUNCTION__);
            return FALSE;
        }

        const char* allowed[] = { "FarmManager", "AgentServer", "SR_ShardManager" };

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

                if (!reinterpret_stdcall(0x00BE0650, char, bodyID, pMsg))
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
