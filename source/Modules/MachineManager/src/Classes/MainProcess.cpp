#include "MainProcess.h"
#include "MemoryUtility.h"
#include "NetEngine/Msg.h"

namespace MachineManager
{
    void CMainProcess::SetupHook()
    {
        MEMUTIL_VFTABLE_HOOK(0x015650E4, 9, &CMainProcess::HandleMsg);
    }

    uint32_t CMainProcess::HandleMsg(CMsg *pMsg, DWORD dwOverlappedID, void *lpParam, CMassiveMsg *pMassiveMsg)
    {
        return reinterpret_thiscall(0x01491B40, uint32_t, this, pMsg, dwOverlappedID, lpParam, pMassiveMsg);
    }

}
