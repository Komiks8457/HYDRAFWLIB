#include "RefDataManager.h"
#include "Database/DbManager.h"
#include "Database/DbRowReader.h"
#include "AppLogger.h"

namespace HydraFramework
{
    CRefDataManager::CRefDataManager() : m_pDbMgr(NULL)
    {
        //PutLog(FATAL, "HydraFramework::RefDataMgr() initialized!");
    }

    long CRefDataManager::InitializeData()
    {
        if (!m_pDbMgr)
            return -65535;

        if (!InitializeObjChar())
            return -65534;

        if (!InitializeObjItem())
            return -65533;

        if (!InitializeObjStruct())
            return -65532;

        if (!InitializeSkillData())
            return -65531;

        if (!InitializeLevel())
            return -65530;

        if (!InitializeNonSplitItem())
            return -65529;

        return TRUE;
    }

    bool CRefDataManager::InitializeNonSplitItem()
    {
        CDbRowReader reader;
        CDbConnection *pConnection = m_pDbMgr->GetConnection(SHARD);

        const stra_t &query("SELECT * FROM _RefNonSplitItem");

        pConnection->ExecuteReader(reader, query.c_str());

        while (reader.FetchNext())
        {
            RefNonSplitItem nonSplitItem;
            nonSplitItem.m_RefItemID = reader.Read<long>(); //RefItemID
            m_NonSplitItem[nonSplitItem.m_RefItemID] = nonSplitItem;
        }

        if (m_NonSplitItem.empty())
        {
            m_pDbMgr->Release(SHARD, pConnection);
            return false;
        }

        //PutLog(FATAL, "HydraFramework::RefDataManager loaded %d RefNonSplitItem entries.", m_NonSplitItem.size());
        m_pDbMgr->Release(SHARD, pConnection);
        return true;
    }

