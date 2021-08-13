#ifndef __OBC_ROOM_SERVICE_H__
#define __OBC_ROOM_SERVICE_H__

#include <QObject>
#include <QJsonObject>

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
class RoomProviderItem : public QObject{
    Q_OBJECT

    Q_PROPERTY(QString roomName READ getRoomName NOTIFY roomNameChanged)
    Q_PROPERTY(QString address READ getAddr NOTIFY addressChanged)

    //QML_ELEMENT
public:


    QString getRoomName() { return m_roomName; }
    QString getAddr() { return m_addr.toString(); }

    RoomProviderItem(const QHostAddress& addr, quint16 port) : m_addr(addr), m_port(port), m_roomName(addr.toString() + "'s room") {}
    RoomProviderItem(const QString& addr, quint16 port) : m_addr(addr), m_port(port), m_roomName(addr + "'s room") {}

    RoomProviderItem() {}

signals:
    void roomNameChanged();
    void addressChanged();

protected:
    QString m_roomName;
    QHostAddress m_addr;
    quint16 m_port;
};
//Q_DECLARE_METATYPE(RoomProviderItem*)
//Q_DECLARE_METATYPE(QQmlListProperty<RoomProviderItem>)


/**
 * 提供房间服务
 */
class RoomService : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool roomCreated READ getRoomCreated WRITE setRoomCreated)

    Q_PROPERTY(QQmlListProperty<RoomProviderItem> roomItems READ getRoomItems NOTIFY roomItemsChanged)
    // NOTIFY elementsChanged

public:
    bool getRoomCreated() { return m_roomCreated; }
    void setRoomCreated(bool status) { m_roomCreated = status; }

    void addRoomItem(RoomProviderItem* item) {
        m_roomItems.push_back(item);
        emit roomItemsChanged();
    }

    RoomService(QObject* parent = nullptr) : QObject(parent), backendSync(BackendSync::getIns()) {
        
        m_roomItems.append(new RoomProviderItem("127.0.0.1", 5556));
        m_roomItems.append(new RoomProviderItem("192.168.1.1", 5557));

        
        connect(backendSync, &BackendSync::receivedMessage, this, [=](QHostAddress addr, quint16 port, QString msg) {
            parseAyncMessage(addr, port, msg);
        });

    }
    ~RoomService() {
        qInfo() << "Room Service Destory";
    }

    Q_INVOKABLE bool createRoom() {
        
        qInfo() << "createRoom";

        QJsonObject obj;

        obj.insert("type", "room");
        obj.insert("ip", backendSync->localAddress());
        obj.insert("port", 9999);
        backendSync->boardcast(JsonToString(obj));
        
        return true;
    }

    QQmlListProperty<RoomProviderItem> getRoomItems() {
        return QQmlListProperty<RoomProviderItem>(this, &m_roomItems);
    }


    //Q_INVOKABLE bool joinRoom();
    

signals:
    void roomItemsChanged();

protected slots:
    void parseAyncMessage(QHostAddress addr, quint16 port, QString msg) {
        QJsonObject obj = JsonFromString(msg);
        if(obj.isEmpty())
        {
            qInfo() << QString("[%1:%2] message invalid.").arg(addr.toString()).arg(port).toStdString().c_str();
            return;
        }

        //需要增加维护列表的函数 不允许重复添加房间..而且需要定时器维护，定时删除可能关掉的房间?(因为当前没加入)
        if(obj.contains("type") && obj["type"] == "room") {
            qInfo() << QString("[%1:%2] room boardcast.").arg(addr.toString()).arg(port).toStdString().c_str();
            addRoomItem(new RoomProviderItem{addr.toString(), port});
        }
    }

private:
    bool m_roomCreated = false;

    //这个数据结构似乎不太合适..还是使用model，改用map比较好
    QList<RoomProviderItem*> m_roomItems;

    BackendSync* backendSync;

};

#endif