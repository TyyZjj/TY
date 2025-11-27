#include "cdbprocess.h"
// DBProcess.cpp :
/************************************************************************
*Description:                                                           *
*Author: realfan                                                        *
*Date of Created:                                                       *
*Date of Modified 2016.12.09 Purpose of Modify Person Name of Modify    *
*------------------  ----------------------  -------------------------  *
*                                                                       *
************************************************************************/
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlDriver>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDir>
#include <QCoreApplication>
#include <QDebug>
//#include <QMessageBox>
#include <QSqlError>
#include <QFileInfo>
//#include "libpublic.h"


//=================================
CDBProcess::CDBProcess(const QString strType)
{
	__strDbType = strType.toUpper();
	QString dbType("SQLITE");
	if ("SQLITE" == __strDbType)
		dbType = "QSQLITE"; //QTPLUGIN += qsqlite Q_IMPORT_PLUGIN(QSQLiteDriverPlugin)
	else if ("MYSQL" == __strDbType)
		dbType = "QMYSQL"; //QTPLUGIN += qsqlmysql Q_IMPORT_PLUGIN(QMYSQLDriverPlugin)
	else if ("SQLSERVER" == __strDbType)
		dbType = "QODBC"; //QTPLUGIN += qsqlodbc  Q_IMPORT_PLUGIN(QODBCDriverPlugin)
	else if ("ACCESS" == __strDbType)
		dbType = "QODBC"; //QTPLUGIN += qsqlodbc  Q_IMPORT_PLUGIN(QODBCDriverPlugin)
	else
	{
		dbType = "";
		//QMessageBox::critical(0, "ERROR", "DB type name invalid!");
		return;
	}
	int iConnIdx = 0;
	while (1)
	{
		__strConnName = QString("MyDBProcessConn%1").arg(++iConnIdx);
		QSqlDatabase dbConn = QSqlDatabase::database(__strConnName, false);

		if (dbConn.isValid())//存在连接
		{
			continue;
		}
		__pDB = new QSqlDatabase(QSqlDatabase::addDatabase(dbType, __strConnName));
		break;
	}

	__bisTrans = false;
}
CDBProcess::~CDBProcess()
{
	qDeleteAll(__lstQrys);
	__lstQrys.clear();
	closeDB();
}

QStringList CDBProcess::getDBDrivers()
{
	return QSqlDatabase::drivers();
}



