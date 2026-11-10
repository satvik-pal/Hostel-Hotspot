#include <iostream>
#include <optional>
#include <string>
#include "MaintenanceManager.h"
#include "HashMap.h"
#include "MinHeap.h"
#include "DynamicArray.h"
#include "Dijkstra.h"

using namespace std;

// ---------- Tiny test helper ----------

int passed = 0;
int failed = 0;

void check(bool condition, const string& description)
{
    if (condition)
    {
        cout << "  [PASS] " << description << endl;
        passed++;
    }
    else
    {
        cout << "  [FAIL] " << description << endl;
        failed++;
    }
}

void section(const string& title)
{
    cout << endl << "== " << title << " ==" << endl;
}

// ---------- Tests ----------

void testWorkerCreation()
{
    section("1. Worker creation");

    MaintenanceWorker w("W001", "Amit Kumar", "Electrical", "Block A", true, 0);

    check(w.getWorkerId() == "W001",       "ID is stored");
    check(w.getName() == "Amit Kumar",     "name is stored");
    check(w.getSkill() == "Electrical",    "skill is stored");
    check(w.getLocation() == "Block A",    "location is stored");
    check(w.getWorkload() == 0,            "workload starts at 0");
    check(w.isOnDuty(),                    "starts on duty");
    check(w.isAvailable(),                 "starts available");
}

void testWorkload()
{
    section("2. Workload limits");

    MaintenanceWorker w("W001", "Amit", "Electrical", "Block A", true, 0);

    bool allAssigned = true;
    for (int i = 0; i < MaintenanceWorker::MAX_WORKLOAD; i++)
    {
        if (!w.assignComplaint())
        {
            allAssigned = false;
        }
    }
    check(allAssigned,                                   "can take MAX_WORKLOAD complaints");
    check(w.getWorkload() == MaintenanceWorker::MAX_WORKLOAD, "workload reached the limit");
    check(!w.hasCapacity(),                              "no capacity at the limit");
    check(!w.isAvailable(),                              "not available at the limit");
    check(w.isOnDuty(),                                  "still on duty at the limit");
    check(!w.assignComplaint(),                          "one more assignment is refused");
    check(w.getWorkload() == MaintenanceWorker::MAX_WORKLOAD, "workload did not exceed the limit");

    check(w.completeComplaint(),                         "completing one complaint succeeds");
    check(w.isAvailable(),                               "available again after completing");

    while (w.getWorkload() > 0)
    {
        w.completeComplaint();
    }
    check(!w.completeComplaint(),                        "completing at workload 0 is refused");
    check(w.getWorkload() == 0,                          "workload never goes negative");
}

void testOnDuty()
{
    section("3. On-duty status");

    MaintenanceWorker w("W001", "Amit", "Electrical", "Block A", true, 0);
    w.assignComplaint();

    w.setOnDuty(false);
    check(!w.isOnDuty(),        "can be set off duty");
    check(!w.isAvailable(),     "off-duty worker is not available");
    check(!w.assignComplaint(), "off-duty worker cannot take complaints");

    w.completeComplaint();
    check(!w.isOnDuty(),        "completing a complaint does NOT put worker back on duty");

    w.setOnDuty(true);
    check(w.isAvailable(),      "available again when back on duty");
}

void testLocation()
{
    section("4. Location update");

    MaintenanceWorker w("W001", "Amit", "Electrical", "Block A", true, 0);
    w.updateLocation("Block D");
    check(w.getLocation() == "Block D", "location is updated");
}

