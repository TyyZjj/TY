#ifndef _SPROCESSTHREADSAPI_H_
#define _SPROCESSTHREADSAPI_H_

#ifdef WIN32
#include <processthreadsapi.h>
#endif // WIN32

//�μ�:https://docs.microsoft.com/zh-cn/Windows/win32/procthread/process-and-thread-functions


//һ����Ҫ�ṹ��

	//1��������Ϣ
	/*typedef struct _PROCESS_INFORMATION {
		HANDLE hProcess;
		HANDLE hThread;
		DWORD dwProcessId;
		DWORD dwThreadId;
	} PROCESS_INFORMATION, *PPROCESS_INFORMATION, *LPPROCESS_INFORMATION;
	*/

	//2������������Ϣ
	/*
	typedef struct _STARTUPINFOA {
		DWORD   cb;	//�ṹ���С
		LPSTR   lpReserved; //������
		LPSTR   lpDesktop; //��������(����ʹ��ڵ�����)
		LPSTR   lpTitle; //���ڱ���
		DWORD   dwX;
			���dwFlagsָ��STARTF_USEPOSITION, ���Ǵ����´���ʱ�������Ͻǵ�xƫ����(����Ϊ��λ)
			����dwX��������
			GUI����, ��CreateWindow��x����ΪCW_USEDEFAULT, �½��̵�һ�ε���CreateWindow�Դ����ص�����ʱ, ��ʹ��ָ����λ��
		DWORD   dwY;
		DWORD   dwXSize;
			���dwFlagsָ��STARTF_USESIZE, ���Ǵ����´���ʱ���ڵĿ��(����Ϊ��λ)
			����dwX��������
		DWORD   dwYSize;
		DWORD   dwXCountChars;
			���dwFlagsָ��STARTF_USECOUNTCHARS, ����ڿ���̨�����д����µĿ���̨����, �ó�Ա�����ַ�����ָ����Ļ���������
		DWORD   dwYCountChars;
		DWORD   dwFillAttribute;
			���dwFlagsָ��STARTF_USEFILLATTRIBUTE, ����ڿ���̨Ӧ�ó����д������µĿ���̨����, ��ó�Ա�ǳ�ʼ�ı��ͱ���ɫ
			��ѡֵ:
				FOREGROUND_BLUE, 
				FOREGROUND_GREEN, 
				FOREGROUND_RED, 
				FOREGROUND_INTENSITY, 
				BACKGROUND_BLUE, 
				BACKGROUND_GREEN, 
				BACKGROUND_RED, 
				BACKGROUND_INTENSITY
		DWORD   dwFlags; ȷ�ϴ�������ʱ, STARTUPINFOA����Щ�ֶλ���Ч. ��ѡֵ:
			STARTF_FORCEONFEEDBACK: �������̺�, ��괦�ڷ���ģʽ2����. 
				����ڴ��ڼ�, ���̽����˵�һ��GUI����, ��ϵͳ���ٸ����������ӵ�ʱ��.
				����������ڽ�����ʾһ������, ϵͳ���ٸ�����������ʱ������ɴ��ڵĻ���.
				ϵͳ�ڵ�һ�ε���GetMessage��رշ�����꣬���۽����Ƿ����ڻ���.
			STARTF_FORCEOFFFEEDBACK
				��������ʱǿ�ƹرշ������. ��ʱ����ʾ������ѡ����
			STARTF_PREVENTPINNING
				���̴������κδ��ڶ����̶ܹ�����������. ������STARTF_TITLEISAPPID���ʹ��.
			STARTF_RUNFULLSCREEN
				����Ӧ��ȫ��ģʽ�����У������Ǵ���ģʽ.������x86����������еĿ���̨Ӧ�ó�����Ч.
			STARTF_TITLEISAPPID
				lpTitle��Ա����һ��AppUserModelID. ������������"��ʼ"�˵���ʾӦ�ó���, ��ʹ������ȷ�Ŀ�ݷ�ʽ����ת�б������.
				������STARTF_TITLEISLINKNAMEһ��ʹ��
			STARTF_TITLEISLINKNAME
				lpTitle�����û�Ϊ�����˽��̶����õĿ�ݷ�ʽ�ļ�(.lnk)��·��. �����Ӧ�ó�����Ҫ���ô�ֵ.
				������STARTF_TITLEISAPPIDһ��ʹ��.
			STARTF_UNTRUSTEDSOURCE
				���������Բ������ε���Դ.
			STARTF_USECOUNTCHARS
				dwXCountChars��dwYCountChars����������Ϣ.
			STARTF_USEFILLATTRIBUTE
				dwFillAttribute��Ա����������Ϣ
			STARTF_USEHOTKEY
				hStdInput��Ա����������Ϣ.
				������STARTF_USESTDHANDLESһ��ʹ��.
			STARTF_USEPOSITION
				dwX��dwY����������Ϣ.
			STARTF_USESHOWWINDOW
				wShowWindow��Ա����������Ϣ.
			STARTF_USESIZE
				dwXSize��dwYSize����������Ϣ.
			STARTF_USESTDHANDLES
				hStdInput��hStdOutput��hStdError��Ա����������Ϣ.
				����ڵ���һ�����̴�������ʱָ���˴˱�־, ���������ǿɼ̳е�, ���Һ�����bInheritHandles������������ΪTRUE
				������STARTF_USEHOTKEYһ��ʹ��.
		WORD    wShowWindow;
			���dwFlagsָ��STARTF_USESHOWWINDOW, ��˳�Ա�����ǿ���ShowWindow������nCmdShow������ָ�����κ�ֵ, SW_SHOWDEFAULT����
			dwFlagsû��ָ��STARTF_USESHOWWINDOWʱ, �˳�Ա��������.
			GUI����, ��һ�ε���ShowWindowʱ, ʹ��wShowWindowָ��Ĭ��ֵ, ��������ShowWindowʱ, ���������SW_SHOWDEFAULT, �����ʹ��wShowWindow�趨��ֵ.
		WORD    cbReserved2;//������, 0
		LPBYTE  lpReserved2;//������, NULL
		HANDLE  hStdInput;
			���dwFlagsָ��STARTF_USESTDHANDLES, ��ó�Ա�����̵ı�׼������.
			���δָ��STARTF_USESTDHANDLES, ���׼�����Ĭ��ֵΪ���̻�����.
			��ϸ��Ϣ���Բμ�IO�ض���.
		HANDLE  hStdOutput;
		HANDLE  hStdError;
	} STARTUPINFOA, *LPSTARTUPINFOA;
	STARTUPINFOW�μ�STARTUPINFOA
	*/

	//3���߳�������
	/*
	typedef struct DECLSPEC_ALIGN(16) _CONTEXT {  //���Ƶ�_WOW64_CONTEXT�ṹ��

	//
	// Register parameter home addresses.
	//
	// N.B. These fields are for convience - they could be used to extend the
	//      context record in the future.  ��Щ�ֶ���Ϊ�˷������-���ǿ������ڽ�����չ�����ļ�¼
	//��չ�Ĵ�����

	DWORD64 P1Home;
	DWORD64 P2Home;
	DWORD64 P3Home;
	DWORD64 P4Home;
	DWORD64 P5Home;
	DWORD64 P6Home;

	//
	// Control flags.
	//

	DWORD ContextFlags;//�ڲ�ѯ��ʱ����Ҫ���ø��ֶΣ���ʾ��ѯ��Щ������CONTEXT�ṹ�ֶ�
		CONTEXT_CONTROL: ����CPU�Ŀ��ƼĴ���. ����ָ��ָ��,��ջָ��,��־�ͺ������ص�ַ. AX, BX, CX, DX, SI, D
		CONTEXT_INTEGER: ���ڱ�ʶCPU�������Ĵ���.DS, ES, FS, GS
		CONTEXT_FLOATING_POINT: ���ڱ�ʶCPU�ĸ���Ĵ���.
		CONTEXT_SEGMENTS: ���ڱ�ʶCPU�ĶμĴ���.SS:SP, CS:IP, FLAGS, BP
		CONTEXT_DEBUG_REGISTER: ���ڱ�ʶCPU�ĵ��ԼĴ���. 
		CONTEXT_EXTENDED_REGISTERS:���ڱ�ʶCPU����չ�Ĵ���I
		CONTEXT_FULL: �൱��CONTEXT_CONTROL|CONTEXT_INTEGER|CONTEXT_SEGMENTS
		
	DWORD MxCsr;

	//
	// Segment Registers and processor flags.�μĴ����ʹ�������ʶ
	//

	WORD   SegCs;
	WORD   SegDs;
	WORD   SegEs;
	WORD   SegFs;
	WORD   SegGs;
	WORD   SegSs;
	DWORD EFlags;

	//
	// Debug registers ���ԼĴ���
	//

	DWORD64 Dr0;
	DWORD64 Dr1;
	DWORD64 Dr2;
	DWORD64 Dr3;
	DWORD64 Dr6;
	DWORD64 Dr7;

	//
	// Integer registers. //���ͼĴ���
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
	// Program counter. ���������
	//

	DWORD64 Rip; //�����ǵ�ǰ��ַ

	//
	// Floating point state.
	// ����״̬

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
	// �����Ĵ���

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


//������Ҫ����
	//1������ǰ�߳�A���ض�B�̷߳����ʱ�򴫵ݲ�����B
	/*
		DWORD QueueUserAPC(
			PAPCFUNC pfnAPC,//typedef VOID (NTAPI *PAPCFUNC)(ULONG_PTR Parameter);
			HANDLE hThread,
			ULONG_PTR dwData); ��������:
				VOID WINAPI APCFunc(ULONG_PTR dwParam)
				{
					cout << "APC����" << endl;
				}

				DWORD WINAPI ThreadFun(PVOID pvParam)
				{
					HANDLE hEvent = (HANDLE)pvParam;
					DWORD dw = WaitForSingleObjectEx(hEvent,INFINITE,TRUE);
					if (dw == WAIT_OBJECT_0)
					{
						cout << "�¼�����" << endl;
					}
					if (dw == WAIT_IO_COMPLETION)
					{
						cout << "����APC��Ŀ" << endl;
						return 0;
					}
					cout << "�ȴ��ɹ�" << endl;
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
				//�о����繲���ڴ�/�����õõþ���???
	*/

	//2������/�̵߳ļ�ʱ��Ϣ
	/*
		GetSystemTimes
		GetProcessTimes: ��ȡָ�����̵ļ�ʱ��Ϣ
		GetThreadTimes
	*/

	//3����ȡ����/���̵ľ��(handle)/id
	/*
		GetCurrentProcess/GetCurrentProcessId: 
		GetCurrentThread/GetCurrentThreadId: 
		GetProcessId
		GetThreadId
		GetProcessIdOfThread:
		ע��:
			GetCurrentProcess��GetCurrentThread��ȡ����α���, ֻ���ڵ�ǰ�����в��ܴ���ǰ���̵ľ��.
				��ʵ���������Ŀǰֻ�Ǽ򵥵ķ���0xffffffff���ֵ. α������õ���CloseHandleȥ�ر�.
	*/

	//4����������/�߳�
	/*
		ExitProcess/TerminateProcess/exit�Ĺ�ϵ������:
			exit��std����. ExitProcess/TerminateProcess��WinApi����.
			��windows��, exit: ����ȫ�ֱ����Ͷ���������, Ȼ�����ExitProcess����.
			ExitProcess����ֻ��������������, �����ͷŶ���. ������滹��Ԥ��ִ�еĴ���, 
				ʵ����Ϊ����ExitProcess��δִ��, ���ܵ����Ѵ����Ķ���û���������˳�, ���������ڴ�й©.
			TerminateProcess��ExitProcessһ��, ֻ��TerminateProcess�ܽ�����������.
			��Ȼexit��ExitProcess��TerminateProcess���ᵼ���ڴ�й¶, ������ϵͳ����к�������.
		ExitThread/TerminateThread
	*/
	
	//����/�̵߳ķ���ֵ
	/*
	5��	GetExitCodeProcess: ��ȡ���̵ķ���ֵ
		GetExitCodeThread
	*/
	
	//6����ͣ/�ȴ�/˯���߳�
	/*
		SwitchToThread/Sleep/WaitForXXX/SuspendThread:
			SwitchToThread: �ͷ�cpuʱ��, ֻҪ�пɵ����߳�, �������ȼ��ϵ�,Ҳ���������
			Sleep(����0): ������������ms, ���������̵߳�������
			Sleep(0): �ͷ�cpuʱ��, ���߳��������»ص��������ж��ǵȴ�����. ʱ��Ƭֻ���ø����ȼ���ͬ����ߵ��߳�
			WaitForXXX: ��Ҫ�����ں�, �յ�ĳ���źŻָ�.
			SuspendThread: ��ͣ�����߳�, ��Ҫ�ֶ�ResumeThread�ָ�
		ResumeThread
	*/

	//7�������߳�
	/*
		HANDLE CreateThread(
			LPSECURITY_ATTRIBUTES lpThreadAttributes,
			SIZE_T dwStackSize,
			LPTHREAD_START_ROUTINE lpStartAddress,
			LPVOID lpParameter,
			DWORD dwCreationFlags,
			LPDWORD lpThreadId);//����ֵ
			//�����̵߳Ļ����ϴ���һ�����߳�. �߳���ֹ���к�, �̶߳�������, ��ͨ��CloseHandle�������رո��̶߳���.
			���߳���Ҫ����CRT��ʱ, ��Ҫ��CreateThread, ��Ӧ����_beginthread�������߳�, _endthread�������߳�. 
			��Ϊû�ж����߳�ΪCRT������, �ᵼ�µ��ڴ���������(����������Լ�д��C++���򶼻�ʹ��CRT��, ����...)
	*/	
	
	//8����������
	/*
		CreateProcessA(
			LPCWSTR lpApplicationName,//���½���Ҫʹ�õĿ�ִ���ļ�������
			LPWSTR lpCommandLine,//�����½��̵��������ַ���
				����ΪLPTSTR, ������Ϊ���ڲ�, CreateProcess���޸Ĵ��������������ַ���, ��Ȼ��������ǰ, ���������ַ�����ԭ.
				�������´����Ǵ����:
					CreateProcess(NULL,TEXT("NOTEPAD"),NULL,NULL,FALSE,0,NULL,NULL,&si,&pi);
				���lpApplicationNameΪ��, ��ִ��ģ������ֱ��봦��lpCommandLine��������ǰ�沢�ɿո���������ַ��ֿ�.
			LPSECURITY_ATTRIBUTES lpProcessAttributes,
			LPSECURITY_ATTRIBUTES lpThreadAttributes,
				����һ���µĽ���, ϵͳ���봴��һ�������ں˶�����½��̵����̵߳��߳��ں˶���,
					lpProcessAttributes��lpThreadAttributes�ͱ�ʶ���������ں˶���İ�ȫ������
			BOOL bInheritHandles,//���ø��ӽ��̵ľ������ļ̳���
			DWORD dwCreationFlags,//��ʶӰ���½��̴�����ʽ�ı�־
				CREATE_DEFAULT_ERROR_MODE: �½��̲��̳е��ý��̵Ĵ���ģʽ.
					�����½��̵�ǰ��Ĭ�ϴ���ģʽ��Ϊ���. �½��̿��Ե���SetErrorMode�������õ�ǰ��Ĭ�ϴ���ģʽ.
				CREATE_NEW_CONSOLE: �½��̽�ʹ��һ���µĿ���̨, �����Ǽ̳и����̵Ŀ���̨.
					������DETACHED_PROCESS��־һ��ʹ��
				CREATE_NEW_PROCESS_GROUP: �½��̽���һ���������ĸ�����. 
					�½��������û���ʶ������ý��̵ı�ʶ����ͬ, ��lpProcessInformation��������.
				CREATE_SEPARATE_WOW_VDM: ֻ������Windows NT. �����־ֻ�е�����һ��16λ��WindowsӦ�ó���ʱ������Ч��.
					�½��̽�����һ��˽�е�����DOS��(VDM)������. (ʹ��ʱ����ϸ�μ�)
				CREATE_SHARED_WOW_VDM: ֻ������Windows NT. �����־ֻ�е�����һ��16λ��WindowsӦ�ó���ʱ������Ч��. (ʹ��ʱ����ϸ�μ�)
				CREATE_SUSPENDED: !!!(��Ҫ) �½��̵����̻߳��ڴ����󱻹���, ֱ������ResumeThread����������ʱ������, 
					���������̾Ϳ����޸��ӽ��̵�ַ�ռ��е��ڴ�, �����ӽ��̵����̵߳����ȼ�, �����ڽ���ִ���κδ���ǰ, 
					�������뵽һ����ҵ��, �������޸ĺ��ӽ��̺�, �ٵ���ResumeThread�������ӽ���ִ�д���.
				CREATE_UNICODE_ENVIRONMENT: ���������, ��lpEnvironment����ָ���Ļ�����ʹ��Unicode�ַ�, ���Ϊ�գ�������ʹ��ANSI�ַ�
				DEBUG_PROCESS: ���ý��̽����������Գ���, �����½��̻ᱻ���������ԵĽ���. 
					ϵͳ�ѱ����Գ����������е����¼�֪ͨ��������.
				DEBUG_ONLY_THIS_PROCESS: ����˱�־û�б������ҵ��ý������ڱ�����, �½��̽���Ϊ���Ե��ý��̵ĵ���������һ�����Զ���.
					������ý���û�б�����, �йص��Ե���Ϊ�Ͳ������.
				DETACHED_PROCESS: ���ڿ���̨���̣��½���û�з��ʸ����̿���̨��Ȩ��. 
					�½��̿���ͨ��AllocConsole�����Լ�����һ���µĿ���̨.
					��������CREATE_NEW_CONSOLE��־һ��ʹ��.
				dwCreationFlags���������������½��̵�������.
					�����������ȼ����־��û�б�ָ��. ��ôĬ�ϵ���������NORMAL_PRIORITY_CLASS,
					���Ǳ������Ľ�����IDLE_PRIORITY_CLASS. 
					������������ӽ��̵�Ĭ����������IDLE_PRIORITY_CLASS.
						HIGH_PRIORITY_CLASS
						IDLE_PRIORITY_CLASS
						NORMAL_PRIORITY_CLASS
						REALTIME_PRIORITY_CLASS
			LPVOID lpEnvironment,//�½��̵Ļ�����. �������Ϊ��, �½���ʹ�õ��ý��̵Ļ���
			LPCWSTR lpCurrentDirectory,//ָ���ӽ��̵Ĺ���·��, ������һ���������������ľ���·��
			LPSTARTUPINFOW lpStartupInfo,//�μ�STARTUPINFOA
			LPPROCESS_INFORMATION lpProcessInformation);//����ֵ, �μ�PROCESS_INFORMATION
			//�μ�https://www.csdn.net/tags/NtzaYg2sNTI1NDgtYmxvZwO0O0OO0O0O.html
				��https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createprocessa
		CreateProcessW

		CreateProcessAsUserA
		CreateProcessAsUserW:
			�ȿ���UAC(user account control): ���Ǵ�ҳ����İ�װ����������������ʱ��ĳ��ֵ�ȫ���䰵��һ����ʾ��(��Ҫ�������³���Դ˼�������и�����?). 
				������UAC������, ��������Ƿ�������ǩ��(��ʶ�����), �Լ���������ǩ���Ƿ�Ϸ�, 
				�����һ���ֵͶ˵�ľ�������������, ���Գ����������, ��Ҫ�Ҷ�UCA��Ȩ.
	*/
	
	//9������Զ���߳�
	/*
		CreateRemoteThread/CreateRemoteThreadEx: ������һ�����������̵�ַ�ռ������е��߳�(Ҳ��:����Զ���߳�).
			CreateRemoteThreadEx����ָ����չ����, ���紦�������.
	*/

	//10���򿪽���/�߳�
	/*
		HANDLE OpenThread(
			DWORD dwDesiredAccess,//�̵߳İ�ȫ������
			BOOL bInheritHandle,
			DWORD dwThreadId);//���߳�, �����̵߳ľ��
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
			����ID��ȡ���
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

	//11������/�߳����ȼ�
	/*
		SetPriorityClass/GetPriorityClass: ����/��ȡ�������ȼ�
		SetThreadPriority/GetThreadPriority: ����/��ȡ�߳����ȼ�
		SetThreadPriorityBoost: ���û�����ϵͳ��ʱ����߳����ȼ�������
		GetThreadPriorityBoost: ָ���̵߳����ȼ���������״̬
		GetProcessPriorityBoost
		SetProcessPriorityBoost
	*/
	
	//12��Tls ���岻��, �����Լ�ʵ��
	/*
	DWORD TlsAlloc();
		LPVOID TlsGetValue(DWORD dwTlsIndex);
		BOOL TlsSetValue(DWORD dwTlsIndex, LPVOID lpTlsValue);
		BOOL TlsFree(DWORD dwTlsIndex);
			//�����е�ȫ�ֱ����뺯���ڵľ�̬����, �����̶߳����Է���. һ���߳��޸ĵ�ֵ���������̶߳���Ч.
			�����ŵ�Ҳ��ȱ��, ˵���ŵ�, �̵߳����ݽ�����ÿ��.
			˵��ȱ��, һ���̹߳���, �����߳�Ҳ����; 
			���̷߳��ʹ�������, ��Ҫ�����ͬ������, Ҳ�������ͬ����ص�BUG.
			����Ҫ��һ���߳��ڲ��ĸ����������ö��ܷ��ʡ��������̲߳��ܷ��ʵı���(����Ϊ�ֲ߳̾���̬����), ����Ҫ�µĻ�����ʵ��, �����TLS.
			�ֲ߳̾��洢�ڲ�ͬ��ƽ̨�в�ͬ��ʵ��, ����ֲ�Բ�̫��. 
			�Һ�Ҫʵ���ֲ߳̾��洢������, ��򵥵İ취���ǽ���һ��ȫ�ֱ�, ͨ����ǰ�߳�IDȥ��ѯ��Ӧ������, ��Ϊ�����̵߳�ID��ͬ, �鵽��������ȻҲ��ͬ��
	*/
	
	//13���ػ�ʱ�ĳ���Ĺر�˳��
	/*
		SetProcessShutdownParameters:
			��ϵͳ�ر��ڼ�, Ϊָ�������������������������Ĺر�˳��
		GetProcessShutdownParameters
	*/
	
	/*
	14��GetProcessVersion
	15��GetStartupInfo: �ú���ȡ�ý���������ʱ��ָ���� STARTUPINFO �ṹ(���������ð�)
	16��GetCurrentProcessToken
		GetCurrentThreadToken
		GetCurrentThreadEffectiveToken
		SetThreadToken
		OpenProcessToken
		OpenThreadToken
		�μ�https://blog.csdn.net/qq_22423659/article/details/53384074����Ȩ���ܽ�
	17��SetThreadStackGuarantee
	18��ProcessIdToSessionId: ������ָ�����̹�����Զ���������Ự
	19��InitializeProcThreadAttributeList 
		DeleteProcThreadAttributeList
		UpdateProcThreadAttribute
		//ʹ��ǰ���μ�:https://docs.microsoft.com/zh-CN/windows/console/creating-a-pseudoconsole-session
	20��SetProcessAffinityUpdateMode:����ָ�����̵Ĺ�������ģʽ
		QueryProcessAffinityUpdateMode
	*/
	
	//21���̶߳�ջ�߽�
	/*
	GetCurrentThreadStackLimits: �̶߳�ջ�߽�
	*/
	
	//22���̵߳������� (ʵ�ֶ�̬��ע��)
	/*
	GetThreadContext/SetThreadContext: ֱ�ӻ�ȡ��ǰ�̵߳�������
	Wow64GetThreadContext/Wow64SetThreadContext: 32λ������64λ����ϵͳ
		��ȡ�߳�������֮ǰ, ����ȵ���SuspendThread�����߳�
		����ڵ�ǰ�̵߳���GetThreadContext, ��Ȼ���سɹ�, ������������Ч
		ע: ����ͨ����ȡ�̵߳�������, �޸�ִ�е�ַ, ʵ�ִ���ע��, ����Ԥ�ڵĴ���

		����:
		CONTEXT Context;
		SuspendThread(hThread);//�����߳�
		Context.ContextFlags = CONTEXT_CONTROL;
		GetThreadContext(hThread, &Context);//��ȡ�߳�Context
		Context.Eip = 0x00010000;//�޸�EIP��Ҳ�����޸��˵�ǰִ�еĴ��룬�����̼߳���󣬻��������ַ����ִ��(�����ַ��Ϊ����ʾ������д�ģ����������ַ���⣬������ܻ����)
		Context.ContextFlags = CONTEXT_CONTROL;
		SetThreadContext(hThread, &Context);//�����߳�Context
		ResumeThread(hThread);//�ָ��߳����У���ʱ�����0x00010000��ַ����ʼ���У�������쳣
	*/
	
	/*
	23��FlushInstructionCache: ˢ��ָ�����̵�ָ���
	24��IsProcessorFeaturePresent: ȷ����ǰ������Ƿ�֧��ָ���Ĵ���������
	25��GetProcessHandleCount:ָ�����̵Ĵ򿪾����
	26��GetCurrentProcessorNumber: �߳����ڴ�����
		GetCurrentProcessorNumberEx
	27��SetThreadIdealProcessorEx: ָ���̵߳����봦����������ѡ�ؼ�����ǰ�����봦����
			�μ�SetProcessAffinityMask/SetThreadAffinityMask
		GetThreadIdealProcessorEx
	28��GetThreadIOPendingFlag: ȷ��ָ���߳��Ƿ����κι����I/O����
	*/

	//29���߳�/���̵���Ϣ
	/*
		GetThreadInformation: �����й�ָ���̵߳���Ϣ
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
				ProcessMemoryPriority,			//�����ڴ����ȼ�
					�μ�MEMORY_PRIORITY_INFORMATION
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
	30��IsProcessCritical: �����Ƿ�������״̬
	31��SetProtectedPolicy:�����ܱ����Ĳ���
		QueryProtectedPolicy
	32��GetSystemCpuSetInformation: ָ�������߳�ʹ���ĸ�CPU
		GetProcessDefaultCpuSets
		SetProcessDefaultCpuSets
		GetThreadSelectedCpuSets
		SetThreadSelectedCpuSets
	33��SetThreadDescription: �߳�����
		GetThreadDescription
*/
#endif