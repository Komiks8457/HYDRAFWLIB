#pragma once

class CStaticPatches {
public:
    static void AgentServerCertPatch();
    static void DownloadServerCertPatch();
    static void FarmManagerCertPatch();
    static void GatewayServerCertPatch();
    static void GlobalManagerCertPatch();
    static void MachineManagerCertPatch();
    static void ShardManagerCertPatch();
    static void GameServerCertPatch();
};
