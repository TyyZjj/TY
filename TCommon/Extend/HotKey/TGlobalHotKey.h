#ifndef _TGLOBALHOTKEY_H_
#define _TGLOBALHOTKEY_H_

#include <QSet>
#include <QPair>
#include <QObject>
#include <QAbstractNativeEventFilter>

//ȫ�ֵ�Windows�����¼�����
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
	QHash<QPair<quint32, quint32>, QString> m_hashHotKey;//��Ҫ���˵��ȼ�����
};

#include <QStringList>
#include <QApplication>
#include <QKeySequence>
class TGlobalHotKey : public QObject
{
	Q_OBJECT
public:
	//******Taoyuyu******2024/05/23**********
	// ����: QStringList lstKey: ���ݿ�����һ������, Ҳ�����Ƕ���������
	//		lstKey.append("A"); lstKey.append("Ctrl+C");
	// ����: QObject * app: 
	//************************************
	TGlobalHotKey(QStringList lstKey, QObject* app);
	~TGlobalHotKey();
	bool registerHotKey(QString strKey);
Q_SIGNALS:
	void signalKeyEvent(QString strKey);

private:
	bool registerHotKey();
	bool unregisterHotKey();

	QSet<QPair<quint32, quint32>> m_setHotKeyPair;//��Ҫ���˵��ȼ�����
	QHash<QPair<quint32, quint32>, QString> m_hashHotKey;//��Ҫ���˵��ȼ�����
	QApplication *m_app;
	TWinKeyEventFilter *m_filter;
	QStringList m_lstKey;
	static quint32 nativeKeycode(Qt::Key keycode);
	static quint32 nativeModifiers(Qt::KeyboardModifiers modifiers);
};

/*����
	QApplication a(argc, argv);
	TGlobalHotKey globalHotKey(QStringList() << "Ctrl+W", &a);
	a.exec();
*/
#endif // !_TGLOBALHOTKEY_H_
