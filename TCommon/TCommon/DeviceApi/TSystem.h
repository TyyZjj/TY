#ifndef _TSYSTEM_H_
#define _TSYSTEM_H_
/*ϵͳ��Ӳ�����*/

#include <QVersionNumber>
#include <QOperatingSystemVersion>
#include "TProcessThreads.h"

#ifdef WIN32
#include <TlHelp32.h>
#endif // WIN32


class TSystem
{
public:
	//1���߳����ڴ����� processthreadsapi.h -> GetCurrentProcessorNumber/GetCurrentProcessorNumberEx
	//2����ȡ��ǰϵͳ�����еĽ���Id
		//->���̿���:CreateToolhelp32Snapshot �� EnumProcessesö�ٽ���
		
	//�Ƿ���32λ����������64λ����ϵͳ��
	static bool IsWow64();

	//���򿪻�����
	static void PowerBoot(bool isStart = true);

	//************************************
	// ����: 	const QString & identifier: ʶ����
	// ����:	bool
	// ����:	�Ƿ�������ϵͳ���е��ڴ�����Ψһ��
	//************************************
	static bool IsSystemSingle(const QString& identifier);

#pragma region �����߳�ģ����Ϣ
	//************************************
	// ����: 	const QString & strProcessName: XXX.exe
	// ����:	QList<unsigned long>
	// ����:	���ݽ������Ʋ��ҽ���id
	//************************************
	static QList<unsigned long> GetProcIdByName(const QString &strProcName);
	//���ݽ���Id�����������(.exe������)
	static QString GetProcName(unsigned long iProcessId);

	//************************************
	// ����: 	const QString & strProcessName: XXX.exe
	// ����:	QMap<����ID, QList<�ý��̶�Ӧ�������߳�>>  //ע: ����, list�е�һ��id�����߳�id
	// ����:	���ݽ������Ʋ������ڲ��߳�id
	//************************************
	static QMap<unsigned long, QList<unsigned long>> GetProcThreadIdByName(const QString &strProcessName);

	//��ȡ���������е�ģ����
	static QList<QString> GetProcModuleNames(unsigned long iProcessId);

#ifdef WIN32
	//��ȡ���������е�ģ����Ϣ
	static QList<MODULEENTRY32W> GetProcModules(unsigned long iProcessId);
#endif // WIN32

	static HWND GetWidByProcId(DWORD dwProcessId, const CHAR *szTitle, QStringList &lstTitle);
#pragma endregion


#pragma region CPU���
	//��ȡCPU������
	static int GetProcessorNumber();
	//************************************
	// ����:	int
	// ����:	��ȡCPU������(�ٷֱ�)
	//************************************
	static int GetCpuUsage();
	//************************************
	// ����: 	void * handle: ���̾��, Ϊnullptrʱ���ȡ��ǰ����
	// ����:	int
	// ����:	��ȡ���̵�CPU������(�ٷֱ�), (ע��: ���ĳ������, ��һ�ε��ô˺���ʱ, �˺������Ƿ���-1)
	//************************************
	static int GetProcCpuUsage(void *handle = nullptr);
	static int GetProcCpuUsage(unsigned long iProcessId);
	//static int GetThreadCpuUsage(void *handle = nullptr);//�̵߳�cpu������, ���岻��

	//************************************
	// ����: 	int mask: ������ [0~64]
	// ����: 	void* handle: ���̾��
	// ����:	void
	// ����:	���ý���(�߳�)ʹ�����ʹ�õĺ�����
	// ָ�������߳�ʹ���ĸ�/��ЩCPU
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


#pragma region IO���
	//ϵͳ��io����
	//���̵�io����
	//winioctl.h _FILESYSTEM_STATISTICS
#pragma endregion


#pragma region �ڴ����
	//************************************
	// ����: 	int & memorypercent: �ڴ���ռ�ñ���
	// ����: 	int & memoryuse: ��ʹ�õ��ڴ� ��λ:MB
	// ����: 	int & memoryfree: ������ڴ� ��λ:MB
	// ����: 	int & memoryall: �ܵ��ڴ� ��λ:MB
	// ����:	void
	// ����:	��ȡ��ǰ�ڴ�״̬
	//************************************
	static void GetMemory(int& memorypercent, 
		int& memoryuse, 
		int& memoryfree, 
		int& memoryall);

	//���̵��ڴ�ʹ���� GetProcessMemoryInfo
#pragma endregion
};



#endif


