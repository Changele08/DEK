# 🔥 DEK – Remove system CmCallback/ObCallback & Terminate PPL process

[![Platform](https://img.shields.io/badge/Platform-Windows%2010%2F11%20x64-blue)](https://github.com/yourname/DEK)
[![Build](https://img.shields.io/badge/Build-VS2022-brightgreen)](https://github.com/yourname/DEK)
[![Stars](https://img.shields.io/github/stars/yourname/DEK?style=social)](https://github.com/yourname/DEK)

<div align="right">
  <a href="README.md">English</a> | <a href="README_zh.md">简体中文</a>
</div>

## DreamEx_Killer

#### 系统要求

+ x64 Windows 10/11；
+ 需要管理员权限。

# 目的与特性
此工具的目的是提供一种简单的方法来移除Windows ObCallback、CmCallback和ppl保护，无需进行复杂的设置或使用本地调试器。
其特点如下：

+ 移除指定PID的PPL保护
+ 移除ObCallback（允许使用PROCESS_ALL_ACCESS打开进程）
+ 移除CmCallback（允许编辑任何注册表项）
+ 设置镜像文件执行选项以劫持目标可执行文件
+ 支持各种用作功能“提供者”的易受攻击的驱动程序。
#### 使用方法

###### DEK -list
###### DEK -prv ProviderID
###### DEK -ps ProcessID
###### DEK -ob
###### DEK -cm
###### DEK -kill ProcessID
###### DEK -hi ExePath DebuggerPath
* -list - 列出当前可用的 provider
* -prv  - 可选，选择易受攻击的驱动程序提供程序；
* -ps   - 修改给定 ProcessID 的进程对象，降低任何保护级别；
* -ob   - 移除所有 ObCallback
* -cm   - 移除所有 CmCallback
* -kill - 使用 WinStationTerminateProcess 来终止给定的进程
* -hi   - 映像劫持以防止它们重启

Example:
+ dek -ps 1234
+ dek -prv 1 -ps 1234
+ dek -ob
+ dek -cm
+ dek -ps pid -ob -cm -kill pid

Windows 11 24H2 运行结果*

# 支持的 provider

注意：若未指定 -prv 命令，则默认使用ID为34的提供程序。

并非 KDU 中的所有提供程序都列在下表，因为其中一些已被 Windows ban 掉了，加载它们将导致出现警告。

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

微软黑名单类型：

证书 - 通过用于签署驱动程序的证书，可以一次性禁用大量文件。

名称 - 通过存储在VERSION_INFO文件资源中的原始文件名条目，此类禁止仅在文件版本交叉检查时才可能实现，否则，如果驱动程序是“已修复/未受影响”的版本，则会导致误报。

哈希/页面哈希 - 通过Authenticode哈希/页面哈希，微软可以禁用特定的驱动程序文件。

# 工作原理

与KDU类似：它使用 BYOVD 思路，通过漏洞驱动访问任意内核内存：

KDU提供了一个用于使用易受攻击驱动程序的抽象接口，使我们能够读写内核的物理/虚拟内存。因此，我们可以搜索内核以获取多个未导出的变量(例如：修改 EPROCESS)。手动解析内核PE文件，并使用特征匹配来获取 ObCallback 等链表头的偏移量。

然后，通过隔离链表头自环禁用 CmCallback。通过将回调函数指针设置为 0 来禁用 ObCallback。通过将给定值写入 PS_PROTECTION 结构来移除/设置 PPL。

# 构建与说明

DEK附带了完整的源代码。若要从源代码构建，您需要安装Microsoft Visual Studio 2022或更高版本。

完整的可执行文件是DEK.exe，其中包含驱动程序数据库和主函数。

# 报告错误和不兼容问题

与KDU类似：

如果您在使用DEK与第三方软件或操作系统时遇到错误或兼容性问题，请随时提交问题。但是，如果这种不兼容性是由您自己的操作引起的，则此类报告将被忽略。

反作弊和反恶意软件的不兼容问题将被忽略，这由您自行负责。

# 免责声明

同样与KDU相似：

使用此程序可能会导致电脑蓝屏死机（BSOD）。所提供的编译二进制文件和源代码均按原样提供，希望它们能有所帮助，但不提供任何形式的担保。由于DEK依赖于完全有漏洞且易受攻击的驱动程序，因此强烈建议仅在虚拟机上使用。

### 有趣的故事 -- 为什么命名为“DreamEx_Killer”

这个想法来自Bilibili上一位名为“缘梦DreamEx”的 up 主。

他上传了一段视频，描述了如何使用易受攻击的驱动程序（WinIO）来读写物理/虚拟内存，并移除操作系统（ob/cm）回调和进程保护列表（PPL）以终止反病毒软件进程（360、huorong等），并设置映像劫持以阻止它们重新启动。

尽管他的账号后来被封禁了（可能就是这个视频导致的），但我在评论区下载了示例 exe 文件（该文件仅由 UPX 打包，我可以轻松地对其进行反汇编）:P。

在逆向了整个文件并从 KDU 项目中学习后，我重写了内存读写接口（将其从单个易受攻击的驱动程序（WinIO）扩展到多个易受攻击的驱动程序（见 provider 列表）），并补完了视频中未展示的代码。

最后，到了给项目取名字的时候了。我需要一个有点冲击力的名字（即使是一个听起来有点“中二”的名字。。）然后我一拍脑袋想到了“DreamEx_Killer”。所以，

就是这个！！！

# 引用

KDU,https://github.com/hfiref0x/KDU

缘梦DreamEx's Video.(在项目中)