#include "Library.h"
#include "Logger/Logger.h"
#include "MemoryUtility.h"
#include "AppLogger.h"
#include "AgentServer.h"
#include "DownloadServer.h"
#include "FarmManager.h"
#include "GatewayServer.h"
#include "GlobalManager.h"
#include "MachineManager.h"
#include "GameServer.h"
#include "ShardManager.h"
#include "HydraFramework.h"
#include "StaticPatches.h"
#include "Vanguard.h"
#include "version.h"

// Config
__inifile HydraIniFile;

// Globals
WCHAR szFullPath[MAX_PATH];
WCHAR szFileName[MAX_PATH];
WCHAR szFileExtn[MAX_PATH];
WCHAR szIniFile[MAX_PATH];

// Structure to hold EXE-specific settings
struct HydraModuleEntry {
    const wchar_t* exeName;
    const wchar_t* iniPath;
    const wchar_t* dllPath;
    void (*specialPatches)(); // Function pointer for unique patches
};

void DisableConsoleSelect()
{
    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    DWORD prev_mode;

    if (GetConsoleMode(hInput, &prev_mode))
    {
        DWORD new_mode = prev_mode & ~(0x0040 | 0x0020);

        new_mode |= 0x0080;

        SetConsoleMode(hInput, new_mode);
    }
}

void ConsoleBanner(const char* fullBuffer)
{
    BOOL TOTAL_WIDTH = 89;

    CONSOLE_SCREEN_BUFFER_INFO csbi;

    // Get handle to standard output
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    if (GetConsoleScreenBufferInfo(hStdout, &csbi)) {
        TOTAL_WIDTH = csbi.srWindow.Right - csbi.srWindow.Left;
    }

    stra_t displayStr = fullBuffer;
    int displayLen = (int)displayStr.length();

    // 2. Calculate padding for the center line
    // TotalWidth - 2 (╠ and ╣) - 2 (Spaces) - displayLen
    int totalInnerEquals = TOTAL_WIDTH - 2 - 2 - displayLen;
    if (totalInnerEquals < 0) totalInnerEquals = 0;

    int leftEquals = totalInnerEquals / 2;
    int rightEquals = totalInnerEquals - leftEquals;

    // 3. Build Top/Bottom Borders (Must be same width as TOTAL_WIDTH)
    stra_t topBorder = "╔";
    stra_t bottomBorder = "╚";
    for(int i = 0; i < (TOTAL_WIDTH - 2); ++i) {
        topBorder += "═";
        bottomBorder += "═";
    }
    topBorder += "╗";
    bottomBorder += "╝";

    // 4. Build the Centered Line
    stra_t centeredLine = "╠";
    for(int i = 0; i < leftEquals; ++i) centeredLine += "═";
    centeredLine += " ";
    centeredLine += displayStr;
    centeredLine += " ";
    for(int i = 0; i < rightEquals; ++i) centeredLine += "═";
    centeredLine += "╣";

    // 5. Output
    printf("%s\n", topBorder.c_str());
    printf("%s\n", centeredLine.c_str());
    printf("%s\n", bottomBorder.c_str());
}

void ShowConsole(bool showConsole, const char* name)
{
    char title[256], banner[256];

    _snprintf(title, sizeof(title), "%s - HydraFramework (%s)", name, BUILD_REVISION_STRING);
    _snprintf(banner, sizeof(banner), "Hydra-%s", name);

    if (AllocConsole() && showConsole)
    {
        SetConsoleTitleA(title);
        SetConsoleOutputCP(65001);
        freopen("CONOUT$", "w+", stdout);
        //ConsoleBanner(banner);
    }
}

// Special Patch Functions
void PatchAgentServer()
{
    CStaticPatches::AgentServerCertPatch();
    AgentServer::CAgentServer::Initialize();

    // MainView
    if (HydraIniFile.getbool(AgentServer::ModuleName, "hidemainview", false))
        MEMUTIL_WRITE_VALUE(BYTE, 0x00491CC2 + 1, SW_HIDE);

    if (HydraIniFile.getbool(AgentServer::ModuleName, "showconsole", false))
        ShowConsole(true, AgentServer::ModuleName);

    if (HydraIniFile.getbool(AgentServer::ModuleName, "outputlogs", false))
        CAppLogger::OutputLogs(true);
}

