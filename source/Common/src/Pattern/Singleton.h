#pragma once

#include "pch.h"

template <class T>
class CSingletonT
{
    static T* s_pObject;

public:
    CSingletonT()
    {
        _ASSERT( s_pObject == NULL );
        ULONG_PTR offset = (ULONG_PTR)(T*)1 - (ULONG_PTR)(CSingletonT<T>*)(T*)1;
        s_pObject = (T*)((ULONG_PTR)this + offset);
    }
    virtual ~CSingletonT()
    {
        s_pObject = 0;
    }

    static T& GetSingleton()
    {
        _ASSERT(s_pObject);
        return (*s_pObject);
    }

    static T* GetSingletonPtr()
    {
        return s_pObject;
    }
};
