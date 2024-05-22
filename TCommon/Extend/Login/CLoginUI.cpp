#include <QDebug>
#include "CLoginUI.h"
#include "ui_CLoginUI.h"
#include "CLoginManager.h"

#define USERIDTYPEBOX_COL		1
#define USERIDEDIT_COL		3

#define ROLE_AUTO_USER_ID	(Qt::UserRole + 1)


CLoginUI::CLoginUI(nLoginUiType type, QWidget *parent)
	: QDialog(parent)
{
	ui = new Ui::CLoginDialog();
	ui->setupUi(this);

	QList<QString> lstLoginRecord = CLoginManager::GetKernel()->GetLoginRecordUserNameList();
	ui->userBox->addItems(lstLoginRecord);

	if (type == nLoginUiType::nLoginUiTechnicianInput)
	{
		m_iType = nLoginUiType::nLoginUiTechnicianInput;
		this->setWindowTitle(QString::fromLocal8Bit("技术员登录"));
		showPasswordAgainLayout(false);
		showAuthorityLayout(false);
		showOtherNameLayout(false);
		showTipsLayout(false);
		showShowLayout(false);
		showOperateLayout(false);
	}
	else if (type == nLoginUiType::nLoginUiEngineerInput)
	{
		m_iType = nLoginUiType::nLoginUiEngineerInput;
		this->setWindowTitle(QString::fromLocal8Bit("工程师登录"));
		
		showPasswordAgainLayout(false);
		showAuthorityLayout(false);
		showOtherNameLayout(false);
		showTipsLayout(false);
		showShowLayout(false);
		showOperateLayout(false);
	}
	else if (type == nLoginUiType::nLoginUiAdminInput)
	{
		m_iType = nLoginUiType::nLoginUiAdminInput;
		this->setWindowTitle(QString::fromLocal8Bit("管理员登录"));
		
		showPasswordAgainLayout(false);
		showAuthorityLayout(false);
		showOtherNameLayout(false);
		showTipsLayout(false);
		showShowLayout(false);
		showOperateLayout(false);
	}
	else if (type == nLoginUiType::nLoginUiNewUserInput)
	{
		m_iType = nLoginUiType::nLoginUiNewUserInput;
		this->setWindowTitle(QString::fromLocal8Bit("添加用户"));
		
		showShowLayout(false);
		showOperateLayout(false);
		
		ui->authorityBox->addItems(QStringList() 
			<< sLoginUser::getAuthorityName(sLoginUser::nUserAuthority::nUserLevel_Operator)
			<< sLoginUser::getAuthorityName(sLoginUser::nUserAuthority::nUserLevel_Technician)
			<< sLoginUser::getAuthorityName(sLoginUser::nUserAuthority::nUserLevel_Engineer)
			<< sLoginUser::getAuthorityName(sLoginUser::nUserAuthority::nUserLevel_Admin));
		ui->authorityBox->setCurrentIndex(0);
	}
	else if (type == nLoginUiType::nLoginUiEditUserInput)
	{
		m_iType = nLoginUiType::nLoginUiEditUserInput;
		this->setWindowTitle(QString::fromLocal8Bit("编辑用户"));
		
		showShowLayout(false);
		showOperateLayout(false);
		
		ui->authorityBox->addItems(QStringList()
			<< sLoginUser::getAuthorityName(sLoginUser::nUserAuthority::nUserLevel_Operator)
			<< sLoginUser::getAuthorityName(sLoginUser::nUserAuthority::nUserLevel_Technician)
			<< sLoginUser::getAuthorityName(sLoginUser::nUserAuthority::nUserLevel_Engineer)
			<< sLoginUser::getAuthorityName(sLoginUser::nUserAuthority::nUserLevel_Admin));
		ui->authorityBox->setCurrentIndex(0);
	}
	else if (type == nLoginUiType::nLoginUiInputUserInput)
	{
		m_iType = nLoginUiType::nLoginUiInputUserInput;
		this->setWindowTitle(QString::fromLocal8Bit("输入用户名"));

		showPasswordLayout(false);
		showPasswordAgainLayout(false);
		showAuthorityLayout(false);
		showOtherNameLayout(false);
		showTipsLayout(false);
		showShowLayout(false);
		showOperateLayout(false);
	}
	else if (type == nLoginUiType::nLoginUiDelUserInput)
	{
		m_iType = nLoginUiType::nLoginUiDelUserInput;
		this->setWindowTitle(QString::fromLocal8Bit("删除用户"));

		showPasswordLayout(false);
		showPasswordAgainLayout(false);
		showAuthorityLayout(false);
		showOtherNameLayout(false);
		showTipsLayout(false);
		showShowLayout(false);
		showOperateLayout(false);
	}
	else
	{
		m_iType = nLoginUiType::nLoginUiNormal;
		this->setWindowTitle(QString::fromLocal8Bit("登录"));

		showPasswordAgainLayout(false);
		showAuthorityLayout(false);
		showOtherNameLayout(false);
		showTipsLayout(false);
		showShowLayout(false);
	}

	m_stackLoginType.push(type);
	m_stackLoginUi.push(this);

	this->adjustSize();
}

CLoginUI::~CLoginUI()
{
	delete ui;
	m_stackLoginType.pop();
	m_stackLoginUi.pop();
}

