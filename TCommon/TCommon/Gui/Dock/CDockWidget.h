#pragma once
#include <QLabel>
#include <QPushButton>
#include <QDockWidget>
#include <QMainWindow>
#include "CDockWidgetFoldWidget.h"

class CDockWidget : public QDockWidget
{
	Q_OBJECT
public:
	explicit CDockWidget(QMainWindow* parent, 
		QWidget * pWidget,
		nFoldDirection direction = nFoldLeft, 
		const QString& title = "", 
		bool bCanClose = false, //能关闭
		Qt::WindowFlags flags = Qt::WindowFlags());
	~CDockWidget();
	void setTitleBarStyleSheet(const QString& styleSheet);
	void hideDock();
	void minimizeDock();

	void setTitle(const QString& title);
	//完全隐藏，包括按键dock
	bool isFoldDockHidden();
	bool isCompleteHidden();
	void setHideState(bool bDockHide, bool bFoldDockHide);
	void setCompleteHide(bool hide, bool &isHidden, bool &isDockHide, bool &isFoldDockHide);

Q_SIGNALS:
	void signalClose(QString strTitle);

private:
	void initWidget();
	void releaseFoldDockWidget();
	void initFoldDockWidget(nFoldDirection direction);
	void resetDirection(nFoldDirection direction);

	QList<QList<QDockWidget*>> getAreaDockWidget(Qt::DockWidgetArea area, int* pCurDockIndx = nullptr);
	void hideAllDock(QList<QList<QDockWidget*>> lstAreaDock = QList<QList<QDockWidget*>>());

private:
	//包含 成员变量 foldContentWidget 的那个dockwidget
	QDockWidget* m_pFoldDockWidget = nullptr;
	CDockWidgetFoldWidget* m_pFoldWidget = nullptr;

	//该dockwidget是靠左还是靠右
	nFoldDirection	m_iDirection;
	//自定义dockwidget的titleBar
	QWidget*		m_pTitleBar = nullptr;
	QLabel*			m_pLabel = nullptr;
	QString			m_strTitle;
	bool			m_bCanClose = false;
	QPushButton*	m_pCloseBtn = nullptr;
	QPushButton*	m_pFoldBtn = nullptr;

	QMainWindow*	m_pParent = nullptr;
};
