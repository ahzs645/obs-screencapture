cmake_minimum_required(VERSION 3.24)
project(obs-screen-capture-node)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Options
option(BUILD_FOR_NODE "Build as Node.js addon" ON)
option(USE_SYSTEM_OBS "Use system OBS installation" OFF)

if(BUILD_FOR_NODE)
    add_subdirectory(node-addon)
endif()