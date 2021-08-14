#include "BackendSync.hpp"

//初始化需要移动到别的地方..因为需要注意初始化顺序.
BackendSync* BackendSync::instance = nullptr;
BackendSync* BackendSync::getIns() {
    if(instance == nullptr)
        instance = new BackendSync;
    return instance;
}

void BackendSync::onConnected() {
    connect(m_webSocket, &QWebSocket::textMessageReceived,
        this, &BackendSync::onTextMessageReceived);
    m_webSocket->sendTextMessage(QStringLiteral("Hello, world!"));
}

void BackendSync::onTextMessageReceived(QString message) {
    qDebug() << "Message received:" << message;
    qInfo() << m_webSocket->peerAddress();
    qInfo() << m_webSocket->peerPort();
    qInfo() << m_webSocket->peerName();

}

void BackendSync::onUdpReadyRead() {
    static QStringList ipList;

    if(ipList.isEmpty()) {
        for(const auto& addr : QNetworkInterface::allAddresses()) {
            const auto& ipv4 = addr.toIPv4Address();
            if(ipv4 == 0)//0.0.0.0
                continue;
            qInfo() << addr.toIPv4Address() << QHostAddress(addr.toIPv4Address()).toString();
            ipList.append( QHostAddress(addr.toIPv4Address()).toString() );
        }
        qInfo() << "ipList size: " << ipList.size();
    }


    QString data;
    char dat[1024] = "";
    QHostAddress addr;
    quint16 port;
    m_socket->readDatagram(dat, 1024, &addr, &port);
    QString ip = QHostAddress(addr.toIPv4Address()).toString();
    data = dat;


    qInfo() << QString("%3 read udp from [%1:%2] : ")
    .arg(QHostAddress(addr.toIPv4Address()).toString())
    .arg(port)
    .arg(ipList.indexOf(ip) >= 0 ? "(self drop)" : "(other)") << dat;

    if(ipList.indexOf(ip) == -1) {
        //other
        emit receivedMessage(addr, port, data);
    }else{
        //self
        emit receivedMessage(addr, port, data);
    }
    
}


//! [processTextMessage]
void BackendSync::processTextMessage(QString message)
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    
    qDebug() << "Message received:" << message;
    
    receivedMessage(pClient->peerAddress(), pClient->peerPort(), message);

    if (pClient) {
        //pClient->sendTextMessage(message);
    }

}
//! [processTextMessage]

//! [processBinaryMessage]
void BackendSync::processBinaryMessage(QByteArray message)
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    
    qDebug() << "Binary Message received:" << message;
    
    if (pClient) {
        pClient->sendBinaryMessage(message);
    }
}
//! [processBinaryMessage]

//! [socketDisconnected]
void BackendSync::socketDisconnected()
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    
    qInfo() << QString("ws-server: closed [%1:%2]").arg(pClient->peerName()).arg(pClient->peerPort());
    qDebug() << "socketDisconnected:" << pClient;
    
    if (pClient) {
        m_clients.removeAll(pClient);
        pClient->deleteLater();
    }
}