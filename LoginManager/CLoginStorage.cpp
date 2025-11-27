#include <QDir>
#include <QDebug>
#include <QJsonArray>
#include <QHostAddress>
#include <QJsonDocument>
#include <QCoreApplication>

#include "dbdatadefine.h"
#include "cdbprocess.h"
#include "libdboperation.h"
#include "CLoginStorage.h"

#ifndef TIME_FORMAT_MILLISECOND_STR
#define TIME_FORMAT_MILLISECOND_STR			"yyyy-MM-dd hh:mm:ss.zzz"
#endif

#define CONNECT_NAME_LOGIN "CONNECT_NAME_LOGIN"

//参照TTextFile的函数GetFileContent
QByteArray GetFileContent(const QString& strFilePath)
{
	QFile file(strFilePath);
	QByteArray txt;
	if (file.open(QIODevice::ReadOnly))
		txt = file.readAll();
	file.close();
	return txt;
}

//参照TTextFile的函数SetFileContent
bool SetFileContent(QString strFilePath, QByteArray content)
{
	QFileInfo fileInfo(strFilePath);
	QString strPath = fileInfo.absolutePath();
	QDir dir;
	if (!dir.exists(strPath) &&
		!dir.mkpath(strPath))
		return false;

	QFile file(strFilePath);
	if (!file.open(QIODevice::Truncate | QIODevice::WriteOnly))
		return false;
	file.write(content);
	return true;
}

CLoginStorage::CLoginStorage(int iStorageMode, QObject *parent)
	: QObject(parent), m_iStorageMode(iStorageMode)
{
	if (m_iStorageMode & ULISM::StorageIn_DB)
	{
		initDB();
	}
}

CLoginStorage::~CLoginStorage()
{
}

void CLoginStorage::initDB()
{
	if (m_iStorageMode & ULISM::StorageIn_DB)
	{
		QStringList lstDrivers = CDBProcess::getDBDrivers();
		QString strIp, strDbName, strUserId, strPassword;
		int iPort;
		bool ret = loadDBPara(strIp, iPort, strDbName, strUserId, strPassword);
		if (ret)
		{
			if (strIp == "localhost")
				strIp = "127.0.0.1";
			QHostAddress hostAddress;
			hostAddress.setAddress(strIp);
			if (hostAddress.protocol() != QAbstractSocket::IPv4Protocol)
				return;
			if (iPort > 65535 || iPort < 0)
				return;
			if (strDbName.isEmpty() ||
				strUserId.isEmpty() ||
				strPassword.isEmpty())
				return;

			m_bDBOpened = CDBOperation::GetKernel()->openDB(strIp, iPort, strDbName, strUserId, strPassword);
		}
	}
}

#define LOGIN_DB_IP		"IP"
#define	LOGIN_DB_PORT	"Port"
#define LOGIN_DB_NAME	"Name"
#define LOGIN_DB_USER	"User"
#define LOGIN_DB_PW		"Password"
bool CLoginStorage::loadDBPara(QString& strIp, int& iPort,
	QString& strDbName, QString& strUserId, QString& strPassword)
{
	bool ret(false);

	QString strPath = QCoreApplication::applicationDirPath() + "/config/LoginPara.ini";
	if (!QFile::exists(strPath))
	{
		return false;
	}

	QByteArray byte = GetFileContent(strPath);
	byte = QByteArray::fromHex(byte);
	QJsonParseError jsonError;
	QJsonDocument jsonDoc = QJsonDocument::fromJson(byte, &jsonError);
	if (jsonError.error != QJsonParseError::NoError)
	{
		return false;
	}

	QJsonObject json = jsonDoc.object();
	strIp = json.value(LOGIN_DB_IP).toString("");
	iPort = json.value(LOGIN_DB_PORT).toInt();
	strDbName = json.value(LOGIN_DB_NAME).toString("");
	strUserId = json.value(LOGIN_DB_USER).toString("");
	strPassword = json.value(LOGIN_DB_PW).toString("");

	return true;
}

bool CLoginStorage::saveDBPara(QString& strIp, int& iPort,
	QString& strDbName, QString& strUserId, QString& strPassword)
{
	QJsonObject json;
	json.insert(LOGIN_DB_IP, strIp);
	json.insert(LOGIN_DB_PORT, iPort);
	json.insert(LOGIN_DB_NAME, strDbName);
	json.insert(LOGIN_DB_USER, strUserId);
	json.insert(LOGIN_DB_PW, strPassword);

	QJsonDocument jsonDoc;
	jsonDoc.setObject(json);
	QByteArray byte = jsonDoc.toJson(QJsonDocument::JsonFormat::Indented);
	QString strPath = QCoreApplication::applicationDirPath() + "/config";
	if (!QFile::exists(strPath))
	{
		QDir dir;
		dir.mkpath(strPath);
	}
	strPath += "/LoginPara.ini";
	bool ret = SetFileContent(strPath, byte.toHex());
	return ret;
}

