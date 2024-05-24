#ifndef _TGLOBALHOTKEY_H_
#define _TGLOBALHOTKEY_H_

#include <QSet>
#include <QPair>
#include <QObject>
#include <QAbstractNativeEventFilter>

//全局的Windows案件事件过滤
class TWinKeyEventFilter : public QObject, public QAbstractNativeEventFilter
{
	Q_OBJECT
public:
	TWinKeyEventFilter(QHash<QPair<quint32, quint32>, QString> hashHotKey = 
		QHash<QPair<quint32, quint32>, QString>());
	~TWinKeyEventFilter();
	void setWinKeyEventFilter(QHash<QPair<quint32, quint32>, QString> hashHotKey);
	void addWinKeyEventFilter(QPair<quint32, quint32> pairKey, QString strKey);
	virtual bool nativeEventFilter(const QByteArray &eventType, void *message, long *);
Q_SIGNALS:
	void signalKeyEvent(QString strKey);

private:
	QHash<QPair<quint32, quint32>, QString> m_hashHotKey;//需要过滤的热键集合
};

#include <QStringList>
#include <QApplication>
#include <QKeySequence>
class TGlobalHotKey : public QObject
{
	Q_OBJECT
public:
	//******Taoyuyu******2024/05/23**********
	// 参数: QStringList lstKey: 内容可以是一个按键, 也可以是多个按键组合
	//		lstKey.append("A"); lstKey.append("Ctrl+C");
	// 参数: QObject * app: 
	//************************************
	TGlobalHotKey(QStringList lstKey, QObject* app);
	~TGlobalHotKey();
	bool registerHotKey(QString strKey);
Q_SIGNALS:
	void signalKeyEvent(QString strKey);

private:
	bool registerHotKey();
	bool unregisterHotKey();

	QSet<QPair<quint32, quint32>> m_setHotKeyPair;//需要过滤的热键集合
	QHash<QPair<quint32, quint32>, QString> m_hashHotKey;//需要过滤的热键集合
	QApplication *m_app;
	TWinKeyEventFilter *m_filter;
	QStringList m_lstKey;
	static quint32 nativeKeycode(Qt::Key keycode);
	static quint32 nativeModifiers(Qt::KeyboardModifiers modifiers);
};

/*范例
	QApplication a(argc, argv);
	TGlobalHotKey globalHotKey(QStringList() << "Ctrl+W", &a);
	a.exec();
*/
#endif // !_TGLOBALHOTKEY_H_