void CLoginUI::setLoginUser(const sLoginUser &loginUser)
{
	ui->userBox->setEditText(loginUser.getUserMainName());
	ui->passwordEdit->setText(loginUser.getPassword());
	ui->passwordEdit2->setText(loginUser.getPassword());
	ui->authorityBox->setCurrentText(sLoginUser::getAuthorityName(loginUser.getAuthority()));

	bool isFirst(true);
	QList<sUserId> lstUserId = loginUser.getUserIdList();
	for (int index = 0; index < lstUserId.count(); index++)
	{
		const sUserId &userId = lstUserId.at(index);
		if (userId.iUserIdType == sUserId::nUserIdType::nUserIdType_UserMainName ||
			userId.iUserIdType == sUserId::nUserIdType::nUserIdType_UserAutoId)
			continue;

		if (isFirst)
		{
			isFirst = false;
			ui->newUserIdTypeBox->setCurrentText(userId.getUserIdTypeName());
			ui->newUserIdEdit->setText(userId.strUserId);
		}
		else
		{
			addUserId(userId);
		}
	}
	ui->passwordTipsEdit->setText(loginUser.getPasswordTips());

	mLoginUser = loginUser;
}

void CLoginUI::setLogin(const QString &strUser, const QString &strPassword)
{
	ui->userBox->setEditText(strUser);
	ui->passwordEdit->setText(strPassword);
}

bool CLoginUI::fillUserId(const sUserId &userId)
{
	QLineEdit* candidateUserIdEdit(nullptr);//候选的
	int iCount = ui->vLayout_otherName->count();
	for (int index = 0; index < iCount; index++)
	{
		QHBoxLayout* layout = qobject_cast<QHBoxLayout*>(ui->vLayout_otherName->itemAt(index)->layout());
		QComboBox* userIdTypeBox = qobject_cast<QComboBox*>(layout->itemAt(USERIDTYPEBOX_COL)->widget());
		QLineEdit* userIdEdit = qobject_cast<QLineEdit*>(layout->itemAt(USERIDEDIT_COL)->widget());
		sUserId::nUserIdType iUserIdType = sUserId::getUserIdType(userIdTypeBox->currentText());
		if (iUserIdType == userId.iUserIdType)
		{
			if (userIdEdit->text().isEmpty())
			{
				userIdEdit->setText(userId.strUserId);
				return true;
			}
			else
				candidateUserIdEdit = userIdEdit;
		}
	}
	if (candidateUserIdEdit != nullptr)
	{
		candidateUserIdEdit->setText(userId.strUserId);
		return true;
	}
	return false;
}

nLoginUiType CLoginUI::topLoginType()
{
	return m_stackLoginType.top();
}

CLoginUI* CLoginUI::topLoginUi()
{
	return m_stackLoginUi.top();
}

