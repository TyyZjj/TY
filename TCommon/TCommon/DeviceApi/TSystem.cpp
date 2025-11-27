#ifdef WIN32
#include <windows.h>
#include <Psapi.h>
#include <TlHelp32.h>
#endif
#include <QDir>
#include <QMutex>
#include <QSettings>
#include <QApplication>
#include <QSharedMemory>
#include "TSystem.h"
#include "TDefine.h"

bool TSystem::IsWow64()
{
	bool bIsWow64 = false;
#ifdef WIN32 
#ifndef _WIN64//64位程序肯定运行在64位操作系统中
	LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
		GetModuleHandle(TEXT("kernel32")), "IsWow64Process");
	if (NULL != fnIsWow64Process)
		fnIsWow64Process(GetCurrentProcess(), &bIsWow64);
#endif
#endif 
	return bIsWow64;
}

void TSystem::PowerBoot(bool isStart /*= true*/)
{
	//注册表路径需要使用双反斜杠，如果是32位系统，要使用QSettings::Registry32Format
#ifdef WIN32 
	QSettings *pSettings(nullptr);
	QString strPowerBootRegisterPath = "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
#ifdef _WIN64//64位程序肯定运行在64位操作系统中
	pSettings = new QSettings(strPowerBootRegisterPath, QSettings::Format::Registry64Format);
#else//32位程序
	BOOL isWow64Process(false);
	HANDLE handle = GetCurrentProcess();
	IsWow64Process(handle, &isWow64Process);
	if (isWow64Process)//32位程序运行在64位操作系统中
		pSettings = new QSettings(strPowerBootRegisterPath, QSettings::Format::Registry64Format);
	else
		pSettings = new QSettings(strPowerBootRegisterPath, QSettings::Format::Registry32Format);
#endif // _WIN64

	//获取应用名称
	QString appname = QApplication::applicationName();
	if (isStart)
	{
		QString apppath = QDir::toNativeSeparators(QApplication::applicationFilePath());//找到应用的目录
		pSettings->setValue(appname, apppath);//写入注册表
	}
	else
	{
		pSettings->remove(appname);
	}

	if (pSettings)
	{
		delete pSettings;
		pSettings = nullptr;
	}
#endif
}

bool TSystem::IsSystemSingle(const QString& identifier)
{
	QSharedMemory *shareMem = new QSharedMemory(identifier);
	volatile short i = 2;

	while (i--)
	{
		if (shareMem->attach(QSharedMemory::ReadOnly))
			shareMem->detach();
	}
	return shareMem->create(1);
}

QList<unsigned long> TSystem::GetProcIdByName(const QString &strProcessName)
{
	QList<unsigned long> lstProcessId;
#ifdef WIN32
	//DWORD dwProcs[1024 * 2];
	//memset(dwProcs, 0, sizeof(dwProcs));
	//DWORD dwNeeded(0);
	//EnumProcesses(dwProcs, sizeof(dwProcs), &dwNeeded);
	//int iProcessCount = dwNeeded / sizeof(DWORD);

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);//拍摄进程快照
	if (hSnapshot == INVALID_HANDLE_VALUE)
		return lstProcessId;
	/*
	typedef struct tagPROCESSENTRY32W
	{
		DWORD   dwSize;					//结构体大小
		DWORD   cntUsage;				//不再使用
		DWORD   th32ProcessID;          // this process 进程id
		ULONG_PTR th32DefaultHeapID;	//不再使用
		DWORD   th32ModuleID;           // associated exe 不再使用
		DWORD   cntThreads;				// 该进程当前有多少子线程
		DWORD   th32ParentProcessID;    // this process's parent process 父进程id
		LONG    pcPriClassBase;         // Base priority of process's threads 此进程创建的任何线程的基本优先级
		DWORD   dwFlags;				//	不再使用
		WCHAR   szExeFile[MAX_PATH];    // Path exe路径
	} PROCESSENTRY32W;
	*/
	PROCESSENTRY32W ps;
	ZeroMemory(&ps, sizeof(PROCESSENTRY32W));
	ps.dwSize = sizeof(PROCESSENTRY32W);
	if (!Process32FirstW(hSnapshot, &ps))
	{
		CloseHandle(hSnapshot);
		return lstProcessId;
	}

	do 
	{
		QString strExeFile = QString::fromUtf16((ushort*)ps.szExeFile);
		if (strExeFile == strProcessName)
			lstProcessId.append(ps.th32ProcessID);
	} while (Process32NextW(hSnapshot, &ps));
	CloseHandle(hSnapshot);
