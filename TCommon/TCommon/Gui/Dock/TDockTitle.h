#pragma once

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class TDockTitleClass; };
QT_END_NAMESPACE

/*
* 演示
m_pDockTitle = new TDockTitle(QApplication::translate("LabelWidget", "Count Down", nullptr), m_pCountDownUI, false, ui.frameCountDown);
ui.gridLayout_4->addWidget(m_pDockTitle, 0, 0, 1, 1);
*/

class TDockTitle : public QWidget
{
	Q_OBJECT

public:
	TDockTitle(QString strTitle, QWidget *pChild, bool bCanClose = true,  QWidget *parent = nullptr);
	~TDockTitle();

	void hideContent(bool isHide = true);
	void setTitle(QString strTitle);

Q_SIGNALS:
	void signalClose();

private:
	void initWidget();

private:
	Ui::TDockTitleClass *ui;
};
