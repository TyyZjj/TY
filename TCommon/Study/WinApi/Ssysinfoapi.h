#ifndef _SSYSINFOAPI_H_
#define _SSYSINFOAPI_H_

#ifdef WIN32
#include <sysinfoapi.h>
#endif // WIN32

//一、重要结构体
/*
	1、系统性的信息
	typedef struct _SYSTEM_INFO {
	union {
		DWORD dwOemId;          // Obsolete field...do not use
		struct {
			WORD wProcessorArchitecture; //处理器结构类型, 可以是以下值:
				PROCESSOR_ARCHITECTURE_AMD64	9	->x64 (AMD or Intel)
				PROCESSOR_ARCHITECTURE_ARM		5	->ARM
				PROCESSOR_ARCHITECTURE_ARM64	12	->ARM64
				PROCESSOR_ARCHITECTURE_IA64		6	->Intel Itanium-based
				PROCESSOR_ARCHITECTURE_INTEL	0	->x86
				PROCESSOR_ARCHITECTURE_UNKNOWN	0xffff	->未知
			WORD wReserved;		//保留字
		} DUMMYSTRUCTNAME;
	} DUMMYUNIONNAME;
	DWORD dwPageSize;			//分页大小(用以申请内存时的最小内存粒度, 参见 VirtualAlloc.)
	LPVOID lpMinimumApplicationAddress;//应用程序和dll等可访问的最低内存地址
	LPVOID lpMaximumApplicationAddress;//应用程序和dll等可访问的最高内存地址
	DWORD_PTR dwActiveProcessorMask;//处理器集的掩码.位0是处理器0;位31是处理器31;
	DWORD dwNumberOfProcessors;//处理器个数 ->具体信息GetLogicalProcessorInformation
	DWORD dwProcessorType;//过时参数
	DWORD dwAllocationGranularity;//分配虚拟内存时, 起始地址的粒度, 参见 VirtualAlloc.
	WORD wProcessorLevel;//处理器级别(用处不大)
	WORD wProcessorRevision;//处理器版本
} SYSTEM_INFO, *LPSYSTEM_INFO;

	2、物理内存和虚拟内存(包括扩展内存)当前状态的信息
	typedef struct _MEMORYSTATUSEX {
	DWORD dwLength;//结构体大小, 调用GlobalMemoryStatusEx之前, 要先设置此值;
	DWORD dwMemoryLoad;//[0,100]正使用的物理内存百分比, 100表示内存已满.
	DWORDLONG ullTotalPhys;//实际物理内存大小,单位:字节.
	DWORDLONG ullAvailPhys;//当前可用的物理内存大小, 单位:字节.
		(可以立即使用的物理内存大小, 不包括需要输出到磁盘的缓冲)
	DWORDLONG ullTotalPageFile;//系统或者当前进程的提交内存限制, 单位:字节.
		要获取系统范围内的内存提交限制 -> GetPerformanceInfo
	DWORDLONG ullAvailPageFile;//当前进程可以提交的最大内存量, 单位:字节. 此值小于ullTotalPageFile.
		要计算系统范围内的可用提交内存, 调用GetPerformanceInfo后, 从CommitLimit的值减去CommitTotal
	DWORDLONG ullTotalVirtual;//进程虚拟空间中, 用户可申请(用户模式)的内存大小,单位:字节.
	DWORDLONG ullAvailVirtual;//进程虚拟空间中, 用户可申请的内存中, 未保存和未提交的内存大小, 单位:字节.
	DWORDLONG ullAvailExtendedVirtual;//保留字
} MEMORYSTATUSEX, *LPMEMORYSTATUSEX;
*/

