#include "MainProcess.h"
#include "AppLogger.h"
#include "MemoryUtility.h"
#include "HydraFramework.h"
#include "NetEngine/NetEngine.h"

namespace AgentServer
{
    void CMainProcess::SetupHook()
    {
        MEMUTIL_VFTABLE_HOOK(0x0051B4E4, 7, &CMainProcess::PrepareMsgHandlers);
        MEMUTIL_VFTABLE_HOOK(0x0051B4E4, 8, &CMainProcess::RegisterMsgHandler);
        MEMUTIL_VFTABLE_HOOK(0x0051B4E4, 9, &CMainProcess::HandleMsg);
    }

    BOOL CMainProcess::PrepareMsgHandlers()
    {
        RegisterMsgHandler(SERVER_ONLY_REQ, addr_from_this(&CMainProcess::CustomMsg));
        RegisterMsgHandler(SERVER_ONLY_ACK, addr_from_this(&CMainProcess::CustomMsg));
        return reinterpret_thiscall(0x004261F0, BOOL, this);
    }

    BOOL CMainProcess::RegisterMsgHandler(WORD wMsgID, uintptr_t fnPtr)
    {
        return reinterpret_thiscall(0x00467660, BOOL, this, wMsgID, fnPtr);
    }

    BOOL CMainProcess::HandleMsg(CMsg *pMsg, DWORD a3, LPVOID a4, CMassiveMsg *pMassiveMsg)
    {
        if (pMsg && pMsg->GetMsgID() == MODULE_CERTIFICATION_ACK && pMsg->GetMsgSize() > 64)
            CustomMsg(pMsg, NULL, NULL, NULL);

        return reinterpret_thiscall(0x00428830, BOOL, this, pMsg, a3, a4, pMassiveMsg);
    }

