#pragma once

#ifndef FRAMEWORK_H
#define FRAMEWORK_H

// 1. System/Build configurations
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef CRT_SECURE_NO_WARNINGS
#define CRT_SECURE_NO_WARNINGS
#endif

// 2. Winsock2 MUST be included BEFORE Windows.h
#include <winsock2.h>
#include <mswsock.h>
#pragma comment(lib, "ws2_32.lib")

// 3. Windows Basics
#include <Windows.h>
#include <tchar.h>
#include <intrin.h>

// 4. C Standard Library
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cwchar>
#include <cctype>
#include <cwctype>
#include <ctime>
#include <cassert>

// 5. C++ Standard Library
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <deque>
#include <queue>
#include <list>
#include <set>
#include <map>
#include <hash_map>
#include <algorithm>
#include <functional>
#include <utility>
#include <iterator>
#include <memory>
#include <limits>
#include <valarray>

#endif