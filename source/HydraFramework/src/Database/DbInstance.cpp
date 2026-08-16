#include "DbInstance.h"
#include "CriticalSection.h"
#include <stdarg.h>

namespace HydraFramework
{
    CDbInstance::CDbInstance() : m_pDbConnection(NULL)
    {}

    CDbInstance::~CDbInstance()
    {
        m_pDbConnection = NULL;
    }

    void CDbInstance::Initialize(CDbConnection *pDbConn)
    {
        ACS_SCOPED_LOCK(m_cs);
        m_pDbConnection = pDbConn;
    }

    bool CDbInstance::TryExecNonQuery(const char *szQuery, ...)
    {
        // 1. Thread Safety: Ensure only one thread uses this connection at a time
        ACS_SCOPED_LOCK(m_cs);

        if (!m_pDbConnection) return false;

        // 2. Safe String Formatting
        va_list args;
        va_start(args, szQuery);
        char szBuffer[4096]; // Sufficient for most SRO queries
        int nLen = _vsnprintf(szBuffer, sizeof(szBuffer) - 1, szQuery, args);
        va_end(args);

        if (nLen <= 0) return false;

        // 3. Statement Management
        SQLHANDLE hStmt = SQL_NULL_HSTMT;
        if (!m_pDbConnection->AllocStmt(hStmt))
        {
            return false;
        }

        SQLRETURN retCode = SQLExecDirectA(hStmt, (SQLCHAR *) szBuffer, SQL_NTS);

        // Always free the statement immediately for Non-Queries
        m_pDbConnection->FreeStmt(hStmt);

        return SQL_SUCCEEDED(retCode);
    }

    bool CDbInstance::TryExecSP(const char* szSpName, const std::vector<stra_t>& params)
    {
        ACS_SCOPED_LOCK(m_cs);
        if (!m_pDbConnection) return false;
        return m_pDbConnection->ExecuteSP(szSpName, params);
    }
}