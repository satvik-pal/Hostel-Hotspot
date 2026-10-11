# HostelPulse

*(formerly Hostel Hotspot)*

## About the Project

HostelPulse is a C++ hostel management system being developed as a team project (PBL).

It aims to manage the main parts of hostel operations: students, hostel infrastructure, complaints, and maintenance work. A student's complaint should eventually be assigned to a suitable maintenance worker, who is routed to the complaint location along the shortest path through the hostel.

## This Branch

`Satvik-maintenance` contains the **Maintenance module**: managing maintenance workers, and later allocating complaints to them and routing them with Dijkstra's algorithm.

## Technologies and Concepts

- C++17
- Object-Oriented Programming (OOP)
- Data Structures and Algorithms (DSA)
- Standard Template Library (STL)
- Git and GitHub

Planned for later phases: CMake, SQLite (persistence), Qt (desktop UI).

## Current Progress

### Phase 1 (complete)

The initial project structure and the foundation of the Maintenance module.

### Phase 2 (in progress)

`MaintenanceWorker` has been refactored:

- On-duty status (`onDuty`) is stored separately from workload
- Availability is computed from both, not stored: a worker is available only if on duty **and** below the workload limit
- Workload limit defined once as `MaintenanceWorker::MAX_WORKLOAD` (10)
- `assignComplaint()` and `completeComplaint()` return `true`/`false` so callers know whether the action succeeded
- Workload cannot go below 0 or above the limit
- `const` getters and `const std::string&` parameters
- No `using namespace std;` in the header

## Maintenance Module

### Implemented

- `MaintenanceWorker` class: ID, name, skill, location, on-duty status, workload
- `MaintenanceManager` class
- Worker registration with automatic ID generation (`W001`, `W002`, ...)
- Worker lookup by ID using `unordered_map` (average O(1))
- Worker removal
- Workload management with a fixed limit
- On-duty status management
- Worker location update

### Worker availability rules

| Function | Meaning |
|---|---|
| `isOnDuty()` | Worker is on shift (not on leave/off duty); set by an admin via `setOnDuty()` |
| `hasCapacity()` | `workload < MAX_WORKLOAD` |
| `isAvailable()` | `isOnDuty() && hasCapacity()`: the worker can take a new complaint |

### Planned (not yet implemented)

- Skill-based worker selection and complaint allocation
- Dijkstra's algorithm on the hostel infrastructure graph for worker routing
- Integration with the Complaint and Hostel Infrastructure modules
- Automated tests
- SQLite persistence
- Qt user interface

## Build and Run

From the project root:

```bash
g++ -std=c++17 -Iinclude main.cpp src/MaintenanceWorker.cpp src/MaintenanceManager.cpp -o hostel
./hostel
```

`-Iinclude` tells the compiler where to find the header files. Every `.cpp` file must be listed so that the linker can find all function definitions.

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