    bool CRefDataManager::InitializeObjChar()
    {
        CDbRowReader reader;
        CDbConnection *pConnection = m_pDbMgr->GetConnection(SHARD);

        const stra_t &query("SELECT CMN.*, CHR.* FROM [dbo].[_RefObjChar] CHR INNER JOIN [dbo].[_RefObjCommon] "
                            "CMN ON CMN.Link = CHR.ID WHERE CMN.[Service] = 1 AND CMN.Bionic = 1 AND CMN.TypeID1 = 1 "
                            "ORDER BY CMN.ID");

        pConnection->ExecuteReader(reader, query.c_str());

        while (reader.FetchNext())
        {
            reader.Read<long>(); //Service

            RefObjChar charData;
            charData.m_ID = reader.Read<long>();
            reader.ReadString(charData.m_CodeName128, 128);
            reader.ReadString(charData.m_ObjName128, 128);
            reader.ReadString(charData.m_OrgObjCodeName128, 128);
            reader.ReadString(charData.m_NameStrID128, 128);
            reader.ReadString(charData.m_DescStrID128, 128);
            charData.m_TypeID.SetIsCashItem(reader.Read<bool>());
            charData.m_TypeID.SetIsBionic(reader.Read<bool>());
            charData.m_TypeID.SetFirst(reader.Read<unsigned char>());
            charData.m_TypeID.SetSecond(reader.Read<unsigned char>());
            charData.m_TypeID.SetThird(reader.Read<unsigned char>());
            charData.m_TypeID.SetFourth(reader.Read<unsigned char>());
            charData.m_DecayTime = reader.Read<long>();
            charData.m_Country = reader.Read<char>();
            charData.m_Rarity = reader.Read<char>();
            charData.m_CanTrade = reader.Read<char>();
            charData.m_CanSell = reader.Read<char>();
            charData.m_CanBuy = reader.Read<char>();
            charData.m_CanBorrow = reader.Read<char>();
            charData.m_CanDrop = reader.Read<char>();
            charData.m_CanPick = reader.Read<char>();
            charData.m_CanRepair = reader.Read<char>();
            charData.m_CanRevive = reader.Read<char>();
            charData.m_CanUse = reader.Read<char>();
            charData.m_CanThrow = reader.Read<char>();
            charData.m_Price = reader.Read<long>();
            charData.m_CostRepair = reader.Read<long>();
            charData.m_CostRevive = reader.Read<long>();
            charData.m_CostBorrow = reader.Read<long>();
            charData.m_KeepingFee = reader.Read<long>();
            charData.m_SellPrice = reader.Read<long>();
            charData.m_ReqLevelType1 = reader.Read<long>();
            charData.m_ReqLevel1 = reader.Read<char>();
            charData.m_ReqLevelType2 = reader.Read<long>();
            charData.m_ReqLevel2 = reader.Read<char>();
            charData.m_ReqLevelType3 = reader.Read<long>();
            charData.m_ReqLevel3 = reader.Read<char>();
            charData.m_ReqLevelType4 = reader.Read<long>();
            charData.m_ReqLevel4 = reader.Read<char>();
            charData.m_MaxContain = reader.Read<long>();
            charData.m_RegionInfo.RegionID = reader.Read<short>();
            charData.m_RegionInfo.RegionBlockID = reader.Read<short>();
            charData.m_RegionInfo.PosX = reader.Read<float>();
            charData.m_RegionInfo.PosY = reader.Read<float>();
            charData.m_RegionInfo.PosZ = reader.Read<float>();
            charData.m_Speed1 = reader.Read<short>();
            charData.m_Speed2 = reader.Read<short>();
            charData.m_Scale = reader.Read<long>();
            charData.m_BCHeight = reader.Read<short>();
            charData.m_BCRadius = reader.Read<short>();
            charData.m_EventID = reader.Read<long>();
            reader.ReadString(charData.m_AssocFileObj128, 128);
            reader.ReadString(charData.m_AssocFileDrop128, 128);
            reader.ReadString(charData.m_AssocFileIcon128, 128);
            reader.ReadString(charData.m_AssocFile1_128, 128);
            reader.ReadString(charData.m_AssocFile2_128, 128);
            reader.Read<long>(); //RefObjCommon.Link
            reader.Read<long>(); //RefObjChar.ID
            charData.m_Lvl = reader.Read<char>();
            charData.m_CharGender = reader.Read<char>();
            charData.m_MaxHP = reader.Read<long>();
            charData.m_MaxMP = reader.Read<long>();

            for (long i = 0; i < Resist::MAX_RESIST_TYPE; ++i)
                charData.m_Resist[i].Value = reader.Read<long>();

            charData.m_InventorySize = reader.Read<char>();
            charData.m_CanStore_TID1 = reader.Read<char>();
            charData.m_CanStore_TID2 = reader.Read<char>();
            charData.m_CanStore_TID3 = reader.Read<char>();
            charData.m_CanStore_TID4 = reader.Read<char>();
            charData.m_CanBeVehicle = reader.Read<char>();
            charData.m_CanControl = reader.Read<char>();
            charData.m_DamagePortion = reader.Read<char>();
            charData.m_MaxPassenger = reader.Read<short>();
            charData.m_AssocTactics = reader.Read<long>();
            charData.m_PD = reader.Read<long>();
            charData.m_MD = reader.Read<long>();
            charData.m_PAR = reader.Read<long>();
            charData.m_MAR = reader.Read<long>();
            charData.m_ER = reader.Read<long>();
            charData.m_BR = reader.Read<long>();
            charData.m_HR = reader.Read<long>();
            charData.m_CHR = reader.Read<long>();
            charData.m_ExpToGive = reader.Read<long>();
            charData.m_CreepType = reader.Read<long>();
            charData.m_Knockdown = reader.Read<char>();
            charData.m_KO_RecoverTime = reader.Read<long>();

            for (long i = 0; i < DefaultSkill::MAX_DEFAULT_SKILL; ++i)
                charData.m_DefaultSkill[i].Value = reader.Read<long>();

            charData.m_TextureType = reader.Read<char>();

            for (long i = 0; i < Except::MAX_EXCEPT_TYPE; ++i)
                charData.m_Except[i].Value = reader.Read<long>();

            reader.Read<long>(); //RefObjChar.Link

            m_CharData[charData.m_ID] = charData;
            m_CharIndex[charData.m_CodeName128] = charData.m_ID;
        }

        if (m_CharData.empty())
        {
            m_pDbMgr->Release(SHARD, pConnection);
            return false;
        }

        //PutLog(FATAL, "HydraFramework::RefDataManager loaded %d RefObjChar entries.", m_CharData.size());
        m_pDbMgr->Release(SHARD, pConnection);
        return true;
    }

