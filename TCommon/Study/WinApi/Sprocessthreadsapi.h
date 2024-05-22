#ifndef _SPROCESSTHREADSAPI_H_
#define _SPROCESSTHREADSAPI_H_

#ifdef WIN32
#include <processthreadsapi.h>
#endif // WIN32

//参见:https://docs.microsoft.com/zh-cn/Windows/win32/procthread/process-and-thread-functions


//一、重要结构体

	//1、进程信息
	/*typedef struct _PROCESS_INFORMATION {
		HANDLE hProcess;
		HANDLE hThread;
		DWORD dwProcessId;
		DWORD dwThreadId;
	} PROCESS_INFORMATION, *PPROCESS_INFORMATION, *LPPROCESS_INFORMATION;
	*/

	//2、进程启动信息
	/*
	typedef struct _STARTUPINFOA {
		DWORD   cb;	//结构体大小
		LPSTR   lpReserved; //保留字
		LPSTR   lpDesktop; //桌面名称(桌面和窗口的名称)
		LPSTR   lpTitle; //窗口标题
		DWORD   dwX;
			如果dwFlags指定STARTF_USEPOSITION, 则是创建新窗口时窗口左上角的x偏移量(像素为单位)
			否则dwX将被忽略
			GUI进程, 若CreateWindow的x参数为CW_USEDEFAULT, 新进程第一次调用CreateWindow以创建重叠窗口时, 将使用指定的位置
		DWORD   dwY;
		DWORD   dwXSize;
			如果dwFlags指定STARTF_USESIZE, 则是创建新窗口时窗口的宽度(像素为单位)
			否则dwX将被忽略
		DWORD   dwYSize;
		DWORD   dwXCountChars;
			如果dwFlags指定STARTF_USECOUNTCHARS, 如果在控制台进程中创建新的控制台窗口, 该成员将在字符列中指定屏幕缓冲区宽度
		DWORD   dwYCountChars;
		DWORD   dwFillAttribute;
			如果dwFlags指定STARTF_USEFILLATTRIBUTE, 如果在控制台应用程序中创建了新的控制台窗口, 则该成员是初始文本和背景色
			可选值:
				FOREGROUND_BLUE, 
				FOREGROUND_GREEN, 
				FOREGROUND_RED, 
				FOREGROUND_INTENSITY, 
				BACKGROUND_BLUE, 
				BACKGROUND_GREEN, 
				BACKGROUND_RED, 
				BACKGROUND_INTENSITY
		DWORD   dwFlags; 确认创建进程时, STARTUPINFOA中哪些字段会生效. 可选值:
			STARTF_FORCEONFEEDBACK: 创建进程后, 光标处于反馈模式2秒钟. 
				如果在此期间, 进程进行了第一次GUI调用, 则系统会再给进程五秒钟的时间.
				如果五秒钟内进程显示一个窗口, 系统会再给进程五秒钟时间来完成窗口的绘制.
				系统在第一次调用GetMessage后关闭反馈光标，无论进程是否正在绘制.
			STARTF_FORCEOFFFEEDBACK
				进程启动时强制关闭反馈光标. 此时将显示正常的选择光标
			STARTF_PREVENTPINNING
				进程创建的任何窗口都不能固定在任务栏上. 必须与STARTF_TITLEISAPPID结合使用.
			STARTF_RUNFULLSCREEN
				进程应在全屏模式下运行，而不是窗口模式.仅对在x86计算机上运行的控制台应用程序有效.
			STARTF_TITLEISAPPID
				lpTitle成员包含一个AppUserModelID. 控制任务栏和"开始"菜单显示应用程序, 并使其与正确的快捷方式和跳转列表相关联.
				不能与STARTF_TITLEISLINKNAME一起使用
			STARTF_TITLEISLINKNAME
				lpTitle包含用户为启动此进程而调用的快捷方式文件(.lnk)的路径. 大多数应用程序不需要设置此值.
				不能与STARTF_TITLEISAPPID一起使用.
			STARTF_UNTRUSTEDSOURCE
				命令行来自不受信任的来源.
			STARTF_USECOUNTCHARS
				dwXCountChars和dwYCountChars包含其他信息.
			STARTF_USEFILLATTRIBUTE
				dwFillAttribute成员包含其他信息
			STARTF_USEHOTKEY
				hStdInput成员包含其他信息.
				不能与STARTF_USESTDHANDLES一起使用.
			STARTF_USEPOSITION
				dwX和dwY包含其他信息.
			STARTF_USESHOWWINDOW
				wShowWindow成员包含其他信息.
			STARTF_USESIZE
				dwXSize和dwYSize包含其他信息.
			STARTF_USESTDHANDLES
				hStdInput、hStdOutput和hStdError成员包含其他信息.
				如果在调用一个进程创建函数时指定了此标志, 则句柄必须是可继承的, 并且函数的bInheritHandles参数必须设置为TRUE
				不能与STARTF_USEHOTKEY一起使用.
		WORD    wShowWindow;
			如果dwFlags指定STARTF_USESHOWWINDOW, 则此成员可以是可在ShowWindow函数的nCmdShow参数中指定的任何值, SW_SHOWDEFAULT除外
			dwFlags没有指定STARTF_USESHOWWINDOW时, 此成员将被忽略.
			GUI进程, 第一次调用ShowWindow时, 使用wShowWindow指定默认值, 后续调用ShowWindow时, 如果参数是SW_SHOWDEFAULT, 则会用使用wShowWindow设定的值.
		WORD    cbReserved2;//保留字, 0
		LPBYTE  lpReserved2;//保留字, NULL
		HANDLE  hStdInput;
			如果dwFlags指定STARTF_USESTDHANDLES, 则该成员是流程的标准输入句柄.
			如果未指定STARTF_USESTDHANDLES, 则标准输入的默认值为键盘缓冲区.
			详细信息可以参见IO重定向.
		HANDLE  hStdOutput;
		HANDLE  hStdError;
	} STARTUPINFOA, *LPSTARTUPINFOA;
	STARTUPINFOW参见STARTUPINFOA
	*/

	//3、线程上下文
	/*
	typedef struct DECLSPEC_ALIGN(16) _CONTEXT {  //类似的_WOW64_CONTEXT结构体

	//
	// Register parameter home addresses.
	//
	// N.B. These fields are for convience - they could be used to extend the
	//      context record in the future.  这些字段是为了方便起见-它们可以用于将来扩展上下文记录
	//扩展寄存器组

	DWORD64 P1Home;
	DWORD64 P2Home;
	DWORD64 P3Home;
	DWORD64 P4Home;
	DWORD64 P5Home;
	DWORD64 P6Home;

	//
	// Control flags.
	//

	DWORD ContextFlags;//在查询的时候需要设置该字段，表示查询哪些其他的CONTEXT结构字段
		CONTEXT_CONTROL: 包含CPU的控制寄存器. 比如指今指针,堆栈指针,标志和函数返回地址. AX, BX, CX, DX, SI, D
		CONTEXT_INTEGER: 用于标识CPU的整数寄存器.DS, ES, FS, GS
		CONTEXT_FLOATING_POINT: 用于标识CPU的浮点寄存器.
		CONTEXT_SEGMENTS: 用于标识CPU的段寄存器.SS:SP, CS:IP, FLAGS, BP
		CONTEXT_DEBUG_REGISTER: 用于标识CPU的调试寄存器. 
		CONTEXT_EXTENDED_REGISTERS:用于标识CPU的扩展寄存器I
		CONTEXT_FULL: 相当于CONTEXT_CONTROL|CONTEXT_INTEGER|CONTEXT_SEGMENTS
		
	DWORD MxCsr;

	//
	// Segment Registers and processor flags.段寄存器和处理器标识
	//

	WORD   SegCs;
	WORD   SegDs;
	WORD   SegEs;
	WORD   SegFs;
	WORD   SegGs;
	WORD   SegSs;
	DWORD EFlags;

	//
	// Debug registers 调试寄存器
	//

	DWORD64 Dr0;
	DWORD64 Dr1;
	DWORD64 Dr2;
	DWORD64 Dr3;
	DWORD64 Dr6;
	DWORD64 Dr7;

	//
	// Integer registers. //整型寄存器
	//

	DWORD64 Rax;
	DWORD64 Rcx;
	DWORD64 Rdx;
	DWORD64 Rbx;
	DWORD64 Rsp;
	DWORD64 Rbp;
	DWORD64 Rsi;
	DWORD64 Rdi;
	DWORD64 R8;
	DWORD64 R9;
	DWORD64 R10;
	DWORD64 R11;
	DWORD64 R12;
	DWORD64 R13;
	DWORD64 R14;
	DWORD64 R15;

	//
	// Program counter. 程序计数器
	//

	DWORD64 Rip; //怀疑是当前地址

	//
	// Floating point state.
	// 浮点状态

	union {
		XMM_SAVE_AREA32 FltSave;
		struct {
			M128A Header[2];
			M128A Legacy[8];
			M128A Xmm0;
			M128A Xmm1;
			M128A Xmm2;
			M128A Xmm3;
			M128A Xmm4;
			M128A Xmm5;
			M128A Xmm6;
			M128A Xmm7;
			M128A Xmm8;
			M128A Xmm9;
			M128A Xmm10;
			M128A Xmm11;
			M128A Xmm12;
			M128A Xmm13;
			M128A Xmm14;
			M128A Xmm15;
		} DUMMYSTRUCTNAME;
	} DUMMYUNIONNAME;

	//
	// Vector registers.
	// 向量寄存器

	M128A VectorRegister[26];
	DWORD64 VectorControl;

	//
	// Special debug control registers.
	//

	DWORD64 DebugControl;
	DWORD64 LastBranchToRip;
	DWORD64 LastBranchFromRip;
	DWORD64 LastExceptionToRip;
	DWORD64 LastExceptionFromRip;
} CONTEXT, *PCONTEXT;
	*/


