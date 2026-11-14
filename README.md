# HostelPulse — Student / User Management Module

**Module owner:** Avinash  
**Branch:** `Avinash-Student`  
**Language/build:** C++17, CMake 3.16+

This module manages student records and Student/Admin login accounts. It is designed as a CMake library so the console demo, automated tests, and later UI can link to the same logic.

## Project structure

```text
HostelPulse-Student-Module/
├── include/
│   ├── AuthManager.h
│   ├── HashMap.h
│   ├── Student.h
│   └── StudentManager.h
├── src/
│   ├── AuthManager.cpp
│   ├── Student.cpp
│   └── StudentManager.cpp
├── tests/
│   └── test_student.cpp
├── CMakeLists.txt
├── main.cpp
├── README.md
└── .gitignore
```

## Features

- Register, find, update, remove, and list students.
- Store student records in the team's `HashMap<Student>` (separate chaining); `HashMap.h` is included unchanged from the supplied team file.
- Read-only student iteration through `forEachStudent` and const student lookup for UI use.
- Student and Admin account roles.
- Login, account creation, password change, account removal, and role lookup.
- Logic classes return results and do not print; console output is in `main.cpp`.
- CTest automated checks, including 1,000 records to exercise HashMap resizing.

## Build and run (macOS/Linux)

Install a C++17 compiler and CMake 3.16 or newer. From this folder:

```bash
cmake -S . -B build
cmake --build build
./build/student_menu
```

## Run automated tests

```bash
ctest --test-dir build --output-on-failure
```

The test executable can also be run directly:

```bash
./build/student_tests
```

The test suite covers Student constructors/getters/update, StudentManager CRUD and const access, read-only iteration, HashMap resizing with 1,000 students, and AuthManager account/password/role behaviour.

## Demo login

A fake demo-only Admin account is created when the console program starts:

- User ID: `ADMIN`
- Password: `change-me`

Do not use this demo password for a real account. Student registration asks for a password of at least 6 characters and creates a Student-role account with the same ID.

## Password security limitation — important

The supplied change specification defines a prototype FNV-1a-based hash repeated 10,000 times, salted with the user ID. This implementation follows that specification so the results are predictable for classroom tests. **FNV-1a is fast and is not a password-hashing algorithm designed to resist attackers.** It is acceptable only as a classroom prototype; do not use it for real accounts or production data. Before real deployment, replace `AuthManager::hashPassword` with a vetted password-hashing library such as Argon2id or bcrypt, with proper random salts and parameters. No plaintext passwords are stored by `AuthManager`.

## Integration notes

- Work only on branch `Avinash-Student` in the shared repository.
- `include/HashMap.h` must remain identical to the team's maintenance-branch version. The supplied file is copied unchanged into this package; verify it against the repository before merging.
- Hostel and room values remain free text until the team agrees on the shared location-name list. Do not add location validation yet.
- Student records and accounts currently live in memory only. They reset when the program exits; persistence/database integration is outside this module package.
- The demo menu is for local testing. The future UI should call the library APIs directly and handle its own presentation.

## Suggested Git workflow

After reviewing and understanding the changes, work on the correct branch and commit with descriptive messages. Do not run `git init` or rename the shared repository's default branch. Do not commit `build/`, executables, real passwords, or personal data.
