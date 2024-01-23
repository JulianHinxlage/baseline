cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 20)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(SOLUTION_NAME "Baseline")
if(WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
    set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
    set(USE_MSVC_RUNTIME_LIBRARY_DLL 1)
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
    add_link_options(-no-pie)
endif()
set(CMAKE_FOLDER Libs)

if(NOT DEFINED BASELINE_LIB_TYPE)
    set(BASELINE_LIB_TYPE SHARED)
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
add_library(${PROJECT_NAME} ${BASELINE_LIB_TYPE} ${SOURCES})
include_directories(${PROJECT_NAME} PRIVATE src)
target_link_libraries(${PROJECT_NAME} common)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER ${SOLUTION_NAME})

### Network ##################
project(network)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/network/*.cpp src/network/*.h)
add_library(${PROJECT_NAME} ${BASELINE_LIB_TYPE} ${SOURCES})
include_directories(${PROJECT_NAME} PRIVATE src)
target_link_libraries(${PROJECT_NAME} common)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER ${SOLUTION_NAME})
if(WIN32)
    target_link_libraries(${PROJECT_NAME} ws2_32)
else()
    target_link_libraries(${PROJECT_NAME} pthread)
endif()

### GUI ##################
project(gui)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/gui/*.cpp src/gui/*.h)
file(GLOB IMGUI_SOURCES CONFIGURE_DEPENDS libs/imgui/*.cpp libs/imgui/misc/cpp/imgui_stdlib.cpp libs/imgui/backends/imgui_impl_glfw.cpp libs/imgui/backends/imgui_impl_opengl3.cpp libs/imguizmo/ImGuizmo.cpp)
add_library(${PROJECT_NAME} ${BASELINE_LIB_TYPE} ${SOURCES} ${IMGUI_SOURCES})
include_directories(${PROJECT_NAME} PRIVATE src)
target_link_libraries(${PROJECT_NAME} common core)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER ${SOLUTION_NAME})
add_subdirectory(libs/glfw)
add_subdirectory(libs/glew/build/cmake)
if(WIN32)
    target_link_libraries(${PROJECT_NAME} glfw opengl32 glew_s imm32)
else()
    target_link_libraries(${PROJECT_NAME} glfw GL glew pthread stdc++fs)
endif()
target_include_directories(${PROJECT_NAME} PUBLIC libs libs/glfw/include libs/glew/include libs/imgui libs/imgui/backends)
set_target_properties(glfw uninstall update_mappings PROPERTIES FOLDER Libs)


### Render ##################
project(render)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/render/*.cpp src/render/*.h)
add_library(${PROJECT_NAME} ${BASELINE_LIB_TYPE} ${SOURCES})
include_directories(${PROJECT_NAME} PRIVATE src)
add_subdirectory(libs/glm)
target_link_libraries(${PROJECT_NAME} common core gui glm)
target_include_directories(${PROJECT_NAME} PUBLIC libs/stb)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER ${SOLUTION_NAME})

### Test ##################
project(test)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/test/*.cpp src/test/*.h)
add_library(${PROJECT_NAME} ${BASELINE_LIB_TYPE} ${SOURCES})
include_directories(${PROJECT_NAME} PRIVATE src)
target_link_libraries(${PROJECT_NAME} common gui)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER ${SOLUTION_NAME})

### Debug Menu ##################
project(debugMenu)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/debugMenu/*.cpp src/debugMenu/*.h)
add_library(${PROJECT_NAME} ${BASELINE_LIB_TYPE} ${SOURCES})
include_directories(${PROJECT_NAME} PRIVATE src)
target_link_libraries(${PROJECT_NAME} core common gui)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER ${SOLUTION_NAME})

### Launch ##################
project(launch)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/launch/*.cpp src/launch/*.h)
add_executable(${PROJECT_NAME} ${SOURCES})
include_directories(${PROJECT_NAME} PRIVATE src)
target_link_libraries(${PROJECT_NAME} core)
add_dependencies(${PROJECT_NAME} test network gui debugMenu)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER ${SOLUTION_NAME})
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})


project(${SOLUTION_NAME})
