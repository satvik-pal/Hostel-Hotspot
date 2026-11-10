# HostelPulse

*(formerly Hostel Hotspot)*

## About the Project

HostelPulse is a C++ hostel management system being developed as a team project (PBL).

It aims to manage the main parts of hostel operations: students, hostel infrastructure, complaints, and maintenance work. A student's complaint should eventually be assigned to a suitable maintenance worker, who is routed to the complaint location along the shortest path through the hostel.

## This Branch

`Satvik-maintenance` contains the **Maintenance module**: managing maintenance workers and allocating complaints to them. Dijkstra's shortest-route algorithm is implemented on top of a custom min-heap and dynamic array, and is used for distance-aware allocation: the chosen worker also receives the shortest route to the complaint. It is currently tested on a stand-in copy of the hostel layout; connecting it to the Infrastructure module's real graph is the next stage.

## Technologies and Concepts

- C++17
- Object-Oriented Programming (OOP)
- Data Structures and Algorithms (DSA)
- Custom data structures (no STL containers or algorithms)
- Git and GitHub

Planned for later phases: CMake, SQLite (persistence), Qt (desktop UI).

## Current Progress

### Phase 1 (complete)

Initial project structure and the foundation of the Maintenance module.

### Phase 2 (in progress)

- `MaintenanceWorker` refactored: separate on-duty status and workload capacity
- `MaintenanceManager` refactored: returns results instead of printing
- Skill-based complaint allocation
- Assignment tracking (which worker handles which complaint)
- Own hash table (`HashMap`) replacing `std::unordered_map`
- Own binary min-heap (`MinHeap`) in preparation for Dijkstra's algorithm
- Own resizable array (`DynamicArray`) replacing `std::vector`, with deep copy
- Dijkstra's shortest-route algorithm (`findShortestRoute`), written against a graph interface
- Distance-aware allocation (`allocateWithRoute`): workload first, then distance to the complaint, then worker ID, returning the worker and their route
- Self-checking test suite covering the whole module

## Maintenance Module

### Implemented

**Worker management**
- `MaintenanceWorker` class: ID, name, skill, location, on-duty status, workload
- Worker registration with automatic, never-reused IDs (`W001`, `W002`, ...)
- Worker lookup by ID (returns `nullptr` if not found)
- Worker removal (refused while the worker still has open complaints)
- Workload management with a fixed limit (`MaintenanceWorker::MAX_WORKLOAD` = 10)
- On-duty status management
- Worker location update

**Complaint allocation**
- Allocate a complaint to a worker with the required skill
- Only available workers are considered (on duty **and** below the workload limit)
- The worker with the lowest workload is chosen; ties go to the lower worker ID so results are deterministic
- A complaint cannot be allocated twice
- Completing a complaint updates the correct worker's workload automatically
- Look up which worker is handling a complaint

**Distance-aware allocation with route**
- `allocateWithRoute(complaintId, skill, complaintLocation, graph)` chooses among available workers with the skill by: (1) lowest workload, (2) shortest walking distance to the complaint, (3) lower worker ID
- Returns the chosen worker **and** the shortest route from the worker's location to the complaint
- One Dijkstra run from the complaint gives the distance to every location (the graph is two-way, so complaint → worker equals worker → complaint), instead of one run per candidate worker
- If the complaint's location is not in the graph, the complaint is still assigned but no route is returned
- Workers at an unknown location rank after workers with a known distance

### Worker availability rules

| Function | Meaning |
|---|---|
| `isOnDuty()` | Worker is on shift (not on leave/off duty); set by an admin via `setOnDuty()` |
| `hasCapacity()` | `workload < MAX_WORKLOAD` |
| `isAvailable()` | `isOnDuty() && hasCapacity()`: the worker can take a new complaint |

On-duty status is stored; availability is computed, so it can never get out of sync with the workload.

### Main functions

| Class | Function | Returns |
|---|---|---|
| `MaintenanceManager` | `addWorker(name, skill, location)` | New worker ID |
| | `findWorker(workerId)` | Pointer to worker, or `nullptr` |
| | `removeWorker(workerId)` | `true` if removed |
| | `getWorkerCount()` | Number of workers |
| | `allocateComplaint(complaintId, requiredSkill)` | `std::optional` worker ID (empty if no suitable worker) |
| | `completeComplaint(complaintId)` | `true` if the complaint was assigned and is now completed |
| | `getAssignedWorker(complaintId)` | `std::optional` worker ID |
| | `allocateWithRoute(complaintId, skill, location, graph)` | `std::optional<AllocationResult>` (worker ID + route) |
| `MaintenanceWorker` | `assignComplaint()` / `completeComplaint()` | `true` if the action succeeded |

