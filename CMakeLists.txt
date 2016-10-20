#################################################
# Check CMAKE Version.
cmake_minimum_required(VERSION 2.8.4 FATAL_ERROR)

#################################################
# Create the project...
project(RadMud CXX)

#################################################
# Set the project name...
SET(PROJECT_NAME RadMud)
SET(PROJECT_SOURCE_DIR src)

#################################################
# Initialize CXXFLAGS...
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wpedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wshadow")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wconversion")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wsign-compare")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wsign-conversion")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-function")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-label")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-parameter")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-value")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-variable")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wuninitialized")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wswitch")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wswitch-default")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wswitch-enum")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic-errors")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmessage-length=0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-switch-default")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O1")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-c++98-compat")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-c++98-compat-pedantic")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-documentation-unknown-command")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-noreturn")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-weak-vtables")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-padded")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-exit-time-destructors")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-prototypes")
endif ()

#################################################
# Activate compiler-specific C++11...
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if (COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif (COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else ()
    message(FATAL_ERROR "Compiler ${CMAKE_CXX_COMPILER} has no C++11 support.")
endif ()

#################################################
# Add cmake modules folder...
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

find_package(Lua52 REQUIRED)
find_package(ZLIB REQUIRED)
find_package(Sqlite3 REQUIRED)

#################################################
# Add subdirectories...
add_subdirectory(src/lua)
add_subdirectory(src/item)
add_subdirectory(src/model)
add_subdirectory(src/sqlite)
add_subdirectory(src/action)
add_subdirectory(src/command)
add_subdirectory(src/creation)
add_subdirectory(src/character)
add_subdirectory(src/utilities)
add_subdirectory(src/structure)

#################################################
# Add project sources...
set(PROJECT_SRC
        ./src/defines.cpp
        ./src/main.cpp
        ./src/mud.cpp
        ./src/updater.cpp
        ./src/utils.cpp
        ./src/opponent.cpp
        ./src/protocol.cpp
        ${RADMUD_LUA_SRC}
        ${RADMUD_ITEM_SRC}
        ${RADMUD_MODEL_SRC}
        ${RADMUD_ACTION_SRC}
        ${RADMUD_SQLITE_SRC}
        ${RADMUD_COMMAND_SRC}
        ${RADMUD_CREATION_SRC}
        ${RADMUD_CHARACTER_SRC}
        ${RADMUD_UTILITIES_SRC}
        ${RADMUD_STRUCTURE_SRC})

#################################################
# Add project includes...
include_directories(
        ${LUA_INCLUDE_DIR}
        ${ZLIB_INCLUDE_DIR}
        ${SQLITE3_INCLUDE_DIR}
        ${RADMUD_STRUCTURE_INC}
        ${RADMUD_UTILITIES_INC}
        ${RADMUD_CHARACTER_INC}
        ${RADMUD_CREATION_INC}
        ${RADMUD_COMMAND_INC}
        ${RADMUD_SQLITE_INC}
        ${RADMUD_ACTION_INC}
        ${RADMUD_MODEL_INC}
        ${RADMUD_ITEM_INC}
        ${RADMUD_LUA_INC}
        ./src
)

#################################################
# Add the executable...
add_executable(
        ${PROJECT_NAME}
        ${PROJECT_SRC}
)

#################################################
# Link the libraries...
target_link_libraries(
        ${PROJECT_NAME}
        ${LUA_LIBRARIES}
        ${SQLITE3_LIBRARY}
        pthread
        dl
        z
)
