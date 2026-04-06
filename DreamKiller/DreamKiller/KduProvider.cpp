#include "KduProvider.h"
#include <cassert>

#include <cstring>

// Check the address is valid (by trying to read from it. )
bool MmIsAddressValid(const KduProvider& provider, PVOID VirtualAddress)
{
    if (!provider.IsValid())
        return false;

    ULONG_PTR temp = 0;
    // 如果 provider 能成功读取，则说明页表项有效且地址合法
    return provider.ReadKernelVirtual((unsigned long long)VirtualAddress, &temp, 1);
}

// Get the address of an exported function in ntoskrnl.exe(by parsing its export table.)
ULONG_PTR MmGetSystemRoutineAddress(const KduProvider& provider, PCSTR FunctionName)
{
    if (!provider.IsValid() || FunctionName == nullptr)
        return 0;

    ULONG_PTR ntosBase = provider.GetNtOsBase();
    if (ntosBase == 0)
        return 0;

    // Read DOS head
    IMAGE_DOS_HEADER dosHdr = { 0 };
    if (!provider.ReadKernelVirtual(ntosBase, &dosHdr, sizeof(dosHdr)))
        return 0;
    if (dosHdr.e_magic != IMAGE_DOS_SIGNATURE)
        return 0;

    // Read NT head
    ULONG_PTR ntHeadersAddr = ntosBase + dosHdr.e_lfanew;
    IMAGE_NT_HEADERS64 ntHdr = { 0 };
    if (!provider.ReadKernelVirtual(ntHeadersAddr, &ntHdr, sizeof(ntHdr)))
        return 0;
    if (ntHdr.Signature != IMAGE_NT_SIGNATURE)
        return 0;

    // Get RVA
    ULONG exportRva = ntHdr.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    if (exportRva == 0)
        return 0;

    ULONG_PTR exportAddr = ntosBase + exportRva;
    IMAGE_EXPORT_DIRECTORY exportDir = { 0 };
    if (!provider.ReadKernelVirtual(exportAddr, &exportDir, sizeof(exportDir)))
        return 0;

    ULONG_PTR namesAddr = ntosBase + exportDir.AddressOfNames;
    ULONG_PTR functionsAddr = ntosBase + exportDir.AddressOfFunctions;
    ULONG_PTR ordinalsAddr = ntosBase + exportDir.AddressOfNameOrdinals;

    for (DWORD i = 0; i < exportDir.NumberOfNames; i++) {
        ULONG nameRva = 0;
        if (!provider.ReadKernelVirtual(namesAddr + i * sizeof(ULONG), &nameRva, sizeof(nameRva)))
            continue;

        ULONG_PTR namePtr = ntosBase + nameRva;
        char name[256] = { 0 };
        if (!provider.ReadKernelVirtual(namePtr, name, sizeof(name) - 1))
            continue;

        if (_stricmp(name, FunctionName) == 0) {
            WORD ordinal = 0;
            if (!provider.ReadKernelVirtual(ordinalsAddr + i * sizeof(WORD), &ordinal, sizeof(ordinal)))
                return 0;

            ULONG functionRva = 0;
            if (!provider.ReadKernelVirtual(functionsAddr + ordinal * sizeof(ULONG), &functionRva, sizeof(functionRva)))
                return 0;

            return ntosBase + functionRva;
        }
    }

    return 0;  // Fail... :(
}