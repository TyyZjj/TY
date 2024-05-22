#pragma once

#include "tcommon_global.h"



class TCOMMON_EXPORT TCommon
{
public:
	TCommon();

	//1、程序退出后, 后台进程存在残余. system("taskkill /f /im XXX.exe") + QApplication::applicationName();
};
