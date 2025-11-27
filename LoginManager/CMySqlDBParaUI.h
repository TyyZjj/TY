#pragma once

#include <QDialog>
namespace Ui { class CMySqlDBParaUI; };

class CMySqlDBParaUI : public QDialog
{
	Q_OBJECT

public:
	CMySqlDBParaUI(QWidget* parent = Q_NULLPTR);
	~CMySqlDBParaUI();

public Q_SLOTS:
	void on_testBtn_clicked();
	void on_okBtn_clicked();

private:
	bool isConnect();

private:
	Ui::CMySqlDBParaUI *ui;
};