void CLoginUI::on_okBtn_clicked()
{
	QString strUser = ui->userBox->currentText();
	QString strPassword = ui->passwordEdit->text();
	if (m_iType != nLoginUiType::nLoginUiDelUserInput &&
		m_iType != nLoginUiType::nLoginUiInputUserInput)
	{
		if (strUser.isEmpty() || strPassword.isEmpty())
		{
			showShowLayout(true);
			ui->showLbl->setText(QString::fromLocal8Bit("用户名/密码不可为空!"));
			return;
		}
	}
	
	if (m_iType == nLoginUiType::nLoginUiNormal)
	{
		bool ret = CLoginManager::GetKernel()->Login(strUser, strPassword);
		if (ret)
		{
			ui->showLbl->clear();
			//showShowLayout(false);
			qDebug() << CLoginManager::GetKernel()->getLastInfo();
			accept();
		}
		else
		{
			ui->showLbl->setText(CLoginManager::GetKernel()->getLastError());
			showShowLayout(true);
			return;
		}
	}
	else if (m_iType == nLoginUiType::nLoginUiTechnicianInput)
	{
		int authority;
		CLoginManager::GetKernel()->Authority(strUser, authority);
		if (authority >= sLoginUser::nUserAuthority::nUserLevel_Technician)
		{
			bool ret = CLoginManager::GetKernel()->Check(strUser, strPassword);
			if (ret)
			{
				CLoginManager::GetKernel()->Login(strUser, strPassword);
	
				ui->showLbl->clear();
				//showShowLayout(false);
				qDebug() << CLoginManager::GetKernel()->getLastInfo();
				accept();
			}
			else
			{
				ui->showLbl->setText(CLoginManager::GetKernel()->getLastError());
				showShowLayout(true);
				return;
			}
		}
		else if (authority > sLoginUser::nUserAuthority::nUserLevel_Invalid)
		{
			ui->showLbl->setText(QString::fromLocal8Bit("%1权限不足!").arg(strUser));
			showShowLayout(true);
			return;
		}
		else
		{
			ui->showLbl->setText(QString::fromLocal8Bit("不存在当前用户!"));
			showShowLayout(true);
			return;
		}
	}
	else if (m_iType == nLoginUiType::nLoginUiEngineerInput)
	{
		int authority;
		CLoginManager::GetKernel()->Authority(strUser, authority);
		if (authority >= sLoginUser::nUserAuthority::nUserLevel_Engineer)
		{
			QString strResult;
			bool ret = CLoginManager::GetKernel()->Check(strUser, strPassword);
			if (ret)
			{
				CLoginManager::GetKernel()->Login(strUser, strPassword);

				ui->showLbl->clear();
				//showShowLayout(false);
				qDebug() << CLoginManager::GetKernel()->getLastInfo();
				accept();
			}
			else
			{
				ui->showLbl->setText(CLoginManager::GetKernel()->getLastError());
				showShowLayout(true);
				return;
			}
		}
		else if (authority > sLoginUser::nUserAuthority::nUserLevel_Invalid)
		{
			ui->showLbl->setText(QString::fromLocal8Bit("%1权限不足!").arg(strUser));
			showShowLayout(true);
			return;
		}
		else
		{
			ui->showLbl->setText(QString::fromLocal8Bit("不存在当前用户!"));
			showShowLayout(true);
			return;
		}
	}
	else if (m_iType == nLoginUiType::nLoginUiAdminInput)
	{
		int authority;
		CLoginManager::GetKernel()->Authority(strUser, authority);
		if (authority >= sLoginUser::nUserAuthority::nUserLevel_Admin)
		{
			bool ret = CLoginManager::GetKernel()->Check(strUser, strPassword);
			if (ret)
			{
				ui->showLbl->clear();
				qDebug() << QString::fromLocal8Bit("用户 %1 验证成功.").arg(strUser);
				accept();
			}
			else
			{
				ui->showLbl->setText(CLoginManager::GetKernel()->getLastError());
				showShowLayout(true);
				return;
			}
		}
		else if (authority > sLoginUser::nUserAuthority::nUserLevel_Invalid)
		{
			ui->showLbl->setText(QString::fromLocal8Bit("%1权限不足!").arg(strUser));
			showShowLayout(true);
			return;
		}
		else
		{
			ui->showLbl->setText(QString::fromLocal8Bit("不存在当前用户!"));
			showShowLayout(true);
			return;
		}
	}
	else if (m_iType == nLoginUiType::nLoginUiNewUserInput)
	{
		QString strPassword2 = ui->passwordEdit2->text();
		if (strPassword != strPassword2)
		{
			ui->showLbl->setText(QString::fromLocal8Bit("两次密码输入不一致."));
			showShowLayout(true);
			return;
		}
		QString strPasswordTips = ui->passwordTipsEdit->toPlainText();
		if (strPasswordTips == strPassword ||
			strPasswordTips.contains(strPassword))
		{
			ui->showLbl->setText(QString::fromLocal8Bit("密码提示不能包含密码信息."));
			showShowLayout(true);
			return;
		}
		QString strAuthority = ui->authorityBox->currentText();
		sLoginUser::nUserAuthority authority = (sLoginUser::nUserAuthority)sLoginUser::getAuthority(strAuthority);

		sLoginUser loginUser;
		loginUser.addUserId(strUser);
		loginUser.setShowUser(strUser);
		loginUser.setPassword(strPassword, strPasswordTips);
		loginUser.setAuthority(authority);
		QList<sUserId> lstUserId = getOtherUserId();
		for (auto &userId : lstUserId)
		{
			bool ret = loginUser.addUserId(userId.strUserId, userId.iUserIdType);
			if (!ret)
			{
				ui->showLbl->setText(loginUser.getLastError());
				showShowLayout(true);
				return;
			}
		}
		if (!loginUser.isValid())
		{
			ui->showLbl->setText(loginUser.getLastError());
			showShowLayout(true);
			return;
		}

		bool ret = CLoginManager::GetKernel()->Save(loginUser);
		if (ret)
		{
			QString strInfo = QString::fromLocal8Bit("用户 %1 添加成功.").arg(strUser);
			qDebug() << strInfo;
			//QMessageBox::information(nullptr, QString::fromLocal8Bit("提示"), strInfo);
			accept();
		}
		else
		{
			ui->showLbl->setText(CLoginManager::GetKernel()->getLastError());
			showShowLayout(true);
			return;
		}
	}
	else if (m_iType == nLoginUiType::nLoginUiEditUserInput)
	{
		QString strPassword2 = ui->passwordEdit2->text();
		if (strPassword != strPassword2)
		{
			ui->showLbl->setText(QString::fromLocal8Bit("两次密码输入不一致."));
			showShowLayout(true);
			return;
		}
		QString strPasswordTips = ui->passwordTipsEdit->toPlainText();
		if (strPasswordTips == strPassword ||
			strPasswordTips.contains(strPassword))
		{
			ui->showLbl->setText(QString::fromLocal8Bit("密码提示不能包含密码信息."));
			showShowLayout(true);
			return;
		}

		sLoginUser loginUser;
		bool ret = getLoginUser(loginUser);
		if (!ret)
			return;

		ret = CLoginManager::GetKernel()->Delete(mLoginUser.getUserMainName());
		if (!ret)
		{
			ui->showLbl->setText(CLoginManager::GetKernel()->getLastError());
			showShowLayout(true);
			return;
		}
		ret = CLoginManager::GetKernel()->Save(loginUser);
		if (!ret)
		{
			ui->showLbl->setText(CLoginManager::GetKernel()->getLastError());
			showShowLayout(true);
			return;
		}
		accept();
	}
	else if (m_iType == nLoginUiType::nLoginUiInputUserInput)
	{
		sLogin login;
		bool ret = CLoginManager::GetKernel()->GetLogin(strUser, login);
		if (!ret)
		{
			ui->showLbl->setText(CLoginManager::GetKernel()->getLastError());
			showShowLayout(true);
		}
		int iAuthority = login.loginUser.getAuthority();
		if (iAuthority >= sLoginUser::nUserAuthority::nUserLevel_Develop)
		{
			ui->showLbl->setText(QString::fromLocal8Bit("权限不足."));
			showShowLayout(true);
			return;
		}
		if (iAuthority <= sLoginUser::nUserAuthority::nUserLevel_Invalid)
		{
			ui->showLbl->setText(CLoginManager::GetKernel()->getLastError());
			showShowLayout(true);
			return;
		}

		CLoginUI loginUi(nLoginUiType::nLoginUiEditUserInput, this);
		loginUi.setLoginUser(login.loginUser);
		if (loginUi.exec() == QDialog::DialogCode::Accepted)
			accept();
		else
			reject();
	}
	else if (m_iType == nLoginUiType::nLoginUiDelUserInput)
	{	
		QString strResult;
		bool ret = CLoginManager::GetKernel()->Delete(strUser);
		if (ret)
		{
			//QMessageBox::information(nullptr, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("删除成功."));
			accept();
		}
		else
		{
			ui->showLbl->setText(CLoginManager::GetKernel()->getLastError());
			showShowLayout(true);
			return;
		}
	}
	else
	{
		ui->showLbl->setText(QString::fromLocal8Bit("无效的类型."));
		showShowLayout(true);
		return;
	}
}