    bool CRefDataManager::InitializeObjItem()
    {
        CDbRowReader reader;
        CDbConnection *pConnection = m_pDbMgr->GetConnection(SHARD);

        const stra_t &query("SELECT CMN.*, ITEM.* FROM [dbo].[_RefObjItem] ITEM INNER JOIN [dbo].[_RefObjCommon] "
                            "CMN ON CMN.Link = ITEM.ID WHERE CMN.[Service] = 1 AND CMN.TypeID1 = 3 ORDER BY CMN.ID");

        pConnection->ExecuteReader(reader, query.c_str());

        while (reader.FetchNext())
        {
            reader.Read<long>(); //Service

            RefObjItem itemData;
            itemData.m_ID = reader.Read<long>();
            reader.ReadString(itemData.m_CodeName128, 128);
            reader.ReadString(itemData.m_ObjName128, 128);
            reader.ReadString(itemData.m_OrgObjCodeName128, 128);
            reader.ReadString(itemData.m_NameStrID128, 128);
            reader.ReadString(itemData.m_DescStrID128, 128);
            itemData.m_TypeID.SetIsCashItem(reader.Read<bool>());
            itemData.m_TypeID.SetIsBionic(reader.Read<bool>());
            itemData.m_TypeID.SetFirst(reader.Read<unsigned char>());
            itemData.m_TypeID.SetSecond(reader.Read<unsigned char>());
            itemData.m_TypeID.SetThird(reader.Read<unsigned char>());
            itemData.m_TypeID.SetFourth(reader.Read<unsigned char>());
            //itemData.m_CashItem = reader.Read<char>();
            //itemData.m_Bionic = reader.Read<char>();
            //itemData.m_TypeID1 = reader.Read<char>();
            //itemData.m_TypeID2 = reader.Read<char>();
            //itemData.m_TypeID3 = reader.Read<char>();
            //itemData.m_TypeID4 = reader.Read<char>();
            itemData.m_DecayTime = reader.Read<long>();
            itemData.m_Country = reader.Read<char>();
            itemData.m_Rarity = reader.Read<char>();
            itemData.m_CanTrade = reader.Read<char>();
            itemData.m_CanSell = reader.Read<char>();
            itemData.m_CanBuy = reader.Read<char>();
            itemData.m_CanBorrow = reader.Read<char>();
            itemData.m_CanDrop = reader.Read<char>();
            itemData.m_CanPick = reader.Read<char>();
            itemData.m_CanRepair = reader.Read<char>();
            itemData.m_CanRevive = reader.Read<char>();
            itemData.m_CanUse = reader.Read<char>();
            itemData.m_CanThrow = reader.Read<char>();
            itemData.m_Price = reader.Read<long>();
            itemData.m_CostRepair = reader.Read<long>();
            itemData.m_CostRevive = reader.Read<long>();
            itemData.m_CostBorrow = reader.Read<long>();
            itemData.m_KeepingFee = reader.Read<long>();
            itemData.m_SellPrice = reader.Read<long>();
            itemData.m_ReqLevelType1 = reader.Read<long>();
            itemData.m_ReqLevel1 = reader.Read<char>();
            itemData.m_ReqLevelType2 = reader.Read<long>();
            itemData.m_ReqLevel2 = reader.Read<char>();
            itemData.m_ReqLevelType3 = reader.Read<long>();
            itemData.m_ReqLevel3 = reader.Read<char>();
            itemData.m_ReqLevelType4 = reader.Read<long>();
            itemData.m_ReqLevel4 = reader.Read<char>();
            itemData.m_MaxContain = reader.Read<long>();
            itemData.m_RegionInfo.RegionID = reader.Read<short>();
            itemData.m_RegionInfo.RegionBlockID = reader.Read<short>();
            itemData.m_RegionInfo.PosX = reader.Read<float>();
            itemData.m_RegionInfo.PosY = reader.Read<float>();
            itemData.m_RegionInfo.PosZ = reader.Read<float>();
            itemData.m_Speed1 = reader.Read<short>();
            itemData.m_Speed2 = reader.Read<short>();
            itemData.m_Scale = reader.Read<long>();
            itemData.m_BCHeight = reader.Read<short>();
            itemData.m_BCRadius = reader.Read<short>();
            itemData.m_EventID = reader.Read<long>();
            reader.ReadString(itemData.m_AssocFileObj128, 128);
            reader.ReadString(itemData.m_AssocFileDrop128, 128);
            reader.ReadString(itemData.m_AssocFileIcon128, 128);
            reader.ReadString(itemData.m_AssocFile1_128, 128);
            reader.ReadString(itemData.m_AssocFile2_128, 128);
            reader.Read<long>(); //RefObjCommon.Link
            reader.Read<long>(); //RefObjItem.ID
            itemData.m_MaxStack = reader.Read<long>();
            itemData.m_ReqGender  = reader.Read<char>();
            itemData.m_ReqStr = reader.Read<long>();
            itemData.m_ReqInt = reader.Read<long>();
            itemData.m_ItemClass = reader.Read<char>();
            itemData.m_SetID = reader.Read<long>();
            itemData.m_Dur_L = reader.Read<float>();
            itemData.m_Dur_U = reader.Read<float>();
            itemData.m_PD_L = reader.Read<float>();
            itemData.m_PD_U = reader.Read<float>();
            itemData.m_PDInc = reader.Read<float>();
            itemData.m_ER_L = reader.Read<float>();
            itemData.m_ER_U = reader.Read<float>();
            itemData.m_ERInc = reader.Read<float>();
            itemData.m_PAR_L = reader.Read<float>();
            itemData.m_PAR_U = reader.Read<float>();
            itemData.m_PARInc = reader.Read<float>();
            itemData.m_BR_L = reader.Read<float>();
            itemData.m_BR_U = reader.Read<float>();
            itemData.m_MD_L = reader.Read<float>();
            itemData.m_MD_U = reader.Read<float>();
            itemData.m_MDInc = reader.Read<float>();
            itemData.m_MAR_L = reader.Read<float>();
            itemData.m_MAR_U = reader.Read<float>();
            itemData.m_MARInc = reader.Read<float>();
            itemData.m_PDStr_L = reader.Read<float>();
            itemData.m_PDStr_U = reader.Read<float>();
            itemData.m_MDInt_L = reader.Read<float>();
            itemData.m_MDInt_U = reader.Read<float>();
            itemData.m_Quivered = reader.Read<bool>();
            itemData.m_Ammo1_TID4 = reader.Read<char>();
            itemData.m_Ammo2_TID4 = reader.Read<char>();
            itemData.m_Ammo3_TID4 = reader.Read<char>();
            itemData.m_Ammo4_TID4 = reader.Read<char>();
            itemData.m_Ammo5_TID4 = reader.Read<char>();
            itemData.m_SpeedClass = reader.Read<char>();
            itemData.m_TwoHanded = reader.Read<bool>();
            itemData.m_Range = reader.Read<short>();
            itemData.m_PAttackMin_L = reader.Read<float>();
            itemData.m_PAttackMin_U = reader.Read<float>();
            itemData.m_PAttackMax_L = reader.Read<float>();
            itemData.m_PAttackMax_U = reader.Read<float>();
            itemData.m_PAttackInc = reader.Read<float>();
            itemData.m_MAttackMin_L = reader.Read<float>();
            itemData.m_MAttackMin_U = reader.Read<float>();
            itemData.m_MAttackMax_L = reader.Read<float>();
            itemData.m_MAttackMax_U = reader.Read<float>();
            itemData.m_MAttackInc = reader.Read<float>();
            itemData.m_PAStrMin_L = reader.Read<float>();
            itemData.m_PAStrMin_U = reader.Read<float>();
            itemData.m_PAStrMax_L = reader.Read<float>();
            itemData.m_PAStrMax_U = reader.Read<float>();
            itemData.m_MAInt_Min_L = reader.Read<float>();
            itemData.m_MAInt_Min_U = reader.Read<float>();
            itemData.m_MAInt_Max_L = reader.Read<float>();
            itemData.m_MAInt_Max_U = reader.Read<float>();
            itemData.m_HR_L = reader.Read<float>();
            itemData.m_HR_U = reader.Read<float>();
            itemData.m_HRInc = reader.Read<float>();
            itemData.m_CHR_L = reader.Read<float>();
            itemData.m_CHR_U = reader.Read<float>();

            for (long i = 0; i < ItemParam::MAX_PARAM; ++i)
            {
                itemData.m_Param[i].Value = reader.Read<long>();
                reader.ReadString(itemData.m_Param[i].Desc, 128);
            }

            itemData.m_MaxMagicOptCount = reader.Read<char>();
            itemData.m_ChildItemCount = reader.Read<char>();

            reader.Read<long>(); //RefObjItem.Link

            m_ItemData[itemData.m_ID] = itemData;
            m_ItemIndex[itemData.m_CodeName128] = itemData.m_ID;
        }

        if (m_ItemData.empty())
        {
            m_pDbMgr->Release(SHARD, pConnection);
            return false;
        }

        //PutLog(FATAL, "HydraFramework::RefDataManager loaded %d RefObjItem entries.", m_ItemData.size());
        m_pDbMgr->Release(SHARD, pConnection);
        return true;
    }

