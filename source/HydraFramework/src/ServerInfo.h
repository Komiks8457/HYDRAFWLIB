#pragma once

#ifndef SERVERINFO_H
#define SERVERINFO_H

#include "pch.h"
#include "AppLogger.h"

struct sServerBody
{
    WORD ID;
    BYTE DivisionID;
    BYTE FarmID;
    BYTE ModuleID;
    BYTE ModuleType;
    WORD CertifyID;
    WORD ListenerPort;
    WORD ShardID;
    WORD MachineID;
    WORD State;

    sServerBody() : ID(0), DivisionID(0), FarmID(0), ModuleID(0), ModuleType(0), CertifyID(0), ListenerPort(0),
                    ShardID(0), MachineID(0), State(0)
    {}
};
typedef std::map<WORD, sServerBody> ServerBody;

struct sDivision
{
    BYTE ID;
    WORD ManagerID;
    std::string Name;
    std::string DbConfigAccount;

    sDivision() : ID(0), ManagerID(0), Name(""), DbConfigAccount("") {
    }
};
typedef std::map<BYTE, sDivision> Division;

struct sFarm
{
    BYTE ID;
    BYTE DivisionID;
    std::string Name;
    std::string DbConfig;

    sFarm() : ID(0), DivisionID(0), Name(""), DbConfig("")
    {}
};
typedef std::map<BYTE, sFarm> Farm;

struct sFarmContent
{
    BYTE nID;
    BYTE FarmID;
    BYTE ContentID;

    sFarmContent() : nID(0), FarmID(0), ContentID(0)
    {}
};
typedef std::map<BYTE, sFarmContent> FarmContent;

struct sModule
{
    BYTE ID;
    std::string Name;

    sModule() : ID(0), Name("")
    {}
};
typedef std::map<BYTE, sModule> Module;

struct sContent
{
    BYTE ID;
    std::string Name;

    sContent() : ID(0), Name("")
    {}
};
typedef std::map<BYTE, sContent> Content;

struct sServerMachine
{
    DWORD ID;
    BYTE DivisionID;
    std::string Name;
    std::string PublicIP;
    std::string PrivateIP;
    WORD ManagerID;

    sServerMachine() : ID(0), DivisionID(0), Name(""), PublicIP(""), ManagerID(0)
    {}
};
typedef std::map<DWORD, sServerMachine> ServerMachine;

struct sServerCord
{
    DWORD ID;
    WORD OutletID;
    WORD InletID;
    DWORD State;
    BYTE BindType;
    DWORD SessionID;

    sServerCord() : ID(0), OutletID(0), InletID(0), State(0), BindType(0), SessionID(0)
    {}
};
typedef std::map<DWORD, sServerCord> ServerCord;

struct sShard
{
    WORD ID;
    WORD MaxUser;
    WORD ManagerID;
    BYTE ContentID;
    std::string Name;
    std::string DbConfigShard;
    std::string DbConfigLog;
    BYTE FarmID;
    BYTE ShardService;
    WORD CurrentUsers;

    sShard() : ID(0), MaxUser(0), ManagerID(0), ContentID(0), Name(""), DbConfigLog(""), FarmID(0),
               ShardService(0), CurrentUsers(0)
    {}
};
typedef std::map<WORD, sShard> Shard;

struct sCertificationBody
{
    ServerBody ServerBodyMap;
    Module ModuleMap;
    Content ContentMap;
    Division DivisionMap;
    FarmContent FarmContentMap;
    Farm FarmMap;
    ServerMachine ServerMachineMap;
    ServerCord ServerCordMap;
    Shard ShardMap;

    std::vector<WORD> GetServerBodyIDByModuleName(E_MODULE::Type Type) const
    {
        return GetServerBodyIDByModuleName(GetModuleName(Type));
    }

    std::vector<WORD> GetServerBodyIDByModuleName(const std::string& moduleName) const
    {
        std::vector<WORD> serverBodyIDs;
        BYTE targetModuleID = 0;
        bool moduleFound = false;

        // Step 1: Find the Module ID corresponding to the given ModuleName
        for (Module::const_iterator it = ModuleMap.begin(); it != ModuleMap.end(); ++it)
        {
            if (it->second.Name == moduleName)
            {
                targetModuleID = it->first;
                moduleFound = true;
                break;
            }
        }

        // If the module name does not exist, return an empty list
        if (!moduleFound)
        {
            PutLog(FATAL, "%s() ModuleName [%s] not found", __FUNCTION__, moduleName);
            return serverBodyIDs;
        }

        // Step 2: Collect all ServerBody IDs that match the target ModuleID
        for (ServerBody::const_iterator it = ServerBodyMap.begin(); it != ServerBodyMap.end(); ++it)
        {
            if (it->second.ModuleID == targetModuleID)
                serverBodyIDs.push_back(it->second.ID);
        }

        return serverBodyIDs;
    }

    void IniDbConfig(WORD wServerBodyID, sDbConfigs& dbConn, const std::string& uid, const std::string& pwd) const
    {
        dbConn.ACCOUNT.clear();
        dbConn.SHARD.clear();
        dbConn.LOG.clear();

        // Step 1: Find the ServerBody entry using the given ID
        ServerBody::const_iterator bodyIt = ServerBodyMap.find(wServerBodyID);
        if (bodyIt == ServerBodyMap.end())
        {
            PutLog(FATAL, "%s() ServerBodyID [%d] not found", __FUNCTION__, wServerBodyID);
            return; // ServerBodyID not found
        }

        const sServerBody& serverBody = bodyIt->second;

        // Step 2: Retrieve ACCOUNT config from DivisionMap using DivisionID (if present)
        if (serverBody.DivisionID != 0)
        {
            Division::const_iterator divIt = DivisionMap.find(serverBody.DivisionID);
            if (divIt != DivisionMap.end())
            {
                dbConn.ACCOUNT = divIt->second.DbConfigAccount;
                stdext::replacesqlinfo(dbConn.ACCOUNT, uid.c_str(), pwd.c_str());
            }
        }

        // Step 3: Retrieve SHARD and LOG configs only if ShardID is valid (not NULL/0)[cite: 1]
        if (serverBody.ShardID != 0)
        {
            Shard::const_iterator shardIt = ShardMap.find(serverBody.ShardID);
            if (shardIt != ShardMap.end())
            {
                dbConn.SHARD = shardIt->second.DbConfigShard;
                stdext::replacesqlinfo(dbConn.SHARD, uid.c_str(), pwd.c_str());

                dbConn.LOG = shardIt->second.DbConfigShard;
                stdext::replacesqlinfo(dbConn.LOG, uid.c_str(), pwd.c_str());
            }
        }
    }
};

#endif //SERVERINFO_H