# Hostel Infrastructure Graph

A C++ program that models a hostel's layout as a **weighted, undirected graph** using an **adjacency list**.

- **Vertices** are locations (Main Gate, Reception, Rooms, Mess, etc.)
- **Edges** are walkable paths between them, weighted by distance in metres

## Features

- Add locations (vertices) to the graph
- Connect locations with weighted, two-way paths (edges)
- Input validation for invalid vertex indexes
- Display the full graph as an adjacency list

## Graph Layout

| Index | Location    |
|-------|-------------|
| 0     | Main Gate   |
| 1     | Reception   |
| 2     | Room 101    |
| 3     | Room 102    |
| 4     | Room 103    |
| 5     | Mess        |
| 6     | Common Room |
| 7     | Washroom    |
| 8     | Stairs      |

## Project Structure

```
.
├── main.cpp
├── README.md
└── .gitignore
```

## How to Build and Run

Requires a C++ compiler such as g++ (C++11 or later).

```bash
g++ main.cpp -o hostel
./hostel          # Linux / macOS
hostel.exe        # Windows
```

## Sample Output

```
  HOSTEL INFRASTRUCTURE GRAPH   
Main Gate -> Reception (10m)  
Reception -> Main Gate (10m)  Room 101 (15m)  Mess (20m)  
Room 101 -> Reception (15m)  Room 102 (8m)  Washroom (5m)  
Room 102 -> Room 101 (8m)  Room 103 (8m)  Washroom (5m)  
Room 103 -> Room 102 (8m)  Stairs (12m)  
Mess -> Reception (20m)  Common Room (10m)  
Common Room -> Mess (10m)  Stairs (15m)  
Washroom -> Room 101 (5m)  Room 102 (5m)  
Stairs -> Room 103 (12m)  Common Room (15m)  
```

## Concepts Used

- Graphs (adjacency list representation)
- Structs and classes
- STL `vector` and `string`

## Future Improvements

- Shortest path between two locations (Dijkstra's algorithm)
- BFS and DFS traversal
- User input for adding locations and paths

## Author

Your Name
