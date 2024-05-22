#ifndef _CLOGIN_H_
#define _CLOGIN_H_

#include <QList>
#include <QString>
#include <QDateTime>
#include <QJsonObject>

//Ĭ�����Ȩ�޵Ĳ���Ա��¼
#define LOGIN_DEFAULT_OPERATOR	"Operator"

struct sUserId
{
#define USERID_USERID		"UserId"
#define USERID_USERIDTYPE	"UserIdType"

	enum nUserIdType
	{
		nUserIdType_Invalid = -1,
		/*һ���û������ж�����͵��û�id.  ����һ���û�, ��ͬ���͵��û�id���ƿ�����ͬ*/
		/*һ���û����û�id���� ���ܺ�����һ���û����û�id���� ��ͬ*/
		/*
		���û�id��ϵͳ�Ĳ�ͬ�û���, Ӧ����Ψһ��
		*/
		//��FXL���豸��, ���û����ǲ����ظ���
		nUserIdType_UserMainName = 0,	//�û�������Id
		nUserIdType_UserAutoId,			//�Զ����ɵ��û�id, �����޸ĺ�ɾ��  
		nUserIdType_Card,		//ˢ����¼ʱ����id //һ���û�ֻ�ܰ�һ����Ƭ?
		nUserIdType_WorkId,		//����
		nUserIdType_PhoneNumber,//�ֻ������¼ʱ�ĵ绰���� //һ���û�ֻ�ܰ�һ���ֻ�����?	
		nUserIdType_Email,		//�����¼
		nUserIdType_QQ,
		nUserIdType_WeChat,		//΢�ŵ�¼ʱ��΢��id //һ���û�ֻ�ܰ�һ��΢��?
		//nUserIdType_Alipay,		//֧����
		//nUserIdType_ThirdParty,	//�������˺� ΢��/֧����/QQ/����
		
		//nUserIdType_Remote,			//��ͨ�û�Զ�̵�½ʱ��id
	};

	QString		strUserId;	//�û�id����
	nUserIdType	iUserIdType = nUserIdType_UserMainName;	//�û�Id����

	sUserId(const QString &strUserId);
	sUserId(const QString &strUserId, nUserIdType iUserIdType);

	bool operator<(const sUserId &userId)const;

	bool operator==(const sUserId &userId)const;

	QString getUserIdTypeName()const;
	static nUserIdType getUserIdType(const QString &strUserIdTypeName);
};


//��¼���û�
struct sLoginUser
{
#define LOGINUSER_USERMAINNAME	"UserMainName"
#define LOGINUSER_SHOWUSER		"ShowUser"
#define LOGINUSER_AUTOUSERID	"AutoUserId"
#define LOGINUSER_PASSWORD		"Password"
#define LOGINUSER_PASSWORD_TIPS	"PasswordTips"
#define LOGINUSER_AUTHORITY		"Authority"
#define LOGINUSER_USERIDCOUNT	"UserIdCount"
#define LOGINUSER_USERIDLIST	"UserIdList"

	//�û�Ȩ��
	enum nUserAuthority
	{
		nUserLevel_Invalid = 0,
		//��
		//�м�Ԥ�������ֹ�С���ݵ��û�
		//��
		nUserLevel_Operator = 0x10,	
		//��
		//�м�Ԥ���� ��ͬ�㼶����ͨ�û�, ʹ֮����ȼ���Ӧ
		//��
		nUserLevel_Technician = 0x500,	//����Ա
		nUserLevel_Engineer,	//����ʦ
		nUserLevel_Admin,		//����Ա
		nUserLevel_Develop,		//������
	};//Todo Ȩ�޿�����ϸ��һЩ, ���Բ��ղ�����վ/��Ϸ��Ȩ�޷ּ�

	//�û�ע�᷽ʽ
	enum nUserRegisterMethod
	{
		nUserRegister_LoginUiManualInput = 0,	//��½�����ֶ�����
		nUserRegister_PhoneVerifyCode,			//�ֻ���֤��ע��
		
		//������ֱ�ӵ�¼ע��
	};

private:
	//�û�ע�᷽ʽ
	nUserRegisterMethod	iUserRegisterMethod = nUserRegisterMethod::nUserRegister_LoginUiManualInput;

