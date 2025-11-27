#ifndef _CLOGIN_H_
#define _CLOGIN_H_

#include <QList>
#include <QString>
#include <QDateTime>
#include <QJsonObject>

//默认最低权限的操作员登录
#define LOGIN_DEFAULT_OPERATOR	"Operator"

struct sUserId
{
#define USERID_USERID		"UserId"
#define USERID_USERIDTYPE	"UserIdType"

	enum nUserIdType
	{
		nUserIdType_Invalid = -1,
		/*一个用户可以有多个类型的用户id.  对于一个用户, 不同类型的用户id名称可以相同*/
		/*一个用户的用户id名称 不能和另外一个用户的用户id名称 相同*/
		/*
		各用户id在系统的不同用户中, 应具有唯一性
		*/
		//在FXL的设备中, 主用户名是不可重复的
		nUserIdType_UserMainName = 0,	//用户主类型Id
		nUserIdType_UserAutoId,			//自动生成的用户id, 不能修改和删除  
		nUserIdType_Card,		//刷卡登录时卡的id //一个用户只能绑定一个卡片?
		nUserIdType_WorkId,		//工号
		nUserIdType_PhoneNumber,//手机号码登录时的电话号码 //一个用户只能绑定一个手机号码?	
		nUserIdType_Email,		//邮箱登录
		nUserIdType_QQ,
		nUserIdType_WeChat,		//微信登录时的微信id //一个用户只能绑定一个微信?
		//nUserIdType_Alipay,		//支付宝
		//nUserIdType_ThirdParty,	//第三方账号 微信/支付宝/QQ/邮箱
		
		//nUserIdType_Remote,			//普通用户远程登陆时的id
	};

	QString		strUserId;	//用户id名称
	nUserIdType	iUserIdType = nUserIdType_UserMainName;	//用户Id类型

	sUserId(const QString &strUserId);
	sUserId(const QString &strUserId, nUserIdType iUserIdType);

	bool operator<(const sUserId &userId)const;

	bool operator==(const sUserId &userId)const;

	QString getUserIdTypeName()const;
	static nUserIdType getUserIdType(const QString &strUserIdTypeName);
};


//登录的用户
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

	//用户权限
	enum nUserAuthority
	{
		nUserLevel_Invalid = 0,
		//↑
		//中间预留给各种关小黑屋的用户
		//↓
		nUserLevel_Operator = 0x10,	
		//↑
		//中间预留给 不同层级的普通用户, 使之能与等级对应
		//↓
		nUserLevel_Technician = 0x500,	//技术员
		nUserLevel_Engineer,	//工程师
		nUserLevel_Admin,		//管理员
		nUserLevel_Develop,		//开发者
	};//Todo 权限可以再细分一些, 可以参照博客网站/游戏的权限分级

	//用户注册方式
	enum nUserRegisterMethod
	{
		nUserRegister_LoginUiManualInput = 0,	//登陆界面手动输入
		nUserRegister_PhoneVerifyCode,			//手机验证码注册
		
		//第三方直接登录注册
	};

private:
	//用户注册方式
	nUserRegisterMethod	iUserRegisterMethod = nUserRegisterMethod::nUserRegister_LoginUiManualInput;

	QString				strUserMainName;//主用户名
	QString				strShowUser;	//显示的用户名, 可以自定义, 可以重复
	QString				strAutoUserId;	//自动生成的用户id
	QString				strPassword;	//密码, 仅用于大多数用户id登录时输入的密码. Todo某些用户id会有独立或者非特定的验证码
	QString				strPasswordTips;//密码提示

	QString				strLastError;	//上次错误
	int					iAuthority = nUserAuthority::nUserLevel_Invalid;
	QList<sUserId>		lstUserId;		//该用户的所有的id

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
	// 日期: 2023/08/11
	// 功能: 添加一个用户id. 需要注意以下情况
	//		1、当已存在 完全一致的用户id名称和类型时, 会添加失败
	//		2、当添加的是主用户名, 且已存在主用户名时, 会添加失败。 
	//		3、当添加的是自动用户名, 会添加失败。
	//		4、当添加的是主用户名, 添加成功时, 会自动生成自动用户名
	//		5、当主用户名不存在时, 需要先添加主用户名称, 否则会失败
	//		6、如需要添加主用户名/自动用户名, 调用有参数的构造函数
	// 返回: bool: 添加是否成功
	// 参数: const QString & strUserId: 用户id名称
	// 参数: sUserId::nUserIdType iUserIdType: 用户id的类型
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
	// 日期: 2023/08/11
	// 功能: 不同权限对应的名称
	// 返回: QT_NAMESPACE::QString
	// 参数: int iAuthority
	//************************************
	static QString getAuthorityName(int iAuthority);
	static int getAuthority(const QString &strAuthorityName);

private:
	//strUserMainName必须是用户主类型Id
	void autoGenerateUserId(const QString &strUserMainName);
};


//软件登陆
struct sLogin
{
#define LOGIN_LOGINUSER		"LoginUser"
#define LOGIN_LOGINTIME		"LoginTime"
#define LOGIN_LOGINTYPE		"LoginType"

	enum nLoginType
	{
		//用户主类型Id,用户id,电话号码,微信等的id都可以在登录界面输入登录
		nLoginType_Normal = 0,	//普通输入, 登录界面输入 

		nLoginType_Card,		//刷卡登录(此时不需要密码, 不需要保留登录记录)

		//nLoginType_Database,	//从数据库间接获取登录信息

		nLoginType_AutoLogin,	//自动登录

		//跨设备远程登录
		nLoginType_CrossDevice = nLoginType_Card,

		nLoginType_PhoneVerifyCode,//手机验证码(此时不需要密码, 需要手机验证码)
	};

	sLoginUser		loginUser;		//登录用户
	QDateTime		loginTime;		//登陆时间
	nLoginType		iLoginType;		//登录类型
};

#endif // _CLOGIN_H_



