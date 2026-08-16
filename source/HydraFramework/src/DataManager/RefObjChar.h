#pragma once

#include "RefObjCommon.h"

namespace HydraFramework
{
    struct Resist
    {
        enum ENUM_RESIST_TYPE
        {
            Frozen,
            Frostbite,
            Burn,
            EShock,
            Poison,
            Zombie,
            Doze,
            Root,
            Slow,
            Fear,
            Myopia,
            Blood,
            Stone,
            Dark,
            Stun,
            Disease,
            Chaos,
            CsePD,
            CseMD,
            CseSTR,
            CseINT,
            CseHP,
            CseMP,
            Resist24,
            Bomb,
            Resist26,
            Resist27,
            Resist28,
            Resist29,
            Resist30,
            Resist31,
            Resist32,
            MAX_RESIST_TYPE
        };

        long Value;
        Resist() : Value(0)
        {}
    };

    struct DefaultSkill
    {
        enum ENUM_DEFAULT_SKILL
        {
            Skill1,
            Skill2,
            Skill3,
            Skill4,
            Skill5,
            Skill6,
            Skill7,
            Skill8,
            Skill9,
            Skill10,
            MAX_DEFAULT_SKILL
        };

        long Value;
        DefaultSkill() : Value(0)
        {}
    };

    struct Except
    {
        enum ENUM_EXCEPT_TYPE
        {
            Except1,
            Except2,
            Except3,
            Except4,
            Except5,
            Except6,
            Except7,
            Except8,
            Except9,
            Except10,
            MAX_EXCEPT_TYPE
        };

        long Value;
        Except() : Value(0)
        {}
    };

    struct RefObjChar: RefObjCommon {
        friend class CRefDataManager;

    private:
        char m_Lvl;
        char m_CharGender;
        long m_MaxHP;
        long m_MaxMP;
        Resist m_Resist[Resist::MAX_RESIST_TYPE];
        char m_InventorySize;
        char m_CanStore_TID1;
        char m_CanStore_TID2;
        char m_CanStore_TID3;
        char m_CanStore_TID4;
        bool m_CanBeVehicle;
        bool m_CanControl;
        char m_DamagePortion;
        short m_MaxPassenger;
        long m_AssocTactics;
        long m_PD;
        long m_MD;
        long m_PAR;
        long m_MAR;
        long m_ER;
        long m_BR;
        long m_HR;
        long m_CHR;
        long m_ExpToGive;
        long m_CreepType;
        bool m_Knockdown;
        long m_KO_RecoverTime;
        DefaultSkill m_DefaultSkill[DefaultSkill::MAX_DEFAULT_SKILL];
        char m_TextureType;
        Except m_Except[Except::MAX_EXCEPT_TYPE];


    public:
        RefObjChar() : m_Lvl(0), m_CharGender(0), m_MaxHP(0), m_MaxMP(0), m_InventorySize(0),
                       m_CanStore_TID1(0), m_CanStore_TID2(0), m_CanStore_TID3(0), m_CanStore_TID4(0),
                       m_CanBeVehicle(0), m_CanControl(0), m_DamagePortion(0), m_MaxPassenger(0), m_AssocTactics(0),
                       m_PD(0), m_MD(0), m_PAR(0), m_MAR(0), m_ER(0), m_BR(0), m_HR(0), m_CHR(0), m_ExpToGive(0),
                       m_CreepType(0), m_Knockdown(0), m_KO_RecoverTime(0), m_TextureType(0)
        {}

        char Lvl() const { return m_Lvl; }

        char CharGender() const { return m_CharGender; }

        long MaxHP() const { return m_MaxHP; }

        long MaxMP() const { return m_MaxMP; }

        long Resist(Resist::ENUM_RESIST_TYPE resistType) const
        {
            if (resistType >= Resist::MAX_RESIST_TYPE)
                return NULL;
            return m_Resist[resistType].Value;
        }

        char InventorySize() const { return m_InventorySize; }

        char CanStore_TID1() const { return m_CanStore_TID1; }

        char CanStore_TID2() const { return m_CanStore_TID2; }

        char CanStore_TID3() const { return m_CanStore_TID3; }

        char CanStore_TID4() const { return m_CanStore_TID4; }

        bool CanBeVehicle() const { return m_CanBeVehicle; }

        bool CanControl() const { return m_CanControl; }

        char DamagePortion() const { return m_DamagePortion; }

        short MaxPassenger() const { return m_MaxPassenger; }

        long AssocTactics() const { return m_AssocTactics; }

        long PD() const { return m_PD; }

        long MD() const { return m_MD; }

        long PAR() const { return m_PAR; }

        long MAR() const { return m_MAR; }

        long ER() const { return m_ER; }

        long BR() const { return m_BR; }

        long HR() const { return m_HR; }

        long CHR() const { return m_CHR; }

        long ExpToGive() const { return m_ExpToGive; }

        long CreepType() const { return m_CreepType; }

        bool Knockdown() const { return m_Knockdown; }

        long KO_RecoverTime() const { return m_KO_RecoverTime; }

        long DefaultSkill(DefaultSkill::ENUM_DEFAULT_SKILL defaultSkill) const
        {
            if (defaultSkill >= DefaultSkill::MAX_DEFAULT_SKILL)
                return NULL;
            return m_DefaultSkill[defaultSkill].Value;
        }

        char TextureType() const { return m_TextureType; }

        long Except(Except::ENUM_EXCEPT_TYPE exceptType) const
        {
            if (exceptType >= Except::MAX_EXCEPT_TYPE)
                return NULL;
            return m_Except[exceptType].Value;
        }
    };
}
