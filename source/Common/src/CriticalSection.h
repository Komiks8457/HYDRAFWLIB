#pragma once

#ifndef CRITICALSECTION_H
#define CRITICALSECTION_H

#include "pch.h"

class CCriticalSection {
public:
    CCriticalSection(DWORD dwSpinCount = 4000) { InitializeCriticalSectionAndSpinCount(&m_cs, dwSpinCount); }
    ~CCriticalSection() { DeleteCriticalSection(&m_cs); }

    __forceinline void Enter() { EnterCriticalSection(&m_cs); }
    __forceinline void Leave() { LeaveCriticalSection(&m_cs); }

private:
    CCriticalSection(const CCriticalSection&);
    CCriticalSection& operator=(const CCriticalSection&);
    CRITICAL_SECTION m_cs;
};

class CCriticalSectionScoped {
public:
    explicit CCriticalSectionScoped(CCriticalSection& cs) : m_cs(cs) {  m_cs.Enter(); }
    ~CCriticalSectionScoped() { m_cs.Leave(); }

private:
    CCriticalSectionScoped(const CCriticalSectionScoped&);
    CCriticalSectionScoped& operator=(const CCriticalSectionScoped&);
    CCriticalSection& m_cs;
};

#define ACS_SCOPED_LOCK(cs) CCriticalSectionScoped cslock(cs)

#endif