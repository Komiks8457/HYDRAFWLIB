#include "MainProcess.h"
#include "MemoryUtility.h"
#include "HydraFramework.h"
#include "AppLogger.h"
#include "NetEngine/NetEngine.h"
#include "GatewayServer.h"
#include "NetEngine/ServerInfo.h"

namespace GatewayServer
{
    DWORD OnHandleMsg_returnAddress = 0x0145DBE5;

    void __cdecl ProcessFilteredMsg(CMsg* pMsg, void* pThis)
    {
        if (!pMsg) return;

        if (pMsg->GetMsgID() == MODULE_CERTIFICATION_ACK && pMsg->GetMsgSize() > 1024)
        {
            CMainProcess* pMainProcess = reinterpret_cast<CMainProcess*>(pThis);
            pMainProcess->CustomMsg(pMsg, 0, 0, NULL);
        }
    }

    __declspec(naked) void OnHandleMsg()
    {
        __asm
        {
            pushad;
            pushfd;
            mov eax, dword ptr ss:[esp + 28h];
            mov ecx, dword ptr ss:[esp + 1Ch];
            push ecx;
            push eax;
            call ProcessFilteredMsg;
            add esp, 8;
            popfd;
            popad;
            sub esp, 8;
            push ebx;
            push ebp;
            jmp OnHandleMsg_returnAddress;
        }
    }

    void CMainProcess::SetupHook()
    {
        MEMUTIL_VFTABLE_HOOK(0x01580C4C, 7, &CMainProcess::PrepareMsgHandlers);
        MEMUTIL_VFTABLE_HOOK(0x01580C4C, 8, &CMainProcess::RegisterMsgHandler);

        // TODO: disable for a moment since vanguard is hooked to mainprocess. We'll find another way later.
        //MEMUTIL_VFTABLE_HOOK(0x01580C4C, 9, &CMainProcess::HandleMsg);
        MEMUTIL_SETUP_HOOK(LongJump, 0x0145DBE0, OnHandleMsg);
    }

    BOOL CMainProcess::PrepareMsgHandlers()
    {
        RegisterMsgHandler(SERVER_ONLY_REQ, addr_from_this(&CMainProcess::CustomMsg));
        RegisterMsgHandler(SERVER_ONLY_ACK, addr_from_this(&CMainProcess::CustomMsg));
        return reinterpret_thiscall(0x014600B0, BOOL, this);
    }

    BOOL CMainProcess::RegisterMsgHandler(WORD wMsgID, uintptr_t fnPtr)
    {
        return reinterpret_thiscall(0x014BE170, BOOL, this, wMsgID, fnPtr);
    }

    BOOL CMainProcess::HandleMsg(CMsg *pMsg, DWORD a3, LPVOID a4, CMassiveMsg *pMassiveMsg)
    {
        if (pMsg && pMsg->GetMsgID() == MODULE_CERTIFICATION_ACK && pMsg->GetMsgSize() > 1024)
            CustomMsg(pMsg, NULL, NULL, NULL);

        return reinterpret_thiscall(0x0145DBE0, BOOL, this, pMsg, a3, a4, pMassiveMsg);
    }

