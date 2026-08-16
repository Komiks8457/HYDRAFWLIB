#pragma once

namespace HydraFramework
{
    struct SkillParam
    {
        enum Enum {
            Param1,
            Param2,
            Param3,
            Param4,
            Param5,
            Param6,
            Param7,
            Param8,
            Param9,
            Param10,
            Param11,
            Param12,
            Param13,
            Param14,
            Param15,
            Param16,
            Param17,
            Param18,
            Param19,
            Param20,
            Param21,
            Param22,
            Param23,
            Param24,
            Param25,
            Param26,
            Param27,
            Param28,
            Param29,
            Param30,
            Param31,
            Param32,
            Param33,
            Param34,
            Param35,
            Param36,
            Param37,
            Param38,
            Param39,
            Param40,
            Param41,
            Param42,
            Param43,
            Param44,
            Param45,
            Param46,
            Param47,
            Param48,
            Param49,
            Param50,
            MAX_SKILL_PARAM
        };

        long Value;
        SkillParam() : Value(0)
        {}
    };

    struct RefSkill {
        friend class CRefDataManager;

    private:
        long m_ID;
        long m_GroupID;
        char m_Basic_Code[128];
        char m_Basic_Name[128];
        char m_Basic_Group[128];
        long m_Basic_Original;
        char m_Basic_Level;
        char m_Basic_Activity;
        long m_Basic_ChainCode;
        long m_Basic_RecycleCost;
        long m_Action_PreparingTime;
        long m_Action_CastingTime;
        long m_Action_ActionDuration;
        long m_Action_ReuseDelay;
        long m_Action_CoolTime;
        long m_Action_FlyingSpeed;
        char m_Action_Interruptable;
        long m_Action_Overlap;
        char m_Action_AutoAttackType;
        char m_Action_InTown;
        short m_Action_Range;
        char m_Target_Required;
        char m_TargetType_Animal;
        char m_TargetType_Land;
        char m_TargetType_Building;
        char m_TargetGroup_Self;
        char m_TargetGroup_Ally;
        char m_TargetGroup_Party;
        char m_TargetGroup_Enemy_M;
        char m_TargetGroup_Enemy_P;
        char m_TargetGroup_Neutral;
        char m_TargetGroup_DontCare;
        char m_TargetEtc_SelectDeadBody;
        long m_ReqCommon_Mastery1;
        long m_ReqCommon_Mastery2;
        char m_ReqCommon_MasteryLevel1;
        char m_ReqCommon_MasteryLevel2;
        short m_ReqCommon_Str;
        short m_ReqCommon_Int;
        long m_ReqLearn_Skill1;
        long m_ReqLearn_Skill2;
        long m_ReqLearn_Skill3;
        char m_ReqLearn_SkillLevel1;
        char m_ReqLearn_SkillLevel2;
        char m_ReqLearn_SkillLevel3;
        long m_ReqLearn_SP;
        char m_ReqLearn_Race;
        char m_Req_Restriction1;
        char m_Req_Restriction2;
        char m_ReqCast_Weapon1;
        char m_ReqCast_Weapon2;
        short m_Consume_HP;
        long m_Consume_MP;
        short m_Consume_HPRatio;
        short m_Consume_MPRatio;
        char m_Consume_WHAN;
        char m_UI_SkillTab;
        char m_UI_SkillPage;
        char m_UI_SkillColumn;
        char m_UI_SkillRow;
        char m_UI_IconFile[128];
        char m_UI_SkillName[128];
        char m_UI_SkillToolTip[128];
        char m_UI_SkillToolTip_Desc[128];
        char m_UI_SkillStudy_Desc[128];
        short m_AI_AttackChance;
        char m_AI_SkillType;
        SkillParam m_Param[SkillParam::MAX_SKILL_PARAM];

