#include <QSqlDatabase>
#include "CLoginManager.h"
#include "CMySqlDBParaUI.h"
#include "ui_CMySqlDBParaUI.h"

CMySqlDBParaUI::CMySqlDBParaUI(QWidget* parent)
	: QDialog(parent)
{
	ui = new Ui::CMySqlDBParaUI();
	ui->setupUi(this);

	QRegExp regExp("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b");
	ui->editIP->setValidator(new QRegExpValidator(regExp, this));

	QString strIp, strDbName, strUserId, strPassword;
	int iPort;
	bool ret = CLoginManager::GetKernel()->loadDBPara(strIp, iPort, strDbName, strUserId, strPassword);
	if (ret)
	{
		ui->editIP->setText(strIp);
		ui->spinBoxPort->setValue(iPort);
		ui->editDBName->setText(strDbName);
		ui->editUserName->setText(strUserId);
		ui->editPassword->setText(strPassword);
	}
}

CMySqlDBParaUI::~CMySqlDBParaUI()
{
	delete ui;
}

void CMySqlDBParaUI::on_testBtn_clicked()
{
	ui->lblTestResult->setText("Connecting...");
	if (isConnect())
	{
		ui->lblTestResult->setText("Connect Success!");
	}
	else
	{
		ui->lblTestResult->setText("Connect Failed!");
	}
}

void CMySqlDBParaUI::on_okBtn_clicked()
{
	QString strIp = ui->editIP->text();
	int iPort = ui->spinBoxPort->value();
	QString strDbName = ui->editDBName->text();
	QString strUserId = ui->editUserName->text();
	QString strPassword = ui->editPassword->text();

	CLoginManager::GetKernel()->saveDBPara(strIp, iPort, strDbName, strUserId, strPassword);

	accept();
}

bool CMySqlDBParaUI::isConnect()
{
	QString strIp = ui->editIP->text();
	int iPort = ui->spinBoxPort->value();
	QString strDbName = ui->editDBName->text();
	QString strUserId = ui->editUserName->text();
	QString strPassword = ui->editPassword->text();

	QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL"); // 选择MySQL数据库驱动
	db.setHostName(strIp);          // 设置数据库服务器地址
	db.setPort(iPort);              // 设置数据库端口
	db.setDatabaseName(strDbName);     // 设置数据库名称
	db.setUserName(strUserId);           // 设置用户名
	db.setPassword(strPassword);       // 设置密码

	bool ret = db.open();
	if (ret)
		db.close();
	
	return ret;
}