    bool CRefDataManager::InitializeObjStruct()
    {
        CDbRowReader reader;
        CDbConnection *pConnection = m_pDbMgr->GetConnection(SHARD);

        const stra_t &query("SELECT CMN.*, STRUCT.* FROM [dbo].[_RefObjStruct] STRUCT INNER JOIN [dbo].[_RefObjCommon] "
                            "CMN ON CMN.Link = STRUCT.ID WHERE CMN.[Service] = 1 AND CMN.TypeID1 = 4 ORDER BY CMN.ID");

        pConnection->ExecuteReader(reader, query.c_str());

        while (reader.FetchNext())
        {
            reader.Read<long>(); //Service

            RefObjStruct structData;
            structData.m_ID = reader.Read<long>();
            reader.ReadString(structData.m_CodeName128, 128);
            reader.ReadString(structData.m_ObjName128, 128);
            reader.ReadString(structData.m_OrgObjCodeName128, 128);
            reader.ReadString(structData.m_NameStrID128, 128);
            reader.ReadString(structData.m_DescStrID128, 128);
            structData.m_TypeID.SetIsCashItem(reader.Read<bool>());
            structData.m_TypeID.SetIsBionic(reader.Read<bool>());
            structData.m_TypeID.SetFirst(reader.Read<unsigned char>());
            structData.m_TypeID.SetSecond(reader.Read<unsigned char>());
            structData.m_TypeID.SetThird(reader.Read<unsigned char>());
            structData.m_TypeID.SetFourth(reader.Read<unsigned char>());
            //structData.m_CashItem = reader.Read<char>();
            //structData.m_Bionic = reader.Read<char>();
            //structData.m_TypeID1 = reader.Read<char>();
            //structData.m_TypeID2 = reader.Read<char>();
            //structData.m_TypeID3 = reader.Read<char>();
            //structData.m_TypeID4 = reader.Read<char>();
            structData.m_DecayTime = reader.Read<long>();
            structData.m_Country = reader.Read<char>();
            structData.m_Rarity = reader.Read<char>();
            structData.m_CanTrade = reader.Read<char>();
            structData.m_CanSell = reader.Read<char>();
            structData.m_CanBuy = reader.Read<char>();
            structData.m_CanBorrow = reader.Read<char>();
            structData.m_CanDrop = reader.Read<char>();
            structData.m_CanPick = reader.Read<char>();
            structData.m_CanRepair = reader.Read<char>();
            structData.m_CanRevive = reader.Read<char>();
            structData.m_CanUse = reader.Read<char>();
            structData.m_CanThrow = reader.Read<char>();
            structData.m_Price = reader.Read<long>();
            structData.m_CostRepair = reader.Read<long>();
            structData.m_CostRevive = reader.Read<long>();
            structData.m_CostBorrow = reader.Read<long>();
            structData.m_KeepingFee = reader.Read<long>();
            structData.m_SellPrice = reader.Read<long>();
            structData.m_ReqLevelType1 = reader.Read<long>();
            structData.m_ReqLevel1 = reader.Read<char>();
            structData.m_ReqLevelType2 = reader.Read<long>();
            structData.m_ReqLevel2 = reader.Read<char>();
            structData.m_ReqLevelType3 = reader.Read<long>();
            structData.m_ReqLevel3 = reader.Read<char>();
            structData.m_ReqLevelType4 = reader.Read<long>();
            structData.m_ReqLevel4 = reader.Read<char>();
            structData.m_MaxContain = reader.Read<long>();
            structData.m_RegionInfo.RegionID = reader.Read<short>();
            structData.m_RegionInfo.RegionBlockID = reader.Read<short>();
            structData.m_RegionInfo.PosX = reader.Read<float>();
            structData.m_RegionInfo.PosY = reader.Read<float>();
            structData.m_RegionInfo.PosZ = reader.Read<float>();
            structData.m_Speed1 = reader.Read<short>();
            structData.m_Speed2 = reader.Read<short>();
            structData.m_Scale = reader.Read<long>();
            structData.m_BCHeight = reader.Read<short>();
            structData.m_BCRadius = reader.Read<short>();
            structData.m_EventID = reader.Read<long>();
            reader.ReadString(structData.m_AssocFileObj128, 128);
            reader.ReadString(structData.m_AssocFileDrop128, 128);
            reader.ReadString(structData.m_AssocFileIcon128, 128);
            reader.ReadString(structData.m_AssocFile1_128, 128);
            reader.ReadString(structData.m_AssocFile2_128, 128);
            reader.Read<long>(); //RefObjCommon.Link
            reader.Read<long>(); //RefObjStruct.ID
            structData.m_DummyData = reader.Read<long>(); //RefObjStruct.Dummy_Data

            m_StructData[structData.m_ID] = structData;
            m_StructIndex[structData.m_CodeName128] = structData.m_ID;
        }

        if (m_ItemData.empty())
        {
            m_pDbMgr->Release(SHARD, pConnection);
            return false;
        }

        //PutLog(FATAL, "HydraFramework::RefDataManager loaded %d RefObjStruct entries.", m_StructData.size());
        m_pDbMgr->Release(SHARD, pConnection);
        return true;
    }

