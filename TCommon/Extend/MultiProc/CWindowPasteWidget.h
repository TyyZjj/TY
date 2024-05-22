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
	/*�Ӵ���ճ������ʱ�Ƿ��ޱ߿�.
		����б߿�ճ����������Ҫȥ�߿�, �ָ�ԭ��ʱ, Ҫ�ָ��߿�*/
	bool m_isWindowHint = false;
	bool m_isRecoveryChildWindowWhenQuit = true;
};
#endif //_CWINDOWPASTEWIDGET_H_


/*����
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
	lstWndName.append(QString::fromLocal8Bit("���������ȱ�ݼ��ϵͳ"));
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