void PatchDownloadServer()
{
    CStaticPatches::DownloadServerCertPatch();
    DownloadServer::CDownloadServer::Initialize();

    // MainView
    if (HydraIniFile.getbool(DownloadServer::ModuleName, "hidemainview", false))
        MEMUTIL_WRITE_VALUE(BYTE, 0x01496422 + 1, SW_HIDE);

    if (HydraIniFile.getbool(DownloadServer::ModuleName, "showconsole", false))
        ShowConsole(true, DownloadServer::ModuleName);

    if (HydraIniFile.getbool(DownloadServer::ModuleName, "outputlogs", false))
        CAppLogger::OutputLogs(true);
}

void PatchFarmManager()
{
    CStaticPatches::FarmManagerCertPatch();
    FarmManager::CFarmManager::Initialize();

    // MainView
    if (HydraIniFile.getbool(FarmManager::ModuleName, "hidemainview", false))
        MEMUTIL_WRITE_VALUE(BYTE, 0x014A35D2 + 1, SW_HIDE);

    if (HydraIniFile.getbool(FarmManager::ModuleName, "showconsole", false))
        ShowConsole(true, FarmManager::ModuleName);

    if (HydraIniFile.getbool(FarmManager::ModuleName, "outputlogs", false))
        CAppLogger::OutputLogs(true);
}

void PatchGatewayServer()
{
    CStaticPatches::GatewayServerCertPatch();
    GatewayServer::CGatewayServer::Initialize();

    // MainView
    if (HydraIniFile.getbool(GatewayServer::ModuleName, "hidemainview", false))
        MEMUTIL_WRITE_VALUE(BYTE, 0x014ED312 + 1, SW_HIDE);

    if (HydraIniFile.getbool(GatewayServer::ModuleName, "showconsole", false))
        ShowConsole(true, GatewayServer::ModuleName);

    if (HydraIniFile.getbool(GatewayServer::ModuleName, "outputlogs", false))
        CAppLogger::OutputLogs(true);
}

void PatchGlobalManager()
{
    CStaticPatches::GlobalManagerCertPatch();
    GlobalManager::CGlobalManager::Initialize();

    // MainView
    if (HydraIniFile.getbool(GlobalManager::ModuleName, "hidemainview", false))
        MEMUTIL_WRITE_VALUE(BYTE, 0x0175ECD2 + 1, SW_HIDE);

    if (HydraIniFile.getbool(GlobalManager::ModuleName, "showconsole", false))
        ShowConsole(true, GlobalManager::ModuleName);

    if (HydraIniFile.getbool(GlobalManager::ModuleName, "outputlogs", false))
        CAppLogger::OutputLogs(true);
}

void PatchMachineManager()
{
    CStaticPatches::MachineManagerCertPatch();
    MachineManager::CMachineManager::Initialize();

    // MainView
    if (HydraIniFile.getbool(MachineManager::ModuleName, "hidemainview", false))
        MEMUTIL_WRITE_VALUE(BYTE, 0x014DEA92 + 1, SW_HIDE);

    if (HydraIniFile.getbool(MachineManager::ModuleName, "showconsole", false))
        ShowConsole(true, MachineManager::ModuleName);

    if (HydraIniFile.getbool(MachineManager::ModuleName, "outputlogs", false))
        CAppLogger::OutputLogs(true);
}

void PatchGameServer()
{
    CStaticPatches::GameServerCertPatch();
    GameServer::CGameServer::Initialize();

    // MainView
    if (HydraIniFile.getbool(GameServer::ModuleName, "hidemainview", false))
        MEMUTIL_WRITE_VALUE(BYTE, 0x00BCD6C2 + 1, SW_HIDE);

    if (HydraIniFile.getbool(GameServer::ModuleName, "showconsole", false))
        ShowConsole(true, GameServer::ModuleName);

    if (HydraIniFile.getbool(GameServer::ModuleName, "outputlogs", false))
        CAppLogger::OutputLogs(true);
}

void PatchSharManager()
{
    CStaticPatches::ShardManagerCertPatch();
    ShardManager::CShardManager::Initialize();

    // MainView
    if (HydraIniFile.getbool(ShardManager::ModuleName, "hidemainview", false))
        MEMUTIL_WRITE_VALUE(BYTE, 0x00A0DC22 + 1, SW_HIDE);

    if (HydraIniFile.getbool(ShardManager::ModuleName, "showconsole", false))
        ShowConsole(true, ShardManager::ModuleName);

    if (HydraIniFile.getbool(ShardManager::ModuleName, "outputlogs", false))
        CAppLogger::OutputLogs(true);
}