    bool CRefDataManager::InitializeSkillData()
    {
        CDbRowReader reader;
        CDbConnection *pConnection = m_pDbMgr->GetConnection(SHARD);

        const stra_t &query("SELECT * FROM [dbo].[_RefSkill] WHERE [Service] = 1 ORDER BY ID");

        pConnection->ExecuteReader(reader, query.c_str());

        while (reader.FetchNext())
        {
            RefSkill skillData;
            reader.Read<long>(); //Service
            skillData.m_ID = reader.Read<long>();
            skillData.m_GroupID = reader.Read<long>();
            reader.ReadString(skillData.m_Basic_Code, 128);
            reader.ReadString(skillData.m_Basic_Name, 128);
            reader.ReadString(skillData.m_Basic_Group, 128);
            skillData.m_Basic_Original = reader.Read<long>();
            skillData.m_Basic_Level = reader.Read<char>();
            skillData.m_Basic_Activity = reader.Read<char>();
            skillData.m_Basic_ChainCode = reader.Read<long>();
            skillData.m_Basic_RecycleCost = reader.Read<long>();
            skillData.m_Action_PreparingTime = reader.Read<long>();
            skillData.m_Action_CastingTime = reader.Read<long>();
            skillData.m_Action_ActionDuration = reader.Read<long>();
            skillData.m_Action_ReuseDelay = reader.Read<long>();
            skillData.m_Action_CoolTime = reader.Read<long>();
            skillData.m_Action_FlyingSpeed = reader.Read<long>();
            skillData.m_Action_Interruptable = reader.Read<char>();
            skillData.m_Action_Overlap = reader.Read<long>();
            skillData.m_Action_AutoAttackType = reader.Read<char>();
            skillData.m_Action_InTown = reader.Read<char>();
            skillData.m_Action_Range = reader.Read<short>();
            skillData.m_Target_Required = reader.Read<char>();
            skillData.m_TargetType_Animal = reader.Read<char>();
            skillData.m_TargetType_Land = reader.Read<char>();
            skillData.m_TargetType_Building = reader.Read<char>();
            skillData.m_TargetGroup_Self = reader.Read<char>();
            skillData.m_TargetGroup_Ally = reader.Read<char>();
            skillData.m_TargetGroup_Party = reader.Read<char>();
            skillData.m_TargetGroup_Enemy_M = reader.Read<char>();
            skillData.m_TargetGroup_Enemy_P = reader.Read<char>();
            skillData.m_TargetGroup_Neutral = reader.Read<char>();
            skillData.m_TargetGroup_DontCare = reader.Read<char>();
            skillData.m_TargetEtc_SelectDeadBody = reader.Read<char>();
            skillData.m_ReqCommon_Mastery1 = reader.Read<long>();
            skillData.m_ReqCommon_Mastery2 = reader.Read<long>();
            skillData.m_ReqCommon_MasteryLevel1 = reader.Read<char>();
            skillData.m_ReqCommon_MasteryLevel2 = reader.Read<char>();
            skillData.m_ReqCommon_Str = reader.Read<short>();
            skillData.m_ReqCommon_Int = reader.Read<short>();
            skillData.m_ReqLearn_Skill1 = reader.Read<long>();
            skillData.m_ReqLearn_Skill2 = reader.Read<long>();
            skillData.m_ReqLearn_Skill3 = reader.Read<long>();
            skillData.m_ReqLearn_SkillLevel1 = reader.Read<char>();
            skillData.m_ReqLearn_SkillLevel2 = reader.Read<char>();
            skillData.m_ReqLearn_SkillLevel3 = reader.Read<char>();
            skillData.m_ReqLearn_SP = reader.Read<long>();
            skillData.m_ReqLearn_Race = reader.Read<char>();
            skillData.m_Req_Restriction1 = reader.Read<char>();
            skillData.m_Req_Restriction2 = reader.Read<char>();
            skillData.m_ReqCast_Weapon1 = reader.Read<char>();
            skillData.m_ReqCast_Weapon2 = reader.Read<char>();
            skillData.m_Consume_HP = reader.Read<short>();
            skillData.m_Consume_MP = reader.Read<long>();
            skillData.m_Consume_HPRatio = reader.Read<short>();
            skillData.m_Consume_MPRatio = reader.Read<short>();
            skillData.m_Consume_WHAN = reader.Read<char>();
            skillData.m_UI_SkillTab = reader.Read<char>();
            skillData.m_UI_SkillPage = reader.Read<char>();
            skillData.m_UI_SkillColumn = reader.Read<char>();
            skillData.m_UI_SkillRow = reader.Read<char>();
            reader.ReadString(skillData.m_UI_IconFile, 128);
            reader.ReadString(skillData.m_UI_SkillName, 128);
            reader.ReadString(skillData.m_UI_SkillToolTip, 128);
            reader.ReadString(skillData.m_UI_SkillToolTip_Desc, 128);
            reader.ReadString(skillData.m_UI_SkillStudy_Desc, 128);
            skillData.m_AI_AttackChance = reader.Read<short>();
            skillData.m_AI_SkillType = reader.Read<char>();

            for (long i = 0; i < SkillParam::MAX_SKILL_PARAM; ++i)
                skillData.m_Param[i].Value = reader.Read<long>();

            m_SkillData[skillData.m_ID] = skillData;
            m_SkillIndex[skillData.m_Basic_Code] = skillData.m_ID;
        }

        if (m_SkillData.empty())
        {
            m_pDbMgr->Release(SHARD, pConnection);
            return false;
        }

        //PutLog(FATAL, "HydraFramework::RefDataManager loaded %d RefSkill entries.", m_SkillData.size());
        m_pDbMgr->Release(SHARD, pConnection);
        return true;
    }

