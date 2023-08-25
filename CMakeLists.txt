cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 20)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(SOLUTION_NAME "baseline")

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


project(core)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/core/*.cpp src/core/*.h)
add_library(${PROJECT_NAME} STATIC ${SOURCES})
include_directories(${PROJECT_NAME} PRIVATE src src/core)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER ${SOLUTION_NAME})

project(launch)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/launch/*.cpp src/launch/*.h)
add_executable(${PROJECT_NAME} ${SOURCES})
include_directories(${PROJECT_NAME} PRIVATE src src/launch)
target_link_libraries(${PROJECT_NAME} core)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER ${SOLUTION_NAME})
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

project(${SOLUTION_NAME})
