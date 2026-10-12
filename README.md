# HostelPulse

*(formerly Hostel Hotspot)*

## About the Project

HostelPulse is a C++ hostel management system being developed as a team project (PBL).

It aims to manage the main parts of hostel operations: students, hostel infrastructure, complaints, and maintenance work. A student's complaint should eventually be assigned to a suitable maintenance worker, who is routed to the complaint location along the shortest path through the hostel.

## This Branch

`Satvik-maintenance` contains the **Maintenance module**: managing maintenance workers and allocating complaints to them. Routing workers with Dijkstra's algorithm is the next stage.

## Technologies and Concepts

- C++17
- Object-Oriented Programming (OOP)
- Data Structures and Algorithms (DSA)
- Standard Template Library (STL)
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
| `unordered_map<string, MaintenanceWorker>` | Worker ID → worker | Average O(1) lookup, worst case O(n) |
| `unordered_map<string, string>` | Complaint ID → worker ID | Average O(1) lookup, worst case O(n) |
| Linear scan in `allocateComplaint()` | Select the best available worker | O(n) over all workers |

A linear scan was chosen over a priority queue because a worker's workload changes after every assignment and completion, and `std::priority_queue` cannot update existing entries. For a hostel-sized team (tens of workers), O(n) is simple and fast enough.

### Known limitations

- Allocation does not yet consider distance to the complaint (planned with Dijkstra)
- A failed allocation does not report *why* it failed (no worker vs. already assigned)
- Worker IDs are compared as text for tie-breaking, which is correct up to `W999`
- Skill names are plain strings and must match exactly (e.g. `"Plumbing"`)

### Planned (not yet implemented)

- Dijkstra's algorithm on the hostel infrastructure graph for worker routing
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

- `-std=c++17` is required (`std::optional` and structured bindings are C++17 features)
- `-Iinclude` tells the compiler where to find the header files
- Every `.cpp` file must be listed so the linker can find all function definitions

## Tests

`main.cpp` currently runs a self-checking test suite for the whole module, from creating a worker to allocating and completing complaints. Each check prints `[PASS]` or `[FAIL]`, followed by a summary:

```text
Result: 55 passed, 0 failed
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

The tests will move to `tests/` once the CMake build is set up.

## Project Structure

```text
Hostel-Hotspot/
│
├── include/
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