    bool CRefDataManager::InitializeLevel()
    {
        CDbRowReader reader;
        CDbConnection *pConnection;

        pConnection = m_pDbMgr->GetConnection(SHARD);

        std::string query("SELECT * FROM [dbo].[_RefLevel]");

        pConnection->ExecuteReader(reader, query.c_str());

        while (reader.FetchNext())
        {
            RefLevel levelData;
            levelData.m_Lvl = reader.Read<BYTE>();
            levelData.m_Exp_C = reader.Read<INT64>();
            levelData.m_Exp_M = reader.Read<long>();
            levelData.m_Cost_M = reader.Read<long>();
            levelData.m_Cost_ST = reader.Read<long>();
            levelData.m_GUST_Mob_Exp = reader.Read<long>();
            levelData.m_JobExp_Trader = reader.Read<long>();
            levelData.m_JobExp_Robber = reader.Read<long>();
            levelData.m_JobExp_Hunter = reader.Read<long>();
            levelData.m_Exp_P = reader.Read<INT64>();
            levelData.m_MaxSP = reader.Read<long>();
            m_LevelData[levelData.m_Lvl] = levelData;
        }

        if (m_LevelData.empty())
        {
            m_pDbMgr->Release(SHARD, pConnection);
            return false;
        }

        //PutLog(FATAL, "HydraFramework::RefDataManager loaded %d RefLevel entries.", m_LevelData.size());
        m_pDbMgr->Release(SHARD, pConnection);
        return true;
    }

