cmake_minimum_required(VERSION 3.15...4.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(PROJECT_NAME Assembler)

# folders
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(LIBS_LOCAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../libsLocal)

#! ! ! ! ! ! !
#set this to ON instead of OFF to ship the game!
#basically this will change the RESOURCES_PATH to be the local path
#! ! ! ! ! ! !
#delete the out folder after changing if visual studio doesn recognize the change
set(PRODUCTION_BUILD ON CACHE BOOL "Make this a production build" FORCE)


project(${PROJECT_NAME} VERSION 0.1 LANGUAGES CXX)


# we define MY_SOURCES to be a list of all the source files for my project
file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(${PROJECT_NAME}  ${MY_SOURCES})

# This is useful to get an ASSETS_PATH in your IDE during development but you should comment this if you compile a release version and uncomment the next line
target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC cmake_PROJECT_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/tests/")

target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC cmake_PROJECT_OUTPUT="${CMAKE_CURRENT_SOURCE_DIR}/output/")


target_precompile_headers(${PROJECT_NAME} PRIVATE
    ${SRC_DIR}/pch.h
)

# include directories so headers can be found
target_include_directories(${PROJECT_NAME} PRIVATE
    ${SRC_DIR}
    ${LIBS_LOCAL_DIR}
)

if(PRODUCTION_BUILD)
	# setup the ASSETS_PATH macro to be in the root folder of your exe
	target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC cmake_PROJECT_ROOT="./tests/") 
    
    get_filename_component(PARENT_DIR "${CMAKE_SOURCE_DIR}" DIRECTORY)
    set(EMU_TEST_DIR "${PARENT_DIR}/emuBazlama/tests/")

    target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC cmake_EMU_HEX_OUTPUT_PATH="${EMU_TEST_DIR}") 

	# remove the option to debug asserts.
	target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC PRODUCTION_BUILD=1) 


endif()

if(MSVC)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
endif()