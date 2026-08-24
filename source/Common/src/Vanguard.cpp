#include "Vanguard.h"
#include "AppLogger.h"

template<> CVanguard* CSingletonT<CVanguard>::s_pObject = NULL;

CVanguard::CVanguard() : m_dllPath(L""), m_hDll(NULL)
{}

bool CVanguard::InitDll()
{
    if (m_dllPath == L"") {
        PutLog(FATAL, "%s No dll path provided", __FUNCTION__);
        return false;
    }

    m_hDll = LoadLibraryW(m_dllPath.c_str());
    return m_hDll ? true : false;
}

CVanguard::~CVanguard()
{}
