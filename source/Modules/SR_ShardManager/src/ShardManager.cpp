#include "ShardManager.h"
#include "AppLogger.h"
#include "HydraFramework.h"
#include "MemoryUtility.h"
#include "Vanguard.h"
#include "Classes/MainProcess.h"
#include "NetEngine/NetEngine.h"

ShardManager::CShardManager *g_pShardManager = NULL;

namespace ShardManager
{
    const char* ModuleName = "SR_ShardManager";

    std::string CShardManager::m_DbUID;
    std::string CShardManager::m_DbPWD;

    BOOL WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, BOOL)
    {
        stdext::inifile iniFile;

        //dword_D19008 = (int)&dword_CDE830;
        MEMUTIL_WRITE_VALUE(uintptr_t, 0x00D19008, 0x00CDE830);

        //assign our g_pGlobalManager global pointer
        g_pShardManager = *reinterpret_cast<CShardManager**>(0x00D19008); // or (CShardManager*)0x00CDE830;

        if (iniFile.load("hydrafwlib.ini"))
        {
            g_pShardManager->DbUID(iniFile.getvalue("DbUserInfo", "UID", "NULL").c_str());
            g_pShardManager->DbPWD(iniFile.getvalue("DbUserInfo", "PWD", "NULL").c_str());
        }

        if (g_pVanguard) {
            // load only if CVanguard was initilized in HydraDLL::CLibrary
            g_pVanguard->InitDll();
        }

        //return sub_9D8E90(0, (CHAR *)"SR_ShardManager", hInstance, (int)lpCmdLine, 1, 0);
        return reinterpret_cdecl(0x009D8E90, BOOL, 0, ModuleName, (BOOL)hInstance, (BOOL)lpCmdLine, 1, 0);
    }

    void CShardManager::Initialize()
    {
        // Hook LogWriter first
        CAppLogger::SetupHook(0x009D8090);

        // MainProcess hook
        CMainProcess::SetupHook();

        // WinMain hook
        MEMUTIL_REPLACE_OFFSET(0x00A531D8, &WinMain);

        // ValidateMsgID hook
        MEMUTIL_REPLACE_OFFSET(0x009F15F7, &CShardManager::check_valid_msgid);

        // Disable ENDLESS_LOOP dump
        MEMUTIL_SETUP_HOOK(LongCall, 0x00A0B4FC, HydraFramework::NO_ENDLESS_LOOP_DUMP);

        // CShardManager vftable hooks
        MEMUTIL_VFTABLE_HOOK(0x00B560EC,  1, &CShardManager::init_module);
        MEMUTIL_VFTABLE_HOOK(0x00B560EC, 11, &CShardManager::init_localdata);

        // Static Patches
        MEMUTIL_WRITE_VALUE(BYTE, 0x0075EB1B + 1, (BYTE)0xC8); //MOB_THIEF
        MEMUTIL_WRITE_VALUE(BYTE, 0x0075EC2F + 2, (BYTE)0xC7); //MOB_THIEF

        MEMUTIL_WRITE_VALUE(BYTE, 0x0075ED3B + 1, (BYTE)0xC8); //MOB_HUNTER
        MEMUTIL_WRITE_VALUE(BYTE, 0x0075EE4F + 2, (BYTE)0xC7); //MOB_HUNTER

        MEMUTIL_WRITE_VALUE(BYTE, 0x00759D4F + 1, (BYTE)0xC8); //UNK
        MEMUTIL_WRITE_VALUE(BYTE, 0x00759E75 + 2, (BYTE)0xC8); //UNK

        MEMUTIL_WRITE_VALUE(BYTE, 0x0072E217 + 1, (BYTE)0xC8); //UNK

        MEMUTIL_WRITE_VALUE(BYTE, 0x0074201C + 1, (BYTE)0xC8); //MERCENARY
        MEMUTIL_WRITE_VALUE(BYTE, 0x007421EF + 1, (BYTE)0xC8); //SIEGEGUARD
    }

    BOOL CShardManager::init_module()
    {
        CHydraProcess::CreateInstance(this, ModuleName);

        if (!g_pHFW) {
            PutLog(FATAL, "Failed to initialize Hydra.");
            return FALSE;
        }

        return reinterpret_thiscall(0x009EFE90, BOOL, this);
    }

    BOOL CShardManager::init_localdata()
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

        return reinterpret_thiscall(0x0059F910, BOOL, this);
    }

    BOOL CShardManager::check_valid_msgid(int a2, WORD wMsgID, int a4)
    {
        if (wMsgID == SERVER_ONLY_ACK || wMsgID == SERVER_ONLY_REQ)
            return 0;

        return reinterpret_thiscall(0x00A0C300, BOOL, this, a2, wMsgID, a4);
    }

    BYTE CShardManager::GetMyServerBodyID()
    {
        return *(BYTE*)MEMUTIL_ADD_PTR(*reinterpret_cast<uintptr_t*>(0x00D193F0), 0);
    }

    HWND CShardManager::GetWinHandle()
    {
        return *(HWND*)MEMUTIL_ADD_PTR(*reinterpret_cast<uintptr_t*>(0x00D19E34), 0x14);
    }

    BOOL CShardManager::SendMsgToServerBody(const char *pName, CMsg* pMsg)
    {
        if (!pName) {
            PutLog(FATAL, "%s(): pName is NULL", __FUNCTION__);
            return FALSE;
        }

        const char* allowed[] = { "FarmManager", "AgentServer", "SR_GameServer" };

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
            if (!reinterpret_stdcall(0x009EC950, char, serverBodyID, pMsg))
            {
                PutLog(FATAL, "Failed to send msg to body ID(%d), MsgID(0x%x)", serverBodyID, pMsg->GetMsgID());
                return FALSE;
            }
        }

        return TRUE;
    }
}
