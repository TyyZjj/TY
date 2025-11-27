#include <QList>
#include <QDebug>
#include <QJsonArray>

#include "CLogin.h"

QString getDefaultUserGroupName(int iDUGL)
{
	if (iDUGL <= DUGL::nUserLevel_Invalid)
	{
		return UserGroup_Invalid;// QString::fromUtf8("无权限");
	}
	else if (iDUGL < DUGL::nUserLevel_Operator)
	{
		return UserGroup_Forbid;// QString::fromUtf8("小黑屋");
	}
	else if (iDUGL < DUGL::nUserLevel_Technician)
	{
		QString strLevel;
		int iLevel = iDUGL - DUGL::nUserLevel_Operator;
		if (iLevel > 0)
			strLevel = QString("(%1)").arg(iLevel);// QString::fromUtf8("(%1级)").arg(iLevel);
		return UserGroup_Operator + strLevel; //QString::fromUtf8("操作员%1").arg(strLevel);
	}
	else if (iDUGL < DUGL::nUserLevel_Engineer)
	{
		return UserGroup_Technician;// QString::fromUtf8("技术员");
	}
	else if (iDUGL < DUGL::nUserLevel_Supervisor)
	{
		return UserGroup_Engineer;// QString::fromUtf8("工程师");
	}
	else if (iDUGL < DUGL::nUserLevel_Admin)
	{
		return UserGroup_Supervisor;// QString::fromUtf8("主管");
	}
	else if (iDUGL < DUGL::nUserLevel_Develop)
	{
		return UserGroup_Admin;// QString::fromUtf8("管理员");
	}
	else if (iDUGL == DUGL::nUserLevel_Develop)
	{
		return UserGroup_Develop;// QString::fromUtf8("开发者");
	}
	else
	{
		return UserGroup_Invalid;// QString::fromUtf8("无效权限");
	}
}

int getDefaultUserGroupLevel(const QString &strDefaultUserGroup)
{
	if (strDefaultUserGroup == UserGroup_Develop_En ||
		strDefaultUserGroup == UserGroup_Develop_Ch/*QString::fromUtf8("开发者")*/)
		return DUGL::nUserLevel_Develop;
	else if (strDefaultUserGroup == UserGroup_Admin_En ||
		strDefaultUserGroup == UserGroup_Admin_Ch/*QString::fromUtf8("管理员")*/)
		return DUGL::nUserLevel_Admin;
	else if (strDefaultUserGroup == UserGroup_Supervisor_En ||
		strDefaultUserGroup == UserGroup_Supervisor_Ch)
		return DUGL::nUserLevel_Supervisor;
	else if (strDefaultUserGroup == UserGroup_Engineer_En ||
		strDefaultUserGroup == UserGroup_Engineer_Ch/*QString::fromUtf8("工程师")*/)
		return DUGL::nUserLevel_Engineer;
	else if (strDefaultUserGroup == UserGroup_Technician_En ||
		strDefaultUserGroup == UserGroup_Technician_Ch/*QString::fromUtf8("技术员")*/)
		return DUGL::nUserLevel_Technician;
	else if (strDefaultUserGroup.contains(UserGroup_Operator_En/*QString::fromUtf8("操作员")*/) ||
		strDefaultUserGroup.contains(UserGroup_Operator_Ch))
	{
		QString strLevel;
		if (strDefaultUserGroup.contains("(") && strDefaultUserGroup.contains(")"))
		{
			strLevel = strDefaultUserGroup.mid(strDefaultUserGroup.indexOf("(") + 1);
			strLevel.chop(1);
		}
		return DUGL::nUserLevel_Operator + strLevel.toInt();
	}
	else if (strDefaultUserGroup == UserGroup_Forbid_En ||
		strDefaultUserGroup == UserGroup_Forbid_Ch/*QString::fromUtf8("小黑屋")*/)
		return DUGL::nUserLevel_Invalid + 1;
	else if (strDefaultUserGroup == UserGroup_Invalid_En ||
		strDefaultUserGroup == UserGroup_Invalid_Ch/*QString::fromUtf8("无权限")*/)
		return DUGL::nUserLevel_Invalid;
	else
		return DUGL::nUserLevel_Invalid;
}

