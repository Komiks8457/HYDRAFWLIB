#include "DbConnection.h"
#include "DbRowReader.h"
#include <stdarg.h>

namespace HydraFramework
{
    CDbConnection::CDbConnection(const std::string &strConnStr)
            : m_strConnStr(strConnStr), m_hEnv(SQL_NULL_HENV), m_hConn(SQL_NULL_HDBC)
    {}

    CDbConnection::~CDbConnection()
    {
        Disconnect();
    }

    bool CDbConnection::Connect()
    {
        SQLRETURN retCode;

        // Allocate Environment
        retCode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_hEnv);
        if (!SQL_SUCCEEDED(retCode)) return false;

        // Set ODBC Version
        SQLSetEnvAttr(m_hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);

        // Allocate Connection Handle
        retCode = SQLAllocHandle(SQL_HANDLE_DBC, m_hEnv, &m_hConn);
        if (!SQL_SUCCEEDED(retCode)) {
            Disconnect();
            return false;
        }

        // Connect to Driver
        SQLCHAR retConnStr[1024] = {0};
        SQLSMALLINT nOutLen;
        retCode = SQLDriverConnectA(m_hConn, NULL, (SQLCHAR*)m_strConnStr.c_str(), SQL_NTS,
                                    retConnStr, sizeof(retConnStr), &nOutLen, SQL_DRIVER_NOPROMPT);

        if (!SQL_SUCCEEDED(retCode)) {
            Disconnect();
            return false;
        }

        return true;
    }

    void CDbConnection::Disconnect()
    {
        if (m_hConn != SQL_NULL_HDBC) {
            SQLDisconnect(m_hConn);
            SQLFreeHandle(SQL_HANDLE_DBC, m_hConn);
            m_hConn = SQL_NULL_HDBC;
        }

        if (m_hEnv != SQL_NULL_HENV) {
            SQLFreeHandle(SQL_HANDLE_ENV, m_hEnv);
            m_hEnv = SQL_NULL_HENV;
        }
    }

    bool CDbConnection::AllocStmt(SQLHANDLE &hStmt) const
    {
        if (m_hConn == SQL_NULL_HDBC) return false;

        SQLRETURN retCode = SQLAllocHandle(SQL_HANDLE_STMT, m_hConn, &hStmt);
        return SQL_SUCCEEDED(retCode);
    }

    bool CDbConnection::FreeStmt(SQLHANDLE &hStmt)
    {
        if (hStmt == SQL_NULL_HSTMT) return true;

        // Safety: Unbind pointers before freeing to avoid memory corruption
        ::SQLFreeStmt(hStmt, SQL_UNBIND);
        ::SQLFreeStmt(hStmt, SQL_RESET_PARAMS);
        ::SQLFreeStmt(hStmt, SQL_CLOSE);

        SQLRETURN retCode = ::SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        hStmt = SQL_NULL_HSTMT; // Clear reference

        return SQL_SUCCEEDED(retCode);
    }

    bool CDbConnection::ExecuteReader(CDbRowReader &reader, const char *szQueryFormat, ...)
    {
        va_list args;
        va_start(args, szQueryFormat);
        char szQuery[0xFFFF]; // 64KB stack buffer
        int nLen = _vsnprintf(szQuery, sizeof(szQuery) - 1, szQueryFormat, args);
        va_end(args);

        if (nLen <= 0) return false;

        SQLHANDLE hStmt = SQL_NULL_HSTMT;
        if (!AllocStmt(hStmt)) return false;

        SQLRETURN retCode = SQLExecDirectA(hStmt, (SQLCHAR*)szQuery, SQL_NTS);

        if (!SQL_SUCCEEDED(retCode) && retCode != SQL_NULL_DATA) {
            FreeStmt(hStmt);
            return false;
        }

        // Fix for C2248: We use Attach() instead of operator=
        reader.Attach(hStmt);
        return true;
    }

    bool CDbConnection::ExecuteSP(const char* szSpName, const std::vector<std::string>& params)
    {
        SQLHANDLE hStmt = SQL_NULL_HSTMT;
        if (!AllocStmt(hStmt)) return false;

        // Build CALL string: { CALL ProcedureName (?, ?, ?) }
        std::string strCall = "{ CALL " + std::string(szSpName) + " (";
        for (size_t i = 0; i < params.size(); ++i) {
            strCall += (i == 0) ? "?" : ", ?";
        }
        strCall += ") }";

        if (!SQL_SUCCEEDED(SQLPrepareA(hStmt, (SQLCHAR*)strCall.c_str(), SQL_NTS))) {
            FreeStmt(hStmt);
            return false;
        }

        // Indicator vectors to keep data alive during execution
        std::vector<SQLLEN> indicators(params.size());

        for (size_t i = 0; i < params.size(); ++i) {
            indicators[i] = params[i].length();
            SQLBindParameter(hStmt, (SQLUSMALLINT)(i + 1), SQL_PARAM_INPUT,
                             SQL_C_CHAR, SQL_CHAR, params[i].length(), 0,
                             (SQLPOINTER)params[i].c_str(), 0, &indicators[i]);
        }

        SQLRETURN ret = SQLExecute(hStmt);
        FreeStmt(hStmt);

        return SQL_SUCCEEDED(ret);
    }
}