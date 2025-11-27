#ifndef CDBPROCESS_H
#define CDBPROCESS_H
//CDBProcess db;
//db.openDB("mytest.db");
//void * pQry = db.openRecordsetBySql("select * from tb1");
//while (db.moveNext(pQry))
//{
//	db.readFieldsValueFromRec("sn", iSN,          //int
//		"name", strName, //QString
//		"height", dHeight,   //double
//		"birthday", dt,             //QDateTime
//		"photo", ba);           //QByteArray
//}
//db.closeRecordset(pQry);
//== == == == == == == == == == == == == == == == == == == == == == == == == == =
//db.addFieldsValueToTbl("TB1",                  //表名
//							"sn", iSN,
//							"name", strName,
//							"height", dHeight,
//							"birthday", dt,
//							"photo", ba);
//== == == == == == == == == == == == == == == == == == == == == == == == == ==
//db.updateTblFieldsValue("TB1", "WHERE sn=20", "height", dHeight,
//							"photo", ba);



#include <QString>
#include <QVariant>
#include <QDateTime>
#include <QSqlQuery>
#include <QSqlError>
#include <QMutex>
#include <QDebug>
struct sTransQrys
{
	QString strSql;
	QStringList lstFlds;
	QList<QVariant> lstVars;

	bool operator == (const sTransQrys& other) const   //注: const 不能省
	{
		return (strSql == other.strSql
			&& lstFlds == other.lstFlds
			&& lstVars == other.lstVars);
	}
};