void testManagerBasics()
{
    section("5. Manager: add / find / remove");

    MaintenanceManager m;

    string id1 = m.addWorker("Amit", "Electrical", "Block A");
    string id2 = m.addWorker("Satish", "Plumbing", "Block C");

    check(id1 == "W001",              "first ID is W001");
    check(id2 == "W002",              "second ID is W002");
    check(id1 != id2,                 "IDs are unique");
    check(m.getWorkerCount() == 2,    "count is 2 after adding two");

    MaintenanceWorker* found = m.findWorker(id1);
    check(found != nullptr,           "existing worker is found");
    check(found != nullptr && found->getName() == "Amit", "found the correct worker");
    check(m.findWorker("W999") == nullptr, "nonexistent worker returns nullptr");

    check(m.removeWorker(id1),        "existing worker is removed");
    check(!m.removeWorker(id1),       "removing again fails");
    check(m.findWorker(id1) == nullptr, "removed worker can't be found");
    check(m.getWorkerCount() == 1,    "count is 1 after removal");

    string id3 = m.addWorker("Ravi", "Electrical", "Block B");
    check(id3 == "W003",              "IDs are not reused after removal");
}

void testAllocation()
{
    section("6. Allocation");

    MaintenanceManager m;
    string amit   = m.addWorker("Amit",   "Electrical", "Block A");
    string ravi   = m.addWorker("Ravi",   "Electrical", "Block B");
    string satish = m.addWorker("Satish", "Plumbing",   "Block C");

    optional<string> r1 = m.allocateComplaint("C001", "Electrical");
    check(r1.has_value() && *r1 == amit,   "tie on workload -> lower ID wins");

    optional<string> r2 = m.allocateComplaint("C002", "Electrical");
    check(r2.has_value() && *r2 == ravi,   "lower workload wins");

    optional<string> r3 = m.allocateComplaint("C003", "Plumbing");
    check(r3.has_value() && *r3 == satish, "matching skill is chosen");

    check(!m.allocateComplaint("C004", "Carpentry"), "no worker with skill -> no allocation");

    m.findWorker(satish)->setOnDuty(false);
    check(!m.allocateComplaint("C005", "Plumbing"),  "off-duty worker is skipped");

    check(!m.allocateComplaint("C001", "Electrical"), "same complaint can't be allocated twice");

    optional<string> who = m.getAssignedWorker("C001");
    check(who.has_value() && *who == amit,  "assignment is recorded");
    check(!m.getAssignedWorker("C999"),     "unknown complaint has no worker");

    check(m.findWorker(amit)->getWorkload() == 1, "assigned worker's workload increased");
}

void testFullWorkers()
{
    section("7. All workers full");

    MaintenanceManager m;
    string amit = m.addWorker("Amit", "Electrical", "Block A");

    for (int i = 0; i < MaintenanceWorker::MAX_WORKLOAD; i++)
    {
        m.allocateComplaint("C" + to_string(i), "Electrical");
    }
    check(m.findWorker(amit)->getWorkload() == MaintenanceWorker::MAX_WORKLOAD, "worker filled to the limit");
    check(!m.allocateComplaint("CX", "Electrical"), "full worker gets no more complaints");

    m.completeComplaint("C0");
    optional<string> r = m.allocateComplaint("CX", "Electrical");
    check(r.has_value() && *r == amit, "freed slot can be used again");
}

void testCompletion()
{
    section("8. Completion and removal rules");

    MaintenanceManager m;
    string amit = m.addWorker("Amit", "Electrical", "Block A");
    m.allocateComplaint("C001", "Electrical");

    check(!m.removeWorker(amit),                  "worker with open complaints can't be removed");
    check(m.completeComplaint("C001"),            "assigned complaint is completed");
    check(m.findWorker(amit)->getWorkload() == 0, "workload decreased after completion");
    check(!m.getAssignedWorker("C001"),           "assignment record is cleared");
    check(!m.completeComplaint("C001"),           "completing twice fails");
    check(!m.completeComplaint("C999"),           "completing unknown complaint fails");
    check(m.removeWorker(amit),                   "free worker can be removed");
}

