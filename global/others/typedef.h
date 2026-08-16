#pragma once

#include "structx.h"

typedef std::string  stra_t;
typedef std::wstring strw_t;

#define ASTR std::string
#define WSTR std::wstring

#define LPCSTRA const std::string*
#define LPRSTRA const std::string&

#define LPCSTRW const std::wstring*
#define LPRSTRW const std::wstring&

// UPCASTING
#define UCAST(ptr, x) static_cast<ptr>(x)

// DOWNCASTING
#define DCAST(ptr, x) dynamic_cast<ptr>(x)

#define NOINLINE __declspec(noinline)

typedef std::map<std::string, std::string> query_map_t;

typedef std::map<std::string, std::list<BYTE>> ServerBodyList;
typedef ServerBodyList::iterator ServerBodyListIterator;
