#include <iostream>
#include <string>
#include "Campus.h"

using namespace std;

string roomTypeName(RoomType type)
{
    switch (type)
    {
        case RoomType::Single: return "Single";
        case RoomType::Double: return "Double";
        case RoomType::Triple: return "Triple";
    }
    return "Unknown";
}

int main()
{
    HostelGraph graph;
    RoomRegistry rooms;

    bool success = Campus::build(graph, rooms);

    cout << "========== HOSTELPULSE CAMPUS ==========\n";
    cout << "Campus build: " << (success ? "SUCCESS" : "FAILED") << "\n\n";

    if (!success)
    {
        return 1;
    }

    int directedEntries = 0;
    for (int i = 0; i < graph.getVertexCount(); ++i)
    {
        directedEntries += graph.getEdgeCount(i);
    }

    cout << "Hostels: " << Campus::HOSTEL_COUNT << '\n';
    cout << "Floors per hostel: " << Campus::FLOORS_PER_HOSTEL << '\n';
    cout << "Rooms per floor: " << Campus::ROOMS_PER_FLOOR << '\n';
    cout << "Total locations: " << graph.getVertexCount() << '\n';
    cout << "Total paths: " << directedEntries / 2 << '\n';
    cout << "Total rooms: " << rooms.getRoomCount() << "\n\n";

    cout << "HOSTEL LIST\n";
    for (int h = 0; h < Campus::HOSTEL_COUNT; ++h)
    {
        cout << h + 1 << ". " << Campus::HOSTEL_NAMES[h] << '\n';
    }

    cout << "\nADJACENCY LIST - ATLANTIS HOSTEL FLOOR 2\n";
    const string prefix = "Atlantis Hostel 2";

    for (int i = 0; i < graph.getVertexCount(); ++i)
    {
        string name = graph.getName(i);

        if (name.rfind(prefix, 0) == 0)
        {
            cout << name << " -> ";

            for (int j = 0; j < graph.getEdgeCount(i); ++j)
            {
                Edge edge = graph.getEdge(i, j);
                cout << graph.getName(edge.destination)
                     << " (" << edge.weight << "m) ";
            }
            cout << '\n';
        }
    }

    cout << "\nROOM DETAILS\n";
    const Room* room = rooms.findRoom("Sai Hostel 204");

    if (room != nullptr)
    {
        cout << "Room: " << room->name << '\n';
        cout << "Hostel: " << room->hostel << '\n';
        cout << "Floor: " << room->floor << '\n';
        cout << "Room number on floor: " << room->number << '\n';
        cout << "Sharing: " << roomTypeName(room->sharing) << '\n';
        cout << "AC: " << (room->hasAC ? "Yes" : "No") << '\n';
    }

    return 0;
}
