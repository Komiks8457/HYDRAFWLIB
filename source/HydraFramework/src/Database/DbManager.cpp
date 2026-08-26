#include "DbManager.h"
#include "AppLogger.h"

namespace HydraFramework
{
    CDbManager::CDbManager() : m_dbMinConn(1), m_dbMaxConn(2), m_IsDbConnected(false)
    {
        //PutLog(FATAL, "HydraFramework::DbMgr() initialized!");
    }

    CDbManager::~CDbManager()
    {
        // C++98 compliant loop
        for (int i = 0; i < MAX_DB; ++i) {
            m_dbPools[i].Shutdown();
        }
    }

    int CDbManager::InitDB(const std::string& ini)
    {
        if (!m_cfg.load(ini))
            return -999;

        char szConnStr[512];
        const char* connStrFormat = "DRIVER={SQL Server};SERVER=%s;UID=%s;PWD=%s;DATABASE=%s;Trusted_Connection=true;";

        // Extraction using stdext::inifile::getvalue
        const std::string &host    = m_cfg.getvalue("Database", "Host", "NULL");
        const std::string &user    = m_cfg.getvalue("Database", "User", "NULL");
        const std::string &pass    = m_cfg.getvalue("Database", "Pass", "NULL");
        const std::string &account = m_cfg.getvalue("Database", "Account", "NULL");
        const std::string &shard   = m_cfg.getvalue("Database", "Shard", "NULL");
        const std::string &log     = m_cfg.getvalue("Database", "Log", "NULL");

        // --- Account DB ---
        if (__stringa(account).contains("NULL")) {
            return -1;
        }

        _snprintf(szConnStr, sizeof(szConnStr), connStrFormat, host.c_str(), user.c_str(), pass.c_str(), account.c_str());
        if (!m_dbPools[ACCOUNT].Initialize(szConnStr, m_dbMaxConn, m_dbMinConn)) {
            m_dbPools[ACCOUNT].Shutdown();
            return -2;
        }

        // --- Shard DB ---
        if (__stringa(shard).contains("NULL")) {
            return -3;
        }

        _snprintf(szConnStr, sizeof(szConnStr), connStrFormat, host.c_str(), user.c_str(), pass.c_str(), shard.c_str());
        if (!m_dbPools[SHARD].Initialize(szConnStr, m_dbMaxConn, m_dbMinConn)) {
            m_dbPools[SHARD].Shutdown();
            return -4;
        }

        // --- Log DB ---
        if (__stringa(log).contains("NULL")) {
            return -5;
        }

        _snprintf(szConnStr, sizeof(szConnStr), connStrFormat, host.c_str(), user.c_str(), pass.c_str(), log.c_str());
        if (!m_dbPools[LOG].Initialize(szConnStr, m_dbMaxConn, m_dbMinConn)) {
            m_dbPools[LOG].Shutdown();
            return -6;
        }

        m_dbInstancePool[ACCOUNT].Initialize(m_dbPools[ACCOUNT].GetConnection());
        m_dbInstancePool[SHARD].Initialize(m_dbPools[SHARD].GetConnection());
        m_dbInstancePool[LOG].Initialize(m_dbPools[LOG].GetConnection());

        m_IsDbConnected = true;
        return TRUE;
    }

    int CDbManager::InitDB(const sDbConfigs& dbInfo)
    {
        if (dbInfo.ACCOUNT.length() > 0) {
            if (m_dbPools[ACCOUNT].Initialize(dbInfo.ACCOUNT, m_dbMaxConn, m_dbMinConn)) {
                m_dbInstancePool[ACCOUNT].Initialize(m_dbPools[ACCOUNT].GetConnection());
            } else {
                m_dbPools[ACCOUNT].Shutdown();
                return -2;
            }
        }

        if (dbInfo.SHARD.length() > 0) {
            if (m_dbPools[SHARD].Initialize(dbInfo.SHARD, m_dbMaxConn, m_dbMinConn)) {
                m_dbInstancePool[SHARD].Initialize(m_dbPools[SHARD].GetConnection());
            } else {
                m_dbPools[SHARD].Shutdown();
                return -4;
            }
        }

        if (dbInfo.LOG.length() > 0) {
            if (m_dbPools[LOG].Initialize(dbInfo.LOG, m_dbMaxConn, m_dbMinConn)) {
                m_dbInstancePool[LOG].Initialize(m_dbPools[LOG].GetConnection());
            } else {
                m_dbPools[LOG].Shutdown();
                return -6;
            }
        }

        m_IsDbConnected = true;
        return TRUE;
    }

    CDbInstance& CDbManager::GetInstance(DbType type)
    {
        ACS_SCOPED_LOCK(m_cs);
        return m_dbInstancePool[type];
    }

    CDbConnection* CDbManager::GetConnection(DbType type)
    {
        ACS_SCOPED_LOCK(m_cs);
        return m_dbPools[type].GetConnection();
    }

    void CDbManager::Release(DbType type, CDbConnection *pDbConn)
    {
        ACS_SCOPED_LOCK(m_cs);
        m_dbPools[type].ReleaseConnection(pDbConn);
    }

    void CDbManager::ExecNonQuery(DbType type, const char* query, ...)
    {
        ACS_SCOPED_LOCK(m_cs);

        char format[1024];
        va_list args;
        va_start(args, query);
        _vsnprintf(format, sizeof(format) - 1, query, args);
        format[sizeof(format) - 1] = '\0';
        va_end(args);

        // FIX: Pass format as a literal/raw string parameter to avoid double-formatting crash
        if (!m_dbInstancePool[type].TryExecNonQuery("%s", format))
            PutLog(FATAL, "Failed to execute: %s", format);
    }
}