#include "TLog.h"

#include <QDir>
#include <QFile>
#include <QDebug>
#include <QCoreApplication>

#if defined(Q_OS_WIN32)

#include <windows.h>
#include <DbgHelp.h>

#include "TConsumeQueue.h"

//生产DUMP文件
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
	HMODULE hDbgHelp = LoadLibrary("DbgHelp.dll");
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

			wsprintf(szFileName, "%s-%04d%02d%02d-%02d%02d%02d-%ld-%ld.dmp",
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


	//日志写入队列
	TConsumeQueue<sLogMsg> _queueLog(1000);
	//日志目录
	static QString _strLogPath;
	//当前日志文件
	static QString _strLogFileFormat;
	//单个日志文件最大
	qint64 _iMaxSpace = MAXLOGSPACE;
	//日志轮换种类
	TLog::nLogRotateType _logRotateType = TLog::Rotate_Hour;
	//程序启动时间
	static QDateTime _dateRunTime;

	//判断是否需要切换时间
	bool rotateTime()
	{
		//拼写日志格式
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
			logformat = logformat.arg(QString::fromUtf8("Debug"));
			break;
		case QtWarningMsg:
			logformat = logformat.arg(QString::fromUtf8("Warning"));
			break;
		case QtCriticalMsg:
			logformat = logformat.arg(QString::fromUtf8("Critical"));
			break;
		case QtFatalMsg:
			logformat = logformat.arg(QString::fromUtf8("Fatal"));
			break;
		case QtInfoMsg:
			logformat = logformat.arg(QString::fromUtf8("Info"));
			break;
		}

		return true;
	};


	void writeLog(sLogMsg logMsg)
	{
		rotateTime();

		//将日志进行区分
		QString tmpformat = _strLogFileFormat;
		handleLogType(logMsg.nType, tmpformat);

		//判断日志是否超过大小
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
	//生成dump文件
	SetUnhandledExceptionFilter(ExceptionFilter);
#endif
	//注册日志消息
	qInstallMessageHandler(processLog);

	_queueLog.consume([](const sLogMsg& logmsg) {
		writeLog(logmsg);
	});

	qDebug() << endl;
	qDebug() << endl;
	qDebug() << QString::fromUtf8("程序开始运行!... 程序名称: %1").arg(QCoreApplication::applicationName()).toUtf8().data();
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
	writeLog(sLogMsg{ QtDebugMsg , QString::fromUtf8("程序结束运行!... 程序名称: %1\r\n\r\n").arg(QCoreApplication::applicationName()).toUtf8().data() });
}

