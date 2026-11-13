#include "HostelGraph.h"

int HostelGraph::addLocation(const std::string& name)
{
    if (name.empty() || indexByName.contains(name))
    {
        return -1;
    }

    int index = names.size();
    names.push(name);
    adjacency.push(DynamicArray<Edge>());
    indexByName.insert(name, index);
    return index;
}

bool HostelGraph::addPath(int u, int v, int distance)
{
    if (u < 0 || v < 0 ||
        u >= getVertexCount() || v >= getVertexCount() ||
        u == v || distance <= 0)
    {
        return false;
    }

    for (int i = 0; i < adjacency[u].size(); ++i)
    {
        if (adjacency[u][i].destination == v)
        {
            return false;
        }
    }

    adjacency[u].push(Edge{v, distance});
    adjacency[v].push(Edge{u, distance});
    return true;
}

bool HostelGraph::addPathByName(const std::string& from,
                                const std::string& to, int distance)
{
    return addPath(findVertex(from), findVertex(to), distance);
}

int HostelGraph::getVertexCount() const
{
    return names.size();
}

int HostelGraph::getEdgeCount(int vertex) const
{
    if (vertex < 0 || vertex >= getVertexCount())
    {
        return 0;
    }
    return adjacency[vertex].size();
}

Edge HostelGraph::getEdge(int vertex, int i) const
{
    if (vertex < 0 || vertex >= getVertexCount() ||
        i < 0 || i >= adjacency[vertex].size())
    {
        return Edge{-1, 0};
    }
    return adjacency[vertex][i];
}

int HostelGraph::findVertex(const std::string& name) const
{
    const int* index = indexByName.find(name);
    return (index == nullptr) ? -1 : *index;
}

std::string HostelGraph::getName(int vertex) const
{
    if (vertex < 0 || vertex >= getVertexCount())
    {
        return "";
    }
    return names[vertex];
}
