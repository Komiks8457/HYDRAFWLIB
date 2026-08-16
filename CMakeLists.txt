cmake_minimum_required(VERSION 3.27)

# Project name and language
project(HYDRAFWLIB LANGUAGES C CXX)

# 1. C++98 Standards Enforcement (VC80 is C++98 by default)
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 2. Global Compiler and Linker Logic
if(MSVC)
    # /EHa: Async Exception Handling
    string(REPLACE "/EHsc" "/EHa" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    string(REPLACE "/EHs" "/EHa" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

    # /Zc:wchar_t-: Common for older VS2005 projects where wchar_t is a typedef, not a native type
    # /Zc:forScope: Ensure standard-compliant for-loop scoping
    add_compile_options(/Zc:wchar_t- /Zc:forScope)

    # Target x86 architecture specifically
    add_compile_options(/arch:SSE2)

    # Disable CRT deprecation warnings (Essential for VC80)
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
    add_definitions(-D_CRT_NON_CONFORMING_SWPRINTFS)
    add_definitions(-D_USE_32BIT_TIME_T)
    add_definitions(-DWIN32_LEAN_AND_MEAN)

    # IMPORTANT: Since the host EXE has NO MSVCR80.dll import,
    # it is likely statically linked (/MT).
    # We will use /MD for the DLL to keep it standard, but we must embed the manifest.
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
    set(CMAKE_MSVC_MANIFEST ON)

    # Target Windows 7 (0x0601)
    add_definitions(-DWINVER=0x0601)
    add_definitions(-D_WIN32_WINNT=0x0601)
endif()

# 3. Include and Link Directories
# Removed $ENV usage to let the CMake Generator handle VS2005 system paths
include_directories(
        $ENV{INCLUDE}
        "${PROJECT_SOURCE_DIR}/global/_pch"
        "${PROJECT_SOURCE_DIR}/global/others"
        "${PROJECT_SOURCE_DIR}/global/utils"
        "${PROJECT_SOURCE_DIR}/global/dxsdk/Include"
        "${PROJECT_SOURCE_DIR}/global/detours/Include"
        "${PROJECT_SOURCE_DIR}/global/vmprotect/Include"
)

link_directories(
        $ENV{LIB}
        "${PROJECT_SOURCE_DIR}/global/dxsdk/Lib"
        "${PROJECT_SOURCE_DIR}/global/detours/Lib"
        "${PROJECT_SOURCE_DIR}/global/vmprotect/Lib"
)

# 4. DLL Specific Definitions
add_definitions(-D_WINDLL)

# 5. Process Source Subdirectory
add_subdirectory(source)