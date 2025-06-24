cmake_minimum_required(VERSION 3.24)
project(obs-screen-capture-node)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# For now, let's build without full OBS integration
# This gives us the foundation to add OBS incrementally
add_subdirectory(node-addon)