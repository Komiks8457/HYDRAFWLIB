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

    bool InitDll();
    void SetDllPath(const wchar_t* dllPath) { m_dllPath = dllPath; }
    HANDLE GetHandle() const { return m_hDll; }
private:
    CVanguard();
    CVanguard(const CVanguard&);
    CVanguard& operator=(const CVanguard&);
    virtual ~CVanguard();

    std::wstring m_dllPath;
    HANDLE m_hDll;
};

#define g_pVanguard (&CVanguard::GetSingleton())
