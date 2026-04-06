# 🔥 DEK – Remove system CmCallback/ObCallback & Terminate PPL process

[![Platform](https://img.shields.io/badge/Platform-Windows%2010%2F11%20x64-blue)](https://github.com/yourname/DEK)
[![Build](https://img.shields.io/badge/Build-VS2022-brightgreen)](https://github.com/yourname/DEK)
[![Stars](https://img.shields.io/github/stars/Changele08/DEK?style=social)](https://github.com/Changele08/DEK)

<div align="right">
  <a href="README.md">English</a> | <a href="Readme_zh.md">简体中文</a>
</div>

## DreamEx_Killer

#### System Requirements

+ x64 Windows 10/11;
+ Administrative privilege is required.

# Purpose and Features

The purpose of this tool is to provide a simple way to Remove the Windows ObCallback & CmCallback & ppl protection without requiring extensive setup or a local debugger.
It features:
+ Remove PPL protection for the specified PID
+ Remove ObCallback(Allow opening processes using PROCESS_ALL_ACCESS)
+ Remove CmCallback(Allow editing of any registry entries)
+ Set Image File Execution Options to hijack target executable
+ Support for various vulnerable drivers used as functionality "providers".

#### Usage

###### DEK -list
###### DEK -prv ProviderID
###### DEK -ps ProcessID
###### DEK -ob
###### DEK -cm
###### DEK -kill ProcessID
###### DEK -hi ExePath DebuggerPath
* -list - list currently available providers;
* -prv  - optional, select vulnerable driver provider;
* -ps   - modify process object of given ProcessID, downgrading any protections;
* -ob   - remove ObCallback globally
* -cm   - remove CmCallback globally
* -kill - Kill given process by using WinStationTerminateProcess
* -hi   - hijack exe file to prevent them restart

Example:
+ dek -ps 1234
+ dek -prv 1 -ps 1234
+ dek -ob
+ dek -cm
+ dek -ps pid -ob -cm -kill pid

# Supported Providers

Note: Provider with Id 34 is assumed as default if no -prv command is specified.

Not all Provider in KDU is here because some of them have been banned by Windows.Loading them will cause a warning.

| Id     | Vendor         | Driver      | Software package                   | Version                      | MSFT blacklist*     |
|--------|----------------|-------------|------------------------------------|-----------------------------|----------------------|
| 1      | MSI            | RTCore64    | MSI Afterburner                    | 4.6.2 build 15658 and below | Page hash            |
| 10     | Realtek        | RtkIo64     | Realtek Dash Client Utility        | Various                     | Name                 |
| 13     | ASUSTeK        | AsIO2       | ASUS GPU Tweak                     | 2.1.7.1 and below           |                      |
| 20     | Dell           | DBUtilDrv2  | Dell BIOS Utility                  | 2.7 and below               |                      |
| 22     | ASUSTeK        | AsIO3       | ASUS GPU TweakII                   | 2.3.0.3                     |                      |
| 26     | HiRes Ent.     | inpoutx64   | Various                            | 1.2.0 and below             |                      |
| 27     | PassMark       | DirectIo64  | PassMark OSForensics               | Any                         |                      |
| 29     | Arthur Liberman| ALSysIO64   | Core Temp                          | 2.0.11 and below            |                      |
| 32     | Lenovo         | LDD         | Lenovo Diagnostics Driver for Windows 10 and later | 1.0.4.0 and below               |  Cert, Name          |
| 34     | MSI            | winio       | MSI Foundation Service             | Undefined                   |                      |
| 44     | AMD            | PdFwKrnl  | AMD Radeon™ Software (Adrenalin Edition and PRO Edition) | 23.9.1 and below             |                      |
| 47     | EVGA  | EleetX1| EVGA ELEET X1     | 1.0.16.0 and below              |                      |
| 49     | ASRock         | AppShopDrv103  | ASRock APP Shop    | 1.0.58 and below       |                      |
| 52     | Intel          | PmxDrv  | Intel(R) Management Engine Tools Driver    | 1.0.0.1003 and below       |                      |
| 53     | Jun Liu        | HwRwDrv  | Hardware read & write driver    | 1.0.0.6 and below       |                      |
| 54     | NetEase        | NeacSafe64  | NeacSafe64 mini-filter driver    | 1.0.0.1 and below       |                      |
| 55     | TechPowerUp    | ThrottleStop  | ThrottleStop                       | 3.0.0.0 and below             |                      |
| 56     | Toshiba        | TPwSav        | Toshiba power saving driver for laptops | 2.0.0.2 and below             |                      |
| 57     | Lenovo         | LnvMSRIO      | Lenovo filter driver | 3.1.0.35 and below             |                      |
| 58     | Teledyne       | CorMem      | Sapera Memory Manager | 9.0.0 and below             |                      |
| 59     | Digital Elect. Corp | Ipctype     | IPCType Device Driver | 1.0.2.0 and below             |                      |
| 60     | Shangke        | WinHwDrv    | Undefined | Undefined             |                      |

MSFT blacklist types:

Cert - by certificate used to sign the driver which makes it possible to ban huge number of files at one time.

Name - by original filename entry stored inside VERSION_INFO file resources, this type of bans are only possible with cross-checking of file version otherwise it will cause false-positives in case if the driver has "fixed/unaffected" version.

Hash/Page hash - by authenticode hash/page hash, allows MSFT to ban exact driver file.

# How it works
Similar to KDU:It uses known vulnerable (or wormhole by design) drivers from legitimate software to access arbitrary kernel memory with read/write primitives.

KDU provides an abstract interface for using vulnerable drivers, which allows us to r/w Phy/Vir Memory of Kernel.So we can searching kernel to get several unexported variable and modify kernel mode process objects(EPROCESS).Manually parsing the kernel PE file and using feature matching to get the offset of ObCallback tab etc. 

Then, disable the CmCallback by isolating the list head.Disable the ObCallback by writting 0 to callback pointer.Remove/Set PPL by writting given value to PS_PROTECTION structure.

# Build and Notes
DEK comes with full source code. To build from source, you need Microsoft Visual Studio 2022 or later. 

Complete working binaries is DEK.exe,which contains drivers database & main function.


# Reporting bugs and incompatibilities

Similar to KDU:

If you experience a bug or incompatibility while using DEK with 3rd party software or OS, feel free to fill an issue. However, if this incompatibility is caused by your own actions, such reports will be ignored.

Anticheat, antimalware incompatibilities will be ignored, that's your own responsibility.

# Disclaimer

Also Similar to KDU:

Using this program might crash your computer with a BSOD. Compiled binary and source code are provided AS-IS in the hope they will be useful BUT WITHOUT WARRANTY OF ANY KIND. Since DEK relies on completely bugged, vulnerable drivers, it is highly recommended to use it on virtual machines only.


### Fun stories -- Why named "DreamEx_Killer"
The idea coming from an Uploder on Bilibili named "缘梦DreamEx".

He uploaded a video descripted how to use vulnerable driver(WinIO) to r/w Phy/Vir memory and remove ob/cm callback & PPL to terminate an AV process(360, huorong etc.) and set hijack the exe to prevent them from restarting.

Although his account was banned later, I downloaded example exe(which is only packed by UPX, and I can disasm it easily) in comment section :P.

After reverse engineering and learning from KDU project, I rewrite the Memory r/w interface(expand them from single Vulnerable Driver(WinIO) to several VD(see the provider list)), and complete the code which didn't display in the video.

FINALLY, it's time for naming. I need a striking name (even if it feels a bit of "..chuunibyou"). "DreamEx_Killer" suddenly popped into my mind.So,
That's it.

(Sorry for wrong name of proj dir...)

# Reference
KDU,https://github.com/hfiref0x/KDU

缘梦DreamEx's Video
