cmake_minimum_required(VERSION 3.5)
project(trackersettings_test LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 REQUIRED COMPONENTS Core Test)

add_executable(trackersettings_test
    trackersettings_test.cpp
    ../src/trackersettings.cpp
)

target_include_directories(trackersettings_test PRIVATE ../src)

target_link_libraries(trackersettings_test Qt5::Core Qt5::Test)