bool CDBProcess::openDB(
	const QString strDBname,
	const QString strSvrName,
	const int nPort,
	const QString strUserID,
	const QString strUserPwd)
{
	if (isOpen())
		return false;
	__strDbName = strDBname;
	bool bRet = false;
/*	if ("SQLITE" == __strDbType)
	{
		bRet = __openSqlite(strDBname);
	}
	else */if ("MYSQL" == __strDbType)
	{
		bRet = __openMySql(strSvrName, nPort, strDBname, strUserID, strUserPwd);
	}
	//else if ("SQLSERVER" == __strDbType)
	//{
	//	bRet = __connectSqlServer(strSvrName, strDBname, strUserID, strUserPwd);
	//}
	//else if ("ACCESS" == __strDbType)
	//{
	//	if (strSvrName.length() < 1)
	//	{
	//		bRet = __openMDB(strDBname, strUserID, strUserPwd);
	//	}
	//	else
	//	{
	//		bRet = __openMDBByUDL(strSvrName); //here strSvrName is UDL file name
	//	}
	//}
	QSqlError lastError = __pDB->lastError();
	if (lastError.isValid())
	{
		qWarning() << lastError.text();
	}
	return bRet;
}
bool CDBProcess::__openMySql(QString strSvrName,
	int nPort,
	QString strDBname,
	QString strUserID,
	const QString strUserPwd)
{
	if (strSvrName.length() < 1) strSvrName = "localhost";
	if (strUserID.length() < 1) strUserID = "root";
	if (strDBname.length() < 1) return false;

	__pDB->setConnectOptions("MYSQL_OPT_RECONNECT=1");
	__pDB->setHostName(strSvrName);
	__pDB->setPort(nPort);
	__pDB->setDatabaseName(strDBname);
	__pDB->setUserName(strUserID);//"root");
	__pDB->setPassword(strUserPwd);
	if (false == __pDB->open())
	{
		return false;
	}
	//m_pDB->exec("SET NAMES utf8;");
	return true;
}
bool CDBProcess::excuteSQL(const QString strExcutePara)
{
	QMutexLocker lock(&__mutexDB);
	QSqlQuery query(*__pDB);
	query.clear();
	if (strExcutePara.isEmpty())
	{
		return true;
	}
	const bool b = query.exec(strExcutePara);
	QSqlError lastError = query.lastError();
	if (lastError.isValid())
	{
		qWarning() << lastError.text();
	}
	return b;
}
//bool CDBProcess::excuteSQL2(const QString strExcutePara)
//{
//	QMutexLocker lock(&__mutexDB);
//	QSqlQuery query(*__pDB);
//	query.clear();
//	bool ret = false;
//	while (1)
//	{
//		int err = 0;
//		if (!query.exec(strExcutePara))
//		{
//			if (query.lastError().text().contains("locked"))
//			{
//				if (err < 5)
//				{
//					err++;
//					LibPublic::Sleep(100);
//					continue;
//				}
//				else
//				{
//					break;
//				}
//			}
//			break;
//		}
//		ret = true;
//		break;
//	}
//	return ret;
//}
bool CDBProcess::dropTable(const QString& strTbl)
{
	return excuteSQL(QString("DROP TABLE %1").arg(strTbl));
}
bool CDBProcess::deleteTable(const QString& strTbl)
{
	bool ret = false;
	if ("SQLITE" == __strDbType)
	{
		ret = excuteSQL(QString("DELETE FROM %1;").arg(strTbl));
		ret = excuteSQL(QString("DELETE FROM sqlite_sequence WHERE name = '%1';").arg(strTbl));
		ret = excuteSQL(QString("VACUUM;"));
	}
	else if ("MYSQL" == __strDbType)
	{
		//selct table_name from INFORMATION_SCHEMA.TABLES where table_name 't5' and TABLE-SCHEMA='test';
		/*SELECT count( * )
		FROM information_schema.TABLES
		WHERE table_name = 'table name'
		AND TABLE_SCHEMA = 'database name'*/
		ret = excuteSQL(QString("truncate table %1;").arg(strTbl));
	}


	return ret;
}
bool CDBProcess::isOpen() const
{
	return __pDB->isOpen();
}

bool CDBProcess::isTableExist(const QString strTable)
{
	bool bRet = false;
	if ("SQLITE" == __strDbType)
	{
		const QString str = QString("select * from sqlite_master where type='table' and name='%1'").arg(strTable);
		void *pRet = openRecordsetBySql(str);
		if (pRet && moveNext(pRet))
		{
			closeRecordset(pRet);
			return true;
		}
		else
		{
			return false;
		}
	}
	else if ("MYSQL" == __strDbType)
	{
		//selct table_name from INFORMATION_SCHEMA.TABLES where table_name 't5' and TABLE-SCHEMA='test';
		/*SELECT count( * )
		FROM information_schema.TABLES
		WHERE table_name = 'table name'
		AND TABLE_SCHEMA = 'database name'*/
	}

	return bRet;
}

void CDBProcess::closeDB()
{
	__pDB->close();
	delete __pDB;
	QSqlDatabase::removeDatabase(__strConnName);
}

void *CDBProcess::openRecordsetBySql(const QString strSql)
{
	//QMutexLocker lock(&__mutexDB);
	__mutexDB.lock();
	if (false == isOpen())
	{
		return nullptr;
	}
	
	QSqlQuery *pQry = new QSqlQuery(*__pDB);
	pQry->clear();
	const bool b = pQry->exec(strSql);
	if (!b)
	{
		QSqlError lastError = pQry->lastError();
		if (lastError.isValid())
		{
			qWarning() << lastError.text();
		}
		delete pQry;
		pQry = nullptr;
	}
	if (pQry)
	{
		__lstQrys << pQry;
	}	
	return pQry;
}
void CDBProcess::closeRecordset(void *p)
{
	QSqlQuery *pQry = static_cast<QSqlQuery *>(p);
	if (pQry)
	{
		__lstQrys.removeOne(pQry);
		pQry->clear();
		delete pQry;
		pQry = nullptr;
	}
	__mutexDB.unlock();
}
bool CDBProcess::recEOF(void *p) const
{
	QSqlQuery *pQry = static_cast<QSqlQuery *>(p);
	if (nullptr == pQry)
		return true;
	return (QSql::AfterLastRow == pQry->at());
}
bool CDBProcess::recBOF(void *p) const
{
	QSqlQuery *pQry = static_cast<QSqlQuery *>(p);
	if (nullptr == pQry)
		return true;
	return (QSql::BeforeFirstRow == pQry->at());
}
bool CDBProcess::moveFirst(void *p) const
{
	if (false == isOpen())
	{
		return false;
	}
	QSqlQuery *pQry = static_cast<QSqlQuery *>(p);
	if (nullptr == pQry)
		return false;
	if (pQry->isActive())
	{
		return pQry->first();
	}
	else
	{
		return false;
	}
}

