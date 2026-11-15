#ifndef HOSTEL_GRAPH_H
#define HOSTEL_GRAPH_H

#include <string>
#include "DynamicArray.h"
#include "HashMap.h"

struct Edge
{
    int destination;
    int weight;
};

class HostelGraph
{
private:
    DynamicArray<std::string> names;
    DynamicArray<DynamicArray<Edge>> adjacency;
    HashMap<int> indexByName;

public:
    int addLocation(const std::string& name);
    bool addPath(int u, int v, int distance);
    bool addPathByName(const std::string& from,
                       const std::string& to, int distance);

    int getVertexCount() const;
    int getEdgeCount(int vertex) const;
    Edge getEdge(int vertex, int i) const;
    int findVertex(const std::string& name) const;
    std::string getName(int vertex) const;
};

#endif
