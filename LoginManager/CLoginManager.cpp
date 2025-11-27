#include <QDebug>
#include "CLogin.h"
#include "CLoginManager.h"

CLoginManager* CLoginManager::m_pInstance = nullptr;

CLoginManager* CLoginManager::GetKernel()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = new CLoginManager;
	}
	return m_pInstance;
}

void CLoginManager::DestroyKernel()
{
	if (m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

QString CLoginManager::userGroup(const QString& strUserId)
{
	sLogin login;
	bool ret = GetLogin(strUserId, login);
	if (ret)
		return login.loginUser.getUserGroup();
	else
		return "";
}

bool CLoginManager::Contains(const sLoginUser& loginUser)
{
	sLoginUser loginUserTemp = loginUser;
	if (!loginUserTemp.isValid())
	{
		m_strLastError = loginUserTemp.getLastError();
		return false;
	}

	QMutexLocker lock(&m_mutex);
	QStringList lstUserIdName = loginUser.getUserIdNameList();
	for (auto& strUserIdName : lstUserIdName)
	{
		for (auto it = m_mapLogin.cbegin(); it != m_mapLogin.cend(); ++it)
		{
			if (it->loginUser.contains(strUserIdName))
				return true;
		}
	}
	return false;
}

bool CLoginManager::Check(const QString& strUserId, const QString& strPassword)
{
	sLogin login;
	bool ret = GetLogin(strUserId, login);
	if (ret)
	{
		if (login.loginUser.getPassword() == strPassword)
		{
			m_strLastInfo = QApplication::translate("LoginManager", "%1 check success, user group:%2.", nullptr)//QString::fromUtf8("%1校验成功, 用户组:%2!")
				.arg(strUserId)
				.arg(login.loginUser.getUserGroup());
			return true;
		}
		else
		{
			m_strLastError = QApplication::translate("LoginManager", "Password error.", nullptr);//QString::fromUtf8("密码错误!");
			QString strPasswordTips = login.loginUser.getPasswordTips();
			if (!strPasswordTips.isEmpty())
				m_strLastError += QApplication::translate("LoginManager", "Password tips:%1.", nullptr).arg(strPasswordTips);//QString::fromUtf8("密码提示:%1.").arg(strPasswordTips);
			return false;
		}
	}
	else
	{
		m_strLastError = QApplication::translate("LoginManager", "User id not exist.", nullptr);// QString::fromUtf8("不存在当前用户!");
		return false;
	}
}

bool CLoginManager::Save(const sLoginUser& newLoginUser)
{
	sLoginUser newLoginUserTemp(newLoginUser);
	if (!newLoginUserTemp.isValid())
	{
		m_strLastError = newLoginUserTemp.getLastError();
		return false;
	}

	bool ret = Contains(newLoginUser);
	if (ret)
	{
		m_strLastInfo = QApplication::translate("LoginManager", "User %1 is already exist.", nullptr)
			.arg(newLoginUser.getShowUser());
		return false;
	}
		
	sLogin login;
	login.loginUser = newLoginUser;

	m_mutex.lock();
	m_mapLogin.insert(newLoginUser.getAutoUserId(), login);
	m_mutex.unlock();
	ret = m_loginStorage.SaveDB(newLoginUser);
	ret |= SaveFile();
	if (ret)
	{
		m_strLastInfo = QApplication::translate("LoginManager", "Add Success.", nullptr);//QString::fromUtf8("添加成功.");
		return true;
	}
	else
	{
		m_strLastError = QApplication::translate("LoginManager", "Add Success, but save faild.", nullptr); //QString::fromUtf8("添加成功, 但保存失败.");
		return false;
	}
}

bool CLoginManager::SaveFile()
{
	m_mutex.lock();
	auto mapLogin = m_mapLogin;
	m_mutex.unlock();
	bool ret = m_loginStorage.saveFile(mapLogin);
	return ret;
}

bool CLoginManager::Load()
{
	QMap<QString, sLogin> mapLogin;
	bool ret = m_loginStorage.load(mapLogin);

	m_mutex.lock();
	m_mapLogin = mapLogin;
	m_mutex.unlock();
	return ret;
}

bool CLoginManager::Delete(const QString& strUserId)
{
	QMutexLocker lock(&m_mutex);
	sLogin login;
	if (GetLogin(strUserId, login))
	{
		QString strUserGroup = login.loginUser.getUserGroup();
		int iLevel = ::getDefaultUserGroupLevel(strUserGroup);
		if (iLevel == DUGL::nUserLevel_Develop)
		{
			m_strLastError = QApplication::translate("LoginManager", "Delete failed, no permission.", nullptr);// QString::fromUtf8("删除失败,权限不足.");
			return false;
		}

		bool ret(true);
		const QString strUserMainName = login.loginUser.getUserMainName();
		const QString strAutoUserId = login.loginUser.getAutoUserId();
		if (strUserId == strUserMainName)//要删除的用户id和主用户id相同
		{
			m_mapLogin.remove(strAutoUserId);
			ret = m_loginStorage.DeleteDB(sUserId(strUserMainName, sUserId::nUserIdType::nUserIdType_UserMainName));
		}
		else if (strUserId == strAutoUserId)//要删除的用户id是自动用户id
		{
			m_strLastError = QApplication::translate("LoginManager", "Delete failed, can not delete auto user id. main user name: %1, auto user id: %2.", nullptr)
				.arg(strUserMainName).arg(strAutoUserId);
			//QString::fromUtf8("删除失败,无法删除自动用户id. 用户主名称:%1, 自动用户id:%2.")
			//	.arg(strUserMainName).arg(strAutoUserId);
			return false;
		}
		else
		{
			sLogin& login = m_mapLogin[strAutoUserId];
			sUserId userId(strUserId);
			bool ret = login.loginUser.getUserId(strUserId, userId);
			if (ret)
				ret = m_loginStorage.DeleteDB(userId);
			login.loginUser.removeUserId(strUserId);
		}

		ret |= SaveFile();
		if (ret)
		{
			m_strLastInfo = QApplication::translate("LoginManager", "User %1 delete success.", nullptr).arg(strUserId);
			// QString::fromUtf8("用户 %1 删除成功.").arg(strUserId);
			return true;
		}
		else
		{
			QApplication::translate("LoginManager", "Delete Success, but save failed.", nullptr);
			// m_strLastError = QString::fromUtf8("删除成功,但保存失败.");
			return false;
		}
	}
	else
	{
		m_strLastError = QApplication::translate("LoginManager", "Can not find user id:%1.", nullptr).arg(strUserId);
		// QString::fromUtf8("无法找到用户id: %1.").arg(strUserId);
		return false;
	}
}

bool CLoginManager::Delete(const sUserId& userId)
{
	QMutexLocker lock(&m_mutex);
	sLogin login;
	if (GetLogin(userId.strUserId, login))
	{
		QString strUserGroup = login.loginUser.getUserGroup();
		int iLevel = ::getDefaultUserGroupLevel(strUserGroup);
		if (iLevel == DUGL::nUserLevel_Develop)
		{
			m_strLastError = QApplication::translate("LoginManager", "Delete failed, no permission.", nullptr);
			// QString::fromUtf8("删除失败,权限不足.");
			return false;
		}

		if (!login.loginUser.contains(userId))
		{
			m_strLastError = QApplication::translate("LoginManager", "User id and type to delete not match. id:%1,type:%2.", nullptr)
				.arg(userId.strUserId).arg(userId.iUserIdType);
			//QString::fromUtf8("要删除的用户id名称和类型不一致. id:%1,类型:%2.")
			//	.arg(userId.strUserId).arg(userId.iUserIdType);
			return false;
		}

		bool ret(true);
		const QString strUserMainName = login.loginUser.getUserMainName();
		const QString strAutoUserId = login.loginUser.getAutoUserId();
		if (userId.iUserIdType == sUserId::nUserIdType::nUserIdType_UserMainName)//要删除的用户id和主用户id相同
		{
			m_mapLogin.remove(strAutoUserId);
			ret = m_loginStorage.DeleteDB(sUserId(strUserMainName, sUserId::nUserIdType::nUserIdType_UserMainName));
		}
		else if (userId.iUserIdType == sUserId::nUserIdType::nUserIdType_UserAutoId)//要删除的用户id是自动用户id
		{
			m_strLastError = QApplication::translate("LoginManager", "Delete failed, can not delete auto user id. main user name: %1, auto user id: %2.", nullptr)
				.arg(strUserMainName).arg(strAutoUserId);
				/*QString::fromUtf8("删除失败,无法删除自动用户id. 用户主名称:%1, 自动用户id:%2.")
				.arg(strUserMainName).arg(strAutoUserId);*/
			return false;
		}
		else
		{
			sLogin& login = m_mapLogin[strAutoUserId];
			login.loginUser.removeUserId(userId.strUserId);
			ret = m_loginStorage.DeleteDB(userId);
		}

		ret |= SaveFile();
		if (ret)
		{
			m_strLastError = QApplication::translate("LoginManager", "Delete success.", nullptr);
			//QString::fromUtf8("删除成功.");
			return true;
		}
		else
		{
			m_strLastError = QApplication::translate("LoginManager", "Delete Success, but save failed.", nullptr);
			// QString::fromUtf8("删除成功,但保存失败.");
			return false;
		}
	}
	else
	{
		m_strLastError = QApplication::translate("LoginManager", "Can not find user id:%1.", nullptr)
			.arg(userId.strUserId);
		// QString::fromUtf8("无法找到用户id: %1.").arg(strUserId);
		return false;
	}
}

bool CLoginManager::DeleteUser(const QString& strUserId)
{
	QMutexLocker lock(&m_mutex);
	sLogin login;
	if (GetLogin(strUserId, login))
	{
		QString strUserGroup = login.loginUser.getUserGroup();
		int iLevel = ::getDefaultUserGroupLevel(strUserGroup);
		if (iLevel == DUGL::nUserLevel_Develop)
		{
			m_strLastError = QApplication::translate("LoginManager", "Delete failed, no permission.", nullptr);
			// QString::fromUtf8("删除失败,权限不足.");
			return false;
		}

		return Delete(login.loginUser.getUserMainName());
	}
	else
	{
		m_strLastError = QApplication::translate("LoginManager", "Can not find user id:%1.", nullptr)
			.arg(strUserId);
		// QString::fromUtf8("无法找到用户id: %1.").arg(strUserId);
		return false;
	}
}

bool CLoginManager::Login(const QString& strUserId, 
	const QString& strLoginVoucher, 
	sLogin::nLoginType iLoginType /*= sLogin::nLoginType::nLoginType_Normal*/)
{
	QMutexLocker lock(&m_mutex);
	if (m_iLoginFailNum > 3)
	{
		int time = m_lastFailTime.secsTo(QDateTime::currentDateTime());
		if (time < 60)
		{
			time = 60 - time;
			m_strLastError = QApplication::translate("LoginManager", "Login fail count > 3, please wait for %1s.", nullptr)
				.arg(time);
			//QString::fromUtf8("登录失败超3次,还需等待%1秒.").arg(time);
			return false;
		}
	}

	sLogin login;
	if (GetLogin(strUserId, login))
	{
		sLoginUser& loginUser = login.loginUser;
		QString strUserMainName = loginUser.getUserMainName();
		QString strAutoUserId = loginUser.getAutoUserId();
		QString strShowUser = loginUser.getShowUser();
		QList<sUserId> lstUserId = loginUser.getUserIdList();
		QString strUserGroup = loginUser.getUserGroup();
		bool ret = isDefaultUserGroup(strUserGroup);
		int iLevel(DUGL::nUserLevel_Invalid);
		if (ret)
		{
			iLevel = ::getDefaultUserGroupLevel(strUserGroup);
			if (iLevel < DUGL::nUserLevel_Operator)
			{
				m_strLastError = QApplication::translate("LoginManager", "Login failed, no permission.", nullptr);
				//QString::fromUtf8("登录失败, 权限不足.");
				return false;
			}
		}

		if (iLoginType == sLogin::nLoginType::nLoginType_Normal)
		{
			//如果strUserId是刷卡用户id, 如果他要用普通方式登录的话, 也可以登录
			if (loginUser.getPassword() != strLoginVoucher)
			{
				m_iLoginFailNum++;
				m_lastFailTime = QDateTime::currentDateTime();
				m_strLastError = QApplication::translate("LoginManager", "Login failed, password error. user id:%1, fail count: %2.", nullptr)
					// QString::fromUtf8("登录失败,密码错误.用户名:%1,失败次数:%2.")
					.arg(strUserId)
					.arg(m_iLoginFailNum);
				QString strPasswordTips = loginUser.getPasswordTips();
				if (!strPasswordTips.isEmpty())
					m_strLastError += QApplication::translate("LoginManager", "\nPassword tips:%1.", nullptr)
					//QString::fromUtf8("\n密码提示:%1.")
					.arg(strPasswordTips);
				return false;
			}
		}
		else if (iLoginType == sLogin::nLoginType::nLoginType_Card)
		{
			;
		}
		else if (iLoginType == sLogin::nLoginType::nLoginType_AutoLogin)
		{
			;
		}
		else if (iLoginType == sLogin::nLoginType::nLoginType_CrossDevice)
		{
			;
		}
		else
		{
			m_strLastError = QApplication::translate("LoginManager", "Login type not support.", nullptr);
			// QString::fromUtf8("暂不支持的登录类型.");
			return false;
		}

		m_iLoginFailNum = 0;
		login.strLoginUserId = strUserId;
		login.loginTime = QDateTime::currentDateTime();
		login.iLoginType = iLoginType;
		m_strLastInfo = QApplication::translate("LoginManager", "%1 Login success, user group:%2.", nullptr)
			// QString::fromUtf8("%1登陆成功,权限级别:%2!")
			.arg(strUserMainName)
			.arg(strUserGroup);

		if (ret && iLevel < DUGL::nUserLevel_Technician)
			m_strLastLoginOperator = strAutoUserId;
		m_mapLoginRecord.insert(login.loginTime, strAutoUserId);
		if (iLoginType != sLogin::nLoginType::nLoginType_CrossDevice)//多台设备登录联动时, 不需要重复更新数据库
		{
			m_loginStorage.saveLoginRecordDB(login);
		}
		SaveLoginRecordFile();

		emit signalLogin(strAutoUserId, strShowUser, m_strLastInfo, strUserGroup);
		return true;
	}
	else
	{
		m_iLoginFailNum++;
		m_lastFailTime = QDateTime::currentDateTime();
		m_strLastError = QApplication::translate("LoginManager", "Login failed, user id not exist. id: %1, fail count: %2.", nullptr)
			//QString::fromUtf8("登录失败,无此用户.用户名:%1,失败次数:%2")
			.arg(strUserId)
			.arg(m_iLoginFailNum);
		return false;
	}
}

bool CLoginManager::CurrentLoginUser(sLogin& login)
{
	QMutexLocker lock(&m_mutexRecord);
	QString strAutoUserId = m_mapLoginRecord.last();
	return GetLogin(strAutoUserId, login);
}

bool CLoginManager::LastOperatorLogin()
{
	m_mutexRecord.lock();
	QString strLastLoginOperator = m_strLastLoginOperator;
	m_mutexRecord.unlock();

	sLogin login;
	if (GetLogin(strLastLoginOperator, login))
	{
		bool ret = Login(login.loginUser.getUserMainName(),
			login.loginUser.getPassword(),
			sLogin::nLoginType::nLoginType_AutoLogin);
		return ret;
	}
	else
	{
		if (GetLogin("Operator", login))
		{
			bool ret = Login(login.loginUser.getUserMainName(),
				login.loginUser.getPassword(),
				sLogin::nLoginType::nLoginType_AutoLogin);
			return ret;
		}
		m_strLastError = QApplication::translate("LoginManager", "User :%1 not exist now.", nullptr)
			.arg(strLastLoginOperator);
		// QString::fromUtf8("用户:%1 已不存在").arg(strLastLoginOperator);
	}

	return false;
}

QList<QString> CLoginManager::GetLoginRecordUserNameList(int iCount /*= ShowLoginRecordNum*/)
{
	QMutexLocker lock(&m_mutex);
	QList<QString> lstLoginRecord;
	for (auto it = --m_mapLoginRecord.cend(); it != --m_mapLoginRecord.cbegin(); --it)
	{
		if (lstLoginRecord.count() > iCount)
			break;

		const QString& strAutoUserId = it.value();
		sLogin login;
		if (GetLogin(strAutoUserId, login))
		{
			QString strUserMainName = login.loginUser.getUserMainName();
			if (!lstLoginRecord.contains(strUserMainName))
				lstLoginRecord.append(strUserMainName);
		}
	}
	return lstLoginRecord;
}

void CLoginManager::Active()
{
	QMutexLocker lock(&m_mutexPlanLogoutTime);
	sLogin login;
	bool ret = CurrentLoginUser(login);
	if (ret)
	{
		QString strUserGroup = login.loginUser.getUserGroup();
		bool ret = isDefaultUserGroup(strUserGroup);
		int iLevel = ::getDefaultUserGroupLevel(strUserGroup);
		if (!ret || iLevel >= DUGL::nUserLevel_Technician)
			m_planLogoutTime = QDateTime::currentDateTime().addSecs(3 * 60);//计划登出时间在3分钟后
		else
			m_planLogoutTime = QDateTime();
	}
	else
		m_planLogoutTime = QDateTime();
}

QJsonObject CLoginManager::toJson()
{
	QJsonObject json;
	QJsonArray loginUserArray;
	for (auto login : m_mapLogin)
	{
		if (!login.loginUser.isValid())
		{
			qDebug() << QApplication::translate("LoginManager", "Invalid login user: %1, %2.", nullptr)//QString::fromUtf8("无效的登录用户: %1, %2")
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

bool CLoginManager::fromJson(const QJsonObject& json)
{
	if (!json.contains("Login"))
		return false;

	bool ret(true);
	QJsonArray loginUserArray = json.value("Login").toArray();
	m_mutex.lock();
	for (int index = 0; index < loginUserArray.count(); ++index)
	{
		QJsonObject json = loginUserArray.at(index).toObject();
		bool isOk(false);
		sLoginUser loginUser = sLoginUser::fromJson(json, &isOk);
		if (isOk)
		{
			QString strAutoUserId = loginUser.getAutoUserId();
			sLogin& login = m_mapLogin[strAutoUserId];
			login.loginUser = loginUser;
		}
		ret &= isOk;
	}
	m_mutex.unlock();

	return ret;
}

bool CLoginManager::GetLogin(const QString& strUserId, sLogin& login)
{
	QMutexLocker lock(&m_mutex);
	for (auto it = m_mapLogin.cbegin(); it != m_mapLogin.cend(); ++it)
	{
		if (it->loginUser.contains(strUserId))
		{
			login = it.value();
			return true;
		}
	}
	m_strLastError = QApplication::translate("LoginManager", "User %1 not exist.", nullptr).arg(strUserId);
	// QString::fromUtf8("用户%1不存在.").arg(strUserId);
	return false;
}

bool CLoginManager::loadDBPara(QString& strIp, int& iPort, QString& strDbName, QString& strUserId, QString& strPassword)
{
	return m_loginStorage.loadDBPara(strIp, iPort, strDbName, strUserId, strPassword);
}

bool CLoginManager::saveDBPara(QString& strIp, int& iPort, 
	QString& strDbName, QString& strUserId, QString& strPassword)
{
	return m_loginStorage.saveDBPara(strIp, iPort, strDbName, strUserId, strPassword);
}

CLoginManager::CLoginManager(): m_loginStorage(ULISM::StorageIn_IniFile | ULISM::StorageIn_DB),
	m_iLoginFailNum(0), m_mutex(QMutex::RecursionMode::Recursive), m_pTimer(nullptr)
{
	Load();
	LoadLoginRecord();
	AddDefaultUser();
	OnTimer();

	//QTimer::singleShot(1000, [&]() {
		LastOperatorLogin();
		//});
}

CLoginManager::~CLoginManager()
{
	SaveLoginRecordFile();
}

bool CLoginManager::LoadLoginRecord()
{
	QMap<QDateTime, QString> mapLoginRecord;
	QString strLastLoginOperator;
	bool ret = m_loginStorage.loadLoginRecord(mapLoginRecord, strLastLoginOperator);
	if (ret && !mapLoginRecord.isEmpty())
	{
		m_mutexRecord.lock();
		m_mapLoginRecord = mapLoginRecord;
		m_strLastLoginOperator = strLastLoginOperator;
		m_mutexRecord.unlock();
		return true;
	}
	return false;
}

bool CLoginManager::SaveLoginRecordFile()
{
	QMutexLocker lock(&m_mutexRecord);
	QMap<QString, QDateTime> mapLoginRecord;
	for (auto it = --m_mapLoginRecord.cend(); it != --m_mapLoginRecord.cbegin(); --it)
	{
		const QString& strAutoUserId = it.value();
		if (mapLoginRecord.contains(strAutoUserId))
			continue;
		mapLoginRecord.insert(strAutoUserId, it.key());
		//如果当前登录用户数量超限制了, 且当前用户是操作员, 则用户登录记录到此为止
		//if (mapLoginRecord.count() > ShowLoginRecordNum * 2)
		//{
		//	sLogin login;
		//	if (GetLogin(strAutoUserId, login))
		//	{
		//		QString strUserGroup = login.loginUser.getUserGroup();
		//		if (DUGL::nUserLevel_Operator == ::getDefaultUserGroupLevel(strUserGroup))
		//			break;
		//	}
		//}
	}

	bool ret = m_loginStorage.saveLoginRecordFile(mapLoginRecord, m_strLastLoginOperator);
	return ret;
}

void CLoginManager::AddDefaultUser()
{
	QMutexLocker lock(&m_mutex);

	sUserId developUserId("\u0054\u0061\u006F\u0079\u0075\u0079\u0075");
	sLoginUser developUser;
	developUser.addUserId(developUserId.strUserId);
	developUser.addUserId("\u0031\u0038\u0036\u0032\u0035\u0030\u0030\u0038\u0039\u0036\u0037", sUserId::nUserIdType_PhoneNumber);
	developUser.setPassword("\u0033\u0032\u0031\u0032\u0038\u0033");
	developUser.setUserGroup(getDefaultUserGroupName(DUGL::nUserLevel_Develop));
	sLogin developLogin;
	developLogin.loginUser = developUser;
	if (!Contains(developUser))
		m_mapLogin.insert(developUser.getAutoUserId(), developLogin);

	sUserId defaultAdminUserId("FXL");
	sLoginUser defaultAdminUser;
	defaultAdminUser.addUserId(defaultAdminUserId.strUserId);
	defaultAdminUser.setPassword("Fxl123");
	defaultAdminUser.setUserGroup(getDefaultUserGroupName(DUGL::nUserLevel_Admin));
	sLogin defaultAdminLogin;
	defaultAdminLogin.loginUser = defaultAdminUser;
	if (!Contains(defaultAdminUser))
		m_mapLogin.insert(defaultAdminUser.getAutoUserId(), defaultAdminLogin);

	sUserId defaultOperatorUserId("Operator");
	sLoginUser defaultOperatorUser;
	defaultOperatorUser.addUserId(defaultOperatorUserId.strUserId);
	defaultOperatorUser.setPassword("Operator");
	defaultOperatorUser.setUserGroup(getDefaultUserGroupName(DUGL::nUserLevel_Operator));
	sLogin defaultOperatorLogin;
	defaultOperatorLogin.loginUser = defaultOperatorUser;
	if (!Contains(defaultOperatorUser))
		m_mapLogin.insert(defaultOperatorUser.getAutoUserId(), defaultOperatorLogin);
}

void CLoginManager::OnTimer()
{
	if (m_pTimer == nullptr)
		m_pTimer = new QTimer;
	m_pTimer->setInterval(5000);
	connect(m_pTimer, &QTimer::timeout, this, [&]() {
		QMutexLocker lock(&m_mutexPlanLogoutTime);
		if (m_planLogoutTime.isNull())
			return;

		if (m_planLogoutTime < QDateTime::currentDateTime())
		{
			m_planLogoutTime = QDateTime();
			bool ret = LastOperatorLogin();
			if (ret)
				qDebug() << QString("auto logout success.");// QString::fromUtf8("自动登出成功.");
			else
				qDebug() << QString("auto logout failed. info:%1.").arg(m_strLastError);// QString::fromUtf8("自动登出失败. 信息:") << m_strLastError;
		}
		}, Qt::QueuedConnection);
	m_pTimer->start();

	connect(this, &CLoginManager::signalLogin, this, [&](const QString& strAutoUserId,
		const QString& strShowUser,
		const QString strResult,
		QString strUserGroup) {
		//Active();
		}, Qt::QueuedConnection);
}

