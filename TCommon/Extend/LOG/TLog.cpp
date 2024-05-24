#include "TLog.h"

#include <QDir>
#include <QFile>
#include <QDebug>
#include <QCoreApplication>

#if defined(Q_OS_WIN32)

#include <windows.h>
#include <DbgHelp.h>

#include "TConsumeQueue.h"

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
			ExpParam.ClientPointers = FALSE;

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

#endif


namespace
{
	struct sLogMsg
	{
		QtMsgType nType;
		QString strMsg;
	};


	//��־д�����
	TConsumeQueue<sLogMsg> _queueLog(1000);
	//��־Ŀ¼
	static QString _strLogPath;
	//��ǰ��־�ļ�
	static QString _strLogFileFormat;
	//������־�ļ����
	qint64 _iMaxSpace = MAXLOGSPACE;
	//��־�ֻ�����
	TLog::nLogRotateType _logRotateType = TLog::Rotate_Hour;
	//��������ʱ��
	static QDateTime _dateRunTime;

	//�ж��Ƿ���Ҫ�л�ʱ��
	bool rotateTime()
	{
		//ƴд��־��ʽ
		QString tmplogfileformat;
		if (_logRotateType == TLog::Rotate_Day)
		{
			tmplogfileformat = _strLogPath;
			QDir().mkpath(tmplogfileformat);

			tmplogfileformat.append("/Log_%1_");
			tmplogfileformat.append(QDateTime::currentDateTime().toString("yyyyMMdd"));
			tmplogfileformat.append("_%2.txt");
		}
		else if (_logRotateType == TLog::Rotate_Hour)
		{
			tmplogfileformat = _strLogPath + QDateTime::currentDateTime().toString("yyyyMMdd");
			QDir().mkpath(tmplogfileformat);

			tmplogfileformat.append("/Log_%1_");
			tmplogfileformat.append(QDateTime::currentDateTime().toString("yyyyMMddhh00"));
			tmplogfileformat.append("_%2.txt");
		}
		else if (_logRotateType == TLog::Rotate_RunTime)
		{
			tmplogfileformat.append("/Log_%1_");
			tmplogfileformat.append(_dateRunTime.toString("yyyyMMddhhmmss"));
			tmplogfileformat.append("_%2.txt");
		}
		
		if (tmplogfileformat == _strLogFileFormat)
		{
			return false;
		}
		else
		{
			_strLogFileFormat = tmplogfileformat;
			return true;
		}
	}

	bool handleLogType(QtMsgType msgtype, QString& logformat)
	{
		switch (msgtype)
		{
		case QtDebugMsg:
			logformat = logformat.arg(QString::fromLocal8Bit("Debug"));
			break;
		case QtWarningMsg:
			logformat = logformat.arg(QString::fromLocal8Bit("Warning"));
			break;
		case QtCriticalMsg:
			logformat = logformat.arg(QString::fromLocal8Bit("Critical"));
			break;
		case QtFatalMsg:
			logformat = logformat.arg(QString::fromLocal8Bit("Fatal"));
			break;
		case QtInfoMsg:
			logformat = logformat.arg(QString::fromLocal8Bit("Info"));
			break;
		}

		return true;
	};


	void writeLog(sLogMsg logMsg)
	{
		rotateTime();

		//����־��������
		QString tmpformat = _strLogFileFormat;
		handleLogType(logMsg.nType, tmpformat);

		//�ж���־�Ƿ񳬹���С
		int index = 0;
		QString tmpformat2 = tmpformat.arg(index);
		QFile inFile(tmpformat2);
		while (inFile.size() >= _iMaxSpace)
		{
			tmpformat2 = tmpformat.arg(index++);
			inFile.setFileName(tmpformat2);
		}

		inFile.open(QIODevice::Append | QIODevice::Text);

		QTextStream inStream(&inFile);
		inStream << logMsg.strMsg << endl;

		inFile.flush();
		inFile.close();
	}

	void processLog(QtMsgType type, const QMessageLogContext &context, const QString &msg)
	{
		QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
		QString line = QString("[%1] %2").arg(time).arg(msg);

		_queueLog.publish(sLogMsg{ type , line });
	}

}

void TLog::initLog(const QString& logpath)
{
	if (logpath.isEmpty())
		_strLogPath = qApp->applicationDirPath() + "/log/";
	else
		_strLogPath = logpath;
	_dateRunTime = QDateTime::currentDateTime();
#if defined(Q_OS_WIN32)
	//����dump�ļ�
	SetUnhandledExceptionFilter(ExceptionFilter);
#endif
	//ע����־��Ϣ
	qInstallMessageHandler(processLog);

	_queueLog.consume([](const sLogMsg& logmsg) {
		writeLog(logmsg);
	});

	qDebug() << endl;
	qDebug() << endl;
	qDebug() << QString::fromLocal8Bit("����ʼ����!... ��������: %1").arg(QCoreApplication::applicationName()).toUtf8().data();
}

void TLog::setLogFileMaxSpace(qint64 space /*= MAXLOGSPACE*/)
{
	_iMaxSpace = space;
}

void TLog::setLogFileRotateType(nLogRotateType type /*= Rotate_Hour*/)
{
	_logRotateType = type;
}

TLog::TLog()
{

}

TLog::~TLog()
{
	writeLog(sLogMsg{ QtDebugMsg , QString::fromLocal8Bit("�����������!... ��������: %1\r\n\r\n").arg(QCoreApplication::applicationName()).toUtf8().data() });
}

