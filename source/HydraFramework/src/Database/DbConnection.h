#pragma once

#include "pch.h"
#include <sql.h>
#include <sqlext.h>
#include <sqlucode.h>
#include <sqltypes.h>
#include "CriticalSection.h"

#pragma comment(lib, "odbc32.lib")

namespace HydraFramework
{
    class CDbRowReader;
    class CDbConnection {
    public:
        CDbConnection(const std::string &strConnStr);
        virtual ~CDbConnection();

        // Connection Management
        bool Connect();
        void Disconnect();

        // Query Execution
        bool ExecuteReader(CDbRowReader &reader, const char *szQueryFormat, ...);

        // Stored Procedure Execution (Optimized with Parameter Binding)
        bool ExecuteSP(const char* szSpName, const std::vector<std::string>& params);

        // Handle Management
        bool AllocStmt(SQLHANDLE &hStmt) const;
        bool FreeStmt(SQLHANDLE &hStmt);

        // Getters
        SQLHANDLE GetEnvHandle() const { return m_hEnv; }
        SQLHANDLE GetConnHandle() const { return m_hConn; }
        const std::string& GetConnString() const { return m_strConnStr; }

    private:
        std::string m_strConnStr;
        SQLHANDLE m_hEnv;
        SQLHANDLE m_hConn;

    private:
        CDbConnection(const CDbConnection&);
        CDbConnection& operator=(const CDbConnection&);
    };
}
