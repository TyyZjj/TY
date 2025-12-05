#pragma once

#include <QLabel>
#include <QWidget>
#include <QSpacerItem>
#include <QMainWindow>
#include <QDockWidget>

#define PROPERTY_DOCK_TYPE	"DockType"
#define DOCK_TYPE_FOLD_BTN	0
#define DOCK_TYPE_FOLD_DOCK	1

enum nFoldDirection
{
	nFoldLeft = ::Qt::DockWidgetArea::LeftDockWidgetArea,
	nFoldRight = ::Qt::DockWidgetArea::RightDockWidgetArea,
	nFoldTop = ::Qt::DockWidgetArea::TopDockWidgetArea,
	nFoldBottom = ::Qt::DockWidgetArea::BottomDockWidgetArea,
};

class TDockWidgetFoldWidget : public QWidget
{
	Q_OBJECT

public:
	TDockWidgetFoldWidget(QMainWindow* pMainWindow, 
		nFoldDirection direction = nFoldLeft,
		const QString& title = "",
		QWidget* parent = nullptr);
	~TDockWidgetFoldWidget();

	//设置成员变量 m_controlWidget 的值
	void setControlWidget(QDockWidget* controlWidget);
	//设置成员变量 m_parternWidget 的值
	void setParternWidget(QWidget* parternWidget);
	void hideDock();
	void hideAllDock();
	QList<QDockWidget*> getAreaDockWidget(Qt::DockWidgetArea area);
	void setTitle(const QString& title);

private:
	void initWidget();

private:
	QMainWindow* m_pMainWindow = nullptr;
	//这个是包含它的dockwidget
	QDockWidget* m_pControlWidget = nullptr;
	//这个是与它搭配的dockwidget
	QWidget* m_pParternWidget = nullptr;
	//折叠的方向
	nFoldDirection m_iDirection;

	QLabel* m_pLabel = nullptr;
	QString m_strTitle;

	QSpacerItem* m_pSpacerItem = nullptr;
};