void testHashMap()
{
    section("9. HashMap (own implementation)");

    HashMap<int> map;
    check(map.size() == 0,                 "starts empty");
    check(map.insert("A", 1),              "insert new key");
    check(!map.insert("A", 99),            "duplicate key is refused");
    check(map.find("A") != nullptr && *map.find("A") == 1, "find returns stored value");
    check(map.find("Z") == nullptr,        "missing key returns nullptr");

    *map.find("A") = 5;
    check(*map.find("A") == 5,             "value can be modified through the pointer");

    // Many keys: forces collisions and several rehashes (16 -> 32 -> 64 -> ...)
    const int N = 1000;
    bool allInserted = true;
    for (int i = 0; i < N; i++)
    {
        if (!map.insert("K" + to_string(i), i)) allInserted = false;
    }
    check(allInserted,                     "1000 keys inserted (with rehashing)");
    check(map.size() == N + 1,             "size is correct after rehashing");

    bool allFound = true;
    for (int i = 0; i < N; i++)
    {
        int* v = map.find("K" + to_string(i));
        if (v == nullptr || *v != i) allFound = false;
    }
    check(allFound,                        "every key still found after rehashing");

    bool allErased = true;
    for (int i = 0; i < N; i += 2)
    {
        if (!map.erase("K" + to_string(i))) allErased = false;
    }
    check(allErased,                       "erase every even key");
    check(map.size() == N / 2 + 1,         "size correct after erasing");
    check(map.find("K0") == nullptr,       "erased key is gone");
    check(map.find("K1") != nullptr,       "neighbouring key survives erase");
    check(!map.erase("K0"),                "erasing a missing key returns false");

    int visited = 0;
    map.forEach([&](const string& key, int& value) { (void)key; (void)value; visited++; });
    check(visited == map.size(),           "forEach visits every entry exactly once");
}
struct Task
{
    int priority;
    int order;
    bool operator<(const Task& other) const { return priority < other.priority; }
};

void testMinHeap()
{
    section("10. MinHeap (own implementation)");

    MinHeap<int> heap;
    int value = -1;

    check(heap.isEmpty(),                    "starts empty");
    check(!heap.pop(value),                  "pop on empty heap returns false");
    check(!heap.peek(value),                 "peek on empty heap returns false");

    heap.push(5);
    heap.push(2);
    heap.push(8);
    heap.push(1);
    heap.push(9);
    check(heap.size() == 5,                  "size is 5 after five pushes");
    check(heap.peek(value) && value == 1,    "peek returns the smallest");
    check(heap.size() == 5,                  "peek does not remove");

    bool sorted = true;
    int expected[] = {1, 2, 5, 8, 9};
    for (int i = 0; i < 5; i++)
    {
        if (!heap.pop(value) || value != expected[i]) sorted = false;
    }
    check(sorted,                            "pops come out in ascending order");
    check(heap.isEmpty(),                    "empty after popping everything");

    heap.push(3);
    heap.push(3);
    heap.push(3);
    bool duplicatesOk = true;
    for (int i = 0; i < 3; i++)
    {
        if (!heap.pop(value) || value != 3) duplicatesOk = false;
    }
    check(duplicatesOk,                      "duplicate values are all kept");

    heap.push(10);
    heap.push(4);
    heap.pop(value);                         // removes 4
    heap.push(7);
    heap.push(1);
    bool interleavedOk = heap.pop(value) && value == 1 &&
                         heap.pop(value) && value == 7 &&
                         heap.pop(value) && value == 10;
    check(interleavedOk,                     "mixed pushes and pops stay ordered");

    // 1000 values in scrambled order: forces several array growths (16 -> 32 -> ... -> 1024)
    const int N = 1000;
    for (int i = 0; i < N; i++)
    {
        heap.push((i * 37) % N);             // 37 and 1000 share no factor, so every value 0..999 appears once
    }
    check(heap.size() == N,                  "1000 items pushed (with growth)");

    bool allAscending = true;
    for (int i = 0; i < N; i++)
    {
        if (!heap.pop(value) || value != i) allAscending = false;
    }
    check(allAscending,                      "1000 items popped in exact ascending order");
    check(heap.isEmpty(),                    "empty again after 1000 pops");

    MinHeap<Task> tasks;
    tasks.push({3, 1});
    tasks.push({1, 2});
    tasks.push({2, 3});
    Task t{0, 0};
    check(tasks.pop(t) && t.priority == 1 && t.order == 2, "works with a custom struct using operator<");
}
DynamicArray<int> buildCountdown(int n)
{
    DynamicArray<int> result;
    for (int i = n; i >= 1; i--)
    {
        result.push(i);
    }
    return result;                           // returned by value
}

