#include <QList>
#include <QDebug>
#include <QJsonArray>
#include "CLogin.h"

sUserId::sUserId(const QString &strUserId) :strUserId(strUserId) {}
sUserId::sUserId(const QString &strUserId, nUserIdType iUserIdType)
	:strUserId(strUserId), iUserIdType(iUserIdType) {}
bool sUserId::operator<(const sUserId &userId)const
{
	if (strUserId == userId.strUserId)
		return iUserIdType < userId.iUserIdType;
	else
		return strUserId < userId.strUserId;
}

QString sUserId::getUserIdTypeName()const
{
	if (iUserIdType == nUserIdType_UserMainName)
		return QString::fromLocal8Bit("用户名");
	else if (iUserIdType == nUserIdType_UserAutoId)
		return QString::fromLocal8Bit("自动用户名");
	else if (iUserIdType == nUserIdType_Card)
		return QString::fromLocal8Bit("卡号");
	else if (iUserIdType == nUserIdType_WorkId)
		return QString::fromLocal8Bit("工号");
	else if (iUserIdType == nUserIdType_PhoneNumber)
		return QString::fromLocal8Bit("手机");
	else if (iUserIdType == nUserIdType_Email)
		return QString::fromLocal8Bit("邮箱");
	else if (iUserIdType == nUserIdType_Email)
		return QString::fromLocal8Bit("QQ");
	else if (iUserIdType == nUserIdType_Email)
		return QString::fromLocal8Bit("微信");
	else
		return QString::fromLatin1("未知");
}

sUserId::nUserIdType sUserId::getUserIdType(const QString &strUserIdTypeName)
{
	if (strUserIdTypeName == QString::fromLocal8Bit("用户名"))
		return nUserIdType_UserMainName;
	else if (strUserIdTypeName == QString::fromLocal8Bit("自动用户名"))
		return nUserIdType_UserAutoId;
	else if (strUserIdTypeName == QString::fromLocal8Bit("卡号"))
		return nUserIdType_Card;
	else if (strUserIdTypeName == QString::fromLocal8Bit("工号"))
		return nUserIdType_WorkId;
	else if (strUserIdTypeName == QString::fromLocal8Bit("手机"))
		return nUserIdType_PhoneNumber;
	else if (strUserIdTypeName == QString::fromLocal8Bit("邮箱"))
		return nUserIdType_Email;
	else if (strUserIdTypeName == QString::fromLocal8Bit("QQ"))
		return nUserIdType_QQ;
	else if (strUserIdTypeName == QString::fromLocal8Bit("微信"))
		return nUserIdType_WeChat;
	else
		return nUserIdType_Invalid;
}

bool sUserId::operator==(const sUserId &userId)const
{
	return strUserId == userId.strUserId &&
		iUserIdType == userId.iUserIdType;
}


sLoginUser::sLoginUser(const QString &strUserMainName, 
	const QString &strShowUser, 
	const QString &strAutoUserId, 
	const QString &strPassword, 
	const QString &strPasswordTips, 
	int iAuthority, QList<sUserId> lstUserId)
{
	this->strUserMainName = strUserMainName;
	this->strShowUser = strShowUser;
	this->strAutoUserId = strAutoUserId;
	this->strPassword = strPassword;
	this->strPasswordTips = strPasswordTips;
	this->iAuthority = iAuthority;
	this->lstUserId = lstUserId;
}

