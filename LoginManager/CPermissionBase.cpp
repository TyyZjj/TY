#include <QMetaObject>
#include "CPermissionBase.h"
#include "CLoginManager.h"

CLoginManagerObj::CLoginManagerObj(CPermissionBase* pBase)
	:m_pBase(pBase)
{
	connect(CLoginManager::GetKernel(), &CLoginManager::signalLogin, this, 
		[&](const QString& strAutoUserId,
		const QString& strShowUser,
		const QString strResult,
			QString strUserGroup) {
				if (m_pBase)
				{
					m_pBase->m_iDUGL = (DUGL)getDefaultUserGroupLevel(strUserGroup);
					m_pBase->login(strAutoUserId, strShowUser, strResult, strUserGroup);
				}
		});
}

CPermissionBase::CPermissionBase()
{
	m_pLoginManager = new CLoginManagerObj(this);
	sLogin login;
	bool ret = CLoginManager::GetKernel()->CurrentLoginUser(login);
	if (ret)
	{
		m_iDUGL = (DUGL)getDefaultUserGroupLevel(login.loginUser.getUserGroup());
	}

	//再主动触发一下, 因为所有继承CPermissionBase的都要重写login, 主动触发, 防止某个画面忘记主动触发
	//在基类构造完成后, 派生类构造函数执行过程中, vtable已经是派生类的
	//如果系统负载高导致延迟, 可能导致触发定时器但是还未进入派生类的构造函数中, 可能会崩溃
	QTimer::singleShot(1, [&]() {
		sLogin login;
		bool ret = CLoginManager::GetKernel()->CurrentLoginUser(login);
		if (ret)
		{
			this->login(login.loginUser.getAutoUserId(),
				login.loginUser.getShowUser(), "",
				login.loginUser.getUserGroup());
		}
		defaultCPSet();
		});
}

CPermissionBase::~CPermissionBase()
{
	if (m_pLoginManager)
	{
		delete m_pLoginManager;
		m_pLoginManager = nullptr;
	}
}

CPSet CPermissionBase::defaultCPSet()
{
	return m_defaultCPSet;
}

bool CPermissionBase::hasPermission(DUGL iNeeded)
{
	return m_iDUGL >= iNeeded;
}

void CPermissionBase::Active()
{
	CLoginManager::GetKernel()->Active();
}

