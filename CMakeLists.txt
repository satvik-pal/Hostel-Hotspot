cmake_minimum_required(VERSION 3.16)
project(HostelHotspot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

add_subdirectory(modules/maintenance)
add_subdirectory(modules/complaint)
add_subdirectory(modules/infrastructure)
add_subdirectory(modules/student)

add_executable(hostel_hotspot app/main.cpp)
target_link_libraries(hostel_hotspot PRIVATE maintenance complaints infrastructure students)

if(MSVC)
    target_compile_options(hostel_hotspot PRIVATE /W4)
else()
    target_compile_options(hostel_hotspot PRIVATE -Wall -Wextra)
endif()
find_package(Qt6 QUIET COMPONENTS Widgets)
if(Qt6_FOUND)
    message(STATUS "Qt6 found: building hostel_hotspot_gui")
    add_executable(hostel_hotspot_gui
        gui/main.cpp
        gui/MainWindow.cpp
        gui/AppCore.cpp
    )
    target_include_directories(hostel_hotspot_gui PRIVATE gui)
    target_link_libraries(hostel_hotspot_gui PRIVATE
        Qt6::Widgets maintenance complaints infrastructure students)
else()
    message(STATUS "Qt6 not found: skipping GUI (console app still builds)")
endif()
