#pragma once

#include "RefObjCommon.h"

namespace HydraFramework
{
    struct RefObjStruct : RefObjCommon {
        friend class CRefDataManager;

    public:
        long m_DummyData;

    };
}
