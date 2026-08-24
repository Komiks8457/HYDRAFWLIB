#pragma once

#include "Pattern/Singleton.h"
#include "CriticalSection.h"
#include "ThreadPool.h"
#include "NetEngine/ServerInfo.h"

///////////////////////////////////////////////////////////
// HydraFramework

#define ENQUEUETASK(func, arg) \
    if (g_pHFW && g_pHFW->GetThreadPool()) { \
        g_pHFW->GetThreadPool()->EnqueueTask(func, arg); \
    } \

namespace HydraFramework
{
    typedef void (*TaskRoutine)(void*);

    class CDbManager;
    class CRefDataManager;
    class CObjManager;
    class CREST;
    class CServer;

    class CHydraProcess: public CSingletonT<CHydraProcess> {
        friend class CSingletonT<CHydraProcess>;
    public:
        static bool CreateInstance(void* ptr, const char* module)
        {
            if (GetSingletonPtr()) return false;
            static CHydraProcess init(ptr, module);
            return true;
        }

        const char* GetVersion() { return m_version; }
        const char* GetSettingFile() { return m_dllName; }
        CThreadPool *GetThreadPool() { return m_ThreadPool; }

        bool InitDbMgr(short min, short max);
        void InitRefDataMgr();
        void InitObjectMgr();

    private:
        CHydraProcess(void* ptr, const char* module);
        CHydraProcess(const CHydraProcess&);
        CHydraProcess& operator=(const CHydraProcess&);
        virtual ~CHydraProcess();

        static void InitTask(void *ptr);

        CCriticalSection m_cs;
        char m_version[25];
        char m_dllName[25];

    public:
        CThreadPool* m_ThreadPool;
        CDbManager* m_DbMgr;
        CRefDataManager* m_RefDataMgr;
        CObjManager* m_ObjMgr;

        DbConnStringInfo m_dbConn;

        ServerInfo m_ServerInfo;
    };

    inline __declspec(naked) int NO_ENDLESS_LOOP_DUMP()
    {
        __asm {
            xor eax, eax;
            ret 4;
        }
    }
}

using namespace HydraFramework;

#define g_pHFW (&CHydraProcess::GetSingleton())