bool isDefaultUserGroup(const QString& strUserGroup)
{
	if (strUserGroup == UserGroup_Develop_En ||
		strUserGroup == UserGroup_Admin_En ||
		strUserGroup == UserGroup_Supervisor_En ||
		strUserGroup == UserGroup_Engineer_En ||
		strUserGroup == UserGroup_Technician_En ||
		strUserGroup == UserGroup_Operator_En ||
		strUserGroup == UserGroup_Forbid_En ||
		strUserGroup == UserGroup_Develop_Ch ||
		strUserGroup == UserGroup_Admin_Ch ||
		strUserGroup == UserGroup_Supervisor_Ch ||
		strUserGroup == UserGroup_Engineer_Ch ||
		strUserGroup == UserGroup_Technician_Ch ||
		strUserGroup == UserGroup_Operator_Ch ||
		strUserGroup == UserGroup_Forbid_Ch)
	{
		return true;
	}
	return false;
}

sUserId::sUserId(const QString& strUserId) :strUserId(strUserId) {}
sUserId::sUserId(const QString& strUserId, nUserIdType iUserIdType)
	: strUserId(strUserId), iUserIdType(iUserIdType) {}
bool sUserId::operator<(const sUserId& userId)const
{
	if (strUserId == userId.strUserId)
		return iUserIdType < userId.iUserIdType;
	else
		return strUserId < userId.strUserId;
}
bool sUserId::operator==(const sUserId& userId)const
{
	return strUserId == userId.strUserId &&
		iUserIdType == userId.iUserIdType;
}

QString sUserId::getUserIdTypeName()const
{
	if (iUserIdType == nUserIdType_UserMainName)
		return UserIdType_UserMainName;//QString::fromUtf8("用户名");
	else if (iUserIdType == nUserIdType_UserAutoId)
		return UserIdType_UserAutoId;//QString::fromUtf8("自动用户名");
	else if (iUserIdType == nUserIdType_Card)
		return UserIdType_Card;// QString::fromUtf8("卡号");
	else if (iUserIdType == nUserIdType_WorkId)
		return UserIdType_WorkId;// QString::fromUtf8("工号");
	else if (iUserIdType == nUserIdType_PhoneNumber)
		return UserIdType_PhoneNumber;// QString::fromUtf8("手机");
	else if (iUserIdType == nUserIdType_Email)
		return UserIdType_Email;// QString::fromUtf8("邮箱");
	else if (iUserIdType == nUserIdType_QQ)
		return UserIdType_QQ;// QString::fromUtf8("QQ");
	else if (iUserIdType == nUserIdType_WeChat)
		return UserIdType_WeChat;// QString::fromUtf8("微信");
	else
		return UserIdType_Invalid;// QString::fromLatin1("未知");
}

sUserId::nUserIdType sUserId::getUserIdType(const QString& strUserIdTypeName)
{
	QString str = UserIdType_Card_Ch;
	if (strUserIdTypeName == UserIdType_UserMainName_En ||
		strUserIdTypeName == UserIdType_UserMainName_Ch/*QString::fromUtf8("用户名")*/)
		return nUserIdType_UserMainName;
	else if (strUserIdTypeName == UserIdType_UserAutoId_En ||
		strUserIdTypeName == UserIdType_UserAutoId_Ch/*QString::fromUtf8("自动用户名")*/)
		return nUserIdType_UserAutoId;
	else if (strUserIdTypeName == UserIdType_Card_En ||
		strUserIdTypeName == UserIdType_Card_Ch/*QString::fromUtf8("卡号")*/)
		return nUserIdType_Card;
	else if (strUserIdTypeName == UserIdType_WorkId_En ||
		strUserIdTypeName == UserIdType_WorkId_Ch/*QString::fromUtf8("工号")*/)
		return nUserIdType_WorkId;
	else if (strUserIdTypeName == UserIdType_PhoneNumber_En ||
		strUserIdTypeName == UserIdType_PhoneNumber_Ch/*QString::fromUtf8("手机")*/)
		return nUserIdType_PhoneNumber;
	else if (strUserIdTypeName == UserIdType_Email_En ||
		strUserIdTypeName == UserIdType_Email_Ch/*QString::fromUtf8("邮箱")*/)
		return nUserIdType_Email;
	else if (strUserIdTypeName == UserIdType_QQ_En ||
		strUserIdTypeName == UserIdType_QQ_Ch/*QString::fromUtf8("QQ")*/)
		return nUserIdType_QQ;
	else if (strUserIdTypeName == UserIdType_WeChat_En ||
		strUserIdTypeName == UserIdType_WeChat_Ch/*QString::fromUtf8("微信")*/)
		return nUserIdType_WeChat;
	else
		return nUserIdType_Invalid;
}

