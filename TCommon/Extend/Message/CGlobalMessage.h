#pragma once
#ifndef _CGLOBALMESSAGE_H_
#define _CGLOBALMESSAGE_H_


#include <QMutex>
#include <QObject>
#include <QPointer> // Ensure QPointer is included
#include "TDefine.h"
#include "TFuncTraits.h"

/*
* 中心化的消息机制--禁止自主创建消息发送器, 过多的消息发送器会导致消息发送的业务逻辑混乱
* Taoyuyu 20251201 todo 优化: 是否要支持一个信号一个接收者相同入参的槽函数绑定多次??? directConnect时的执行效率待验证
* 1. 通过消息ID来区分消息
* 2. 一个消息可以有多个接收者，如
*	attach("Meassage", obj1, &obj1::fun), attach("Meassage", obj2, &obj2::fun)
* 3. 一个消息可以有多个重载的信号, 一个接收者可以绑定多个重载的槽函数, 但对于 一个消息 一个接收者 同一个信号形参只能绑定一次, 如:
*	attach("Meassage", obj, QOverload<>::of(&obj::fun))						->成功
*	attach("Meassage", obj, QOverload<const QVariant&>::of(&obj::fun))		->成功, 重载的槽函数
*	attach("Meassage", obj, QOverload<const QVariant&>::of(&obj::fun))		->失败, 重复绑定
* 4. 适用于跨线程、跨模块的消息发送, 不适用高强度的实时性消息发送
*/
class Transfer final : public QObject
{
	Q_OBJECT
public:
	Transfer(QObject* parent = nullptr) : QObject(parent) {}
public:
Q_SIGNALS:
	void notify();
	void notify(const QVariant& variant);
	void notify(const QVariantList& lstVar);
};

//Transfer::notify()信号对应的函数指针

enum ConnectSignalIndex
{
	ConnectSignal_Void = 0,
	ConnectSignal_Variant,
	ConnectSignal_VariantList,
	ConnectSignal_Invalid,
};

struct ConnectInfo
{
	QString strMsgId;	//消息ID
	QPointer<Transfer> pTransfer = nullptr;	//消息发送器

	struct ReceiverConnection
	{
		QPointer<QObject> pReceiver = nullptr;	//接收者
		QMetaObject::Connection connection;	//连接句柄
		ConnectSignalIndex iSignalIndex = ConnectSignal_Invalid;

		bool operator==(const ReceiverConnection& other) const
		{
			return pReceiver == other.pReceiver &&
				iSignalIndex == other.iSignalIndex;
		}
	};
	QList<ReceiverConnection> lstReceiverConnections;	//接收者连接句柄, 一个消息可以有多个接收者
};

class CGlobalMessage final : public QObject
{
	Q_OBJECT
public:
	Kernel(CGlobalMessage);

	void notify(const QString& messageId);
	void notify(const QString& messageId, const QVariant& variant);
	void notify(const QString& messageId, const QVariantList& lstVar);

	//QMetaObject::Connection attach(const QString& strMsgId, QObject* pReceiver, const char* pMethod, ::Qt::ConnectionType linkType = ::Qt::AutoConnection);
    template<typename Receiver, typename Func>
    QMetaObject::Connection attach(const QString& strMsgId, Receiver* pReceiver, Func func, ::Qt::ConnectionType linkType = ::Qt::AutoConnection)
    {
        QMetaObject::Connection connection;
        if (!pReceiver || !func)
            return connection;

        QMutexLocker locker(&m_mutex);
        ConnectInfo& connInfo = m_mapConnectInfo[strMsgId];
        if (connInfo.pTransfer == nullptr)
        {
            connInfo.strMsgId = strMsgId;
            connInfo.pTransfer = new Transfer(this);
        }

		ConnectSignalIndex iSignalIndex = ConnectSignalIndex::ConnectSignal_Invalid;
		if (FuncTraits<void(Transfer::*)()>::get_signature_info().rpEquals(FuncTraits<Func>::get_signature_info()))
        {
			iSignalIndex = ConnectSignalIndex::ConnectSignal_Void;
		}
        else if (FuncTraits<void(Transfer::*)(const QVariant&)>::get_signature_info().rpEquals(FuncTraits<Func>::get_signature_info()))
		{
			iSignalIndex = ConnectSignalIndex::ConnectSignal_Variant;
		}
		else if (FuncTraits<void(Transfer::*)(const QVariantList&)>::get_signature_info().rpEquals(FuncTraits<Func>::get_signature_info()))
		{
			iSignalIndex = ConnectSignalIndex::ConnectSignal_VariantList;
		}
		else
		{
			//不支持的签名
			return connection;
		}

        //防止重复连接
        for (auto recvConn : connInfo.lstReceiverConnections)
        {
            if (recvConn.pReceiver == pReceiver &&
				recvConn.iSignalIndex == iSignalIndex)
            {
                if (recvConn.connection)
                    return recvConn.connection;
                else
                    connInfo.lstReceiverConnections.removeOne(recvConn);
            }
        }

		if (iSignalIndex == ConnectSignalIndex::ConnectSignal_Void)
		{
			connection = QObject::connect(
				connInfo.pTransfer.data(),
				QOverload<>::of(&Transfer::notify),
				pReceiver,
				[pReceiver, func]() {
					// 这是一个不安全的运行时操作,但能解决编译时的签名问题。
					(static_cast<Receiver*>(pReceiver)->*reinterpret_cast<void (Receiver::*)()>(func))();
				},
				linkType);
		}
		else if (iSignalIndex == ConnectSignalIndex::ConnectSignal_Variant)
		{
			connection = QObject::connect(
				connInfo.pTransfer.data(),
				QOverload<const QVariant&>::of(&Transfer::notify),
				pReceiver,
				[pReceiver, func](const QVariant& variant) {
					// 这是一个不安全的运行时操作,但能解决编译时的签名问题。
					(static_cast<Receiver*>(pReceiver)->*reinterpret_cast<void (Receiver::*)(const QVariant&)>(func))(variant);
				},
				linkType);
		}
		else if (iSignalIndex == ConnectSignalIndex::ConnectSignal_VariantList)
		{
			connection = QObject::connect(
				connInfo.pTransfer.data(),
				QOverload<const QVariantList&>::of(&Transfer::notify),
				pReceiver,
				[pReceiver, func](const QVariantList& lstVar) {
					// 这是一个不安全的运行时操作，但能解决编译时的签名问题。
					(static_cast<Receiver*>(pReceiver)->*reinterpret_cast<void (Receiver::*)(const QVariantList&)>(func))(lstVar);
				},
				linkType);
		}
        
		if (connection)
		{
			connInfo.lstReceiverConnections.append({ pReceiver, connection, iSignalIndex });
		}
        return connection;
    }