class QSqlDatabase;
class QSqlQuery;
class CDBProcess
{
public:
	CDBProcess(const QString strType = "sqlite"); //sqlite  mysql  access sqlserver
	virtual ~CDBProcess();
	static QStringList getDBDrivers();
	//************************************
	// 作者:    ysn
	// 日期:    2018/02/25
	// 参数:    bool isopen
	// 返回:    void
	// 功能:		开启事务模式
	//************************************
	void openTransAct(bool isopen);
	//************************************
	// 作者:    ysn
	// 日期:    2018/02/25
	// 返回:    bool
	// 功能:		开启事务模式，需要执行此函数，提交事务	
	//************************************
	bool commitTrans();
	//************************************
	// 作者:    ysn
	// 日期:    2018/02/23
	// 参数:    const QString strSvrName
	// 参数:    const int nPort
	// 参数:    const QString strDBname
	// 参数:    const QString strUserID
	// 参数:    const QString strUserPwd
	// 返回:    bool
	// 功能:		打开数据库
	//			对于sqlite，只有strDBname有效，其它参数忽略，如db.openDB("", "mysqlite.db")
	//			对于MS Access数据库，strSvrName为空，用户名默认为"sa"，如db.openDB("", "myaccess.mdb");
	//			对MSAccess数据库，也可通过一个UDL文件，打开，如db.openDB("my.udl", "");
	//			mysql和sqlserver，就是各个参数依次提供，如db.openDB("svr1", "mydb", "user1", "abcd");
	//************************************
	bool openDB(
		const QString strDBname,
		const QString strSvrName = "", const int nPort = 0,
		const QString strUserID = "",
		const QString strUserPwd = "");
	void closeDB();
	//************************************
	// 作者:    ysn
	// 日期:    2018/02/23
	// 参数:    const QString
	// 返回:    bool
	// 功能:		执行Sql语句
	//************************************
	bool excuteSQL(const QString);
	////************************************
	//// 作者:    ysn
	//// 日期:    2018/02/23
	//// 参数:    const QString strExcutePara
	//// 返回:    bool
	//// 功能:		执行Sql语句，加入防锁死
	////************************************
	//bool excuteSQL2(const QString strExcutePara);
	//************************************
	// 作者:    ysn
	// 日期:    2018/02/23
	// 参数:    const QString strSql
	// 返回:    void *
	// 功能:		打开记录集
	//************************************
	void *openRecordsetBySql(const QString strSql);
	//************************************
	// 作者:    ysn
	// 日期:    2018/02/23
	// 参数:    void *
	// 返回:    void
	// 功能:		关闭记录集
	//************************************
	void closeRecordset(void *);
	//************************************
	// 作者:    ysn
	// 日期:    2018/02/23
	// 返回:    bool
	// 功能:		是否打开 
	//************************************
	bool isOpen() const;
	//************************************
	// 作者:    ysn
	// 日期:    2018/02/23
	// 参数:    void * p
	// 返回:    bool
	// 功能:		记录集游标是否在结尾
	//************************************
	bool recEOF(void *p) const;
	//************************************
	// 作者:    ysn
	// 日期:    2018/02/23
	// 参数:    void * p
	// 返回:    bool
	// 功能:		记录集游标是否在第一条记录之前
	//************************************
	bool recBOF(void *p) const;
	//************************************
	// 作者:    ysn
	// 日期:    2018/02/23
	// 参数:    const QString strTable
	// 返回:    bool
	// 功能:		表是否存在
	//************************************
	bool isTableExist(const QString strTable);
	//************************************
	// 作者:    ysn
	// 日期:    2018/02/23
	// 参数:    const QString
	// 返回:    bool
	// 功能:		删除一个表
	//************************************
	bool dropTable(const QString& strTbl);
	//************************************
	// 作者:    ysn
	// 日期:    2019/03/19
	// 参数:    const QString
	// 返回:    bool
	// 功能:		清空一个表
	//************************************
	bool deleteTable(const QString& strTbl);
	//************************************
	// 作者:    ysn
	// 日期:    2018/02/23
	// 参数:    void * p
	// 返回:    long
	// 功能:		获取记录数
	//************************************
	long getRecordCount(void *p) const;
	//************************************
	// 作者:    ysn
	// 日期:    2018/02/23
	// 参数:    void * p
	// 返回:    bool
	// 功能:		记录集光标移动
	//************************************
	bool moveFirst(void *p) const;
	bool movePrevious(void *p) const;
	bool moveNext(void *p) const;
	bool moveLast(void *p) const;
	bool moveTo(int n, void *p) const;
	//************************************
	// 作者:    ysn
	// 日期:    2018/02/23
	// 返回:    bool
	// 功能:		事务操作
	//************************************
	bool transaction();
	bool execsqls(const QStringList& sqls);
	bool commit();
	bool rollback();
	//************************************
	// 作者:    ysn
	// 日期:    2018/02/23
	// 返回:    QString
	// 功能:		获取数据库名称
	//************************************
	QString getDbName() const
	{
		return __strDbName;
	}
	QString getDbType() const
	{
		return __strDbType;
	}
public:
	//************************************
	// 作者:    ysn
	// 日期:    2018/02/23
	// 参数:    const void * p
	// 参数:    const QString str
	// 参数:    T & t
	// 返回:    bool
	// 功能:		读取记录集的当前记录的各字段值
	//			void * pQry = db.openRecordsetBySql("select * from tb1");
	//			while (db.moveNext(pQry))
	//			{
	//				db.readFieldsValueFromRec("sn", iSN,          //int
	//										"name", strName, //QString
	//										"height", dHeight,   //double
	//										"birthday", dt,             //QDateTime
	//										"photo", ba);           //QByteArray
	//			}
	//			db.closeRecordset(pQry);
	template <typename T>
	bool readFieldsValueFromRec(const void *p, const QString str, T& t) const
	{
		const QSqlQuery *pQry = static_cast<const QSqlQuery *>(p);
		if (nullptr == pQry || false == pQry->isValid())
		{
			return false;
		}
		t = pQry->value(str).value<T>();
		return true;
	}