#endif // WIN32
	return lstProcessId;
}

QString TSystem::GetProcName(unsigned long iProcessId)
{
	QString strProcName;
#ifdef WIN32
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);//拍摄进程快照
	if (hSnapshot == INVALID_HANDLE_VALUE)
		return strProcName;

	PROCESSENTRY32W ps;
	ZeroMemory(&ps, sizeof(PROCESSENTRY32W));
	ps.dwSize = sizeof(PROCESSENTRY32W);
	if (!Process32FirstW(hSnapshot, &ps))
	{
		CloseHandle(hSnapshot);
		return strProcName;
	}

	do
	{
		if (ps.th32ProcessID == iProcessId)
		{
			strProcName = QString::fromUtf16((ushort*)ps.szExeFile);
			break;
		}
	} while (Process32NextW(hSnapshot, &ps));
	CloseHandle(hSnapshot);
#endif // WIN32
	return strProcName;
}

QMap<unsigned long, QList<unsigned long>> TSystem::GetProcThreadIdByName(const QString &strProcessName)
{
	QMap<unsigned long, QList<unsigned long>> mapProcessThreadIdList;
#ifdef WIN32
	QList<unsigned long> lstProcessId = GetProcIdByName(strProcessName);
	if (lstProcessId.isEmpty())
		return mapProcessThreadIdList;

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0); // 系统所有线程快照
	if (hSnapshot == INVALID_HANDLE_VALUE)
		return mapProcessThreadIdList;
																	   
	/*
	typedef struct tagTHREADENTRY32
	{
		DWORD   dwSize;				//结构体大小
		DWORD   cntUsage;			//不再使用, 0
		DWORD   th32ThreadID;       // this thread
		DWORD   th32OwnerProcessID; // Process this thread is associated with
		LONG    tpBasePri;			//分配给线程的内核基本优先级. 
						//优先级是一个从0到31的数字, 0代表可能的最低线程优先级. 更多信息参见KeQueryPriorityThread
		LONG    tpDeltaPri;			//不再使用, 0
		DWORD   dwFlags;			//不再使用, 0
	} THREADENTRY32;
	*/
	THREADENTRY32 te;
	ZeroMemory(&te, sizeof(THREADENTRY32));
	te.dwSize = sizeof(THREADENTRY32);
	if (!Thread32First(hSnapshot, &te))
	{
		CloseHandle(hSnapshot);
		return mapProcessThreadIdList;
	}

	do 
	{
		if (lstProcessId.contains(te.th32OwnerProcessID))//其中第一个线程id为该进程对应的主线程id
		{
			QList<unsigned long> &lstProcessThreadId = mapProcessThreadIdList[te.th32OwnerProcessID];
			lstProcessThreadId.append(te.th32ThreadID);
		}
	} while (Thread32Next(hSnapshot, &te));
	CloseHandle(hSnapshot);

#endif // WIN32
	return mapProcessThreadIdList;
}

QList<QString> TSystem::GetProcModuleNames(unsigned long iProcessId)
{
	QList<QString> lstModuleName;
#ifdef WIN32
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, iProcessId);//拍摄进程快照
	if (hSnapshot == INVALID_HANDLE_VALUE)
		return lstModuleName;

	MODULEENTRY32W md;
	ZeroMemory(&md, sizeof(MODULEENTRY32W));
	md.dwSize = sizeof(MODULEENTRY32W);
	if (!Module32FirstW(hSnapshot, &md))
	{
		CloseHandle(hSnapshot);
		return lstModuleName;
	}

	do 
	{
		if (md.th32ProcessID == iProcessId)
		{
			QString strModuleName = QString::fromUtf16((ushort*)md.szModule);
			lstModuleName.append(strModuleName);
		}
	} while (Module32NextW(hSnapshot, &md));
	CloseHandle(hSnapshot);

