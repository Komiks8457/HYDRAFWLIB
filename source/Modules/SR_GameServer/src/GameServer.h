#pragma once

#include "pch.h"

class CMsg;

namespace GameServer
{
    extern const char* ModuleName;

    class CGameServer {
    public:
        static void Initialize();
        BYTE GetMyServerBodyID();
        HWND GetWinHandle();
        BYTE SendMsgToServerBody(const char *pName, CMsg *pMsg);

    private:
        BOOL init_module();
        BOOL init_localdata();
        BOOL check_valid_msgid(int a2, WORD wMsgID, int a4);

        static std::string m_DbUID;
        static std::string m_DbPWD;

    public:
        static void DbUID(const std::string& uid) { m_DbUID = uid; }
        static void DbPWD(const std::string& pwd) { m_DbPWD = pwd; }
        static std::string& DbUID() { return m_DbUID; }
        static std::string& DbPWD() { return m_DbPWD; }
    };
}

extern GameServer::CGameServer *g_pGameServer;
