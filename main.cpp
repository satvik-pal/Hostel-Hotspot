
#include <iostream>
#include <cstring>
using namespace std;

#define SIZE 9
#define MAX_EDGES 10
#define NAME_SIZE 30

struct Edge {
    int destination;
    int weight;
};

class HostelGraph {
private:
    char vertices[SIZE][NAME_SIZE];
    Edge adjList[SIZE][MAX_EDGES];
    int edgeCount[SIZE];
    int vertexCount;

public:
    HostelGraph() {
        vertexCount = 0;

        for (int i = 0; i < SIZE; i++) {
            edgeCount[i] = 0;
        }
    }

    void addVertex(const char name[]) {
        if (vertexCount >= SIZE) {
            cout << "Maximum locations reached\n";
            return;
        }

        strcpy(vertices[vertexCount], name);
        vertexCount++;
    }

    void addEdge(int u, int v, int distance) {
        if (u < 0 || v < 0 ||
            u >= vertexCount || v >= vertexCount) {
            cout << "Invalid location index\n";
            return;
        }

        if (edgeCount[u] >= MAX_EDGES ||
            edgeCount[v] >= MAX_EDGES) {
            cout << "Maximum connections reached\n";
            return;
        }

        adjList[u][edgeCount[u]++] = {v, distance};
        adjList[v][edgeCount[v]++] = {u, distance};
    }

    void displayGraph() {
        cout << "\nHOSTEL INFRASTRUCTURE GRAPH\n";

        for (int i = 0; i < vertexCount; i++) {
            cout << vertices[i] << " -> ";

            for (int j = 0; j < edgeCount[i]; j++) {
                int dest = adjList[i][j].destination;

                cout << vertices[dest]
                     << " (" << adjList[i][j].weight
                     << "m)  ";
            }

            cout << endl;
        }
    }
};

int main() {
    HostelGraph hostel;

    hostel.addVertex("Main Gate");
    hostel.addVertex("Reception");
    hostel.addVertex("Room 101");
    hostel.addVertex("Room 102");
    hostel.addVertex("Room 103");
    hostel.addVertex("Mess");
    hostel.addVertex("Common Room");
    hostel.addVertex("Washroom");
    hostel.addVertex("Stairs");

    hostel.addEdge(0, 1, 10);
    hostel.addEdge(1, 2, 15);
    hostel.addEdge(2, 3, 8);
    hostel.addEdge(3, 4, 8);
    hostel.addEdge(1, 5, 20);
    hostel.addEdge(5, 6, 10);
    hostel.addEdge(2, 7, 5);
    hostel.addEdge(3, 7, 5);
    hostel.addEdge(4, 8, 12);
    hostel.addEdge(6, 8, 15);

    hostel.displayGraph();

    return 0;
}
