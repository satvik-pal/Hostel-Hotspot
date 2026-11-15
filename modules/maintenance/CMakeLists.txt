cmake_minimum_required(VERSION 3.16)
project(HostelPulse LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler warnings (MSVC on Windows uses different flags)
if(MSVC)
    set(PROJECT_WARNINGS /W4)
else()
    set(PROJECT_WARNINGS -Wall -Wextra)
endif()

# ---------- Maintenance module (library) ----------
add_library(maintenance
    src/MaintenanceWorker.cpp
    src/MaintenanceManager.cpp
)
target_include_directories(maintenance PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_options(maintenance PRIVATE ${PROJECT_WARNINGS})

# ---------- Demo program ----------
add_executable(hostel main.cpp)
target_link_libraries(hostel PRIVATE maintenance)
target_compile_options(hostel PRIVATE ${PROJECT_WARNINGS})

# ---------- Automated tests ----------
enable_testing()
add_executable(maintenance_tests tests/test_maintenance.cpp)
target_link_libraries(maintenance_tests PRIVATE maintenance)
target_compile_options(maintenance_tests PRIVATE ${PROJECT_WARNINGS})
add_test(NAME maintenance_tests COMMAND maintenance_tests)