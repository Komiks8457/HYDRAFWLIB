#pragma once

#include "Pattern/Singleton.h"

class CVanguard : public CSingletonT<CVanguard> {
        friend class CSingletonT<CVanguard>;
public:
    static bool CreateInstance()
    {
        if (GetSingletonPtr())
            return false;

        static CVanguard init;
        return true;
    }

    std::wstring m_dllPath;
private:
    CVanguard();
    CVanguard(const CVanguard&);
    CVanguard& operator=(const CVanguard&);
    virtual ~CVanguard();
};

#define g_pVanguard (&CVanguard::GetSingleton())
