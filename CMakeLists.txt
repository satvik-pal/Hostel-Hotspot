cmake_minimum_required(VERSION 3.16)
project(HostelPulseStudents LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    set(PROJECT_WARNINGS /W4)
else()
    set(PROJECT_WARNINGS -Wall -Wextra -Wpedantic)
endif()

add_library(students
    src/Student.cpp
    src/StudentManager.cpp
    src/AuthManager.cpp
)
target_include_directories(students PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_options(students PRIVATE ${PROJECT_WARNINGS})

add_executable(student_menu main.cpp)
target_link_libraries(student_menu PRIVATE students)
target_compile_options(student_menu PRIVATE ${PROJECT_WARNINGS})

enable_testing()
add_executable(student_tests tests/test_student.cpp)
target_link_libraries(student_tests PRIVATE students)
target_compile_options(student_tests PRIVATE ${PROJECT_WARNINGS})
add_test(NAME student_tests COMMAND student_tests)
