#pragma once

#include "DbConnection.h"

namespace HydraFramework
{
    class CDbConnectionPool {
    public:
        CDbConnectionPool();
        virtual ~CDbConnectionPool();

        // Initialize the pool with a specific connection string and size limits
        bool Initialize(const std::string& strConn, int nMaxConn, int nMinConn);

        // Get an idle connection or create a new one if within limits
        CDbConnection* GetConnection();

        // Return a connection to the pool for reuse
        void ReleaseConnection(CDbConnection* pConn);

        // Shutdown and cleanup all connections
        void Shutdown();

    private:
        // Internal helper to create a single connection
        CDbConnection* CreateNewConnection();

        std::string m_strConnStr;
        int m_nMaxConnections;
        int m_nMinConnections;
        int m_nCurrentTotal; // Tracks total connections (Idle + Active)

        std::vector<CDbConnection*> m_idlePool;
        CCriticalSection m_cs;

    private:
        CDbConnectionPool(const CDbConnectionPool&);
        CDbConnectionPool& operator=(const CDbConnectionPool&);
    };
}