	template <typename T, typename ... Args>
	bool readFieldsValueFromRec(const void *p, const QString str, T & t, Args& ... args) const
	{
		const QSqlQuery *pQry = static_cast<const QSqlQuery *>(p);
		if (nullptr == pQry || false == pQry->isValid())
		{
			return false;
		}
		t = pQry->value(str).value<T>();
		bool bRet = readFieldsValueFromRec(p, args...);
		//QSqlError lastError = qry.lastError();
		//QString strErr = lastError.driverText();
		QSqlError lastError = pQry->lastError();
		if (lastError.isValid())
		{
			qWarning() << lastError.text();
		}
		return bRet;
	}
	template <typename T>
	bool readFieldsValueFromRecByIndex(const void *p, const int index, T& t) const
	{
		const QSqlQuery *pQry = static_cast<const QSqlQuery *>(p);
		if (nullptr == pQry || false == pQry->isValid())
		{
			return false;
		}
		t = pQry->value(index).value<T>();
		return true;
	}
private:
	template <typename T>
	void extractArg1(QStringList &lstFlds, QList<QVariant> &lstVars, const T &t) const
	{
		const QVariant var(t);
		if (lstFlds.size() == lstVars.size())
		{
			lstFlds << var.toString();
		}
		else
		{
			lstVars << var;
		}
	}
public:
	//************************************
	// 作者:    ysn
	// 日期:    2018/02/23
	// 参数:    const QString & strTbl
	// 参数:    const QString & strFld
	// 参数:    const T & t
	// 参数:    const Args & ... args
	// 返回:    bool
	// 功能:		向一个数据表中填加一条记录
	//== == == == == == == == == == == == == == == == == == == == == == == == == == =
	//			db.addFieldsValueToTbl("TB1",                  //表名
	//									"sn", iSN,
	//									"name", strName,
	//									"height", dHeight,
	//									"birthday", dt,
	//									"photo", ba);
	//************************************
	template<typename T, typename ... Args>
	bool addFieldsValueToTbl(const QString &strTbl, const QString &strFld, const T &t, const Args &... args)
	{
		QStringList lstFlds;
		QList<QVariant> lstVars;
		lstFlds << strFld;
		lstVars.push_back(t);
		std::initializer_list<int> {(extractArg1(lstFlds, lstVars, args), 0)...};
		//--------------------------------------
		QString strFlds;
		QString strPlaceholders;
		for (const QString &str : lstFlds)
		{
			strFlds += "," + str;
			strPlaceholders += ",:" + str;
		}
		QString strSql = QString("REPLACE INTO %1(%2) VALUES (%3)").arg(strTbl).arg(strFlds.mid(1)).arg(strPlaceholders.mid(1));
	

		QMutexLocker lock(&__mutexDB);
		if (!__bisTrans)
		{
			QSqlQuery qry(*__pDB);
			qry.prepare(strSql);
			const int iCount = lstFlds.size();
			for (int i = 0; i < iCount; ++i)
			{
				qry.bindValue(":" + lstFlds.at(i), lstVars.at(i));
			}
			const bool bRet = qry.exec();
			QSqlError lastError = qry.lastError();
			if (lastError.isValid())
			{
				qDebug() << lastError.text();
			}
			return bRet;
		}
		else
		{
			__lstTransQrys.append(sTransQrys{ strSql, lstFlds, lstVars });
			return true;
		}
	}
	template<typename T, typename ... Args>
	bool addFieldsValueToTbl2(const QString &strTbl, const QString &strFld, const T &t, const Args &... args)
	{
		QStringList lstFlds;
		QList<QVariant> lstVars;
		lstFlds << strFld;
		lstVars.push_back(t);
		std::initializer_list<int> {(extractArg1(lstFlds, lstVars, args), 0)...};
		//--------------------------------------
		QString strFlds;
		QString strPlaceholders;
		for (const QString &str : lstFlds)
		{
			strFlds += "," + str;
			strPlaceholders += ",:" + str;
		}
		QString strSql = QString("REPLACE INTO %1(%2) VALUES (%3)").arg(strTbl).arg(strFlds.mid(1)).arg(strPlaceholders.mid(1));

		QMutexLocker lock(&__mutexDB);
		QSqlQuery qry(*__pDB);
		qry.prepare(strSql);
		const int iCount = lstFlds.size();
		for (int i = 0; i < iCount; ++i)
		{
			qry.bindValue(":" + lstFlds.at(i), lstVars.at(i));
		}
		const bool bRet = qry.exec();
		QSqlError lastError = qry.lastError();
		if (lastError.isValid())
		{
			qDebug() << lastError.text();
		}
		return bRet;
	}
	//************************************
	// 作者:    ysn
	// 日期:    2018/02/23
	// 参数:    const QString & strTbl
	// 参数:    const QString & strWhere
	// 参数:    const QString & strFld
	// 参数:    const T & t
	// 参数:    const Args & ... args
	// 返回:    bool
	// 功能:		修改表中符合WHERE子句条件的记录
	//== == == == == == == == == == == == == == == == == == == == == == == == == ==
	//			db.updateTblFieldsValue("TB1", 
	//									"WHERE sn=20", 
	//									"height", dHeight,
	//									"photo", ba);
	//************************************
	template<typename T, typename ... Args>
	bool updateTblFieldsValue(const QString &strTbl, const QString &strWhere, const QString &strFld, const T &t, const Args &... args)
	{
		QStringList lstFlds;
		QList<QVariant> lstVars;
		lstFlds << strFld;
		lstVars << t;
		std::initializer_list<int> {(extractArg1(lstFlds, lstVars, args), 0)...};
		//---------------------------
		QString strFlds;
		for (const QString &str : lstFlds)
		{
			strFlds += "," + str + "=:" + str;
		}
		QString strSql = QString("UPDATE %1 SET %2 %3").arg(strTbl).arg(strFlds.mid(1)).arg(strWhere);

		QMutexLocker lock(&__mutexDB);
		QSqlQuery qry(*__pDB);
		qry.prepare(strSql);
		//qDebug() << strSql;
		const int iCount = lstFlds.size();
		for (int i = 0; i < iCount; ++i)
		{
			qry.bindValue(":" + lstFlds.at(i), lstVars.at(i));
		}
		if (!__bisTrans)
		{
			const bool bRet = qry.exec();
			QSqlError lastError = qry.lastError();
			if (lastError.isValid())
			{
				qWarning() << lastError.text();
			}
			return bRet;
		}
		else
		{
			__lstTransQrys.append(sTransQrys{ strSql, lstFlds, lstVars });
			return true;
		}
	}
	template<typename T, typename ... Args>
	bool updateTblFieldsValue2(const QString &strTbl, const QString &strWhere, const QString &strFld, const T &t, const Args &... args)
	{
		QStringList lstFlds;
		QList<QVariant> lstVars;
		lstFlds << strFld;
		lstVars << t;
		std::initializer_list<int> {(extractArg1(lstFlds, lstVars, args), 0)...};
		//---------------------------
		QString strFlds;
		for (const QString &str : lstFlds)
		{
			strFlds += "," + str + "=:" + str;
		}
		QString strSql = QString("UPDATE %1 SET %2 %3").arg(strTbl).arg(strFlds.mid(1)).arg(strWhere);

		QMutexLocker lock(&__mutexDB);
		QSqlQuery qry(*__pDB);
		qry.prepare(strSql);
		//qDebug() << strSql;
		const int iCount = lstFlds.size();
		for (int i = 0; i < iCount; ++i)
		{
			qry.bindValue(":" + lstFlds.at(i), lstVars.at(i));
		}
		const bool bRet = qry.exec();
		QSqlError lastError = qry.lastError();
		if (lastError.isValid())
		{
			qWarning() << lastError.text();
		}
		return bRet;
	}
protected:
	QMutex					__mutexDB;
	QSqlDatabase *			__pDB;
protected:
	//bool __openMDB(const QString strMDBname, QString strUserID = "sa", const QString strPassword = "");
	//bool __openMDBByUDL(const QString strUDL);
	//bool __connectSqlServer(const QString strSvrName, const QString strDBname, const QString strUserID, const QString strUserPwd);
	//bool __openSqlite(const QString &strDbName);
	bool __openMySql(QString strSvrName, int nPort, const QString strDBname, QString strUserID, const QString strUserPwd);
private:

	QString					__strConnName;
	QString					__strDbType;
	QString					__strDbName;
	QList<QSqlQuery *>		__lstQrys;

	
	QList<sTransQrys>		__lstTransQrys;
	bool					__bisTrans;

};

#endif // CDBPROCESS_H
