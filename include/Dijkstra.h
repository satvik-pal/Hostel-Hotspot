#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "DynamicArray.h"
#include "MinHeap.h"

// Larger than any real route length in the hostel (metres).
const int UNREACHABLE_DISTANCE = 1000000000;

// Result of a shortest-route search.
struct RouteResult
{
    bool reachable = false;          // false if no route exists (or invalid input)
    int distance = -1;               // total length in metres, -1 if not reachable
    DynamicArray<int> path;          // vertex numbers from source to destination
};

// One entry in the min-heap: "vertex can be reached with this distance".
struct HeapEntry
{
    int distance;
    int vertex;

    bool operator<(const HeapEntry& other) const
    {
        if (distance != other.distance)
        {
            return distance < other.distance;
        }
        return vertex < other.vertex;          // tie-break: makes results deterministic
    }
};

// Dijkstra's shortest-path algorithm.
//
// Works with ANY graph class that provides:
//   int  getVertexCount() const;
//   int  getEdgeCount(int vertex) const;
//   Edge getEdge(int vertex, int i) const;    // Edge has .destination and .weight
//
// Requirement: all edge weights must be >= 0.
// Time: O((V + E) log V)    Space: O(V + E)
template <typename Graph>
RouteResult findShortestRoute(const Graph& graph, int source, int destination)
{
    RouteResult result;
    int vertexCount = graph.getVertexCount();

    // Invalid locations: no route
    if (source < 0 || source >= vertexCount ||
        destination < 0 || destination >= vertexCount)
    {
        return result;
    }

    // Step 1: every distance starts as "unreachable", no parents yet
    DynamicArray<int> dist(vertexCount, UNREACHABLE_DISTANCE);
    DynamicArray<int> parent(vertexCount, -1);

    dist[source] = 0;

    MinHeap<HeapEntry> heap;
    heap.push({0, source});

    // Step 2: always expand the closest vertex not yet finalised
    HeapEntry current{0, 0};
    while (heap.pop(current))
    {
        int u = current.vertex;

        if (current.distance > dist[u])
        {
            continue;                           // outdated entry: a shorter route was already found
        }
        if (u == destination)
        {
            break;                              // destination's distance is now final
        }

        // Step 3: relaxation - try to improve each neighbour through u
        for (int i = 0; i < graph.getEdgeCount(u); i++)
        {
            auto edge = graph.getEdge(u, i);
            int v = edge.destination;
            int newDistance = dist[u] + edge.weight;

            if (newDistance < dist[v])
            {
                dist[v] = newDistance;
                parent[v] = u;
                heap.push({newDistance, v});
            }
        }
    }

    // Step 4: no route found
    if (dist[destination] == UNREACHABLE_DISTANCE)
    {
        return result;
    }

    // Step 5: rebuild the route by following parents back from the destination
    for (int v = destination; v != -1; v = parent[v])
    {
        result.path.push(v);
    }
    result.path.reverse();

    result.reachable = true;
    result.distance = dist[destination];
    return result;
}

#endif