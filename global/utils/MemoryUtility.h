#pragma once

#include "CallHelper.h"
#include "detours.h"

#define DECLARE_VIRTUAL_NOIMPL(ret_type, fn, ret_value) \
    virtual ret_type fn { return ret_value; }

// VC80 compatible address extraction
template<typename T>
inline uintptr_t addr_from_this(T funptr) {
    union {
        T ptr;
        uintptr_t addr;
    } converter;
    converter.ptr = funptr;
    return converter.addr;
}

enum E_MEMUTUL_HOOK_TYPE {
    LongJump = 0xE9,
    LongCall = 0xE8
};

enum E_ASM_OPCODE
{
    // Data Movement
    MOV_rm_r   = 0x89, // Move register to register/memory
    MOV_r_rm   = 0x8B, // Move register/memory to register
    PUSH       = 0x50, // Push register (0x50 - 0x57 depending on reg)
    JMP_SHORT  = 0xEB, // Short Jump (8-bit relative)
    JMP_NEAR   = 0xE9, // Near Jump (32-bit relative)
    JE         = 0x74, // Jump if Equal / Zero
    JNE        = 0x75, // Jump if Not Equal / Not Zero
    CALL       = 0xE8, // Call procedure
};

#define MEMUTIL_ASM_OPCODE_LONG_JUMP    (char)0xE9
#define MEMUTIL_ASM_OPCODE_SHORT_JUMP   (char)0xEB
#define MEMUTIL_ASM_OPCODE_LONG_CALL    (char)0xE8
#define MEMUTIL_ASM_OPCODE_NOP          (char)0x90

// --- Macros (Preserved for compatibility) ---
#define MEMUTIL_WRITE_VALUE(type, offset, value) \
    CMemoryUtility::Write<type>((uintptr_t)(offset), (value))

#define MEMUTIL_WRITE_POINTER(offset, dataPtr, dataLen) \
    CMemoryUtility::WriteRaw((uintptr_t)(offset), (dataPtr), (dataLen))

#define MEMUTIL_ADD_PTR(ptr, offset) \
	(((uintptr_t)(ptr)) + offset)

#define MEMUTIL_NOP(offset, count) \
    CMemoryUtility::Nop((uintptr_t)(offset), (count))

#define MEMUTIL_READ_BY_PTR_OFFSET(ptr, offset, type) \
    *(type*)(((uintptr_t)(ptr)) + (offset))

#define MEMUTIL_WRITE_BY_PTR_OFFSET(ptr, offset, type, value) \
    *(type*)(((uintptr_t)(ptr)) + (offset)) = (value)

#define MEMUTIL_VFTABLE_GET_FUNC(vtable_addr, index) \
    (*((DWORD*)(vtable_addr) + (index)))

#define MEMUTIL_SETUP_HOOK(type, src, dest) \
    CMemoryUtility::SetupHook((type), (uintptr_t)(src), (uintptr_t)(dest))

#define MEMUTIL_REPLACE_ADDRESS(address, offset) \
    CMemoryUtility::ReplaceAddr((uintptr_t)(address), addr_from_this(offset))

#define MEMUTIL_REPLACE_OFFSET(address, offset) \
    CMemoryUtility::ReplaceOffset((uintptr_t)(address), addr_from_this(offset))

#define MEMUTIL_VFTABLE_HOOK(vftable_addr, index, target) \
    CMemoryUtility::VFTableHook((uintptr_t)(vftable_addr), (int)(index), addr_from_this(target))

#define MEMUTIL_HOOK_FN(src, target) \
    DetourTransactionBegin(); \
    DetourUpdateThread(GetCurrentThread()); \
    DetourAttach(&(PVOID&)src, (PVOID)target); \
    DetourTransactionCommit();

#define MEMUTIL_UNHOOK(src, target) \
    DetourTransactionBegin(); \
    DetourUpdateThread(GetCurrentThread()); \
    DetourDetach(&(PVOID&)src, (PVOID)target); \
    DetourTransactionCommit();

