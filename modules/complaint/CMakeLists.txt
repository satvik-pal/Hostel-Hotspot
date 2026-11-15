cmake_minimum_required(VERSION 3.16)
project(HostelPulseComplaints LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    set(PROJECT_WARNINGS /W4)
else()
    set(PROJECT_WARNINGS -Wall -Wextra)
endif()

# ---------- Complaint module (library) ----------
add_library(complaints
    src/Complaint.cpp
    src/PriorityRules.cpp
    src/ComplaintManager.cpp
)
target_include_directories(complaints PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_options(complaints PRIVATE ${PROJECT_WARNINGS})

# ---------- Console menu ----------
add_executable(complaint_menu main.cpp)
target_link_libraries(complaint_menu PRIVATE complaints)
target_compile_options(complaint_menu PRIVATE ${PROJECT_WARNINGS})

# ---------- Automated tests ----------
enable_testing()
add_executable(complaint_tests tests/test_complaints.cpp)
target_link_libraries(complaint_tests PRIVATE complaints)
target_compile_options(complaint_tests PRIVATE ${PROJECT_WARNINGS})
add_test(NAME complaint_tests COMMAND complaint_tests)
