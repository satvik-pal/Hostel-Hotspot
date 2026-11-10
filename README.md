# HostelPulse

*(formerly Hostel Hotspot)*

## About the Project

HostelPulse is a C++ hostel management system being developed as a team project (PBL).

It aims to manage the main parts of hostel operations: students, hostel infrastructure, complaints, and maintenance work. A student's complaint should eventually be assigned to a suitable maintenance worker, who is routed to the complaint location along the shortest path through the hostel.

## This Branch

`Satvik-maintenance` contains the **Maintenance module**: managing maintenance workers and allocating complaints to them. A custom min-heap and dynamic array have been built as the first steps towards routing workers with Dijkstra's algorithm.

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
| `MaintenanceWorker` | `assignComplaint()` / `completeComplaint()` | `true` if the action succeeded |

### Data Structures and Complexity

| Structure | Purpose | Complexity |
|---|---|---|
| `HashMap<MaintenanceWorker>` | Worker ID → worker | Average O(1) insert/find/erase, worst case O(n) |
| `HashMap<std::string>` | Complaint ID → worker ID | Average O(1) insert/find/erase, worst case O(n) |
| Linear scan in `allocateComplaint()` | Select the best available worker | O(n) over all workers |
| `MinHeap<T>` | Always returns the smallest item (for Dijkstra) | push/pop O(log n), peek O(1) |
| `DynamicArray<T>` | Resizable array for Dijkstra's distances, parents and routes | push amortised O(1), access O(1), copy/reverse O(n) |

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

### Known limitations

- Allocation does not yet consider distance to the complaint (planned with Dijkstra)
- A failed allocation does not report *why* it failed (no worker vs. already assigned)
- Worker IDs are compared as text for tie-breaking, which is correct up to `W999`
- Skill names are plain strings and must match exactly (e.g. `"Plumbing"`)

### Planned (not yet implemented)

- Dijkstra's algorithm on the hostel infrastructure graph for worker routing (min-heap and dynamic array done; the algorithm itself pending)
- Distance-aware worker selection
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
- `HashMap.h`, `MinHeap.h` and `DynamicArray.h` are header-only, so they do not need to be listed
- `-Iinclude` tells the compiler where to find the header files
- Every `.cpp` file must be listed so the linker can find all function definitions

## Tests

`main.cpp` currently runs a self-checking test suite for the whole module, from creating a worker to allocating and completing complaints. Each check prints `[PASS]` or `[FAIL]`, followed by a summary:

```text
Result: 101 passed, 0 failed
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

The tests will move to `tests/` once the CMake build is set up.

## Project Structure

```text
Hostel-Hotspot/
│
├── include/
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