bool CLoginStorage::load(QMap<QString, sLogin>& mapLogin)
{
	if ((m_iStorageMode & ULISM::StorageIn_DB) && m_bDBOpened)
	{
		bool ret = loadDB(mapLogin);
		if (ret)
			return ret;
	}
	if (m_iStorageMode & ULISM::StorageIn_IniFile)
	{
		bool ret = loadFile(mapLogin);
		if (ret)
			return ret;
	}
	return false;
}

bool CLoginStorage::loadLoginRecord(QMap<QDateTime, QString>& mapLoginRecord, QString& strLastLoginOperator)
{
	if (m_iStorageMode & ULISM::StorageIn_DB && m_bDBOpened)
	{
		bool ret = loadLoginRecordDB(mapLoginRecord, strLastLoginOperator);
		if (ret)
			return ret;
	}
	if (m_iStorageMode & ULISM::StorageIn_IniFile)
	{
		bool ret = loadLoginRecordFile(mapLoginRecord, strLastLoginOperator);
		if (ret)
			return ret;
	}
	return false;
}

bool CLoginStorage::saveFile(const QMap<QString, sLogin> &mapLogin)
{
	if (m_iStorageMode & ULISM::StorageIn_IniFile)
	{
		bool ret(false);
		QJsonObject json = toJson(mapLogin);
		if (json.isEmpty())
		{
			qDebug() << QString("Login user is empty.");// QString::fromUtf8("登录用户信息为空.");
			return false;
		}
		QJsonDocument jsonDoc;
		jsonDoc.setObject(json);
		QByteArray byte = jsonDoc.toJson(QJsonDocument::JsonFormat::Indented);
		QString strPath = QCoreApplication::applicationDirPath() + "/config";
		if (!QFile::exists(strPath))
		{
			QDir dir;
			dir.mkpath(strPath);
		}
		strPath += "/USER.ini";
		ret = SetFileContent(strPath, byte.toHex());
		return ret;
	}
	return true;
}

bool CLoginStorage::saveLoginRecordFile(const QMap<QString, QDateTime> &mapLoginRecord,
	const QString& strLastLoginOperator)
{
	if (m_iStorageMode & ULISM::StorageIn_IniFile)
	{
		QJsonObject json;
		QJsonArray jsonArray;
		for (auto it = mapLoginRecord.cbegin(); it != mapLoginRecord.cend(); ++it)
		{
			QJsonObject jsonObject;
			jsonObject.insert(LOGIN_LOGINUSER, it.key());
			jsonObject.insert(LOGIN_LOGINTIME, it.value().toString(TIME_FORMAT_MILLISECOND_STR));
			jsonArray.append(jsonObject);
		}
		json.insert("LoginRecord", jsonArray);
		json.insert("LastLoginOperator", strLastLoginOperator);

		QJsonDocument jsonDoc;
		jsonDoc.setObject(json);
		QByteArray byte = jsonDoc.toJson(QJsonDocument::JsonFormat::Indented);
		QString strPath = QCoreApplication::applicationDirPath() + "/config";
		if (!QFile::exists(strPath))
		{
			QDir dir;
			dir.mkpath(strPath);
		}
		strPath += "/LoginRecord.ini";
		bool ret = SetFileContent(strPath, byte.toHex());

		return ret;
	}
	return true;
}

bool CLoginStorage::SaveDB(const sLoginUser& newLoginUser)
{
	if ((m_iStorageMode & ULISM::StorageIn_DB) && m_bDBOpened)
	{
		return CDBOperation::GetKernel()->Save(newLoginUser);
	}
	return false;
}

bool CLoginStorage::DeleteDB(const sUserId& userId)
{
	if ((m_iStorageMode & ULISM::StorageIn_DB) && m_bDBOpened)
	{
		if (userId.iUserIdType == sUserId::nUserIdType::nUserIdType_UserAutoId)
			return false; //自动用户id不可以删除

		return CDBOperation::GetKernel()->Delete(userId);
	}
	return false;
}