sLoginUser::sLoginUser(const QString& strUserMainName, 
	const QString& strShowUser, 
	const QString& strAutoUserId, 
	const QString& strUserGroup, 
	const QString& strPassword, 
	const QString& strPasswordTips, 
	QList<sUserId> lstUserId)
{
	this->strUserMainName = strUserMainName;
	this->strShowUser = strShowUser;
	this->strAutoUserId = strAutoUserId;
	this->strUserGroup = strUserGroup;
	this->strPassword = strPassword;
	this->strPasswordTips = strPasswordTips;
	this->lstUserId = lstUserId;
}

bool sLoginUser::isValid()
{
	bool ret(true);
	ret &= (!this->strUserMainName.isEmpty());
	if (!ret)
	{
		this->strLastError = QApplication::translate("LoginManager", "Main user name is empty.", nullptr); //QString::fromUtf8("主用户名为空.");
		return false;
	}
	ret &= (!this->strShowUser.isEmpty());
	if (!ret)
	{
		this->strLastError = QApplication::translate("LoginManager", "Show user name is empty.", nullptr); //QString::fromUtf8("显示用户名称为空.");
		return false;
	}
	ret &= (!this->strAutoUserId.isEmpty());
	if (!ret)
	{
		this->strLastError = QApplication::translate("LoginManager", "Auto user id is empty.", nullptr); //QString::fromUtf8("自动用户id为空.");
		return false;
	}
	ret &= (!this->strUserGroup.isEmpty());
	if (!ret)
	{
		this->strLastError = QApplication::translate("LoginManager", "User group is empty.", nullptr); //QString::fromUtf8("用户组不能为空.");
		return false;
	}
	ret &= (!this->lstUserId.isEmpty());
	if (!ret)
	{
		this->strLastError = QApplication::translate("LoginManager", "User id list is empty.", nullptr); //QString::fromUtf8("用户id列表不能为空.");
		return false;
	}

	QList<sUserId> lstUserId;
	bool isUserMainNameExist(false), isAutoUserIdExist(false);
	for (const sUserId& userId : this->lstUserId)
	{
		if (lstUserId.contains(userId))
		{
			this->strLastError = QApplication::translate("LoginManager", "User id repeat in user id list. id: %1, type: %2.", nullptr)
				.arg(userId.strUserId).arg(userId.getUserIdTypeName());/*QString::fromUtf8("用户id列表中存在用户id重复. 名称:%1, 类型:%2.")
				.arg(userId.strUserId).arg(userId.getUserIdTypeName());*/
			return false;
		}
		lstUserId.append(userId);
		if (userId.iUserIdType == sUserId::nUserIdType::nUserIdType_UserMainName)
		{
			if (isUserMainNameExist)
			{
				this->strLastError = QApplication::translate("LoginManager", "Multiple user main name in user id list.id:%1.", nullptr)
					.arg(userId.strUserId); /*QString::fromUtf8("用户id列表中存在多个主用户名称: %1.").arg(userId.strUserId);*/
				return false;
			}
			if (userId.strUserId != this->strUserMainName)
			{
				this->strLastError = QApplication::translate("LoginManager", "Main user name not match the main user name in user id list.%1, %2.", nullptr)
					.arg(userId.strUserId).arg(this->strUserMainName);/*QString::fromUtf8("主用户名称和用户id列表中主用户名称不一致: %1, %2.")
					.arg(userId.strUserId).arg(this->strUserMainName);*/
				return false;
			}
			isUserMainNameExist = true;
		}
		else if (userId.iUserIdType == sUserId::nUserIdType::nUserIdType_UserAutoId)
		{
			if (isAutoUserIdExist)
			{
				this->strLastError = QApplication::translate("LoginManager", "Multiple auto user id in user id list.%1.", nullptr)
					.arg(userId.strUserId);/*QString::fromUtf8("用户id列表中存在多个自动用户id: %1.")
					.arg(userId.strUserId);*/
				return false;
			}
			if (userId.strUserId != this->strAutoUserId)
			{
				this->strLastError = QApplication::translate("LoginManager", "Auto user id not match the auto user id in user id list.%1, %2.", nullptr)
					.arg(userId.strUserId).arg(this->strAutoUserId);/*QString::fromUtf8("自动用户id和用户id列表中自动用户id不一致: %1, %2.")
					.arg(userId.strUserId).arg(this->strAutoUserId);*/
				return false;
			}
			isAutoUserIdExist = true;
		}
	}
	ret &= isUserMainNameExist;
	if (!ret)
	{
		this->strLastError = QApplication::translate("LoginManager", "Main user name not exist in user id list.", nullptr);//QString::fromUtf8("用户id列表中不存在主用户名称.");
		return false;
	}
	ret &= isAutoUserIdExist;
	if (!ret)
	{
		this->strLastError = QApplication::translate("LoginManager", "Auto user id not exist in user id list.", nullptr); //QString::fromUtf8("用户id列表中不存在自动用户id.");
		return false;
	}
	return ret;
}