//二、函数
/*
	1、	BOOL GlobalMemoryStatusEx(LPMEMORYSTATUSEX lpBuffer);//系统当前物理内存和虚拟内存使用情况
	2、	GetSystemInfo(LPSYSTEM_INFO lpSystemInfo);//返回系统信息
		GetNativeSystemInfo: 供在64位操作系统上运行的32位进程 获取系统信息(此时不应使用GetSystemInfo)
	3、	GetSystemTime(LPSYSTEMTIME lpSystemTime)/SetSystemTime;//返回当前系统的UTC时间
		GetSystemTimeAsFileTime(LPFILETIME lpSystemTimeAsFileTime);//返回当前系统的UTC时间(参见LPFILETIME和LPSYSTEMTIME的区别)
		GetSystemTimePreciseAsFileTime: //尽可能高的精度(<1us)检索当前系统日期和时间
	4、	GetLocalTime(LPSYSTEMTIME lpSystemTime)/SetLocalTime(SYSTEMTIME* lpSystemTime);
			返回本地时间
	5、	DWORD GetVersion();//版本号 例:
			{
				DWORD dwVersion = 0;
				DWORD dwMajorVersion = 0;
				DWORD dwMinorVersion = 0;
				DWORD dwBuild = 0;
				dwVersion = GetVersion();
				dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
				dwMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));
				if (dwVersion < 0x80000000)
					dwBuild = (DWORD)(HIWORD(dwVersion));
			}
		GetVersionExA/GetVersionExW
	6、	DWORD GetTickCount();//系统启动以来经过的毫秒数, 最长为49.7天
		ULONGLONG GetTickCount64();//系统启动以来经过的毫秒数
	7、	GetSystemTimeAdjustment/SetSystemTimeAdjustment
			确定系统是否对其时间时钟应用定期时间调整, 并获取任何此类调整的值和周期
		BOOL GetSystemTimeAdjustmentPrecise/SetSystemTimeAdjustmentPrecise;
			确定系统是否对其时间时钟应用周期性编程时间调整, 并获取任何此类调整的值和周期
	8、	GetSystemDirectoryA/GetSystemDirectoryW
			返回系统目录, C:\Windows\System32
	9、	GetWindowsDirectoryA/GetWindowsDirectoryW
			返回Windows目录, C:\Windows
		GetSystemWindowsDirectoryA/GetSystemWindowsDirectoryW
			多用户系统上共享Windows目录的路径, C:\Windows
	10、GetComputerNameExA/GetComputerNameExW
		SetComputerNameA,SetComputerNameW
		SetComputerNameExA
		SetComputerNameExW,
		SetComputerNameEx2W
		DnsHostnameToComputerNameExW
			获取计算机名称, 范例如下:
				NetBIOS	DESKTOP-G72T22C
				DNS hostname					DESKTOP-G72T22C
				DNS domain						""
				DNS fully-qualified				DESKTOP-G72T22C
				Physical NetBIOS				DESKTOP-G72T22C
				Physical DNS hostname			DESKTOP-G72T22C
				Physical DNS domain				""
				Physical DNS fully-qualified	DESKTOP-G72T22C
	11、GetLogicalProcessorInformation:处理器和相关硬件的信息
		(NUMA节点数、物理CPU数、CPU核心数、逻辑CPU数、各级Cache等)
		GetLogicalProcessorInformationEx:处理器和相关硬件之间关系的信息
	12、GetProductInfo:本地计算机上操作系统的产品类型
	13、EnumSystemFirmwareTables:枚举指定类型的所有系统固件表(使用时再详细参阅, Win10可能已不支持)
		GetSystemFirmwareTable:从固件表提供程序检索指定的固件表
	14、GetPhysicallyInstalledSystemMemory: 返回RAM内存的大小, 单位KB
	15、InstallELAMCertificateInfo: 
			安装资源文件中指定的证书信息, 该文件在生成时链接到ELAM驱动程序;
			反恶意软件供应商使用此API, 使用用户模式启动受保护的反恶意软件服务;
	16、GetProcessorSystemCycleTime: 处理器自激活以来执行延迟过程调用(DPC)和中断服务例程(ISR)所用的周期时间
	17、GetIntegratedDisplaySize:检索屏幕大小的最佳估计值, 单位:英寸


*/

#endif // !_SSYSINFOAPI_H_