bool CDBProcess::movePrevious(void *p) const
{
	if (false == isOpen())
	{
		return false;
	}
	QSqlQuery *pQry = static_cast<QSqlQuery *>(p);
	if (nullptr == pQry)
		return false;
	if (pQry->isActive())
	{
		return pQry->previous();
	}
	else
	{
		return false;
	}
}
bool CDBProcess::moveNext(void *p) const
{
	if (false == isOpen())
	{
		return false;
	}
	QSqlQuery *pQry = static_cast<QSqlQuery *>(p);
	if (nullptr == pQry)
		return false;
	if (pQry->isActive())
	{
		return pQry->next();
	}
	else
	{
		return false;
	}
}
bool CDBProcess::moveLast(void *p) const
{
	if (false == isOpen())
	{
		return false;
	}
	QSqlQuery *pQry = static_cast<QSqlQuery *>(p);
	if (nullptr == pQry)
		return false;
	if (pQry->isActive())
	{
		return pQry->last();
	}
	else
	{
		return false;
	}
}
bool CDBProcess::moveTo(int n, void *p) const
{
	if (false == isOpen())
	{
		return false;
	}
	QSqlQuery *pQry = static_cast<QSqlQuery *>(p);
	if (nullptr == pQry)
		return false;
	if (pQry->isActive())
	{
		return pQry->seek(n);
	}
	else
	{
		return false;
	}
}