bool sLoginUser::contains(const QString& strUserId) const
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

bool sLoginUser::contains(const sUserId& userId) const
{
	return this->lstUserId.contains(userId);
}

bool sLoginUser::addUserId(const QString& strUserId, 
	sUserId::nUserIdType iUserIdType /*= sUserId::nUserIdType::nUserIdType_UserMainName*/)
{
	if (iUserIdType == sUserId::nUserIdType::nUserIdType_UserAutoId)
	{
		strLastError = QApplication::translate("LoginManager", "Auto user id can not set manually.", nullptr);// QString::fromUtf8("用户自动Id不允许手动设置.");
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
				strLastError = QApplication::translate("LoginManager", "Main user name can not repeat addition.", nullptr);//QString::fromUtf8("主用户名不允许重复添加");
				return false;//UserMainName不允许重置
			}
		}
		else
		{
			strLastError = QApplication::translate("LoginManager", "Register mode not support.", nullptr); //QString::fromUtf8("尚不支持的用户注册方式");
			return false;
		}
	}
	if (this->strUserMainName.isEmpty())
	{
		strLastError = QApplication::translate("LoginManager", "Main user name must add first.", nullptr);// QString::fromUtf8("先要添加主用户名");
		return false;
	}
	this->lstUserId.append(sUserId(strUserId, iUserIdType));
	return true;
}

bool sLoginUser::removeUserId(const QString& strUserId)
{
	if (strUserId == this->strUserMainName)
	{
		this->strLastError = QApplication::translate("LoginManager", "Can not remove main user name: %1.", nullptr)
			.arg(strUserId);// QString::fromUtf8("不能移除主用户名: %1").arg(strUserId);
		return false;
	}
	else if (strUserId == this->strAutoUserId)
	{
		this->strLastError = QApplication::translate("LoginManager", "Can not remove auto user id: %1.", nullptr)
			.arg(strUserId);// QString::fromUtf8("不能移除自动用户id: %1").arg(strUserId);
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
			this->strLastError = QApplication::translate("LoginManager", "User id not exist: %1.", nullptr)
				.arg(strUserId);// QString::fromUtf8("不存在当前用户: %1").arg(strUserId);
			return false;
		}
	}
}

bool sLoginUser::removeUserId(const QString& strUserId, sUserId::nUserIdType iUserIdType)
{
	if (iUserIdType == sUserId::nUserIdType::nUserIdType_UserMainName)
	{
		this->strLastError = QApplication::translate("LoginManager", "Can not remove main user name: %1, %2.", nullptr)
			.arg(strUserId).arg(iUserIdType);// QString::fromUtf8("不能移除主用户名: %1").arg(strUserId);
		return false;
	}
	else if (iUserIdType == sUserId::nUserIdType::nUserIdType_UserAutoId)
	{
		this->strLastError = QApplication::translate("LoginManager", "Can not remove auto user id: %1, %2.", nullptr)
			.arg(strUserId).arg(iUserIdType);// QString::fromUtf8("不能移除自动用户id: %1").arg(strUserId);
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
			this->strLastError = QApplication::translate("LoginManager", "User id not exist: %1, %2.", nullptr)
				.arg(strUserId).arg(iUserIdType);// QString::fromUtf8("不存在当前用户: %1").arg(strUserId);
			return false;
		}
	}
}