    public:
        RefSkill() : m_ID(0), m_GroupID(0), m_Basic_Original(0), m_Basic_Level(0), m_Basic_Activity(0),
                     m_Basic_ChainCode(0), m_Basic_RecycleCost(0), m_Action_PreparingTime(0), m_Action_CastingTime(0),
                     m_Action_ActionDuration(0) ,m_Action_ReuseDelay(0) ,m_Action_CoolTime(0) ,m_Action_FlyingSpeed(0),
                     m_Action_Interruptable(0) ,m_Action_Overlap(0) ,m_Action_AutoAttackType(0) ,m_Action_InTown(0),
                     m_Action_Range(0), m_Target_Required(0), m_TargetType_Animal(0), m_TargetType_Land(0),
                     m_TargetType_Building(0), m_TargetGroup_Self(0), m_TargetGroup_Ally(0), m_TargetGroup_Party(0),
                     m_TargetGroup_Enemy_M(0), m_TargetGroup_Enemy_P(0), m_TargetGroup_Neutral(0),
                     m_TargetGroup_DontCare(0), m_TargetEtc_SelectDeadBody(0), m_ReqCommon_Mastery1(0),
                     m_ReqCommon_Mastery2(0), m_ReqCommon_MasteryLevel1(0), m_ReqCommon_MasteryLevel2(0),
                     m_ReqCommon_Str(0), m_ReqCommon_Int(0), m_ReqLearn_Skill1(0), m_ReqLearn_Skill2(0),
                     m_ReqLearn_Skill3(0), m_ReqLearn_SkillLevel1(0), m_ReqLearn_SkillLevel2(0),
                     m_ReqLearn_SkillLevel3(0), m_ReqLearn_SP(0), m_ReqLearn_Race(0), m_Req_Restriction1(0),
                     m_Req_Restriction2(0), m_ReqCast_Weapon1(0), m_ReqCast_Weapon2(0), m_Consume_HP(0),
                     m_Consume_MP(0), m_Consume_HPRatio(0), m_Consume_MPRatio(0), m_Consume_WHAN(0),
                     m_UI_SkillTab(0), m_UI_SkillPage(0), m_UI_SkillColumn(0), m_UI_SkillRow(0),
                     m_AI_AttackChance(0), m_AI_SkillType(0)
        {
            ZeroMemory(m_Basic_Code, sizeof(m_Basic_Code));
            ZeroMemory(m_Basic_Name, sizeof(m_Basic_Name));
            ZeroMemory(m_Basic_Group, sizeof(m_Basic_Group));
            ZeroMemory(m_UI_IconFile, sizeof(m_UI_IconFile));
            ZeroMemory(m_UI_SkillName, sizeof(m_UI_SkillName));
            ZeroMemory(m_UI_SkillToolTip, sizeof(m_UI_SkillToolTip));
            ZeroMemory(m_UI_SkillToolTip_Desc, sizeof(m_UI_SkillToolTip_Desc));
            ZeroMemory(m_UI_SkillStudy_Desc, sizeof(m_UI_SkillStudy_Desc));
        }

