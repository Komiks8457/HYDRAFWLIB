#include "ObjManager.h"
#include "AppLogger.h"
#include "Logger/Logger.h"

namespace HydraFramework
{
    CObjManager::CObjManager() : m_pDbMgr(NULL)
    {
        //PutLog(FATAL, "HydraFramework::ObjectMgr() initialized!");
    }

    CObjManager::~CObjManager()
    {
        for (pc_map::iterator it = m_pc_map.begin(); it != m_pc_map.end(); ++it)
            delete it->second;

        for (p2_map::iterator it = m_p2_map.begin(); it != m_p2_map.end(); ++it)
            delete it->second;

        for (mob_map::iterator it = m_mob_map.begin(); it != m_mob_map.end(); ++it)
            delete it->second;

        m_pc_map.clear();
        m_p2_map.clear();
        m_mob_map.clear();
        m_obj_map.clear();
    }

    void CObjManager::AddObject(CGObjPC* pPC, DWORD dwGameID)
    {
        ACS_SCOPED_LOCK(m_cs);
        if (IsValidObj(pPC) || IsValidObj(dwGameID))
            return;
        PCExt* pPCExt = new PCExt();
        m_pc_map[pPC] = pPCExt;
        m_pc_map[pPC]->m_pPC = pPC;
        m_pc_map[pPC]->m_dwGameID = dwGameID;
        m_obj_map[dwGameID] = pPCExt;
        Logger::info2("%sPC() : 0x%p", __FUNCTION__, pPC);
    }

    void CObjManager::AddObject(CGObjCOS_NewPet* pP2, DWORD dwGameID)
    {
        ACS_SCOPED_LOCK(m_cs);
        if (IsValidObj(pP2) || IsValidObj(dwGameID))
            return;
        P2Ext* pP2Ext = new P2Ext();
        m_p2_map[pP2] = pP2Ext;
        m_p2_map[pP2]->m_pP2 = pP2;
        m_p2_map[pP2]->m_dwGameID = dwGameID;
        m_obj_map[dwGameID] = pP2Ext;
        Logger::info2("%sFellow() : 0x%p", __FUNCTION__, pP2);
    }

    void CObjManager::AddObject(CGObjMob* pMob, DWORD dwGameID)
    {
        ACS_SCOPED_LOCK(m_cs);
        if (IsValidObj(pMob) || IsValidObj(dwGameID))
            return;
        MobExt* pMobExt = new MobExt();
        m_mob_map[pMob] = pMobExt;
        m_mob_map[pMob]->m_pMob = pMob;
        m_mob_map[pMob]->m_dwGameID = dwGameID;
        m_obj_map[dwGameID] = pMobExt;
        //Logger::info2("%sMob() : 0x%p", __FUNCTION__, pMob);
    }

    void CObjManager::AddObject(CGObjNPCNPC* pNPC, DWORD dwGameID)
    {
        ACS_SCOPED_LOCK(m_cs);
        if (IsValidObj(pNPC) || IsValidObj(dwGameID))
            return;
        NPCExt* pNPCExt = new NPCExt();
        m_npc_map[pNPC] = pNPCExt;
        m_npc_map[pNPC]->m_pNPC = pNPC;
        m_npc_map[pNPC]->m_dwGameID = dwGameID;
        m_obj_map[dwGameID] = pNPCExt;
        //Logger::info2("%sMob() : 0x%p", __FUNCTION__, pMob);
    }

    PCExt* CObjManager::GetObjExt(CGObjPC* pPC)
    {
        ACS_SCOPED_LOCK(m_cs);
        return IsValidObj(pPC) ? m_pc_map[pPC] : NULL;
    }

    P2Ext* CObjManager::GetObjExt(CGObjCOS_NewPet* pP2)
    {
        ACS_SCOPED_LOCK(m_cs);
        return IsValidObj(pP2) ? m_p2_map[pP2] : NULL;
    }

    MobExt* CObjManager::GetObjExt(CGObjMob* pMob)
    {
        ACS_SCOPED_LOCK(m_cs);
        return IsValidObj(pMob) ? m_mob_map[pMob] : NULL;
    }

    NPCExt* CObjManager::GetObjExt(CGObjNPCNPC* pNPC)
    {
        ACS_SCOPED_LOCK(m_cs);
        return IsValidObj(pNPC) ? m_npc_map[pNPC] : NULL;
    }

