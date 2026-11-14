#ifndef CAMPUS_H
#define CAMPUS_H

#include <string>
#include "HostelGraph.h"
#include "Room.h"

namespace Campus
{
    const int HOSTEL_COUNT = 5;
    const int FLOORS_PER_HOSTEL = 5;
    const int ROOMS_PER_FLOOR = 10;

    const std::string HOSTEL_NAMES[HOSTEL_COUNT] = {
        "Atlantis Hostel",
        "Sai Hostel",
        "Mother Teresa",
        "Subhash Chandra Bose Hostel",
        "Bhagat Singh Hostel"
    };

    const std::string MAIN_GATE = "Main Gate";
    const std::string MESS = "Mess";
    const std::string MAINTENANCE_OFFICE = "Maintenance Office";

    std::string entranceName(const std::string& hostel);
    std::string stairsName(const std::string& hostel, int floor);
    std::string roomName(const std::string& hostel, int floor, int number);
    RoomType sharingFor(int floor);
    bool hasACFor(int floor, int number);

    bool build(HostelGraph& graph, RoomRegistry& rooms);
}

#endif
