/*******************************************************************************
*
*  (C) COPYRIGHT AUTHORS, 2018 - 2023
*
*  TITLE:       VICTIM.H
*
*  VERSION:     1.30
*
*  DATE:        20 Mar 2023
*
*  Victim support prototypes and definitions.
*
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
* ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED
* TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
* PARTICULAR PURPOSE.
*
*******************************************************************************/

#pragma once

typedef BOOL(WINAPI* pfnVictimCreate)(
    _In_opt_ HINSTANCE ModuleBase,
    _In_ LPCWSTR Name,
    _In_ ULONG ResourceId,
    _In_ ACCESS_MASK DesiredAccess,
    _Out_opt_ PHANDLE VictimHandle,
    _Out_opt_ PVOID *VictimImage,
    _In_opt_ pfnLoadDriverCallback Callback,
    _In_opt_ PVOID CallbackParam
    );

typedef BOOL(WINAPI* pfnVictimRelease)(
    _In_ LPCWSTR Name
    );

typedef VOID(WINAPI* pfnVictimExecute)(
    _In_ LPCWSTR Name,
    _In_ ACCESS_MASK DesiredAccess,
    _Out_opt_ PHANDLE VictimHandle
    );

typedef enum _VICTIM_INFORMATION {
    VictimImageInformation = 0,
    VictimDriverInformation,
    VictimRopChainInformation,
    MaxVictimInformation
} VICTIM_INFORMATION;

typedef struct _VICTIM_IMAGE_INFORMATION {
    ULONG DispatchOffset;
    ULONG DispatchPageOffset;
    ULONG JumpValue;
} VICTIM_IMAGE_INFORMATION, * PVICTIM_IMAGE_INFORMATION;

typedef struct _VICTIM_DRIVER_INFORMATION {
    ULONG_PTR LoadedImageBase;
    ULONG ImageSize;
} VICTIM_DRIVER_INFORMATION, * PVICTIM_DRIVER_INFORMATION;

typedef struct _VICTIM_LOAD_PARAMETERS {
    struct _KDU_VICTIM_PROVIDER *Provider;
} VICTIM_LOAD_PARAMETERS, * PVICTIM_LOAD_PARAMETERS;

//
// No optional victim flags specified, this is default value.
//
#define KDU_VICTIM_FLAGS_NONE               0x00000000

//
// Victim can be reloaded.
//
#define KDU_VICTIM_FLAGS_SUPPORT_RELOAD     0x00000001

typedef struct _KDU_VICTIM_PROVIDER {
    LPCWSTR Name; //same as device name
    LPCWSTR Desc; //optional
    ULONG ResourceId;
    ULONG VictimId;
    ACCESS_MASK DesiredAccess;
    union {
        ULONG Flags;
        struct {
            ULONG SupportReload : 1;
            ULONG Reserved : 31;
        };
    };
    struct {
        pfnVictimCreate Create;
        pfnVictimRelease Release;
        pfnVictimExecute Execute;
    } Callbacks;

    struct {
        PVOID DispatchSignature;
        ULONG DispatchSignatureLength;
        PVOID VictimImage;
    } Data;

} KDU_VICTIM_PROVIDER, * PKDU_VICTIM_PROVIDER;
