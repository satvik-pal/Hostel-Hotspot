#include "Room.h"

bool RoomRegistry::addRoom(const Room& room)
{
    if (room.name.empty())
    {
        return false;
    }
    return rooms.insert(room.name, room);
}

const Room* RoomRegistry::findRoom(const std::string& name) const
{
    return rooms.find(name);
}

int RoomRegistry::getRoomCount() const
{
    return rooms.size();
}