    RefObjChar *CRefDataManager::GetRefCharData(DWORD dwID)
    {
        ACS_SCOPED_LOCK(m_cs);
        if (m_CharData.find(dwID) == m_CharData.end())
            return NULL;
        return &m_CharData[dwID];
    }

    RefObjChar *CRefDataManager::GetRefCharData(const char* codename)
    {
        ACS_SCOPED_LOCK(m_cs);
        CharIndex::iterator it = m_CharIndex.find(codename);
        if (it == m_CharIndex.end())
            return NULL;
        return &m_CharData[it->second];
    }

    RefObjItem *CRefDataManager::GetRefItemData(DWORD dwID)
    {
        ACS_SCOPED_LOCK(m_cs);
        if (m_ItemData.find(dwID) == m_ItemData.end())
            return NULL;
        return &m_ItemData[dwID];
    }

    RefObjItem *CRefDataManager::GetRefItemData(const char* codename)
    {
        ACS_SCOPED_LOCK(m_cs);
        ItemIndex::iterator it = m_ItemIndex.find(codename);
        if (it == m_ItemIndex.end())
            return NULL;
        return &m_ItemData[it->second];
    }

    RefObjStruct *CRefDataManager::GetRefStructData(DWORD dwID)
    {
        ACS_SCOPED_LOCK(m_cs);
        if (m_StructData.find(dwID) == m_StructData.end())
            return NULL;
        return &m_StructData[dwID];
    }