        long SkillID() const { return m_ID; }
        long GroupID() const { return m_GroupID; }
        const char* Basic_Code() const { return m_Basic_Code; }
        const char* Basic_Name() const { return m_Basic_Name; }
        const char* Basic_Group() const { return m_Basic_Group; }
        long Basic_Original() const { return m_Basic_Original; }
        char Basic_Level() const { return m_Basic_Level; }
        char Basic_Activity() const { return m_Basic_Activity; }
        long Basic_ChainCode() const { return m_Basic_ChainCode; }
        long Basic_RecycleCost() const { return m_Basic_RecycleCost; }
        long Action_PreparingTime() const { return m_Action_PreparingTime; }
        long Action_CastingTime() const { return m_Action_CastingTime; }
        long Action_ActionDuration() const { return m_Action_ActionDuration; }
        long Action_ReuseDelay() const { return m_Action_ReuseDelay; }
        long Action_CoolTime() const { return m_Action_CoolTime; }
        long Action_FlyingSpeed() const { return m_Action_FlyingSpeed; }
        char Action_Interruptable() const { return m_Action_Interruptable; }
        long Action_Overlap() const { return m_Action_Overlap; }
        char Action_AutoAttackType() const { return m_Action_AutoAttackType; }
        char Action_InTown() const { return m_Action_InTown; }
        short Action_Range() const { return m_Action_Range; }
        char Target_Required() const { return m_Target_Required; }
        char TargetType_Animal() const { return m_TargetType_Animal; }
        char TargetType_Land() const { return m_TargetType_Land; }
        char TargetType_Building() const { return m_TargetType_Building; }
        char TargetGroup_Self() const { return m_TargetGroup_Self; }
        char TargetGroup_Ally() const { return m_TargetGroup_Ally; }
        char TargetGroup_Party() const { return m_TargetGroup_Party; }
        char TargetGroup_Enemy_M() const { return m_TargetGroup_Enemy_M; }
        char TargetGroup_Enemy_P() const { return m_TargetGroup_Enemy_P; }
        char TargetGroup_Neutral() const { return m_TargetGroup_Neutral; }
        char TargetGroup_DontCare() const { return m_TargetGroup_DontCare; }
        char TargetEtc_SelectDeadBody() const { return m_TargetEtc_SelectDeadBody; }
        long ReqCommon_Mastery1() const { return m_ReqCommon_Mastery1; }
        long ReqCommon_Mastery2() const { return m_ReqCommon_Mastery2; }
        char ReqCommon_MasteryLevel1() const { return m_ReqCommon_MasteryLevel1; }
        char ReqCommon_MasteryLevel2() const { return m_ReqCommon_MasteryLevel2; }
        short ReqCommon_Str() const { return m_ReqCommon_Str; }
        short ReqCommon_Int() const { return m_ReqCommon_Int; }
        long ReqLearn_Skill1() const { return m_ReqLearn_Skill1; }
        long ReqLearn_Skill2() const { return m_ReqLearn_Skill2; }
        long ReqLearn_Skill3() const { return m_ReqLearn_Skill3; }
        char ReqLearn_SkillLevel1() const { return m_ReqLearn_SkillLevel1; }
        char ReqLearn_SkillLevel2() const { return m_ReqLearn_SkillLevel2; }
        char ReqLearn_SkillLevel3() const { return m_ReqLearn_SkillLevel3; }
        long ReqLearn_SP() const { return m_ReqLearn_SP; }
        char ReqLearn_Race() const { return m_ReqLearn_Race; }
        char Req_Restriction1() const { return m_Req_Restriction1; }
        char Req_Restriction2() const { return m_Req_Restriction2; }
        char ReqCast_Weapon1() const { return m_ReqCast_Weapon1; }
        char ReqCast_Weapon2() const { return m_ReqCast_Weapon2; }
        short Consume_HP() const { return m_Consume_HP; }
        long Consume_MP() const { return m_Consume_MP; }
        short Consume_HPRatio() const { return m_Consume_HPRatio; }
        short Consume_MPRatio() const { return m_Consume_MPRatio; }
        char Consume_WHAN() const { return m_Consume_WHAN; }
        char UI_SkillTab() const { return m_UI_SkillTab; }
        char UI_SkillPage() const { return m_UI_SkillPage; }
        char UI_SkillColumn() const { return m_UI_SkillColumn; }
        char UI_SkillRow() const { return m_UI_SkillRow; }
        const char* UI_IconFile() const { return m_UI_IconFile; }
        const char* UI_SkillName() const { return m_UI_SkillName; }
        const char* UI_SkillToolTip() const { return m_UI_SkillToolTip; }
        const char* UI_SkillToolTip_Desc() const { return m_UI_SkillToolTip_Desc; }
        const char* UI_SkillStudy_Desc() const { return m_UI_SkillStudy_Desc; }
        short AI_AttackChance() const { return m_AI_AttackChance; }
        char AI_SkillType() const { return m_AI_SkillType; }

        long Param(SkillParam::Enum skillParam) const
        {
            if (skillParam >= SkillParam::MAX_SKILL_PARAM)
                return NULL;
            return m_Param[skillParam].Value;
        }

        bool HasParam(long nParam) const
        {
            for (long i = 0; i < SkillParam::MAX_SKILL_PARAM; ++i)
            {
                if (m_Param[i].Value == nParam)
                    return true;
            }

            return false;
        }
    };
}