long CDBProcess::getRecordCount(void *p) const
{
	if (false == isOpen())
	{
		return -1;
	}
	QSqlQuery *pQry = static_cast<QSqlQuery *>(p);
	if (nullptr == pQry)
		return -1;
	if (__pDB->driver()->hasFeature(QSqlDriver::QuerySize))
	{
		return pQry->size();
	}
	else
	{
		int i = pQry->at();
		pQry->last();
		int iRows = pQry->at() + 1;
		pQry->seek(i);
		return iRows;
	}
}
#if 0
//"id%+s|name%-s|age%-d|pret%*d", 15, pch, &i, &iRet
bool CDBProcess::exexProc(const QString strStoreProc, QString str1, QString &str2)
{
	QSqlQuery *pQry = (QSqlQuery *)m_mapQry[-1];
	if (nullptr == pQry)
	{
		m_mapQry.remove(-1);

		pQry = new QSqlQuery(*m_pDB);
		m_mapQry.insert(-1, pQry);
	}
	pQry->clear();
	QString strSql = "exec " + strStoreProc + " ?, ? output";
	pQry->prepare(strSql);
	pQry->bindValue(0, str1);
	pQry->bindValue(1, 0, QSql::Out);
	bool b = pQry->exec();
	//    if(!b)
	//    {
	//        QString str = pQry->lastError().text();
	//    }

	str2 = b ? pQry->boundValue(1).toString() : "";

	return b;
}
bool CDBProcess::execStoreProcOfArgList(int idx, const QString strStoreProc, const char *szFldsInfo, ...)
{
	if (nullptr == szFldsInfo)
	{
		return false;
	}
	QSqlQuery *pQry = (QSqlQuery *)m_mapQry[idx];
	if (nullptr == pQry)
	{
		m_mapQry.remove(idx);

		pQry = new QSqlQuery(*m_pDB);
		m_mapQry.insert(idx, pQry);
	}
	pQry->clear();

	QString strFlds(szFldsInfo);
	QStringList strlst = strFlds.split("|");
	//QString strSql = QString("CALL %1").arg(strStoreProc) ;
	QString strSql = QString("exec %1").arg(strStoreProc);
	for (int i = 0; i < strlst.size(); ++i)
	{
		if (0 == i)
			strSql += " ?";
		else if (strlst.size() - 1 == i)
			strSql += ",?";
		else
			strSql += ",?";
	}
	pQry->prepare(strSql);


	va_list arg_ptr;
	va_start(arg_ptr, szFldsInfo);
	//pQry->prepare(strSql);//("INSERT INTO person (id, forename, surname) VALUES (:id, :forename, :surname)");
	//bool bExed = false;
	for (int i = 0; i < strlst.size(); ++i)
	{
		//-表示输出 +或无表示输入 *表示return  //"id%+s|name%-s|age%-d|pret%*d", 15, pch, &i, &iRet
		QString strFldInfo = strlst.at(i);
		QString strFldName = strFldInfo.mid(0, strFldInfo.indexOf('%'));//get field name
		QChar ch = (strFldInfo.mid(strFldInfo.indexOf('%') + 1, 1)).at(0);
		QSql::ParamTypeFlag eParamTypeFlag = QSql::In;
		switch (ch.toLatin1())
		{
		case L'-':
			eParamTypeFlag = QSql::Out;
			ch = (strFldInfo.mid(strFldInfo.indexOf('%') + 2, 1)).at(0);
			break;
		case '+':
			eParamTypeFlag = QSql::In;
			ch = (strFldInfo.mid(strFldInfo.indexOf('%') + 2, 1)).at(0);
			break;
		default:
			eParamTypeFlag = QSql::In;
		}
		if (QSql::In == eParamTypeFlag)
		{
			switch (ch.toLatin1())
			{
			case 'i':
			{
						//64bit整数
						qint64 k = va_arg(arg_ptr, qint64);
						pQry->bindValue(i, k);
						break;
			}
			case 'd':
			case 'u':
			{
						//整数
						int k = va_arg(arg_ptr, int);
						pQry->bindValue(i, k);
						break;
			}
			case 'f':
			case 'g':
			{
						//小数double类型,无float类型
						double d = va_arg(arg_ptr, double);
						pQry->bindValue(i, d);
						break;
			}
			case 's':
			{
						//文本
						//char *pstr = va_arg(arg_ptr, char*);
						QString *pch = va_arg(arg_ptr, QString *);
						QString strVal = *pch;
						pQry->bindValue(i, strVal);
						break;
			}
			case 'b':
			{
						//二进制
						QByteArray *pData = va_arg(arg_ptr, QByteArray *);
						pQry->bindValue(i, *pData);
						break;
			}
			case 't':
			{
						//时间日期
						QDateTime *pdt = va_arg(arg_ptr, QDateTime *);
						pQry->bindValue(i, *pdt);
						break;
			}
			} //end of switch
		}
		else if (QSql::Out == eParamTypeFlag)
		{
			va_arg(arg_ptr, void *); //shift arg
			pQry->bindValue(i, 0, QSql::Out);
		}
	} //end of for
	va_end(arg_ptr);
	if (false == pQry->exec())
	{
		QSqlError err = pQry->lastError();
		QString str = err.text();
		return false;
	}
	va_start(arg_ptr, szFldsInfo);
	for (int i = 0; i < strlst.size(); ++i)
	{
		//-表示输出 +或无表示输入 *表示return  //"id%+s|name%-s|age%-d|pret%*d", 15, pch, &i, &iRet
		QString strFldInfo = strlst.at(i);
		QString strFldName = strFldInfo.mid(0, strFldInfo.indexOf('%'));//get field name
		QChar ch = (strFldInfo.mid(strFldInfo.indexOf('%') + 1, 1)).at(0);
		QSql::ParamTypeFlag eParamTypeFlag = QSql::In;
		switch (ch.toLatin1())
		{
		case L'-':
			eParamTypeFlag = QSql::Out;
			ch = (strFldInfo.mid(strFldInfo.indexOf('%') + 2, 1)).at(0);
			break;
		case '+':
			eParamTypeFlag = QSql::In;
			ch = (strFldInfo.mid(strFldInfo.indexOf('%') + 2, 1)).at(0);
			break;
		default:
			eParamTypeFlag = QSql::In;
		}
		if (QSql::In == eParamTypeFlag)
		{
			switch (ch.toLatin1())
			{
			case 'i':
			{
						//整数
						va_arg(arg_ptr, qint64);
						//pQry->bindValue(i, k);
						break;
			}
			case 'd':
			case 'u':
			{
						//整数
						va_arg(arg_ptr, int);
						//pQry->bindValue(i, k);
						break;
			}
			case 'f':
			case 'g':
			{
						//小数double类型,无float类型
						va_arg(arg_ptr, double);
						//pQry->bindValue(i, d);
						break;
			}
			case 's'://文本
			case 'b'://二进制
			case 't'://时间日期
			{
						 va_arg(arg_ptr, void *);
						 break;
			}
			} //end of switch
		}
		else if (QSql::Out == eParamTypeFlag)
		{
			//int i = query.boundValue(1).toInt(); // i is 65
			void *pRet = va_arg(arg_ptr, void *); //shift arg
			QVariant var = pQry->boundValue(i);
			switch (ch.toLatin1())
			{
			case 'i':
			{
						//整数
						qint64 *pInt = (qint64 *)pRet;
						*pInt = var.toLongLong();
						break;
			}
			case 'd':
			case 'u':
			{
						//整数
						int *pInt = (int *)pRet;
						*pInt = var.toInt();
						break;
			}
			case 'B':
			{
						unsigned char *puc = (unsigned char *)pRet;
						*puc = (unsigned char)var.toInt();
						break;
			}
			case 'f':
			case 'g':
			{
						//小数double类型,无float类型
						double *pD = (double *)pRet;
						*pD = var.toDouble();
						break;
			}
			case 's':
			{
						//文本
						QString *pstr = (QString *)pRet;
						*pstr = var.toString();
						break;
			}
			case 'b':
			{
						//二进制
						QByteArray *pData = va_arg(arg_ptr, QByteArray *);
						*pData = var.toByteArray();
						break;
			}
			case 't':
			{
						//时间日期
						QDateTime *dt = va_arg(arg_ptr, QDateTime *);
						*dt = var.toDateTime();
						break;
			}
			} //end of switch
		}
	} //end of for

	return true;
}
#endif
//==============================================================

