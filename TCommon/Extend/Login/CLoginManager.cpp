#include "CLoginManager.h"
#include <QDir>
#include <QDebug>
#include <QMessageBox>
#include <QCoreApplication>
#include "TTimeDefine.h"

//参照TTextFile的函数GetFileContent
QByteArray GetFileContent(const QString &strFilePath)
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

CLoginManager* CLoginManager::m_pInstance = nullptr;

CLoginManager * CLoginManager::GetKernel()
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

void CLoginManager::Authority(const QString &strUserId, int &iAuthority)
{
	sLogin login;
	bool ret = GetLogin(strUserId, login);
	if (ret)
		iAuthority = login.loginUser.getAuthority();
	else
		iAuthority = sLoginUser::nUserAuthority::nUserLevel_Invalid;
}

bool CLoginManager::Contains(const sLoginUser &loginUser)
{
	sLoginUser loginUserTemp = loginUser;
	if (!loginUserTemp.isValid())
	{
		m_strLastError = loginUserTemp.getLastError();
		return false;
	}

	QMutexLocker lock(&m_mutex);
	QStringList lstUserIdName = loginUser.getUserIdNameList();
	for (auto &strUserIdName : lstUserIdName)
	{
		for (auto it = m_mapLogin.cbegin(); it != m_mapLogin.cend(); ++it)
		{
			if (it->loginUser.contains(strUserIdName))
				return true;
		}
	}
	return false;
}

bool CLoginManager::Check(const QString &strUserId, const QString &strPassword)
{
	sLogin login;
	bool ret = GetLogin(strUserId, login);
	if (ret)
	{
		if (login.loginUser.getPassword() == strPassword)
		{
			m_strLastInfo = QString::fromLocal8Bit("%1校验成功, 权限级别:%2!")
				.arg(strUserId)
				.arg(login.loginUser.getAuthority());
			return true;
		}
		else
		{
			m_strLastError = QString::fromLocal8Bit("密码错误!");
			QString strPasswordTips = login.loginUser.getPasswordTips();
			if (!strPasswordTips.isEmpty())
				m_strLastError += QString::fromLocal8Bit("密码提示:%1.").arg(strPasswordTips);
			return false;
		}
	}
	else
	{
		m_strLastError = QString::fromLocal8Bit("不存在当前用户!");
		return false;
	}
}

bool CLoginManager::Save(const sLoginUser &newLoginUser)
{
	sLoginUser newLoginUserTemp(newLoginUser);
	if (!newLoginUserTemp.isValid())
	{
		m_strLastError = newLoginUserTemp.getLastError();
		return false;
	}

	bool ret = Contains(newLoginUser);
	if (ret)
		return false;

	sLogin login;
	login.loginUser = newLoginUser;

	m_mutex.lock();
	m_mapLogin.insert(newLoginUser.getAutoUserId(), login);
	m_mutex.unlock();

	ret = Save();
	if (ret)
	{
		m_strLastInfo = QString::fromLocal8Bit("添加成功.");
		return true;
	}
	else
	{
		m_strLastError = QString::fromLocal8Bit("添加成功, 但保存失败.");
		return false;
	}
}

bool CLoginManager::Delete(const QString &strUserId)
{
	int iAuthority(sLoginUser::nUserAuthority::nUserLevel_Invalid);
	Authority(strUserId, iAuthority);
	if (iAuthority == sLoginUser::nUserAuthority::nUserLevel_Develop)
	{
		m_strLastError = QString::fromLocal8Bit("删除失败,权限不足.");
		return false;
	}
	else if (iAuthority <= sLoginUser::nUserAuthority::nUserLevel_Invalid)
	{
		m_strLastError = QString::fromLocal8Bit("删除失败,无此用户.");
		return false;
	}
	else
	{
		QMutexLocker lock(&m_mutex);
		sLogin login;
		if (GetLogin(strUserId, login))
		{
			const QString strUserMainName = login.loginUser.getUserMainName();
			const QString strAutoUserId = login.loginUser.getAutoUserId();
			if (strUserId == strUserMainName)//要删除的用户id和主用户id相同
			{
				m_mapLogin.remove(strAutoUserId);
			}
			else if (strUserId == strAutoUserId)//要删除的用户id是自动用户id
			{
				m_strLastError = QString::fromLocal8Bit("删除失败,无法删除自动用户id. 用户主名称:%1, 自动用户id:%2.")
					.arg(strUserMainName).arg(strAutoUserId);
				return false;
			}
			else
			{
				sLogin &login = m_mapLogin[strAutoUserId];
				login.loginUser.removeUserId(strUserId);
			}
		}
		else
		{
			m_strLastError = QString::fromLocal8Bit("无法找到用户id: %1.").arg(strUserId);
			return false;
		}

		bool ret = Save();
		if (ret)
		{
			m_strLastInfo = QString::fromLocal8Bit("用户 %1 删除成功.").arg(strUserId);
			return true;
		}
		else
		{
			m_strLastError = QString::fromLocal8Bit("删除成功,但保存失败.");
			return false;
		}
	}
}

