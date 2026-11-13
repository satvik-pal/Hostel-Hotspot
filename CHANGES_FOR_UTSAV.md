# Changes made to your Infrastructure module (review before committing)

Prepared by Satvik. Your design and logic are unchanged; these are integration fixes only.

## What changed

1. `include/HashMap.h` and `include/DynamicArray.h` replaced with the team's shared versions
   from `Satvik-maintenance`. Your versions were different implementations; with them, the
   Maintenance module's Dijkstra does not compile (`DynamicArray` had no `reverse()`), and
   `HashMap` had no non-const `forEach`. All your original tests still pass with the shared files.
2. `tests/test_infrastructure.cpp`: added the two tests from the spec that were missing
   (now 34 checks):
   - every location is reachable from Main Gate (breadth-first traversal, DynamicArray as queue)
   - six shortest-path distances (193, 105, 93, 24, 34, 370 m) that the Maintenance module expects.
     They read hostel names from `Campus::HOSTEL_NAMES`, so they keep working if names change.
3. `main.cpp`: `#include <Campus.h>` changed to `#include "Campus.h"` (project headers use quotes).
4. `README.md`: replaced the "starter headers" note with the shared-headers rule, documented
   that `getEdge` returns `Edge{-1, 0}` for invalid input, and described the tests.

Not changed: hostel names. The team still has to agree on one final list.

## Verified

- 34/34 tests pass, built with -Wall -Wextra and memory checking (AddressSanitizer + UBSan): no warnings, no errors.
- The Maintenance module's Dijkstra runs on this HostelGraph and gives the expected routes.

## How to commit (from your branch)

    git checkout Utsav-Hostel
    git pull origin Utsav-Hostel
    # copy these files into the repository root (replace the old main.cpp/README.md)
    git fetch origin
    git show origin/Satvik-maintenance:include/HashMap.h      > include/HashMap.h
    git show origin/Satvik-maintenance:include/DynamicArray.h > include/DynamicArray.h
    cmake -S . -B build && cmake --build build
    ctest --test-dir build --output-on-failure          # expect 100% tests passed
    git add CMakeLists.txt .gitignore README.md main.cpp include src tests
    git commit -m "Build 5-hostel campus graph with room registry, team data structures and tests"
    git push origin Utsav-Hostel

The two `git show` lines make the shared headers byte-identical to the team's copy.
Read every change and make sure you can explain it in the viva.
