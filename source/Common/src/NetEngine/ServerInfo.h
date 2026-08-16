#pragma once

#ifndef SERVERINFO_H
#define SERVERINFO_H

#include "pch.h"
#include "Msg.h"

struct sServerBody
{
    WORD ID;
    WORD DivisionID;
    WORD ShardID;
    std::string Name;

    sServerBody() : ID(0), DivisionID(0), ShardID(0), Name("")
    {}

    void Serialize(CMsg* pMsg) const
    {
        pMsg->Write(ID);
        pMsg->Write(DivisionID);
        pMsg->Write(ShardID);
        pMsg->Write(Name);
    }

    void Deserialize(CMsg* pMsg)
    {
        pMsg->Read(ID);
        pMsg->Read(DivisionID);
        pMsg->Read(ShardID);
        pMsg->Read(Name);
    }
};

typedef std::map<BYTE, sServerBody> ServerBody;

struct sDivision
{
    BYTE ID;
    std::string Name;
    std::string DbConfigAccount;

    sDivision() : ID(0), Name(""), DbConfigAccount("")
    {}

    void Serialize(CMsg* pMsg) const
    {
        pMsg->Write(ID);
        pMsg->Write(Name);
        pMsg->Write(DbConfigAccount);
    }

    void Deserialize(CMsg* pMsg)
    {
        pMsg->Read(ID);
        pMsg->Read(Name);
        pMsg->Read(DbConfigAccount);
    }
};

typedef std::map<BYTE, sDivision> Division;

struct sShard
{
    WORD ID;
    std::string Name;
    std::string DbConfigShard;
    std::string DbConfigLog;

    sShard() : ID(0), Name(""), DbConfigLog("")
    {}

    void Serialize(CMsg* pMsg) const
    {
        pMsg->Write(ID);
        pMsg->Write(Name);
        pMsg->Write(DbConfigShard);
        pMsg->Write(DbConfigLog);
    }

    void Deserialize(CMsg* pMsg)
    {
        pMsg->Read(ID);
        pMsg->Read(Name);
        pMsg->Read(DbConfigShard);
        pMsg->Read(DbConfigLog);
    }
};

typedef std::map<WORD, sShard> Shard;

struct sServerInfo
{
    ServerBody ServerBodyMap; // Renamed to avoid name collision with type/member scoping
    Division DivisionMap;
    Shard ShardMap;

    void Serialize(CMsg* pMsg) const
    {
        // Serialize ServerBody map
        WORD bodySize = static_cast<WORD>(ServerBodyMap.size());
        pMsg->Write(bodySize);
        ServerBody::const_iterator itBody = ServerBodyMap.begin();
        ServerBody::const_iterator endBody = ServerBodyMap.end();
        for (; itBody != endBody; ++itBody)
        {
            pMsg->Write(itBody->first);
            itBody->second.Serialize(pMsg);
        }

        // Serialize DivisionMap map
        WORD divSize = static_cast<WORD>(DivisionMap.size());
        pMsg->Write(divSize);
        Division::const_iterator itDiv = DivisionMap.begin();
        Division::const_iterator endDiv = DivisionMap.end();
        for (; itDiv != endDiv; ++itDiv)
        {
            pMsg->Write(itDiv->first);
            itDiv->second.Serialize(pMsg);
        }

        // Serialize ShardMap map
        WORD shardSize = static_cast<WORD>(ShardMap.size());
        pMsg->Write(shardSize);
        Shard::const_iterator itShard = ShardMap.begin();
        Shard::const_iterator endShard = ShardMap.end();
        for (; itShard != endShard; ++itShard)
        {
            pMsg->Write(itShard->first);
            itShard->second.Serialize(pMsg);
        }
    }

    void Deserialize(CMsg* pMsg)
    {
        ServerBodyMap.clear();
        DivisionMap.clear();
        ShardMap.clear();

        // Deserialize ServerBody map
        WORD bodySize = 0;
        pMsg->Read(bodySize);
        for (WORD i = 0; i < bodySize; ++i)
        {
            BYTE key = 0;
            pMsg->Read(key);
            sServerBody body;
            body.Deserialize(pMsg);
            ServerBodyMap[key] = body;
        }

        // Deserialize DivisionMap map
        WORD divSize = 0;
        pMsg->Read(divSize);
        for (WORD i = 0; i < divSize; ++i)
        {
            BYTE key = 0;
            pMsg->Read(key);
            sDivision div;
            div.Deserialize(pMsg);
            DivisionMap[key] = div;
        }

        // Deserialize ShardMap map
        WORD shardSize = 0;
        pMsg->Read(shardSize);
        for (WORD i = 0; i < shardSize; ++i)
        {
            WORD key = 0;
            pMsg->Read(key);
            sShard shard;
            shard.Deserialize(pMsg);
            ShardMap[key] = shard;
        }
    }
};

typedef std::map<WORD, sServerInfo> ServerInfo;

#endif //SERVERINFO_H