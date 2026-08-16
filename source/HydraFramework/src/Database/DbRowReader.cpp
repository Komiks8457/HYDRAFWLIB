#include "DbRowReader.h"

namespace HydraFramework
{
    void CDbRowReader::ReadString(std::string& str, int nMaxLen)
    {
        if (nMaxLen <= 0) return;
        ACS_SCOPED_LOCK(m_cs);
        SQLLEN cb;
        str.resize(nMaxLen);
        SQLRETURN ret = ::SQLGetData(m_hStmt, m_nColID++, SQL_C_CHAR,
                                     &str[0], nMaxLen + 1, &cb);
        if (cb == SQL_NULL_DATA || ret == SQL_ERROR)
        {
            str.clear();
            return;
        }
        if (cb >= 0 && cb < nMaxLen)
            str.resize(cb);
    }

    void CDbRowReader::ReadString(char* pDest, int nMaxLen)
    {
        if (!pDest || nMaxLen <= 0) return;
        ACS_SCOPED_LOCK(m_cs);
        SQLLEN cb;
        SQLRETURN ret = ::SQLGetData(m_hStmt, m_nColID++, SQL_C_CHAR,
                                     pDest, nMaxLen, &cb);
        if (cb == SQL_NULL_DATA || ret == SQL_ERROR)
        {
            pDest[0] = '\0';
            return;
        }
        pDest[nMaxLen - 1] = '\0';
    }

    std::vector<BYTE> CDbRowReader::ReadBinary(int nDataLen)
    {
        ACS_SCOPED_LOCK(m_cs);
        std::vector<BYTE> result(nDataLen);
        SQLLEN cb;
        ::SQLGetData(m_hStmt, m_nColID++, SQL_C_BINARY, &result[0],
                     nDataLen, &cb);
        if (cb == SQL_NULL_DATA) result.clear();
        return result;
    }

    SimpleTime CDbRowReader::ReadDateTime()
    {
        ACS_SCOPED_LOCK(m_cs);
        SQL_TIMESTAMP_STRUCT ts;
        SQLLEN cb;
        ::SQLGetData(m_hStmt, m_nColID++, SQL_C_TYPE_TIMESTAMP, &ts,
                     0, &cb);
        SimpleTime st;
        if (cb != SQL_NULL_DATA) {
            st.year = ts.year; st.month = ts.month; st.day = ts.day;
            st.hour = ts.hour; st.min = ts.minute; st.sec = ts.second;
        }
        return st;
    }
}