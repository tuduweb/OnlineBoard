#ifndef __OBC_BACKENDSYNC_H__
#define __OBC_BACKENDSYNC_H__

#include <QObject>
#include <QtWebSockets/QWebSocket>

#include <QUrl>
//#include <QThread>

#include <QNetworkInterface>
#include <QUdpSocket>
#include <QHostAddress>

class BackendSync : public QObject
{
    Q_OBJECT

public:
    BackendSync()
    {
        //QUrl url = "ws://192.168.123.243:3000";
        QUrl url = QStringLiteral("ws://192.168.123.30:3000");

        connect(&m_webSocket, &QWebSocket::connected, this, &BackendSync::onConnected);
        connect(&m_webSocket, &QWebSocket::disconnected, this, &BackendSync::closed);

        connect(&m_webSocket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
                [=](QAbstractSocket::SocketError error) { 
                    qInfo() << "ws error : " << error << m_webSocket.errorString();
                 });

        m_webSocket.open(url);


        m_socket = new QUdpSocket();
        m_socket->bind(QHostAddress::Any, 9999);
        connect(m_socket, &QUdpSocket::readyRead, this, [=](){
            QString data;
            char dat[1024] = "";
            m_socket->readDatagram(dat, 1024);
            data = dat;
            qInfo() << "read udp : " << dat;
        });

    }

    ~BackendSync()
    {
        m_webSocket.close();
    }

public:
    void sendMessasge(QString message) {
        m_webSocket.sendTextMessage(message);
        boardcast(message);
    }

    void boardcast(QString msg) {
        QByteArray message = msg.toUtf8();
        m_socket->writeDatagram(message, QHostAddress::Broadcast, 9999);
    }

signals:
    void closed();

private slots:
    void onConnected();
    void onTextMessageReceived(QString message);

private:
    QWebSocket m_webSocket;
    QUrl m_url;

    QUdpSocket* m_socket;

};

#endif