void CLoginUI::on_cancelBtn_clicked()
{
	reject();
}

void CLoginUI::on_newUserBtn_clicked()
{
	CLoginUI loginUi(nLoginUiType::nLoginUiAdminInput, this);
	if (loginUi.exec() == QDialog::DialogCode::Accepted)
	{
		CLoginUI loginUi2(nLoginUiType::nLoginUiNewUserInput, this);
		loginUi2.exec();
	}
}

void CLoginUI::on_editUserBtn_clicked()
{
	CLoginUI loginUi(nLoginUiType::nLoginUiAdminInput, this);
	if (loginUi.exec() == QDialog::DialogCode::Accepted)
	{
		CLoginUI loginUi2(nLoginUiType::nLoginUiInputUserInput, this);
		loginUi2.exec();
	}
}

void CLoginUI::on_delUserBtn_clicked()
{
	CLoginUI loginUi(nLoginUiType::nLoginUiAdminInput, this);
	if (loginUi.exec() == QDialog::DialogCode::Accepted)
	{
		CLoginUI loginUi2(nLoginUiType::nLoginUiDelUserInput, this);
		loginUi2.exec();
	}
}

void CLoginUI::on_addUserIdBtn_clicked()
{
	int iCount = ui->vLayout_otherName->count();

	QHBoxLayout* layout = new QHBoxLayout();
	layout->setObjectName(QString::fromUtf8("hLayout_otherName%1").arg(iCount));
	
	QSpacerItem* hSpacer1 = new QSpacerItem(30, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);
	layout->addItem(hSpacer1);

	QComboBox* userIdTypeBox = new QComboBox(ui->borderFrame);
	userIdTypeBox->addItems(QStringList() << QString::fromLocal8Bit("卡号")
		<< QString::fromLocal8Bit("工号")
		<< QString::fromLocal8Bit("手机")
		<< QString::fromLocal8Bit("邮箱")
		<< QString::fromLocal8Bit("QQ")
		<< QString::fromLocal8Bit("微信"));
	userIdTypeBox->setObjectName(QString::fromUtf8("newUserIdTypeBox%1").arg(iCount));
	userIdTypeBox->setMinimumSize(QSize(70, 30));
	userIdTypeBox->setMaximumSize(QSize(70, 30));
	layout->addWidget(userIdTypeBox);

	QSpacerItem* hSpacer2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	layout->addItem(hSpacer2);

	QLineEdit *userIdEdit = new QLineEdit(ui->borderFrame);
	userIdEdit->setObjectName(QString::fromUtf8("newUserIdEdit%1").arg(iCount));
	userIdEdit->setMinimumSize(QSize(120, 30));
	userIdEdit->setMaximumSize(QSize(120, 30));
	layout->addWidget(userIdEdit);

	QPushButton *removeUserIdBtn = new QPushButton(ui->borderFrame);
	removeUserIdBtn->setObjectName(QString::fromUtf8("removeUserIdBtn%1").arg(iCount));
	removeUserIdBtn->setText("-");
	removeUserIdBtn->setMinimumSize(QSize(30, 30));
	removeUserIdBtn->setMaximumSize(QSize(30, 30));
	layout->addWidget(removeUserIdBtn);

	QSpacerItem* hSpacer3 = new QSpacerItem(ui->hSpacer_otherName3->sizeHint().width(), 20, QSizePolicy::Fixed, QSizePolicy::Minimum);
	layout->addItem(hSpacer3);

	connect(removeUserIdBtn, &QPushButton::clicked, this, [&,
		layout, hSpacer1, userIdTypeBox, hSpacer2, userIdEdit, removeUserIdBtn, hSpacer3]() {
		
		layout->removeItem(hSpacer1);
		layout->removeItem(hSpacer2);
		layout->removeItem(hSpacer3);
		layout->removeWidget(userIdTypeBox);
		layout->removeWidget(userIdEdit);
		layout->removeWidget(removeUserIdBtn);
		userIdTypeBox->deleteLater();
		userIdEdit->deleteLater();
		removeUserIdBtn->deleteLater();
		ui->vLayout_otherName->removeItem(layout);
		layout->deleteLater();

		this->adjustSize();
	}, Qt::QueuedConnection);

	ui->vLayout_otherName->addLayout(layout);


	

	this->adjustSize();
}