#define MEMUTIL_ISOFFSETPATCHED(target, expected) \
    CMemoryUtility::IsOffsetModified(target,expected)

#define MEMUTIL_ISOFFSETPATCHED_OPCODE(target, expected, opcode) \
    CMemoryUtility::IsOffsetModified(target,expected, opcode)

class CMemoryUtility
{
private:
    // Centralized internal writer to handle protection and cache flushing
    static bool InternalPatch(uintptr_t addr, const void* data, size_t size)
    {
        LPVOID lpAddr = (LPVOID)addr;
        DWORD dwOld;
        if (VirtualProtect(lpAddr, size, PAGE_EXECUTE_READWRITE, &dwOld))
        {
            memcpy(lpAddr, data, size);
            VirtualProtect(lpAddr, size, dwOld, &dwOld);
            // Critical for VC80/VS2005 era apps to prevent crashes on multi-core
            FlushInstructionCache(GetCurrentProcess(), lpAddr, size);
            return true;
        }
        return false;
    }

public:
    template<typename T>
    static bool Write(uintptr_t offset, const T& value)
    {
        return InternalPatch(offset, &value, sizeof(T));
    }

    static bool WriteRaw(uintptr_t offset, const void* data, size_t length)
    {
        return InternalPatch(offset, data, length);
    }

    static bool Nop(uintptr_t offset, size_t count)
    {
        DWORD dwOld;
        if (VirtualProtect((LPVOID)offset, count, PAGE_EXECUTE_READWRITE,
                           &dwOld)) {
            memset((LPVOID)offset, MEMUTIL_ASM_OPCODE_NOP, count);
            VirtualProtect((LPVOID)offset, count, dwOld, &dwOld);
            FlushInstructionCache(GetCurrentProcess(), (LPVOID)offset, count);
            return true;
        }
        return false;
    }

    static bool SetupHook(E_MEMUTUL_HOOK_TYPE type, uintptr_t src, uintptr_t dest)
    {
        unsigned char buf[5];
        buf[0] = (unsigned char)type;
        // JMP/CALL distance calculation
        DWORD rel = (DWORD)(dest - (src + 5));
        memcpy(&buf[1], &rel, 4);
        return InternalPatch(src, buf, 5);
    }

    static void ReplaceAddr(uintptr_t addr, uintptr_t value)
    {
        InternalPatch(addr, &value, sizeof(uintptr_t));
    }

    static void ReplaceOffset(uintptr_t instruction_addr, uintptr_t target_addr)
    {
        uintptr_t offset_location = instruction_addr + 1;
        DWORD relative_dist = (DWORD)(target_addr - (instruction_addr + 5));
        InternalPatch(offset_location, &relative_dist, sizeof(DWORD));
    }

    static void VFTableHook(uintptr_t vftable_addr, int index, uintptr_t target_func)
    {
        uintptr_t entry_addr = vftable_addr + (index * sizeof(void*));
        ReplaceAddr(entry_addr, target_func);
    }

    // True = Modified, False = Intact
    static bool IsOffsetModified(DWORD targetAddress, DWORD expectedValue)
    {
        DWORD currentValue = *(reinterpret_cast<DWORD*>(targetAddress));
        return currentValue != expectedValue;
    }

    // True = Modified, False = Intact
    static bool IsOffsetModified(DWORD targetAddress, DWORD expectedValue, E_ASM_OPCODE opcode)
    {
        if (*(reinterpret_cast<unsigned char*>(targetAddress)) != opcode)
            return true;

        DWORD relativeDisplacement = *(reinterpret_cast<DWORD*>(targetAddress + 1));
        DWORD actualTargetAddress = (targetAddress + 5) + relativeDisplacement;
        return actualTargetAddress != expectedValue;
    }
};
