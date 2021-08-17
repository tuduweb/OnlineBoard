#ifndef __OBC_ROOM_SERVICE_H__
#define __OBC_ROOM_SERVICE_H__

#include <QObject>
#include <QJsonObject>
#include <QEventLoop>

//BASE lib
#include "utils/OBCHelpers.hpp"

#include "core/BackendSync.hpp"

#include <QQmlListProperty>
#include <QtQml/qqml.h>

/**
 * 定义广播的房间的数据结构 供..使用
 * 需要注册类型才能使用!!
 * qmlRegisterType<RoomProviderItem>("obc.qml.Controls", 1, 0, "RoomProviderItem");
*/
class RoomProviderItem : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString roomName READ getRoomName NOTIFY roomNameChanged)
    Q_PROPERTY(QString address READ getAddr NOTIFY addressChanged)
    Q_PROPERTY(quint16 port READ getPort NOTIFY portChanged)

    //QML_ELEMENT
public:
    QString getRoomName() { return m_roomName; }
    QString getAddr() { return m_addr.toString(); }
    quint16 getPort() { return m_port; }

    RoomProviderItem(const QHostAddress &addr, quint16 port) : m_addr(QHostAddress(QHostAddress(addr).toIPv4Address()).toString()), m_port(port), m_roomName(addr.toString() + "'s room") {}
    RoomProviderItem(const QString &addr, quint16 port) : m_addr(QHostAddress(QHostAddress(addr).toIPv4Address()).toString()), m_port(port), m_roomName(addr + "'s room") {}

    RoomProviderItem() {}

signals:
    void roomNameChanged();
    void addressChanged();
    void portChanged();

protected:
    QString m_roomName;
    QHostAddress m_addr;
    quint16 m_port;

    QString m_roomHash; //房间校验
};
//Q_DECLARE_METATYPE(RoomProviderItem*)
//Q_DECLARE_METATYPE(QQmlListProperty<RoomProviderItem>)

/**
 * 房间内的用户Item
 */
class RoomUserItem : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString userName READ getUserName NOTIFY userNameChanged)

public:
    RoomUserItem(const QString &userName) : m_userName(userName) {}
    RoomUserItem() {} //强制需要有一个默认方法
    ~RoomUserItem() {}

    QString getUserName() { return m_userName; }

signals:
    void userNameChanged();

private:
    QString m_userName;
    QHostAddress m_userAddress;
    quint16 m_userPort;

    //对应了一个Websocket连接..
};

/**
 * 提供房间服务
 */
class RoomService : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool roomCreated READ getRoomCreated WRITE setRoomCreated)
    Q_PROPERTY(QQmlListProperty<RoomProviderItem> roomItems READ getRoomItems NOTIFY roomItemsChanged)
    // NOTIFY elementsChanged
    Q_PROPERTY(QQmlListProperty<RoomUserItem> userItems READ getUserItems NOTIFY userItemsChanged)

