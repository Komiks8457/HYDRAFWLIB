#include "NetEngine.h"

#include "AppLogger.h"
#include "MemoryUtility.h"
#include "Logger/Logger.h"

CNetEngine* g_pNetEngine = NULL;

CNetEngine::vfTableMap CNetEngine::m_vftableMap;

bool CNetEngine::Initialize(void* ptr)
{
    if (ptr == NULL || g_pNetEngine)
        return false;

    g_pNetEngine = reinterpret_cast<CNetEngine*>(ptr);

    for (int i = 0; i <= 43; ++i) {
        DWORD vftFunc = MEMUTIL_VFTABLE_GET_FUNC(*(uintptr_t*)g_pNetEngine, i);
        m_vftableMap[i] = vftFunc;
    }

    PutLog(WARNING, "CNetEngine Hook Initialized!");
    return true;
}

CMsg* CNetEngine::NewMsg(WORD wMsgID, bool IsEncrypted)
{
    CMsg* pMsg = reinterpret_stdcall(m_vftableMap[18], CMsg*, this, IsEncrypted);

    if (pMsg == NULL) {
        Logger::error("%s(...), pMsg is NULL", __FUNCTION__);
        return NULL;
    }

    pMsg->SetMsgID(wMsgID);

    return pMsg;
}

CMsg* CNetEngine::NewMsgFrom(WORD wMsgID, const char* szFile, BOOL nLine, const char* szFnName, bool IsEncrypted)
{
    CMsg* pMsg = reinterpret_stdcall(m_vftableMap[19], CMsg*, this, szFile, nLine, szFnName, IsEncrypted);

    if (pMsg == NULL) {
        Logger::error("%s(...), pMsg is NULL", __FUNCTION__);
        return NULL;
    }

    pMsg->SetMsgID(wMsgID);
    return pMsg;
}

void CNetEngine::DelMsg(CMsg* pMsg)
{
    if (!pMsg) return;

    pMsg->ResetPos();

    reinterpret_stdcall(m_vftableMap[20], void, this, pMsg);

    DelMsg(pMsg);
}

IBSNet::NERR CNetEngine::SendMsg(const DWORD dwSessionID, CMsg* pMsg)
{
    return reinterpret_stdcall(m_vftableMap[22], IBSNet::NERR, this, dwSessionID, pMsg);
}

MacAddress CNetEngine::GetMacAddress(DWORD dwSession) const
{
    BYTE mac[6] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xF0, 0x0D };

    if (!reinterpret_stdcall(m_vftableMap[36], BOOL, this, dwSession, mac)) {
        PutLog(FATAL, "%s failed.", __FUNCTIONP__);
    }

    return MacAddress(mac);
}