void PatchSMC() {
    MEMUTIL_WRITE_VALUE(BYTE, 0x004288A2, 0xEB);
}

// --- Configuration Table ---
// Add new modules here to support them automatically
static const HydraModuleEntry g_ModuleTable[] = {
    { L"agentserver.exe",     L".\\Vanguard\\AgentServer.ini",    L".\\Vanguard\\AgentServer.dll",    PatchAgentServer },
    { L"downloadserver.exe",  L".\\Vanguard\\DownloadServer.ini", L".\\Vanguard\\DownloadServer.dll", PatchDownloadServer },
    { L"farmmanager.exe",     L".\\Vanguard\\FarmManager.ini",    L".\\Vanguard\\FarmManager.dll",    PatchFarmManager },
    { L"gatewayserver.exe",   L".\\Vanguard\\GatewayServer.ini",  L".\\Vanguard\\GatewayServer.dll",  PatchGatewayServer },
    { L"globalmanager.exe",   L".\\Vanguard\\GlobalManager.ini",  L".\\Vanguard\\GlobalManager.dll",  PatchGlobalManager },
    { L"machinemanager.exe",  L".\\Vanguard\\MachineManager.ini", L".\\Vanguard\\MachineManager.dll", PatchMachineManager },
    { L"gameserver.exe",      L".\\Vanguard\\GameServer.ini",     L".\\Vanguard\\GameServer.dll",     PatchGameServer },
    { L"shardmanager.exe",    L".\\Vanguard\\ShardManager.ini",   L".\\Vanguard\\ShardManager.dll",   PatchSharManager },
    { L"smc.exe",             L".\\Vanguard\\SMC.ini",            L".\\Vanguard\\SMC.dll",            PatchSMC }
};

void GetCurrentDLLNameW(WCHAR* szName, size_t bufferSize)
{
    HMODULE hModule = NULL;

    if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           (LPCWSTR)GetCurrentDLLNameW, &hModule))
    {
        if (GetModuleFileNameW(hModule, szName, (DWORD)bufferSize))
        {
            WCHAR* pDot = wcsrchr(szName, L'.');

            if (pDot != NULL) {
                size_t index = pDot - szName;
                if (index + 5 <= bufferSize)
                    wcscpy(pDot, L".ini");
            } else {
                size_t len = wcslen(szName);
                if (len + 5 <= bufferSize)
                    wcscat(szName, L".ini");
            }
        }
    }
}

void CLibrary::DoSomeWork()
{
    stdext::inifile vanguard;

    HANDLE hProcess = GetCurrentProcess();
    GetModuleFileNameW(NULL, szFullPath, MAX_PATH);
    _wsplitpath(szFullPath, NULL, NULL, szFileName, szFileExtn);

    GetCurrentDLLNameW(szIniFile, MAX_PATH);

    if (!HydraIniFile.load(szIniFile))
    {
        char err[256];
        _snprintf(err, sizeof(err), "Cannot find \"%ls\", contact administrator.", szIniFile);
        MessageBoxA(NULL, err, "Hydra", MB_ICONERROR | MB_OK);
        TerminateProcess(hProcess, EXIT_FAILURE);
        return;
    }

    const stra_t fileName = __tostra(szFileName);
    const stra_t extension = HydraIniFile.getvalue(fileName, "plugindll");

    // --- Core Logic: Loop through the table ---
    for (int i = 0; i < sizeof(g_ModuleTable) / sizeof(HydraModuleEntry); ++i)
    {
        if (!__stringw(szFullPath).endswith(g_ModuleTable[i].exeName))
            continue;

        // 2. Load Vanguard
        if (vanguard.load(g_ModuleTable[i].iniPath) && __chkfile(g_ModuleTable[i].dllPath).exists())
        {
            CVanguard::CreateInstance();
            g_pVanguard->m_dllPath = g_ModuleTable[i].dllPath;
        }

        // 3. Load Addons
        if (__stringa(extension).endswith(".dll") && __chkfile(extension).exists())
        {
            if (!LoadLibraryA(extension.c_str()))
            {
                MessageBoxA(NULL, extension.c_str(), "Failed to load Extension", MB_ICONERROR | MB_OK);
                TerminateProcess(hProcess, EXIT_FAILURE);
                return;
            }
        }

        // 1. Run Special patches (like SMC or GlobalManager)
        if (g_ModuleTable[i].specialPatches)
            g_ModuleTable[i].specialPatches();
    }
}
