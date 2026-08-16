#include "AppLogger.h"
#include "MemoryUtility.h"
#include "Logger/Logger.h"
#include "pch.h"

CAppLogger::pfnLogWriter CAppLogger::m_pfnLogWriter = NULL;
bool CAppLogger::m_Initialized = FALSE;
bool CAppLogger::m_OutputLogs = FALSE;

void CAppLogger::SetupHook(unsigned int dwAddr)
{
    m_pfnLogWriter = reinterpret_cast<pfnLogWriter>(dwAddr);
    MEMUTIL_HOOK_FN(m_pfnLogWriter, addr_from_this(&CAppLogger::WriteLog))
}

char CAppLogger::WriteLog(E_LOG_MSG_TYPE Type, const char* format, ...)
{
    char logmsg[8192];
    va_list ArgList;
    va_start(ArgList, format);
    _vsnprintf(logmsg, 8191, format, ArgList);
    va_end(ArgList);

    if (__stringa(logmsg).endswith("was initialized successfully") && !m_Initialized)
        m_Initialized = TRUE;

    if (m_OutputLogs)
    {
        switch (Type) {
            case NORMAL:
                Logger::info(logmsg);
            break;
            case FATAL:
                Logger::error(logmsg);
            break;
            case WARNING:
                Logger::warn(logmsg);
            default: ;
        }
    }

    return m_pfnLogWriter(Type, logmsg);
}