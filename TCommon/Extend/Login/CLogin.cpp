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
		return QString::fromLocal8Bit("�û���");
	else if (iUserIdType == nUserIdType_UserAutoId)
		return QString::fromLocal8Bit("�Զ��û���");
	else if (iUserIdType == nUserIdType_Card)
		return QString::fromLocal8Bit("����");
	else if (iUserIdType == nUserIdType_WorkId)
		return QString::fromLocal8Bit("����");
	else if (iUserIdType == nUserIdType_PhoneNumber)
		return QString::fromLocal8Bit("�ֻ�");
	else if (iUserIdType == nUserIdType_Email)
		return QString::fromLocal8Bit("����");
	else if (iUserIdType == nUserIdType_Email)
		return QString::fromLocal8Bit("QQ");
	else if (iUserIdType == nUserIdType_Email)
		return QString::fromLocal8Bit("΢��");
	else
		return QString::fromLatin1("δ֪");
}

sUserId::nUserIdType sUserId::getUserIdType(const QString &strUserIdTypeName)
{
	if (strUserIdTypeName == QString::fromLocal8Bit("�û���"))
		return nUserIdType_UserMainName;
	else if (strUserIdTypeName == QString::fromLocal8Bit("�Զ��û���"))
		return nUserIdType_UserAutoId;
	else if (strUserIdTypeName == QString::fromLocal8Bit("����"))
		return nUserIdType_Card;
	else if (strUserIdTypeName == QString::fromLocal8Bit("����"))
		return nUserIdType_WorkId;
	else if (strUserIdTypeName == QString::fromLocal8Bit("�ֻ�"))
		return nUserIdType_PhoneNumber;
	else if (strUserIdTypeName == QString::fromLocal8Bit("����"))
		return nUserIdType_Email;
	else if (strUserIdTypeName == QString::fromLocal8Bit("QQ"))
		return nUserIdType_QQ;
	else if (strUserIdTypeName == QString::fromLocal8Bit("΢��"))
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
		this->strLastError = QString::fromLocal8Bit("���û���Ϊ��.");
		return false;
	}
	ret &= (!this->strShowUser.isEmpty());
	if (!ret)
	{
		this->strLastError = QString::fromLocal8Bit("��ʾ�û�����Ϊ��.");
		return false;
	}
	ret &= (!this->strAutoUserId.isEmpty());
	if (!ret)
	{
		this->strLastError = QString::fromLocal8Bit("�Զ��û�idΪ��.");
		return false;
	}
	ret &= (!this->lstUserId.isEmpty());
	if (!ret)
	{
		this->strLastError = QString::fromLocal8Bit("�û�id�б���Ϊ��.");
		return false;
	}

	QList<sUserId> lstUserId;
	bool isUserMainNameExist(false), isAutoUserIdExist(false);
	for (const sUserId &userId : this->lstUserId)
	{
		if (lstUserId.contains(userId))
		{
			this->strLastError = QString::fromLocal8Bit("�û�id�б��д����û�id�ظ�. ����:%1, ����:%2.")
				.arg(userId.strUserId).arg(userId.getUserIdTypeName());
			return false;
		}
		lstUserId.append(userId);
		if (userId.iUserIdType == sUserId::nUserIdType::nUserIdType_UserMainName)
		{
			if (isUserMainNameExist)
			{
				this->strLastError = QString::fromLocal8Bit("�û�id�б��д��ڶ�����û�����: %1.").arg(userId.strUserId);
				return false;
			}
			if (userId.strUserId != this->strUserMainName)
			{	
				this->strLastError = QString::fromLocal8Bit("���û����ƺ��û�id�б������û����Ʋ�һ��: %1, %2.")
					.arg(userId.strUserId).arg(this->strUserMainName);
				return false;
			}
			isUserMainNameExist = true;
		}
		else if (userId.iUserIdType == sUserId::nUserIdType::nUserIdType_UserAutoId)
		{
			if (isAutoUserIdExist)
			{
				this->strLastError = QString::fromLocal8Bit("�û�id�б��д��ڶ���Զ��û�id: %1.").arg(userId.strUserId);
				return false;
			}
			if (userId.strUserId != this->strAutoUserId)
			{
				this->strLastError = QString::fromLocal8Bit("�Զ��û�id���û�id�б����Զ��û�id��һ��: %1, %2.")
					.arg(userId.strUserId).arg(this->strAutoUserId);
				return false;
			}
			isAutoUserIdExist = true;
		}
	}
	ret &= isUserMainNameExist;
	if (!ret)
	{
		this->strLastError = QString::fromLocal8Bit("�û�id�б��в��������û�����.");
		return false;
	}
	ret &= isAutoUserIdExist;
	if (!ret)
	{
		this->strLastError = QString::fromLocal8Bit("�û�id�б��в������Զ��û�id.");
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
		strLastError = QString::fromLocal8Bit("�û��Զ�Id�������ֶ�����");
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
				strLastError = QString::fromLocal8Bit("���û����������ظ����");
				return false;//UserMainName����������
			}
		}
		else
		{
			strLastError = QString::fromLocal8Bit("�в�֧�ֵ��û�ע�᷽ʽ");
			return false;
		}
	}
	if (this->strUserMainName.isEmpty())
	{
		strLastError = QString::fromLocal8Bit("��Ҫ������û���");
		return false;
	}
	this->lstUserId.append(sUserId(strUserId, iUserIdType));
	return true;
}
bool sLoginUser::removeUserId(const QString &strUserId)
{
	if (strUserId == this->strUserMainName)
	{
		this->strLastError = QString::fromLocal8Bit("�����Ƴ����û���: %1").arg(strUserId);
		return false;
	}
	else if (strUserId == this->strAutoUserId)
	{
		this->strLastError = QString::fromLocal8Bit("�����Ƴ��Զ��û�id: %1").arg(strUserId);
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
			this->strLastError = QString::fromLocal8Bit("�����ڵ�ǰ�û�: %1").arg(strUserId);
			return false;
		}
	}
}
bool sLoginUser::removeUserId(const QString &strUserId, sUserId::nUserIdType iUserIdType)
{
	if (iUserIdType == sUserId::nUserIdType::nUserIdType_UserMainName)
	{
		this->strLastError = QString::fromLocal8Bit("�����Ƴ����û���: %1").arg(strUserId);
		return false;
	}
	else if (iUserIdType == sUserId::nUserIdType::nUserIdType_UserAutoId)
	{
		this->strLastError = QString::fromLocal8Bit("�����Ƴ��Զ��û�id: %1").arg(strUserId);
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
			this->strLastError = QString::fromLocal8Bit("�����ڵ�ǰ�û�: %1, %2")
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
			return true;//�Ѿ�����
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
				//strLastError = QString::fromLocal8Bit("���û�������������");
				return true;
			}
		}
	}
	else if (iUserIdType == sUserId::nUserIdType::nUserIdType_UserAutoId)
	{
		strLastError = QString::fromLocal8Bit("�û��Զ�Id����������");
		return false;//UserAutoId����������
	}
	else
	{
		sUserId userId(strUserId, iUserIdType);
		if (this->lstUserId.contains(userId))
			return true;//��id�Ѿ�����
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
		qDebug() << QString::fromLocal8Bit("��Ч�ĵ�¼�û�: %1, %2")
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
		return QString::fromLocal8Bit("��Ȩ��");
	}
	else if (iAuthority < sLoginUser::nUserAuthority::nUserLevel_Operator)
	{
		return QString::fromLocal8Bit("С����");
	}
	else if (iAuthority < sLoginUser::nUserAuthority::nUserLevel_Technician)
	{
		QString strLevel;
		int iLevel = iAuthority - sLoginUser::nUserAuthority::nUserLevel_Operator;
		if (iLevel > 0)
			strLevel = QString::fromLocal8Bit("(%1��)").arg(iLevel);
		return QString::fromLocal8Bit("����Ա%1").arg(strLevel);
	}
	else if (iAuthority < sLoginUser::nUserAuthority::nUserLevel_Engineer)
	{
		return QString::fromLocal8Bit("����Ա");
	}
	else if (iAuthority < sLoginUser::nUserAuthority::nUserLevel_Admin)
	{
		return QString::fromLocal8Bit("����ʦ");
	}
	else if (iAuthority < sLoginUser::nUserAuthority::nUserLevel_Develop)
	{
		return QString::fromLocal8Bit("����Ա");
	}
	else if (iAuthority == sLoginUser::nUserAuthority::nUserLevel_Develop)
	{
		return QString::fromLocal8Bit("������");
	}
	else
	{
		return QString::fromLocal8Bit("��ЧȨ��");
	}
}

