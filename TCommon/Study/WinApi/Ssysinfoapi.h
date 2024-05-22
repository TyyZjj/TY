#ifndef _SSYSINFOAPI_H_
#define _SSYSINFOAPI_H_

#ifdef WIN32
#include <sysinfoapi.h>
#endif // WIN32

//һ����Ҫ�ṹ��
/*
	1��ϵͳ�Ե���Ϣ
	typedef struct _SYSTEM_INFO {
	union {
		DWORD dwOemId;          // Obsolete field...do not use
		struct {
			WORD wProcessorArchitecture; //�������ṹ����, ����������ֵ:
				PROCESSOR_ARCHITECTURE_AMD64	9	->x64 (AMD or Intel)
				PROCESSOR_ARCHITECTURE_ARM		5	->ARM
				PROCESSOR_ARCHITECTURE_ARM64	12	->ARM64
				PROCESSOR_ARCHITECTURE_IA64		6	->Intel Itanium-based
				PROCESSOR_ARCHITECTURE_INTEL	0	->x86
				PROCESSOR_ARCHITECTURE_UNKNOWN	0xffff	->δ֪
			WORD wReserved;		//������
		} DUMMYSTRUCTNAME;
	} DUMMYUNIONNAME;
	DWORD dwPageSize;			//��ҳ��С(���������ڴ�ʱ����С�ڴ�����, �μ� VirtualAlloc.)
	LPVOID lpMinimumApplicationAddress;//Ӧ�ó����dll�ȿɷ��ʵ�����ڴ��ַ
	LPVOID lpMaximumApplicationAddress;//Ӧ�ó����dll�ȿɷ��ʵ�����ڴ��ַ
	DWORD_PTR dwActiveProcessorMask;//��������������.λ0�Ǵ�����0;λ31�Ǵ�����31;
	DWORD dwNumberOfProcessors;//���������� ->������ϢGetLogicalProcessorInformation
	DWORD dwProcessorType;//��ʱ����
	DWORD dwAllocationGranularity;//���������ڴ�ʱ, ��ʼ��ַ������, �μ� VirtualAlloc.
	WORD wProcessorLevel;//����������(�ô�����)
	WORD wProcessorRevision;//�������汾
} SYSTEM_INFO, *LPSYSTEM_INFO;

	2�������ڴ�������ڴ�(������չ�ڴ�)��ǰ״̬����Ϣ
	typedef struct _MEMORYSTATUSEX {
	DWORD dwLength;//�ṹ���С, ����GlobalMemoryStatusEx֮ǰ, Ҫ�����ô�ֵ;
	DWORD dwMemoryLoad;//[0,100]��ʹ�õ������ڴ�ٷֱ�, 100��ʾ�ڴ�����.
	DWORDLONG ullTotalPhys;//ʵ�������ڴ��С,��λ:�ֽ�.
	DWORDLONG ullAvailPhys;//��ǰ���õ������ڴ��С, ��λ:�ֽ�.
		(��������ʹ�õ������ڴ��С, ��������Ҫ��������̵Ļ���)
	DWORDLONG ullTotalPageFile;//ϵͳ���ߵ�ǰ���̵��ύ�ڴ�����, ��λ:�ֽ�.
		Ҫ��ȡϵͳ��Χ�ڵ��ڴ��ύ���� -> GetPerformanceInfo
	DWORDLONG ullAvailPageFile;//��ǰ���̿����ύ������ڴ���, ��λ:�ֽ�. ��ֵС��ullTotalPageFile.
		Ҫ����ϵͳ��Χ�ڵĿ����ύ�ڴ�, ����GetPerformanceInfo��, ��CommitLimit��ֵ��ȥCommitTotal
	DWORDLONG ullTotalVirtual;//��������ռ���, �û�������(�û�ģʽ)���ڴ��С,��λ:�ֽ�.
	DWORDLONG ullAvailVirtual;//��������ռ���, �û���������ڴ���, δ�����δ�ύ���ڴ��С, ��λ:�ֽ�.
	DWORDLONG ullAvailExtendedVirtual;//������
} MEMORYSTATUSEX, *LPMEMORYSTATUSEX;
*/

