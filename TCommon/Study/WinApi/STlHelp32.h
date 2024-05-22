#ifndef _STLHELP32_H_
#define _STLHELP32_H_

#ifdef WIN32
#include <TlHelp32.h>
#endif // WIN32


//һ������ͽṹ��
//1�����̿��ղ�������
/*
TH32CS_INHERIT: ���վ���ǿɼ̳е�
TH32CS_SNAPALL: ϵͳ�е����н��̺��߳�, �Լ� th32ProcessID ��ָ���Ľ��̵ĶѺ�ģ��
TH32CS_SNAPHEAPLIST: ������ th32ProcessID ��ָ���Ľ��̵����ж�
TH32CS_SNAPMODULE: ������ th32ProcessID ��ָ���Ľ��̵�����ģ��
TH32CS_SNAPMODULE32: ��64λ���̵���ʱ, ���������� th32ProcessID ��ָ���Ľ��̵�����32λģ��
TH32CS_SNAPPROCESS: ������ϵͳ�е����н���
TH32CS_SNAPTHREAD: ������ϵͳ�е������߳�

typedef struct tagPROCESSENTRY32 {
  DWORD     dwSize; // �ýṹ��Ĵ�С��
  DWORD     cntUsage; // ����Ϊ 0��
  DWORD     th32ProcessID; // ���̱�ʶ����
  ULONG_PTR th32DefaultHeapID; // ����Ϊ 0��
  DWORD     th32ModuleID; // ����Ϊ 0��
  DWORD     cntThreads; // ����������ִ���߳�����
  DWORD     th32ParentProcessID; // �����˽��̵Ľ��̵ı�ʶ����
  LONG      pcPriClassBase; // �˽��̴������κ��̵߳Ļ������ȼ���
  DWORD     dwFlags; // ����Ϊ 0��
  CHAR      szExeFile[MAX_PATH]; // ���̵Ŀ�ִ���ļ������ơ�
} PROCESSENTRY32;

typedef struct tagMODULEENTRY32 {
  DWORD   dwSize;  // �ýṹ��Ĵ�С��
  DWORD   th32ModuleID;  // ����Ϊ 0��
  DWORD   th32ProcessID; // Ҫ�����ģ��Ľ��̵ı�ʶ��.
  DWORD   GlblcntUsage; // ģ�������Ŀ��ͨ������Ϊ 0xFFFF��
  DWORD   ProccntUsage; // ͬ�ϡ�
  BYTE    *modBaseAddr; // ���������̵���������ģ��Ļ���ַ
  DWORD   modBaseSize; // ģ��Ĵ�С.
  HMODULE hModule; // ���������̵��������ж�ģ��ľ����
  char    szModule[MAX_MODULE_NAME32 + 1]; // ģ�����ơ�
  char    szExePath[MAX_PATH]; // ģ��·����
} MODULEENTRY32;

typedef struct tagTHREADENTRY32 {
  DWORD dwSize; // �ýṹ���С��
  DWORD cntUsage; // ����Ϊ 0��
  DWORD th32ThreadID; // �̱߳�ʶ����
  DWORD th32OwnerProcessID; // �������̵߳Ľ��̵ı�ʶ����
  LONG  tpBasePri; // ������̵߳��ں˻������ȼ�����(0~31)(���ȼ����Ϊ 0)��
  LONG  tpDeltaPri; // ����Ϊ 0��
  DWORD dwFlags; // ����Ϊ 0��
} THREADENTRY32;

typedef struct tagHEAPLIST32 {
  SIZE_T  dwSize; // �ýṹ��Ĵ�С��
  DWORD   th32ProcessID; // Ҫ���Ľ��̵ı�ʶ����
  ULONG_PTR th32HeapID; // �ѱ�ʶ����
  DWORD   dwFlags; // ͨ��Ϊ HF32_DEFAULT��
} HEAPLIST32;
*/


#endif
