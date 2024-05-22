#ifndef _CWINDOWPASTEWIDGET_H_
#define _CWINDOWPASTEWIDGET_H_

#include <QWidget>

class CWindowPasteWidget : public QWidget 
{
public:
	CWindowPasteWidget(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

	bool paste(HWND child);

protected:
	virtual void paintEvent(QPaintEvent *event);
	virtual void moveEvent(QMoveEvent *event);
	virtual void closeEvent(QCloseEvent *event);

private:
	HWND mParent;
	HWND mChild;

	bool m_isPaste = false;
	/*子窗口粘贴进来时是否无边框.
		如果有边框，粘贴进来后需要去边框, 恢复原样时, 要恢复边框*/
	bool m_isWindowHint = false;
	bool m_isRecoveryChildWindowWhenQuit = true;
};
#endif //_CWINDOWPASTEWIDGET_H_


/*范例
class Test : public CWindowPasteWidget
{
	Q_OBJECT

public:
	Test(QWidget *parent = Q_NULLPTR);

private:
	Ui::TestClass ui;
};


Test::Test(QWidget *parent)
	: CWindowPasteWidget(parent)
{
	ui.setupUi(this);

	QStringList lstWndName;
	lstWndName.append(QString::fromLocal8Bit("富鑫林外观缺陷检测系统"));
	sProcInfo procInfo("E:/NewGit/alg-KB-Outlook/Bin/Release/Inspection-wg.exe", lstWndName);
	CMultiProcManager::GetKernel()->start(procInfo);

	connect(CMultiProcManager::GetKernel(), &CMultiProcManager::signalProcessStarted, this,
		[&](sProcInfo procInfo) {
		if (!procInfo.mapProcWnd.isEmpty())
		{
			WId wid = procInfo.mapProcWnd.first();
			paste((HWND)wid);
		}
	}, Qt::QueuedConnection);
}
*/