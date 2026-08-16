#include "HydraFramework.h"
#include "AppLogger.h"
#include "NetEngine/NetEngine.h"
#include "MemoryUtility.h"
#include "Database/DbManager.h"
#include "DataManager/RefDataManager.h"
#include "ObjectManager/ObjManager.h"
#include "ThreadPool.h"
#include "version.h"

///////////////////////////////////////////////////////////
// HydraFramework

template<> CHydraProcess* CSingletonT<CHydraProcess>::s_pObject = NULL;

namespace HydraFramework
{
    void CHydraProcess::InitTask(void *ptr)
    {
        DWORD netengine = MEMUTIL_ADD_PTR(ptr, 0x14C);
        while (!g_pNetEngine && !IS_READY_STATE) {
            CNetEngine::Initialize(netengine);
            Sleep(50);
        }
        PutLog(FATAL, "%s complete, NetEngine OK!", __FUNCTION__);
    }

    CHydraProcess::CHydraProcess(void* ptr, const char* module) : m_ThreadPool(NULL), m_DbMgr(NULL),
                                                                  m_RefDataMgr(NULL), m_ObjMgr(NULL)
    {
        _snprintf(m_version, sizeof(m_version), "%s", BUILD_REVISION_STRING);
        _snprintf(m_dllName, sizeof(m_dllName), "%s", stdext::getmodule());

        PutLog(FATAL, "========================================================");
        PutLog(FATAL, "HydraFramework - module init");
        PutLog(FATAL, "%s %s Compiled (%s)", __DATE__, __TIME__, m_version);
        PutLog(FATAL, "========================================================");

        m_ThreadPool = new CThreadPool(5);
        m_ThreadPool->EnqueueTask(InitTask, ptr);
    }

    bool CHydraProcess::InitDbMgr(short min, short max)
    {
        m_DbMgr = new CDbManager();
        m_DbMgr->m_dbMinConn = min;
        m_DbMgr->m_dbMaxConn = max;

        int dbConnect = m_DbMgr->InitDB(&m_dbConn);

        if (dbConnect < 0) {
            PutLog(FATAL, "%s() Failed (error: %d)", __FUNCTION__, dbConnect);
            return false;
        }

        PutLog(FATAL, "%s() OK!", __FUNCTION__);
        return true;
    }

    void CHydraProcess::InitRefDataMgr()
    {
        if (m_DbMgr == NULL) {
            PutLog(FATAL, "%s, m_DbMgr is NULL", __FUNCTION__);
            return;
        }

        if (!m_DbMgr->IsDbConnected()) {
            PutLog(FATAL, "%s, m_DbMgr is initialized but not connected", __FUNCTION__);
            return;
        }

        m_RefDataMgr = new CRefDataManager();
        m_RefDataMgr->m_pDbMgr = m_DbMgr;

        PutLog(FATAL, "%s() OK!", __FUNCTION__);
    }

    void CHydraProcess::InitObjectMgr()
    {
        if (m_DbMgr == NULL) {
            PutLog(FATAL, "%s, m_DbMgr is NULL", __FUNCTION__);
            return;
        }

        if (!m_DbMgr->IsDbConnected()) {
            PutLog(FATAL, "%s, m_DbMgr is initialized but not connected", __FUNCTION__);
            return;
        }

        m_ObjMgr = new CObjManager();
        m_ObjMgr->m_pDbMgr = m_DbMgr;

        PutLog(FATAL, "%s() OK!", __FUNCTION__);
    }

    CHydraProcess::~CHydraProcess()
    {
        delete m_ThreadPool;

        if (m_RefDataMgr) delete m_RefDataMgr;
        if (m_ObjMgr) delete m_ObjMgr;
        if (m_DbMgr) delete m_DbMgr;
    }
}
