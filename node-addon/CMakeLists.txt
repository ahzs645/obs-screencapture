# Get Node.js configuration
execute_process(
    COMMAND node -p "require('node-addon-api').include_dir"
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE NODE_ADDON_API_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
    COMMAND node -p "process.execPath.replace('/bin/node', '/include/node')"
    OUTPUT_VARIABLE NODE_INCLUDE_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Create the Node addon
add_library(obs_screen_capture SHARED
    src/obs_screen_capture.cpp
    src/obs_wrapper.cpp
)

if(APPLE)
    target_sources(obs_screen_capture PRIVATE src/permission_manager.mm)
    target_link_libraries(obs_screen_capture PRIVATE
        "-framework Cocoa"
        "-framework CoreGraphics" 
        "-framework ScreenCaptureKit"
        "-framework AVFoundation"
        "-framework ApplicationServices"
    )
endif()

target_include_directories(obs_screen_capture PRIVATE
    ${CMAKE_SOURCE_DIR}/${NODE_ADDON_API_DIR}
    ${NODE_INCLUDE_DIR}
    src/
)

# Add OBS if available
if(USE_SYSTEM_OBS AND OBS_INCLUDE_DIR AND OBS_LIBRARY)
    target_include_directories(obs_screen_capture PRIVATE ${OBS_INCLUDE_DIR})
    target_link_libraries(obs_screen_capture PRIVATE ${OBS_LIBRARY})
    
    # Find OBS plugins directory
    get_filename_component(OBS_LIB_DIR ${OBS_LIBRARY} DIRECTORY)
    find_path(OBS_PLUGINS_DIR obs-plugins
        PATHS 
            ${OBS_LIB_DIR}/../lib
            /usr/local/lib
            /opt/homebrew/lib
    )
    
    if(OBS_PLUGINS_DIR)
        target_compile_definitions(obs_screen_capture PRIVATE 
            OBS_PLUGINS_PATH="${OBS_PLUGINS_DIR}/obs-plugins"
        )
    endif()
endif()

# Configure as Node addon
set_target_properties(obs_screen_capture PROPERTIES
    PREFIX ""
    SUFFIX ".node"
    CXX_STANDARD 17
    POSITION_INDEPENDENT_CODE ON
)

# Node.js addon linking
if(APPLE)
    set_target_properties(obs_screen_capture PROPERTIES
        LINK_FLAGS "-undefined dynamic_lookup"
    )
elseif(WIN32)
    execute_process(
        COMMAND node -p "process.execPath.replace('node.exe', 'node.lib')"
        OUTPUT_VARIABLE NODE_LIB_FILE
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    target_link_libraries(obs_screen_capture PRIVATE ${NODE_LIB_FILE})
endif()

target_compile_definitions(obs_screen_capture PRIVATE NAPI_DISABLE_CPP_EXCEPTIONS)

# OBS integration will be added incrementally