#define HSPACER_DEFAULT_HEIGHT	20
void CLoginUI::showUserNameLayout(bool bShow /*= true*/)
{
	if (bShow)
	{
		ui->userLbl->show();
		ui->userBox->show();
		
		QSize s1 = ui->hSpacer_userName1->sizeHint();
		QSizePolicy sizePolicy1 = ui->hSpacer_userName1->sizePolicy();
		ui->hSpacer_userName1->changeSize(s1.width(), HSPACER_DEFAULT_HEIGHT,
			sizePolicy1.horizontalPolicy(), sizePolicy1.verticalPolicy());

		QSize s2 = ui->hSpacer_userName2->sizeHint();
		QSizePolicy sizePolicy2 = ui->hSpacer_userName2->sizePolicy();
		ui->hSpacer_userName2->changeSize(s2.width(), HSPACER_DEFAULT_HEIGHT,
			sizePolicy2.horizontalPolicy(), sizePolicy2.verticalPolicy());

		QSize s3 = ui->hSpacer_userName3->sizeHint();
		QSizePolicy sizePolicy3 = ui->hSpacer_userName3->sizePolicy();
		ui->hSpacer_userName3->changeSize(s3.width(), HSPACER_DEFAULT_HEIGHT,
			sizePolicy3.horizontalPolicy(), sizePolicy3.verticalPolicy());
	}
	else
	{
		ui->userLbl->hide();
		ui->userBox->hide();
		
		QSize s1 = ui->hSpacer_userName1->sizeHint();
		QSizePolicy sizePolicy1 = ui->hSpacer_userName1->sizePolicy();
		ui->hSpacer_userName1->changeSize(s1.width(), 0,
			sizePolicy1.horizontalPolicy(), sizePolicy1.verticalPolicy());

		QSize s2 = ui->hSpacer_userName2->sizeHint();
		QSizePolicy sizePolicy2 = ui->hSpacer_userName2->sizePolicy();
		ui->hSpacer_userName2->changeSize(s2.width(), 0,
			sizePolicy2.horizontalPolicy(), sizePolicy2.verticalPolicy());

		QSize s3 = ui->hSpacer_userName3->sizeHint();
		QSizePolicy sizePolicy3 = ui->hSpacer_userName3->sizePolicy();
		ui->hSpacer_userName3->changeSize(s3.width(), 0,
			sizePolicy3.horizontalPolicy(), sizePolicy3.verticalPolicy());
	}
}

void CLoginUI::showPasswordLayout(bool bShow /*= true*/)
{
	if (bShow)
	{
		ui->passwordLbl->show(); 
		ui->passwordEdit->show();
		
		QSize s1 = ui->hSpacer_password1->sizeHint();
		QSizePolicy sizePolicy1 = ui->hSpacer_password1->sizePolicy();
		ui->hSpacer_password1->changeSize(s1.width(), HSPACER_DEFAULT_HEIGHT,
			sizePolicy1.horizontalPolicy(), sizePolicy1.verticalPolicy());

		QSize s2 = ui->hSpacer_password2->sizeHint();
		QSizePolicy sizePolicy2 = ui->hSpacer_password2->sizePolicy();
		ui->hSpacer_password2->changeSize(s2.width(), HSPACER_DEFAULT_HEIGHT,
			sizePolicy2.horizontalPolicy(), sizePolicy2.verticalPolicy());

		QSize s3 = ui->hSpacer_password3->sizeHint();
		QSizePolicy sizePolicy3 = ui->hSpacer_password3->sizePolicy();
		ui->hSpacer_password3->changeSize(s3.width(), HSPACER_DEFAULT_HEIGHT,
			sizePolicy3.horizontalPolicy(), sizePolicy3.verticalPolicy());
	}
	else
	{
		ui->passwordLbl->hide();
		ui->passwordEdit->hide();

		QSize s1 = ui->hSpacer_password1->sizeHint();
		QSizePolicy sizePolicy1 = ui->hSpacer_password1->sizePolicy();
		ui->hSpacer_password1->changeSize(s1.width(), 0,
			sizePolicy1.horizontalPolicy(), sizePolicy1.verticalPolicy());

		QSize s2 = ui->hSpacer_password2->sizeHint();
		QSizePolicy sizePolicy2 = ui->hSpacer_password2->sizePolicy();
		ui->hSpacer_password2->changeSize(s2.width(), 0,
			sizePolicy2.horizontalPolicy(), sizePolicy2.verticalPolicy());

		QSize s3 = ui->hSpacer_password3->sizeHint();
		QSizePolicy sizePolicy3 = ui->hSpacer_password3->sizePolicy();
		ui->hSpacer_password3->changeSize(s3.width(), 0,
			sizePolicy2.horizontalPolicy(), sizePolicy3.verticalPolicy());
	}
}

void CLoginUI::showPasswordAgainLayout(bool bShow /*= true*/)
{
	if (bShow)
	{
		ui->passwordLbl2->show();
		ui->passwordEdit2->show();

		QSize s1 = ui->hSpacer_passwordAgain1->sizeHint();
		QSizePolicy sizePolicy1 = ui->hSpacer_passwordAgain1->sizePolicy();
		ui->hSpacer_passwordAgain1->changeSize(s1.width(), HSPACER_DEFAULT_HEIGHT,
			sizePolicy1.horizontalPolicy(), sizePolicy1.verticalPolicy());

		QSize s2 = ui->hSpacer_passwordAgain2->sizeHint();
		QSizePolicy sizePolicy2 = ui->hSpacer_passwordAgain2->sizePolicy();
		ui->hSpacer_passwordAgain2->changeSize(s2.width(), HSPACER_DEFAULT_HEIGHT,
			sizePolicy2.horizontalPolicy(), sizePolicy2.verticalPolicy());

		QSize s3 = ui->hSpacer_passwordAgain3->sizeHint();
		QSizePolicy sizePolicy3 = ui->hSpacer_passwordAgain3->sizePolicy();
		ui->hSpacer_passwordAgain3->changeSize(s3.width(), HSPACER_DEFAULT_HEIGHT,
			sizePolicy3.horizontalPolicy(), sizePolicy3.verticalPolicy());
	}
	else
	{
		ui->passwordLbl2->hide();
		ui->passwordEdit2->hide();

		QSize s1 = ui->hSpacer_passwordAgain1->sizeHint();
		QSizePolicy sizePolicy1 = ui->hSpacer_passwordAgain1->sizePolicy();
		ui->hSpacer_passwordAgain1->changeSize(s1.width(), 0,
			sizePolicy1.horizontalPolicy(), sizePolicy1.verticalPolicy());

		QSize s2 = ui->hSpacer_passwordAgain2->sizeHint();
		QSizePolicy sizePolicy2 = ui->hSpacer_passwordAgain2->sizePolicy();
		ui->hSpacer_passwordAgain2->changeSize(s2.width(), 0,
			sizePolicy2.horizontalPolicy(), sizePolicy2.verticalPolicy());

		QSize s3 = ui->hSpacer_passwordAgain3->sizeHint();
		QSizePolicy sizePolicy3 = ui->hSpacer_passwordAgain3->sizePolicy();
		ui->hSpacer_passwordAgain3->changeSize(s3.width(), 0,
			sizePolicy2.horizontalPolicy(), sizePolicy3.verticalPolicy());
	}
}