bool sLoginUser::isValid() 
{
	bool ret(true);
	ret &= (!this->strUserMainName.isEmpty());
	if (!ret)
	{
		this->strLastError = QString::fromLocal8Bit("主用户名为空.");
		return false;
	}
	ret &= (!this->strShowUser.isEmpty());
	if (!ret)
	{
		this->strLastError = QString::fromLocal8Bit("显示用户名称为空.");
		return false;
	}
	ret &= (!this->strAutoUserId.isEmpty());
	if (!ret)
	{
		this->strLastError = QString::fromLocal8Bit("自动用户id为空.");
		return false;
	}
	ret &= (!this->lstUserId.isEmpty());
	if (!ret)
	{
		this->strLastError = QString::fromLocal8Bit("用户id列表不能为空.");
		return false;
	}

	QList<sUserId> lstUserId;
	bool isUserMainNameExist(false), isAutoUserIdExist(false);
	for (const sUserId &userId : this->lstUserId)
	{
		if (lstUserId.contains(userId))
		{
			this->strLastError = QString::fromLocal8Bit("用户id列表中存在用户id重复. 名称:%1, 类型:%2.")
				.arg(userId.strUserId).arg(userId.getUserIdTypeName());
			return false;
		}
		lstUserId.append(userId);
		if (userId.iUserIdType == sUserId::nUserIdType::nUserIdType_UserMainName)
		{
			if (isUserMainNameExist)
			{
				this->strLastError = QString::fromLocal8Bit("用户id列表中存在多个主用户名称: %1.").arg(userId.strUserId);
				return false;
			}
			if (userId.strUserId != this->strUserMainName)
			{	
				this->strLastError = QString::fromLocal8Bit("主用户名称和用户id列表中主用户名称不一致: %1, %2.")
					.arg(userId.strUserId).arg(this->strUserMainName);
				return false;
			}
			isUserMainNameExist = true;
		}
		else if (userId.iUserIdType == sUserId::nUserIdType::nUserIdType_UserAutoId)
		{
			if (isAutoUserIdExist)
			{
				this->strLastError = QString::fromLocal8Bit("用户id列表中存在多个自动用户id: %1.").arg(userId.strUserId);
				return false;
			}
			if (userId.strUserId != this->strAutoUserId)
			{
				this->strLastError = QString::fromLocal8Bit("自动用户id和用户id列表中自动用户id不一致: %1, %2.")
					.arg(userId.strUserId).arg(this->strAutoUserId);
				return false;
			}
			isAutoUserIdExist = true;
		}
	}
	ret &= isUserMainNameExist;
	if (!ret)
	{
		this->strLastError = QString::fromLocal8Bit("用户id列表中不存在主用户名称.");
		return false;
	}
	ret &= isAutoUserIdExist;
	if (!ret)
	{
		this->strLastError = QString::fromLocal8Bit("用户id列表中不存在自动用户id.");
		return false;
	}
	return ret;
}
bool sLoginUser::contains(const QString &strUserId)const
{
	if (this->strUserMainName == strUserId ||
		this->strAutoUserId == strUserId)
		return true;
	for (auto it = this->lstUserId.cbegin(); it != this->lstUserId.cend(); ++it)
	{
		if (it->strUserId == strUserId)
			return true;
	}
	return false;
}
bool sLoginUser::contains(const sUserId &userId)const
{
	return this->lstUserId.contains(userId);
}
bool sLoginUser::addUserId(const QString &strUserId,
	sUserId::nUserIdType iUserIdType /*= sUserId::nUserIdType::nUserIdType_UserMainName*/)
{
	if (iUserIdType == sUserId::nUserIdType::nUserIdType_UserAutoId)
	{
		strLastError = QString::fromLocal8Bit("用户自动Id不允许手动设置");
		return false;
	}
	if (iUserIdType == sUserId::nUserIdType::nUserIdType_UserMainName)
	{
		if (iUserRegisterMethod == nUserRegisterMethod::nUserRegister_LoginUiManualInput)
		{
			if (this->strUserMainName.isEmpty())
			{
				this->strUserMainName = strUserId;
				this->strShowUser = strUserId;
				autoGenerateUserId(strUserId);
				this->lstUserId.append(sUserId(this->strAutoUserId, sUserId::nUserIdType::nUserIdType_UserAutoId));
			}
			else
			{
				strLastError = QString::fromLocal8Bit("主用户名不允许重复添加");
				return false;//UserMainName不允许重置
			}
		}
		else
		{
			strLastError = QString::fromLocal8Bit("尚不支持的用户注册方式");
			return false;
		}
	}
	if (this->strUserMainName.isEmpty())
	{
		strLastError = QString::fromLocal8Bit("先要添加主用户名");
		return false;
	}
	this->lstUserId.append(sUserId(strUserId, iUserIdType));
	return true;
}
bool sLoginUser::removeUserId(const QString &strUserId)
{
	if (strUserId == this->strUserMainName)
	{
		this->strLastError = QString::fromLocal8Bit("不能移除主用户名: %1").arg(strUserId);
		return false;
	}
	else if (strUserId == this->strAutoUserId)
	{
		this->strLastError = QString::fromLocal8Bit("不能移除自动用户id: %1").arg(strUserId);
		return false;
	}
	else
	{
		if (contains(strUserId))
		{
			for (auto it = this->lstUserId.begin(); it != this->lstUserId.end();)
			{
				if (it->strUserId == strUserId)
					this->lstUserId.erase(it);
				else
					++it;
			}
			return true;
		}
		else
		{
			this->strLastError = QString::fromLocal8Bit("不存在当前用户: %1").arg(strUserId);
			return false;
		}
	}
}
bool sLoginUser::removeUserId(const QString &strUserId, sUserId::nUserIdType iUserIdType)
{
	if (iUserIdType == sUserId::nUserIdType::nUserIdType_UserMainName)
	{
		this->strLastError = QString::fromLocal8Bit("不能移除主用户名: %1").arg(strUserId);
		return false;
	}
	else if (iUserIdType == sUserId::nUserIdType::nUserIdType_UserAutoId)
	{
		this->strLastError = QString::fromLocal8Bit("不能移除自动用户id: %1").arg(strUserId);
		return false;
	}
	else
	{
		sUserId userId(strUserId, iUserIdType);
		if (this->lstUserId.contains(userId))
		{
			this->lstUserId.removeAll(userId);
			return true;
		}
		else
		{
			this->strLastError = QString::fromLocal8Bit("不存在当前用户: %1, %2")
				.arg(strUserId);
			return false;
		}
	}
}
bool sLoginUser::setUserId(const QString &strUserId,
	sUserId::nUserIdType iUserIdType /*= sUserId::nUserIdType::nUserIdType_UserMainName*/)
{
	if (iUserIdType == sUserId::nUserIdType::nUserIdType_UserMainName)
	{
		if (this->strUserMainName == strUserId)
		{
			return true;//已经是了
		}
		else
		{
			if (this->strUserMainName.isEmpty())
			{
				return addUserId(strUserId, iUserIdType);
			}
			else
			{
				for (auto &userId : this->lstUserId)
				{
					if (userId.iUserIdType == sUserId::nUserIdType::nUserIdType_UserMainName)
						userId.strUserId = strUserId;
				}
				this->strUserMainName = strUserId;
				this->strShowUser = strUserId;
				//strLastError = QString::fromLocal8Bit("主用户名不允许重置");
				return true;
			}
		}
	}
	else if (iUserIdType == sUserId::nUserIdType::nUserIdType_UserAutoId)
	{
		strLastError = QString::fromLocal8Bit("用户自动Id不允许重置");
		return false;//UserAutoId不允许重置
	}
	else
	{
		sUserId userId(strUserId, iUserIdType);
		if (this->lstUserId.contains(userId))
			return true;//该id已经存在
		else
			return addUserId(strUserId, iUserIdType);
	}
}
void sLoginUser::setShowUser(const QString &strShowUser)
{
	this->strShowUser = strShowUser;
}
void sLoginUser::setPassword(const QString &strPassword, const QString &strPasswordTips /*= QString()*/)
{
	this->strPassword = strPassword;
	if (!strPasswordTips.isEmpty())
		this->strPasswordTips = strPasswordTips;
}

