cmake_minimum_required(VERSION 3.24)
project(obs-screen-capture-node)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Option to use system OBS for development
option(USE_SYSTEM_OBS "Use system OBS installation" ON)

if(USE_SYSTEM_OBS)
    # Find OBS installation - try framework first, then regular library
    find_path(OBS_INCLUDE_DIR obs/obs.h
        PATHS 
            /usr/local/include
            /opt/homebrew/include
            "C:/Program Files/obs-studio/include"
    )
    
    # Try to find OBS framework first (macOS)
    if(APPLE)
        find_library(OBS_LIBRARY 
            NAMES obs libobs
            PATHS 
                /usr/local/lib/libobs.framework
                /opt/homebrew/lib/libobs.framework
            NO_DEFAULT_PATH
        )
    endif()
    
    # If framework not found, try regular library
    if(NOT OBS_LIBRARY)
        find_library(OBS_LIBRARY 
            NAMES obs libobs
            PATHS 
                /usr/local/lib
                /opt/homebrew/lib
                "C:/Program Files/obs-studio/bin/64bit"
        )
    endif()
    
    if(OBS_INCLUDE_DIR AND OBS_LIBRARY)
        message(STATUS "Found OBS: ${OBS_LIBRARY}")
        add_compile_definitions(HAVE_OBS)
    else()
        message(WARNING "OBS not found, building foundation only")
    endif()
endif()

add_subdirectory(node-addon)