    BaseExt* CObjManager::GetObjExt(DWORD dwGameID)
    {
        ACS_SCOPED_LOCK(m_cs);
        return IsValidObj(dwGameID) ? m_obj_map[dwGameID] : NULL;
    }

    void CObjManager::DelObject(CGObjPC *pPC)
    {
        ACS_SCOPED_LOCK(m_cs);
        pc_map::iterator it = m_pc_map.find(pPC);
        if (it == m_pc_map.end())
            return;
        DelObjectIndex(it->second->m_dwGameID);
        delete it->second;
        m_pc_map.erase(it);
        //Logger::info2("%sPC() : 0x%p", __FUNCTION__, pPC);
    }

    void CObjManager::DelObject(CGObjCOS_NewPet *pP2)
    {
        ACS_SCOPED_LOCK(m_cs);
        p2_map::iterator it = m_p2_map.find(pP2);
        if (it == m_p2_map.end())
            return;
        DelObjectIndex(it->second->m_dwGameID);
        delete it->second;
        m_p2_map.erase(it);
        //Logger::info2("%sFellow() : 0x%p", __FUNCTION__, pP2);
    }

    void CObjManager::DelObject(CGObjMob* pMob)
    {
        ACS_SCOPED_LOCK(m_cs);
        mob_map::iterator it = m_mob_map.find(pMob);
        if (it == m_mob_map.end())
            return;
        DelObjectIndex(it->second->m_dwGameID);
        delete it->second;
        m_mob_map.erase(it);
        //Logger::info2("%sMob() : 0x%p", __FUNCTION__, pMob);
    }

    void CObjManager::DelObject(CGObjNPCNPC* pNPC)
    {
        ACS_SCOPED_LOCK(m_cs);
        npc_map::iterator it = m_npc_map.find(pNPC);
        if (it == m_npc_map.end())
            return;
        DelObjectIndex(it->second->m_dwGameID);
        delete it->second;
        m_npc_map.erase(it);
        Logger::info2("%sNPC() : 0x%p", __FUNCTION__, pNPC);
    }

    void CObjManager::DelObject(DWORD dwGameID)
    {
        ACS_SCOPED_LOCK(m_cs);
        BaseExt* pBExt = GetObjExt(dwGameID);
        if (!pBExt) return;

        if (pBExt->m_IsPC) {
            DelObject(static_cast<PCExt*>(pBExt)->m_pPC);
        } else if (pBExt->m_IsP2) {
            DelObject(static_cast<P2Ext*>(pBExt)->m_pP2);
        } else if (pBExt->m_IsMob) {
            DelObject(static_cast<MobExt*>(pBExt)->m_pMob);
        } else if (pBExt->m_IsNPC) {
            DelObject(static_cast<NPCExt*>(pBExt)->m_pNPC);
        } else {
            Logger::info2("%sBaseExt() : 0x%p", __FUNCTION__, pBExt);
        }
    }

    void CObjManager::DelObjectIndex(DWORD dwGameID)
    {
        ACS_SCOPED_LOCK(m_cs);
        if (!IsValidObj(dwGameID))
            return;
        m_obj_map.erase(dwGameID);
    }

    bool CObjManager::IsValidObj(CGObjPC* pPC)
    {
        ACS_SCOPED_LOCK(m_cs);
        return m_pc_map.find(pPC) != m_pc_map.end();
    }

    bool CObjManager::IsValidObj(CGObjCOS_NewPet* pP2)
    {
        ACS_SCOPED_LOCK(m_cs);
        return m_p2_map.find(pP2) != m_p2_map.end();
    }

    bool CObjManager::IsValidObj(CGObjNPCNPC* pNPC)
    {
        ACS_SCOPED_LOCK(m_cs);
        return m_npc_map.find(pNPC) != m_npc_map.end();
    }

    bool CObjManager::IsValidObj(CGObjMob* pMob)
    {
        ACS_SCOPED_LOCK(m_cs);
        return m_mob_map.find(pMob) != m_mob_map.end();
    }

    bool CObjManager::IsValidObj(DWORD dwGameID)
    {
        ACS_SCOPED_LOCK(m_cs);
        return m_obj_map.find(dwGameID) != m_obj_map.end();
    }
}