int sLoginUser::getAuthority(const QString &strAuthorityName)
{
	if (strAuthorityName == QString::fromLocal8Bit("������"))
		return sLoginUser::nUserAuthority::nUserLevel_Develop;
	else if (strAuthorityName == QString::fromLocal8Bit("����Ա"))
		return sLoginUser::nUserAuthority::nUserLevel_Admin;
	else if (strAuthorityName == QString::fromLocal8Bit("����ʦ"))
		return sLoginUser::nUserAuthority::nUserLevel_Engineer;
	else if (strAuthorityName == QString::fromLocal8Bit("����Ա"))
		return sLoginUser::nUserAuthority::nUserLevel_Technician;
	else if (strAuthorityName.contains(QString::fromLocal8Bit("����Ա")))
	{
		QString strLevel;
		if (strAuthorityName.contains("(") && strAuthorityName.contains(")"))
		{
			strLevel = strAuthorityName.mid(strAuthorityName.indexOf("(") + 1);
			strLevel.chop(2);
		}
		return sLoginUser::nUserAuthority::nUserLevel_Operator + strLevel.toInt();
	}
	else if (strAuthorityName == QString::fromLocal8Bit("С����"))
		return sLoginUser::nUserAuthority::nUserLevel_Invalid + 1;
	else if (strAuthorityName == QString::fromLocal8Bit("��Ȩ��"))
		return sLoginUser::nUserAuthority::nUserLevel_Invalid;
	else
		return sLoginUser::nUserAuthority::nUserLevel_Invalid;
}

void sLoginUser::autoGenerateUserId(const QString &strUserMainName)
{
	this->strAutoUserId = QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch());
}