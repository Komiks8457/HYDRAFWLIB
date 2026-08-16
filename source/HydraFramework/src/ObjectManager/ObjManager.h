#pragma once

#include "pch.h"
#include "CriticalSection.h"
#include "NetEngine/NetEngine.h"

class CGObjPC;
class CGObjCOS_NewPet;
class CGObjMob;
class CGObjNPCNPC;

namespace HydraFramework
{
    struct BaseExt
    {
        bool m_IsPC, m_IsP2, m_IsMob, m_IsNPC;

        BaseExt() : m_IsPC(false), m_IsP2(false), m_IsMob(false), m_IsNPC(false)
        {}

        virtual ~BaseExt()
        {}
    };

    enum EActionType
    {
        ACTION_SKILL    = 0,
        ACTION_ITEM     = 1,
        ACTION_CHAT     = 2,
        ACTION_MOUNT    = 3
    };

    struct PCExt : public BaseExt
    {
        CGObjPC* m_pPC;
        DWORD m_dwGameID;
        DWORD m_dwJID;
        DWORD m_dwCharID;
        DWORD m_dwSessionID;
        bool m_bIsInvSorting;
        bool m_bIsNPCMenuOpened;

        std::queue<CMsg*> m_qMsg;
        std::map<EActionType, DWORD> m_mapCooldowns;
        CCriticalSection m_csTask;

        PCExt() : m_pPC(NULL), m_dwGameID(0), m_dwJID(0), m_dwCharID(0),
                  m_dwSessionID(0), m_bIsInvSorting(false), m_bIsNPCMenuOpened(false)
        { m_IsPC = true; }

        bool IsActionReady(EActionType nActionID)
        {
            std::map<EActionType, DWORD>::iterator it = m_mapCooldowns.find(nActionID);
            if (it == m_mapCooldowns.end())
                return true;
            return (GetTickCount() >= it->second);
        }

        void SetActionCooldown(EActionType nActionID, DWORD dwDurationMs)
        {
            m_mapCooldowns[nActionID] = GetTickCount() + dwDurationMs;
        }

        ~PCExt()
        {
            while(!m_qMsg.empty())
            {
                DELMSG(m_qMsg.front());
                m_qMsg.pop();
            }

            m_mapCooldowns.clear();
        }

    private:
        PCExt(const PCExt&);
        PCExt& operator=(const PCExt&);
    };

    struct P2Ext : public BaseExt
    {
        CGObjCOS_NewPet* m_pP2;
        DWORD m_dwGameID;
        LONG64 m_llExpOffset;
        PCExt* m_pOwner;

        std::queue<CMsg*> m_qMsg;
        CCriticalSection m_csTask;

        CGObjPC* GetOwner() const { return m_pOwner->m_pPC; }

        P2Ext() : m_pP2(NULL), m_dwGameID(0), m_llExpOffset(0), m_pOwner(NULL)
        { m_IsP2 = true; }

        ~P2Ext()
        {
            while(!m_qMsg.empty()) {
                DELMSG(m_qMsg.front());
                m_qMsg.pop();
            }
        }

    private:
        P2Ext(const P2Ext&);
        P2Ext& operator=(const P2Ext&);
    };

    struct MobExt : public BaseExt
    {
        CGObjMob* m_pMob;
        DWORD m_dwGameID;

        CCriticalSection m_csTask;

        MobExt() : m_pMob(NULL), m_dwGameID(0)
        { m_IsMob = true; }

    private:
        MobExt(const MobExt&);
        MobExt& operator=(const MobExt&);
    };

    struct NPCExt : public BaseExt
    {
        CGObjNPCNPC* m_pNPC;
        DWORD m_dwGameID;

        CCriticalSection m_csTask;

        NPCExt() : m_pNPC(NULL), m_dwGameID(0)
        { m_IsNPC = true; }

    private:
        NPCExt(const NPCExt&);
        NPCExt& operator=(const NPCExt&);
    };

    typedef std::map<CGObjPC*, PCExt*> pc_map;
    typedef std::map<CGObjCOS_NewPet*, P2Ext*> p2_map;
    typedef std::map<CGObjMob*, MobExt*> mob_map;
    typedef std::map<CGObjNPCNPC*, NPCExt*> npc_map;
    typedef std::map<DWORD, BaseExt*> obj_map;

    class CObjManager {
        friend class CHydraProcess;
    public:

        void AddObject(CGObjPC* pPC, DWORD dwGameID);
        void AddObject(CGObjCOS_NewPet* pP2, DWORD dwGameID);
        void AddObject(CGObjMob* pMob, DWORD dwGameID);
        void AddObject(CGObjNPCNPC* pNPC, DWORD dwGameID);

        PCExt* GetObjExt(CGObjPC* pPC);
        P2Ext* GetObjExt(CGObjCOS_NewPet* pP2);
        MobExt* GetObjExt(CGObjMob* pMob);
        NPCExt* GetObjExt(CGObjNPCNPC* pNPC);
        BaseExt* GetObjExt(DWORD dwGameID);

        void DelObject(CGObjPC* pPC);
        void DelObject(CGObjCOS_NewPet* pP2);
        void DelObject(CGObjMob* pMob);
        void DelObject(CGObjNPCNPC* pNPC);
        void DelObject(DWORD dwGameID);

        bool IsValidObj(CGObjPC* pPC);
        bool IsValidObj(CGObjCOS_NewPet* pP2);
        bool IsValidObj(CGObjMob* pMob);
        bool IsValidObj(CGObjNPCNPC* pNPC);
        bool IsValidObj(DWORD dwGameID);

    private:
        class CDbManager* m_pDbMgr;
        CCriticalSection m_cs;

        pc_map m_pc_map;
        p2_map m_p2_map;
        mob_map m_mob_map;
        npc_map m_npc_map;
        obj_map m_obj_map; //global obj index

        void DelObjectIndex(DWORD dwGameID);

        CObjManager();
        CObjManager(const CObjManager&);
        CObjManager& operator=(const CObjManager&);
        ~CObjManager();
    };
}

