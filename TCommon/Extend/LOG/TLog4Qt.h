#ifndef _TLOG4QT_H_
#define _TLOG4QT_H_

#include <QTimer>
#include <QDebug>
#include <QString>
#include <QCoreApplication>
#include "TSystem.h"

#if defined(Q_OS_WIN32)

#include <windows.h>
#include <io.h>
#include <DbgHelp.h>

//����DUMP�ļ�
int GenerateMiniDump(HANDLE hFile, PEXCEPTION_POINTERS pExceptionPointers, PWCHAR pwAppName)
{
	BOOL bOwnDumpFile = FALSE;
	HANDLE hDumpFile = hFile;
	MINIDUMP_EXCEPTION_INFORMATION ExpParam;

	typedef BOOL(WINAPI * MiniDumpWriteDumpT)(
		HANDLE,
		DWORD,
		HANDLE,
		MINIDUMP_TYPE,
		PMINIDUMP_EXCEPTION_INFORMATION,
		PMINIDUMP_USER_STREAM_INFORMATION,
		PMINIDUMP_CALLBACK_INFORMATION
		);

	MiniDumpWriteDumpT pfnMiniDumpWriteDump = nullptr;
	HMODULE hDbgHelp = LoadLibrary(L"DbgHelp.dll");
	if (hDbgHelp)
		pfnMiniDumpWriteDump = (MiniDumpWriteDumpT)GetProcAddress(hDbgHelp, "MiniDumpWriteDump");

	if (pfnMiniDumpWriteDump)
	{
		if (hDumpFile == nullptr || hDumpFile == INVALID_HANDLE_VALUE)
		{
			//TCHAR szPath[MAX_PATH] = { 0 };
			TCHAR szFileName[MAX_PATH] = { 0 };
			//TCHAR* szAppName = pwAppName;
			TCHAR* szVersion = (TCHAR*)L"v1.0";
			TCHAR dwBufferSize = MAX_PATH;
			SYSTEMTIME stLocalTime;

			GetLocalTime(&stLocalTime);
			//GetTempPath(dwBufferSize, szPath);

			//wsprintf(szFileName, L"%s%s", szPath, szAppName);
			CreateDirectory(szFileName, nullptr);

			wsprintf(szFileName, L"%s-%04d%02d%02d-%02d%02d%02d-%ld-%ld.dmp",
				//szPath, szAppName, szVersion,
				szVersion,
				stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay,
				stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond,
				GetCurrentProcessId(), GetCurrentThreadId());
			hDumpFile = CreateFile(szFileName, GENERIC_READ | GENERIC_WRITE,
				FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);

			bOwnDumpFile = TRUE;
			OutputDebugString(szFileName);
		}

		if (hDumpFile != INVALID_HANDLE_VALUE)
		{
			ExpParam.ThreadId = GetCurrentThreadId();
			ExpParam.ExceptionPointers = pExceptionPointers;
			ExpParam.ClientPointers = TRUE;

			pfnMiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),
				hDumpFile, MiniDumpWithDataSegs, (pExceptionPointers ? &ExpParam : nullptr), nullptr, nullptr);

			if (bOwnDumpFile)
				CloseHandle(hDumpFile);
		}
	}

	if (hDbgHelp != nullptr)
		FreeLibrary(hDbgHelp);

	return EXCEPTION_EXECUTE_HANDLER;
}

LONG WINAPI ExceptionFilter(LPEXCEPTION_POINTERS lpExceptionInfo)
{
	if (IsDebuggerPresent())
	{
		return EXCEPTION_CONTINUE_SEARCH;
	}

	return GenerateMiniDump(nullptr, lpExceptionInfo, (PWCHAR)L"test");
}


