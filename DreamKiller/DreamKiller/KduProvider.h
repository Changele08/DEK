#pragma once
#include <memory>
#include "../global.h"   //  KDU_CONTEXT, KDUProviderCreate/Release µÈ

// this file provides a C++ wrapper for KDU provider,
// encapsulating the C-style API into a more user-friendly class interface.
// 
// r/w memory by simple func instead of "m_Context->Provider->Callbacks.XXX"

class KduProvider {
public:
	// create provider context with specified provider ID, shellcode version and action type.
    KduProvider(ULONG ProviderId, ULONG ShellCodeVersion, KDU_ACTION_TYPE ActionType)
    {
        m_Context = nullptr;

        LONG NtBuildNumber;
        BOOLEAN HvciEnabled;
        BOOLEAN hvciStrict;
        BOOLEAN hvciIUM;

        //
        // Providers maybe *not* HVCI compatible.
        //
        if (supQueryHVCIState(&HvciEnabled, &hvciStrict, &hvciIUM)) {

            if (HvciEnabled) {
                printf_s("[*] Windows HVCI mode detected\r\n");
            }
        }

        OSVERSIONINFO osv;
        CHAR vendorString[0x20];

        RtlFillMemory(vendorString, sizeof(vendorString), 0);
        GET_CPU_VENDOR_STRING(vendorString);
        printf_s("[*] CPU vendor string: %s\r\n", vendorString);

        RtlSecureZeroMemory(&osv, sizeof(osv));
        osv.dwOSVersionInfoSize = sizeof(osv);
        RtlGetVersion((PRTL_OSVERSIONINFOW)&osv);

        NtBuildNumber = osv.dwBuildNumber;

        m_Context = KDUProviderCreate(ProviderId, HvciEnabled, NtBuildNumber,
            ShellCodeVersion, ActionType);
    }

    // destructor : Release resources & Unload driver
    ~KduProvider()
    {
        if (m_Context) {
            KDUProviderRelease(m_Context);
            m_Context = nullptr;
        }
    }
	// No copying allowed!!(Handle cannot be simply duplicated)
	// (but you can rewrite it by using shared_ptr etc. to share the context :P )
    KduProvider(KduProvider&& other)
        : m_Context(std::exchange(other.m_Context, nullptr))
    {
    }

    KduProvider& operator=(KduProvider&& other)
    {
        if (this != &other) {
            if (m_Context)
                KDUProviderRelease(m_Context);
            m_Context = std::exchange(other.m_Context, nullptr);
        }
        return *this;
    }

    // Check context is valid
    bool IsValid() const
    {
        return m_Context != nullptr && m_Context->DeviceHandle != nullptr;
    }

	// Get Ntoskrnl.exe base address(provided by provider)
    ULONG_PTR GetNtOsBase() const { return m_Context ? m_Context->NtOsBase : 0; }

    // Get device handle (can be used to call Nt* functions directly)
    HANDLE GetDeviceHandle() const
    {
        return IsValid() ? m_Context->DeviceHandle : nullptr;
    }

	// Get CR3 value
    ULONG_PTR GetCR3() const
    {
        if (!IsValid() || !m_Context->Provider->Callbacks.QueryPML4Value)
            return 0;

        ULONG_PTR cr3 = 0;
        if (m_Context->Provider->Callbacks.QueryPML4Value(m_Context->DeviceHandle, &cr3))
            return cr3;
        return 0;
    }

    // Vritual addr -> Physical addr
    bool VirtualToPhysical(ULONG_PTR VirtualAddress, ULONG_PTR* PhysicalAddress) const
    {
        if (!IsValid() || !PhysicalAddress)
            return false;

        if (m_Context->Provider->Callbacks.VirtualToPhysical) {
            return m_Context->Provider->Callbacks.VirtualToPhysical(
                m_Context->DeviceHandle, VirtualAddress, PhysicalAddress);
        }

        return false;
    }

    // R/W Kernel memory
    bool ReadKernelVirtual(ULONG_PTR Address, PVOID Buffer, ULONG NumberOfBytes) const
    {
        if (!IsValid() || !Buffer || NumberOfBytes == 0)
            return false;

        if (m_Context->Provider->Callbacks.ReadKernelVM) {
            return m_Context->Provider->Callbacks.ReadKernelVM(
                m_Context->DeviceHandle, Address, Buffer, NumberOfBytes);
        }

        return false;
    }

    bool WriteKernelVirtual(ULONG_PTR Address, const PVOID Buffer, ULONG NumberOfBytes) const
    {
        if (!IsValid() || !Buffer || NumberOfBytes == 0)
            return false;

        if (m_Context->Provider->Callbacks.WriteKernelVM) {
            return m_Context->Provider->Callbacks.WriteKernelVM(
                m_Context->DeviceHandle, Address, const_cast<PVOID>(Buffer), NumberOfBytes);
        }
        return false;
    }

	// R/W Physical memory
    bool ReadPhysical(ULONG_PTR PhysicalAddress, PVOID Buffer, ULONG NumberOfBytes) const
    {
        if (!IsValid() || !Buffer || NumberOfBytes == 0)
            return false;

        if (m_Context->Provider->Callbacks.ReadPhysicalMemory) {
            return m_Context->Provider->Callbacks.ReadPhysicalMemory(
                m_Context->DeviceHandle, PhysicalAddress, Buffer, NumberOfBytes);
        }
        return false;
    }

    bool WritePhysical(ULONG_PTR PhysicalAddress, const PVOID Buffer, ULONG NumberOfBytes) const
    {
        if (!IsValid() || !Buffer || NumberOfBytes == 0)
            return false;

        if (m_Context->Provider->Callbacks.WritePhysicalMemory) {
            return m_Context->Provider->Callbacks.WritePhysicalMemory(
                m_Context->DeviceHandle, PhysicalAddress, const_cast<PVOID>(Buffer), NumberOfBytes);
        }
        return false;
    }


	// Get provider metadata (driver name, device name, etc.)
    const KDU_DB_ENTRY* GetMetadata() const
    {
        return IsValid() ? m_Context->Provider->LoadData : nullptr;
    }

private:
    PKDU_CONTEXT m_Context;
};

bool MmIsAddressValid(const KduProvider& provider, PVOID VirtualAddress);
ULONG_PTR MmGetSystemRoutineAddress(const KduProvider& provider, PCSTR FunctionName);