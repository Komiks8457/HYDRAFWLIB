#pragma once

#include "DbInstance.h"
#include "CriticalSection.h"

namespace HydraFramework
{
    class CDbRowReader {
    public:
        CDbRowReader() : m_hStmt(SQL_NULL_HSTMT), m_nColID(1), m_bIsClosed(true) {}
        CDbRowReader(SQLHANDLE hStmt) : m_hStmt(hStmt), m_nColID(1), m_bIsClosed(false) {}
        ~CDbRowReader() { Close(); }

        void Attach(SQLHANDLE hStmt) {
            ACS_SCOPED_LOCK(m_cs);
            if (m_hStmt != SQL_NULL_HSTMT && !m_bIsClosed) Close();
            m_hStmt = hStmt;
            m_nColID = 1;
            m_bIsClosed = (hStmt == SQL_NULL_HSTMT);
        }

        void Close() {
            ACS_SCOPED_LOCK(m_cs);
            if (!m_bIsClosed && m_hStmt != SQL_NULL_HSTMT) {
                ::SQLFreeStmt(m_hStmt, SQL_CLOSE);
                m_bIsClosed = true;
            }
        }

        bool FetchNext() {
            ACS_SCOPED_LOCK(m_cs);
            m_nColID = 1;
            SQLRETURN ret = ::SQLFetch(m_hStmt);
            return SQL_SUCCEEDED(ret);
        }

        template<typename T>
        T Read() {
            ACS_SCOPED_LOCK(m_cs);
            T result = T();
            SQLLEN cb = 0;
            // BufferLength MUST be sizeof(T)
            ::SQLGetData(m_hStmt, m_nColID++, GetColType<T>(), &result,
                         sizeof(T), &cb);
            return (cb == SQL_NULL_DATA) ? T() : result;
        }

        std::vector<BYTE> ReadBinary(int nDataLen);
        void ReadString(std::string& str, int nMaxLen);
        void ReadString(char *pDest, int nMaxLen);
        SimpleTime ReadDateTime();

    private:
        template<typename T>
        static SQLSMALLINT GetColType()
        {
            // 1-byte integers (TINYINT)
            if (typeid(T) == typeid(unsigned char) || typeid(T) == typeid(unsigned __int8))
                return SQL_C_UTINYINT;
            if (typeid(T) == typeid(char) || typeid(T) == typeid(__int8))
                return SQL_C_STINYINT;

            // 2-byte integers (SMALLINT)
            if (typeid(T) == typeid(unsigned short))
                return SQL_C_USHORT;
            if (typeid(T) == typeid(short))
                return SQL_C_SSHORT;

            // 4-byte integers (INT / LONG)
            if (typeid(T) == typeid(unsigned int) || typeid(T) == typeid(unsigned __int32))
                return SQL_C_ULONG;
            if (typeid(T) == typeid(int) || typeid(T) == typeid(__int32))
                return SQL_C_SLONG;

            // 8-byte integers (BIGINT)
            if (typeid(T) == typeid(unsigned __int64))
                return SQL_C_UBIGINT;
            if (typeid(T) == typeid(long long) || typeid(T) == typeid(__int64))
                return SQL_C_SBIGINT;

            // Floating point
            if (typeid(T) == typeid(float))
                return SQL_C_FLOAT;
            if (typeid(T) == typeid(double))
                return SQL_C_DOUBLE;

            // Boolean
            if (typeid(T) == typeid(bool))
                return SQL_C_BIT;

            // Default fallback
            return SQL_C_DEFAULT;
        }

        SQLHANDLE m_hStmt;
        int m_nColID;
        bool m_bIsClosed;
        CCriticalSection m_cs;

    private:
        CDbRowReader(const CDbRowReader&);
        CDbRowReader& operator=(const CDbRowReader&);
    };
}