	QString				strUserMainName;//���û���
	QString				strShowUser;	//��ʾ���û���, �����Զ���, �����ظ�
	QString				strAutoUserId;	//�Զ����ɵ��û�id
	QString				strPassword;	//����, �����ڴ�����û�id��¼ʱ���������. TodoĳЩ�û�id���ж������߷��ض�����֤��
	QString				strPasswordTips;//������ʾ

	QString				strLastError;	//�ϴδ���
	int					iAuthority = nUserAuthority::nUserLevel_Invalid;
	QList<sUserId>		lstUserId;		//���û������е�id

public:
	sLoginUser(){}
	sLoginUser(const QString &strUserMainName,
		const QString &strShowUser,
		const QString &strAutoUserId,
		const QString &strPassword,
		const QString &strPasswordTips,
		int iAuthority,
		QList<sUserId> lstUserId);

	bool isValid();
	bool contains(const QString &strUserId)const;
	bool contains(const sUserId &userId)const;
	//******Taoyuyu***********************   
	// ����: 2023/08/11
	// ����: ���һ���û�id. ��Ҫע���������
	//		1�����Ѵ��� ��ȫһ�µ��û�id���ƺ�����ʱ, �����ʧ��
	//		2������ӵ������û���, ���Ѵ������û���ʱ, �����ʧ�ܡ� 
	//		3������ӵ����Զ��û���, �����ʧ�ܡ�
	//		4������ӵ������û���, ��ӳɹ�ʱ, ���Զ������Զ��û���
	//		5�������û���������ʱ, ��Ҫ��������û�����, �����ʧ��
	//		6������Ҫ������û���/�Զ��û���, �����в����Ĺ��캯��
	// ����: bool: ����Ƿ�ɹ�
	// ����: const QString & strUserId: �û�id����
	// ����: sUserId::nUserIdType iUserIdType: �û�id������
	//************************************
	bool addUserId(const QString &strUserId,
		sUserId::nUserIdType iUserIdType = sUserId::nUserIdType::nUserIdType_UserMainName);
	bool removeUserId(const QString &strUserId);
	bool removeUserId(const QString &strUserId, sUserId::nUserIdType iUserIdType);
	bool setUserId(const QString &strUserId,
		sUserId::nUserIdType iUserIdType = sUserId::nUserIdType::nUserIdType_UserMainName);
	void setShowUser(const QString &strShowUser);
	void setPassword(const QString &strPassword, const QString &strPasswordTips = QString());
	void setAuthority(int iAuthority);

	QString getUserMainName()const;
	QString getShowUser()const;
	QString getAutoUserId()const;
	QString getPassword()const;
	QString getPasswordTips()const;
	int getAuthority()const;
	QStringList getUserIdNameList()const;
	QList<sUserId> getUserIdList(const QString &strUserId = QString())const;
	bool getUserId(const QString &strUserId, sUserId &userId)const;
	QString getLastError()const;

	static sLoginUser fromJson(const QJsonObject &json, bool *isOk = nullptr);
	static QJsonObject toJson(sLoginUser loginUser);

	//******Taoyuyu***********************   
	// ����: 2023/08/11
	// ����: ��ͬȨ�޶�Ӧ������
	// ����: QT_NAMESPACE::QString
	// ����: int iAuthority
	//************************************
	static QString getAuthorityName(int iAuthority);
	static int getAuthority(const QString &strAuthorityName);

private:
	//strUserMainName�������û�������Id
	void autoGenerateUserId(const QString &strUserMainName);
};


//�����½
struct sLogin
{
#define LOGIN_LOGINUSER		"LoginUser"
#define LOGIN_LOGINTIME		"LoginTime"
#define LOGIN_LOGINTYPE		"LoginType"

	enum nLoginType
	{
		//�û�������Id,�û�id,�绰����,΢�ŵȵ�id�������ڵ�¼���������¼
		nLoginType_Normal = 0,	//��ͨ����, ��¼�������� 

		nLoginType_Card,		//ˢ����¼(��ʱ����Ҫ����, ����Ҫ������¼��¼)

		nLoginType_PhoneVerifyCode,//�ֻ���֤��(��ʱ����Ҫ����, ��Ҫ�ֻ���֤��)
		
		//���豸Զ�̵�¼
		nLoginType_CrossDevice = nLoginType_Card,
	};

	sLoginUser		loginUser;		//��¼�û�
	QDateTime		loginTime;		//��½ʱ��
	nLoginType		iLoginType;		//��¼����
};

#endif // _CLOGIN_H_