bool CLoginManager::Delete(const sUserId &userId)
{
	int iAuthority(sLoginUser::nUserAuthority::nUserLevel_Invalid);
	Authority(userId.strUserId, iAuthority);
	if (iAuthority == sLoginUser::nUserAuthority::nUserLevel_Develop)
	{
		m_strLastError = QString::fromLocal8Bit("删除失败,权限不足.");
		return false;
	}
	else if (iAuthority <= sLoginUser::nUserAuthority::nUserLevel_Invalid)
	{
		m_strLastError = QString::fromLocal8Bit("删除失败,无此用户.");
		return false;
	}
	else
	{
		QMutexLocker lock(&m_mutex);
		sLogin login;
		if (GetLogin(userId.strUserId, login))
		{
			if (!login.loginUser.contains(userId))
			{
				m_strLastError = QString::fromLocal8Bit("要删除的用户id名称和类型不一致. id:%1,类型:%2.")
					.arg(userId.strUserId).arg(userId.iUserIdType);
				return false;
			}

			const QString strUserMainName = login.loginUser.getUserMainName();
			const QString strAutoUserId = login.loginUser.getAutoUserId();
			if (userId.iUserIdType == sUserId::nUserIdType::nUserIdType_UserMainName)//要删除的用户id和主用户id相同
			{
				m_mapLogin.remove(strAutoUserId);
			}
			else if (userId.iUserIdType == sUserId::nUserIdType::nUserIdType_UserAutoId)//要删除的用户id是自动用户id
			{
				m_strLastError = QString::fromLocal8Bit("删除失败,无法删除自动用户id. 用户主名称:%1, 自动用户id:%2.")
					.arg(strUserMainName).arg(strAutoUserId);
				return false;
			}
			else
			{
				sLogin &login = m_mapLogin[strAutoUserId];
				login.loginUser.removeUserId(userId.strUserId);
			}
		}
		else
		{
			m_strLastError = QString::fromLocal8Bit("无法找到用户id: %1.").arg(userId.strUserId);
			return false;
		}

		bool ret = Save();
		if (ret)
		{
			m_strLastError = QString::fromLocal8Bit("删除成功.");
			return true;
		}
		else
		{
			m_strLastError = QString::fromLocal8Bit("删除成功,但保存失败.");
			return false;
		}
	}
}

bool CLoginManager::DeleteUser(const QString &strUserId)
{
	int iAuthority(sLoginUser::nUserAuthority::nUserLevel_Invalid);
	Authority(strUserId, iAuthority);
	if (iAuthority == sLoginUser::nUserAuthority::nUserLevel_Develop)
	{
		m_strLastError = QString::fromLocal8Bit("删除失败,权限不足.");
		return false;
	}
	else if (iAuthority <= sLoginUser::nUserAuthority::nUserLevel_Invalid)
	{
		m_strLastError = QString::fromLocal8Bit("删除失败,无此用户.");
		return false;
	}
	else
	{
		QMutexLocker lock(&m_mutex);
		sLogin login;
		if (GetLogin(strUserId, login))
		{
			return Delete(login.loginUser.getUserMainName());
		}
		else
		{
			m_strLastError = QString::fromLocal8Bit("无法找到用户id: %1.").arg(strUserId);
			return false;
		}
	}
}

