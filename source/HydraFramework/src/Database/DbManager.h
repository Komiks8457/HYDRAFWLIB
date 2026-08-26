#pragma once

#include "DbInstance.h"
#include "DbConnectionPool.h"
#include "CriticalSection.h"

namespace HydraFramework
{
    enum DbType {
        ACCOUNT,
        SHARD,
        LOG,
        MAX_DB
    };

    class CDbManager {
        friend class CHydraProcess;
    public:
        int InitDB(const std::string& ini);
        int InitDB(const sDbConfigs& dbInfo);

        // Original style accessors
        CDbInstance& GetInstance(DbType type);
        CDbConnection* GetConnection(DbType type);
        void Release(DbType type, CDbConnection *pDbConn);

        // Optimized shorthand for logic threads
        CDbInstance& GetAccountDB() { return m_dbInstancePool[ACCOUNT]; }
        CDbInstance& GetShardDB()   { return m_dbInstancePool[SHARD]; }
        CDbInstance& GetLogDB()     { return m_dbInstancePool[LOG]; }

        void ExecNonQuery(DbType type, const char* query, ...);

        bool IsDbConnected() { return m_IsDbConnected; }
    private:
        CDbManager();
        CDbManager(const CDbManager&);
        CDbManager& operator=(const CDbManager&);
        ~CDbManager();

        __inifile m_cfg; // Maps to stdext::inifile[cite: 2]

        CDbConnectionPool m_dbPools[MAX_DB];
        CDbInstance       m_dbInstancePool[MAX_DB];
        CCriticalSection  m_cs;

        BYTE m_dbMinConn;
        BYTE m_dbMaxConn;

        bool m_IsDbConnected;
    };
}
