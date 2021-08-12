#ifndef __OBC_ROOM_SERVICE_H__
#define __OBC_ROOM_SERVICE_H__

#include <QObject>

class RoomService : QObject
{
    Q_OBJECT

public:
    RoomService() {
        //
    }
    ~RoomService();

    Q_INVOKABLE bool createRoom() {
        
        //
        
        return true;
    }
    //Q_INVOKABLE bool joinRoom();

};

#endif