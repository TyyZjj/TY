#ifndef _CLOGINUI_H_
#define _CLOGINUI_H_
#include <QList>
#include <QStack>
#include <QDialog>
#include "CLogin.h"
namespace Ui { class CLoginDialog; };


enum nLoginUiType
{
	nLoginUiNormal = 0,			//��ͨ��¼
	nLoginUiTechnicianInput,	//����Ա������Ȩ������
	nLoginUiEngineerInput,		//����ʦ������Ȩ������
	nLoginUiAdminInput,			//����Ա������Ȩ������
	nLoginUiNewUserInput,		//������û�
	nLoginUiEditUserInput,		//�༭�û�
	nLoginUiInputUserInput,		//�����û���
	nLoginUiDelUserInput,		//ɾ���û�
	nLoginUiTypeEnd,
};


class CLoginUI : public QDialog
{
	Q_OBJECT

public:
	CLoginUI(nLoginUiType type = nLoginUiType::nLoginUiNormal, 
		QWidget *parent = Q_NULLPTR);
	~CLoginUI();

	void setLoginUser(const sLoginUser &loginUser);
	void setLogin(const QString &strUser, const QString &strPassword);
	//******Taoyuyu***********************   
	// ����: 2023/08/25
	// ����: ����û�id(��Ҫ����ˢ����¼ʱ, ��Ƭ��) ע���addUserId������
	// ����: void
	// ����: const sUserId & userId: 
	//************************************
	bool fillUserId(const sUserId &userId);

	//����ĵ�¼����, ����ֱ���ڶ���Ĵ�������������
	//�������ڻ�ȡ���㴰��, ���������������
	static nLoginUiType topLoginType();
	static CLoginUI* topLoginUi();

public Q_SLOTS:
	void on_okBtn_clicked();

	void on_cancelBtn_clicked();

	void on_newUserBtn_clicked();
	void on_editUserBtn_clicked();
	void on_delUserBtn_clicked();

	void on_addUserIdBtn_clicked();

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
	// ����: 2023/08/28
	// ����: �½������һ���û�id
	// ����: void
	// ����: const sUserId & userId: 
	//************************************
	void addUserId(const sUserId &userId);
	//******Taoyuyu***********************   
	// ����: 2023/08/25
	// ����: ��ȡ�����û����͡��Զ��û�����֮����������õ��û�id
	// ����: QT_NAMESPACE::QList<sUserId>
	//************************************
	QList<sUserId> getOtherUserId();

	//******Taoyuyu***********************   
	// ����: 2023/08/25
	// ����: ���ݵ�ǰ�ĵ�¼�����е������Ϣ, ��ȡ��¼�û���Ϣ
	// ����: bool
	// ����: sLoginUser & loginUser: 
	//************************************
	bool getLoginUser(sLoginUser &loginUser);
private:
	Ui::CLoginDialog *ui;

	nLoginUiType m_iType;

	sLoginUser mLoginUser;	//��ǰ�ĵ�¼�û�

	//��¼���Ͷ�ջ
	static QStack<nLoginUiType> m_stackLoginType;
	static QStack<CLoginUI*> m_stackLoginUi;

	//sLoginUser mLoginUser;	//��ǰ�ĵ�¼�û�
};
#endif // _CLOGINUI_H_