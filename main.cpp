#include <iostream>
#include <cstring>
using namespace std;

#define HOSTELS 2
#define FLOORS 5
#define ROOMS 5
#define SIZE 60
#define MAX_EDGES 10
#define NAME_SIZE 30

struct Edge {
    int destination;
    int weight;
};

struct Room {
    int roomNumber;
};

struct Floor {
    int floorNumber;
    Room rooms[ROOMS];
};

struct Hostel {
    char hostelName[NAME_SIZE];
    Floor floors[FLOORS];
};

class HostelGraph {
private:
    Hostel hostels[HOSTELS];

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

        initializeHostels();
    }

    void initializeHostels() {

        strcpy(hostels[0].hostelName, "Vikram hostel");
        strcpy(hostels[1].hostelName, "Shivalik hostel");

        for (int h = 0; h < HOSTELS; h++) {
            for (int f = 0; f < FLOORS; f++) {

                hostels[h].floors[f].floorNumber = f + 1;

                for (int r = 0; r < ROOMS; r++) {
                    hostels[h].floors[f].rooms[r].roomNumber =
                        (f + 1) * 100 + (r + 1);
                }
            }
        }
    }

    void displayHostels() {

        cout << "\n========== HOSTEL STRUCTURE ==========\n";

        for (int h = 0; h < HOSTELS; h++) {

            cout << "\nHostel Name: "
                 << hostels[h].hostelName << endl;

            for (int f = 0; f < FLOORS; f++) {

                cout << "\nFloor "
                     << hostels[h].floors[f].floorNumber
                     << ": ";

                for (int r = 0; r < ROOMS; r++) {

                    cout << "Room "
                         << hostels[h].floors[f].rooms[r].roomNumber
                         << "  ";
                }

                cout << endl;
            }
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

        cout << "\n========== HOSTEL GRAPH ==========\n";

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

  
    hostel.displayHostels();

   
    hostel.addVertex("Main Gate");
    hostel.addVertex("Reception");
    hostel.addVertex("Mess");
    hostel.addVertex("Common Room");
    hostel.addVertex("Washroom");
    hostel.addVertex("Stairs");
    hostel.addVertex("Vikram hostel");
    hostel.addVertex("Shivalik hostel");

    
    hostel.addEdge(0, 1, 10);
    hostel.addEdge(1, 2, 20);
    hostel.addEdge(2, 3, 10);
    hostel.addEdge(3, 5, 15);
    hostel.addEdge(4, 5, 5);
    hostel.addEdge(1, 6, 15);
    hostel.addEdge(1, 7, 20);

  
    hostel.displayGraph();

    return 0;
}


