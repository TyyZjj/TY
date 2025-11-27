#include <QFile>
#include <QDebug>
#include "libdboperation.h"
#include "cdbprocess.h"

LibDBOperation::LibDBOperation(QObject *parent)
:m_pTransThread(nullptr), m_mutexDb(QMutex::RecursionMode::Recursive)
{
	m_pTransThread = new CTransThread(this);
	m_pTransThread->start();
}

LibDBOperation::~LibDBOperation()
{
	if (m_pTransThread)
	{
		m_pTransThread->requestInterruption();
		m_pTransThread->quit();
		m_pTransThread->wait();
	}
	if (m_pTransThread)
	{
		delete m_pTransThread;
		m_pTransThread = nullptr;
	}
	qDeleteAll(m_dbProMap);
	m_dbProMap.clear();


}


//QString         strDbType;
//QString         strDbName;
//QString         strDbAddress;
//QString			strDbSvrName = "";
//int				iDbPort = 0;
//QString			strDbUserID = "";
//QString			StrDbUserPwd = "";
//bool			bDbIsTrans = false;
bool LibDBOperation::openDB(const sDBInfo& info)
{
	bool ret = true;
	//QMutexLocker lock(&m_mutexDb);
	auto itmap = m_dbProMap.find(info.strDbName);
	if (itmap == m_dbProMap.end())
	{
		if (!info.strDbName.isEmpty()
			&& !info.strDbType.isEmpty())
		{
			//创建数据库操作
			CDBProcess * dbp = new CDBProcess(info.strDbType);
			//打开数据库
			if (dbp->openDB(info.strDbAddress, info.strDbSvrName, info.iDbPort, info.strDbUserID, info.StrDbUserPwd))
			{
				m_dbProMap.insert(info.strDbName, dbp);
				//打开事务模式
				dbp->openTransAct(info.bDbIsTrans);
				//事务线程
				if (info.bDbIsTrans)
				{
					m_pTransThread->addTransDb(dbp);
				}
			
			}
			else
			{
				ret = false;
			}
		}
		
	}
	return ret;
}
bool LibDBOperation::closeDB(const QString& name)
{
	bool ret = true;
	//QMutexLocker lock(&m_mutexDb);
	auto itmap = m_dbProMap.find(name);
	if (itmap != m_dbProMap.end())
	{
		delete itmap.value();
		itmap.value() = nullptr;
		m_dbProMap.erase(itmap);
	}
	return ret;
}
bool LibDBOperation::isDBOpen(const QString& name)
{
	//QMutexLocker lock(&m_mutexDb);
	CDBProcess * ptrdb = getDbProcess(name);
	return ptrdb;
}
bool LibDBOperation::initDB(const QString& dbname, const QString& path)
{
	try
	{
		//读取数据库生成文件
		if (!QFile(path).exists())
		{
			return false;
		}
		QString sql = GetFileContent(path);
		if (sql.isEmpty())
		{
			return false;
		}
		//QMutexLocker lock(&m_mutexDb);
		auto pro = getDbProcess(dbname);
		if (!pro)
		{
			return false;
		}
		//手动开启事务
		if (pro->transaction())
		{
			//执行sql
			if (!pro->execsqls(sql.split(";")))
			{
				//失败回滚
				pro->rollback();
				return false;
			}
			//提交
			pro->commit();
		}
		else
		{
			return false;
		}
		return true;
	}
	catch (...)
	{
		getDbProcess(dbname)->rollback();
		return false;
	}
}
//bool LibDBOperation::commitTrans()
//{
//	//如果开始事务模式，需要循环调用此函数
//	bool ret = true;	
//	QMutexLocker lock(&m_mutexDb);
//	for (auto itmap : m_dbProMap)
//	{
//		ret = itmap->commitTrans();
//	}	
//	return ret;
//}
CDBProcess * LibDBOperation::getDbProcess(const QString& dbname)
{
	QMutexLocker lock(&m_mutexDb);
	auto itmap = m_dbProMap.constFind(dbname);
	if (itmap != m_dbProMap.constEnd())
	{
		return itmap.value();
	}
	else
	{
		qWarning() << "DBProcess is not valid! dbname: " + dbname;
	}
	return nullptr;
};



