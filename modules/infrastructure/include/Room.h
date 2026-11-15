#ifndef ROOM_H
#define ROOM_H

#include <string>
#include "HashMap.h"

enum class RoomType
{
    Single = 1,
    Double = 2,
    Triple = 3
};

struct Room
{
    std::string name;
    std::string hostel;
    int floor;
    int number;
    RoomType sharing;
    bool hasAC;
};

class RoomRegistry
{
private:
    HashMap<Room> rooms;

public:
    bool addRoom(const Room& room);
    const Room* findRoom(const std::string& name) const;
    int getRoomCount() const;

    template <typename Action>
    void forEachRoom(Action action) const
    {
        rooms.forEach([&](const std::string&, const Room& room)
        {
            action(room);
        });
    }
};

#endif
