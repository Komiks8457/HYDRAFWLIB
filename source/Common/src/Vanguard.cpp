#include "Vanguard.h"

template<> CVanguard* CSingletonT<CVanguard>::s_pObject = NULL;

CVanguard::CVanguard() : m_dllPath(L"")
{}

CVanguard::~CVanguard()
{}
