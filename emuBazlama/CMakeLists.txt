cmake_minimum_required(VERSION 3.15...4.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(PROJECT_NAME EmuBazlama)

# folders
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(LIBS_LOCAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../libsLocal)

#Disable header only
set(SPDLOG_BUILD_SHARED OFF)
set(SPDLOG_BUILD_EXAMPLE OFF)
set(SPDLOG_BUILD_TESTS OFF)
set(SPDLOG_BUILD_BENCH OFF)


#! ! ! ! ! ! !
#set this to ON instead of OFF to ship the game!
#basically this will change the RESOURCES_PATH to be the local path
#! ! ! ! ! ! !
#delete the out folder after changing if visual studio doesn recognize the change
set(PRODUCTION_BUILD ON CACHE BOOL "Make this a production build" FORCE)





project(${PROJECT_NAME} VERSION 0.1 LANGUAGES CXX)

message(STATUS "=========MESSAGE !!LIBS HERE!!===========: ${LIBS_LOCAL_DIR}")
#====================LIBS=============================================#
#=====================================================================#

add_subdirectory(${LIBS_LOCAL_DIR}/SDL3 SDL_build)
add_subdirectory(${LIBS_LOCAL_DIR}/spdlog spdlog_build)
add_subdirectory(${LIBS_LOCAL_DIR}/imgui-docking imgui_build)
add_subdirectory(${LIBS_LOCAL_DIR}/tiny-process tiny_process_build)


#=====================================================================#
#=====================================================================#

# we define MY_SOURCES to be a list of all the source files for my project
file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(${PROJECT_NAME}  ${MY_SOURCES})

#====================LIBS=============================================#
#=====================================================================#

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${LIBS_LOCAL_DIR}
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    SDL3::SDL3
    spdlog::spdlog
    imgui
    tiny-process-library
)
#=====================================================================#
#=====================================================================#

# PATHS
# This is useful to get an ASSETS_PATH in your IDE during development but you should comment this if you compile a release version and uncomment the next line
target_compile_definitions(${PROJECT_NAME} PUBLIC cmake_PROJECT_TESTS="${CMAKE_CURRENT_SOURCE_DIR}/tests/")

target_compile_definitions(${PROJECT_NAME} PUBLIC cmake_PROJECT_OUTPUT="${CMAKE_CURRENT_SOURCE_DIR}/output/")

target_compile_definitions(${PROJECT_NAME} PUBLIC cmake_PROJECT_LOGS="${CMAKE_CURRENT_SOURCE_DIR}/logs/")

get_filename_component(ASM_DEBUG
    "${CMAKE_CURRENT_SOURCE_DIR}/../Assembler/out/build/x64-Debug"
    ABSOLUTE
)

#TODO RELEASE ASSEMBLER
get_filename_component(ASM_RELEASE
    "${CMAKE_CURRENT_SOURCE_DIR}/../Assembler/out/build/x64-Debug"
    ABSOLUTE
)

target_compile_definitions(${PROJECT_NAME} PUBLIC
    cmake_ASSEMBLER_PATH=$<$<CONFIG:Debug>:"${ASM_DEBUG}/">
    cmake_ASSEMBLER_PATH=$<$<CONFIG:Release>:"${ASM_RELEASE}/">
)

#=====================================================================#
#=====================================================================#



target_precompile_headers(${PROJECT_NAME} PRIVATE
    ${SRC_DIR}/pch.h
)

if(PRODUCTION_BUILD)
	# setup the ASSETS_PATH macro to be in the root folder of your exe
	target_compile_definitions(${PROJECT_NAME} PUBLIC cmake_PROJECT_TESTS="./tests/") 

	# remove the option to debug asserts.
	target_compile_definitions(${PROJECT_NAME} PUBLIC PRODUCTION_BUILD=1) 
endif()


if(MSVC)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif()