void testDynamicArray()
{
    section("11. DynamicArray (own implementation)");

    DynamicArray<int> empty;
    check(empty.isEmpty() && empty.size() == 0, "default array is empty");

    DynamicArray<int> filled(5, -1);
    bool allMinusOne = true;
    for (int i = 0; i < filled.size(); i++)
    {
        if (filled[i] != -1) allMinusOne = false;
    }
    check(filled.size() == 5 && allMinusOne,  "size + fill constructor sets every element");

    filled[2] = 42;
    check(filled[2] == 42 && filled[1] == -1, "operator[] reads and writes one element");

    DynamicArray<int> zero(0, 7);
    check(zero.isEmpty(),                     "size 0 gives an empty array");
    zero.push(1);
    check(zero.size() == 1 && zero[0] == 1,   "push works after size-0 construction");

    DynamicArray<int> numbers;
    const int N = 1000;
    for (int i = 0; i < N; i++)
    {
        numbers.push(i);
    }
    bool allKept = numbers.size() == N;
    for (int i = 0; i < N; i++)
    {
        if (numbers[i] != i) allKept = false;
    }
    check(allKept,                            "1000 pushes kept in order (with growth)");

    DynamicArray<int> odd;
    odd.push(1); odd.push(2); odd.push(3);
    odd.reverse();
    check(odd[0] == 3 && odd[1] == 2 && odd[2] == 1, "reverse with odd length");

    DynamicArray<int> even;
    even.push(1); even.push(2); even.push(3); even.push(4);
    even.reverse();
    check(even[0] == 4 && even[1] == 3 && even[2] == 2 && even[3] == 1, "reverse with even length");

    DynamicArray<int> one;
    one.push(9);
    one.reverse();
    empty.reverse();
    check(one[0] == 9 && empty.isEmpty(),     "reverse with 0 or 1 elements is safe");

    DynamicArray<int> original;
    original.push(10); original.push(20);
    DynamicArray<int> copy(original);
    copy[0] = 99;
    copy.push(30);
    check(original[0] == 10 && original.size() == 2, "copy constructor makes an independent copy");
    check(copy[0] == 99 && copy.size() == 3,          "the copy itself has the changes");

    DynamicArray<int> assigned;
    assigned.push(5);
    assigned = original;
    assigned[1] = 77;
    check(assigned.size() == 2 && assigned[0] == 10 && assigned[1] == 77, "assignment copies the contents");
    check(original[1] == 20,                  "assignment makes an independent copy");

    DynamicArray<int>& sameArray = assigned;
    assigned = sameArray;                    // a = a, written via a reference to avoid a compiler warning
    check(assigned.size() == 2 && assigned[0] == 10, "self-assignment is safe");

    DynamicArray<int> countdown = buildCountdown(3);
    check(countdown.size() == 3 && countdown[0] == 3 && countdown[2] == 1, "array can be returned from a function");

    DynamicArray<string> names;
    names.push("Reception");
    names.push("Room 101");
    check(names.size() == 2 && names[1] == "Room 101", "works with strings");

    numbers.clear();
    check(numbers.isEmpty(),                  "clear removes all elements");
}
// ---------- Test-only graph (stand-in until the Infrastructure module exposes its edges) ----------

struct TestEdge
{
    int destination;
    int weight;
};

class TestGraph
{
    private:
        DynamicArray<DynamicArray<TestEdge>> adjacency;
        DynamicArray<string> names;

    public:
        TestGraph(int vertexCount)
            : adjacency(vertexCount, DynamicArray<TestEdge>()),
              names(vertexCount, "")
        {
        }

        void setName(int vertex, const string& name)
        {
            names[vertex] = name;
        }

        void addEdge(int u, int v, int weight)          // two-way, like the hostel graph
        {
            adjacency[u].push({v, weight});
            adjacency[v].push({u, weight});
        }