void sLoginUser::setAuthority(int iAuthority)
{
	this->iAuthority = iAuthority;
}
QString sLoginUser::getUserMainName()const
{
	return this->strUserMainName;
}
QString sLoginUser::getShowUser()const
{
	return this->strShowUser;
}
QString sLoginUser::getAutoUserId()const
{
	return this->strAutoUserId;
}
QString sLoginUser::getPassword()const
{
	return this->strPassword;
}
QString sLoginUser::getPasswordTips()const
{
	return this->strPasswordTips;
}
int sLoginUser::getAuthority()const
{
	return this->iAuthority;
}

QStringList sLoginUser::getUserIdNameList()const
{
	QStringList lstUserIdName;
	for (auto &userId : this->lstUserId)
	{
		if (!lstUserIdName.contains(userId.strUserId))
			lstUserIdName.append(userId.strUserId);
	}
	return lstUserIdName;
}
QList<sUserId> sLoginUser::getUserIdList(const QString &strUserId /*= QString()*/)const
{
	if (!strUserId.isEmpty())
	{
		QList<sUserId> lstUserId;
		for (auto userId : this->lstUserId)
		{
			if (userId.strUserId == strUserId)
				lstUserId.append(userId);
		}
		return lstUserId;
	}
	else
		return this->lstUserId;
}
bool sLoginUser::getUserId(const QString &strUserId, sUserId &userId)const
{
	for (const sUserId &userIdTemp : this->lstUserId)
	{
		if (userIdTemp.strUserId == strUserId)
		{
			userId = userIdTemp;
			return true;
		}
	}
	return false;
}
QString sLoginUser::getLastError()const
{
	return strLastError;
}