//
//bool LibDBOperation::getSetting(const QString& key, QByteArray& data, QString& info/* = QString()*/, QDateTime& time/* = QDateTime()*/)
//{
//
//	bool ret = false;
//	auto db = getDbProcess(DBCONFIG);
//	if (!db)
//	{
//		return false;
//	}
//	//只根据ID查询
//	QString sql = QString("select Data,Info,EditTime from %1 where Setid = '%2'").arg(SETTINGDBNAME).arg(key);
//	void * pQry = db->openRecordsetBySql(sql);
//	while (db->moveNext(pQry))
//	{
//		qint64 tmp;
//		ret = db->readFieldsValueFromRec(pQry,
//			"Data", data,
//			"Info", info,
//			"EditTime", tmp);
//		time = QDateTime::fromMSecsSinceEpoch(tmp);
//	}
//	db->closeRecordset(pQry);
//	return ret;
//}
//bool LibDBOperation::setSetting(const QString& key, const QByteArray& data, const QString& info/* = QString()*/)
//{
//	auto db = getDbProcess(DBCONFIG);
//	if (!db)
//	{
//		return false;
//	}
//	//插入图片数据
//	return db->addFieldsValueToTbl(SETTINGDBNAME,
//		"Setid", key,
//		"Data", data,
//		"Info", info,
//		"EditTime", QDateTime::currentDateTime().toMSecsSinceEpoch());					//修改时间，为自动添加时间戳作为记录时间
//}
//
//bool LibDBOperation::getCustomLogo(QPixmap& logo)
//{
//	bool ret = false;
//	QString key = "logo";
//	QByteArray data;
//	//查询logo
//	ret = getSetting(key, data);
//	if (ret)
//	{
//		LibPublic::byteToTemp(data, logo);
//	}
//	return ret;
//}
//bool LibDBOperation::setCustomLogo(const QPixmap& logo)
//{
//	bool ret = false;
//	QString key = "logo";
//	QByteArray data;
//	if (LibPublic::tempTobyte(logo, data))
//	{
//		ret = setSetting(key, data);
//	}
//	return ret;
//}
//bool LibDBOperation::getCustomInitImg(QPixmap& img)
//{
//	bool ret = false;
//	QString key = "initimg";
//	QByteArray data;
//	//查询logo
//	ret = getSetting(key, data);
//	if (ret)
//	{
//		LibPublic::byteToTemp(data, img);
//	}
//	return ret;
//}
//bool LibDBOperation::setCustomInitImg(const QPixmap& img)
//{
//	bool ret = false;
//	QString key = "initimg";
//	QByteArray data;
//	if (LibPublic::tempTobyte(img, data))
//	{
//		ret = setSetting(key, data);
//	}
//	return ret;
//}
//bool LibDBOperation::getPeriod(int& period)
//{
//	bool ret = false;
//	QString key = "period";
//	QByteArray data;
//	//查询logo
//	ret = getSetting(key, data);
//	if (ret)
//	{
//		LibPublic::byteToTemp(data, period);
//	}
//	return ret;
//}
//bool LibDBOperation::setPeriod(int period)
//{
//	bool ret = false;
//	QString key = "period";
//	QByteArray data;
//	if (LibPublic::tempTobyte(period, data))
//	{
//		ret = setSetting(key, data);
//	}
//	return ret;
//}
//bool LibDBOperation::getRights(int& rights)
//{
//	bool ret = false;
//	QString key = "rights";
//	QByteArray data;
//	//查询logo
//	ret = getSetting(key, data);
//	if (ret)
//	{
//		LibPublic::byteToTemp(data, rights);
//	}
//	return ret;
//}
//
//bool LibDBOperation::setRights(int rights)
//{
//	bool ret = false;
//	QString key = "rights";
//	QByteArray data;
//	if (LibPublic::tempTobyte(rights, data))
//	{
//		ret = setSetting(key, data);
//	}
//	return ret;
//}
//
//bool LibDBOperation::login(const sLoginInfo& info)
//{
//	auto db = getDbProcess(DBCONFIG);
//	if (!db)
//	{
//		return false;
//	}
//	QString loginId = QString("login%1_%2").arg(QDateTime::currentDateTime().toMSecsSinceEpoch()).arg(LibPublic::getRandom());
//	//插入
//	return db->addFieldsValueToTbl(LOGINDBNAME,
//		"loginId", loginId,
//		"ProdName", info.strProdName,
//		"ProdLine", info.strProdLine,
//		"LotNo", info.strLotNo,
//		"Model", info.strModel,
//		"UserName", info.strUserName,
//		"LoginTime", info.dLoginTime.toMSecsSinceEpoch(),
//		"LogoutTime", info.dLogoutTime.toMSecsSinceEpoch());
//
//}
//
//bool LibDBOperation::logout(const sLoginInfo& info)
//{
//	auto db = getDbProcess(DBCONFIG);
//	if (!db)
//	{
//		return false;
//	}
//	QString strwhere = QString("WHERE ProdName='%1' and ProdLine='%2' and LotNo='%3' and Model='%4' and UserName='%5'")
//		.arg(info.strProdName)
//		.arg(info.strProdLine)
//		.arg(info.strLotNo)
//		.arg(info.strModel)
//		.arg(info.strUserName);
//	//更新
//	return db->updateTblFieldsValue(LOGINDBNAME,
//		strwhere,
//		"LoginTime", info.dLoginTime.toMSecsSinceEpoch(),
//		"LogoutTime", info.dLogoutTime.toMSecsSinceEpoch());
//}
//
//bool LibDBOperation::loadLastLogin(sLoginInfo& info)
//{
//	bool ret = false;
//	auto db = getDbProcess(DBCONFIG);
//	if (!db)
//	{
//		return false;
//	}
//	QString sql = QString("select ProdName,ProdLine,LotNo,Model,UserName,LoginTime,LogoutTime from sLoginInfo order by LoginTime desc limit 1");
//	void * pQry = db->openRecordsetBySql(sql);
//	while (db->moveNext(pQry))
//	{
//		qint64 logintime;
//		qint64 logouttime;
//		ret = db->readFieldsValueFromRec(pQry,
//			"ProdName", info.strProdName,
//			"ProdLine", info.strProdLine,
//			"LotNo", info.strLotNo,
//			"Model", info.strModel,
//			"UserName", info.strUserName,
//			"LoginTime", logintime,
//			"LogoutTime", logouttime);
//		info.dLoginTime = QDateTime::fromMSecsSinceEpoch(logintime);
//		info.dLogoutTime = QDateTime::fromMSecsSinceEpoch(logouttime);
//	}
//	db->closeRecordset(pQry);
//	return ret;
//}
//
//bool LibDBOperation::loadEnableDevice(QList<sDeviceInfo>& lst)
//{
//	bool ret = false;
//	auto db = getDbProcess(DBCONFIG);
//	if (!db)
//	{
//		return false;
//	}
//	QString sql = QString("select Name,DevcType,Active,Params,Status from %1 where 1=1").arg(DEVICEINFONAME);
//	sql += " and Status = " + QString::number(DeviceStatus_Enable);
//	sql += " limit 1000";
//	void * pQry = db->openRecordsetBySql(sql);
//	while (db->moveNext(pQry))
//	{
//		//QString			strDevcName = QString();
//		//int				iDevcType = DeviceType_Invalid;					//种类	
//		//QList<QVariant>	lstParams = QList<QVariant>();					//参数列表 [0] strDevcName 
//		//int				iStatus = -1;									//状态
//		//int				iIndex = -1;
//		//int				iActive = 1;										//是否激活 0 未激活 1 激活
//		sDeviceInfo info;
//		qint64 time;
//		ret = db->readFieldsValueFromRec(pQry,
//			"Name", info.strDevcName,
//			"DevcType", info.iDevcType,
//			"Active", info.iActive,
//			"Params", info.lstParams,
//			"Status", info.iStatus);
//		lst.append(info);
//	}
//	db->closeRecordset(pQry);
//	return ret;
//}
//
//bool LibDBOperation::saveEnableDevice(const QList<sDeviceInfo>& lst)
//{
//	bool ret = false;
//	auto db = getDbProcess(DBCONFIG);
//	if (!db)
//	{
//		return false;
//	}
//	//先将所有状态改为不启用
//	ret = db->updateTblFieldsValue(DEVICEINFONAME,
//		"",
//		"Status", DeviceStatus_UnEnable);
//	if (ret)
//	{
//		for (auto info : lst)
//		{
//			db->addFieldsValueToTbl(DEVICEINFONAME,
//				"Name", info.strDevcName,
//				"DevcType", info.iDevcType,
//				"Active", info.iActive,
//				"Params", info.lstParams,
//				"Status", DeviceStatus_Enable,
//				"EditTime", QDateTime::currentDateTime().toMSecsSinceEpoch());
//		}
//	}
//	//删除未启用设备
//	QString sql = QString("delete from %1 where 1=1").arg(DEVICEINFONAME);
//	sql += " and Status = " + QString::number(DeviceStatus_Enable);
//	ret = db->excuteSQL(sql);
//
//	return ret;
//}
//bool LibDBOperation::addEnableDevice(const QList<sDeviceInfo>& lst)
//{
//	bool ret = false;
//	auto db = getDbProcess(DBCONFIG);
//	if (!db)
//	{
//		return false;
//	}
//	for (auto info : lst)
//	{
//		db->addFieldsValueToTbl(DEVICEINFONAME,
//			"Name", info.strDevcName,
//			"DevcType", info.iDevcType,
//			"Active", info.iActive,
//			"Params", info.lstParams,
//			"Status", DeviceStatus_Enable,
//			"EditTime", QDateTime::currentDateTime().toMSecsSinceEpoch());
//	}
//	return ret;
//}
//bool LibDBOperation::loadDevcCmdDatas(const sDeviceInfo& info, QList<sDeviceCmd>& cmds)
//{
//	if (!info.isValid())
//	{
//		return false;
//	}
//	bool ret = false;
//	auto db = getDbProcess(DBCONFIG);
//	if (!db)
//	{
//		return false;
//	}
//	QString sql = QString("select DevcCmds from %1	where DeviceId in (select DeviceId from %2 where Name = '%3')").arg(DEVCCMDDATANAME).arg(DEVICEINFONAME).arg(info.strDevcName);
//	sql += " order by EditTime desc limit 1";
//	void * pQry = db->openRecordsetBySql(sql);
//	while (db->moveNext(pQry))
//	{
//		sDeviceCmd cmd;
//		QByteArray tmpData;
//		ret = db->readFieldsValueFromRec(pQry,
//			"DevcCmds", tmpData);
//		if (LibPublic::byteToTemp(tmpData, cmd))
//		{
//			cmds.append(cmd);
//		}
//
//	}
//	db->closeRecordset(pQry);
//	return !cmds.isEmpty();
//}
//bool LibDBOperation::saveDevcCmdDatas(const sDeviceInfo& info, const QList<sDeviceCmd>& cmds)
//{
//	if (!info.isValid())
//	{
//		return false;
//	}
//	auto db = getDbProcess(DBCONFIG);
//	if (!db)
//	{
//		return false;
//	}
//	bool ret = false;
//	QString strdeviceid;
//	QString sql = QString("select DeviceId from %1 where Name = '%2'").arg(DEVICEINFONAME).arg(info.strDevcName);
//	sql += " limit 1";
//	void * pQry = db->openRecordsetBySql(sql);
//	while (db->moveNext(pQry))
//	{
//		ret = db->readFieldsValueFromRec(pQry,
//			"Name", strdeviceid);
//	}
//	db->closeRecordset(pQry);
//	if (ret)
//	{
//		QByteArray tmpData;
//		LibPublic::tempTobyte(cmds, tmpData);
//
//		ret = db->addFieldsValueToTbl(DEVCCMDDATANAME,
//			"DeviceId", strdeviceid,
//			"DevcCmds", tmpData,
//			"Status", DeviceStatus_Enable,
//			"EditTime", QDateTime::currentDateTime().toMSecsSinceEpoch());
//	}
//
//
//	return ret;
//}
//
//bool LibDBOperation::loadAllEngineNames(QStringList& names)
//{
//	bool ret = false;
//	auto db = getDbProcess(DBCONFIG);
//	if (!db)
//	{
//		return false;
//	}
//	names.clear();
//	//减少数据量，所以直接进行数据库操作
//	QString sql = QString("select Name from %1 where 1=1").arg(ENGINEINFO);
//	sql += " limit 1000";
//
//	void * pQry = db->openRecordsetBySql(sql);
//	while (db->moveNext(pQry))
//	{
//		QString name;
//		ret = db->readFieldsValueFromRec(pQry,
//			"Name", name);
//		names.append(name);
//	}
//	db->closeRecordset(pQry);
//
//	return names.count() > 0;
//}
//bool LibDBOperation::loadAllEngineNamesAndDBId(QMap<QString, QString>& enginfo)
//{
//	bool ret = false;
//
//	auto db = getDbProcess(DBCONFIG);
//	if (!db)
//	{
//		return false;
//	}
//	enginfo.clear();
//	//减少数据量，所以直接进行数据库操作
//	QString sql = QString("select Name,EngineId from %1 where 1=1").arg(ENGINEINFO);
//	sql += " limit 1000";
//
//	void * pQry = db->openRecordsetBySql(sql);
//	while (db->moveNext(pQry))
//	{
//		QString name;
//		QString engid;
//		ret = db->readFieldsValueFromRec(pQry,
//			"Name", name,
//			"EngineId", engid);
//		enginfo.insert(engid, name);
//	}
//	db->closeRecordset(pQry);
//
//	return enginfo.count() > 0;
//}
//bool LibDBOperation::loadEngineNameByEngineDBId(const QString& id, QString& name)
//{
//	bool ret = false;
//	auto db = getDbProcess(DBCONFIG);
//	if (!db)
//	{
//		return false;
//	}
//	if (id.isEmpty())
//	{
//		return false;
//	}
//	//减少数据量，所以直接进行数据库操作
//	QString sql = QString("select Name from %1 where 1=1").arg(ENGINEINFO);
//	if (!id.isEmpty())
//	{
//		sql += " and EngineId = '" + id + "'";
//	}
//	sql += " limit 1000";
//	void * pQry = db->openRecordsetBySql(sql);
//	while (db->moveNext(pQry))
//	{
//		ret = db->readFieldsValueFromRec(pQry,
//			"Name", name);
//	}
//	db->closeRecordset(pQry);
//
//	return ret;
//}
//bool LibDBOperation::loadEngineDBIdByEngineName(const QString& name, QString& id)
//{
//	bool ret = false;
//	auto db = getDbProcess(DBCONFIG);
//	if (!db)
//	{
//		return false;
//	}
//	if (name.isEmpty())
//	{
//		return false;
//	}
//	//减少数据量，所以直接进行数据库操作
//	QString sql = QString("select EngineId from %1 where 1=1").arg(ENGINEINFO);
//	if (!name.isEmpty())
//	{
//		sql += " and Name = '" + name + "'";
//	}
//	sql += " limit 1000";
//	void * pQry = db->openRecordsetBySql(sql);
//	while (db->moveNext(pQry))
//	{
//		ret = db->readFieldsValueFromRec(pQry,
//			"EngineId", id);
//	}
//	db->closeRecordset(pQry);
//
//	return ret;
//}
//bool LibDBOperation::updateEngCamNodes(const QString& name, const QByteArray& data)
//{
//	bool ret = false;
//	if (name.isEmpty())
//	{
//		return false;
//	}
//	auto db = getDbProcess(DBCONFIG);
//	if (!db)
//	{
//		return false;
//	}
//	db->addFieldsValueToTbl(ENGINEINFO,
//		"Name", name,
//		"EngineData", data,
//		"EditTime", QDateTime::currentDateTime().toMSecsSinceEpoch());
//	return ret;
//}
//bool LibDBOperation::loadEngineInfo(const QString& name, sEngineInfo& engineInfo)
//{
//	bool ret = false;
//	if (name.isEmpty())
//	{
//		return false;
//	}
//	auto db = getDbProcess(DBCONFIG);
//	if (!db)
//	{
//		return false;
//	}
//	QString sql = QString("select EngineData from %1 where 1=1").arg(ENGINEINFO);
//	sql += " and Name = '" + name + "'";
//	sql += " order by EditTime desc limit 1";
//
//	void * pQry = db->openRecordsetBySql(sql);
//	while (db->moveNext(pQry))
//	{
//		QByteArray tmpData;
//		ret = db->readFieldsValueFromRec(pQry,
//			"EngineData", tmpData);
//		LibPublic::byteToTemp(tmpData, engineInfo);
//	}
//	db->closeRecordset(pQry);
//	return engineInfo.isValid();
//}
//bool LibDBOperation::saveEngineInfo(const QString& name, const sEngineInfo& engineInfo)
//{
//	bool ret = false;
//	auto db = getDbProcess(DBCONFIG);
//	if (!db)
//	{
//		return false;
//	}
//	QByteArray tmpData;
//	LibPublic::tempTobyte(engineInfo, tmpData);
//	db->addFieldsValueToTbl(ENGINEINFO,
//		"Name", name,
//		"EngineData", tmpData,
//		"EditTime", QDateTime::currentDateTime().toMSecsSinceEpoch());
//	return ret;
//}
//bool LibDBOperation::loadEventActs(QString& name, sToDoSystemInfo& todoinfo)
//{
//	bool ret = false;
//	if (name.isEmpty())
//	{
//		return false;
//	}
//	auto db = getDbProcess(DBCONFIG);
//	if (!db)
//	{
//		return false;
//	}
//	QString sql = QString("select EventData from %1 where 1=1").arg(EVENTACTIONNAME);
//	sql += " and Name = '" + name + "'";
//	sql += " order by EditTime desc limit 1";
//
//	void * pQry = db->openRecordsetBySql(sql);
//	while (db->moveNext(pQry))
//	{
//		QByteArray tmpData;
//		ret = db->readFieldsValueFromRec(pQry,
//			"EventData", tmpData);
//		if (ret)
//		{
//			ret = LibPublic::byteToTemp(tmpData, todoinfo);
//		}
//	}
//	db->closeRecordset(pQry);
//	return ret;
//}
//bool LibDBOperation::saveEventActs(const QString& name, const sToDoSystemInfo& todoinfo)
//{
//	bool ret = false;
//	auto db = getDbProcess(DBCONFIG);
//	if (!db)
//	{
//		return false;
//	}
//	QByteArray tmpData;
//	LibPublic::tempTobyte(todoinfo, tmpData);
//	db->addFieldsValueToTbl(EVENTACTIONNAME,
//		"Name", name,
//		"EventData", tmpData,
//		"EditTime", QDateTime::currentDateTime().toMSecsSinceEpoch());
//	return ret;
//}
//bool LibDBOperation::loadAllEvtActNames(QStringList& names)
//{
//	bool ret = false;
//
//	auto db = getDbProcess(DBCONFIG);
//	if (!db)
//	{
//		return false;
//	}
//	//减少数据量，所以直接进行数据库操作
//	QString sql = QString("select Name from %1 where 1=1").arg(EVENTACTIONNAME);
//	sql += " limit 1000";
//
//	void * pQry = db->openRecordsetBySql(sql);
//	while (db->moveNext(pQry))
//	{
//		QString name;
//		ret = db->readFieldsValueFromRec(pQry,
//			"Name", name);
//		names.append(name);
//	}
//	db->closeRecordset(pQry);
//
//	return names.count() > 0;
//}
//bool LibDBOperation::loadAlgoFactoryInfo(QString& name, QByteArray& factoryinfo)
//{
//	bool ret = false;
//	if (name.isEmpty())
//	{
//		return false;
//	}
//	auto db = getDbProcess(DBCONFIG);
//	if (!db)
//	{
//		return false;
//	}
//	QString sql = QString("select FactoryData from %1 where 1=1").arg(ALGOFACTORYINFO);
//	sql += " and Name = '" + name + "'";
//	sql += " order by EditTime desc limit 1";
//	void * pQry = db->openRecordsetBySql(sql);
//	while (db->moveNext(pQry))
//	{
//		QByteArray tmpData;
//		ret = db->readFieldsValueFromRec(pQry,
//			"FactoryData", tmpData);
//		if (ret)
//		{
//			ret = LibPublic::byteToTemp(tmpData, factoryinfo);
//		}
//	}
//	db->closeRecordset(pQry);
//	return ret;
//}
//
//bool LibDBOperation::saveAlgoFactoryInfo(const QString& name, const QByteArray& factoryinfo)
//{
//	bool ret = false;
//	auto db = getDbProcess(DBCONFIG);
//	if (!db)
//	{
//		return false;
//	}
//	db->addFieldsValueToTbl(EVENTACTIONNAME,
//		"Name", name,
//		"FactoryData", factoryinfo,
//		"EditTime", QDateTime::currentDateTime().toMSecsSinceEpoch());
//	return ret;
//}
//bool LibDBOperation::loadAllAlgoFactoryInfoNames(QStringList& names)
//{
//	bool ret = false;
//
//	auto db = getDbProcess(DBCONFIG);
//	if (!db)
//	{
//		return false;
//	}
//	names.clear();
//	//减少数据量，所以直接进行数据库操作
//	QString sql = QString("select Name from %1 where 1=1").arg(ALGOFACTORYINFO);
//	sql += " limit 1000";
//
//	void * pQry = db->openRecordsetBySql(sql);
//	while (db->moveNext(pQry))
//	{
//		QString name;
//		ret = db->readFieldsValueFromRec(pQry,
//			"Name", name);
//		names.append(name);
//	}
//	db->closeRecordset(pQry);
//
//	return names.count() > 0;
//}
//bool LibDBOperation::loadFactoryNameByDBId(const QString& id, QString& name)
//{
//	bool ret = false;
//	auto db = getDbProcess(DBCONFIG);
//	if (!db)
//	{
//		return false;
//	}
//	//减少数据量，所以直接进行数据库操作
//	QString sql = QString("select Name from %1 where 1=1").arg(ALGOFACTORYINFO);
//	if (!id.isEmpty())
//	{
//		sql += " and FactoryId = '" + id + "'";
//	}
//	sql += " limit 1000";
//	void * pQry = db->openRecordsetBySql(sql);
//	while (db->moveNext(pQry))
//	{
//		ret = db->readFieldsValueFromRec(pQry,
//			"Name", name);
//	}
//	db->closeRecordset(pQry);
//
//	return ret;
//}
//
//bool LibDBOperation::loadDBIdByFactoryName(const QString& name, QString& id)
//{
//	bool ret = false;
//	auto db = getDbProcess(DBCONFIG);
//	if (!db)
//	{
//		return false;
//	}
//	//减少数据量，所以直接进行数据库操作
//	QString sql = QString("select FactoryId from %1 where 1=1").arg(ALGOFACTORYINFO);
//	if (!name.isEmpty())
//	{
//		sql += " and Name = '" + name + "'";
//	}
//	sql += " limit 1000";
//	void * pQry = db->openRecordsetBySql(sql);
//	while (db->moveNext(pQry))
//	{
//		ret = db->readFieldsValueFromRec(pQry,
//			"FactoryId", id);
//	}
//	db->closeRecordset(pQry);
//
//	return ret;
//}
//bool LibDBOperation::loadCollectResult(const QString& code, const QString& resultIndex, QString& resultType, QString& result, QString& resultdata)
//{
//	bool ret = false;
//	if (code.isEmpty()
//		|| resultIndex.isEmpty())
//	{
//		return false;
//	}
//	auto db = getDbProcess(DBDATA);
//	if (!db)
//	{
//		return false;
//	}
//	QString sql = QString("select ResultType,Result,ResultData from %1 where 1=1").arg(ALGOFACTORYINFO);
//	sql += " and Code = '" + code + "'";
//	sql += " and ResultIndex = '" + resultIndex + "'";
//	sql += " order by EditTime desc limit 1";
//	void * pQry = db->openRecordsetBySql(sql);
//	while (db->moveNext(pQry))
//	{
//		ret = db->readFieldsValueFromRec(pQry,
//			"ResultType", resultType,
//			"Result", result,
//			"ResultData", resultdata);
//	}
//	db->closeRecordset(pQry);
//	return ret;
//}
//bool LibDBOperation::saveCollectResult(const QString& code, const QString& resultIndex, const QString& resultType, const QString& result, const QString& resultdata)
//{
//	auto db = getDbProcess(DBDATA);
//	if (!db)
//	{
//		return false;
//	}
//	//插入图片数据
//	return db->addFieldsValueToTbl(COLLECTRESULTDBNAME,
//		"Code", code,
//		"ResultIndex", resultIndex,
//		"ResultType", resultType,
//		"Result", result,
//		"ResultData", resultdata,
//		"EditTime", QDateTime::currentDateTime().toMSecsSinceEpoch());					//修改时间，为自动添加时间戳作为记录时间
//}
//
//bool LibDBOperation::saveImgeData(const QString& id, const QPixmap& pix)
//{
//	if (id.isEmpty())
//	{
//		return false;
//	}
//	auto db = getDbProcess(DBDATA);
//	if (!db)
//	{
//		return false;
//	}
//	bool ret = false;
//	QImage img = pix.toImage();
//	QByteArray tmpData;
//	LibPublic::tempTobyte(img, tmpData);
//	return db->addFieldsValueToTbl(IMAGEDBNAME,
//		"Data", tmpData,
//		"EditTime", QDateTime::currentDateTime().toMSecsSinceEpoch());
//	return ret;
//}
//
//bool LibDBOperation::saveImgeData(const QString& id, const QImage& img)
//{
//	if (id.isEmpty())
//	{
//		return false;
//	}
//	auto db = getDbProcess(DBDATA);
//	if (!db)
//	{
//		return false;
//	}
//	bool ret = false;
//	QByteArray tmpData;
//	LibPublic::tempTobyte(img, tmpData);
//	return db->addFieldsValueToTbl(IMAGEDBNAME,
//		"Data", tmpData,
//		"EditTime", QDateTime::currentDateTime().toMSecsSinceEpoch());
//	return ret;
//}
//
//bool LibDBOperation::saveImgeData(const QString& id, const QString& path)
//{
//	if (id.isEmpty())
//	{
//		return false;
//	}
//	auto db = getDbProcess(DBDATA);
//	if (!db)
//	{
//		return false;
//	}
//	bool ret = false;
//	QImage img(path);
//	QByteArray tmpData;
//	LibPublic::tempTobyte(img, tmpData);
//	return db->addFieldsValueToTbl(IMAGEDBNAME,
//		"Data", tmpData,
//		"EditTime", QDateTime::currentDateTime().toMSecsSinceEpoch());
//	return ret;
//}
//bool LibDBOperation::saveImgeData(const QString& id, const QByteArray& data)
//{
//	if (id.isEmpty())
//	{
//		return false;
//	}
//	auto db = getDbProcess(DBDATA);
//	if (!db)
//	{
//		return false;
//	}
//	bool ret = false;
//	return db->addFieldsValueToTbl(IMAGEDBNAME,
//		"Data", data,
//		"EditTime", QDateTime::currentDateTime().toMSecsSinceEpoch());
//	return ret;
//}
//bool LibDBOperation::getDataByImgId(const QString& id, QByteArray& data)
//{
//	if (id.isEmpty())
//	{
//		return false;
//	}
//	//只需任意条件不为空，就可查询
//	bool ret = false;
//	auto db = getDbProcess(DBDATA);
//	if (!db)
//	{
//		return false;
//	}
//	QString sql = QString("select Data from %1 where 1=1").arg(IMAGEDBNAME);
//	sql += " and ImgId = '" + id + "'";
//	sql += " order by EditTime desc limit 1";
//	void * pQry = db->openRecordsetBySql(sql);
//	while (db->moveNext(pQry))
//	{
//		ret = db->readFieldsValueFromRec(pQry,
//			"Data", data);
//	}
//	db->closeRecordset(pQry);
//	return ret;
//}
//bool LibDBOperation::getPixmapByImgId(const QString& id, QPixmap& pix)
//{
//	if (id.isEmpty())
//	{
//		return false;
//	}
//	//只需任意条件不为空，就可查询
//	bool ret = false;
//	auto db = getDbProcess(DBDATA);
//	if (!db)
//	{
//		return false;
//	}
//	QString sql = QString("select Data from %1 where 1=1").arg(IMAGEDBNAME);
//	sql += " and ImgId = '" + id + "'";
//	sql += " order by EditTime desc limit 1";
//	void * pQry = db->openRecordsetBySql(sql);
//	while (db->moveNext(pQry))
//	{
//		QByteArray tmpdata;
//		QImage img;
//		ret = db->readFieldsValueFromRec(pQry,
//			"Data", tmpdata);
//		LibPublic::byteToTemp(tmpdata, img);
//		pix.fromImage(img);
//	}
//	db->closeRecordset(pQry);
//	return ret;
//}
//bool LibDBOperation::loadAllCameraUserID(QMap<QPair<QString, int>, QString >& camuseridmap)
//{
//	bool ret = false;
//	auto db = getDbProcess(DBCONFIG);
//	if (!db)
//	{
//		return false;
//	}
//	camuseridmap.clear();
//	//减少数据量，所以直接进行数据库操作
//	QString sql = QString("select Name,CameraId,CameraType from %1 where 1=1").arg(CAMERAINFO);
//	sql += " limit 1000";
//
//	void * pQry = db->openRecordsetBySql(sql);
//	while (db->moveNext(pQry))
//	{
//		QString camearid;
//		QString userid;
//		int camtype;
//		ret = db->readFieldsValueFromRec(pQry,
//			"Name", userid,
//			"CameraType", camtype,
//			"CameraId", camearid);
//		if (ret)
//		{
//			camuseridmap.insert(QPair<QString, int>{camearid, camtype}, userid);
//		}
//	}
//	db->closeRecordset(pQry);
//
//	return camuseridmap.count() > 0;
//}
//bool LibDBOperation::loadAllCamDbIdAndUserId(QMap<QString, QString >& camuseridmap)
//{
//	bool ret = false;
//	auto db = getDbProcess(DBCONFIG);
//	if (!db)
//	{
//		return false;
//	}
//	camuseridmap.clear();
//	//减少数据量，所以直接进行数据库操作
//	QString sql = QString("select CamId,Name from %1 where 1=1").arg(CAMERAINFO);
//	sql += " limit 1000";
//
//	void * pQry = db->openRecordsetBySql(sql);
//	while (db->moveNext(pQry))
//	{
//		QString camId;
//		QString name;
//		ret = db->readFieldsValueFromRec(pQry,
//			"CamId", camId,
//			"Name", name);
//		if (ret)
//		{
//			camuseridmap.insert(camId, name);
//		}
//	}
//	db->closeRecordset(pQry);
//
//	return camuseridmap.count() > 0;
//}
//bool LibDBOperation::loadCameraNameByCameraDBId(const QString& id, QString& name)
//{
//	bool ret = false;
//	auto db = getDbProcess(DBCONFIG);
//	if (!db)
//	{
//		return false;
//	}
//	if (id.isEmpty())
//	{
//		return false;
//	}
//	//减少数据量，所以直接进行数据库操作
//	QString sql = QString("select Name from %1 where 1=1").arg(CAMERAINFO);
//	if (!id.isEmpty())
//	{
//		sql += " and CamId = '" + id + "'";
//	}
//	sql += " limit 1000";
//	void * pQry = db->openRecordsetBySql(sql);
//	while (db->moveNext(pQry))
//	{
//		ret = db->readFieldsValueFromRec(pQry,
//			"Name", name);
//	}
//	db->closeRecordset(pQry);
//
//	return ret;
//}
//bool LibDBOperation::loadCameraDBIdByCameraName(const QString& name, QString& id)
//{
//	bool ret = false;
//	auto db = getDbProcess(DBCONFIG);
//	if (!db)
//	{
//		return false;
//	}
//	if (name.isEmpty())
//	{
//		return false;
//	}
//	//减少数据量，所以直接进行数据库操作
//	QString sql = QString("select CamId from %1 where 1=1").arg(CAMERAINFO);
//	if (!name.isEmpty())
//	{
//		sql += " and Name = '" + name + "'";
//	}
//	sql += " limit 1000";
//	void * pQry = db->openRecordsetBySql(sql);
//	while (db->moveNext(pQry))
//	{
//		ret = db->readFieldsValueFromRec(pQry,
//			"CamId", id);
//	}
//	db->closeRecordset(pQry);
//
//	return ret;
//}
//bool LibDBOperation::loadCameraIDByCameraDBId(const QString& id, QPair<QString, int>& caminfo)
//{
//	bool ret = false;
//	auto db = getDbProcess(DBCONFIG);
//	if (!db)
//	{
//		return false;
//	}
//	if (id.isEmpty())
//	{
//		return false;
//	}
//	//减少数据量，所以直接进行数据库操作
//	QString sql = QString("select CameraId,CameraType from %1 where 1=1").arg(CAMERAINFO);
//	if (!id.isEmpty())
//	{
//		sql += " and CamId = '" + id + "'";
//	}
//	sql += " limit 1000";
//	void * pQry = db->openRecordsetBySql(sql);
//	while (db->moveNext(pQry))
//	{
//		QString camearid;
//		int camtype;
//		ret = db->readFieldsValueFromRec(pQry,
//			"CameraType", camtype,
//			"CameraId", camearid);
//		if (ret)
//		{
//			caminfo.first = camearid;
//			caminfo.second = camtype;
//		}
//	}
//	db->closeRecordset(pQry);
//
//	return ret;
//}
//
//bool LibDBOperation::loadCameraIDByCameraName(const QString& name, QPair<QString, int>& caminfo)
//{
//	bool ret = false;
//	auto db = getDbProcess(DBCONFIG);
//	if (!db)
//	{
//		return false;
//	}
//	if (name.isEmpty())
//	{
//		return false;
//	}
//	//减少数据量，所以直接进行数据库操作
//	QString sql = QString("select CameraId,CameraType from %1 where 1=1").arg(CAMERAINFO);
//	if (!name.isEmpty())
//	{
//		sql += " and Name = '" + name + "'";
//	}
//	sql += " limit 1000";
//	void * pQry = db->openRecordsetBySql(sql);
//	while (db->moveNext(pQry))
//	{
//		QString camearid;
//		int camtype;
//		ret = db->readFieldsValueFromRec(pQry,
//			"CameraType", camtype,
//			"CameraId", camearid);
//		if (ret)
//		{
//			caminfo.first = camearid;
//			caminfo.second = camtype;
//		}
//	}
//	db->closeRecordset(pQry);
//
//	return ret;
//}
//
//bool LibDBOperation::loadCameraDBIdByCameraID(const QPair<QString, int>& caminfo, QString& id)
//{
//	bool ret = false;
//	auto db = getDbProcess(DBCONFIG);
//	if (!db)
//	{
//		return false;
//	}
//	if (caminfo.first.isEmpty())
//	{
//		return false;
//	}
//	//减少数据量，所以直接进行数据库操作
//	QString sql = QString("select CamId from %1 where 1=1").arg(CAMERAINFO);
//	if (!caminfo.first.isEmpty())
//	{
//		sql += " and CameraId = '" + caminfo.first + "'";
//		sql += " and CameraType = " + QString::number(caminfo.second);
//	}
//	sql += " limit 1000";
//	void * pQry = db->openRecordsetBySql(sql);
//	while (db->moveNext(pQry))
//	{
//		ret = db->readFieldsValueFromRec(pQry,
//			"CamId", id);
//	}
//	db->closeRecordset(pQry);
//
//	return ret;
//}
//bool LibDBOperation::loadCameraInfo(const QString& name, QString& camearid, int& cameratype, QByteArray& tmpCameraNode, QByteArray& tmpAdjustFocus)
//{
//	bool ret = false;
//	if (name.isEmpty())
//	{
//		return false;
//	}
//	auto db = getDbProcess(DBCONFIG);
//	if (!db)
//	{
//		return false;
//	}
//	QString sql = QString("select CameraId,CameraType,TmpCameraNode,TmpAdjustFocus from %1 where 1=1").arg(CAMERAINFO);
//	sql += " and Name = '" + name + "'";
//	sql += " order by EditTime desc limit 1";
//
//	void * pQry = db->openRecordsetBySql(sql);
//	while (db->moveNext(pQry))
//	{
//		ret = db->readFieldsValueFromRec(pQry,
//			"CameraId", camearid,
//			"CameraType", cameratype,
//			"TmpCameraNode", tmpCameraNode,
//			"TmpAdjustFocus", tmpAdjustFocus);
//	}
//	db->closeRecordset(pQry);
//	return ret;
//}
//bool LibDBOperation::saveCameraInfo(const QString& name, const QString& camearid, int cameratype, const QByteArray& tmpCameraNode, const QByteArray& tmpAdjustFocus)
//{
//	bool ret = false;
//	if (name.isEmpty()
//		|| camearid.isEmpty())
//	{
//		return false;
//	}
//	auto db = getDbProcess(DBCONFIG);
//	if (!db)
//	{
//		return false;
//	}
//	ret = db->addFieldsValueToTbl(CAMERAINFO,
//		"Name", name,
//		"CameraId", camearid,
//		"CameraType", cameratype,
//		"TmpCameraNode", tmpCameraNode,
//		"TmpAdjustFocus", tmpAdjustFocus);
//	return ret;
//}
//bool LibDBOperation::delCameraInfo(const QString& name, const QString& camearid, int cameratype)
//{
//	bool ret = false;
//	if (name.isEmpty()
//		|| camearid.isEmpty())
//	{
//		return false;
//	}
//	auto db = getDbProcess(DBCONFIG);
//	if (!db)
//	{
//		return false;
//	}
//	QString sql = QString("delete from %1 where 1=1").arg(CAMERAINFO);
//	sql += " and Name = '" + name + "'";
//	sql += " and CameraId = '" + camearid + "'";
//	sql += " and CameraType = '" + QString::number(cameratype) + "'";
//	return db->excuteSQL(sql);
//	return ret;
//}
//bool LibDBOperation::loadResult(const QString& code, const QString& imgId, QString& result, QString& resimgId, QByteArray& resultdata)
//{
//	if (code.isEmpty()
//		|| imgId.isEmpty())
//	{
//		return false;
//	}
//	auto db = getDbProcess(DBCONFIG);
//	if (!db)
//	{
//		return false;
//	}
//	bool ret = false;
//	QString sql = QString("select Result,ResImgId,ResultData from %1 where 1=1").arg(RESULTDBNAME);
//	sql += " and Code = '" + code + "'";
//	sql += " and ImgId = '" + imgId + "'";
//	sql += " order by EditTime desc limit 1";
//	void * pQry = db->openRecordsetBySql(sql);
//	while (db->moveNext(pQry))
//	{
//		ret = db->readFieldsValueFromRec(pQry,
//			"Result", result,
//			"ResImgId", resimgId,
//			"ResultData", resultdata);
//	}
//	db->closeRecordset(pQry);
//	return ret;
//}
//bool LibDBOperation::saveResult(const QString& code, const QString& imgId, const QString& result, const QString& resimgId, const QByteArray& resultdata)
//{
//	bool ret = false;
//	if (code.isEmpty()
//		|| imgId.isEmpty())
//	{
//		return false;
//	}
//	auto db = getDbProcess(DBCONFIG);
//	if (!db)
//	{
//		return false;
//	}
//	ret = db->addFieldsValueToTbl(RESULTDBNAME,
//		"Code", code,
//		"ImgId", imgId,
//		"Result", result,
//		"ResImgId", resimgId,
//		"ResultData", resultdata,
//		"EditTime", QDateTime::currentDateTime().toMSecsSinceEpoch());
//	return ret;
//}











