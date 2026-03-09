cmake_minimum_required(VERSION 3.15...4.0)

project(Assembler VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# folders
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(LIBS_LOCAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../libsLocal)

add_subdirectory(${LIBS_LOCAL_DIR}/SDL3 SDL_build)

# we define MY_SOURCES to be a list of all the source files for my project
file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(Assembler ${MY_SOURCES})

# This is useful to get an ASSETS_PATH in your IDE during development but you should comment this if you compile a release version and uncomment the next line
target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC cmake_PROJECT_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/tests/")

# Uncomment this line to setup the ASSETS_PATH macro to the final assets directory when you share the project
#target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC cmake_PROJECT_ROOT="./tests/") 

target_precompile_headers(Assembler PRIVATE
    ${SRC_DIR}/pch.h
)

# Link
target_link_libraries(Assembler PRIVATE SDL3::SDL3)

# include directories so headers can be found
target_include_directories(Assembler PRIVATE
    ${SRC_DIR}
    ${LIBS_LOCAL_DIR}
)

if(MSVC)
    set_target_properties(Assembler PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
endif()