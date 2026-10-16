# HostelPulse — Student/User Management Module

A C++ console-based Student/User Management module for the HostelPulse hostel-management project. This module demonstrates OOP fundamentals and the C++ STL `unordered_map` for efficient student record management.

## Features

- Register a student
- Search a student by Student ID
- Update student details
- Remove a student
- Display all registered students
- Display the total number of students
- Reject empty IDs and duplicate IDs

## Technologies and concepts

- C++ (C++17)
- Classes and objects
- Encapsulation (`private` data members and public member functions)
- Default and parameterized constructors
- C++ Standard Template Library (STL)
- `unordered_map<string, Student>` hash map

## Project structure

```text
HostelPulse-Student-Management/
├── Student.h
├── Student.cpp
├── StudentManager.h
├── StudentManager.cpp
├── main.cpp
├── README.md
└── .gitignore
```

## Requirements

A C++ compiler such as `g++` or Apple Clang with C++17 support.

## Compile and run on macOS

Open Terminal in this folder and run:

```bash
g++ -std=c++17 main.cpp Student.cpp StudentManager.cpp -o hostel
./hostel
```

The program opens a menu. Use option `1` to register a student, `2` to search, `3` to update, `4` to remove, `5` to display all students, `6` to count students, and `0` to exit.

## Where the hash map is used

In `StudentManager.h`:

```cpp
std::unordered_map<std::string, Student> students;
```

The Student ID is the key and the `Student` object is the value.

- `emplace()` registers a student and prevents duplicate IDs.
- `find()` searches for a student by ID.
- `erase()` removes a student by ID.
- Iteration over the map displays all students.

Insertion, lookup, and deletion have **average-case O(1)** time complexity, although their worst case can be O(n).

## OOP implementation

- **`Student`** stores a student's ID, name, phone, hostel, and room.
- **Encapsulation:** the data members are private and accessed through public member functions.
- **Constructors:** a default constructor and a parameterized constructor initialize student objects.
- **`StudentManager`** owns the hash map and provides the record-management operations.

## Current limitations

- Records are held in memory only; they are lost when the program exits.
- This is the standalone console module, not yet connected to the web frontend or the other project modules.
- Authentication, persistent storage, and backend/API integration are not included in this checkpoint.

## Suggested manual tests

1. Register a student with ID `S001`.
2. Register `S001` again and confirm the duplicate is rejected.
3. Search for `S001` and confirm the details appear.
4. Search for an unknown ID and confirm “Student not found.”
5. Update `S001` and verify the changed details.
6. Remove `S001`, then search for it again.
7. Display all students and check the total count.

## GitHub upload

1. Create a new repository on GitHub.
2. Upload these files, or use Git from this folder:

```bash
git init
git add .
git commit -m "Add Student Management module"
git branch -M main
git remote add origin https://github.com/YOUR-USERNAME/YOUR-REPOSITORY.git
git push -u origin main
```

Replace the example remote URL with your own GitHub repository URL.
