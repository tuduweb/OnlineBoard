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

    BackendSync() : m_pWebSocketServer(nullptr), m_socket(nullptr), m_webSocket(nullptr)
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
        if(m_webSocket)
            m_webSocket->close();
    }

public:
    QString localAddress() {
        //可能需要加入判断..
        return QHostAddress(m_socket->localAddress()).toString();
    }
    /**
     * 发送消息给远程服务器
     */
    void sendMessasge(const QString& message) {
        //Q_ASSERT(m_webSocket != nullptr);

        if(m_webSocket) {
            m_webSocket->sendTextMessage(message);
        }else{
            serverBoardcast(message);
        }
       
        //boardcast(message);
    }

    void boardcast(const QString& msg) {
        QByteArray message = msg.toUtf8();
        m_socket->writeDatagram(message, QHostAddress::Broadcast, 21817);
    }

    void boardcast(const QString& msg, quint16 port) {
        QByteArray message = msg.toUtf8();
        m_socket->writeDatagram(message, QHostAddress::Broadcast, port);
    }

    void serverBoardcast(const QString& msg) {
        qInfo() << "===============================================";
        qInfo() << "send message : " << msg;
        for(const auto& client : m_clients) {
            qInfo() << "send to " << client->peerAddress().toString(); 
            client->sendTextMessage(msg);
        }
        qInfo() << "***********************************************";

    }

    void sendUdpMessage(const QString& msg, const QHostAddress& addr, quint16 port) {
        QByteArray message = msg.toUtf8();
        m_socket->writeDatagram(message, addr, port);
    }

    void initWSClient() {

        if(m_webSocket)
        {
            qInfo() << "websocket client already exist!";
            return;
        }
        m_webSocket = new QWebSocket;

        //QUrl url = "ws://192.168.123.243:3000";
        QUrl url = QStringLiteral("ws://192.168.123.30:3000");

        connect(m_webSocket, &QWebSocket::connected, this, &BackendSync::onConnected);
        connect(m_webSocket, &QWebSocket::disconnected, this, &BackendSync::closed);

        connect(m_webSocket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
                [=](QAbstractSocket::SocketError error) { 
                    qInfo() << "ws error : " << error << m_webSocket->errorString();
                 });

        m_webSocket->open(url);
    }

    void initWSClient(const QString& ip, quint16 port) {

        if(m_webSocket)
        {
            qInfo() << "websocket client already exist!";
            //return;
            m_webSocket->close();
            m_webSocket->deleteLater();
        }
        m_webSocket = new QWebSocket;

        QUrl url = QString("ws://%1:%2").arg(ip).arg(port);

        //可能还需要区分一下来源 不然调用多次就connect了多次..
        connect(m_webSocket, &QWebSocket::connected, this, &BackendSync::onConnected);
        connect(m_webSocket, &QWebSocket::connected, this, &BackendSync::connected);
        connect(m_webSocket, &QWebSocket::disconnected, this, &BackendSync::closed);

        connect(m_webSocket, &QWebSocket::stateChanged, this, [=](QAbstractSocket::SocketState state){
            qInfo() << m_webSocket << "state changed : " << state;
        });
        connect(m_webSocket, &QWebSocket::stateChanged, this, &BackendSync::stateChanged);

        //aboutToClose() // 关闭前

        connect(m_webSocket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
                [=](QAbstractSocket::SocketError error) { 
                    qInfo() << "ws error : " << error << m_webSocket->errorString();
                 });

        m_webSocket->open(url);

    }

    bool initWSServer() {
        m_pWebSocketServer = new QWebSocketServer(QStringLiteral("Echo Server"), QWebSocketServer::NonSecureMode, this);
        
        int port{12345};


        qInfo() << "websocket server start ..";
        if (m_pWebSocketServer->listen(QHostAddress::Any, port)) {
            qDebug() << "websocket listening on port" << port;
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
            return true;
        }
        return false;
    }

    void initUdpSocket() {


        for(const auto& addr : QNetworkInterface::allAddresses()) {
            const auto& ipv4 = addr.toIPv4Address();
            qInfo() << addr.toIPv4Address() << QHostAddress(addr.toIPv4Address()).toString();
        }


        qInfo() << "init UDP Socket";
        m_socket = new QUdpSocket();
        bool res = m_socket->bind(QHostAddress::Any, 21817);
        qInfo() << "bind result : " << res;

        //udp有connected的信号? 似乎没有阿
        connect(m_socket, &QUdpSocket::connected, this, [=](){
            qInfo() << "connected";
            m_socket->writeDatagram("hello", m_socket->peerAddress(), m_socket->peerPort());
            //m_socket->writeDatagram("hello", QHostAddress("192.168.123.30"), 9999);
        });
        connect(m_socket, &QUdpSocket::readyRead, this, &BackendSync::onUdpReadyRead);
    }

    QAbstractSocket::SocketState WSState() {
        if(m_webSocket == nullptr)
            return QAbstractSocket::SocketState::UnconnectedState;
        return m_webSocket->state();
    }

signals:
    void connected();
    void closed();
    void serverClosed();
    void stateChanged(QAbstractSocket::SocketState state);

    //接收到udp广播消息 这里需要改个名字..
    void receivedMessage(QHostAddress addr, quint16 port, const QString& msg);

    void serverTextMessageReceived(const QString& message);
    void serverBinaryMessageReceived(const QByteArray& byteArray);
protected slots:
    void onUdpReadyRead();

    //server slots
    void processTextMessage(QString message);
    void processBinaryMessage(QByteArray message);
    void socketDisconnected();

private slots:
    void onConnected();
    //void onClosed();
    void onTextMessageReceived(QString message);

private:
    //clients
    QWebSocket* m_webSocket;

    //server
    QWebSocketServer *m_pWebSocketServer;
    QList<QWebSocket *> m_clients;

    QUrl m_url;

    QUdpSocket* m_socket;

};



#endif