	void detach(const QString& strMsgId, QObject* pReceiver);

	template<typename Receiver, typename Func>
	void detach(const QString& strMsgId, Receiver* pReceiver, Func func)
	{
		QMutexLocker locker(&m_mutex);
		auto it = m_mapConnectInfo.find(strMsgId);
		if (it != m_mapConnectInfo.end())
		{
			ConnectSignalIndex iSignalIndex = ConnectSignalIndex::ConnectSignal_Invalid;
			if (FuncTraits<void(Transfer::*)()>::get_signature_info().rpEquals(FuncTraits<Func>::get_signature_info()))
			{
				iSignalIndex = ConnectSignalIndex::ConnectSignal_Void;
			}
			else if (FuncTraits<void(Transfer::*)(const QVariant&)>::get_signature_info().rpEquals(FuncTraits<Func>::get_signature_info()))
			{
				iSignalIndex = ConnectSignalIndex::ConnectSignal_Variant;
			}
			else if (FuncTraits<void(Transfer::*)(const QVariantList&)>::get_signature_info().rpEquals(FuncTraits<Func>::get_signature_info()))
			{
				iSignalIndex = ConnectSignalIndex::ConnectSignal_VariantList;
			}
			else
			{
				//不支持的签名
				return;
			}
			auto& connInfo = it.value();
			for (auto recvConn : connInfo.lstReceiverConnections)
			{
				if (recvConn.pReceiver == pReceiver &&
					recvConn.iSignalIndex == iSignalIndex)
				{
					if (recvConn.connection)
					{
						QObject::disconnect(recvConn.connection);
					}
					connInfo.lstReceiverConnections.removeOne(recvConn);
					break;
				}
			}
		}
	}

private:
	CGlobalMessage(QObject* parent = nullptr): m_mutex(QMutex::Recursive) {}
	~CGlobalMessage() {}
	Q_DISABLE_COPY(CGlobalMessage);

	QMutex m_mutex;
	QMap<QString, ConnectInfo> m_mapConnectInfo;
};

/*Sample
class TestApp : public QObject
{
	Q_OBJECT

public:
	TestApp(QObject *parent = nullptr);
	~TestApp();

public Q_SLOTS:
	void recvMessage1();
	void recvMessage2(const QVariant& var);
};

CGlobalMessage::GetKernel()->attach("TestMessage", this, &TestApp::recvMessage1);
CGlobalMessage::GetKernel()->attach("TestMessage", this, &TestApp::recvMessage2);

CGlobalMessage::GetKernel()->notify("TestMessage");
CGlobalMessage::GetKernel()->notify("TestMessage", QVariant("Hello World!"));

CGlobalMessage::GetKernel()->detach("TestMessage", this, &TestApp::recvMessage1);
CGlobalMessage::GetKernel()->detach("TestMessage", this, &TestApp::recvMessage2);
CGlobalMessage::GetKernel()->detach("TestMessage", this);//会删除"TestMessage"信号中所有和this对象相关的连接(&TestApp::recvMessage1和&TestApp::recvMessage2)
*/

#endif // !_CGLOBALMESSAGE_H_