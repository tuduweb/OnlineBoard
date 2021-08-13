#ifndef __OBC_BACKENDSYNC_H__
#define __OBC_BACKENDSYNC_H__

#include <QObject>
#include <QtWebSockets/QWebSocket>
#include "QtWebSockets/qwebsocketserver.h"

#include <QUrl>
//#include <QThread>

#include <QNetworkInterface>
#include <QUdpSocket>
#include <QHostAddress>

class BackendSync : public QObject
{
    Q_OBJECT

public:
    static BackendSync* instance;
    static BackendSync* getIns();

    BackendSync() : m_pWebSocketServer(nullptr), m_socket(nullptr)
    {

#if 0
        initWSClient();
#else
        //initWSServer();
#endif

        initUdpSocket();

    }

    ~BackendSync()
    {
        m_webSocket.close();
    }

public:
    QString localAddress() {
        //可能需要加入判断..
        return QHostAddress(m_socket->localAddress()).toString();
    }
    void sendMessasge(QString message) {
        m_webSocket.sendTextMessage(message);
        boardcast(message);
    }

    void boardcast(QString msg) {
        QByteArray message = msg.toUtf8();
        m_socket->writeDatagram(message, QHostAddress::Broadcast, 9999);
    }

    void sendUdpMessage(const QString& msg, const QHostAddress& addr, quint16 port) {
        QByteArray message = msg.toUtf8();
        m_socket->writeDatagram(message, addr, port);
    }

    void initWSClient() {

        //QUrl url = "ws://192.168.123.243:3000";
        QUrl url = QStringLiteral("ws://192.168.123.30:3000");

        connect(&m_webSocket, &QWebSocket::connected, this, &BackendSync::onConnected);
        connect(&m_webSocket, &QWebSocket::disconnected, this, &BackendSync::closed);

        connect(&m_webSocket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
                [=](QAbstractSocket::SocketError error) { 
                    qInfo() << "ws error : " << error << m_webSocket.errorString();
                 });

        m_webSocket.open(url);
    }

    void initWSClient(QString ip, quint16 port) {

        QUrl url = QString("ws://%1:%2").arg(ip).arg(port);

        //可能还需要区分一下来源 不然调用多次就connect了多次..
        connect(&m_webSocket, &QWebSocket::connected, this, &BackendSync::onConnected);
        connect(&m_webSocket, &QWebSocket::disconnected, this, &BackendSync::closed);

        connect(&m_webSocket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
                [=](QAbstractSocket::SocketError error) { 
                    qInfo() << "ws error : " << error << m_webSocket.errorString();
                 });

        m_webSocket.open(url);

    }

    void initWSServer() {
        m_pWebSocketServer = new QWebSocketServer(QStringLiteral("Echo Server"), QWebSocketServer::NonSecureMode, this);
        
        int port{12345};


        qInfo() << "websocket server start ..";
        if (m_pWebSocketServer->listen(QHostAddress::Any, port)) {
            qDebug() << "Echoserver listening on port" << port;
            connect(m_pWebSocketServer, &QWebSocketServer::newConnection,
                    this, [=](){
                        QWebSocket *pSocket = m_pWebSocketServer->nextPendingConnection();

                        qInfo() << QString("ws-server: new connection [%1:%2]").arg(pSocket->peerAddress().toString()).arg(pSocket->peerPort());

                        connect(pSocket, &QWebSocket::textMessageReceived, this, &BackendSync::processTextMessage);
                        connect(pSocket, &QWebSocket::binaryMessageReceived, this, &BackendSync::processBinaryMessage);
                        connect(pSocket, &QWebSocket::disconnected, this, &BackendSync::socketDisconnected);

                        m_clients << pSocket;


                    });
            connect(m_pWebSocketServer, &QWebSocketServer::closed, this, &BackendSync::serverClosed);
        }
    }

    void initUdpSocket() {


        for(const auto& addr : QNetworkInterface::allAddresses()) {
            const auto& ipv4 = addr.toIPv4Address();
            qInfo() << addr.toIPv4Address() << QHostAddress(addr.toIPv4Address()).toString();
        }


        qInfo() << "init UDP Socket";
        m_socket = new QUdpSocket();
        bool res = m_socket->bind(QHostAddress::Any, 9999);
        qInfo() << "bind result : " << res;
        connect(m_socket, &QUdpSocket::connected, this, [=](){
            qInfo() << "connected";
            //m_socket->writeDatagram("hello", m_socket->peerAddress(), m_socket->peerPort());
            m_socket->writeDatagram("hello", QHostAddress("192.168.123.30"), 9999);
        });
        connect(m_socket, &QUdpSocket::readyRead, this, &BackendSync::onUdpReadyRead);
    }

signals:
    void closed();
    void serverClosed();

    void receivedMessage(QHostAddress addr, quint16 port, QString msg);

protected slots:
    void onUdpReadyRead();

    //server slots
    void processTextMessage(QString message);
    void processBinaryMessage(QByteArray message);
    void socketDisconnected();

private slots:
    void onConnected();
    void onTextMessageReceived(QString message);

private:
    //clients
    QWebSocket m_webSocket;

    //server
    QWebSocketServer *m_pWebSocketServer;
    QList<QWebSocket *> m_clients;

    QUrl m_url;

    QUdpSocket* m_socket;

};



#endif