#include "DbConnectionPool.h"
#include "CriticalSection.h"

namespace HydraFramework
{
    CDbConnectionPool::CDbConnectionPool() : m_nMaxConnections(0), m_nMinConnections(0), m_nCurrentTotal(0)
    {}

    CDbConnectionPool::~CDbConnectionPool()
    {
        Shutdown();
    }

    bool CDbConnectionPool::Initialize(const std::string& strConn, int nMaxConn, int nMinConn)
    {
        ACS_SCOPED_LOCK(m_cs);

        m_strConnStr = strConn;
        m_nMaxConnections = nMaxConn;
        m_nMinConnections = nMinConn;

        // Pre-allocate the minimum number of connections
        for (int i = 0; i < m_nMinConnections; ++i)
        {
            CDbConnection* pConn = CreateNewConnection();
            if (pConn)
            {
                m_idlePool.push_back(pConn);
            }
            else
            {
                // If we can't even start the minimum, initialization fails
                return false;
            }
        }

        return true;
    }

    CDbConnection* CDbConnectionPool::GetConnection()
    {
        ACS_SCOPED_LOCK(m_cs);

        // 1. Try to get an existing idle connection
        while (!m_idlePool.empty())
        {
            CDbConnection* pConn = m_idlePool.back();
            m_idlePool.pop_back();

            // Optional: Add a "Ping" check here if your driver supports it
            // to ensure the connection hasn't timed out on the SQL side.
            if (pConn) return pConn;
        }

        // 2. If no idle connections, check if we can grow the pool
        if (m_nCurrentTotal < m_nMaxConnections)
        {
            return CreateNewConnection();
        }

        // 3. Pool is exhausted and at Max limit
        // In a real server, you might wait here, but for SRO logic, returning NULL
        // allows the caller to log an error or retry.
        return NULL;
    }

    void CDbConnectionPool::ReleaseConnection(CDbConnection* pConn)
    {
        if (!pConn) return;

        ACS_SCOPED_LOCK(m_cs);

        // Put the connection back into the idle pool for reuse
        m_idlePool.push_back(pConn);
    }

    CDbConnection* CDbConnectionPool::CreateNewConnection()
    {
        // Note: We don't lock here because this is called by functions already holding the lock
        CDbConnection* pNewConn = new CDbConnection(m_strConnStr);

        if (pNewConn->Connect())
        {
            m_nCurrentTotal++;
            return pNewConn;
        }

        delete pNewConn;
        return NULL;
    }

    void CDbConnectionPool::Shutdown()
    {
        ACS_SCOPED_LOCK(m_cs);

        // Delete all idle connections
        for (size_t i = 0; i < m_idlePool.size(); ++i)
        {
            if (m_idlePool[i])
            {
                m_idlePool[i]->Disconnect();
                delete m_idlePool[i];
            }
        }

        m_idlePool.clear();
        m_nCurrentTotal = 0;
    }
}