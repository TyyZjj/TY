#ifndef _CLOGINUI_H_
#define _CLOGINUI_H_
#include <QList>
#include <QStack>
#include <QDialog>
#include "CLogin.h"
namespace Ui { class CLoginDialog; };


enum nLoginUiType
{
	nLoginUiNormal = 0,			//普通登录
	nLoginUiTechnicianInput,	//技术员或以上权限输入
	nLoginUiEngineerInput,		//工程师或以上权限输入
	nLoginUiAdminInput,			//管理员或以上权限输入
	nLoginUiNewUserInput,		//添加新用户
	nLoginUiEditUserInput,		//编辑用户
	nLoginUiInputUserInput,		//输入用户名
	nLoginUiDelUserInput,		//删除用户
	nLoginUiTypeEnd,
};


class LOGINMANAGER_EXPORT CLoginUI : public QDialog
{
	Q_OBJECT

public:
	CLoginUI(nLoginUiType type = nLoginUiType::nLoginUiNormal, 
		QWidget *parent = Q_NULLPTR);
	~CLoginUI();

	void setLoginUser(const sLoginUser &loginUser);
	void setLogin(const QString &strUser, const QString &strPassword);
	//******Taoyuyu***********************   
	// 日期: 2023/08/25
	// 功能: 填充用户id(主要用于刷卡登录时, 卡片绑定) 注意和addUserId的区别
	// 返回: void
	// 参数: const sUserId & userId: 
	//************************************
	bool fillUserId(const sUserId &userId);

	//顶层的登录类型, 用于直接在顶层的窗口内输入数据
	//可以用于获取顶层窗口, 并往里面填充数据
	static nLoginUiType topLoginType();
	static CLoginUI* topLoginUi();

public Q_SLOTS:
	void on_okBtn_clicked();

	void on_cancelBtn_clicked();

	void on_newUserBtn_clicked();
	void on_editUserBtn_clicked();
	void on_delUserBtn_clicked();

	void on_addUserIdBtn_clicked();
	void on_editDBBtn_clicked();

private:
	void showUserNameLayout(bool bShow = true);
	void showPasswordLayout(bool bShow = true);
	void showPasswordAgainLayout(bool bShow = true);
	void showAuthorityLayout(bool bShow = true);
	void showOtherNameLayout(bool bShow = true);
	void showTipsLayout(bool bShow = true);
	void showShowLayout(bool bShow = true);
	void showOperateLayout(bool bShow = true);
	void showBtnLayout(bool bShow = true);

	//******Taoyuyu***********************   
	// 日期: 2023/08/28
	// 功能: 新建并添加一个用户id
	// 返回: void
	// 参数: const sUserId & userId: 
	//************************************
	void addUserId(const sUserId &userId);
	//******Taoyuyu***********************   
	// 日期: 2023/08/25
	// 功能: 获取除主用户类型、自动用户类型之外的其他配置的用户id
	// 返回: QT_NAMESPACE::QList<sUserId>
	//************************************
	QList<sUserId> getOtherUserId();

	//******Taoyuyu***********************   
	// 日期: 2023/08/25
	// 功能: 根据当前的登录界面中的填充信息, 获取登录用户信息
	// 返回: bool
	// 参数: sLoginUser & loginUser: 
	//************************************
	bool getLoginUser(sLoginUser &loginUser);
private:
	Ui::CLoginDialog *ui;

	nLoginUiType m_iType;

	sLoginUser mLoginUser;	//当前的登录用户

	//登录类型堆栈
	static QStack<nLoginUiType> m_stackLoginType;
	static QStack<CLoginUI*> m_stackLoginUi;

	//sLoginUser mLoginUser;	//当前的登录用户
};
#endif // _CLOGINUI_H_