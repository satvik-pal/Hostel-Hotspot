#include <iostream>
#include <optional>
#include <string>
#include "MaintenanceManager.h"
#include "HashMap.h"
#include "MinHeap.h"

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

    cout << endl << "Result: " << passed << " passed, " << failed << " failed" << endl;

    return failed == 0 ? 0 : 1;

    
}

