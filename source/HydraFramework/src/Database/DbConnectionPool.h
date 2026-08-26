#pragma once

#include "DbConnection.h"
#include <vector>
#include <string>

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
        void Shutdown_NoLock();

    private:
        // Internal helper to create a single connection
        CDbConnection* CreateNewConnection();

        std::string m_strConnStr;
        int m_nMaxConnections;
        int m_nMinConnections;
        int m_nCurrentTotal; // Tracks total connections (Idle + Active)

        std::vector<CDbConnection*> m_idlePool;
        CCriticalSection m_cs;

        // Non-copyable
        CDbConnectionPool(const CDbConnectionPool&);
        CDbConnectionPool& operator=(const CDbConnectionPool&);
    };

    // RAII Guard to automatically release connections and prevent leaks in consumer classes
    class CDbConnectionGuard {
    public:
        explicit CDbConnectionGuard(CDbConnectionPool& pool)
            : m_pool(pool), m_pConn(pool.GetConnection()) {}

        ~CDbConnectionGuard() {
            if (m_pConn) {
                m_pool.ReleaseConnection(m_pConn);
            }
        }

        CDbConnection* operator->() const { return m_pConn; }
        CDbConnection* Get() const { return m_pConn; }
        bool IsValid() const { return m_pConn != NULL; }

    private:
        CDbConnectionPool& m_pool;
        CDbConnection* m_pConn;

        CDbConnectionGuard(const CDbConnectionGuard&);
        CDbConnectionGuard& operator=(const CDbConnectionGuard&);
    };
}