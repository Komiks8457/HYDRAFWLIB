#pragma once

#ifndef VMPROTECT_H
#define VMPROTECT_H

#include "VMProtectSDK.h"

void CreateFullDirectoryPath(const char* path)
{
    char temp[MAX_PATH];
    char* p = NULL;
    size_t len;

    _snprintf(temp, sizeof(temp), "%s", path);
    len = strlen(temp);
    if (temp[len - 1] == '\\' || temp[len - 1] == '/') {
        temp[len - 1] = 0;
    }

    for (p = temp + 1; *p; p++) {
        if (*p == '\\' || *p == '/') {
            char cb = *p;
            *p = 0;
            CreateDirectoryA(temp, NULL);
            *p = cb;
        }
    }

    CreateDirectoryA(temp, NULL);
}

bool VMProtectDumpFile(const char* virtualFileName, const char* targetFolder)
{
    // 1. Construct the full output path first to check existence
    char outPath[MAX_PATH];
    _snprintf(outPath, MAX_PATH, "%s\\%s", targetFolder, virtualFileName);

    // 2. Check if the file already exists on the physical disk
    DWORD dwAttrib = GetFileAttributesA(outPath);
    if (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY)) {
        // File exists, skip extraction
        return true;
    }

    // 3. If not exists, proceed with VMProtect extraction
    if (!VMProtectIsProtected()) return false;

    CreateFullDirectoryPath(targetFolder);

    HANDLE hVirtual = CreateFileA(virtualFileName, GENERIC_READ,
                                  FILE_SHARE_READ,
                                  NULL, OPEN_EXISTING,
                                  FILE_ATTRIBUTE_NORMAL, NULL);

    if (hVirtual == INVALID_HANDLE_VALUE) return false;

    DWORD fileSize = GetFileSize(hVirtual, NULL);
    if (fileSize == 0) {
        CloseHandle(hVirtual);
        return false;
    }

    char* buffer = new char[fileSize];
    DWORD bytesRead;
    bool success = false;

    if (ReadFile(hVirtual, buffer, fileSize, &bytesRead,
                 NULL)) {
        // 4. Write to physical disk since we confirmed it's missing
        HANDLE hPhysical = CreateFileA(outPath, GENERIC_WRITE, 0,
                                       NULL,
                                       CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,
                                       NULL);

        if (hPhysical != INVALID_HANDLE_VALUE) {
            DWORD bytesWritten;
            if (WriteFile(hPhysical, buffer, fileSize,
                          &bytesWritten, NULL)) {
                success = true;
            }
            CloseHandle(hPhysical);
        }
    }

    delete[] buffer;
    CloseHandle(hVirtual);
    return success;
}

#endif