public:
    bool getRoomCreated() { return m_roomCreated; }
    void setRoomCreated(bool status) { m_roomCreated = status; }

    void addRoomItem(RoomProviderItem *item)
    {
        m_roomItems.push_back(item);
        emit roomItemsChanged();
    }

    void addRoomUserItem(RoomUserItem *item)
    {
        m_userItems.push_back(item);
        emit userItemsChanged();
    }

    RoomService(QObject *parent = nullptr) : QObject(parent), backendSync(BackendSync::getIns())
    {

        m_roomItems.append(new RoomProviderItem("127.0.0.1", 5556));
        m_roomItems.append(new RoomProviderItem("192.168.123.30", 12345));

        m_userItems.append(new RoomUserItem{"房主"});

        connect(backendSync, &BackendSync::receivedMessage, this, [=](QHostAddress addr, quint16 port, QString msg)
                { parseAyncMessage(addr, port, msg); });
    }
    ~RoomService()
    {
        qInfo() << "Room Service Destory";
    }

    Q_INVOKABLE bool createRoom()
    {

        qInfo() << "createRoom";

        QJsonObject obj;

        obj.insert("type", "room");
        obj.insert("ip", backendSync->localAddress());
        obj.insert("port", 12345);
        backendSync->boardcast(JsonToString(obj), 21817);

        if (backendSync->initWSServer())
        {

            qInfo() << "init websocket server";

            //////client -> server
            connect(backendSync, &BackendSync::receivedMessage, this, &RoomService::parseClientMessage);

            return true;
        }

        return false;
    }

    //加入房间操作
    Q_INVOKABLE bool joinRoom(const QString& ip, quint16 port)
    {
        qInfo() << ip << port;
        //可能需要权限..所以可能需要弄个状态机..这里默认不弄权限了
        QJsonObject obj;
        obj.insert("type", "room");
        obj.insert("req", "join");
        obj.insert("userName", "user-" + GenerateRandomString(5));

        QEventLoop eventLoop;

        connect(backendSync, &BackendSync::stateChanged, &eventLoop, &QEventLoop::quit);

        //异步初始化 并连接
        backendSync->initWSClient(ip, port);
        qInfo() << "exec state:" << backendSync->WSState();
        eventLoop.exec();
        qInfo() << "quit state:" << backendSync->WSState();

        if (backendSync->WSState() == QAbstractSocket::SocketState::ConnectedState)
        {
            connect(backendSync, &BackendSync::serverTextMessageReceived, this, &RoomService::parseServerMessage);
            backendSync->sendMessasge(JsonToString(obj));
        }

        qInfo() << "eventLoop quit";

        //异步..

        return true;
    }

    Q_INVOKABLE void startPaint()
    {
        //
        QJsonObject obj;
        obj.insert("type", "room");
        obj.insert("req", "start");

        /* 需要修改成以下的方式 */
        // for(const auto& client : m_userItems) {
        //     //
        // }

        //backendSync->boardcast()
        qInfo() << "startPaint boardcast";
        backendSync->serverBoardcast(JsonToString(obj));
    }

    QQmlListProperty<RoomProviderItem> getRoomItems()
    {
        return QQmlListProperty<RoomProviderItem>(this, &m_roomItems);
    }

    QQmlListProperty<RoomUserItem> getUserItems()
    {
        return QQmlListProperty<RoomUserItem>(this, &m_userItems);
    }

    //Q_INVOKABLE bool joinRoom();

signals:
    void roomItemsChanged();
    void userItemsChanged();

    void startPaintReceived();

protected:
    void timerEvent(QTimerEvent *event) override {}

protected slots:
    void parseAyncMessage(QHostAddress addr, quint16 port, QString msg)
    {
        QJsonObject obj = JsonFromString(msg);
        if (obj.isEmpty())
        {
            qInfo() << QString("[%1:%2] message invalid.").arg(addr.toString()).arg(port).toStdString().c_str();
            return;
        }

        //需要增加维护列表的函数 不允许重复添加房间..而且需要定时器维护，定时删除可能关掉的房间?(因为当前没加入)
        if (obj.contains("type") && obj["type"] == "room")
        {
            qInfo() << QString("[%1:%2] room udp boardcast.").arg(addr.toString()).arg(port).toStdString().c_str();
            //这里添加的是ws地址 不是udp地址(地址ip一样) 端口一定要注意
            addRoomItem(new RoomProviderItem{addr.toString(), static_cast<quint16>(obj["port"].toInt())});
        }
    }

    void parseServerMessage(const QString &msg)
    {

        QJsonObject obj = JsonFromString(msg);
        if (obj.isEmpty())
        {
            qInfo() << QString("server message invalid.").toStdString().c_str();
            return;
        }

        //
        if (obj.contains("type") && obj["type"] == "room")
        {

            if (obj.contains("userName"))
            {
                qInfo() << "add user item" << obj["userName"];
                addRoomUserItem(new RoomUserItem{obj["userName"].toString()});
            }

            if (obj.contains("req"))
            {

                if (obj["req"] == "start")
                {
                    //开始画图
                    emit startPaintReceived();
                }
            }
        }
    }

    void parseClientMessage(QHostAddress addr, quint16 port, QString msg)
    {

        QJsonObject obj = JsonFromString(msg);
        if (obj.isEmpty())
        {
            qInfo() << QString("[%1:%2] client message invalid.").arg(addr.toString()).arg(port).toStdString().c_str();
            return;
        }

        //
        if (obj.contains("type") && obj["type"] == "room")
        {

            if (obj.contains("userName"))
            {
                qInfo() << "add user item" << obj["userName"];
                addRoomUserItem(new RoomUserItem{obj["userName"].toString()});
            }
        }
    }

private:
    bool m_roomCreated = false;

    //这个数据结构似乎不太合适..还是使用model，改用map比较好
    QList<RoomProviderItem *> m_roomItems;

    QList<RoomUserItem *> m_userItems;

    BackendSync *backendSync;
};

#endif