bool sLoginUser::setUserId(const QString& strUserId, 
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
				for (auto& userId : this->lstUserId)
				{
					if (userId.iUserIdType == sUserId::nUserIdType::nUserIdType_UserMainName)
						userId.strUserId = strUserId;
				}
				this->strUserMainName = strUserId;
				this->strShowUser = strUserId;
				//strLastError = QString::fromUtf8("主用户名不允许重置");
				return true;
			}
		}
	}
	else if (iUserIdType == sUserId::nUserIdType::nUserIdType_UserAutoId)
	{
		strLastError = QApplication::translate("LoginManager", "Auto user id cannot reset.", nullptr);// QString::fromUtf8("用户自动Id不允许重置");
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

void sLoginUser::setShowUser(const QString& strShowUser)
{
	this->strShowUser = strShowUser;
}

void sLoginUser::setPassword(const QString& strPassword, 
	const QString& strPasswordTips /*= QString()*/)
{
	this->strPassword = strPassword;
	if (!strPasswordTips.isEmpty())
		this->strPasswordTips = strPasswordTips;
}

void sLoginUser::setUserGroup(QString strUserGroup)
{
	this->strUserGroup = strUserGroup;
}

QStringList sLoginUser::getUserIdNameList() const
{
	QStringList lstUserIdName;
	for (auto& userId : this->lstUserId)
	{
		if (!lstUserIdName.contains(userId.strUserId))
			lstUserIdName.append(userId.strUserId);
	}
	return lstUserIdName;
}

QList<sUserId> sLoginUser::getUserIdList(const QString& strUserId /*= QString()*/) const
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

QString sLoginUser::getLastError()const
{
	return strLastError;
}

sLoginUser sLoginUser::fromJson(const QJsonObject& json, bool* isOk /*= nullptr*/)
{
	QString strUserMainName = json.value(LOGINUSER_USERMAINNAME).toString("");
	QString strShowUser = json.value(LOGINUSER_SHOWUSER).toString("");
	QString strAutoUserId = json.value(LOGINUSER_AUTOUSERID).toString("");
	QString strUserGroup = json.value(LOGINUSER_USERGROUP).toString("");
	QString strPassword = json.value(LOGINUSER_PASSWORD).toString("");
	QString strPasswordTips = json.value(LOGINUSER_PASSWORD_TIPS).toString("");
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
		strUserGroup, strPassword, strPasswordTips,  lstUserId);
	if (isOk)
		*isOk = loginUser.isValid();

	return loginUser;
}

QJsonObject sLoginUser::toJson(sLoginUser loginUser)
{
	if (!loginUser.isValid())
	{
		qDebug() << QString::fromUtf8("无效的登录用户: %1, %2")
			.arg(loginUser.getUserMainName())
			.arg(loginUser.getLastError());
		return QJsonObject();
	}

	QJsonObject loginUserJson;
	loginUserJson.insert(LOGINUSER_USERMAINNAME, loginUser.getUserMainName());
	loginUserJson.insert(LOGINUSER_SHOWUSER, loginUser.getShowUser());
	loginUserJson.insert(LOGINUSER_AUTOUSERID, loginUser.getAutoUserId());
	loginUserJson.insert(LOGINUSER_USERGROUP, loginUser.getUserGroup());
	loginUserJson.insert(LOGINUSER_PASSWORD, loginUser.getPassword());
	loginUserJson.insert(LOGINUSER_PASSWORD_TIPS, loginUser.getPasswordTips());
	loginUserJson.insert(LOGINUSER_USERIDCOUNT, loginUser.getUserIdList().count());

	QJsonArray userIdArray;
	const QList<sUserId> lstUserId = loginUser.getUserIdList();
	for (const sUserId& userId : lstUserId)
	{
		QJsonObject userIdJson;
		userIdJson.insert(USERID_USERID, userId.strUserId);
		userIdJson.insert(USERID_USERIDTYPE, userId.iUserIdType);
		userIdArray.append(userIdJson);
	}
	loginUserJson.insert(LOGINUSER_USERIDLIST, userIdArray);

	return loginUserJson;
}

bool sLoginUser::getUserId(const QString& strUserId, sUserId& userId)const
{
	for (const sUserId& userIdTemp : this->lstUserId)
	{
		if (userIdTemp.strUserId == strUserId)
		{
			userId = userIdTemp;
			return true;
		}
	}
	return false;
}

void sLoginUser::autoGenerateUserId(const QString& strUserMainName)
{
	static long long iLast(QDateTime::currentDateTime().toMSecsSinceEpoch());
	long long iCurrent = QDateTime::currentDateTime().toMSecsSinceEpoch();

	if (iCurrent - iLast < 10)
	{
		this->strAutoUserId = QString::number(++iLast);
	}
	else
	{
		iLast = iCurrent;
		this->strAutoUserId = QString::number(iCurrent);
	}
}


