# Changes made to your Complaint module (please review)

Prepared by Satvik with the team's agreement, following `Complaint_Module_Change_Spec.pdf`.
Your original design is kept: Priority enum, automatic IDs (C001...), keyword priority rules, menu style.

## What changed

1. Split into files: `include/` + `src/` + `tests/`, CMake library `complaints`, C++17.
2. `Complaint`: getters for every field, status (Pending/Assigned/Resolved), arrival order, assigned worker; only valid transitions allowed.
3. `PriorityRules`: fixed category list (= worker skills), exact validation, `toLowerCopy` without `std::transform`, keywords kept plus "short circuit".
4. `ComplaintManager`: `addComplaint(...)` takes parameters (no `cin`), team `HashMap` instead of `unordered_map`, `findComplaint`, `markAssigned`, `markResolved`, counts, read-only iteration.
5. Priority queue: team `MinHeap` with higher-priority-first, then first-come-first-served; `nextPending()` uses lazy deletion.
6. `main.cpp`: menu with 6 options; all input/output lives here.
7. 34 automated tests (CTest); README rewritten.

## Verified

34/34 tests pass with -Wall -Wextra -Wpedantic and memory checking (AddressSanitizer + UBSan); no warnings. Menu tested with scripted input.

## How to commit (on your branch)

    git checkout Saksham-Complaint
    git pull origin Saksham-Complaint
    # copy the files from this folder into the repository root (replace main.cpp and README.md)
    git fetch origin
    git show origin/Satvik-maintenance:include/HashMap.h > include/HashMap.h
    git show origin/Satvik-maintenance:include/MinHeap.h > include/MinHeap.h
    cmake -S . -B build && cmake --build build
    ctest --test-dir build --output-on-failure      # expect 100% tests passed
    git add CMakeLists.txt .gitignore README.md main.cpp include src tests
    git status                                      # no build/ folder
    git commit -m "Restructure Complaint module: library, status tracking, MinHeap priority queue, tests"
    git push origin Saksham-Complaint

Read every change so you can explain the heap ordering, lazy deletion and status rules in the viva.