### Data Structures and Complexity

| Structure | Purpose | Complexity |
|---|---|---|
| `HashMap<MaintenanceWorker>` | Worker ID → worker | Average O(1) insert/find/erase, worst case O(n) |
| `HashMap<std::string>` | Complaint ID → worker ID | Average O(1) insert/find/erase, worst case O(n) |
| Linear scan in `allocateComplaint()` | Select the best available worker | O(n) over all workers |
| `MinHeap<T>` | Always returns the smallest item (for Dijkstra) | push/pop O(log n), peek O(1) |
| `DynamicArray<T>` | Resizable array for Dijkstra's distances, parents and routes | push amortised O(1), access O(1), copy/reverse O(n) |
| Dijkstra (`findShortestRoute`) | Shortest route between two locations | O((V + E) log V) time, O(V + E) space |
| `computeDistancesFrom` | Distance from one location to every location (one Dijkstra run) | O((V + E) log V) |
| `allocateWithRoute` | Distance-aware worker selection + route | O((V + E) log V + n) — two Dijkstra runs plus one scan of n workers |

A linear scan was chosen over a heap because a worker's workload changes after every assignment and completion, and a plain binary heap cannot cheaply update existing entries. For a hostel-sized team (tens of workers), O(n) is simple and fast enough.

### HashMap (own implementation)

The project does not use STL containers or algorithms for its data structures, so `HashMap` (`include/HashMap.h`) is implemented from scratch:

- **Structure:** array of buckets; each bucket is a singly linked list (separate chaining)
- **Hash function:** djb2 (`hash = hash * 33 + c`, starting at 5381), then `% capacity`
- **Insert:** new node at the head of its bucket's list; duplicate keys are refused
- **Rehashing:** when the load factor would exceed 0.75, capacity doubles (starting at 16) and every node is relinked into its new bucket
- **Erase:** standard linked-list unlink (head or middle), then `delete`
- **Memory:** the destructor frees every node and the bucket array; copying is disabled to prevent double deletion
- **Iteration:** `forEach(action)` calls a function on every entry
- **Template:** `HashMap<V>` works for any value type; it lives entirely in the header because templates are compiled where they are used

### MinHeap (own implementation)

`MinHeap` (`include/MinHeap.h`) is a binary min-heap implemented from scratch. It will be used by Dijkstra's algorithm to always process the closest unvisited location next.

- **Structure:** a complete binary tree stored in a dynamic array; for index `i`, the parent is `(i - 1) / 2` and the children are `2i + 1` and `2i + 2`
- **Heap property:** every parent is less than or equal to its children, so the smallest item is always at index 0
- **Push:** place the item at the end, then *sift up* while it is smaller than its parent
- **Pop:** take the root, move the last item to the root, then *sift down* by swapping with the smaller child
- **Growth:** the array doubles when full (starting at 16)
- **Safety:** `pop()` and `peek()` return `false` on an empty heap; the destructor frees the array and copying is disabled
- **Template:** `MinHeap<T>` works with any type that defines `operator<`, e.g. `(distance, location)` pairs for Dijkstra or complaints ordered by priority

### DynamicArray (own implementation)

`DynamicArray` (`include/DynamicArray.h`) is a resizable array implemented from scratch as a replacement for `std::vector`. Dijkstra's algorithm will use it for the distance array, the parent array and the returned route.

- **Structure:** a dynamically allocated block with a capacity (allocated size) and a count (elements in use)
- **Growth:** the block doubles when full (starting at 8), so `push` is amortised O(1)
- **Construction:** empty, or `DynamicArray(size, fillValue)` to create `size` elements all set to `fillValue` (e.g. distances set to infinity)
- **Access:** `operator[]` returns a reference so elements can be read and written like a normal array; the index is not checked
- **Reverse:** reverses the elements in place (used when rebuilding a route from destination back to source)
- **Deep copy (Rule of Three):** a copy constructor, copy assignment operator (with a self-assignment check) and destructor, so copies are fully independent and arrays can be returned from functions
- **Template:** `DynamicArray<T>` works for any type with a default constructor and copy assignment

### Dijkstra's Algorithm

`findShortestRoute(graph, source, destination)` (`include/Dijkstra.h`) finds the shortest route in metres between two hostel locations.

**Graph interface.** The functions are templates, so they work with any graph class that provides:

```cpp
int  getVertexCount() const;
int  getEdgeCount(int vertex) const;
Edge getEdge(int vertex, int i) const;          // Edge has .destination and .weight
int  findVertex(const std::string& name) const; // location name -> vertex number, -1 if unknown (used by allocateWithRoute)
```