void CLoginUI::showAuthorityLayout(bool bShow /*= true*/)
{
	if (bShow)
	{
		ui->authorityLbl->show();
		ui->authorityBox->show();
		
		QSize s1 = ui->hSpacer_authority1->sizeHint();
		QSizePolicy sizePolicy1 = ui->hSpacer_authority1->sizePolicy();
		ui->hSpacer_authority1->changeSize(s1.width(), HSPACER_DEFAULT_HEIGHT,
			sizePolicy1.horizontalPolicy(), sizePolicy1.verticalPolicy());

		QSize s2 = ui->hSpacer_authority2->sizeHint();
		QSizePolicy sizePolicy2 = ui->hSpacer_authority2->sizePolicy();
		ui->hSpacer_authority2->changeSize(s2.width(), HSPACER_DEFAULT_HEIGHT,
			sizePolicy2.horizontalPolicy(), sizePolicy2.verticalPolicy());

		QSize s3 = ui->hSpacer_authority3->sizeHint();
		QSizePolicy sizePolicy3 = ui->hSpacer_authority3->sizePolicy();
		ui->hSpacer_authority3->changeSize(s3.width(), HSPACER_DEFAULT_HEIGHT,
			sizePolicy3.horizontalPolicy(), sizePolicy3.verticalPolicy());
	}
	else
	{
		ui->authorityLbl->hide();
		ui->authorityBox->hide();

		QSize s1 = ui->hSpacer_authority1->sizeHint();
		QSizePolicy sizePolicy1 = ui->hSpacer_authority1->sizePolicy();
		ui->hSpacer_authority1->changeSize(s1.width(), 0,
			sizePolicy1.horizontalPolicy(), sizePolicy1.verticalPolicy());

		QSize s2 = ui->hSpacer_authority2->sizeHint();
		QSizePolicy sizePolicy2 = ui->hSpacer_authority2->sizePolicy();
		ui->hSpacer_authority2->changeSize(s2.width(), 0,
			sizePolicy2.horizontalPolicy(), sizePolicy2.verticalPolicy());

		QSize s3 = ui->hSpacer_authority3->sizeHint();
		QSizePolicy sizePolicy3 = ui->hSpacer_authority3->sizePolicy();
		ui->hSpacer_authority3->changeSize(s3.width(), 0,
			sizePolicy2.horizontalPolicy(), sizePolicy3.verticalPolicy());
	}
}

void CLoginUI::showOtherNameLayout(bool bShow /*= true*/)
{
	if (bShow)
	{
		ui->newUserIdTypeBox->show();
		ui->newUserIdEdit->show();
		ui->addUserIdBtn->show();

		QSize s1 = ui->hSpacer_authority1->sizeHint();
		QSizePolicy sizePolicy1 = ui->hSpacer_authority1->sizePolicy();
		ui->hSpacer_authority1->changeSize(s1.width(), HSPACER_DEFAULT_HEIGHT,
			sizePolicy1.horizontalPolicy(), sizePolicy1.verticalPolicy());

		QSize s2 = ui->hSpacer_authority2->sizeHint();
		QSizePolicy sizePolicy2 = ui->hSpacer_authority2->sizePolicy();
		ui->hSpacer_authority2->changeSize(s2.width(), HSPACER_DEFAULT_HEIGHT,
			sizePolicy2.horizontalPolicy(), sizePolicy2.verticalPolicy());

		QSize s3 = ui->hSpacer_authority3->sizeHint();
		QSizePolicy sizePolicy3 = ui->hSpacer_authority3->sizePolicy();
		ui->hSpacer_authority3->changeSize(s3.width(), HSPACER_DEFAULT_HEIGHT,
			sizePolicy3.horizontalPolicy(), sizePolicy3.verticalPolicy());
	}
	else
	{
		ui->newUserIdTypeBox->hide();
		ui->newUserIdEdit->hide();
		ui->addUserIdBtn->hide();

		QSize s1 = ui->hSpacer_authority1->sizeHint();
		QSizePolicy sizePolicy1 = ui->hSpacer_authority1->sizePolicy();
		ui->hSpacer_authority1->changeSize(s1.width(), 0,
			sizePolicy1.horizontalPolicy(), sizePolicy1.verticalPolicy());

		QSize s2 = ui->hSpacer_authority2->sizeHint();
		QSizePolicy sizePolicy2 = ui->hSpacer_authority2->sizePolicy();
		ui->hSpacer_authority2->changeSize(s2.width(), 0,
			sizePolicy2.horizontalPolicy(), sizePolicy2.verticalPolicy());

		QSize s3 = ui->hSpacer_authority3->sizeHint();
		QSizePolicy sizePolicy3 = ui->hSpacer_authority3->sizePolicy();
		ui->hSpacer_authority3->changeSize(s3.width(), 0,
			sizePolicy2.horizontalPolicy(), sizePolicy3.verticalPolicy());
	}
}

