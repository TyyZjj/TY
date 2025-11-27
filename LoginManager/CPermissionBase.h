#ifndef _PERMISSIONBASE_H_
#define _PERMISSIONBASE_H_
#include <QObject>

#include "CPermission.h"
#include "CLogin.h"

#ifndef EnableWidget
#define EnableWidget(wdg, enable)\
if (wdg && enable != wdg->isEnabled())wdg->setEnabled(enable);
#endif

#ifndef PermissionEnable
#define PermissionEnable(iDUGL, wdg)\
if (hasPermission(iDUGL)){EnableWidget(wdg, true)}
#endif

#ifndef PermissionUnable
#define PermissionUnable(iDUGL, wdg)\
if (!hasPermission(iDUGL)){EnableWidget(wdg, false)}
#endif

class CPermissionBase;
class CLoginManagerObj : public QObject
{
	Q_OBJECT
public:
	CLoginManagerObj(CPermissionBase* pBase);
	~CLoginManagerObj() { m_pBase = nullptr; }
private:
	CPermissionBase* m_pBase = nullptr;
};


//权限的基础类, 如果某个画面/功能需要应用权限, 需要继承此类
class LOGINMANAGER_EXPORT CPermissionBase
{
	friend class CLoginManagerObj;
public:
	CPermissionBase();
	virtual ~CPermissionBase();
	//virtual QObject* object() = 0;
	
	virtual CPSet defaultCPSet();

protected:
	virtual void login(const QString& strAutoUserId,
		const QString& strShowUser,
		const QString strResult,
		QString strUserGroup) = 0;

	//执行某项动态操作时, 需要动态判断一堆Widget是否满足权限, 仅适用于所有用户均为默认用户组时
	//当存在一堆控件需要操作时, 推荐使用此同名函数
	virtual void enableLoginWidget(){}

	//执行某项动态操作时, 需要动态判断是否满足权限, 仅适用于所有用户均为默认用户组时
	//iNeeded:猜想操作需要的权限
	bool hasPermission(DUGL iNeeded);
	//预留 当可以动态创建用户组之后, 应该通过此接口+m_defaultCPSet来判断当前操作是否满足权限
	bool hasPermission(QString strPermissionName) = delete;

	//自动登出
	//如果需要自动登出, 调用此函数, 软件会设定时间自动登出时间, 并在时间到达后自动登出
	//如果要求有自动登出功能, 但是人员操作时不自动登出, 需要在敏感界面识别到人员操作(如鼠标移动)时，调用此函数, 会自动刷新登出时间
	void Active();
	
private:
	CLoginManagerObj* m_pLoginManager = nullptr;
	CPSet m_defaultCPSet;	
	CPSet m_cpSet;

	DUGL m_iDUGL = DUGL::nUserLevel_Invalid;//当前登录的用户的DUGL
};


#endif

