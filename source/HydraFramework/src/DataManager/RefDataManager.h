#pragma once

#include "pch.h"
#include "CriticalSection.h"
#include "RefLevel.h"
#include "RefNonSplitItem.h"
#include "RefObjChar.h"
#include "RefObjItem.h"
#include "RefObjStruct.h"
#include "RefSkill.h"

namespace HydraFramework
{
    typedef std::map<DWORD, RefLevel> LevelData;
    typedef std::map<DWORD, RefObjChar> CharData;
    typedef std::map<std::string, DWORD> CharIndex;
    typedef std::map<DWORD, RefObjItem> ItemData;
    typedef std::map<std::string, DWORD> ItemIndex;
    typedef std::map<DWORD, RefObjStruct> StructData;
    typedef std::map<std::string, DWORD> StructIndex;
    typedef std::map<DWORD, RefSkill> SkillData;
    typedef std::map<std::string, DWORD> SkillIndex;
    typedef std::map<DWORD, RefNonSplitItem> NonSplitItem;

    class CRefDataManager {
        friend class CHydraProcess;
    public:
        long InitializeData();
        RefObjChar *GetRefCharData(DWORD dwID);
        RefObjChar *GetRefCharData(const char* codename);
        RefObjItem *GetRefItemData(DWORD dwID);
        RefObjItem *GetRefItemData(const char* codename);
        RefObjStruct *GetRefStructData(DWORD dwID);
        RefObjStruct *GetRefStructData(const char* codename);
        RefSkill *GetRefSkillData(DWORD dwID);
        RefSkill *GetRefSkillData(const char* codename);
        RefLevel *GetLevelData(BYTE btLvl);
        bool IsNonSplitItem(DWORD dwRefItemID);

    private:
        bool InitializeLevel();
        bool InitializeObjChar();
        bool InitializeObjItem();
        bool InitializeObjStruct();
        bool InitializeSkillData();
        bool InitializeNonSplitItem();
        void CleanUp();

        CRefDataManager();
        CRefDataManager(const CRefDataManager&);
        CRefDataManager& operator=(const CRefDataManager&);
        ~CRefDataManager() { CleanUp(); }

        class CDbManager* m_pDbMgr;
        CCriticalSection m_cs;
        LevelData m_LevelData;
        CharData m_CharData;
        CharIndex m_CharIndex;
        ItemData m_ItemData;
        ItemIndex m_ItemIndex;
        StructData m_StructData;
        StructIndex m_StructIndex;
        SkillData m_SkillData;
        SkillIndex m_SkillIndex;
        NonSplitItem m_NonSplitItem;
    };
}
