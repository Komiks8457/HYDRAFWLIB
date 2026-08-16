#pragma once

namespace HydraFramework
{
    struct RefNonSplitItem {
        friend class CRefDataManager;
    private:
        long m_RefItemID;
    public:
        RefNonSplitItem() : m_RefItemID(0) {}
        long RefItemID() const { return m_RefItemID; }
    };
}