        int getVertexCount() const                { return adjacency.size(); }
        int getEdgeCount(int vertex) const        { return adjacency[vertex].size(); }
        TestEdge getEdge(int vertex, int i) const { return adjacency[vertex][i]; }

        int findVertex(const string& name) const        // -1 if the location is unknown
        {
            for (int i = 0; i < names.size(); i++)
            {
                if (names[i] == name)
                {
                    return i;
                }
            }
            return -1;
        }
};

// Same 9 locations and distances as the Infrastructure module's main.cpp
TestGraph buildHostelLayout()
{
    TestGraph g(9);
    g.setName(0, "Main Gate");   g.setName(1, "Reception");   g.setName(2, "Room 101");
    g.setName(3, "Room 102");    g.setName(4, "Room 103");    g.setName(5, "Mess");
    g.setName(6, "Common Room"); g.setName(7, "Washroom");    g.setName(8, "Stairs");

    g.addEdge(0, 1, 10); g.addEdge(1, 2, 15); g.addEdge(2, 3, 8);
    g.addEdge(3, 4, 8);  g.addEdge(1, 5, 20); g.addEdge(5, 6, 10);
    g.addEdge(2, 7, 5);  g.addEdge(3, 7, 5);  g.addEdge(4, 8, 12);
    g.addEdge(6, 8, 15);
    return g;
}

bool pathIs(const DynamicArray<int>& path, const int expected[], int length)
{
    if (path.size() != length) return false;
    for (int i = 0; i < length; i++)
    {
        if (path[i] != expected[i]) return false;
    }
    return true;
}