bool CLoginManager::Login(const QString &strUserId, 
	const QString &strLoginVoucher, 
	sLogin::nLoginType iLoginType)
{
	QMutexLocker lock(&m_mutex);
	if (m_iLoginFailNum > 3)
	{
		int time = m_lastFailTime.secsTo(QDateTime::currentDateTime());
		if (time < 60)
		{
			time = 60 - time;
			m_strLastError = QString::fromLocal8Bit("登录失败超3次,还需等待%1秒.").arg(time);
			return false;
		}
	}

	sLogin login;
	if (GetLogin(strUserId, login))
	{
		sLoginUser &loginUser = login.loginUser;
		QString strUserMainName = loginUser.getUserMainName();
		QString strAutoUserId = loginUser.getAutoUserId();
		QList<sUserId> lstUserId = loginUser.getUserIdList();
		int iAuthority = loginUser.getAuthority();
		QString strAuthorityName = sLoginUser::getAuthorityName(iAuthority);
		if (iAuthority < sLoginUser::nUserAuthority::nUserLevel_Operator)
		{
			m_strLastError = QString::fromLocal8Bit("登录失败, 权限不足.");
			return false;
		}

		if (iLoginType == sLogin::nLoginType::nLoginType_Normal)
		{
			//如果strUserId是刷卡用户id, 如果他要用普通方式登录的话, 也可以登录
			if (loginUser.getPassword() != strLoginVoucher)
			{
				m_iLoginFailNum++;
				m_lastFailTime = QDateTime::currentDateTime();
				m_strLastError = QString::fromLocal8Bit("登录失败,密码错误.用户名:%1,失败次数:%2.")
					.arg(strUserId)
					.arg(m_iLoginFailNum);
				QString strPasswordTips = loginUser.getPasswordTips();
				if (!strPasswordTips.isEmpty())
					m_strLastError += QString::fromLocal8Bit("\n密码提示:%1.").arg(strPasswordTips);
				return false;
			}
		}
		else if (iLoginType == sLogin::nLoginType::nLoginType_Card)
		{
			;
		}
		else
		{
			m_strLastError = QString::fromLocal8Bit("暂不支持的登录类型.");
			return false;
		}

		m_iLoginFailNum = 0;
		login.loginTime = QDateTime::currentDateTime();
		login.iLoginType = iLoginType;
		m_strLastInfo = QString::fromLocal8Bit("%1登陆成功,权限级别:%2!")
			.arg(strUserMainName)
			.arg(strAuthorityName);

		if (iAuthority < sLoginUser::nUserAuthority::nUserLevel_Technician)
			m_strLastLoginOperator = strAutoUserId;
		m_mapLoginRecord.insert(login.loginTime, strAutoUserId);
		SaveLoginRecord();

		emit signalLogin(strAutoUserId, m_strLastInfo, iAuthority);
		return true;
	}
	else
	{
		m_iLoginFailNum++;
		m_lastFailTime = QDateTime::currentDateTime();
		m_strLastError = QString::fromLocal8Bit("登录失败,无此用户.用户名:%1,失败次数:%2")
			.arg(strUserId)
			.arg(m_iLoginFailNum);
		return false;
	}
}

bool CLoginManager::CurrentLoginUser(sLogin &login)
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
			sLogin::nLoginType::nLoginType_Normal);
		return ret;
	}
	else
	{
		m_strLastError = QString::fromLocal8Bit("用户:%1 已不存在").arg(strLastLoginOperator);
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

		const QString &strAutoUserId = it.value();
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
		if (login.loginUser.getAuthority()
			>= sLoginUser::nUserAuthority::nUserLevel_Technician)
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
			qDebug() << QString::fromLocal8Bit("无效的登录用户: %1, %2")
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

bool CLoginManager::fromeJson(const QJsonObject &json)
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
			sLogin &login = m_mapLogin[strAutoUserId];
			login.loginUser = loginUser;
		}
		ret &= isOk;
	}
	m_mutex.unlock();

	return ret;
}

bool CLoginManager::Save()
{
	bool ret(false);
	QMutexLocker lock(&m_mutex);
	if (m_iUserLoginInfoStorageMode == nUserLoginInfoStorageMode::StorageIn_IniFile)
	{
		QJsonObject json = toJson();
		if (json.isEmpty())
		{
			qDebug() << QString::fromLocal8Bit("登录用户信息为空.");
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
	}

	return ret;
}

bool CLoginManager::Load()
{
	m_mutex.lock();
	m_mapLogin.clear();
	m_mutex.unlock();

	if (m_iUserLoginInfoStorageMode == nUserLoginInfoStorageMode::StorageIn_IniFile)
	{
		QString strPath = QCoreApplication::applicationDirPath() + "/config/USER.ini";
		if (!QFile::exists(strPath))
		{
			AddSuperUser();
			return false;
		}

		QByteArray byte = GetFileContent(strPath);
		byte = QByteArray::fromHex(byte);
		QJsonParseError jsonError;
		QJsonDocument jsonDoc = QJsonDocument::fromJson(byte, &jsonError);
		if (jsonError.error != QJsonParseError::NoError)
		{
			AddSuperUser();
			return false;
		}

		QJsonObject json = jsonDoc.object();
		fromeJson(json);
	}

	AddSuperUser();

	return true;
}

bool CLoginManager::GetLogin(const QString &strUserId, sLogin &login)
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
	m_strLastError = QString::fromLocal8Bit("用户%1不存在.").arg(strUserId);
	return false;
}