//bool LibDBOperation::loadConfigData(const QString& location, const QString& name, const QString& type, QByteArray& data)
//{
//	if (location.isEmpty()
//		|| name.isEmpty()
//		|| type.isEmpty())
//	{
//		return false;
//	}
//	auto db = getDbProcess(DBCONFIG);
//	if (!db)
//	{
//		return false;
//	}
//	bool ret = false;
//	QString sql = QString("select Data from %1 where 1=1").arg(CONFIGDBNAME);
//	sql += " and Location = '" + location + "'";
//	sql += " and Name = '" + name + "'";
//	sql += " and type = '" + type + "'";
//	sql += " order by EditTime desc limit 1";
//	void * pQry = db->openRecordsetBySql(sql);
//	while (db->moveNext(pQry))
//	{
//		ret = db->readFieldsValueFromRec(pQry,
//			"Data", data);
//	}
//	db->closeRecordset(pQry);
//	return ret;
//}
//bool LibDBOperation::saveConfigData(const QString& location, const QString& name, const QString& type, const QByteArray& data)
//{
//	if (location.isEmpty()
//		|| name.isEmpty()
//		|| type.isEmpty())
//	{
//		return false;
//	}
//	bool ret = false;
//	auto db = getDbProcess(DBCONFIG);
//	if (!db)
//	{
//		return false;
//	}
//	ret = db->addFieldsValueToTbl(CONFIGDBNAME,
//		"Location", location,
//		"Name", name,
//		"type", type,
//		"Data", data);
//	return ret;
//}
//bool LibDBOperation::delConfigData(const QString& location, const QString& name, const QString& type)
//{
//	bool ret = false;
//	if (location.isEmpty()
//		|| name.isEmpty())
//	{
//		return false;
//	}
//	auto db = getDbProcess(DBCONFIG);
//	if (!db)
//	{
//		return false;
//	}
//	QString sql = QString("delete from %1 where 1=1").arg(CONFIGDBNAME);
//	sql += " and Location = '" + location + "'";
//	sql += " and Name = '" + name + "'";
//	sql += " and type = '" + type + "'";
//	return db->excuteSQL(sql);
//}
//bool LibDBOperation::getCustomLogo(const QString& location, QPixmap& logo)
//{
//	if (location.isEmpty())
//	{
//		return false;
//	}
//	bool ret = false;
//	QByteArray tmpdata;
//	ret = loadConfigData(location, "Logo", "QPixmap", tmpdata);
//	if (ret)
//	{
//		LibPublic::byteToTemp(tmpdata, logo);
//	}
//	return ret;
//}
//bool LibDBOperation::setCustomLogo(const QString& location, const QPixmap& logo)
//{
//	if (location.isEmpty())
//	{
//		return false;
//	}
//	bool ret = false;
//	QByteArray tmpdata;
//	LibPublic::tempTobyte(logo, tmpdata);
//	ret = saveConfigData(location, "Logo", "QPixmap", tmpdata);
//	return ret;
//}
//bool LibDBOperation::getCustomInitImg(const QString& location, QPixmap& img)
//{
//	if (location.isEmpty())
//	{
//		return false;
//	}
//	bool ret = false;
//	QByteArray tmpdata;
//	ret = loadConfigData(location, "InitImg", "QPixmap", tmpdata);
//	if (ret)
//	{
//		LibPublic::byteToTemp(tmpdata, img);
//	}
//	return ret;
//}
//bool LibDBOperation::setCustomInitImg(const QString& location, const QPixmap& img)
//{
//	if (location.isEmpty())
//	{
//		return false;
//	}
//	bool ret = false;
//	QByteArray tmpdata;
//	LibPublic::tempTobyte(img, tmpdata);
//	ret = saveConfigData(location, "InitImg", "QPixmap", tmpdata);
//	return ret;
//}
//bool LibDBOperation::getPeriod(const QString& location, int& period)
//{
//	if (location.isEmpty())
//	{
//		return false;
//	}
//	bool ret = false;
//	QByteArray tmpdata;
//	ret = loadConfigData(location, "Period", "int", tmpdata);
//	if (ret)
//	{
//		LibPublic::byteToTemp(tmpdata, period);
//	}
//	return ret;
//}
//bool LibDBOperation::setPeriod(const QString& location, int period)
//{
//	if (location.isEmpty())
//	{
//		return false;
//	}
//	bool ret = false;
//	QByteArray tmpdata;
//	LibPublic::tempTobyte(period, tmpdata);
//	ret = saveConfigData(location, "Period", "int", tmpdata);
//	return ret;
//}
//bool LibDBOperation::getRights(const QString& location, int& rights)
//{
//	if (location.isEmpty())
//	{
//		return false;
//	}
//	bool ret = false;
//	QByteArray tmpdata;
//	ret = loadConfigData(location, "Rights", "int", tmpdata);
//	if (ret)
//	{
//		LibPublic::byteToTemp(tmpdata, rights);
//	}
//	return ret;
//}
//bool LibDBOperation::setRights(const QString& location, int rights)
//{
//	if (location.isEmpty())
//	{
//		return false;
//	}
//	bool ret = false;
//	QByteArray tmpdata;
//	LibPublic::tempTobyte(rights, tmpdata);
//	ret = saveConfigData(location, "Rights", "int", tmpdata);
//	return ret;
//}
//
//
//
//
//
//bool LibDBOperation::loadEngine(const QString& location, const QString& name, sEngineInfo& info)
//{
//	bool ret = false;
//	QByteArray tmpdata;
//	ret = loadConfigData(location, name, "sEngineInfo", tmpdata);
//	if (ret)
//	{
//		LibPublic::byteToTemp(tmpdata, info);
//	}
//	return ret;
//}
//bool LibDBOperation::saveEngine(const QString& location, const QString& name, const sEngineInfo& info)
//{
//	bool ret = false;
//	QByteArray tmpdata;
//	LibPublic::tempTobyte(info, tmpdata);
//	ret = saveConfigData(location, name, "sEngineInfo", tmpdata);
//	return ret;
//}
//bool LibDBOperation::delEngine(const QString& location, const QString& name)
//{
//	bool ret = false;
//	ret = delConfigData(location, name, "sEngineInfo");
//	return ret;
//}
//bool LibDBOperation::loadAllEngineNames(const QString& location, QStringList& names)
//{
//	if (location.isEmpty())
//	{
//		return false;
//	}
//	bool ret = false;
//	auto db = getDbProcess(DBCONFIG);
//	if (!db)
//	{
//		return false;
//	}
//	QString sql = QString("select Name from %1 where 1=1").arg(CONFIGDBNAME);
//	sql += " and Location = '" + location + "'";
//	sql += " and type = 'sEngineInfo'";
//	sql += " order by EditTime desc";
//	void * pQry = db->openRecordsetBySql(sql);
//	while (db->moveNext(pQry))
//	{
//		QString tmpstr;
//		ret = db->readFieldsValueFromRec(pQry,
//			"Name", tmpstr);
//		if (ret)
//		{
//			names.append(tmpstr);
//		}		
//	}
//	db->closeRecordset(pQry);
//	return ret;
//}