bool CLoginStorage::saveLoginRecordDB(const sLogin& login, QString strMemo/* = ""*/)
{
	if ((m_iStorageMode & ULISM::StorageIn_DB) && m_bDBOpened)
	{
		return CDBOperation::GetKernel()->saveLoginRecord(login);
	}
	return false;
}

bool CLoginStorage::loadFile(QMap<QString, sLogin>& mapLogin)
{
	if (m_iStorageMode & ULISM::StorageIn_IniFile)
	{
		bool ret(false);

		QString strPath = QCoreApplication::applicationDirPath() + "/config/USER.ini";
		if (!QFile::exists(strPath))
		{
			return false;
		}

		QByteArray byte = GetFileContent(strPath);
		byte = QByteArray::fromHex(byte);
		QJsonParseError jsonError;
		QJsonDocument jsonDoc = QJsonDocument::fromJson(byte, &jsonError);
		if (jsonError.error != QJsonParseError::NoError)
		{
			return false;
		}

		QJsonObject json = jsonDoc.object();
		mapLogin = fromJson(json);
		return !mapLogin.isEmpty();
	}
	return false;
}

bool CLoginStorage::loadDB(QMap<QString, sLogin>& mapLogin)
{
	if ((m_iStorageMode & ULISM::StorageIn_DB) && m_bDBOpened)
	{
		return CDBOperation::GetKernel()->loadLogin(mapLogin);
	}
	return false;
}

bool CLoginStorage::loadLoginRecordFile(QMap<QDateTime, QString>& mapLoginRecord,
	QString& strLastLoginOperator)
{
	if (m_iStorageMode & ULISM::StorageIn_IniFile)
	{
		QString strPath = QCoreApplication::applicationDirPath() + "/config/LoginRecord.ini";
		if (!QFile::exists(strPath))
			return false;

		QByteArray byte = GetFileContent(strPath);
		byte = QByteArray::fromHex(byte);
		QJsonParseError jsonError;
		QJsonDocument jsonDoc = QJsonDocument::fromJson(byte, &jsonError);
		if (jsonError.error != QJsonParseError::NoError)
			return false;

		QJsonObject json = jsonDoc.object();
		QJsonArray jsonArray = json.value("LoginRecord").toArray();
		mapLoginRecord.clear();
		for (int index = 0; index < jsonArray.count(); ++index)
		{
			QJsonObject jsonObject = jsonArray.at(index).toObject();
			QString strAutoUserId = jsonObject.value(LOGIN_LOGINUSER).toString();
			QString strLoginTime = jsonObject.value(LOGIN_LOGINTIME).toString();
			QDateTime loginTime = QDateTime::fromString(strLoginTime, TIME_FORMAT_MILLISECOND_STR);
			mapLoginRecord.insert(loginTime, strAutoUserId);
		}
		strLastLoginOperator = json.value("LastLoginOperator").toString();
		return !mapLoginRecord.isEmpty();
	}

	return false;
}

bool CLoginStorage::loadLoginRecordDB(QMap<QDateTime, QString>& mapLoginRecord, QString& strLastLoginOperator)
{
	if ((m_iStorageMode & ULISM::StorageIn_DB) && m_bDBOpened)
	{
		return CDBOperation::GetKernel()->loadLoginRecord(mapLoginRecord, strLastLoginOperator);
	}
	return false;
}

QJsonObject CLoginStorage::toJson(const QMap<QString, sLogin>& mapLogin)
{
	QJsonObject json;
	QJsonArray loginUserArray;
	for (auto login : mapLogin)
	{
		if (!login.loginUser.isValid())
		{
			qDebug() << QString("Not valid login user: %1, %2")//QString::fromUtf8("无效的登录用户: %1, %2")
				.arg(login.loginUser.getUserMainName())
				.arg(login.loginUser.getLastError());
			return json;
		}

		QJsonObject loginUserJson = sLoginUser::toJson(login.loginUser);
		loginUserArray.append(loginUserJson);
	}
	json.insert("Login", loginUserArray);
	return json;
}

QMap<QString, sLogin> CLoginStorage::fromJson(const QJsonObject& json)
{
	QMap<QString, sLogin> mapLogin;
	if (!json.contains("Login"))
		return mapLogin;

	bool ret(true);
	QJsonArray loginUserArray = json.value("Login").toArray();
	for (int index = 0; index < loginUserArray.count(); ++index)
	{
		QJsonObject json = loginUserArray.at(index).toObject();
		bool isOk(false);
		sLoginUser loginUser = sLoginUser::fromJson(json, &isOk);
		if (isOk)
		{
			QString strAutoUserId = loginUser.getAutoUserId();
			sLogin& login = mapLogin[strAutoUserId];
			login.loginUser = loginUser;
		}
		ret &= isOk;
	}

	return mapLogin;
}

