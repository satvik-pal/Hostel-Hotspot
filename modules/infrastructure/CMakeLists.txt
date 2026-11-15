cmake_minimum_required(VERSION 3.16)
project(HostelPulseInfrastructure LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    set(PROJECT_WARNINGS /W4)
else()
    set(PROJECT_WARNINGS -Wall -Wextra)
endif()

add_library(infrastructure
    src/HostelGraph.cpp
    src/Room.cpp
    src/Campus.cpp
)

target_include_directories(infrastructure PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)
target_compile_options(infrastructure PRIVATE ${PROJECT_WARNINGS})

add_executable(campus_demo main.cpp)
target_link_libraries(campus_demo PRIVATE infrastructure)
target_compile_options(campus_demo PRIVATE ${PROJECT_WARNINGS})

enable_testing()
add_executable(infrastructure_tests tests/test_infrastructure.cpp)
target_link_libraries(infrastructure_tests PRIVATE infrastructure)
target_compile_options(infrastructure_tests PRIVATE ${PROJECT_WARNINGS})
add_test(NAME infrastructure_tests COMMAND infrastructure_tests)
