cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 20)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(SOLUTION_NAME "baseline")
if(WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
    set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
    set(USE_MSVC_RUNTIME_LIBRARY_DLL 1)
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
    add_link_options(-no-pie)
endif()


function(assign_source_group)
    foreach(source IN LISTS ${ARGN})
        file(RELATIVE_PATH source_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${source}")
        get_filename_component(source_path "${source_rel}" PATH)
        string(REPLACE "/" "\\" source_path_msvc "${source_path}")
        source_group("${source_path_msvc}" FILES "${source}")
    endforeach()
endfunction(assign_source_group)

file(GLOB_RECURSE ALL_SOURCES CONFIGURE_DEPENDS src/*.cpp src/*.h)
assign_source_group(ALL_SOURCES)


### Common ##################
project(common)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/common/*.cpp src/common/*.h)
add_library(${PROJECT_NAME} STATIC ${SOURCES})
include_directories(${PROJECT_NAME} PRIVATE src)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER ${SOLUTION_NAME})

### Core ####################
project(core)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/core/*.cpp src/core/*.h)
add_library(${PROJECT_NAME} STATIC ${SOURCES})
include_directories(${PROJECT_NAME} PRIVATE src)
target_link_libraries(${PROJECT_NAME} common)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER ${SOLUTION_NAME})

### Test ##################
project(test)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/test/*.cpp src/test/*.h)
add_library(${PROJECT_NAME} SHARED ${SOURCES})
include_directories(${PROJECT_NAME} PRIVATE src)
target_link_libraries(${PROJECT_NAME} common)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER ${SOLUTION_NAME})

### Launch ##################
project(launch)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/launch/*.cpp src/launch/*.h)
add_executable(${PROJECT_NAME} ${SOURCES})
include_directories(${PROJECT_NAME} PRIVATE src)
target_link_libraries(${PROJECT_NAME} core)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER ${SOLUTION_NAME})
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

project(${SOLUTION_NAME})