#endif // WIN32
	return lstModuleName;
}

#ifdef WIN32
QList<MODULEENTRY32> TSystem::GetProcModules(unsigned long iProcessId)
{
	QList<MODULEENTRY32> lstModules;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, iProcessId);//拍摄进程快照
	if (hSnapshot == INVALID_HANDLE_VALUE)
		return lstModules;

	MODULEENTRY32 md;
	ZeroMemory(&md, sizeof(MODULEENTRY32W));
	md.dwSize = sizeof(MODULEENTRY32W);
	if (!Module32First(hSnapshot, &md))
	{
		CloseHandle(hSnapshot);
		return lstModules;
	}
	do
	{
		if (md.th32ProcessID == iProcessId)
			lstModules.append(md);
	} while (Module32Next(hSnapshot, &md));
	CloseHandle(hSnapshot);

	return lstModules;
}
#endif // WIN32

HWND TSystem::GetWidByProcId(DWORD dwProcessId, const CHAR *szTitle, QStringList &lstTitle)
{
	struct PorcessTopWnd
	{
		HWND hwnd;
		DWORD dwProcessId;
		CHAR szTitle[256];
		QStringList lstTitle;//所有该进程已枚举的所有窗口的名称, 当目标窗口存在时, 只枚举到目标窗口. 当目标窗口不存在时, 枚举所有窗口
	};
	PorcessTopWnd processTopwnd = { 0 };
	processTopwnd.dwProcessId = dwProcessId;
#ifdef WIN32
	lstrcpyA(processTopwnd.szTitle, szTitle);
	EnumWindows([](HWND hwnd, LPARAM lparam) {
		PorcessTopWnd *processTopWnd = (PorcessTopWnd *)lparam;

		DWORD dwProcessId = 0;
		GetWindowThreadProcessId(hwnd, &dwProcessId);
		if (dwProcessId == processTopWnd->dwProcessId)
		{
			if (lstrcmpA(processTopWnd->szTitle, "") == 0)
			{
				processTopWnd->hwnd = hwnd;
				return TRUE;
			}
			CHAR szTitle[128] = ("");
			::GetWindowTextA(hwnd, szTitle, 128);
			processTopWnd->lstTitle.append(szTitle);
			if (lstrcmpA(szTitle, processTopWnd->szTitle) == 0)
			{
				processTopWnd->hwnd = hwnd;
				return FALSE;
			}
		}
		return TRUE;
	}, (LPARAM)&processTopwnd);
	lstTitle = processTopwnd.lstTitle;
#endif
	return processTopwnd.hwnd;
}

int TSystem::GetProcessorNumber()
{
	int iNum(0);
#ifdef WIN32
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	iNum = (int)info.dwNumberOfProcessors;
#endif // WIN32
	return iNum;
}

#ifdef WIN32
__int64 file_time_2_utc(const FILETIME* ftime)
{
	LARGE_INTEGER li;
	li.LowPart = ftime->dwLowDateTime;
	li.HighPart = ftime->dwHighDateTime;
	return li.QuadPart;
}
#endif // WIN32