void testDijkstra()
{
    section("12. Dijkstra shortest route");

    // Direct route
    TestGraph direct(2);
    direct.addEdge(0, 1, 7);
    RouteResult r1 = findShortestRoute(direct, 0, 1);
    int p1[] = {0, 1};
    check(r1.reachable && r1.distance == 7,  "direct route: distance 7");
    check(pathIs(r1.path, p1, 2),            "direct route: path 0 -> 1");

    // Shortest in metres is NOT the route with fewest edges
    TestGraph detour(3);
    detour.addEdge(0, 1, 10);                // direct but long
    detour.addEdge(0, 2, 2);
    detour.addEdge(2, 1, 3);                 // two edges, total 5
    RouteResult r2 = findShortestRoute(detour, 0, 1);
    int p2[] = {0, 2, 1};
    check(r2.reachable && r2.distance == 5,  "longer path in edges but shorter in metres is chosen");
    check(pathIs(r2.path, p2, 3),            "path goes through the detour 0 -> 2 -> 1");

    // Unreachable
    TestGraph split(3);
    split.addEdge(0, 1, 4);                  // vertex 2 has no connections
    RouteResult r3 = findShortestRoute(split, 0, 2);
    check(!r3.reachable && r3.distance == -1 && r3.path.isEmpty(), "unreachable location is reported");

    // Same source and destination
    RouteResult r4 = findShortestRoute(direct, 1, 1);
    int p4[] = {1};
    check(r4.reachable && r4.distance == 0 && pathIs(r4.path, p4, 1), "same start and end: distance 0");

    // Invalid vertex numbers
    check(!findShortestRoute(direct, 0, 5).reachable,  "destination out of range -> not reachable");
    check(!findShortestRoute(direct, -1, 0).reachable, "negative source -> not reachable");

    // Real hostel layout
    TestGraph hostel = buildHostelLayout();

    RouteResult r5 = findShortestRoute(hostel, 1, 4);           // Reception -> Room 103
    int p5[] = {1, 2, 3, 4};
    check(r5.reachable && r5.distance == 31, "Reception -> Room 103 is 31 m");
    check(pathIs(r5.path, p5, 4),            "via Room 101 and Room 102");

    RouteResult r6 = findShortestRoute(hostel, 0, 8);           // Main Gate -> Stairs
    int p6[] = {0, 1, 2, 3, 4, 8};
    check(r6.reachable && r6.distance == 53, "Main Gate -> Stairs is 53 m (not 55 m via Mess)");
    check(pathIs(r6.path, p6, 6),            "via the rooms, not the Mess");

    RouteResult r7 = findShortestRoute(hostel, 7, 5);           // Washroom -> Mess
    check(r7.reachable && r7.distance == 40, "Washroom -> Mess is 40 m");

    RouteResult r8 = findShortestRoute(hostel, 4, 1);           // reverse direction
    check(r8.reachable && r8.distance == 31, "route is the same length in the reverse direction");

    // Every route's edges must add up to its reported distance
    bool allConsistent = true;
    for (int s = 0; s < 9; s++)
    {
        for (int d = 0; d < 9; d++)
        {
            RouteResult r = findShortestRoute(hostel, s, d);
            if (!r.reachable || r.path[0] != s || r.path[r.path.size() - 1] != d)
            {
                allConsistent = false;
                continue;
            }
            int total = 0;
            for (int k = 0; k + 1 < r.path.size(); k++)
            {
                int from = r.path[k];
                int to = r.path[k + 1];
                bool edgeFound = false;
                for (int e = 0; e < hostel.getEdgeCount(from); e++)
                {
                    if (hostel.getEdge(from, e).destination == to)
                    {
                        total += hostel.getEdge(from, e).weight;
                        edgeFound = true;
                        break;
                    }
                }
                if (!edgeFound) allConsistent = false;
            }
            if (total != r.distance) allConsistent = false;
        }
    }
    check(allConsistent, "all 81 routes: path is valid and its edges add up to the distance");

    // Cross-check every distance against Floyd-Warshall (a different, brute-force algorithm)
    const int V = 9;
    int best[V][V];
    for (int i = 0; i < V; i++)
    {
        for (int j = 0; j < V; j++)
        {
            best[i][j] = (i == j) ? 0 : UNREACHABLE_DISTANCE;
        }
        for (int e = 0; e < hostel.getEdgeCount(i); e++)
        {
            TestEdge edge = hostel.getEdge(i, e);
            if (edge.weight < best[i][edge.destination])
            {
                best[i][edge.destination] = edge.weight;
            }
        }
    }
    for (int k = 0; k < V; k++)
    {
        for (int i = 0; i < V; i++)
        {
            for (int j = 0; j < V; j++)
            {
                if (best[i][k] + best[k][j] < best[i][j])
                {
                    best[i][j] = best[i][k] + best[k][j];
                }
            }
        }
    }
    bool allShortest = true;
    for (int s = 0; s < V; s++)
    {
        for (int d = 0; d < V; d++)
        {
            if (findShortestRoute(hostel, s, d).distance != best[s][d]) allShortest = false;
        }
    }
    check(allShortest, "all 81 distances match Floyd-Warshall (they really are the shortest)");
}

