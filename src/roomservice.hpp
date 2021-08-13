#ifndef __OBC_ROOM_SERVICE_H__
#define __OBC_ROOM_SERVICE_H__

#include <QObject>
#include <QJsonObject>

//BASE lib
#include "utils/OBCHelpers.hpp"

#include "core/BackendSync.hpp"


class RoomService : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool roomCreated READ getRoomCreated WRITE setRoomCreated)
public:
    bool getRoomCreated() { return m_roomCreated; }
    void setRoomCreated(bool status) { m_roomCreated = status; }

    RoomService(QObject* parent = nullptr) : QObject(parent), backendSync(BackendSync::getIns()) {
        //
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
    //Q_INVOKABLE bool joinRoom();
    BackendSync* backendSync;

private:
    bool m_roomCreated = false;
};

#endif