    BOOL CMainProcess::CustomMsg(CMsg *pMsg, DWORD, LPVOID, CMassiveMsg*)
    {
        if (!pMsg)
            return FALSE;

        DWORD dwSessionID = pMsg->GetSessionID();
        WORD wMsgID = pMsg->GetMsgID();

        switch (wMsgID) {
            case MODULE_CERTIFICATION_ACK: { //0x600D
                if (pMsg->GetMsgSize() > 64)
                {
                    BYTE listMarker = 0;

                    pMsg->Read(listMarker);
                    while (true) {
                        BYTE flag = pMsg->Read<BYTE>();
                        if (flag == 2) break; // End of list marker
                        if (flag == 1) {
                            sServerBody body;

                            body.DivisionID = pMsg->Read<BYTE>();
                            body.FarmID = pMsg->Read<BYTE>();
                            BYTE randomJunkCount = pMsg->Read<BYTE>();
                            body.ModuleID = pMsg->Read<BYTE>();
                            body.ID = pMsg->Read<WORD>();
                            body.ModuleType = pMsg->Read<BYTE>();

                            // Skip random junk bytes loop
                            for (int i = 0; i < randomJunkCount % 5; i++) {
                                pMsg->Read<BYTE>();
                            }

                            body.CertifyID = pMsg->Read<WORD>();
                            body.ListenerPort = pMsg->Read<WORD>();
                            body.ShardID = pMsg->Read<WORD>();
                            body.MachineID = pMsg->Read<DWORD>();
                            body.State = pMsg->Read<DWORD>();

                            g_pHFW->m_ServerInfo.ServerBodyMap[body.ID] = body;
                        }
                    }

                    // 2. Parse Modules
                    pMsg->Read(listMarker);
                    while (true) {
                        BYTE flag = pMsg->Read<BYTE>();
                        if (flag == 2) break;
                        if (flag == 1) {
                            sModule module;

                            module.ID = pMsg->Read<BYTE>();
                            pMsg->Read(module.Name);

                            g_pHFW->m_ServerInfo.ModuleMap[module.ID] = module;
                        }
                    }

                    // 3. Parse Content
                    pMsg->Read(listMarker);
                    while (true) {
                        BYTE flag = pMsg->Read<BYTE>();
                        if (flag == 2) break;
                        if (flag == 1) {
                            sContent content;

                            content.ID = pMsg->Read<BYTE>();
                            pMsg->Read(content.Name);

                            g_pHFW->m_ServerInfo.ContentMap[content.ID] = content;
                        }
                    }

                    // 4. Parse Divisions
                    pMsg->Read(listMarker);
                    while (true) {
                        BYTE flag = pMsg->Read<BYTE>();
                        if (flag == 2) break;
                        if (flag == 1) {
                            sDivision div;

                            div.ID = pMsg->Read<BYTE>();
                            div.ManagerID = pMsg->Read<WORD>();
                            pMsg->Read(div.Name);
                            pMsg->Read(div.DbConfigAccount);

                            g_pHFW->m_ServerInfo.DivisionMap[div.ID] = div;
                        }
                    }

                    // 5. Parse FarmContent
                    pMsg->Read(listMarker);
                    BYTE index = 0;
                    while (true) {
                        BYTE flag = pMsg->Read<BYTE>();
                        if (flag == 2) break;
                        if (flag == 1) {
                            sFarmContent farm;

                            farm.nID = index;
                            farm.FarmID = pMsg->Read<BYTE>();
                            farm.ContentID = pMsg->Read<BYTE>();

                            g_pHFW->m_ServerInfo.FarmContentMap[index] = farm;
                        }
                        index++;
                    }

                    // 6. Parse Farms
                    pMsg->Read(listMarker);
                    while (true) {
                        BYTE flag = pMsg->Read<BYTE>();
                        if (flag == 2) break;
                        if (flag == 1) {
                            sFarm farm;

                            farm.ID = pMsg->Read<BYTE>();
                            farm.DivisionID = pMsg->Read<BYTE>();
                            pMsg->Read(farm.Name);
                            pMsg->Read(farm.DbConfig);

                            g_pHFW->m_ServerInfo.FarmMap[farm.ID] = farm;
                        }
                    }

                    // 7. Parse ServerMachines
                    pMsg->Read(listMarker);
                    while (true) {
                        BYTE flag = pMsg->Read<BYTE>();
                        if (flag == 2) break;
                        if (flag == 1) {
                            sServerMachine server;

                            server.ID = pMsg->Read<DWORD>();
                            server.DivisionID = pMsg->Read<BYTE>();
                            pMsg->Read(server.Name);
                            pMsg->Read(server.PublicIP);
                            pMsg->Read(server.PrivateIP);
                            pMsg->Read<WORD>(); //zeroVal, not used
                            server.ManagerID = pMsg->Read<WORD>();

                            g_pHFW->m_ServerInfo.ServerMachineMap[server.ID] = server;
                        }
                    }

                    // 8. Parse ServerCords
                    pMsg->Read(listMarker);
                    while (true) {
                        BYTE flag = pMsg->Read<BYTE>();
                        if (flag == 2) break;
                        if (flag == 1) {
                            sServerCord serverCord;

                            serverCord.ID = pMsg->Read<DWORD>(); //cordId, not used
                            serverCord.OutletID = pMsg->Read<WORD>(); //outletId, not used
                            BYTE randomJunkByte0 = pMsg->Read<BYTE>();
                            serverCord.InletID = pMsg->Read<WORD>(); //inletId, not used
                            serverCord.State = pMsg->Read<DWORD>(); //state, not used
                            serverCord.BindType = pMsg->Read<BYTE>(); //bindType, not used
                            serverCord.SessionID = pMsg->Read<DWORD>(); //sessionId, not used

                            g_pHFW->m_ServerInfo.ServerCordMap[serverCord.ID] = serverCord;

                            if (randomJunkByte0 % 7 == 0) {
                                pMsg->Read<BYTE>();
                            }
                        }
                    }

                    // 9. Parse Shards
                    pMsg->Read(listMarker);
                    while (true) {
                        BYTE flag = pMsg->Read<BYTE>();
                        if (flag == 2) break;
                        if (flag == 1) {
                            sShard shard;

                            shard.ID = pMsg->Read<WORD>();
                            shard.MaxUser = pMsg->Read<WORD>();
                            shard.ManagerID = pMsg->Read<WORD>();
                            shard.ContentID = pMsg->Read<BYTE>();
                            pMsg->Read(shard.Name);
                            pMsg->Read(shard.DbConfigShard);
                            pMsg->Read(shard.DbConfigLog);
                            shard.FarmID = pMsg->Read<BYTE>();
                            shard.ShardService = pMsg->Read<BYTE>();
                            shard.CurrentUsers = pMsg->Read<WORD>();

                            g_pHFW->m_ServerInfo.ShardMap[shard.ID] = shard;
                        }
                    }

                    pMsg->ResetPos();

                    // *DO NOT DELETE*
                    return TRUE;
                }
                break;
            }
            case SERVER_ONLY_REQ: {
                //TODO
                break;
            }
            case SERVER_ONLY_ACK: {
                //TODO
                break;
            }
            default: {
                char buffer[256];
                ZeroMemory(buffer, sizeof(buffer));
                reinterpret_cdecl(0x00463640, char*, buffer, pMsg);
                PutLog(FATAL, "Unhandled msg received %s", buffer);
            }
        }

        DELMSG(pMsg);
        return TRUE;
    }
}
