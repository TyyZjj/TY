#ifndef _TLOG_H_
#define _TLOG_H_


#include <QObject>
#include <QTime>
#include <QString>

#define MAXLOGSPACE 100*1024*1024		//100MB

class TLog :public QObject
{
	Q_OBJECT;
public:
	static TLog * GetKernel()
	{
		static TLog instance;
		return &instance;
	};
	//日志模式
	enum nLogRotateType
	{
		Rotate_Day,		//按天存
		Rotate_Hour,	//按小时保存
		Rotate_RunTime,	//按运行时间保存
	}; 

	//************************************
	// 函数:		initLog
	// 参数: 	const QString & logpath
	// 返回:		void
	// 功能:		初始化日志
	//************************************
	void initLog(const QString& logpath = "");

	//************************************
	// 函数:		setLogFileMaxSpace
	// 参数: 	qint64 space
	// 返回:		void
	// 功能:		设置每份日志最大大小
	//************************************
	void setLogFileMaxSpace(qint64 space = MAXLOGSPACE);
	//************************************
	// 函数:		setLogFileRotateType
	// 参数: 	nLogRotateType type
	// 返回:		void
	// 功能:		设置日志分隔种类
	//************************************
	void setLogFileRotateType(nLogRotateType type = Rotate_Hour);
private:
	TLog();
	~TLog();
};

#endif // !_TLOG_H_