# HostelPulse — Complaint Management Module

**Module owner:** Saksham
**Branch:** `Saksham-Complaint`
**Language/build:** C++17, CMake 3.16+

This module registers student complaints, assigns each one a priority, schedules them in a priority queue, and tracks their status (Pending → Assigned → Resolved). It is built as a CMake library so the console menu, the automated tests and the future UI use the same logic.

## Project structure

```text
HostelPulse_Complaint_Module/
├── include/
│   ├── Complaint.h          Complaint data, getters, status transitions
│   ├── ComplaintManager.h   storage (HashMap), scheduling (MinHeap), status
│   ├── PriorityRules.h      categories and keyword priority rules
│   ├── HashMap.h            team shared hash table (unchanged copy)
│   └── MinHeap.h            team shared binary min-heap (unchanged copy)
├── src/
│   ├── Complaint.cpp
│   ├── ComplaintManager.cpp
│   └── PriorityRules.cpp
├── tests/
│   └── test_complaints.cpp
├── CMakeLists.txt
├── main.cpp                 console menu (all input/output)
├── README.md
└── .gitignore
```

## Features

- Register a complaint from parameters: `addComplaint(studentId, category, description, location)` returns the new ID (`C001`, `C002`, ...) or nothing if the input is invalid.
- Fixed categories, which are also the maintenance worker skill names: **Electrical, Plumbing, Carpentry, Cleaning, AC Repair, Internet** (exact spelling).
- Keyword-based priority (High / Medium / Low). High: fire, sparking, exposed wire, gas leak, flood, short circuit. Medium: electrical, plumbing, water, internet.
- Priority queue: `nextPending()` returns the most urgent pending complaint; equal priorities are handled first come, first served.
- Status tracking with enforced transitions: Pending → Assigned (with worker ID) → Resolved.
- Library classes never read input or print; the console menu in `main.cpp` does all I/O.

## Data structures

| Structure | Use | Complexity |
|---|---|---|
| `HashMap<Complaint>` | complaint ID → complaint | average O(1) insert/find |
| `MinHeap<QueuedComplaint>` | scheduling order | push/pop O(log n), peek O(1) |

`QueuedComplaint::operator<` defines "smaller" as "handle first": higher priority first, then lower arrival number.

**Lazy deletion.** A binary heap cannot remove an item from the middle cheaply. When a complaint is assigned, its queue entry stays in the heap; `nextPending()` discards entries whose complaint is no longer Pending when they reach the top. `nextPending()` only *peeks*, so if allocation fails (for example no plumber on duty) the complaint stays at the front.

**Known limitation:** if the front complaint cannot be allocated, the complaints behind it wait until it is.

## Integration with the Maintenance module

```cpp
std::optional<std::string> next = complaints.nextPending();
const Complaint* c = complaints.findComplaint(*next);
auto result = maintenance.allocateWithRoute(c->getId(), c->getCategory(), c->getLocation(), campusGraph);
if (result) complaints.markAssigned(c->getId(), result->workerId);
// when the work is done:
maintenance.completeComplaint(complaintId);
complaints.markResolved(complaintId);
```

The category is passed directly as the required worker skill, and the location must be a campus location name from the Infrastructure module (e.g. `Sai Hostel 204`). Location validation happens in the application layer, which has access to the campus graph.

## Build and run

```bash
cmake -S . -B build
cmake --build build
./build/complaint_menu
```

## Tests

```bash
ctest --test-dir build --output-on-failure
./build/complaint_tests          # shows every check
```

34 checks: priority rules and categories, registration and validation, ID generation, field storage, scheduling order (priority, then arrival), peek-without-removal, status transitions and invalid transitions, read-only iteration, and 1000 complaints.

## Shared team files

`include/HashMap.h` and `include/MinHeap.h` are copied unchanged from the `Satvik-maintenance` branch. Do not edit them here; update them once in the shared version so every module stays compatible.

## Credits

Original design (Complaint class, automatic IDs, keyword priority rules, console menu): Saksham.
Restructuring into a library with status tracking and the priority queue, following the team change specification: carried out by Satvik with the team's agreement while Saksham was unavailable.
