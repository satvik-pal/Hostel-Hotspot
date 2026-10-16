#include <iostream>
#include <vector>
#include <string>
using namespace std;

struct Edge {
    int destination;
    int weight;
};

class HostelGraph {
private:
    vector<string> vertices;
    vector<vector<Edge>> adjList;

public:
    void addVertex(const string& name) {
        vertices.push_back(name);
        adjList.push_back({});
    }

    void addEdge(int u, int v, int distance) {
        if (u < 0 || v < 0 || u >= (int)vertices.size() || v >= (int)vertices.size()) {
            cout << "Invalid vertex index\n";
            return;
        }
        adjList[u].push_back({v, distance});
        adjList[v].push_back({u, distance});
    }

    void displayGraph() const {
        cout << "\n  HOSTEL INFRASTRUCTURE GRAPH   \n";

        for (size_t i = 0; i < vertices.size(); i++) {
            cout << vertices[i] << " -> ";

            for (const auto& edge : adjList[i]) {
                cout << vertices[edge.destination]
                     << " (" << edge.weight << "m)  ";
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