This avoids a second graph system: the Infrastructure module's `HostelGraph` only needs these four functions to be used directly. Until then, Dijkstra is tested on a test-only graph that reproduces the same 9-location hostel layout.

**Algorithm.**
1. Set every distance to "unreachable" and the source's distance to 0
2. Repeatedly take the closest unfinished location from the min-heap
3. Skip outdated heap entries (lazy deletion), and stop early once the destination is taken
4. **Relaxation:** for each neighbour, if going through the current location is shorter, update its distance and parent and push it onto the heap
5. Rebuild the route by following `parent[]` back from the destination, then reversing it

**Result:** a `RouteResult` with `reachable`, `distance` (metres) and `path` (location numbers from source to destination).

**Shared core.** `runDijkstra()` contains the algorithm once; `findShortestRoute()` uses it with an early stop at the destination, and `computeDistancesFrom()` uses it without stopping to get the distance to every location.

**Requirements and edge cases:** edge weights must be non-negative; invalid location numbers and unreachable destinations return `reachable = false`; the same source and destination gives distance 0.

**Example (hostel layout):** Reception → Room 103 = 31 m via Room 101 and Room 102.

### Known limitations

- Dijkstra and distance-aware allocation currently run on a test-only copy of the hostel layout until the Infrastructure module's graph provides `getVertexCount()`, `getEdgeCount()`, `getEdge()` and `findVertex()`
- Location names must match the graph's names exactly (e.g. `"Room 103"`)
- The single-run distance optimisation assumes the graph is two-way (undirected)
- A worker's location is not updated automatically after reaching a complaint
- Dijkstra assumes all distances are non-negative
- A failed allocation does not report *why* it failed (no worker vs. already assigned)
- Worker IDs are compared as text for tie-breaking, which is correct up to `W999`
- Skill names are plain strings and must match exactly (e.g. `"Plumbing"`)

### Planned (not yet implemented)

- Connecting Dijkstra and `allocateWithRoute` to the Infrastructure module's `HostelGraph` (requires `getVertexCount()`, `getEdgeCount()`, `getEdge()` and `findVertex()`)
- Integration with the Complaint and Hostel Infrastructure modules
- SQLite persistence (with inactive status instead of deleting workers)
- Qt user interface

## Build and Run

From the project root:

```bash
g++ -std=c++17 -Iinclude main.cpp src/MaintenanceWorker.cpp src/MaintenanceManager.cpp -o hostel
./hostel
```

- `-std=c++17` is required (`std::optional` is a C++17 feature)
- `HashMap.h`, `MinHeap.h`, `DynamicArray.h` and `Dijkstra.h` are header-only, so they do not need to be listed
- `-Iinclude` tells the compiler where to find the header files
- Every `.cpp` file must be listed so the linker can find all function definitions

## Tests

`main.cpp` currently runs a self-checking test suite for the whole module, from creating a worker to allocating and completing complaints. Each check prints `[PASS]` or `[FAIL]`, followed by a summary:

```text
Result: 133 passed, 0 failed
```

The program exits with code `0` if all tests pass and `1` otherwise.

Covered:

1. Worker creation
2. Workload limits
3. On-duty status
4. Location update
5. Manager add / find / remove, unique and non-reused IDs
6. Allocation: tie-break, lowest workload, skill matching, missing skill, off-duty workers, duplicate complaints
7. All workers full, and reuse of a freed slot
8. Completion and removal rules
9. `HashMap`: insert, duplicates, find, modify through pointer, 1000 keys with rehashing, erase, `forEach`
10. `MinHeap`: empty heap, ascending pops, peek, duplicates, mixed push/pop, 1000 items with growth, custom struct
11. `DynamicArray`: fill constructor, element access, 1000 pushes with growth, reverse, independent copies, self-assignment, returning from a function, strings, clear
12. Dijkstra: direct route, shorter route with more edges, unreachable location, same start and end, invalid input, hostel layout routes, all 81 routes checked for validity and cross-checked against Floyd-Warshall
13. Distance-aware allocation: all-distances run, location lookup, nearer worker wins on equal workload, workload beats distance, routes returned, skill/duty/duplicate rules, unknown complaint location, unknown worker location

The tests will move to `tests/` once the CMake build is set up.

## Project Structure

```text
Hostel-Hotspot/
│
├── include/
│   ├── Dijkstra.h
│   ├── DynamicArray.h
│   ├── HashMap.h
│   ├── MinHeap.h
│   ├── MaintenanceWorker.h
│   └── MaintenanceManager.h
│
├── src/
│   ├── MaintenanceWorker.cpp
│   └── MaintenanceManager.cpp
│
├── tests/
├── data/
├── docs/
├── main.cpp
└── README.md
```
