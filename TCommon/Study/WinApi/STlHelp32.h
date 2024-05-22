#ifndef _STLHELP32_H_
#define _STLHELP32_H_

#ifdef WIN32
#include <TlHelp32.h>
#endif // WIN32


//一、定义和结构体
//1、进程快照参数定义
/*
TH32CS_INHERIT: 快照句柄是可继承的
TH32CS_SNAPALL: 系统中的所有进程和线程, 以及 th32ProcessID 中指定的进程的堆和模块
TH32CS_SNAPHEAPLIST: 快照中 th32ProcessID 中指定的进程的所有堆
TH32CS_SNAPMODULE: 快照中 th32ProcessID 中指定的进程的所有模块
TH32CS_SNAPMODULE32: 从64位进程调用时, 包括快照中 th32ProcessID 中指定的进程的所有32位模块
TH32CS_SNAPPROCESS: 快照中系统中的所有进程
TH32CS_SNAPTHREAD: 快照中系统中的所有线程

typedef struct tagPROCESSENTRY32 {
  DWORD     dwSize; // 该结构体的大小。
  DWORD     cntUsage; // 必须为 0。
  DWORD     th32ProcessID; // 进程标识符。
  ULONG_PTR th32DefaultHeapID; // 必须为 0。
  DWORD     th32ModuleID; // 必须为 0。
  DWORD     cntThreads; // 进程启动的执行线程数。
  DWORD     th32ParentProcessID; // 创建此进程的进程的标识符。
  LONG      pcPriClassBase; // 此进程创建的任何线程的基本优先级。
  DWORD     dwFlags; // 必须为 0。
  CHAR      szExeFile[MAX_PATH]; // 进程的可执行文件的名称。
} PROCESSENTRY32;

typedef struct tagMODULEENTRY32 {
  DWORD   dwSize;  // 该结构体的大小。
  DWORD   th32ModuleID;  // 必须为 0。
  DWORD   th32ProcessID; // 要检查其模块的进程的标识符.
  DWORD   GlblcntUsage; // 模块加载数目，通常设置为 0xFFFF。
  DWORD   ProccntUsage; // 同上。
  BYTE    *modBaseAddr; // 在所属进程的上下文中模块的基地址
  DWORD   modBaseSize; // 模块的大小.
  HMODULE hModule; // 在所属进程的上下文中对模块的句柄。
  char    szModule[MAX_MODULE_NAME32 + 1]; // 模块名称。
  char    szExePath[MAX_PATH]; // 模块路径。
} MODULEENTRY32;

typedef struct tagTHREADENTRY32 {
  DWORD dwSize; // 该结构体大小。
  DWORD cntUsage; // 必须为 0。
  DWORD th32ThreadID; // 线程标识符。
  DWORD th32OwnerProcessID; // 创建该线程的进程的标识符。
  LONG  tpBasePri; // 分配给线程的内核基本优先级级别(0~31)(优先级最低为 0)。
  LONG  tpDeltaPri; // 必须为 0。
  DWORD dwFlags; // 必须为 0。
} THREADENTRY32;

typedef struct tagHEAPLIST32 {
  SIZE_T  dwSize; // 该结构体的大小。
  DWORD   th32ProcessID; // 要检查的进程的标识符。
  ULONG_PTR th32HeapID; // 堆标识符。
  DWORD   dwFlags; // 通常为 HF32_DEFAULT。
} HEAPLIST32;
*/


#endif
