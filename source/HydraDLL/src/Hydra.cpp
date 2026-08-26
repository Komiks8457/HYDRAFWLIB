#include "Library.h"
#include "MemoryUtility.h"
#include "Logger/Logger.h"

HMODULE hMyModule = NULL;
volatile LONG HydraInitState = 0;

typedef LPSTR (WINAPI *PGET_COMMAND_LINE_A)(VOID);
PGET_COMMAND_LINE_A TrueGetCommandLineA = GetCommandLineA;

typedef LPWSTR (WINAPI *PGET_COMMAND_LINE_W)(VOID);
PGET_COMMAND_LINE_W TrueGetCommandLineW = GetCommandLineW;

typedef BOOL (WINAPI *PALLOC_CONSOLE)(VOID);
PALLOC_CONSOLE TrueAllocConsole = AllocConsole;

void TriggerHydraInit()
{
    if (HydraInitState > 0)
        return;

    if (InterlockedCompareExchange(&HydraInitState, 1, 0) == 0)
    {
        HydraInitState = 2;
        CLibrary::DoSomeWork();
    }
}

LPSTR WINAPI MyGetCommandLineA(VOID)
{
    TriggerHydraInit();
    return TrueGetCommandLineA();
}

LPWSTR WINAPI MyGetCommandLineW(VOID)
{
    TriggerHydraInit();
    return TrueGetCommandLineW();
}

NOINLINE BOOL WINAPI MyAllocConsole(VOID)
{
    void* retAddr = _ReturnAddress();
    HMODULE hCallerModule = NULL;

    if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           (LPCSTR)retAddr, &hCallerModule))
    {
        if (hCallerModule == hMyModule)
            return TrueAllocConsole();
    }

    SetLastError(ERROR_ACCESS_DENIED);
    return FALSE;
}

BOOL APIENTRY DllMain(HMODULE hInst, const DWORD ul_reason_for_call, LPVOID lpReserved)
{
    hMyModule = hInst; // assign hMyModule first.

    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(hMyModule);
            MEMUTIL_HOOK_FN(TrueAllocConsole, MyAllocConsole)
            MEMUTIL_HOOK_FN(TrueGetCommandLineA, MyGetCommandLineA)
            MEMUTIL_HOOK_FN(TrueGetCommandLineW, MyGetCommandLineW)
            break;
        case DLL_PROCESS_DETACH:
            if (lpReserved == NULL)
            {
                MEMUTIL_UNHOOK(TrueAllocConsole, MyAllocConsole)
                MEMUTIL_UNHOOK(TrueGetCommandLineA, MyGetCommandLineA)
                MEMUTIL_UNHOOK(TrueGetCommandLineW, MyGetCommandLineW)
            }
            break;
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        default: break;
    }
    return TRUE;
}