bool CLoginManager::SaveLoginRecord()
{
	QMutexLocker lock(&m_mutexRecord);
	QMap<QString, QDateTime> mapLoginRecord;
	for (auto it = --m_mapLoginRecord.cend(); it != --m_mapLoginRecord.cbegin(); --it)
	{
		const QString &strAutoUserId = it.value();
		if (mapLoginRecord.contains(strAutoUserId))
			continue;
		mapLoginRecord.insert(strAutoUserId, it.key());
		//如果当前登录用户数量超限制了, 且当前用户是操作员, 则用户登录记录到此为止
		if (mapLoginRecord.count() > ShowLoginRecordNum * 2)
		{
			sLogin login;
			if (GetLogin(strAutoUserId, login))
			{
				int iAuthority = login.loginUser.getAuthority();
				if (iAuthority == sLoginUser::nUserAuthority::nUserLevel_Operator)
					break;
			}
		}
	}

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
	json.insert("LastLoginOperator", m_strLastLoginOperator);

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

bool CLoginManager::LoadLoginRecord()
{
	QMutexLocker lock(&m_mutexRecord);
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
	m_mapLoginRecord.clear();
	for (int index = 0; index < jsonArray.count(); ++index)
	{
		QJsonObject jsonObject = jsonArray.at(index).toObject();
		QString strAutoUserId = jsonObject.value(LOGIN_LOGINUSER).toString();
		QString strLoginTime = jsonObject.value(LOGIN_LOGINTIME).toString();
		QDateTime loginTime = QDateTime::fromString(strLoginTime, TIME_FORMAT_MILLISECOND_STR);
		m_mapLoginRecord.insert(loginTime, strAutoUserId);
	}
	m_strLastLoginOperator = json.value("LastLoginOperator").toString();
	return true;
}

void CLoginManager::AddSuperUser()
{
	QMutexLocker lock(&m_mutex);

	sUserId developUserId("Taoyuyu");
	sLoginUser developUser;
	developUser.addUserId(developUserId.strUserId);
	developUser.addUserId("18625008967", sUserId::nUserIdType_PhoneNumber);
	developUser.setPassword("321283");
	developUser.setAuthority(sLoginUser::nUserAuthority::nUserLevel_Develop);
	sLogin developLogin;
	developLogin.loginUser = developUser;
	if (!Contains(developUser))
		m_mapLogin.insert(developUser.getAutoUserId(), developLogin);

	sUserId defaultAdminUserId("FXL");
	sLoginUser defaultAdminUser;
	defaultAdminUser.addUserId(defaultAdminUserId.strUserId);
	defaultAdminUser.setPassword("lxf2022");
	defaultAdminUser.setAuthority(sLoginUser::nUserAuthority::nUserLevel_Admin);
	sLogin defaultAdminLogin;
	defaultAdminLogin.loginUser = defaultAdminUser;
	if (!Contains(defaultAdminUser))
		m_mapLogin.insert(defaultAdminUser.getAutoUserId(), defaultAdminLogin);

	sUserId defaultOperatorUserId("Operator");
	sLoginUser defaultOperatorUser;
	defaultOperatorUser.addUserId(defaultOperatorUserId.strUserId);
	defaultOperatorUser.setPassword("Operator");
	defaultOperatorUser.setAuthority(sLoginUser::nUserAuthority::nUserLevel_Admin);
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
				qDebug() << QString::fromLocal8Bit("自动登出成功.");
			else
				qDebug() << QString::fromLocal8Bit("自动登出失败. 信息:") << m_strLastError;
		}
	}, Qt::QueuedConnection);
	m_pTimer->start();

	connect(this, &CLoginManager::signalLogin, this, [&](const QString &strUser, const QString strResult, int authority) {
		Active();
	}, Qt::QueuedConnection);
}
