#include "BackendSync.hpp"

void BackendSync::onConnected() {
    connect(&m_webSocket, &QWebSocket::textMessageReceived,
        this, &BackendSync::onTextMessageReceived);
    m_webSocket.sendTextMessage(QStringLiteral("Hello, world!"));
}

void BackendSync::onTextMessageReceived(QString message) {
    qDebug() << "Message received:" << message;

    
}
