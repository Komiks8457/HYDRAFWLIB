#pragma once

#include "DbConnection.h"

namespace HydraFramework
{
    class CDbInstance {
    public:
        CDbInstance();
        ~CDbInstance();

        // Link this instance to a specific connection from the pool
        void Initialize(CDbConnection *pDbConn);

        // Standard Non-Query Execution (INSERT, UPDATE, DELETE)
        bool TryExecNonQuery(const char *szQuery, ...);

        // Execute a Stored Procedure without expecting a return set
        bool TryExecSP(const char* szSpName, const std::vector<stra_t>& params);

        // Access to the raw connection if manual ExecuteReader is needed
        CDbConnection* GetConnection() { return m_pDbConnection; }

    private:
        CDbConnection *m_pDbConnection;
        CCriticalSection m_cs;

    private:
        CDbInstance(const CDbInstance&);
        CDbInstance& operator=(const CDbInstance&);
    };
}