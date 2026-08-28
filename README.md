# Hostel Hotspot

## About the Project

Hostel Hotspot is a C++ based hostel management system developed as a team project.

The project aims to manage different aspects of hostel operations, including students, hostels, complaints, and maintenance.

## Project Modules

- Student Management
- Hostel Management
- Complaint Management
- Maintenance Management

## Technologies & Concepts

- C++
- Object-Oriented Programming (OOP)
- Data Structures and Algorithms (DSA)
- Standard Template Library (STL)
- Git & GitHub

## Current Progress

### Phase 1

The initial project structure has been created and the basic Maintenance module has been implemented.

### Maintenance Module

The current implementation includes:

- `MaintenanceWorker` class
- `MaintenanceManager` class
- Worker registration
- Automatic Worker ID generation
- Worker lookup by ID
- Worker removal
- Worker workload management
- Worker availability management
- Worker location update
- `unordered_map` for storing workers

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