void testAllocateWithRoute()
{
    section("13. Distance-aware allocation with route");

    TestGraph hostel = buildHostelLayout();

    // computeDistancesFrom: one run gives every distance
    DynamicArray<int> fromReception = computeDistancesFrom(hostel, hostel.findVertex("Reception"));
    check(fromReception[hostel.findVertex("Room 103")] == 31 &&
          fromReception[hostel.findVertex("Main Gate")] == 10 &&
          fromReception[hostel.findVertex("Stairs")] == 43,       "computeDistancesFrom gives every distance in one run");

    bool matchesSingleRoutes = true;
    for (int v = 0; v < hostel.getVertexCount(); v++)
    {
        if (fromReception[v] != findShortestRoute(hostel, 1, v).distance) matchesSingleRoutes = false;
    }
    check(matchesSingleRoutes,                                    "all-distances result matches single-route results");

    check(hostel.findVertex("Mess") == 5 && hostel.findVertex("Library") == -1, "findVertex: known name and unknown name");

    MaintenanceManager m;
    string farElectrician  = m.addWorker("Amit",   "Electrical", "Main Gate");   // W001, 41 m from Room 103
    string nearElectrician = m.addWorker("Ravi",   "Electrical", "Room 102");    // W002,  8 m from Room 103
    string plumber         = m.addWorker("Satish", "Plumbing",   "Mess");        // W003

    // 1. Equal workload -> nearer worker wins, even though it has the higher ID
    optional<AllocationResult> a1 = m.allocateWithRoute("C001", "Electrical", "Room 103", hostel);
    int route1[] = {3, 4};
    check(a1.has_value() && a1->workerId == nearElectrician,    "equal workload: nearer worker wins over lower ID");
    check(a1.has_value() && a1->route.reachable && a1->route.distance == 8 &&
          pathIs(a1->route.path, route1, 2),                    "route Room 102 -> Room 103 is 8 m");

    // 2. Workload comes first -> farther worker with less work wins
    optional<AllocationResult> a2 = m.allocateWithRoute("C002", "Electrical", "Room 103", hostel);
    int route2[] = {0, 1, 2, 3, 4};
    check(a2.has_value() && a2->workerId == farElectrician,     "lower workload wins over distance");
    check(a2.has_value() && a2->route.distance == 41 &&
          pathIs(a2->route.path, route2, 5),                    "route Main Gate -> Room 103 is 41 m");

    // 3. Both now have workload 1 -> distance decides again
    optional<AllocationResult> a3 = m.allocateWithRoute("C003", "Electrical", "Room 103", hostel);
    check(a3.has_value() && a3->workerId == nearElectrician,    "workload tie again: nearer worker wins");

    // 4. Skill matching still applies
    optional<AllocationResult> a4 = m.allocateWithRoute("C004", "Plumbing", "Washroom", hostel);
    check(a4.has_value() && a4->workerId == plumber &&
          a4->route.distance == 40,                             "plumber chosen; Mess -> Washroom is 40 m");

    // 5. Same rules as before
    check(!m.allocateWithRoute("C001", "Electrical", "Room 103", hostel), "same complaint can't be allocated twice");
    check(!m.allocateWithRoute("C005", "Carpentry", "Room 101", hostel),  "no worker with the skill -> no allocation");

    m.findWorker(plumber)->setOnDuty(false);
    check(!m.allocateWithRoute("C006", "Plumbing", "Mess", hostel),      "off-duty worker is skipped");

    optional<string> who = m.getAssignedWorker("C001");
    check(who.has_value() && *who == nearElectrician,           "assignment is recorded");
    check(m.completeComplaint("C001") &&
          m.findWorker(nearElectrician)->getWorkload() == 1,    "completing works for route-based allocations");

    // 6. Unknown complaint location: worker is still assigned, but no route
    MaintenanceManager m2;
    string w1 = m2.addWorker("Amit", "Electrical", "Reception");
    optional<AllocationResult> a5 = m2.allocateWithRoute("C010", "Electrical", "Library", hostel);
    check(a5.has_value() && a5->workerId == w1 && !a5->route.reachable, "unknown complaint location: assigned, no route");

    // 7. Worker at an unknown location ranks after workers with a known distance
    MaintenanceManager m3;
    string unknownSpot = m3.addWorker("Amit", "Electrical", "Rooftop");     // W001, location not in graph
    string reception   = m3.addWorker("Ravi", "Electrical", "Reception");   // W002
    optional<AllocationResult> a6 = m3.allocateWithRoute("C020", "Electrical", "Room 101", hostel);
    check(a6.has_value() && a6->workerId == reception &&
          a6->route.distance == 15,                             "known location beats unknown location");
    (void)unknownSpot;
}

// ---------- Run everything ----------

int main()
{
    cout << "HostelPulse - Maintenance module tests" << endl;

    testWorkerCreation();
    testWorkload();
    testOnDuty();
    testLocation();
    testManagerBasics();
    testAllocation();
    testFullWorkers();
    testCompletion();
    testHashMap();
    testMinHeap();
    testDynamicArray();
    testDijkstra();
    testAllocateWithRoute();

    cout << endl << "Result: " << passed << " passed, " << failed << " failed" << endl;

    return failed == 0 ? 0 : 1;

    
}