bool CDBOperation::openDB(QString& strIp, int& iPort,
	QString& strDbName, QString& strUserId, QString& strPassword)
{
	sDBInfo dbInfo;
	dbInfo.strDbSvrName = strIp;
	dbInfo.iDbPort = iPort;
	dbInfo.strDbAddress = strDbName;
	dbInfo.strDbUserID = strUserId;
	dbInfo.StrDbUserPwd = strPassword;
	dbInfo.strDbType = "MYSQL";
	dbInfo.bDbIsTrans = true;
	dbInfo.strDbName = CONNECT_NAME_LOGIN;
	bool ret = LibDBOperation::openDB(dbInfo);
	return ret;
}

bool CDBOperation::loadLogin(QMap<QString, sLogin>& mapLogin)
{
	mapLogin.clear();
	QMutexLocker lock(&m_mutexDb);
	CDBProcess* ptrdb = getDbProcess(CONNECT_NAME_LOGIN);
	if (!ptrdb)
		return false;

	bool ret(true);
	QString sql = QString("select AutoUserId,\
UserId,\
UserIdType from %1;").arg("userid");
	void* pQry = ptrdb->openRecordsetBySql(sql);
	QMap<QString, sUserId> mapUserId;
	while (ptrdb->moveNext(pQry))
	{
		QString strAutoUserId, strUserId;
		int iUserIdType;
		ret &= ptrdb->readFieldsValueFromRec(pQry,
			"AutoUserId", strAutoUserId,
			"UserId", strUserId,
			"UserIdType", iUserIdType);
		if (iUserIdType > sUserId::nUserIdType::nUserIdType_Invalid &&
			iUserIdType < sUserId::nUserIdType::nUserIdType_End)
		{
			sUserId userId(strUserId, (sUserId::nUserIdType)iUserIdType);
			mapUserId.insertMulti(strAutoUserId, userId);
		}
	}
	ptrdb->closeRecordset(pQry);

	sql = QString("select AutoUserId,\
UserMainName,\
UserGroup,\
Password,\
PasswordTips from %1;").arg("userlist");
	pQry = ptrdb->openRecordsetBySql(sql);
	QList<sLoginUser> lstLoginUser;
	while (ptrdb->moveNext(pQry))
	{
		QString strAutoUserId, strUserMainName, strUserGroup, strPassword, strPasswordTips;
		ret &= ptrdb->readFieldsValueFromRec(pQry,
			"AutoUserId", strAutoUserId,
			"UserMainName", strUserMainName,
			"UserGroup", strUserGroup,
			"Password", strPassword,
			"PasswordTips", strPasswordTips);
		sLoginUser loginUser(strUserMainName, strUserMainName,
			strAutoUserId, strUserGroup, strPassword, strPasswordTips,
			QList<sUserId>() << sUserId(strUserMainName)
			<< sUserId(strAutoUserId, sUserId::nUserIdType::nUserIdType_UserAutoId));
		lstLoginUser.append(loginUser);
	}
	ptrdb->closeRecordset(pQry);

	for (auto & loginUser : lstLoginUser)
	{
		QString strAutoUserId = loginUser.getAutoUserId();
		if (mapUserId.contains(strAutoUserId))
		{
			const QList<sUserId> &lstUserId = mapUserId.values(strAutoUserId);
			for (auto & userId : lstUserId)
			{
				if (userId.iUserIdType != sUserId::nUserIdType::nUserIdType_UserMainName &&
					userId.iUserIdType != sUserId::nUserIdType::nUserIdType_UserAutoId)
				{
					loginUser.addUserId(userId.strUserId, userId.iUserIdType);
				}
			}
		}
		if (loginUser.isValid())
		{
			sLogin login;
			login.loginUser = loginUser;
			mapLogin.insert(strAutoUserId, login);
		}
	}
	return !mapLogin.isEmpty();
}