sLoginUser sLoginUser::fromJson(const QJsonObject &json, bool *isOk /*= nullptr*/)
{
	QString strUserMainName = json.value(LOGINUSER_USERMAINNAME).toString("");
	QString strShowUser = json.value(LOGINUSER_SHOWUSER).toString("");
	QString strAutoUserId = json.value(LOGINUSER_AUTOUSERID).toString("");
	QString strPassword = json.value(LOGINUSER_PASSWORD).toString("");
	QString strPasswordTips = json.value(LOGINUSER_PASSWORD_TIPS).toString("");
	int iAuthority = json.value(LOGINUSER_AUTHORITY).toInt(nUserAuthority::nUserLevel_Invalid);
	QJsonArray userIdArray = json.value(LOGINUSER_USERIDLIST).toArray();
	int iUserIdCount = json.value(LOGINUSER_USERIDCOUNT).toInt(0);
	QList<sUserId> lstUserId;
	for (int i = 0; i < iUserIdCount; i++)
	{
		QJsonObject userIdJson = userIdArray.at(i).toObject();
		QString strUserId = userIdJson.value(USERID_USERID).toString();
		int iUserIdType = userIdJson.value(USERID_USERIDTYPE).toInt(sUserId::nUserIdType::nUserIdType_UserMainName);
		lstUserId.append(sUserId(strUserId, (sUserId::nUserIdType)iUserIdType));
		//loginUser.addUserId(strUserId, (sUserId::nUserIdType)iUserIdType);
	}
	
	sLoginUser loginUser(strUserMainName, strShowUser, strAutoUserId,
		strPassword, strPasswordTips, iAuthority, lstUserId);
	if (isOk)
		*isOk = loginUser.isValid();

	return loginUser;
}

QJsonObject sLoginUser::toJson(sLoginUser loginUser)
{
	if (!loginUser.isValid())
	{
		qDebug() << QString::fromLocal8Bit("无效的登录用户: %1, %2")
			.arg(loginUser.getUserMainName())
			.arg(loginUser.getLastError());
		return QJsonObject();
	}

	QJsonObject loginUserJson;
	loginUserJson.insert(LOGINUSER_USERMAINNAME, loginUser.getUserMainName());
	loginUserJson.insert(LOGINUSER_SHOWUSER, loginUser.getShowUser());
	loginUserJson.insert(LOGINUSER_AUTOUSERID, loginUser.getAutoUserId());
	loginUserJson.insert(LOGINUSER_PASSWORD, loginUser.getPassword());
	loginUserJson.insert(LOGINUSER_PASSWORD_TIPS, loginUser.getPasswordTips());
	loginUserJson.insert(LOGINUSER_AUTHORITY, loginUser.getAuthority());
	loginUserJson.insert(LOGINUSER_USERIDCOUNT, loginUser.getUserIdList().count());

	QJsonArray userIdArray;
	const QList<sUserId> lstUserId = loginUser.getUserIdList();
	for (const sUserId &userId : lstUserId)
	{
		QJsonObject userIdJson;
		userIdJson.insert(USERID_USERID, userId.strUserId);
		userIdJson.insert(USERID_USERIDTYPE, userId.iUserIdType);
		userIdArray.append(userIdJson);
	}
	loginUserJson.insert(LOGINUSER_USERIDLIST, userIdArray);

	return loginUserJson;
}

