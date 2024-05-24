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
	//��־ģʽ
	enum nLogRotateType
	{
		Rotate_Day,		//�����
		Rotate_Hour,	//��Сʱ����
		Rotate_RunTime,	//������ʱ�䱣��
	}; 

	//************************************
	// ����:		initLog
	// ����: 	const QString & logpath
	// ����:		void
	// ����:		��ʼ����־
	//************************************
	void initLog(const QString& logpath = "");

	//************************************
	// ����:		setLogFileMaxSpace
	// ����: 	qint64 space
	// ����:		void
	// ����:		����ÿ����־����С
	//************************************
	void setLogFileMaxSpace(qint64 space = MAXLOGSPACE);
	//************************************
	// ����:		setLogFileRotateType
	// ����: 	nLogRotateType type
	// ����:		void
	// ����:		������־�ָ�����
	//************************************
	void setLogFileRotateType(nLogRotateType type = Rotate_Hour);
private:
	TLog();
	~TLog();
};

#endif // !_TLOG_H_