#include "CGlobalMessage.h"

void CGlobalMessage::notify(const QString& messageId)
{
	QMutexLocker locker(&m_mutex);
	if (m_mapConnectInfo.contains(messageId))
	{
		auto& info = m_mapConnectInfo[messageId];
		if (info.pTransfer)
		{
			emit info.pTransfer->notify();
		}
	}
}

void CGlobalMessage::notify(const QString& messageId, const QVariant& variant)
{
	QMutexLocker locker(&m_mutex);
	auto it = m_mapConnectInfo.find(messageId);
	if (it != m_mapConnectInfo.end())
	{
		auto& info = m_mapConnectInfo[messageId];
		if (info.pTransfer)
		{
			emit info.pTransfer->notify(variant);
		}
	}
}

void CGlobalMessage::notify(const QString& messageId, const QVariantList& lstVar)
{
	QMutexLocker locker(&m_mutex);
	auto it = m_mapConnectInfo.find(messageId);
	if (it != m_mapConnectInfo.end())
	{
		auto& info = m_mapConnectInfo[messageId];
		if (info.pTransfer)
		{
			emit info.pTransfer->notify(lstVar);
		}
	}
}

//QMetaObject::Connection CGlobalMessage::attach(const QString& strMsgId, QObject* pReceiver, const char* pMethod, ::Qt::ConnectionType linkType)
//{
//	QMetaObject::Connection connection;
//	if (!pReceiver || !pMethod)
//		return connection;
//
//	QMutexLocker locker(&m_mutex);
//	ConnectInfo &connInfo = m_mapConnectInfo[strMsgId];
//	if (connInfo.pTransfer == nullptr)
//	{
//		connInfo.strMsgId = strMsgId;
//		connInfo.pTransfer = new Transfer(this);
//	}
//	//防止重复连接
//	for (auto recvConn : connInfo.lstReceiverConnections)
//	{
//		if (recvConn.pReceiver == pReceiver)
//		{
//			if (recvConn.connection)
//				return recvConn.connection;
//			else
//				connInfo.lstReceiverConnections.removeOne(recvConn);
//		}
//	}
//
//	ConnectSignalIndex iSignalIndex = ConnectSignalIndex::ConnectSignal_Invalid;
//	connection = QObject::connect(connInfo.pTransfer.data(), SIGNAL(notify()), pReceiver, pMethod, linkType);
//	if (connection)
//	{
//		iSignalIndex = ConnectSignalIndex::ConnectSignal_Void;
//	}
//	else
//	{
//		connection = QObject::connect(connInfo.pTransfer.data(), SIGNAL(notify(const QVariant&)), pReceiver, pMethod, linkType);
//		if (connection)
//		{
//			iSignalIndex = ConnectSignalIndex::ConnectSignal_Variant;
//		}
//		else
//		{
//			connection = QObject::connect(connInfo.pTransfer.data(), SIGNAL(notify(const QVariantList&)), pReceiver, pMethod, linkType);
//			if (connection)
//			{
//				iSignalIndex = ConnectSignalIndex::ConnectSignal_VariantList;
//			}
//		}
//	}
//	if (connection)
//	{
//		connInfo.lstReceiverConnections.append({ pReceiver, connection,iSignalIndex });
//	}
//
//	return connection;
//}

void CGlobalMessage::detach(const QString& strMsgId, QObject* pReceiver)
{
	QMutexLocker locker(&m_mutex);
	if (m_mapConnectInfo.contains(strMsgId))
	{
		auto& info = m_mapConnectInfo[strMsgId];
		if (info.pTransfer)
		{
			for (auto recvConn : info.lstReceiverConnections)
			{
				if (recvConn.pReceiver == pReceiver)
				{
					QObject::disconnect(recvConn.connection);
					info.lstReceiverConnections.removeOne(recvConn);
				}
			}
		}
	}
}
