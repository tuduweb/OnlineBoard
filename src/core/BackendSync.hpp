#ifndef __OBC_BACKENDSYNC_H__
#define __OBC_BACKENDSYNC_H__

#include <QObject>
#include <QtWebSockets/QWebSocket>

#include <QUrl>
#include <QThread>
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
    }

    ~BackendSync()
    {
        m_webSocket.close();
    }

public:
    void sendMessasge(QString message) {
        m_webSocket.sendTextMessage(message);
    }

signals:
    void closed();

private slots:
    void onConnected();
    void onTextMessageReceived(QString message);

private:
    QWebSocket m_webSocket;
    QUrl m_url;
};

#endif