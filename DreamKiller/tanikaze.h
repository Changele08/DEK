// 这个头文件只能被包含一次，在 kduprov.cpp中！

#include <Windows.h>
#include "consts.h"
#include "ntos/ntbuilds.h"
#include "kdubase.h"
#include "resource.h"

KDU_DB_ENTRY gProvEntry[] = {
    {
        KDU_MIN_NTBUILDNUMBER,
        KDU_MAX_NTBUILDNUMBER,
        IDR_RTCORE64,
        KDU_PROVIDER_UNWINDER_RTCORE,
        KDU_VICTIM_DEFAULT,
        SourceBaseNone,
        KDUPROV_FLAGS_NONE,
        KDUPROV_SC_ALL_DEFAULT,
        (LPWSTR)L"CVE-2019-16098",
        (LPWSTR)L"RTCore64",
        (LPWSTR)L"RTCore64",
        (LPWSTR)L"MICRO-STAR INTERNATIONAL CO., LTD."
    },

    {
        KDU_MIN_NTBUILDNUMBER,
        KDU_MAX_NTBUILDNUMBER,
        IDR_RTKIO64,
        KDU_PROVIDER_RTKIO64,
        KDU_VICTIM_DEFAULT,
        SourceBasePhyMem,
        KDUPROV_FLAGS_SIGNATURE_WHQL | KDUPROV_FLAGS_PML4_FROM_LOWSTUB,
        KDUPROV_SC_ALL_DEFAULT,
        (LPWSTR)L"Realtek Dash Client Utility",
        (LPWSTR)L"rtkio64",
        (LPWSTR)L"rtkio",
        (LPWSTR)L"Realtek Semiconductor Corp."
    },

    {
        KDU_MIN_NTBUILDNUMBER,
        KDU_MAX_NTBUILDNUMBER,
        IDR_ASIO2,
        KDU_PROVIDER_ASUSIO2,
        KDU_VICTIM_DEFAULT,
        SourceBaseWinIo,
        KDUPROV_FLAGS_SIGNATURE_WHQL | KDUPROV_FLAGS_PML4_FROM_LOWSTUB,
        KDUPROV_SC_ALL_DEFAULT,
        (LPWSTR)L"ASUS GPU Tweak",
        (LPWSTR)L"AsIO2",
        (LPWSTR)L"Asusgio2",
        (LPWSTR)L"ASUSTeK Computer Inc."
    },

    {
        KDU_MIN_NTBUILDNUMBER,
        KDU_MAX_NTBUILDNUMBER,
        IDR_DBUTILDRV2,
        KDU_PROVIDER_DBUTILDRV2,
        KDU_VICTIM_DEFAULT,
        SourceBaseNone,
        KDUPROV_FLAGS_SIGNATURE_WHQL | KDUPROV_FLAGS_NO_FORCED_SD,
        KDUPROV_SC_ALL_DEFAULT,
        (LPWSTR)L"CVE-2021-36276",
        (LPWSTR)L"DBUtilDrv2",
        (LPWSTR)L"DBUtil_2_5",
        (LPWSTR)L"Microsoft Windows Hardware Compatibility Publisher"
    },

    {
        KDU_MIN_NTBUILDNUMBER,
        KDU_MAX_NTBUILDNUMBER,
        IDR_ASIO3,
        KDU_PROVIDER_ASUSIO3,
        KDU_VICTIM_DEFAULT,
        SourceBaseWinIo,
        KDUPROV_FLAGS_SIGNATURE_WHQL | KDUPROV_FLAGS_PML4_FROM_LOWSTUB,
        KDUPROV_SC_ALL_DEFAULT,
        (LPWSTR)L"ASUS GPU Tweak II",
        (LPWSTR)L"AsIO3",
        (LPWSTR)L"Asusgio3",
        (LPWSTR)L"ASUSTeK Computer Inc."
    },

    {
        KDU_MIN_NTBUILDNUMBER,
        KDU_MAX_NTBUILDNUMBER,
        IDR_INPOUTX64,
        KDU_PROVIDER_INPOUTX64,
        KDU_VICTIM_DEFAULT,
        SourceBaseWinIo,
        KDUPROV_FLAGS_PML4_FROM_LOWSTUB,
        KDUPROV_SC_ALL_DEFAULT,
        (LPWSTR)L"inpoutx64 Driver Version 1.2",
        (LPWSTR)L"inpoutx64",
        (LPWSTR)L"inpoutx64",
        (LPWSTR)L"Red Fox UK Limited"
     },

     {
        KDU_MIN_NTBUILDNUMBER,
        KDU_MAX_NTBUILDNUMBER,
        IDR_PASSMARK_OSF,
        KDU_PROVIDER_PASSMARK_OSF,
        KDU_VICTIM_DEFAULT,
        SourceBaseNone,
        KDUPROV_FLAGS_SIGNATURE_WHQL | KDUPROV_FLAGS_PML4_FROM_LOWSTUB,
        KDUPROV_SC_ALL_DEFAULT,
        (LPWSTR)L"PassMark OSForensics DirectIO",
        (LPWSTR)L"DirectIo64",
        (LPWSTR)L"DIRECTIO64",
        (LPWSTR)L"PassMark Software Pty Ltd"
     },

     {
        KDU_MIN_NTBUILDNUMBER,
        KDU_MAX_NTBUILDNUMBER,
        IDR_ALSYSIO64,
        KDU_PROVIDER_ALCPU,
        KDU_VICTIM_DEFAULT,
        SourceBaseNone,
        KDUPROV_FLAGS_SIGNATURE_WHQL | KDUPROV_FLAGS_PHYSICAL_BRUTE_FORCE,
        KDUPROV_SC_ALL_DEFAULT,
        (LPWSTR)L"Core Temp",
        (LPWSTR)L"ALSysIO64",
        (LPWSTR)L"ALSysIO",
        (LPWSTR)L"ALCPU (Arthur Liberman)"
     },

     {
        NT_WIN10_REDSTONE4,
        KDU_MAX_NTBUILDNUMBER,
        IDR_LDD,
        KDU_PROVIDER_LENOVO_DD,
        KDU_VICTIM_DEFAULT,
        SourceBaseNone,
        KDUPROV_FLAGS_SIGNATURE_WHQL | KDUPROV_FLAGS_PREFER_PHYSICAL | KDUPROV_FLAGS_USE_SYMBOLS,
        KDUPROV_SC_ALL_DEFAULT,
        (LPWSTR)L"Lenovo Diagnostics Driver for Windows 10 and later (CVE-2022-3699)",
        (LPWSTR)L"LenovoDiagnosticsDriver",
        (LPWSTR)L"LenovoDiagnosticsDriver",
        (LPWSTR)L"Lenovo"
     },

     {
        KDU_MIN_NTBUILDNUMBER,
        KDU_MAX_NTBUILDNUMBER,
        IDR_MSI_WINIO,
        KDU_PROVIDER_MSI_WINIO,
        KDU_VICTIM_DEFAULT,
        SourceBaseWinIo,
        KDUPROV_FLAGS_SIGNATURE_WHQL | KDUPROV_FLAGS_PML4_FROM_LOWSTUB,
        KDUPROV_SC_ALL_DEFAULT,
        (LPWSTR)L"MSI Foundation Service",
        (LPWSTR)L"WinIo",
        (LPWSTR)L"WinIo",
        (LPWSTR)L"Microsoft Windows Hardware Compatibility Publisher"
     },

    {
        KDU_MIN_NTBUILDNUMBER,
        KDU_MAX_NTBUILDNUMBER,
        IDR_AMD_PDFWKRNL,
        KDU_PROVIDER_AMD_PDFWKRNL,
        KDU_VICTIM_DEFAULT,
        SourceBaseNone,
        KDUPROV_FLAGS_SIGNATURE_WHQL | KDUPROV_FLAGS_PREFER_VIRTUAL,
        KDUPROV_SC_ALL_DEFAULT,
        (LPWSTR)L"AMD USB-C Power Delivery Firmware Update Utility CVE-2023-20598",
        (LPWSTR)L"PdFwKrnl",
        (LPWSTR)L"PdFwKrnl",
        (LPWSTR)L"Advanced Micro Devices Inc."
    },

    {
        KDU_MIN_NTBUILDNUMBER,
        KDU_MAX_NTBUILDNUMBER,
        IDR_EVGA_ELEETX1,
        KDU_PROVIDER_EVGA_ELEETX1,
        KDU_VICTIM_DEFAULT,
        SourceBaseNone,
        KDUPROV_FLAGS_SIGNATURE_WHQL | KDUPROV_FLAGS_PHYSICAL_BRUTE_FORCE,
        KDUPROV_SC_ALL_DEFAULT,
        (LPWSTR)L"EVGA Low Level Driver",
        (LPWSTR)L"EleetX1",
        (LPWSTR)L"EleetX1",
        (LPWSTR)L"EVGA Corp."
    },

     {
        KDU_MIN_NTBUILDNUMBER,
        KDU_MAX_NTBUILDNUMBER,
        IDR_ASROCKAPPSHOP103,
        KDU_PROVIDER_ASROCK3,
        KDU_VICTIM_DEFAULT,
        SourceBaseRWEverything,
        KDUPROV_FLAGS_PHYSICAL_BRUTE_FORCE,
        KDUPROV_SC_ALL_DEFAULT,
        (LPWSTR)L"AppShopDrv103 Driver",
        (LPWSTR)L"AppShopDrv103",
        (LPWSTR)L"AppShopDrv103",
        (LPWSTR)L"ASROCK Incorporation"
     },

    {
        KDU_MIN_NTBUILDNUMBER,
        KDU_MAX_NTBUILDNUMBER,
        IDR_PMXDRV64,
        KDU_PROVIDER_INTEL_PMXDRV,
        KDU_VICTIM_DEFAULT,
        SourceBaseNone,
        KDUPROV_FLAGS_SIGNATURE_WHQL | KDUPROV_FLAGS_PML4_FROM_LOWSTUB,
        KDUPROV_SC_ALL_DEFAULT,
        (LPWSTR)L"Intel(R) Management Engine Tools Driver",
        (LPWSTR)L"PMxDrv",
        (LPWSTR)L"Pmxdrv",
        (LPWSTR)L"Intel(R) Embedded Subsystems and IP Blocks Group"
    },

    {
        KDU_MIN_NTBUILDNUMBER,
        KDU_MAX_NTBUILDNUMBER,
        IDR_HWRWDRVX64,
        KDU_PROVIDER_HWRWDRVX64,
        KDU_VICTIM_DEFAULT,
        SourceBaseWinRing0,
        KDUPROV_FLAGS_PHYSICAL_BRUTE_FORCE,
        KDUPROV_SC_ALL_DEFAULT,
        (LPWSTR)L"Hardware read & write driver",
        (LPWSTR)L"HwRwDrv.x64",
        (LPWSTR)L"HwRwDrv",
        (LPWSTR)L"Open Source Developer, Jun Liu"
    },

    {
        NT_WIN10_THRESHOLD1,
        KDU_MAX_NTBUILDNUMBER,
        IDR_NEACSAFE64,
        KDU_PROVIDER_NEACSAFE64,
        KDU_VICTIM_DEFAULT,
        SourceBaseNone,
        KDUPROV_FLAGS_SIGNATURE_WHQL | KDUPROV_FLAGS_NO_FORCED_SD | KDUPROV_FLAGS_FS_FILTER,
        KDUPROV_SC_ALL_DEFAULT,
        (LPWSTR)L"NeacSafe64 mini-filter driver (CVE-2025-45737)",
        (LPWSTR)L"NeacSafe64",
        (LPWSTR)L"OWNeacSafePort",
        (LPWSTR)L"Microsoft Windows Hardware Compatibility Publisher"
    },

    {
        KDU_MIN_NTBUILDNUMBER,
        KDU_MAX_NTBUILDNUMBER,
        IDR_THROTTLESTOP,
        KDU_PROVIDER_TPUP,
        KDU_VICTIM_DEFAULT,
        SourceBaseNone,
        KDUPROV_FLAGS_SIGNATURE_WHQL | KDUPROV_FLAGS_USE_SUPERFETCH,
        KDUPROV_SC_ALL_DEFAULT,
        (LPWSTR)L"TechPowerUp ThrottleStop (CVE-2025-7771)",
        (LPWSTR)L"ThrottleStop",
        (LPWSTR)L"ThrottleStop",
        (LPWSTR)L"TechPowerUp"
    },

    {
        KDU_MIN_NTBUILDNUMBER,
        KDU_MAX_NTBUILDNUMBER,
        IDR_TPWSAV,
        KDU_PROVIDER_TOSHIBA,
        KDU_VICTIM_DEFAULT,
        SourceBaseNone,
        KDUPROV_FLAGS_SIGNATURE_WHQL | KDUPROV_FLAGS_USE_SUPERFETCH,
        KDUPROV_SC_ALL_DEFAULT,
        (LPWSTR)L"Toshiba power saving driver for laptops",
        (LPWSTR)L"TPwSav",
        (LPWSTR)L"EBIoDispatch",
        (LPWSTR)L"Compal Electronics"
    },

    {
        KDU_MIN_NTBUILDNUMBER,
        KDU_MAX_NTBUILDNUMBER,
        IDR_LENOVOMSRIO,
        KDU_PROVIDER_LENOVOMSRIO,
        KDU_VICTIM_DEFAULT,
        SourceBaseWinRing0,
        KDUPROV_FLAGS_SIGNATURE_WHQL | KDUPROV_FLAGS_PREFER_PHYSICAL | KDUPROV_FLAGS_USE_SUPERFETCH,
        KDUPROV_SC_ALL_DEFAULT,
        (LPWSTR)L"Lenovo MSR I/O Driver (CVE-2025-8061)",
        (LPWSTR)L"LnvMSRIO",
        (LPWSTR)L"WinMsrDev",
        (LPWSTR)L"Lenovo"
    },

    {
        KDU_MIN_NTBUILDNUMBER,
        KDU_MAX_NTBUILDNUMBER,
        IDR_TELEDYNE,
        KDU_PROVIDER_TELEDYNE,
        KDU_VICTIM_DEFAULT,
        SourceBaseMapMem,
        KDUPROV_FLAGS_SIGNATURE_WHQL | KDUPROV_FLAGS_PREFER_PHYSICAL | KDUPROV_FLAGS_PML4_FROM_LOWSTUB,
        KDUPROV_SC_ALL_DEFAULT,
        (LPWSTR)L"Sapera Memory Manager",
        (LPWSTR)L"CORMEM",
        (LPWSTR)L"CORMEM",
        (LPWSTR)L"Teledyne Digital Imaging Inc."
    },

    {
        KDU_MIN_NTBUILDNUMBER,
        KDU_MAX_NTBUILDNUMBER,
        IDR_IPCTYPE,
        KDU_PROVIDER_IPCTYPE,
        KDU_VICTIM_DEFAULT,
        SourceBaseNone,
        KDUPROV_FLAGS_USE_SUPERFETCH,
        KDUPROV_SC_ALL_DEFAULT,
        (LPWSTR)L"IPCType Device Driver for 64bit",
        (LPWSTR)L"IPCType",
        (LPWSTR)L"IPCType",
        (LPWSTR)L"Digital Electronics Corporation"
    },

    {
        KDU_MIN_NTBUILDNUMBER,
        KDU_MAX_NTBUILDNUMBER,
        IDR_SHANGKE_WHD,
        KDU_PROVIDER_SHANGKE_WHD,
        KDU_VICTIM_DEFAULT,
        SourceBaseWinRing0,
        KDUPROV_FLAGS_SIGNATURE_WHQL | KDUPROV_FLAGS_PREFER_PHYSICAL | KDUPROV_FLAGS_USE_SUPERFETCH,
        KDUPROV_SC_ALL_DEFAULT,
        (LPWSTR)L"Guangzhou Shangke Information Technology giveio driver",
        (LPWSTR)L"WinHwDriver",
        (LPWSTR)L"WinHwDriver",
        (LPWSTR)L"Guangzhou Shangke Information Technology"
    }

};

KDU_DB gProvTable_tanikaze = {
        RTL_NUMBER_OF(gProvEntry),
        gProvEntry
};