//bool LibDBOperation::loadEventActs(const QString& location, const QString& name, sToDoSystemInfo& info)
//{
//	bool ret = false;
//	QByteArray tmpdata;
//	ret = loadConfigData(location, name, "sToDoSystemInfo", tmpdata);
//	if (ret)
//	{
//		LibPublic::byteToTemp(tmpdata, info);
//	}
//	return ret;
//}
//bool LibDBOperation::saveEventActs(const QString& location, const QString& name, const sToDoSystemInfo& info)
//{
//	bool ret = false;
//	QByteArray tmpdata;
//	LibPublic::tempTobyte(info, tmpdata);
//	ret = saveConfigData(location, name, "sToDoSystemInfo", tmpdata);
//	return ret;
//}
//bool LibDBOperation::delEventActs(const QString& location, const QString& name)
//{
//	bool ret = false;
//	ret = delConfigData(location, name, "sToDoSystemInfo");
//	return ret;
//}
//bool LibDBOperation::loadAllEvtActNames(const QString& location, QStringList& names)
//{
//	if (location.isEmpty())
//	{
//		return false;
//	}
//	bool ret = false;
//	auto db = getDbProcess(DBCONFIG);
//	if (!db)
//	{
//		return false;
//	}
//	QString sql = QString("select Name from %1 where 1=1").arg(CONFIGDBNAME);
//	sql += " and Location = '" + location + "'";
//	sql += " and type = 'sToDoSystemInfo'";
//	sql += " order by EditTime desc";
//	void * pQry = db->openRecordsetBySql(sql);
//	while (db->moveNext(pQry))
//	{
//		QString tmpstr;
//		ret = db->readFieldsValueFromRec(pQry,
//			"Name", tmpstr);
//		if (ret)
//		{
//			names.append(tmpstr);
//		}
//	}
//	db->closeRecordset(pQry);
//	return ret;
//}
//bool LibDBOperation::loadEnableEventActs(const QString& location, QString& enable)
//{
//	if (location.isEmpty())
//	{
//		return false;
//	}
//	bool ret = false;
//	//获取启用设备列表
//	QByteArray tmpdata;
//	ret = loadConfigData(location, "EnableEventActs", "QString", tmpdata);
//	if (ret)
//	{
//		LibPublic::byteToTemp(tmpdata, enable);
//	}
//
//	return ret;
//}
//bool LibDBOperation::saveEnableEventActs(const QString& location, const QString& enable)
//{
//	if (location.isEmpty())
//	{
//		return false;
//	}
//	bool ret = false;
//	QByteArray tmpdata;
//	LibPublic::tempTobyte(enable, tmpdata);
//	ret = saveConfigData(location, "EnableEventActs", "QString", tmpdata);
//	return ret;
//}
//bool LibDBOperation::loadAlgoFactory(const QString& location, const QString& name, QByteArray& info)
//{
//	bool ret = false;
//	ret = loadConfigData(location, name, "AlgoFactory", info);
//	return ret;
//}
//bool LibDBOperation::saveAlgoFactory(const QString& location, const QString& name, const QByteArray& info)
//{
//	bool ret = false;
//	ret = saveConfigData(location, name, "AlgoFactory", info);
//	return ret;
//}
//bool LibDBOperation::delAlgoFactory(const QString& location, const QString& name)
//{
//	bool ret = false;
//	ret = delConfigData(location, name, "AlgoFactory");
//	return ret;
//}
//bool LibDBOperation::loadAllAlgoFactoryNames(const QString& location, QStringList& names)
//{
//	if (location.isEmpty())
//	{
//		return false;
//	}
//	bool ret = false;
//	auto db = getDbProcess(DBCONFIG);
//	if (!db)
//	{
//		return false;
//	}
//	QString sql = QString("select Name from %1 where 1=1").arg(CONFIGDBNAME);
//	sql += " and Location = '" + location + "'";
//	sql += " and type = 'AlgoFactory'";
//	sql += " order by EditTime desc";
//	void * pQry = db->openRecordsetBySql(sql);
//	while (db->moveNext(pQry))
//	{
//		QString tmpstr;
//		ret = db->readFieldsValueFromRec(pQry,
//			"Name", tmpstr);
//		if (ret)
//		{
//			names.append(tmpstr);
//		}
//	}
//	db->closeRecordset(pQry);
//	return ret;
//}
//bool LibDBOperation::loadEnableAlgoFactory(const QString& location, QString& enable)
//{
//	if (location.isEmpty())
//	{
//		return false;
//	}
//	bool ret = false;
//	//获取启用设备列表
//	QByteArray tmpdata;
//	ret = loadConfigData(location, "EnableAlgoFactory", "QString", tmpdata);
//	if (ret)
//	{
//		LibPublic::byteToTemp(tmpdata, enable);
//	}
//
//	return ret;
//}
//bool LibDBOperation::saveEnableAlgoFactory(const QString& location, const QString& enable)
//{
//	if (location.isEmpty())
//	{
//		return false;
//	}
//	bool ret = false;
//	QByteArray tmpdata;
//	LibPublic::tempTobyte(enable, tmpdata);
//	ret = saveConfigData(location, "EnableAlgoFactory", "QString", tmpdata);
//	return ret;
//}
//bool LibDBOperation::loadAlgoData(const QString& location, const QString& name, QByteArray& info)
//{
//	bool ret = false;
//	ret = loadConfigData(location, name, "AlgoData", info);
//	return ret;
//}
//bool LibDBOperation::saveAlgoData(const QString& location, const QString& name, const QByteArray& info)
//{
//	bool ret = false;
//	ret = saveConfigData(location, name, "AlgoData", info);
//	return ret;
//}
//bool LibDBOperation::loadCamera(const QString& location, const QString& name, ::sCameraInfo& info)
//{
//	bool ret = false;
//	QByteArray tmpdata;
//	ret = loadConfigData(location, name, "sCameraInfo", tmpdata);
//	if (ret)
//	{
//		LibPublic::byteToTemp(tmpdata, info);
//	}
//	return ret;
//}
//bool LibDBOperation::saveCamera(const QString& location, const QString& name, const ::sCameraInfo& info)
//{
//	bool ret = false;
//	QByteArray tmpdata;
//	LibPublic::tempTobyte(info, tmpdata);
//	ret = saveConfigData(location, name, "sCameraInfo", tmpdata);
//	return ret;
//}
//bool LibDBOperation::delCamera(const QString& location, const QString& name)
//{
//	bool ret = false;
//	ret = delConfigData(location, name, "sCameraInfo");
//	return ret;
//}
//bool LibDBOperation::loadAllCameras(const QString& location, QList<::sCameraInfo>& lstinfo)
//{
//	if (location.isEmpty())
//	{
//		return false;
//	}
//	bool ret = false;
//	auto db = getDbProcess(DBCONFIG);
//	if (!db)
//	{
//		return false;
//	}
//	QString sql = QString("select Data from %1 where 1=1").arg(CONFIGDBNAME);
//	sql += " and Location = '" + location + "'";
//	sql += " and type = 'sCameraInfo'";
//	sql += " order by EditTime desc";
//	void * pQry = db->openRecordsetBySql(sql);
//	while (db->moveNext(pQry))
//	{
//		QByteArray tmpdata;
//		ret = db->readFieldsValueFromRec(pQry,
//			"Data", tmpdata);
//		if (ret)
//		{
//			::sCameraInfo info;
//			LibPublic::byteToTemp(tmpdata, info);
//			lstinfo.append(info);
//		}
//	}
//	db->closeRecordset(pQry);
//	return ret;
//}
//
//bool LibDBOperation::delAllConfig(const QString& location)
//{
//	if (location.isEmpty())
//	{
//		return false;
//	}
//	bool ret = false;
//	auto db = getDbProcess(DBCONFIG);
//	if (!db)
//	{
//		return false;
//	}
//	//ret = db->transaction();
//	//if (!ret)
//	//{
//	//	return false;
//	//}
//	ret = db->deleteTable(CONFIGDBNAME);
//	//if (!ret)
//	//{
//	//	db->rollback();
//	//	return ret;
//	//}
//	//ret = db->commit();
//	//if (!ret)
//	//{
//	//	db->rollback();
//	//	return ret;
//	//}
//	return ret;
//}
//
//
//
//bool LibDBOperation::login(const QString& location, const sLoginInfo& info)
//{
//	if (location.isEmpty())
//	{
//		return false;
//	}
//	bool ret = false;
//	auto db = getDbProcess(DBDATA);
//	if (!db)
//	{
//		return false;
//	}
//	//插入  2019-03-20 12:00:53.182
//	return db->addFieldsValueToTbl(LOGINDBNAME,
//		"Location", location,
//		"ProdName", info.strProdName,
//		"ProdLine", info.strProdLine,
//		"LotNo", info.strLotNo,
//		"Model", info.strModel,
//		"UserName", info.strUserName,
//		"InOrOut", "in");
//}
//bool LibDBOperation::logout(const QString& location, const sLoginInfo& info)
//{
//	if (location.isEmpty())
//	{
//		return false;
//	}
//	bool ret = false;
//	auto db = getDbProcess(DBDATA);
//	if (!db)
//	{
//		return false;
//	}
//	//插入  2019-03-20 12:00:53.182
//	return db->addFieldsValueToTbl(LOGINDBNAME,
//		"Location", location,
//		"ProdName", info.strProdName,
//		"ProdLine", info.strProdLine,
//		"LotNo", info.strLotNo,
//		"Model", info.strModel,
//		"UserName", info.strUserName,
//		"InOrOut", "out");
//}
//bool LibDBOperation::loadLastLogin(const QString& location, sLoginInfo& info)
//{
//	if (location.isEmpty())
//	{
//		return false;
//	}
//	bool ret = false;
//	auto db = getDbProcess(DBDATA);
//	if (!db)
//	{
//		return false;
//	}
//	QString sql = QString("select ProdName,ProdLine,LotNo,Model,UserName from %1 where 1=1").arg(LOGINDBNAME);
//	sql += " and InOrOut = 'in'";
//	sql += " and Location = '" + location + "'";
//	sql += " order by EditTime desc limit 1";
//	void * pQry = db->openRecordsetBySql(sql);
//	while (db->moveNext(pQry))
//	{
//		ret = db->readFieldsValueFromRec(pQry,
//			"ProdName", info.strProdName,
//			"ProdLine", info.strProdLine,
//			"LotNo", info.strLotNo,
//			"Model", info.strModel,
//			"UserName", info.strUserName);
//	}
//	db->closeRecordset(pQry);
//	return ret;
//}
//
//
//bool LibDBOperation::loadImageData(const QString& location, const QString& name, QByteArray& data)
//{
//	if (location.isEmpty())
//	{
//		return false;
//	}
//	bool ret = false;
//	if (name.isEmpty())
//	{
//		return false;
//	}
//	auto db = getDbProcess(DBDATA);
//	if (!db)
//	{
//		return false;
//	}
//	QString sql = QString("select Data from %1 where 1=1").arg(IMAGEDBNAME);
//	sql += " and Location = '" + location + "'";
//	sql += " and Name = '" + name + "'";
//	sql += " order by EditTime desc limit 1";
//	void * pQry = db->openRecordsetBySql(sql);
//	while (db->moveNext(pQry))
//	{
//		ret = db->readFieldsValueFromRec(pQry,
//			"Data", data);
//		
//	}
//	db->closeRecordset(pQry);
//	return ret;
//}
//bool LibDBOperation::loadImageData(const QString& location, const QString& name, QPixmap& pix)
//{
//	if (location.isEmpty())
//	{
//		return false;
//	}
//	bool ret = false;
//	if (name.isEmpty())
//	{
//		return false;
//	}
//	auto db = getDbProcess(DBDATA);
//	if (!db)
//	{
//		return false;
//	}
//	QString sql = QString("select Data from %1 where 1=1").arg(IMAGEDBNAME);
//	sql += " and Location = '" + location + "'";
//	sql += " and Name = '" + name + "'";
//	sql += " order by EditTime desc limit 1";
//	void * pQry = db->openRecordsetBySql(sql);
//	while (db->moveNext(pQry))
//	{
//		QByteArray tmpdata;
//		ret = db->readFieldsValueFromRec(pQry,
//			"Data", tmpdata);
//		if (ret)
//		{
//			LibPublic::byteToTemp(tmpdata, pix);
//		}
//	}
//	db->closeRecordset(pQry);
//	return ret;
//}
//bool LibDBOperation::loadImageData(const QString& location, const QString& name, QImage& img)
//{
//	if (location.isEmpty())
//	{
//		return false;
//	}
//	bool ret = false;
//	if (name.isEmpty())
//	{
//		return false;
//	}
//	auto db = getDbProcess(DBDATA);
//	if (!db)
//	{
//		return false;
//	}
//	QString sql = QString("select Data from %1 where 1=1").arg(IMAGEDBNAME);
//	sql += " and Location = '" + location + "'";
//	sql += " and Name = '" + name + "'";
//	sql += " order by EditTime desc limit 1";
//	void * pQry = db->openRecordsetBySql(sql);
//	while (db->moveNext(pQry))
//	{
//		QByteArray tmpdata;
//		ret = db->readFieldsValueFromRec(pQry,
//			"Data", tmpdata);
//		if (ret)
//		{
//			LibPublic::byteToTemp(tmpdata, img);
//		}
//	}
//	db->closeRecordset(pQry);
//	return ret;
//}
//bool LibDBOperation::saveImageData(const QString& location, const QString& name, const QPixmap& pix)
//{
//	if (location.isEmpty())
//	{
//		return false;
//	}
//	bool ret = false;
//	if (name.isEmpty())
//	{
//		return false;
//	}
//	auto db = getDbProcess(DBDATA);
//	if (!db)
//	{
//		return false;
//	}
//		QByteArray tmpData;
//		LibPublic::tempTobyte(pix, tmpData);
//	//插入图片数据
//		return db->addFieldsValueToTbl(IMAGEDBNAME,
//			"Location", location,
//			"Name", name,
//			"Data", tmpData);
//}
//bool LibDBOperation::saveImageData(const QString& location, const QString& name, const QImage& img)
//{
//	if (location.isEmpty())
//	{
//		return false;
//	}
//	bool ret = false;
//	if (name.isEmpty())
//	{
//		return false;
//	}
//	auto db = getDbProcess(DBDATA);
//	if (!db)
//	{
//		return false;
//	}
//	QByteArray tmpData;
//	LibPublic::tempTobyte(img, tmpData);
//	//插入图片数据
//	return db->addFieldsValueToTbl(IMAGEDBNAME,
//		"Location", location,
//		"Name", name,
//		"Data", tmpData);
//}
//bool LibDBOperation::saveImageData(const QString& location, const QString& name, const QString& path)
//{
//	if (location.isEmpty())
//	{
//		return false;
//	}
//	bool ret = false;
//	if (name.isEmpty())
//	{
//		return false;
//	}
//	auto db = getDbProcess(DBDATA);
//	if (!db)
//	{
//		return false;
//	}
//	QByteArray tmpData;
//	
//	LibPublic::tempTobyte(QImage(path), tmpData);
//	//插入图片数据
//	return db->addFieldsValueToTbl(IMAGEDBNAME,
//		"Location", location,
//		"Name", name,
//		"Data", tmpData);
//}
//bool LibDBOperation::loadResultData(const QString& location, const QString& name, QString& code, QString& result, QByteArray& resultdata, QStringList& srcimgid, QStringList& resimgid)
//{
//	if (location.isEmpty())
//	{
//		return false;
//	}
//	bool ret = false;
//	if (name.isEmpty())
//	{
//		return false;
//	}
//	auto db = getDbProcess(DBDATA);
//	if (!db)
//	{
//		return false;
//	}
//	QString sql = QString("select ResultType,Result,ResultData from %1 where 1=1").arg(RESULTDBNAME);
//	sql += " and Location = '" + location + "'";
//	sql += " and Name = '" + name + "'";
//	sql += " order by EditTime desc limit 1";
//	void * pQry = db->openRecordsetBySql(sql);
//	while (db->moveNext(pQry))
//	{
//		QByteArray tmpdata1;
//		QByteArray tmpdata2;
//		ret = db->readFieldsValueFromRec(pQry,
//			"Code", code,
//			"Result", result,
//			"Resultdata", resultdata,
//			"SrcImgId", tmpdata1,
//			"ResImgId", tmpdata2);
//		if (ret)
//		{
//			LibPublic::byteToTemp(tmpdata1, srcimgid);
//			LibPublic::byteToTemp(tmpdata2, resimgid);
//		}
//	}
//	db->closeRecordset(pQry);
//	return ret;
//}
//bool LibDBOperation::saveResultData(const QString& location, const QString& name, const QString& code, const QString& result, const QByteArray& resultdata, const QStringList& srcimgid, const QStringList& resimgid)
//{
//	if (location.isEmpty())
//	{
//		return false;
//	}
//	bool ret = false;
//	if (name.isEmpty())
//	{
//		return false;
//	}
//	auto db = getDbProcess(DBDATA);
//	if (!db)
//	{
//		return false;
//	}
//	QByteArray tmpdata1;
//	QByteArray tmpdata2;
//	LibPublic::tempTobyte(srcimgid, tmpdata1);
//	LibPublic::tempTobyte(resimgid, tmpdata2);
//	//插入图片数据
//	return db->addFieldsValueToTbl(RESULTDBNAME,
//		"Location", location,
//		"Name", name,
//		"Code", code,
//		"Result", result,
//		"Resultdata", resultdata,
//		"SrcImgId", tmpdata1,
//		"ResImgId", tmpdata2);
//}


CTransThread::CTransThread(QObject *parent)
	: QThread(parent)
{

}

void CTransThread::addTransDb(CDBProcess * pdb)
{
	m_lstTransDb.append(pdb);
}

void CTransThread::run()
{
	msleep(3000);
	while (!isInterruptionRequested())
	{
		for (auto pdb : m_lstTransDb)
		{
			pdb->commitTrans();
		}
		msleep(1000);
	}
};