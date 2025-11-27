#ifndef LIBDBOPERATION_H
#define LIBDBOPERATION_H

#include "dbdatadefine.h"
#include <QObject>
#include <QMap>
#include <QThread>
#include <QMutex>
//#include "commondefine.h"

#ifndef Kernel
#define Kernel(ClassName)\
static ClassName* GetKernel(){static ClassName * instance = nullptr; if(!instance){instance = new ClassName;}return instance;}\
static void DestroyKernel(){ ClassName * instance = ClassName::GetKernel(); if(instance){delete instance;instance=nullptr;}}
#endif


class CDBProcess;
class CTransThread;
class LibDBOperation : public QObject
{
public:
	//************************************
	// 作者:    ysn
	// 日期:    2017/12/15
	// 返回:    LibCore *
	// 功能: 
	//************************************
	//static LibDBOperation* GetKernel()
	//{
	//	static LibDBOperation instance;
	//	return &instance;
	//};
	Kernel(LibDBOperation)
	//************************************
	// 作者:    ysn
	// 日期:    2018/02/25
	// 参数:    const DBInfo & info
	// 返回:    bool
	// 功能:		打开和关闭
	//************************************
	bool openDB(const sDBInfo& info);
	bool closeDB(const QString& name);
	bool isDBOpen(const QString& name);
	//************************************
	// 作者:    ysn
	// 日期:    2018/02/25
	// 参数:    const QString & dbname
	// 参数:    const QString & path
	// 返回:    bool
	// 功能:		根据文件生成数据库
	//************************************
	bool initDB(const QString& dbname, const QString& path);
	//************************************
	// 作者:    ysn
	// 日期:    2018/02/25
	// 返回:    bool
	// 功能:		事务模式 提交事务
	//************************************
	//bool commitTrans();
	

protected:


protected:	
	LibDBOperation(QObject *parent = 0);
	~LibDBOperation();
	Q_DISABLE_COPY(LibDBOperation)
	//************************************
	// 作者:    ysn
	// 日期:    2018/02/25
	// 返回:    CDBProcess 
	// 功能:		获取DB指针
	//************************************
	CDBProcess * getDbProcess(const QString& dbname);
	
protected:
	QMutex									m_mutexDb;
	QMap<QString, CDBProcess *>				m_dbProMap;
	
	CTransThread *							m_pTransThread;
};

class CTransThread : public QThread
{
	Q_OBJECT
public:
	//************************************
	// 作者:		yangshunan
	// 日期:		2020/07/30
	// 函数:		CTransThread
	// 参数: 	QObject * parent
	// 返回:		
	// 功能:		事务处理线程 
	//************************************
	CTransThread(QObject *parent = nullptr);

	//************************************
	// 作者:		yangshunan
	// 日期:		2020/07/30
	// 函数:		addTransDb
	// 参数: 	CDBProcess * pdb
	// 返回:		void
	// 功能:		添加需要执行事务的数据库
	//************************************
	void addTransDb(CDBProcess * pdb);
protected:
	void run();
private:
	QList<CDBProcess *>						m_lstTransDb;
};
#endif // LIBDBOPERATION_H
