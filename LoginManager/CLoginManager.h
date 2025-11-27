#ifndef _CLOGINMANAGER_H_
#define _CLOGINMANAGER_H_
#include <QTimer>
#include <QJsonArray>
#include <QJsonObject>
#include <QApplication>
#include <QJsonDocument>
#include <QMutexLocker>

#include "CLoginStorage.h"



class LOGINMANAGER_EXPORT CLoginManager : public QObject
{
    Q_OBJECT
public:
	

	static CLoginManager* GetKernel();
	static void DestroyKernel();

	QString userGroup(const QString& strUserId);

	//******Taoyuyu***********************   
	// 日期: 2023/08/14
	// 功能: 当前所有的登录用户(m_mapLogin)中是否包含用户信息
	// 返回: bool
	// 参数: const sLoginUser & loginUser
	//************************************
	bool Contains(const sLoginUser& loginUser);

	//******Taoyuyu***********************   
	// 日期: 2023/08/22
	// 功能: 校验用户名和密码是否正确
	// 返回: bool
	// 参数: const QString & strUserId: 
	// 参数: const QString & strPassword: 
	//************************************
	bool Check(const QString& strUserId,
		const QString& strPassword);

	//******Taoyuyu***********************   
	// 日期: 2023/08/22
	// 功能: 保存新用户
	// 返回: bool
	// 参数: const sLoginUser & newLoginUser: 新用户对象
	//************************************
	bool Save(const sLoginUser& newLoginUser);

	/*
	删除用户的某个id
	如果输入的用户id是用户的主id, 那么直接删除整个用户登录信息
	如果输入的用户id是自动用户id, 那么返回失败
	如果输入的用户id是其他用户id, 那么删除这个id
	*/
	bool Delete(const QString& strUserId);
	bool Delete(const sUserId& userId);
	bool DeleteUser(const QString& strUserId);

	/*
	软件登录操作
	strUserId: 登录的用户id
	strLoginVoucher: 登录凭据, 可以是密码, 可以是手机验证码, 具体视登录类型而定
	iLoginType: 登陆类型
	*/
	bool Login(const QString& strUserId,
		const QString& strLoginVoucher,
		sLogin::nLoginType iLoginType = sLogin::nLoginType::nLoginType_Normal);

	//******Taoyuyu***********************   
	// 日期: 2023/08/22
	// 功能: 当前的登录用户
	// 返回: bool
	// 参数: sLogin & login: 
	//************************************
	bool CurrentLoginUser(sLogin& login);

	//以最后一次登录的操作员账号登录
	bool LastOperatorLogin();

	//获取登录记录中的用户名列表(按登录时间逆序排序)
	QList<QString> GetLoginRecordUserNameList(int iCount = ShowLoginRecordNum);

	//技术员及以上权限用户保持活跃
	//比如: 管理员权限登录后, 如果界面一直在操作, 则调用此接口, 保持登录状态
	void Active();

	//用户登录信息json
	QJsonObject toJson();
	bool fromJson(const QJsonObject& json);

	bool SaveFile();
	bool Load();

	QString getLastInfo() { return m_strLastInfo; }
	QString getLastError() { return m_strLastError; }

	//******Taoyuyu***********************   
	// 日期: 2023/08/14
	// 功能: 获取用户id的登录信息
	// 返回: bool
	// 参数: const QString & strUserId: 用户id
	// 参数: sLogin & login: 用户的登录信息
	//************************************
	bool GetLogin(const QString& strUserId, sLogin& login);

	//m_loginStorage的m_iStorageMode & ULISM::StorageIn_DB为真时
	bool loadDBPara(QString& strIp, int& iPort,
		QString& strDbName, QString& strUserId, QString& strPassword);
	bool saveDBPara(QString& strIp, int& iPort,
		QString& strDbName, QString& strUserId, QString& strPassword);

Q_SIGNALS:
	void signalLogin(const QString& strAutoUserId,
		const QString& strShowUser,
		const QString strResult,
		QString strUserGroup = UserGroup_Operator);

	//登录用户列表
	void signalLoginUserList(QJsonObject json);

private:
	CLoginManager();
	~CLoginManager();

	bool LoadLoginRecord();//加载登录日志
	bool SaveLoginRecordFile();
	

	//添加超级用户
	void AddDefaultUser();
	void OnTimer();

private:
	static CLoginManager* m_pInstance;

	CLoginStorage	m_loginStorage;
	

	QString			m_strLastInfo;		//如果调用函数成功, 并想获取一些信息, 询问此对象内容
	QString			m_strLastError;

	QDateTime		m_lastFailTime;		//上次登录失败时间, 登录失败超过三次后, 需要等待60s
	int				m_iLoginFailNum;	//登录失败次数

	QMutex			m_mutexRecord;
	QString			m_strLastLoginOperator;			//最后一次登录的操作员, 软件每次开启时, 都使用此账号直接登录
	QMap<QDateTime, QString>	m_mapLoginRecord;	//登录记录(包含往期记录)

	QMutex						m_mutex;
	QMap<QString, UP>			m_mapUP;			//所有用户组及对应的权限
	QMap<QString, sLogin>		m_mapLogin;			//注册的登录用户 用户列表	QMap<自动用户id, sLoginUser>

	QMutex			m_mutexPlanLogoutTime;
	QDateTime		m_planLogoutTime;	//计划登出时间
	QTimer* m_pTimer; //定时器, 当以技术员及以上权限登录时, 自动开启定时器, 超时自动登出到操作员账号
};


#endif