void CLoginUI::showTipsLayout(bool bShow /*= true*/)
{
	if (bShow)
	{
		ui->passwordTipsLbl->show();
		ui->passwordTipsEdit->show();

		QSize s1 = ui->hSpacer_tips1->sizeHint();
		QSizePolicy sizePolicy1 = ui->hSpacer_tips1->sizePolicy();
		ui->hSpacer_tips1->changeSize(s1.width(), HSPACER_DEFAULT_HEIGHT,
			sizePolicy1.horizontalPolicy(), sizePolicy1.verticalPolicy());

		QSize s2 = ui->hSpacer_tips2->sizeHint();
		QSizePolicy sizePolicy2 = ui->hSpacer_tips2->sizePolicy();
		ui->hSpacer_tips2->changeSize(s2.width(), HSPACER_DEFAULT_HEIGHT,
			sizePolicy2.horizontalPolicy(), sizePolicy2.verticalPolicy());
	}
	else
	{
		ui->passwordTipsLbl->hide();
		ui->passwordTipsEdit->hide();

		QSize s1 = ui->hSpacer_tips1->sizeHint();
		QSizePolicy sizePolicy1 = ui->hSpacer_tips1->sizePolicy();
		ui->hSpacer_tips1->changeSize(s1.width(), 0,
			sizePolicy1.horizontalPolicy(), sizePolicy1.verticalPolicy());

		QSize s2 = ui->hSpacer_tips2->sizeHint();
		QSizePolicy sizePolicy2 = ui->hSpacer_tips2->sizePolicy();
		ui->hSpacer_tips2->changeSize(s2.width(), 0,
			sizePolicy2.horizontalPolicy(), sizePolicy2.verticalPolicy());
	}
}

void CLoginUI::showShowLayout(bool bShow /*= true*/)
{
	if (bShow)
	{
		ui->showLbl->show();

		QSize s1 = ui->hSpacer_show1->sizeHint();
		QSizePolicy sizePolicy1 = ui->hSpacer_show1->sizePolicy();
		ui->hSpacer_show1->changeSize(s1.width(), HSPACER_DEFAULT_HEIGHT,
			sizePolicy1.horizontalPolicy(), sizePolicy1.verticalPolicy());

		QSize s2 = ui->hSpacer_show2->sizeHint();
		QSizePolicy sizePolicy2 = ui->hSpacer_show2->sizePolicy();
		ui->hSpacer_show2->changeSize(s2.width(), HSPACER_DEFAULT_HEIGHT,
			sizePolicy2.horizontalPolicy(), sizePolicy2.verticalPolicy());
	}
	else
	{
		ui->showLbl->hide();

		QSize s1 = ui->hSpacer_show1->sizeHint();
		QSizePolicy sizePolicy1 = ui->hSpacer_show1->sizePolicy();
		ui->hSpacer_show1->changeSize(s1.width(), 0,
			sizePolicy1.horizontalPolicy(), sizePolicy1.verticalPolicy());

		QSize s2 = ui->hSpacer_show2->sizeHint();
		QSizePolicy sizePolicy2 = ui->hSpacer_show2->sizePolicy();
		ui->hSpacer_show2->changeSize(s2.width(), 0,
			sizePolicy2.horizontalPolicy(), sizePolicy2.verticalPolicy());
	}
}

void CLoginUI::showOperateLayout(bool bShow /*= true*/)
{
	if (bShow)
	{
		ui->newUserBtn->show();
		ui->editUserBtn->show();
		ui->delUserBtn->show();

		QSize s1 = ui->hSpacer_operate1->sizeHint();
		QSizePolicy sizePolicy1 = ui->hSpacer_operate1->sizePolicy();
		ui->hSpacer_operate1->changeSize(s1.width(), HSPACER_DEFAULT_HEIGHT,
			sizePolicy1.horizontalPolicy(), sizePolicy1.verticalPolicy());

		QSize s2 = ui->hSpacer_operate2->sizeHint();
		QSizePolicy sizePolicy2 = ui->hSpacer_operate2->sizePolicy();
		ui->hSpacer_operate2->changeSize(s2.width(), HSPACER_DEFAULT_HEIGHT,
			sizePolicy2.horizontalPolicy(), sizePolicy2.verticalPolicy());
	}
	else
	{
		ui->newUserBtn->hide();
		ui->editUserBtn->hide();
		ui->delUserBtn->hide();

		QSize s1 = ui->hSpacer_operate1->sizeHint();
		QSizePolicy sizePolicy1 = ui->hSpacer_operate1->sizePolicy();
		ui->hSpacer_operate1->changeSize(s1.width(), 0,
			sizePolicy1.horizontalPolicy(), sizePolicy1.verticalPolicy());

		QSize s2 = ui->hSpacer_operate2->sizeHint();
		QSizePolicy sizePolicy2 = ui->hSpacer_operate2->sizePolicy();
		ui->hSpacer_operate2->changeSize(s2.width(), 0,
			sizePolicy2.horizontalPolicy(), sizePolicy2.verticalPolicy()); 
	}
}

