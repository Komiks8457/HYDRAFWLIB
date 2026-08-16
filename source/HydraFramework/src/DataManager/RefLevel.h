#pragma once

namespace HydraFramework
{
    struct RefLevel {
        friend class CRefDataManager;

    private:
        BYTE m_Lvl;
        uint64_t m_Exp_C;
        long m_Exp_M;
        long m_Cost_M;
        long m_Cost_ST;
        long m_GUST_Mob_Exp;
        long m_JobExp_Trader;
        long m_JobExp_Robber;
        long m_JobExp_Hunter;
        uint64_t m_Exp_P;
        long m_MaxSP;

    public:
        RefLevel() : m_Lvl(0), m_Exp_C(0), m_Exp_M(0), m_Cost_M(0), m_Cost_ST(0), m_GUST_Mob_Exp(0),
                         m_JobExp_Trader(0), m_JobExp_Robber(0), m_JobExp_Hunter(0), m_Exp_P(0), m_MaxSP(0)
        {}

        BYTE Lvl() const { return m_Lvl; }

        uint64_t Exp_C() const { return m_Exp_C; }

        long Exp_M() const { return m_Exp_M; }

        long Cost_M() const { return m_Cost_M; }

        long Cost_ST() const { return m_Cost_ST; }

        long GUST_Mob_Exp() const { return m_GUST_Mob_Exp; }

        long JobExp_Trader() const { return m_JobExp_Trader; }

        long JobExp_Robber() const { return m_JobExp_Robber; }

        long JobExp_Hunter() const { return m_JobExp_Hunter; }

        uint64_t Exp_P() const { return m_Exp_P; }

        long MaxSP() const { return m_MaxSP; }
    };
}
