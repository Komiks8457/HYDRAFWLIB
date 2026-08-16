#include "NetEngine.h"
#include "MemoryUtility.h"

CNetEngine* g_pNetEngine = NULL;
uintptr_t dwNetEngineEntry = 0;

bool CNetEngine::Initialize(uintptr_t ptr)
{
    if (ptr == NULL)
        return false;

    if (g_pNetEngine)
        return true;

    g_pNetEngine = reinterpret_cast<CNetEngine*>(ptr);

    // Safely extract the VFTABLE pointer (vptr) from the object instance
    dwNetEngineEntry = *reinterpret_cast<uintptr_t*>(g_pNetEngine);

    //MEMUTIL_VFTABLE_HOOK(dwNetEngineEntry, 22, &CNetEngine::sub_C0B610);

    return true;
}

CMsg* CNetEngine::NewMsg_IMPL(WORD wMsgID, bool IsEncrypted)
{
    DWORD dwVFTableAddress = MEMUTIL_VFTABLE_GET_FUNC(dwNetEngineEntry, 18);
    CMsg* pMsg = reinterpret_stdcall(dwVFTableAddress, CMsg*, this, IsEncrypted);

    if (pMsg) pMsg->SetMsgID(wMsgID);

    return pMsg;
}

CMsg* CNetEngine::NewMsgFrom_IMPL(WORD wMsgID, const char* szFile, BOOL nLine, const char* szFnName, bool IsEncrypted)
{
    DWORD dwVFTableAddress = MEMUTIL_VFTABLE_GET_FUNC(dwNetEngineEntry, 19);
    CMsg* pMsg = reinterpret_stdcall(dwVFTableAddress, CMsg*, this, szFile, nLine, szFnName, IsEncrypted);

    if (!pMsg)
        return NULL;

    pMsg->SetMsgID(wMsgID);
    return pMsg;
}

void CNetEngine::DelMsg_IMPL(CMsg* pMsg)
{
    if (!pMsg)
        return;

    pMsg->ResetPos();

    // Index 20: DelMsg
    DWORD dwVFTableAddress = MEMUTIL_VFTABLE_GET_FUNC(dwNetEngineEntry, 20);
    reinterpret_stdcall(dwVFTableAddress, void, this, pMsg);
}

IBSNet::NERR CNetEngine::SendMsg_IMPL(const DWORD dwSessionID, CMsg* pMsg)
{
    // Index 22: SendMsg
    DWORD dwVFTableAddress = MEMUTIL_VFTABLE_GET_FUNC(dwNetEngineEntry, 22);
    return reinterpret_stdcall(dwVFTableAddress, IBSNet::NERR, this, dwSessionID, pMsg);
}