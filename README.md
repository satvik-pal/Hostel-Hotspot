# HostelPulse – Hostel Infrastructure Module

## Campus configuration

This C++17 project models a campus containing five hostels:

1. Atlantis Hostel
2. Sai Hostel
3. Mother Teresa
4. Subhash Chandra Bose Hostel
5. Bhagat Singh Hostel

Each hostel has 5 floors and 10 rooms per floor: 250 rooms total.

## Layered campus graph

The graph contains 283 locations:
- 3 campus places: Main Gate, Mess, Maintenance Office
- 5 hostel entrances
- 25 stair landings
- 250 rooms

It contains 283 undirected paths (566 adjacency entries).

## Room naming

Room names use `<Hostel> <floor><two-digit-room>`, for example:
- `Sai Hostel 204`
- `Atlantis Hostel 310`
- `Bhagat Singh Hostel 501`

## Room properties

| Floors | Sharing | AC |
|---|---|---|
| 1–2 | Triple | No |
| 3–4 | Double | Rooms 01–05 have AC; 06–10 do not |
| 5 | Single | Yes |

## Graph distances

| Connection | Distance |
|---|---:|
| Main Gate – Maintenance Office | 30 m |
| Main Gate – Mess | 80 m |
| Main Gate – first hostel entrance | 60 m |
| Consecutive hostel entrances | 70 m |
| Mess – middle hostel entrance | 50 m |
| Entrance – Floor 1 stairs | 10 m |
| Stairs between consecutive floors | 6 m |
| Stairs – first room on floor | 5 m |
| Consecutive rooms on same floor | 4 m |

All paths are bidirectional and have positive distances.

## Integration contract

`HostelGraph` provides:
- `getVertexCount() const`
- `getEdgeCount(int vertex) const`
- `getEdge(int vertex, int i) const`
- `findVertex(const std::string& name) const`
- `getName(int vertex) const`

It also provides `addLocation`, `addPath`, and `addPathByName`.

## Build

Requirements: CMake 3.16+ and a C++17 compiler.

```bash
cmake -S . -B build
cmake --build build
```

Run the demo:

```bash
./build/campus_demo
```

On Windows, run `build\Debug\campus_demo.exe` or
`build\campus_demo.exe`, depending on the generator.

## Tests

```bash
ctest --test-dir build --output-on-failure
```

`tests/test_infrastructure.cpp` (34 checks) covers: location and path validation, two-way paths, name lookup up to 1000 locations, room registry, naming format, campus size (283 locations, 283 paths, 250 rooms, 50 rooms per hostel), room types and AC, reachability of every location from Main Gate, and shortest-path distances that the Maintenance module's Dijkstra relies on.

## Project structure

```text
HostelPulse_Infrastructure/
├── include/
│   ├── DynamicArray.h
│   ├── HashMap.h
│   ├── HostelGraph.h
│   ├── Room.h
│   └── Campus.h
├── src/
│   ├── HostelGraph.cpp
│   ├── Room.cpp
│   └── Campus.cpp
├── tests/
│   └── test_infrastructure.cpp
├── main.cpp
├── CMakeLists.txt
└── README.md
```

## Shared team data structures

`include/HashMap.h` and `include/DynamicArray.h` are the team's shared implementations, copied unchanged from the `Satvik-maintenance` branch (no STL containers are used). Do not edit them in this branch; changes are made once, in the shared version, so that all modules stay compatible when merged.

Notes for callers:
- `getEdge(vertex, i)` returns `Edge{-1, 0}` for an invalid vertex or index, so callers that pass unchecked values must test for `destination == -1`.
- Paths are two-way and all distances are positive, as required by Dijkstra's algorithm in the Maintenance module.

## Hostel names

The hostel names above are this version's proposal. Every module uses these names as exact text keys (rooms, complaint locations, worker locations, routes), so the team must agree on one final list and spelling before merging. Changing them later only requires editing `Campus::HOSTEL_NAMES` in `include/Campus.h`; the tests read the names from there.