    BOOL CMainProcess::CustomMsg(CMsg *pMsg, DWORD, LPVOID, CMassiveMsg*)
    {
        if (!pMsg)
            return FALSE;

        DWORD dwSessionID = pMsg->GetSessionID();
        WORD wMsgID = pMsg->GetMsgID();

        switch (wMsgID) {
            case MODULE_CERTIFICATION_ACK: { //0x600D
                if (pMsg->GetMsgSize() > 1024)
                {
                    BYTE listMarker = 0;
                    ServerBody tempServerBodies;
                    Division tempDivisions;
                    Shard tempShards;

                    pMsg->Read(listMarker);
                    while (true) {
                        BYTE flag = pMsg->Read<BYTE>();
                        if (flag == 2) break; // End of list marker
                        if (flag == 1) {
                            sServerBody body;

                            body.DivisionID = pMsg->Read<BYTE>(); //divisionID
                            pMsg->Read<BYTE>(); //farmID, not used
                            BYTE randomJunkCount = pMsg->Read<BYTE>();
                            BYTE moduleID = pMsg->Read<BYTE>(); //moduleID
                            body.ID = pMsg->Read<WORD>(); //ID
                            pMsg->Read<BYTE>(); //moduleType, not used

                            // Skip random junk bytes loop
                            for (int i = 0; i < randomJunkCount % 5; i++) {
                                pMsg->Read<BYTE>();
                            }

                            pMsg->Read<WORD>(); //certifierID, not used
                            pMsg->Read<WORD>(); //listenerPort, not used
                            body.ShardID = pMsg->Read<WORD>(); //shardID, not used
                            pMsg->Read<DWORD>(); //machineID, not used
                            pMsg->Read<DWORD>(); //state, not used

                            if (moduleID != 0)
                                tempServerBodies[moduleID] = body;
                        }
                    }

                    // 2. Parse Modules
                    pMsg->Read(listMarker);
                    while (true) {
                        BYTE flag = pMsg->Read<BYTE>();
                        if (flag == 2) break;
                        if (flag == 1) {
                            BYTE modId = pMsg->Read<BYTE>();
                            std::string modName;
                            pMsg->Read(modName);
                            ServerBody::iterator it = tempServerBodies.find(modId);
                            if (it != tempServerBodies.end()) {
                                it->second.Name = modName;
                            }
                        }
                    }

                    // 3. Parse Content
                    pMsg->Read(listMarker);
                    while (true) {
                        BYTE flag = pMsg->Read<BYTE>();
                        if (flag == 2) break;
                        if (flag == 1) {
                            pMsg->Read<BYTE>(); //contentID, not used
                            pMsg->ReadStringA(); //contentName, not used
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
                            pMsg->Read<WORD>();
                            pMsg->Read(div.Name);
                            pMsg->Read(div.DbConfigAccount);

                            tempDivisions[div.ID] = div;
                        }
                    }

                    // 5. Parse FarmContent
                    pMsg->Read(listMarker);
                    while (true) {
                        BYTE flag = pMsg->Read<BYTE>();
                        if (flag == 2) break;
                        if (flag == 1) {
                            pMsg->Read<BYTE>(); //farmID, not used
                            pMsg->Read<BYTE>(); //contentID, not used
                        }
                    }

                    // 6. Parse Farms
                    pMsg->Read(listMarker);
                    while (true) {
                        BYTE flag = pMsg->Read<BYTE>();
                        if (flag == 2) break;
                        if (flag == 1) {
                            pMsg->Read<BYTE>(); //farmID, not used
                            pMsg->Read<BYTE>(); //divisionID, not used
                            pMsg->ReadStringA(); //farmName, not used
                            pMsg->ReadStringA(); //dbConfig, not used
                        }
                    }

                    // 7. Parse ServerMachines
                    pMsg->Read(listMarker);
                    while (true) {
                        BYTE flag = pMsg->Read<BYTE>();
                        if (flag == 2) break;
                        if (flag == 1) {
                            pMsg->Read<DWORD>(); //machineId, not used
                            pMsg->Read<BYTE>(); //divisionId, not used
                            pMsg->ReadStringA(); //name, not used
                            pMsg->ReadStringA(); //publicIp, not used
                            pMsg->ReadStringA(); //privateIp, not used
                            pMsg->Read<WORD>(); //zeroVal, not used
                            pMsg->Read<WORD>(); //managerBodyId, not used
                        }
                    }

                    // 8. Parse ServerCords
                    pMsg->Read(listMarker);
                    while (true) {
                        BYTE flag = pMsg->Read<BYTE>();
                        if (flag == 2) break;
                        if (flag == 1) {
                            pMsg->Read<DWORD>(); //cordId, not used
                            pMsg->Read<WORD>(); //outletId, not used
                            BYTE randomJunkByte0 = pMsg->Read<BYTE>();
                            pMsg->Read<WORD>(); //inletId, not used
                            pMsg->Read<DWORD>(); //state, not used
                            pMsg->Read<BYTE>(); //bindType, not used
                            pMsg->Read<DWORD>(); //sessionId, not used

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
                            pMsg->Read<WORD>(); //maxUser, not used
                            pMsg->Read<WORD>(); //manageBodyID, not used
                            pMsg->Read<BYTE>(); //contentID, not used
                            pMsg->Read(shard.Name);
                            pMsg->Read(shard.DbConfigShard);
                            pMsg->Read(shard.DbConfigLog);
                            pMsg->Read<BYTE>(); //farmID, not used
                            pMsg->Read<BYTE>(); //shardService, not used
                            pMsg->Read<WORD>(); //currentUser, not used

                            tempShards[shard.ID] = shard;
                        }
                    }

                    // ==========================================
                    // 10. Populate m_ServerInfo Map
                    // ==========================================
                    g_pHFW->m_ServerInfo.clear();

                    ServerBody::iterator itBody = tempServerBodies.begin();
                    ServerBody::iterator endBody = tempServerBodies.end();
                    for (; itBody != endBody; ++itBody)
                    {
                        WORD modId = itBody->first;
                        const sServerBody& body = itBody->second;

                        sServerInfo serverInfo;

                        // Populate ServerBody map inside sServerInfo
                        serverInfo.ServerBodyMap[modId] = body;

                        // Bind matching Division if present
                        Division::iterator itDiv = tempDivisions.find(body.DivisionID);
                        if (itDiv != tempDivisions.end()) {
                            serverInfo.DivisionMap[itDiv->first] = itDiv->second;
                        }

                        // Bind matching Shard if present
                        Shard::iterator itShard = tempShards.find(body.ShardID);
                        if (itShard != tempShards.end()) {
                            serverInfo.ShardMap[itShard->first] = itShard->second;
                        }

                        g_pHFW->m_ServerInfo[modId] = serverInfo;
                    }

                    pMsg->ResetPos();

                    // *DO NOT DELETE*
                    return TRUE;
                }
                break;
            }
            case SERVER_ONLY_REQ: {
                // TODO
                break;
            }
            case SERVER_ONLY_ACK: {
                //TODO
                break;
            }
            default: {
                char buffer[256];
                ZeroMemory(buffer, sizeof(buffer));
                reinterpret_cdecl(0x014C1ED0, char*, buffer, pMsg);
                PutLog(FATAL, "Unhandled msg received %s", buffer);
            }
        }

        DELMSG(pMsg);
        return TRUE;
    }
}
