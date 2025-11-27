#ifndef _TSYSTEM_H_
#define _TSYSTEM_H_
/*系统和硬件相关*/

#include <QVersionNumber>
#include <QOperatingSystemVersion>
#include "TProcessThreads.h"

#ifdef WIN32
#include <TlHelp32.h>
#endif // WIN32


class TSystem
{
public:
	//1、线程所在处理器 processthreadsapi.h -> GetCurrentProcessorNumber/GetCurrentProcessorNumberEx
	//2、获取当前系统中所有的进程Id
		//->进程快照:CreateToolhelp32Snapshot 或 EnumProcesses枚举进程
		
	//是否是32位进程运行于64位操作系统中
	static bool IsWow64();

	//程序开机自启
	static void PowerBoot(bool isStart = true);

	//************************************
	// 参数: 	const QString & identifier: 识别码
	// 返回:	bool
	// 功能:	是否在整个系统运行的内存中是唯一的
	//************************************
	static bool IsSystemSingle(const QString& identifier);

#pragma region 进程线程模块信息
	//************************************
	// 参数: 	const QString & strProcessName: XXX.exe
	// 返回:	QList<unsigned long>
	// 功能:	根据进程名称查找进程id
	//************************************
	static QList<unsigned long> GetProcIdByName(const QString &strProcName);
	//根据进程Id查找其进程名(.exe的名称)
	static QString GetProcName(unsigned long iProcessId);

	//************************************
	// 参数: 	const QString & strProcessName: XXX.exe
	// 返回:	QMap<进程ID, QList<该进程对应的所有线程>>  //注: 其中, list中第一个id是主线程id
	// 功能:	根据进程名称查找其内部线程id
	//************************************
	static QMap<unsigned long, QList<unsigned long>> GetProcThreadIdByName(const QString &strProcessName);

	//获取进程下所有的模块名
	static QList<QString> GetProcModuleNames(unsigned long iProcessId);

#ifdef WIN32
	//获取进程下所有的模块信息
	static QList<MODULEENTRY32> GetProcModules(unsigned long iProcessId);
#endif // WIN32

	static HWND GetWidByProcId(DWORD dwProcessId, const CHAR *szTitle, QStringList &lstTitle);
#pragma endregion


#pragma region CPU相关
	//获取CPU核心数
	static int GetProcessorNumber();
	//************************************
	// 返回:	int
	// 功能:	获取CPU利用率(百分比)
	//************************************
	static int GetCpuUsage();
	//************************************
	// 参数: 	void * handle: 进程句柄, 为nullptr时则获取当前进程
	// 返回:	int
	// 功能:	获取进程的CPU利用率(百分比), (注意: 针对某个进程, 第一次调用此函数时, 此函数总是返回-1)
	//************************************
	static int GetProcCpuUsage(void *handle = nullptr);
	static int GetProcCpuUsage(unsigned long iProcessId);
	//static int GetThreadCpuUsage(void *handle = nullptr);//线程的cpu利用率, 意义不大

	//************************************
	// 参数: 	int mask: 核心数 [0~64]
	// 参数: 	void* handle: 进程句柄
	// 返回:	void
	// 功能:	设置进程(线程)使用最多使用的核心数
	// 指定进程线程使用哪个/哪些CPU
	//	GetSystemCpuSetInformation
	//	GetProcessDefaultCpuSets
	//	SetProcessDefaultCpuSets
	//	GetThreadSelectedCpuSets
	//	SetThreadSelectedCpuSets
	//	SetProcessAffinityMask
	//	SetThreadAffinityMask(handle, mask);
	//************************************
	static void SetProcCpuMask(int mask, void* handle = nullptr);
	static void SetThreadCpuMask(int mask, void* handle = nullptr);
#pragma endregion


#pragma region IO相关
	//系统的io速率
	//进程的io速率
	//winioctl.h _FILESYSTEM_STATISTICS
#pragma endregion


#pragma region 内存相关
	//************************************
	// 参数: 	int & memorypercent: 内存已占用比例
	// 参数: 	int & memoryuse: 已使用的内存 单位:MB
	// 参数: 	int & memoryfree: 空余的内存 单位:MB
	// 参数: 	int & memoryall: 总的内存 单位:MB
	// 返回:	void
	// 功能:	获取当前内存状态
	//************************************
	static void GetMemory(int& memorypercent, 
		int& memoryuse, 
		int& memoryfree, 
		int& memoryall);

	//进程的内存使用量 GetProcessMemoryInfo
#pragma endregion
};



#endif


