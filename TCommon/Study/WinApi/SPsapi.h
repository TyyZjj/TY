#ifndef _SPSAPI_H
#define _SPSAPI_H

#ifdef WIN32
#include <Psapi.h>
#endif // WIN32

//一、重要结构体
	
	


//二、重要函数
	//1、枚举系统中的每个进程对象
	/*
	EnumProcesses(
		_Out_writes_bytes_(cb) DWORD* lpidProcess,
		_In_ DWORD cb,
		_Out_ LPDWORD lpcbNeeded
	);
	*/

	//2、枚举进程中的每个模块
	/*
	EnumProcessModules/EnumProcessModulesEx
	*/




#endif