void CLoginUI::showBtnLayout(bool bShow /*= true*/)
{
	if (bShow)
	{
		ui->okBtn->show();
		ui->cancelBtn->show();

		QSize s1 = ui->hSpacer_btn1->sizeHint();
		QSizePolicy sizePolicy1 = ui->hSpacer_btn1->sizePolicy();
		ui->hSpacer_btn1->changeSize(s1.width(), HSPACER_DEFAULT_HEIGHT,
			sizePolicy1.horizontalPolicy(), sizePolicy1.verticalPolicy());

		QSize s2 = ui->hSpacer_btn2->sizeHint();
		QSizePolicy sizePolicy2 = ui->hSpacer_btn2->sizePolicy();
		ui->hSpacer_btn2->changeSize(s2.width(), HSPACER_DEFAULT_HEIGHT,
			sizePolicy2.horizontalPolicy(), sizePolicy2.verticalPolicy());

		QSize s3 = ui->hSpacer_btn3->sizeHint();
		QSizePolicy sizePolicy3 = ui->hSpacer_btn3->sizePolicy();
		ui->hSpacer_btn3->changeSize(s3.width(), HSPACER_DEFAULT_HEIGHT,
			sizePolicy3.horizontalPolicy(), sizePolicy3.verticalPolicy());
	}
	else
	{
		ui->okBtn->hide();
		ui->cancelBtn->hide();

		QSize s1 = ui->hSpacer_btn1->sizeHint();
		QSizePolicy sizePolicy1 = ui->hSpacer_btn1->sizePolicy();
		ui->hSpacer_btn1->changeSize(s1.width(), 0,
			sizePolicy1.horizontalPolicy(), sizePolicy1.verticalPolicy());

		QSize s2 = ui->hSpacer_btn2->sizeHint();
		QSizePolicy sizePolicy2 = ui->hSpacer_btn2->sizePolicy();
		ui->hSpacer_btn2->changeSize(s2.width(), 0,
			sizePolicy2.horizontalPolicy(), sizePolicy2.verticalPolicy());

		QSize s3 = ui->hSpacer_btn3->sizeHint();
		QSizePolicy sizePolicy3 = ui->hSpacer_btn3->sizePolicy();
		ui->hSpacer_btn3->changeSize(s3.width(), 0,
			sizePolicy2.horizontalPolicy(), sizePolicy3.verticalPolicy()); 
	}
}



void CLoginUI::addUserId(const sUserId &userId)
{
	on_addUserIdBtn_clicked();

	int iCount = ui->vLayout_otherName->count();
	QHBoxLayout* layout = qobject_cast<QHBoxLayout*>(ui->vLayout_otherName->itemAt(iCount - 1)->layout());
	QComboBox* userIdTypeBox = qobject_cast<QComboBox*>(layout->itemAt(USERIDTYPEBOX_COL)->widget());
	QLineEdit* userIdEdit = qobject_cast<QLineEdit*>(layout->itemAt(USERIDEDIT_COL)->widget());
	userIdTypeBox->setCurrentText(userId.getUserIdTypeName());
	userIdEdit->setText(userId.strUserId);
}

QList<sUserId> CLoginUI::getOtherUserId()
{
	QList<sUserId> lstUserId;
	int iCount = ui->vLayout_otherName->count();
	for (int index = 0; index < iCount; index++)
	{
		QHBoxLayout* layout = qobject_cast<QHBoxLayout*>(ui->vLayout_otherName->itemAt(index)->layout());
		QComboBox* userIdTypeBox = qobject_cast<QComboBox*>(layout->itemAt(USERIDTYPEBOX_COL)->widget());
		QLineEdit* userIdEdit = qobject_cast<QLineEdit*>(layout->itemAt(USERIDEDIT_COL)->widget());
		sUserId::nUserIdType iUserIdType = sUserId::getUserIdType(userIdTypeBox->currentText());
		QString strUserId = userIdEdit->text();
		if (!strUserId.isEmpty() &&
			iUserIdType != sUserId::nUserIdType::nUserIdType_Invalid)
			lstUserId.append(sUserId(strUserId, iUserIdType));
	}
	return lstUserId;
}

bool CLoginUI::getLoginUser(sLoginUser &loginUser)
{
	QString strUserMainName = ui->userBox->currentText();
	QString strPassword = ui->passwordEdit->text();
	int iAuthority = sLoginUser::getAuthority(ui->authorityBox->currentText());
	QString strPasswordTips = ui->passwordTipsEdit->toPlainText();

	if (!mLoginUser.getAutoUserId().isEmpty())
		loginUser = mLoginUser;
	loginUser.setUserId(strUserMainName, sUserId::nUserIdType::nUserIdType_UserMainName);
	loginUser.setShowUser(strUserMainName);
	loginUser.setPassword(strPassword, strPasswordTips);
	loginUser.setAuthority(iAuthority);
	QList<sUserId> lstUserId = loginUser.getUserIdList();
	for (auto userId : lstUserId)
	{
		if (userId.iUserIdType != sUserId::nUserIdType::nUserIdType_UserMainName &&
			userId.iUserIdType != sUserId::nUserIdType::nUserIdType_UserAutoId)
		{
			bool ret = loginUser.removeUserId(userId.strUserId, userId.iUserIdType);
			if (!ret)
			{
				ui->showLbl->setText(loginUser.getLastError());
				showShowLayout(true);
				return false;
			}
		}
	}
	lstUserId = getOtherUserId();
	for (auto &userId : lstUserId)
	{
		bool ret = loginUser.addUserId(userId.strUserId, userId.iUserIdType);
		if (!ret)
		{
			ui->showLbl->setText(loginUser.getLastError());
			showShowLayout(true);
			return false;
		}
	}
	return true;
}

QStack<nLoginUiType> CLoginUI::m_stackLoginType = QStack<nLoginUiType>();

QStack<CLoginUI*> CLoginUI::m_stackLoginUi = QStack<CLoginUI*>();

