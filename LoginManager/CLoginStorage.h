#pragma once

#include <QObject>
#include "CPermission.h"
#include "CLogin.h"
#include "libdboperation.h"

#define ShowLoginRecordNum	6//显示登录记录的条数

class  CDBOperation : public LibDBOperation
{
	Q_OBJECT
public:
	Kernel(CDBOperation)

	bool openDB(QString& strIp, int& iPort,
			QString& strDbName, QString& strUserId, QString& strPassword);
	bool loadLogin(QMap<QString, sLogin>& mapLogin);
	bool loadLoginRecord(QMap<QDateTime, QString>& mapLoginRecord, QString& strLastLoginOperator);
	bool Save(const sLoginUser& newLoginUser);
	bool Delete(const sUserId& userId);
	bool saveLoginRecord(const sLogin& login, QString strMemo = "");

private:
	CDBOperation(QObject* parent = nullptr);
	~CDBOperation();
};

class CLoginStorage : public QObject
{
	Q_OBJECT

public:
	//用户登录信息存储方式(可以兼容存储)
	enum nUserLoginInfoStorageMode {
		StorageIn_IniFile = 1,			//保存在ini文件中
		StorageIn_DB = 0b10,			//
		StorageIn_RemoteUrl = 0b100,	//远程url
	};
#define ULISM CLoginStorage::nUserLoginInfoStorageMode 

	CLoginStorage(int iStorageMode = ULISM::StorageIn_IniFile | ULISM::StorageIn_DB, QObject *parent = nullptr);
	~CLoginStorage();

	void initDB();
	bool loadDBPara(QString& strIp, int& iPort,
		QString& strDbName, QString& strUserId, QString& strPassword);
	bool saveDBPara(QString& strIp, int& iPort,
		QString& strDbName, QString& strUserId, QString& strPassword);

	bool load(QMap<QString, sLogin>& mapLogin);
	bool loadLoginRecord(QMap<QDateTime, QString>& mapLoginRecord, QString& strLastLoginOperator);

	bool saveFile(const QMap<QString, sLogin> &mapLogin);
	bool saveLoginRecordFile(const QMap<QString, QDateTime> &mapLoginRecord,
		const QString& strLastLoginOperator);//保存文件时, 因为文件只能一次性写入, 避免写入过多次, 所以写入文件时, 每个用户只记录最后一次登录的事件 

	bool SaveDB(const sLoginUser& newLoginUser);
	bool DeleteDB(const sUserId &userId);
	//补注的内容, 当多个设备登录联动, 且设置了超时自动以操作员权限登录时, 可能会同时更新数据库登陆记录, strMemo此字段作为补充标识
	bool saveLoginRecordDB(const sLogin& login, QString strMemo = "");

	bool isDBOpened() { return m_bDBOpened; }
private:
	bool loadFile(QMap<QString, sLogin>& mapLogin);
	bool loadDB(QMap<QString, sLogin>& mapLogin);

	bool loadLoginRecordFile(QMap<QDateTime, QString>& mapLoginRecord, QString& strLastLoginOperator);
	bool loadLoginRecordDB(QMap<QDateTime, QString>& mapLoginRecord, QString& strLastLoginOperator);

	//用户登录信息json
	QJsonObject toJson(const QMap<QString, sLogin>& mapLogin);
	QMap<QString, sLogin> fromJson(const QJsonObject& json);

private:
	int m_iStorageMode = ULISM::StorageIn_IniFile | ULISM::StorageIn_DB;

	bool m_bDBOpened = false;
};