bool CDBOperation::loadLoginRecord(QMap<QDateTime, QString>& mapLoginRecord, QString& strLastLoginOperator)
{
	mapLoginRecord.clear();
	QMutexLocker lock(&m_mutexDb);
	CDBProcess* ptrdb = getDbProcess(CONNECT_NAME_LOGIN);
	if (!ptrdb)
		return false;

	QDateTime lastOperatorLoginTime;
	bool ret(true);
	QString sql = QString("select AutoUserId,\
LoginTime,\
UserGroup from %1;").arg("loginrecord");
	sql += " order by Id desc";
	void* pQry = ptrdb->openRecordsetBySql(sql);
	while (ptrdb->moveNext(pQry))
	{
		QString strAutoUserId, strLoginTime, strUserGroup;
		ret &= ptrdb->readFieldsValueFromRec(pQry,
			"AutoUserId", strAutoUserId,
			"LoginTime", strLoginTime,
			"UserGroup", strUserGroup);

		QDateTime loginTime = QDateTime::fromString(strLoginTime, TIME_FORMAT_MILLISECOND_STR);
		int iLevel = getDefaultUserGroupLevel(strUserGroup);
		if (iLevel < DUGL::nUserLevel_Technician &&
			iLevel >= DUGL::nUserLevel_Operator &&
			loginTime > lastOperatorLoginTime)
		{
			strLastLoginOperator = strAutoUserId;
			lastOperatorLoginTime = loginTime;
		}
		
		if (loginTime.isValid())
		{
			mapLoginRecord.insert(loginTime, strAutoUserId);
		}
	}
	ptrdb->closeRecordset(pQry);

	return !mapLoginRecord.isEmpty();
}

bool CDBOperation::Save(const sLoginUser& newLoginUser)
{
	QMutexLocker lock(&m_mutexDb);
	CDBProcess* ptrdb = getDbProcess(CONNECT_NAME_LOGIN);
	if (!ptrdb)
		return false;

	bool ret = ptrdb->addFieldsValueToTbl("userlist",
		"AutoUserId", newLoginUser.getAutoUserId(),
		"UserMainName", newLoginUser.getUserMainName(), 
		"UserGroup", newLoginUser.getUserGroup(), 
		"Password", newLoginUser.getPassword(), 
		"PasswordTips", newLoginUser.getPasswordTips());

	QList<sUserId> lstUserId = newLoginUser.getUserIdList();
	for (auto &userId : lstUserId)
	{
		ret &= ptrdb->addFieldsValueToTbl("userid",
			"AutoUserId", newLoginUser.getAutoUserId(),
			"UserMainName", newLoginUser.getUserMainName(),
			"UserId", userId.strUserId,
			"UserIdType", userId.iUserIdType);
	}

	return ret;
}

bool CDBOperation::Delete(const sUserId& userId)
{
	QMutexLocker lock(&m_mutexDb);
	CDBProcess* ptrdb = getDbProcess(CONNECT_NAME_LOGIN);
	if (!ptrdb)
		return false;

	if (userId.iUserIdType == sUserId::nUserIdType::nUserIdType_UserMainName)
	{
		QString sql = QString("delete from %1 where 1=1").arg("userid");
		sql += " and UserMainName = '" + userId.strUserId + "'";
		sql += ";";
		bool ret = ptrdb->excuteSQL(sql);
		
		sql = QString("delete from %1 where 1=1").arg("userlist");
		sql += " and UserMainName = '" + userId.strUserId + "'";
		sql += ";";
		ret &= ptrdb->excuteSQL(sql);

		return ret;
	}
	else if (userId.iUserIdType != sUserId::nUserIdType::nUserIdType_UserAutoId)
	{
		QString sql = QString("delete from %1 where 1=1").arg("userid");
		sql += " and UserId = '" + userId.strUserId + "'";
		sql += " and UserIdType = '" + QString::number(userId.iUserIdType) + "'";
		sql += ";";
		bool ret = ptrdb->excuteSQL(sql);

		return ret;
	}
	
	return false;
	
}

bool CDBOperation::saveLoginRecord(const sLogin& login, QString strMemo /*= ""*/)
{
	QMutexLocker lock(&m_mutexDb);
	CDBProcess* ptrdb = getDbProcess(CONNECT_NAME_LOGIN);
	if (!ptrdb)
		return false;

	bool ret = ptrdb->addFieldsValueToTbl("loginrecord",
		"AutoUserId", login.loginUser.getAutoUserId(),
		"LoginTime", login.loginTime.toString(TIME_FORMAT_MILLISECOND_STR),
		"LoginUserId", login.strLoginUserId,
		"LoginType", login.iLoginType,
		"UserGroup", login.loginUser.getUserGroup(),
		"Memo", strMemo);
	return ret;
}

CDBOperation::CDBOperation(QObject* parent /*= nullptr*/) : LibDBOperation(parent)
{
	
}

CDBOperation::~CDBOperation()
{
	closeDB(CONNECT_NAME_LOGIN);
}
