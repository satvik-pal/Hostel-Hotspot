#include <iostream>
#include <string>
#include "Campus.h"

namespace
{
    const int NOT_REACHED = 1000000000;

    // Simple O(V^2) shortest-distance check, independent of the Maintenance module.
    int shortestDistance(const HostelGraph& graph, int source, int target)
    {
        int count = graph.getVertexCount();
        DynamicArray<int> dist(count, NOT_REACHED);
        DynamicArray<bool> done(count, false);
        dist[source] = 0;

        for (int step = 0; step < count; ++step)
        {
            int u = -1;
            for (int v = 0; v < count; ++v)
            {
                if (!done[v] && (u == -1 || dist[v] < dist[u]))
                {
                    u = v;
                }
            }
            if (u == -1 || dist[u] == NOT_REACHED)
            {
                break;
            }
            done[u] = true;

            for (int i = 0; i < graph.getEdgeCount(u); ++i)
            {
                Edge edge = graph.getEdge(u, i);
                if (dist[u] + edge.weight < dist[edge.destination])
                {
                    dist[edge.destination] = dist[u] + edge.weight;
                }
            }
        }
        return dist[target];
    }
}

int main()
{
    int passed = 0;
    int failed = 0;

    auto check = [&](bool condition, const std::string& description)
    {
        if (condition)
        {
            ++passed;
            std::cout << "[PASS] " << description << '\n';
        }
        else
        {
            ++failed;
            std::cout << "[FAIL] " << description << '\n';
        }
    };

    {
        HostelGraph graph;
        int a = graph.addLocation("A");
        int b = graph.addLocation("B");

        check(a == 0 && b == 1 && graph.getVertexCount() == 2,
              "Locations receive sequential indexes");
        check(graph.addLocation("") == -1 &&
              graph.addLocation("A") == -1,
              "Empty and duplicate locations are rejected");
        check(graph.findVertex("B") == 1 &&
              graph.findVertex("Missing") == -1,
              "Name lookup works");
        check(graph.getName(0) == "A" &&
              graph.getName(-1).empty() &&
              graph.getName(2).empty(),
              "Name retrieval validates indexes");
        check(graph.addPath(a, b, 7),
              "Valid path can be added");
        check(graph.getEdgeCount(a) == 1 &&
              graph.getEdgeCount(b) == 1 &&
              graph.getEdge(a, 0).destination == b &&
              graph.getEdge(b, 0).destination == a &&
              graph.getEdge(a, 0).weight == 7,
              "Path is stored in both directions");
        check(!graph.addPath(-1, b, 2) &&
              !graph.addPath(a, 2, 2) &&
              !graph.addPath(a, a, 2) &&
              !graph.addPath(a, b, 0) &&
              !graph.addPath(a, b, -3),
              "Invalid paths are rejected");
        check(!graph.addPath(a, b, 9),
              "Duplicate path is rejected");
        check(!graph.addPathByName("A", "Missing", 3),
              "Unknown path endpoint is rejected");
        check(graph.getEdgeCount(-1) == 0 &&
              graph.getEdgeCount(2) == 0,
              "Out-of-range edge count returns zero");
    }

    {
        HostelGraph graph;
        bool allAdded = true;
        for (int i = 0; i < 1000; ++i)
        {
            std::string name = "L" + std::to_string(i);
            if (graph.addLocation(name) != i)
            {
                allAdded = false;
            }
        }

        bool allFound = true;
        for (int i = 0; i < 1000; ++i)
        {
            std::string name = "L" + std::to_string(i);
            if (graph.findVertex(name) != i)
            {
                allFound = false;
            }
        }
        check(allAdded && allFound,
              "Graph and hash lookup grow to 1000 locations");
    }

    {
        RoomRegistry registry;
        Room sample{"Test 101", "Test", 1, 1, RoomType::Triple, false};
        check(registry.addRoom(sample), "New room can be registered");
        check(!registry.addRoom(sample), "Duplicate room is rejected");
        Room empty{"", "Test", 1, 2, RoomType::Triple, false};
        check(!registry.addRoom(empty), "Empty room name is rejected");
        check(registry.findRoom("Missing") == nullptr,
              "Missing room returns nullptr");
    }

    check(Campus::roomName("Marina", 2, 4) == "Marina 204" &&
          Campus::roomName("Marina", 2, 10) == "Marina 210",
          "Room naming uses two-digit room numbers");
    check(Campus::stairsName("Marina", 2) == "Marina Stairs F2" &&
          Campus::entranceName("Marina") == "Marina Entrance",
          "Entrance and stair naming is correct");

    {
        HostelGraph graph;
        RoomRegistry rooms;
        bool built = Campus::build(graph, rooms);

        check(built, "Campus builds successfully");
        check(graph.getVertexCount() == 283,
              "Campus has 283 locations");

        int edgeEntries = 0;
        for (int i = 0; i < graph.getVertexCount(); ++i)
        {
            edgeEntries += graph.getEdgeCount(i);
        }

        check(edgeEntries == 566 && edgeEntries / 2 == 283,
              "Campus has 283 undirected paths");
        check(rooms.getRoomCount() == 250,
              "Campus has 250 rooms");

        int hostelRoomCounts[Campus::HOSTEL_COUNT] = {0, 0, 0, 0, 0};
        rooms.forEachRoom([&](const Room& room)
        {
            for (int h = 0; h < Campus::HOSTEL_COUNT; ++h)
            {
                if (room.hostel == Campus::HOSTEL_NAMES[h])
                {
                    ++hostelRoomCounts[h];
                }
            }
        });

        bool fiftyEach = true;
        for (int h = 0; h < Campus::HOSTEL_COUNT; ++h)
        {
            if (hostelRoomCounts[h] != 50)
            {
                fiftyEach = false;
            }
        }
        check(fiftyEach, "Each hostel has 50 rooms");

        int marina204 = graph.findVertex("Sai Hostel 204");
        bool neighboursCorrect = false;
        if (marina204 != -1 && graph.getEdgeCount(marina204) == 2)
        {
            Edge e0 = graph.getEdge(marina204, 0);
            Edge e1 = graph.getEdge(marina204, 1);
            std::string n0 = graph.getName(e0.destination);
            std::string n1 = graph.getName(e1.destination);
            neighboursCorrect =
                ((n0 == "Sai Hostel 203" && n1 == "Sai Hostel 205") ||
                 (n0 == "Sai Hostel 205" && n1 == "Sai Hostel 203")) &&
                e0.weight == 4 && e1.weight == 4;
        }
        check(neighboursCorrect,
              "A middle room connects to previous and next room");

        const Room* triple = rooms.findRoom("Sai Hostel 204");
        const Room* doubleNoAc = rooms.findRoom("Mother Teresa 307");
        const Room* doubleAc = rooms.findRoom("Mother Teresa 303");
        const Room* singleAc = rooms.findRoom("Bhagat Singh Hostel 510");

        check(triple != nullptr &&
              triple->sharing == RoomType::Triple && !triple->hasAC,
              "Floor 2 room is triple sharing and non-AC");
        check(doubleNoAc != nullptr &&
              doubleNoAc->sharing == RoomType::Double &&
              !doubleNoAc->hasAC,
              "Floor 3 room 07 is double sharing and non-AC");
        check(doubleAc != nullptr &&
              doubleAc->sharing == RoomType::Double &&
              doubleAc->hasAC,
              "Floor 3 room 03 is double sharing and AC");
        check(singleAc != nullptr &&
              singleAc->sharing == RoomType::Single &&
              singleAc->hasAC,
              "Floor 5 room is single sharing and AC");
    }

    {
        HostelGraph graph;
        RoomRegistry rooms;
        Campus::build(graph, rooms);

        // Breadth-first traversal from Main Gate, using DynamicArray as a queue
        DynamicArray<bool> visited(graph.getVertexCount(), false);
        DynamicArray<int> queue;
        int start = graph.findVertex(Campus::MAIN_GATE);
        queue.push(start);
        visited[start] = true;

        int head = 0;
        while (head < queue.size())
        {
            int u = queue[head];
            ++head;
            for (int i = 0; i < graph.getEdgeCount(u); ++i)
            {
                int v = graph.getEdge(u, i).destination;
                if (!visited[v])
                {
                    visited[v] = true;
                    queue.push(v);
                }
            }
        }
        check(queue.size() == graph.getVertexCount(),
              "Every location is reachable from Main Gate");

        // Contract distances (hostel names taken from Campus::HOSTEL_NAMES,
        // so these tests keep working if the names change)
        auto distanceBetween = [&](const std::string& from, const std::string& to)
        {
            return shortestDistance(graph, graph.findVertex(from), graph.findVertex(to));
        };
        const std::string* H = Campus::HOSTEL_NAMES;

        check(distanceBetween(Campus::MAINTENANCE_OFFICE, Campus::roomName(H[1], 2, 4)) == 193,
              "Maintenance Office -> second hostel room 204 is 193 m");
        check(distanceBetween(Campus::MAINTENANCE_OFFICE, Campus::roomName(H[0], 1, 1)) == 105,
              "Maintenance Office -> first hostel room 101 is 105 m");
        check(distanceBetween(Campus::MESS, Campus::roomName(H[2], 3, 5)) == 93,
              "Mess -> middle hostel room 305 is 93 m");
        check(distanceBetween(Campus::roomName(H[1], 2, 4), Campus::roomName(H[1], 2, 10)) == 24,
              "Room 204 -> room 210 on the same floor is 24 m");
        check(distanceBetween(Campus::roomName(H[1], 1, 1), Campus::roomName(H[1], 5, 1)) == 34,
              "Room 101 -> room 501 in the same hostel is 34 m");
        check(distanceBetween(Campus::roomName(H[0], 5, 10), Campus::roomName(H[4], 1, 1)) == 370,
              "First hostel room 510 -> last hostel room 101 is 370 m");
    }

    std::cout << "\nResult: " << passed << " passed, "
              << failed << " failed\n";

    return (failed == 0) ? 0 : 1;
}
