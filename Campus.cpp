#include "Campus.h"

namespace
{
    const int MAIN_GATE_TO_OFFICE = 30;
    const int MAIN_GATE_TO_MESS = 80;
    const int MAIN_GATE_TO_FIRST_ENTRANCE = 60;
    const int ENTRANCE_TO_ENTRANCE = 70;
    const int MESS_TO_MIDDLE_ENTRANCE = 50;
    const int ENTRANCE_TO_FIRST_STAIRS = 10;
    const int STAIRS_BETWEEN_FLOORS = 6;
    const int STAIRS_TO_FIRST_ROOM = 5;
    const int BETWEEN_ROOMS = 4;
}

namespace Campus
{
    std::string entranceName(const std::string& hostel)
    {
        return hostel + " Entrance";
    }

    std::string stairsName(const std::string& hostel, int floor)
    {
        return hostel + " Stairs F" + std::to_string(floor);
    }

    std::string roomName(const std::string& hostel, int floor, int number)
    {
        std::string roomNumber = (number < 10 ? "0" : "") +
                                 std::to_string(number);
        return hostel + " " + std::to_string(floor) + roomNumber;
    }

    RoomType sharingFor(int floor)
    {
        if (floor >= 1 && floor <= 2)
        {
            return RoomType::Triple;
        }
        if (floor >= 3 && floor <= 4)
        {
            return RoomType::Double;
        }
        if (floor == 5)
        {
            return RoomType::Single;
        }
        return RoomType::Single;
    }

    bool hasACFor(int floor, int number)
    {
        if (floor == 3 || floor == 4)
        {
            return number >= 1 && number <= 5;
        }
        return floor == 5;
    }

    bool build(HostelGraph& graph, RoomRegistry& rooms)
    {
        if (graph.addLocation(MAIN_GATE) == -1 ||
            graph.addLocation(MESS) == -1 ||
            graph.addLocation(MAINTENANCE_OFFICE) == -1)
        {
            return false;
        }

        if (!graph.addPathByName(MAIN_GATE, MAINTENANCE_OFFICE,
                                 MAIN_GATE_TO_OFFICE) ||
            !graph.addPathByName(MAIN_GATE, MESS, MAIN_GATE_TO_MESS))
        {
            return false;
        }

        for (int h = 0; h < HOSTEL_COUNT; ++h)
        {
            const std::string& hostel = HOSTEL_NAMES[h];
            std::string entrance = entranceName(hostel);

            if (graph.addLocation(entrance) == -1)
            {
                return false;
            }

            if (h == 0)
            {
                if (!graph.addPathByName(MAIN_GATE, entrance,
                                         MAIN_GATE_TO_FIRST_ENTRANCE))
                {
                    return false;
                }
            }
            else
            {
                if (!graph.addPathByName(
                        entranceName(HOSTEL_NAMES[h - 1]),
                        entrance, ENTRANCE_TO_ENTRANCE))
                {
                    return false;
                }
            }

            for (int floor = 1; floor <= FLOORS_PER_HOSTEL; ++floor)
            {
                std::string stairs = stairsName(hostel, floor);

                if (graph.addLocation(stairs) == -1)
                {
                    return false;
                }

                if (floor == 1)
                {
                    if (!graph.addPathByName(entrance, stairs,
                                             ENTRANCE_TO_FIRST_STAIRS))
                    {
                        return false;
                    }
                }
                else
                {
                    if (!graph.addPathByName(
                            stairsName(hostel, floor - 1),
                            stairs, STAIRS_BETWEEN_FLOORS))
                    {
                        return false;
                    }
                }

                for (int number = 1; number <= ROOMS_PER_FLOOR; ++number)
                {
                    std::string name = roomName(hostel, floor, number);

                    if (graph.addLocation(name) == -1)
                    {
                        return false;
                    }

                    if (number == 1)
                    {
                        if (!graph.addPathByName(stairs, name,
                                                 STAIRS_TO_FIRST_ROOM))
                        {
                            return false;
                        }
                    }
                    else
                    {
                        if (!graph.addPathByName(
                                roomName(hostel, floor, number - 1),
                                name, BETWEEN_ROOMS))
                        {
                            return false;
                        }
                    }

                    Room room{
                        name,
                        hostel,
                        floor,
                        number,
                        sharingFor(floor),
                        hasACFor(floor, number)
                    };

                    if (!rooms.addRoom(room))
                    {
                        return false;
                    }
                }
            }
        }

        const std::string& middleHostel = HOSTEL_NAMES[HOSTEL_COUNT / 2];

        if (!graph.addPathByName(MESS, entranceName(middleHostel),
                                 MESS_TO_MIDDLE_ENTRANCE))
        {
            return false;
        }

        return true;
    }
}