//二、重要函数
	//1、允许当前线程A在特定B线程方便的时候传递参数给B
	/*
		DWORD QueueUserAPC(
			PAPCFUNC pfnAPC,//typedef VOID (NTAPI *PAPCFUNC)(ULONG_PTR Parameter);
			HANDLE hThread,
			ULONG_PTR dwData); 范例如下:
				VOID WINAPI APCFunc(ULONG_PTR dwParam)
				{
					cout << "APC函数" << endl;
				}

				DWORD WINAPI ThreadFun(PVOID pvParam)
				{
					HANDLE hEvent = (HANDLE)pvParam;
					DWORD dw = WaitForSingleObjectEx(hEvent,INFINITE,TRUE);
					if (dw == WAIT_OBJECT_0)
					{
						cout << "事件触发" << endl;
					}
					if (dw == WAIT_IO_COMPLETION)
					{
						cout << "处理APC项目" << endl;
						return 0;
					}
					cout << "等待成功" << endl;
					return 0;
				}

				int main()
				{
					HANDLE hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
					HANDLE hThread = (HANDLE)_beginthreadex(NULL,0,(unsigned int (__stdcall *)(void *))ThreadFun,(PVOID)hEvent,0,NULL);
					Sleep(1000);
					QueueUserAPC(APCFunc,hThread,NULL);
					WaitForSingleObject(hThread,INFINITE);
					return 0;
				}
				//感觉不如共享内存/变量用得得劲啊???
	*/

	//2、进程/线程的计时信息
	/*
		GetSystemTimes
		GetProcessTimes: 获取指定进程的计时信息
		GetThreadTimes
	*/

	//3、获取进程/进程的句柄(handle)/id
	/*
		GetCurrentProcess/GetCurrentProcessId: 
		GetCurrentThread/GetCurrentThreadId: 
		GetProcessId
		GetThreadId
		GetProcessIdOfThread:
		注意:
			GetCurrentProcess和GetCurrentThread获取的是伪句柄, 只能在当前进程中才能代表当前进程的句柄.
				事实上这个函数目前只是简单的返回0xffffffff这个值. 伪句柄不用调用CloseHandle去关闭.
	*/

	//4、结束进程/线程
	/*
		ExitProcess/TerminateProcess/exit的关系和区别:
			exit是std函数. ExitProcess/TerminateProcess是WinApi函数.
			在windows中, exit: 进行全局变量和对象析构后, 然后调用ExitProcess函数.
			ExitProcess函数只是用来结束进程, 并不释放对象. 如果后面还有预期执行的代码, 
				实际因为调用ExitProcess而未执行, 可能导致已创建的对象没有析构而退出, 进而导致内存泄漏.
			TerminateProcess和ExitProcess一样, 只是TerminateProcess能结束其他进程.
			虽然exit、ExitProcess、TerminateProcess都会导致内存泄露, 但操作系统会进行后续清理.
		ExitThread/TerminateThread
	*/
	
	//进程/线程的返回值
	/*
	5、	GetExitCodeProcess: 获取进程的返回值
		GetExitCodeThread
	*/
	
	//6、暂停/等待/睡眠线程
	/*
		SwitchToThread/Sleep/WaitForXXX/SuspendThread:
			SwitchToThread: 释放cpu时间, 只要有可调度线程, 即便优先级较低,也会让其调度
			Sleep(大于0): 主动休眠若干ms, 允许其他线程调度运行
			Sleep(0): 释放cpu时间, 让线程马上重新回到就绪队列而非等待队列. 时间片只能让给优先级相同或更高的线程
			WaitForXXX: 需要访问内核, 收到某个信号恢复.
			SuspendThread: 暂停挂起线程, 需要手动ResumeThread恢复
		ResumeThread
	*/

	//7、创建线程
	/*
		HANDLE CreateThread(
			LPSECURITY_ATTRIBUTES lpThreadAttributes,
			SIZE_T dwStackSize,
			LPTHREAD_START_ROUTINE lpStartAddress,
			LPVOID lpParameter,
			DWORD dwCreationFlags,
			LPDWORD lpThreadId);//返回值
			//在主线程的基础上创建一个新线程. 线程终止运行后, 线程对象仍在, 须通过CloseHandle函数来关闭该线程对象.
			子线程需要调用CRT库时, 不要用CreateThread, 而应该用_beginthread来创建线程, _endthread来销毁线程. 
			因为没有对子线程为CRT库分配堆, 会导致低内存错误而崩溃(大多数我们自己写的C++程序都会使用CRT库, 所以...)
	*/	
	
	//8、创建进程
	/*
		CreateProcessA(
			LPCWSTR lpApplicationName,//向新进程要使用的可执行文件的名称
			LPWSTR lpCommandLine,//传给新进程的命令行字符串
				类型为LPTSTR, 这是因为在内部, CreateProcess会修改传给他的命令行字符串, 当然在它返回前, 它会把这个字符串还原.
				所以以下代码是错误的:
					CreateProcess(NULL,TEXT("NOTEPAD"),NULL,NULL,FALSE,0,NULL,NULL,&si,&pi);
				如果lpApplicationName为空, 可执行模块的名字必须处于lpCommandLine参数的最前面并由空格符与后面的字符分开.
			LPSECURITY_ATTRIBUTES lpProcessAttributes,
			LPSECURITY_ATTRIBUTES lpThreadAttributes,
				创建一个新的进程, 系统必须创建一个进程内核对象和新进程的主线程的线程内核对象,
					lpProcessAttributes和lpThreadAttributes就标识着这两个内核对象的安全描述符
			BOOL bInheritHandles,//设置父子进程的句柄描绘表的继承性
			DWORD dwCreationFlags,//标识影响新进程创建方式的标志
				CREATE_DEFAULT_ERROR_MODE: 新进程不继承调用进程的错误模式.
					赋予新进程当前的默认错误模式作为替代. 新进程可以调用SetErrorMode函数设置当前的默认错误模式.
				CREATE_NEW_CONSOLE: 新进程将使用一个新的控制台, 而不是继承父进程的控制台.
					不能与DETACHED_PROCESS标志一起使用
				CREATE_NEW_PROCESS_GROUP: 新进程将是一个进程树的根进程. 
					新进程树的用户标识符与调用进程的标识符相同, 由lpProcessInformation参数返回.
				CREATE_SEPARATE_WOW_VDM: 只适用于Windows NT. 这个标志只有当运行一个16位的Windows应用程序时才是有效的.
					新进程将会在一个私有的虚拟DOS机(VDM)中运行. (使用时再详细参见)
				CREATE_SHARED_WOW_VDM: 只适用于Windows NT. 这个标志只有当运行一个16位的Windows应用程序时才是有效的. (使用时再详细参见)
				CREATE_SUSPENDED: !!!(重要) 新进程的主线程会在创建后被挂起, 直到调用ResumeThread函数被调用时才运行, 
					这样父进程就可以修改子进程地址空间中的内存, 更改子进程的主线程的优先级, 或者在进程执行任何代码前, 
					把它加入到一个作业中, 父进程修改好子进程后, 再调用ResumeThread来允许子进程执行代码.
				CREATE_UNICODE_ENVIRONMENT: 如果被设置, 由lpEnvironment参数指定的环境块使用Unicode字符, 如果为空，环境块使用ANSI字符
				DEBUG_PROCESS: 调用进程将被当作调试程序, 并且新进程会被当作被调试的进程. 
					系统把被调试程序发生的所有调试事件通知给调试器.
				DEBUG_ONLY_THIS_PROCESS: 如果此标志没有被设置且调用进程正在被调试, 新进程将成为调试调用进程的调试器的另一个调试对象.
					如果调用进程没有被调试, 有关调试的行为就不会产生.
				DETACHED_PROCESS: 对于控制台进程，新进程没有访问父进程控制台的权限. 
					新进程可以通过AllocConsole函数自己创建一个新的控制台.
					不可以与CREATE_NEW_CONSOLE标志一起使用.
				dwCreationFlags参数还用来控制新进程的优先类.
					如果下面的优先级类标志都没有被指定. 那么默认的优先类是NORMAL_PRIORITY_CLASS,
					除非被创建的进程是IDLE_PRIORITY_CLASS. 
					在这种情况下子进程的默认优先类是IDLE_PRIORITY_CLASS.
						HIGH_PRIORITY_CLASS
						IDLE_PRIORITY_CLASS
						NORMAL_PRIORITY_CLASS
						REALTIME_PRIORITY_CLASS
			LPVOID lpEnvironment,//新进程的环境块. 如果参数为空, 新进程使用调用进程的环境
			LPCWSTR lpCurrentDirectory,//指定子进程的工作路径, 必须是一个包含驱动器名的绝对路径
			LPSTARTUPINFOW lpStartupInfo,//参见STARTUPINFOA
			LPPROCESS_INFORMATION lpProcessInformation);//返回值, 参见PROCESS_INFORMATION
			//参见https://www.csdn.net/tags/NtzaYg2sNTI1NDgtYmxvZwO0O0OO0O0O.html
				和https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createprocessa
		CreateProcessW

		CreateProcessAsUserA
		CreateProcessAsUserW:
			先科普UAC(user account control): 就是大家常见的安装软件或者启动程序的时候的出现的全屏变暗的一个提示框(你要允许以下程序对此计算机进行更改吗?). 
				正常的UAC级别下, 会检测程序是否有数字签名(可识别程序), 以及他的数字签名是否合法, 
				这对于一部分低端的木马具有提醒作用, 所以除非特殊情况, 不要乱对UCA降权.
	*/
	
	//9、创建远程线程
	/*
		CreateRemoteThread/CreateRemoteThreadEx: 够创建一个在其它进程地址空间中运行的线程(也称:创建远程线程).
			CreateRemoteThreadEx可以指定扩展属性, 比如处理器相关.
	*/

	//10、打开进程/线程
	/*
		HANDLE OpenThread(
			DWORD dwDesiredAccess,//线程的安全描述符
			BOOL bInheritHandle,
			DWORD dwThreadId);//打开线程, 返回线程的句柄
				THREAD_ALL_ACCESS
				THREAD_DIRECT_IMPERSONATION
				THREAD_GET_CONTEXT
				THREAD_IMPERSONATE
				THREAD_QUERY_INFORMATION
				THREAD_QUERY_LIMITED_INFORMATION
				THREAD_SET_CONTEXT
				THREAD_SET_INFORMATION
				THREAD_SET_LIMITED_INFORMATION
				THREAD_SET_THREAD_TOKEN
				THREAD_SUSPEND_RESUME
				THREAD_TERMINATE
		OpenProcess
			根据ID获取句柄
				PROCESS_ALL_ACCESS
				PROCESS_CREATE_PROCESS
				PROCESS_CREATE_THREAD
				PROCESS_DUP_HANDLE
				PROCESS_QUERY_INFORMATION
				PROCESS_QUERY_LIMITED_INFORMATION
				PROCESS_SET_INFORMATION
				PROCESS_SET_QUOTA
				PROCESS_SUSPEND_RESUME
				PROCESS_TERMINATE
				PROCESS_VM_OPERATION
				PROCESS_VM_READ
				PROCESS_VM_WRITE
				SYNCHRONIZE
	*/

	//11、进程/线程优先级
	/*
		SetPriorityClass/GetPriorityClass: 设置/获取进程优先级
		SetThreadPriority/GetThreadPriority: 设置/获取线程优先级
		SetThreadPriorityBoost: 禁用或启用系统临时提高线程优先级的能力
		GetThreadPriorityBoost: 指定线程的优先级提升控制状态
		GetProcessPriorityBoost
		SetProcessPriorityBoost
	*/
	
	//12、Tls 意义不大, 可以自己实现
	/*
	DWORD TlsAlloc();
		LPVOID TlsGetValue(DWORD dwTlsIndex);
		BOOL TlsSetValue(DWORD dwTlsIndex, LPVOID lpTlsValue);
		BOOL TlsFree(DWORD dwTlsIndex);
			//进程中的全局变量与函数内的静态变量, 各个线程都可以访问. 一个线程修改的值，对所有线程都生效.
			这是优点也是缺点, 说是优点, 线程的数据交换变得快捷.
			说是缺点, 一个线程挂了, 其它线程也不保; 
			多线程访问共享数据, 需要昂贵的同步开销, 也容易造成同步相关的BUG.
			如需要在一个线程内部的各个函数调用都能访问、但其它线程不能访问的变量(被称为线程局部静态变量), 就需要新的机制来实现, 这就是TLS.
			线程局部存储在不同的平台有不同的实现, 可移植性不太好. 
			幸好要实现线程局部存储并不难, 最简单的办法就是建立一个全局表, 通过当前线程ID去查询相应的数据, 因为各个线程的ID不同, 查到的数据自然也不同了
	*/
	
	//13、关机时的程序的关闭顺序
	/*
		SetProcessShutdownParameters:
			在系统关闭期间, 为指定进程设置他相对于其它程序的关闭顺序
		GetProcessShutdownParameters
	*/
	
	/*
	14、GetProcessVersion
	15、GetStartupInfo: 该函数取得进程在启动时被指定的 STARTUPINFO 结构(看起来有用啊)
	16、GetCurrentProcessToken
		GetCurrentThreadToken
		GetCurrentThreadEffectiveToken
		SetThreadToken
		OpenProcessToken
		OpenThreadToken
		参见https://blog.csdn.net/qq_22423659/article/details/53384074进程权限总结
	17、SetThreadStackGuarantee
	18、ProcessIdToSessionId: 检索与指定进程关联的远程桌面服务会话
	19、InitializeProcThreadAttributeList 
		DeleteProcThreadAttributeList
		UpdateProcThreadAttribute
		//使用前景参见:https://docs.microsoft.com/zh-CN/windows/console/creating-a-pseudoconsole-session
	20、SetProcessAffinityUpdateMode:设置指定进程的关联更新模式
		QueryProcessAffinityUpdateMode
	*/
	
	//21、线程堆栈边界
	/*
	GetCurrentThreadStackLimits: 线程堆栈边界
	*/
	
	//22、线程的上下文 (实现动态库注入)
	/*
	GetThreadContext/SetThreadContext: 直接获取当前线程的上下文
	Wow64GetThreadContext/Wow64SetThreadContext: 32位运行于64位操作系统
		获取线程上下文之前, 最好先调用SuspendThread挂起线程
		如果在当前线程调用GetThreadContext, 虽然返回成功, 但是上下文无效
		注: 可以通过获取线程的上下文, 修改执行地址, 实现代码注入, 运行预期的代码

		范例:
		CONTEXT Context;
		SuspendThread(hThread);//挂起线程
		Context.ContextFlags = CONTEXT_CONTROL;
		GetThreadContext(hThread, &Context);//获取线程Context
		Context.Eip = 0x00010000;//修改EIP，也就是修改了当前执行的代码，后续线程激活后，会在这个地址继续执行(这个地址是为了演示随意填写的，由于这个地址问题，程序可能会崩溃)
		Context.ContextFlags = CONTEXT_CONTROL;
		SetThreadContext(hThread, &Context);//设置线程Context
		ResumeThread(hThread);//恢复线程运行，此时程序从0x00010000地址处开始运行，程序会异常
	*/
	
	/*
	23、FlushInstructionCache: 刷新指定进程的指令缓存
	24、IsProcessorFeaturePresent: 确定当前计算机是否支持指定的处理器功能
	25、GetProcessHandleCount:指定进程的打开句柄数
	26、GetCurrentProcessorNumber: 线程所在处理器
		GetCurrentProcessorNumberEx
	27、SetThreadIdealProcessorEx: 指定线程的理想处理器，并可选地检索以前的理想处理器
			参见SetProcessAffinityMask/SetThreadAffinityMask
		GetThreadIdealProcessorEx
	28、GetThreadIOPendingFlag: 确定指定线程是否有任何挂起的I/O请求
	*/

	//29、线程/进程的信息
	/*
		GetThreadInformation: 检索有关指定线程的信息
		SetThreadInformation
			typedef enum _THREAD_INFORMATION_CLASS {
				ThreadMemoryPriority,
				ThreadAbsoluteCpuPriority,
				ThreadDynamicCodePolicy,
				ThreadPowerThrottling,
				ThreadInformationClassMax
			} THREAD_INFORMATION_CLASS;
		SetProcessInformation
		GetProcessInformation
			typedef enum _PROCESS_INFORMATION_CLASS {
				ProcessMemoryPriority,			//进程内存优先级
					参见MEMORY_PRIORITY_INFORMATION
					#define MEMORY_PRIORITY_LOWEST           0
					#define MEMORY_PRIORITY_VERY_LOW         1
					#define MEMORY_PRIORITY_LOW              2
					#define MEMORY_PRIORITY_MEDIUM           3
					#define MEMORY_PRIORITY_BELOW_NORMAL     4
					#define MEMORY_PRIORITY_NORMAL           5
				ProcessMemoryExhaustionInfo,	
				ProcessAppMemoryInfo,
				ProcessInPrivateInfo,
				ProcessPowerThrottling,
				ProcessReservedValue1,
				ProcessTelemetryCoverageInfo,
				ProcessProtectionLevelInfo,
				ProcessLeapSecondInfo,
				ProcessMachineTypeInfo,
				ProcessInformationClassMax
			} PROCESS_INFORMATION_CLASS;
	*/
	
	/*
	30、IsProcessCritical: 进程是否处于严重状态
	31、SetProtectedPolicy:设置受保护的策略
		QueryProtectedPolicy
	32、GetSystemCpuSetInformation: 指定进程线程使用哪个CPU
		GetProcessDefaultCpuSets
		SetProcessDefaultCpuSets
		GetThreadSelectedCpuSets
		SetThreadSelectedCpuSets
	33、SetThreadDescription: 线程描述
		GetThreadDescription
*/
#endif