bool CDBProcess::transaction()
{
	if (false == __pDB->driver()->hasFeature(QSqlDriver::Transactions))
	{
		return false;
	}
	__pDB->transaction();
	return true;
}
bool CDBProcess::execsqls(const QStringList& sqls)
{
	for (QString sql : sqls)
	{
		sql = sql.trimmed();
		if (!sql.isEmpty())
		{
			if (!excuteSQL(sql))
			{
				return false;
			}
		}	
	}
	return true;
}
bool CDBProcess::commit()
{
	if (false == __pDB->driver()->hasFeature(QSqlDriver::Transactions))
	{
		return false;
	}
	__pDB->commit();
	return true;
}
bool CDBProcess::rollback()
{
	if (false == __pDB->driver()->hasFeature(QSqlDriver::Transactions))
	{
		return false;
	}
	__pDB->rollback();
	return true;
}
void CDBProcess::openTransAct(bool isopen)
{
	__bisTrans = isopen;
	commitTrans();
}
bool CDBProcess::commitTrans()
{
	try
	{
		QMutexLocker lock(&__mutexDB);
		if (!__bisTrans)
		{
			return false;
		}
		//__mutexTrans.lock();
	
		if (!isOpen())
		{
			return false;
		}
		bool bRet = false;
		bRet = transaction();
		if (!bRet)
		{
			QSqlError lastError = __pDB->lastError();
			if (lastError.isValid())
			{
				qWarning() << lastError.text();
			}
			return bRet;
		}

		for (auto trans : __lstTransQrys)
		{
			QSqlQuery qry(*__pDB);
			qry.prepare(trans.strSql);
			const int iCount = trans.lstFlds.size();
			for (int i = 0; i < iCount; ++i)
			{
				qry.bindValue(":" + trans.lstFlds.at(i), trans.lstVars.at(i));
			}
			bRet = qry.exec();
			if (!bRet)
			{
				rollback();
				QSqlError lastError = qry.lastError();
				if (lastError.isValid())
				{
					qWarning() << lastError.text();
				}
				__lstTransQrys.removeOne(trans);
				//__mutexTrans.unlock();
				return bRet;
			}		
		}
		__lstTransQrys.clear();
		//__mutexTrans.unlock();
		bRet = commit();
		if (!bRet)
		{
			rollback();
			QSqlError lastError = __pDB->lastError();
			if (lastError.isValid())
			{
				qWarning() << lastError.text();
			}
			return bRet;
		}
		

		return bRet;
	}
	catch (...)
	{
		rollback();
		QSqlError lastError = __pDB->lastError();
		if (lastError.isValid())
		{
			qWarning() << lastError.text();
		}
		return false;
	}
	
}