//��������
/*
	1��	BOOL GlobalMemoryStatusEx(LPMEMORYSTATUSEX lpBuffer);//ϵͳ��ǰ�����ڴ�������ڴ�ʹ�����
	2��	GetSystemInfo(LPSYSTEM_INFO lpSystemInfo);//����ϵͳ��Ϣ
		GetNativeSystemInfo: ����64λ����ϵͳ�����е�32λ���� ��ȡϵͳ��Ϣ(��ʱ��Ӧʹ��GetSystemInfo)
	3��	GetSystemTime(LPSYSTEMTIME lpSystemTime)/SetSystemTime;//���ص�ǰϵͳ��UTCʱ��
		GetSystemTimeAsFileTime(LPFILETIME lpSystemTimeAsFileTime);//���ص�ǰϵͳ��UTCʱ��(�μ�LPFILETIME��LPSYSTEMTIME������)
		GetSystemTimePreciseAsFileTime: //�����ܸߵľ���(<1us)������ǰϵͳ���ں�ʱ��
	4��	GetLocalTime(LPSYSTEMTIME lpSystemTime)/SetLocalTime(SYSTEMTIME* lpSystemTime);
			���ر���ʱ��
	5��	DWORD GetVersion();//�汾�� ��:
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
	6��	DWORD GetTickCount();//ϵͳ�������������ĺ�����, �Ϊ49.7��
		ULONGLONG GetTickCount64();//ϵͳ�������������ĺ�����
	7��	GetSystemTimeAdjustment/SetSystemTimeAdjustment
			ȷ��ϵͳ�Ƿ����ʱ��ʱ��Ӧ�ö���ʱ�����, ����ȡ�κδ��������ֵ������
		BOOL GetSystemTimeAdjustmentPrecise/SetSystemTimeAdjustmentPrecise;
			ȷ��ϵͳ�Ƿ����ʱ��ʱ��Ӧ�������Ա��ʱ�����, ����ȡ�κδ��������ֵ������
	8��	GetSystemDirectoryA/GetSystemDirectoryW
			����ϵͳĿ¼, C:\Windows\System32
	9��	GetWindowsDirectoryA/GetWindowsDirectoryW
			����WindowsĿ¼, C:\Windows
		GetSystemWindowsDirectoryA/GetSystemWindowsDirectoryW
			���û�ϵͳ�Ϲ���WindowsĿ¼��·��, C:\Windows
	10��GetComputerNameExA/GetComputerNameExW
		SetComputerNameA,SetComputerNameW
		SetComputerNameExA
		SetComputerNameExW,
		SetComputerNameEx2W
		DnsHostnameToComputerNameExW
			��ȡ���������, ��������:
				NetBIOS	DESKTOP-G72T22C
				DNS hostname					DESKTOP-G72T22C
				DNS domain						""
				DNS fully-qualified				DESKTOP-G72T22C
				Physical NetBIOS				DESKTOP-G72T22C
				Physical DNS hostname			DESKTOP-G72T22C
				Physical DNS domain				""
				Physical DNS fully-qualified	DESKTOP-G72T22C
	11��GetLogicalProcessorInformation:�����������Ӳ������Ϣ
		(NUMA�ڵ���������CPU����CPU���������߼�CPU��������Cache��)
		GetLogicalProcessorInformationEx:�����������Ӳ��֮���ϵ����Ϣ
	12��GetProductInfo:���ؼ�����ϲ���ϵͳ�Ĳ�Ʒ����
	13��EnumSystemFirmwareTables:ö��ָ�����͵�����ϵͳ�̼���(ʹ��ʱ����ϸ����, Win10�����Ѳ�֧��)
		GetSystemFirmwareTable:�ӹ̼����ṩ�������ָ���Ĺ̼���
	14��GetPhysicallyInstalledSystemMemory: ����RAM�ڴ�Ĵ�С, ��λKB
	15��InstallELAMCertificateInfo: 
			��װ��Դ�ļ���ָ����֤����Ϣ, ���ļ�������ʱ���ӵ�ELAM��������;
			�����������Ӧ��ʹ�ô�API, ʹ���û�ģʽ�����ܱ����ķ������������;
	16��GetProcessorSystemCycleTime: �������Լ�������ִ���ӳٹ��̵���(DPC)���жϷ�������(ISR)���õ�����ʱ��
	17��GetIntegratedDisplaySize:������Ļ��С����ѹ���ֵ, ��λ:Ӣ��


*/

#endif // !_SSYSINFOAPI_H_