QString sLoginUser::getAuthorityName(int iAuthority)
{
	if (iAuthority <= sLoginUser::nUserAuthority::nUserLevel_Invalid)
	{
		return QString::fromLocal8Bit("无权限");
	}
	else if (iAuthority < sLoginUser::nUserAuthority::nUserLevel_Operator)
	{
		return QString::fromLocal8Bit("小黑屋");
	}
	else if (iAuthority < sLoginUser::nUserAuthority::nUserLevel_Technician)
	{
		QString strLevel;
		int iLevel = iAuthority - sLoginUser::nUserAuthority::nUserLevel_Operator;
		if (iLevel > 0)
			strLevel = QString::fromLocal8Bit("(%1级)").arg(iLevel);
		return QString::fromLocal8Bit("操作员%1").arg(strLevel);
	}
	else if (iAuthority < sLoginUser::nUserAuthority::nUserLevel_Engineer)
	{
		return QString::fromLocal8Bit("技术员");
	}
	else if (iAuthority < sLoginUser::nUserAuthority::nUserLevel_Admin)
	{
		return QString::fromLocal8Bit("工程师");
	}
	else if (iAuthority < sLoginUser::nUserAuthority::nUserLevel_Develop)
	{
		return QString::fromLocal8Bit("管理员");
	}
	else if (iAuthority == sLoginUser::nUserAuthority::nUserLevel_Develop)
	{
		return QString::fromLocal8Bit("开发者");
	}
	else
	{
		return QString::fromLocal8Bit("无效权限");
	}
}

int sLoginUser::getAuthority(const QString &strAuthorityName)
{
	if (strAuthorityName == QString::fromLocal8Bit("开发者"))
		return sLoginUser::nUserAuthority::nUserLevel_Develop;
	else if (strAuthorityName == QString::fromLocal8Bit("管理员"))
		return sLoginUser::nUserAuthority::nUserLevel_Admin;
	else if (strAuthorityName == QString::fromLocal8Bit("工程师"))
		return sLoginUser::nUserAuthority::nUserLevel_Engineer;
	else if (strAuthorityName == QString::fromLocal8Bit("技术员"))
		return sLoginUser::nUserAuthority::nUserLevel_Technician;
	else if (strAuthorityName.contains(QString::fromLocal8Bit("操作员")))
	{
		QString strLevel;
		if (strAuthorityName.contains("(") && strAuthorityName.contains(")"))
		{
			strLevel = strAuthorityName.mid(strAuthorityName.indexOf("(") + 1);
			strLevel.chop(2);
		}
		return sLoginUser::nUserAuthority::nUserLevel_Operator + strLevel.toInt();
	}
	else if (strAuthorityName == QString::fromLocal8Bit("小黑屋"))
		return sLoginUser::nUserAuthority::nUserLevel_Invalid + 1;
	else if (strAuthorityName == QString::fromLocal8Bit("无权限"))
		return sLoginUser::nUserAuthority::nUserLevel_Invalid;
	else
		return sLoginUser::nUserAuthority::nUserLevel_Invalid;
}

void sLoginUser::autoGenerateUserId(const QString &strUserMainName)
{
	this->strAutoUserId = QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch());
}