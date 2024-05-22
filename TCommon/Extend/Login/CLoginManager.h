#ifndef _CLOGINMANAGER_H_
#define _CLOGINMANAGER_H_
#include <QTimer>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMutexLocker>
//#include "libpublic.h"

#include "CLogin.h"


#define ShowLoginRecordNum	6//��ʾ��¼��¼������


class CLoginManager : public QObject
{
	Q_OBJECT
public:
	//�û���¼��Ϣ�洢��ʽ
	enum nUserLoginInfoStorageMode {
		StorageIn_IniFile = 0,	//������ini�ļ���
		StorageIn_DB,			//
		StorageIn_RemoteUrl,	//Զ��url
	};
	static CLoginManager * GetKernel();
	static void DestroyKernel();

	//******Taoyuyu***********************   
	// ����: 2023/08/14
	// ����: ��ȡ�û���¼Ȩ��
	// ����: void
	// ����: const QString & strUserId
	// ����: int & iAuthority : ����sLoginUser::nUserAuthority
	//************************************
	void Authority(const QString &strUserId,
		int &iAuthority);

	//******Taoyuyu***********************   
	// ����: 2023/08/14
	// ����: ��ǰ���еĵ�¼�û�(m_mapLogin)���Ƿ�����û���Ϣ
	// ����: bool
	// ����: const sLoginUser & loginUser
	//************************************
	bool Contains(const sLoginUser &loginUser);

	//******Taoyuyu***********************   
	// ����: 2023/08/22
	// ����: У���û����������Ƿ���ȷ
	// ����: bool
	// ����: const QString & strUserId: 
	// ����: const QString & strPassword: 
	//************************************
	bool Check(const QString &strUserId,
		const QString &strPassword);

	//******Taoyuyu***********************   
	// ����: 2023/08/22
	// ����: �������û�
	// ����: bool
	// ����: const sLoginUser & newLoginUser: ���û�����
	//************************************
	bool Save(const sLoginUser &newLoginUser);

	/*
	ɾ���û���ĳ��id
	���������û�id���û�����id, ��ôֱ��ɾ�������û���¼��Ϣ
	���������û�id���Զ��û�id, ��ô����ʧ��
	���������û�id�������û�id, ��ôɾ�����id
	*/
	bool Delete(const QString &strUserId);
	bool Delete(const sUserId &userId);
	bool DeleteUser(const QString &strUserId);

	/*
	�����¼����
	strUserId: ��¼���û�id
	strLoginVoucher: ��¼ƾ��, ����������, �������ֻ���֤��, �����ӵ�¼���Ͷ���
	iLoginType: ��½����
	*/
	bool Login(const QString &strUserId,
		const QString &strLoginVoucher,
		sLogin::nLoginType iLoginType = sLogin::nLoginType::nLoginType_Normal);

	//******Taoyuyu***********************   
	// ����: 2023/08/22
	// ����: ��ǰ�ĵ�¼�û�
	// ����: bool
	// ����: sLogin & login: 
	//************************************
	bool CurrentLoginUser(sLogin &login);

	//�����һ�ε�¼�Ĳ���Ա�˺ŵ�¼
	bool LastOperatorLogin();

	//��ȡ��¼��¼�е��û����б�(����¼ʱ����������)
	QList<QString> GetLoginRecordUserNameList(int iCount = ShowLoginRecordNum);

	//����Ա������Ȩ���û����ֻ�Ծ
	//����: ����ԱȨ�޵�¼��, �������һֱ�ڲ���, ����ô˽ӿ�, ���ֵ�¼״̬
	void Active();

	//�û���¼��Ϣjson
	QJsonObject toJson();
	bool fromeJson(const QJsonObject &json);

	bool Save();
	bool Load();

	QString getLastInfo() { return m_strLastInfo; }
	QString getLastError() { return m_strLastError; }

	//******Taoyuyu***********************   
	// ����: 2023/08/14
	// ����: ��ȡ�û�id�ĵ�¼��Ϣ
	// ����: bool
	// ����: const QString & strUserId: �û�id
	// ����: sLogin & login: �û��ĵ�¼��Ϣ
	//************************************
	bool GetLogin(const QString &strUserId, sLogin &login);

Q_SIGNALS:
	void signalLogin(const QString &strUser,
		const QString strResult,
		int authority = (int)sLoginUser::nUserAuthority::nUserLevel_Invalid);
	//��¼�û��б�
	void signalLoginUserList(QJsonObject json);

private:
	CLoginManager() :m_iUserLoginInfoStorageMode(nUserLoginInfoStorageMode::StorageIn_IniFile),
		m_iLoginFailNum(0), m_mutex(QMutex::RecursionMode::Recursive), m_pTimer(nullptr) {
		Load();
		LoadLoginRecord();
		OnTimer();
	};
	~CLoginManager() {
		SaveLoginRecord();
	};
	
	bool SaveLoginRecord();
	bool LoadLoginRecord();//���ص�¼��־

	//��ӳ����û�
	void AddSuperUser();

	//���Լ���Ա������Ȩ�޵�¼ʱ, �Զ�������ʱ��, ��ʱ�Զ��ǳ�������Ա�˺�
	void OnTimer();

private:
	static CLoginManager* m_pInstance;

	nUserLoginInfoStorageMode m_iUserLoginInfoStorageMode;

	QString			m_strLastInfo;		//������ú����ɹ�, �����ȡһЩ��Ϣ, ѯ�ʴ˶�������
	QString			m_strLastError;


	QDateTime		m_lastFailTime;		//�ϴε�¼ʧ��ʱ��, ��¼ʧ�ܳ������κ�, ��Ҫ�ȴ�60s
	int				m_iLoginFailNum;	//��¼ʧ�ܴ���

	QMutex			m_mutexRecord;
	QString			m_strLastLoginOperator;			//���һ�ε�¼�Ĳ���Ա, ���ÿ�ο���ʱ, ��ʹ�ô��˺�ֱ�ӵ�¼
	QMap<QDateTime, QString>	m_mapLoginRecord;	//��¼��¼(�������ڼ�¼)

	QMutex						m_mutex;
	QMap<QString, sLogin>		m_mapLogin;			//ע��ĵ�¼�û� �û��б�	QMap<�Զ��û�id, sLoginUser>

	QMutex			m_mutexPlanLogoutTime;
	QDateTime		m_planLogoutTime;	//�ƻ��ǳ�ʱ��
	QTimer *		m_pTimer; //��ʱ��, ���Լ���Ա������Ȩ�޵�¼ʱ, �Զ�������ʱ��, ��ʱ�Զ��ǳ�������Ա�˺�

};

#endif // _CLOGINMANAGER_H_