    RefObjStruct *CRefDataManager::GetRefStructData(const char* codename)
    {
        ACS_SCOPED_LOCK(m_cs);
        StructIndex::iterator it = m_StructIndex.find(codename);
        if (it == m_StructIndex.end())
            return NULL;
        return &m_StructData[it->second];
    }

    RefSkill *CRefDataManager::GetRefSkillData(DWORD dwID)
    {
        ACS_SCOPED_LOCK(m_cs);
        if (m_SkillData.find(dwID) == m_SkillData.end())
            return NULL;
        return &m_SkillData[dwID];
    }

    RefSkill *CRefDataManager::GetRefSkillData(const char* codename)
    {
        ACS_SCOPED_LOCK(m_cs);
        SkillIndex::iterator it = m_SkillIndex.find(codename);
        if (it == m_SkillIndex.end())
            return NULL;
        return &m_SkillData[it->second];
    }

    RefLevel *CRefDataManager::GetLevelData(BYTE btLvl)
    {
        ACS_SCOPED_LOCK(m_cs);
        if (m_LevelData.find(btLvl) == m_LevelData.end())
            return NULL;
        return &m_LevelData[btLvl];
    }

    bool CRefDataManager::IsNonSplitItem(DWORD dwRefItemID)
    {
        ACS_SCOPED_LOCK(m_cs);
        if (m_NonSplitItem.empty())
        {
            PutLog(FATAL, "%s is called but m_NonSplitItem is empty.");
        }
        return m_NonSplitItem.find(dwRefItemID) != m_NonSplitItem.end();
    }

    void CRefDataManager::CleanUp()
    {
        m_CharData.clear();
        m_CharIndex.clear();
        m_ItemData.clear();
        m_ItemIndex.clear();
        m_StructData.clear();
        m_StructIndex.clear();
        m_SkillData.clear();
        m_SkillIndex.clear();
        m_NonSplitItem.clear();
    }
}