int TSystem::GetCpuUsage()
{
	int iCpuUsage(-1);
#ifdef WIN32
	static FILETIME preidleTime;
	static FILETIME prekernelTime;
	static FILETIME preuserTime;

	FILETIME idleTime;
	FILETIME kernelTime;
	FILETIME userTime;

	GetSystemTimes(&idleTime, &kernelTime, &userTime);

	quint64 a, b;
	int idle, kernel, user;

	a = file_time_2_utc(&preidleTime);//(preidleTime.dwHighDateTime << 31) | preidleTime.dwLowDateTime;
	b = file_time_2_utc(&idleTime);//(idleTime.dwHighDateTime << 31) | idleTime.dwLowDateTime;
	idle = b - a;

	a = file_time_2_utc(&prekernelTime);// (prekernelTime.dwHighDateTime << 31) | prekernelTime.dwLowDateTime;
	b = file_time_2_utc(&kernelTime);//(kernelTime.dwHighDateTime << 31) | kernelTime.dwLowDateTime;
	kernel = b - a;

	a = file_time_2_utc(&preuserTime);// (preuserTime.dwHighDateTime << 31) | preuserTime.dwLowDateTime;
	b = file_time_2_utc(&userTime); //(userTime.dwHighDateTime << 31) | userTime.dwLowDateTime;
	user = b - a;

	iCpuUsage = (kernel + user - idle) * 100 / (kernel + user);

	preidleTime = idleTime;
	prekernelTime = kernelTime;
	preuserTime = userTime;
#endif // WIN32
	return iCpuUsage;
}

int TSystem::GetProcCpuUsage(void *handle /*= nullptr*/)
{
	int iCpuUsage(-1);
#ifdef WIN32
	if (handle == nullptr)
		handle = GetCurrentProcess();
	static void * pHandle(nullptr);
	static int iProcessorCount = GetProcessorNumber();
	static QMutex mutex;
	mutex.lock();

	static long long last_time(0);
	static long long last_system_time(0);
	if (pHandle != handle)//新的进程, 之前的进程可以继续上一步
	{
		pHandle = handle;
		last_time = 0;
		last_system_time = 0;
	}

	FILETIME now;
	FILETIME creation_time;
	FILETIME exit_time;
	FILETIME kernel_time;
	FILETIME user_time;
	long long system_time;
	long long time;
	long long system_time_delta;
	long long time_delta;
	GetSystemTimeAsFileTime(&now);
	if (!GetProcessTimes(pHandle, &creation_time, &exit_time, &kernel_time, &user_time))
		return iCpuUsage;
	system_time = (file_time_2_utc(&kernel_time) + file_time_2_utc(&user_time)) / iProcessorCount;
	time = file_time_2_utc(&now);
	if ((last_system_time == 0) || (last_time == 0))//当前进程第一次GetProcessTimes
	{
		last_system_time = system_time;
		last_time = time;
		mutex.unlock();
		return iCpuUsage;
	}
	system_time_delta = system_time - last_system_time;
	time_delta = time - last_time;
	if (time_delta == 0)
	{
		mutex.unlock();
		return iCpuUsage;
	}
	iCpuUsage = (int)((system_time_delta * 100 + time_delta / 2) / time_delta);
	last_system_time = system_time;
	last_time = time;

	mutex.unlock();
#endif // WIN32
	return iCpuUsage;
}


int TSystem::GetProcCpuUsage(unsigned long iProcessId)
{
	if (iProcessId == 0)
		GetProcCpuUsage();

	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, iProcessId);
	if (handle)
		return GetProcCpuUsage(handle);
	return 0;
}

void TSystem::SetProcCpuMask(int mask, void* handle /*= nullptr*/)
{
#ifdef WIN32
	if (mask > 0)
	{
		if (handle)
			SetProcessAffinityMask(handle, mask);
		else
			SetProcessAffinityMask(GetCurrentProcess(), mask);
	}
#endif // WIN32
}

void TSystem::SetThreadCpuMask(int mask, void* handle /*= nullptr*/)
{
#ifdef WIN32
	if (mask > 0)
	{
		if (handle)
			SetThreadAffinityMask(handle, mask);
		else
			SetThreadAffinityMask(GetCurrentThread(), mask);
	}
#endif // WIN32
}

void TSystem::GetMemory(int& memorypercent, int& memoryuse, int& memoryfree, int& memoryall)
{
#ifdef WIN32
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);
	memorypercent = statex.dwMemoryLoad;
	memoryall = statex.ullTotalPhys / MB;
	memoryfree = statex.ullAvailPhys / MB;
	memoryuse = memoryall - memoryfree;
#endif
}