//��ȡ�汾
std::string GetSoftVersion(const char* exepath)
{
	std::string strVersionInfo;
	if (!exepath)
		return strVersionInfo;
	if (_access(exepath, 0) != 0)
		return strVersionInfo;

	HMODULE hDll = NULL;
	char szDbgHelpPath[MAX_PATH];

	sprintf_s(szDbgHelpPath, sizeof(szDbgHelpPath), "version.dll");
	hDll = ::LoadLibraryA(szDbgHelpPath);
	if (hDll == NULL)
	{
		return strVersionInfo;
	}

	typedef DWORD(WINAPI* func_GetFileVersionInfoSizeA)(LPCSTR, LPDWORD);
	func_GetFileVersionInfoSizeA p_GetFileVersionInfoSizeA =
		(func_GetFileVersionInfoSizeA)::GetProcAddress(hDll, "GetFileVersionInfoSizeA");


	typedef DWORD(WINAPI* func_GetFileVersionInfoA)(LPCSTR, DWORD, DWORD, LPVOID);
	func_GetFileVersionInfoA p_GetFileVersionInfoA =
		(func_GetFileVersionInfoA)::GetProcAddress(hDll, "GetFileVersionInfoA");


	typedef DWORD(WINAPI* func_VerQueryValueA)(LPCVOID, LPCSTR, LPVOID*, PUINT);
	func_VerQueryValueA p_VerQueryValueA =
		(func_VerQueryValueA)::GetProcAddress(hDll, "VerQueryValueA");


	if (p_GetFileVersionInfoSizeA == NULL
		|| p_GetFileVersionInfoA == NULL
		|| p_VerQueryValueA == NULL)
	{
		if (hDll)
		{
			FreeLibrary(hDll);
			hDll = NULL;
		}
		return strVersionInfo;
	}

	UINT infoSize = p_GetFileVersionInfoSizeA(exepath, 0);
	if (infoSize != 0) {
		strVersionInfo.resize(infoSize, 0);
		char* pBuf = NULL;
		pBuf = new char[infoSize];
		VS_FIXEDFILEINFO* pVsInfo;
		if (p_GetFileVersionInfoA(exepath, 0, infoSize, pBuf)) {
			if (p_VerQueryValueA(pBuf, "\\", (void**)&pVsInfo, &infoSize))
			{
				sprintf_s(pBuf, infoSize, "%d.%d.%d.%d",
					HIWORD(pVsInfo->dwFileVersionMS),
					LOWORD(pVsInfo->dwFileVersionMS),
					HIWORD(pVsInfo->dwFileVersionLS),
					LOWORD(pVsInfo->dwFileVersionLS));

				strVersionInfo = pBuf;
			}
		}
		delete[] pBuf;
	}

	if (hDll)
	{
		FreeLibrary(hDll);
	}
	return strVersionInfo;
}

#endif


#include "log4qt/logger.h"
#include "log4qt/log4qt.h"
#include "log4qt/logmanager.h"
#include "log4qt/propertyconfigurator.h"
class TLog4Qt
{
public:

	//************************************
	// ����:		initLog
	// ����: 	const QString & logpath
	// ����:		void
	// ����:		��ʼ����־, ���̿�ʼʱ����
	//************************************
	static void initLog(const QString& logconfig = "")
	{
#if defined(Q_OS_WIN32)
		//����dump�ļ�
		SetUnhandledExceptionFilter(ExceptionFilter);
#endif
		if (logconfig.isEmpty())
			Log4Qt::PropertyConfigurator::configure(QCoreApplication::applicationDirPath() + +"/Config/Log4Qt.conf");
		else
			Log4Qt::PropertyConfigurator::configure(logconfig);

		qDebug() << QString::fromLocal8Bit("=============================================").toUtf8().data();
		qDebug() << QString::fromLocal8Bit("����ʼ����!... ��������: %1").arg(QCoreApplication::applicationName()).toUtf8().data();
		qDebug() << QString::fromLocal8Bit("=============================================").toUtf8().data();
	};

	//�����˳�ǰ����
	static void shutDownLog()
	{
		qDebug() << QString::fromLocal8Bit("=============================================").toUtf8().data();
		qDebug() << QString::fromLocal8Bit("�����������!... ��������: %1").arg(QCoreApplication::applicationName()).toUtf8().data();
		qDebug() << QString::fromLocal8Bit("=============================================").toUtf8().data();

	}

	static void monitoring(int interval = 60000)
	{
		if (interval >= 10000)
		{
			QTimer::singleShot(interval, [&, interval]() {

				int cpupercent;
				cpupercent = TSystem::GetCpuUsage();

				int memorypercent, memoryuse, memoryfree, memoryall;
				TSystem::GetMemory(memorypercent, memoryuse, memoryfree, memoryall);

				qDebug() << QString::fromLocal8Bit("=======CPU: %1%  �ڴ�: %2% (���� %3 MB / �� %4 MB)=======")
					.arg(cpupercent)
					.arg(memorypercent)
					.arg(memoryuse)
					.arg(memoryall)
					.toUtf8().data();

				monitoring(interval);
			});
		}

	}

	static void version(const QStringList& filelst)
	{
		qDebug() << QString::fromLocal8Bit("=============================================").toUtf8().data();
		for (auto file : filelst)
		{
			QString version = QString::fromLocal8Bit(GetSoftVersion(file.toLocal8Bit().data()).c_str());

			qDebug() << QString::fromLocal8Bit("=======�ļ�����: %1 �汾��Ϣ: %2=======")
				.arg(file)
				.arg(version)
				.toUtf8().data();

		}
		qDebug() << QString::fromLocal8Bit("=============================================").toUtf8().data();
	};
};
#endif