#include "NetEngine.h"

#include "AppLogger.h"
#include "MemoryUtility.h"
#include "Logger/Logger.h"

CNetEngine* g_pNetEngine = NULL;

CNetEngine::vfTableMap CNetEngine::m_vftableMap;

bool CNetEngine::Initialize(CNetEngine* pNetEngine)
{
    if (pNetEngine == NULL)
        return false;

    if (g_pNetEngine)
        return true;

    g_pNetEngine = pNetEngine;

    for (int i = 0; i <= 43; ++i) {
        DWORD vftFunc = MEMUTIL_VFTABLE_GET_FUNC(*(uintptr_t*)g_pNetEngine, i);
        m_vftableMap[i] = vftFunc;
    }

    return true;
}

CMsg* CNetEngine::NewMsg_IMPL(WORD wMsgID, bool IsEncrypted)
{
    CMsg* pMsg = reinterpret_stdcall(m_vftableMap[18], CMsg*, IsEncrypted);

    if (pMsg == NULL) {
        Logger::error("%s(...), pMsg is NULL", __FUNCTION__);
        return NULL;
    }

    pMsg->SetMsgID(wMsgID);

    return pMsg;
}

CMsg* CNetEngine::NewMsgFrom_IMPL(WORD wMsgID, const char* szFile, BOOL nLine, const char* szFnName, bool IsEncrypted)
{
    CMsg* pMsg = reinterpret_stdcall(m_vftableMap[19], CMsg*, this, szFile, nLine, szFnName, IsEncrypted);

    if (pMsg == NULL) {
        Logger::error("%s(...), pMsg is NULL", __FUNCTION__);
        return NULL;
    }

    pMsg->SetMsgID(wMsgID);
    return pMsg;
}

void CNetEngine::DelMsg_IMPL(CMsg* pMsg)
{
    if (!pMsg) return;

    pMsg->ResetPos();

    reinterpret_stdcall(m_vftableMap[20], void, this, pMsg);

    DelMsg(pMsg);
}

IBSNet::NERR CNetEngine::SendMsg_IMPL(const DWORD dwSessionID, CMsg* pMsg)
{
    return reinterpret_stdcall(m_vftableMap[22], IBSNet::NERR, this, dwSessionID, pMsg);
}

BOOL CNetEngine::GetMacAddress_IMPL(DWORD dwSession, BYTE* pAddress)
{
    return reinterpret_stdcall(m_vftableMap[36], BOOL, this, dwSession, pAddress);
}

BOOL CNetEngine::DisplayVersion_IMPL()
{
    return reinterpret_stdcall(m_vftableMap[38], BOOL, this);
}
