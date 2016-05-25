#################################################
# Check CMAKE Version.
cmake_minimum_required(VERSION 2.8.4 FATAL_ERROR)

option(USE_CLANG       "build application with clang" OFF)
option(FLAG_EVERYTHING "build with clang and set warning level to everything" OFF)

if(USE_CLANG)
    SET (CMAKE_C_COMPILER   "clang")
	SET (CMAKE_CXX_COMPILER "clang++")
endif(USE_CLANG)

#################################################
# Create the project.
project(RadMud CXX)

#################################################
# Set the project name.
SET(PROJECT_NAME RadMud)
SET(PROJECT_SOURCE_DIR src)

#################################################
# Initialize CXXFLAGS.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wpedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic-errors")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wconversion")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O1")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmessage-length=0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

if(USE_CLANG)
	if(FLAG_EVERYTHING)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-c++98-compat")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-c++98-compat-pedantic")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-global-constructors")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-exit-time-destructors")
	endif(FLAG_EVERYTHING)
endif(USE_CLANG)


#################################################
# Compiler-specific C++11 activation.
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
    message(FATAL_ERROR "Compiler ${CMAKE_CXX_COMPILER} has no C++11 support.")
endif()

#################################################
# Set Modules dir.
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

set(PROJECT_SRC
    ./src/actions.cpp
    ./src/area.cpp
    ./src/building.cpp
    ./src/character.cpp
    ./src/comm_combat.cpp
    ./src/comm_communication.cpp
    ./src/comm_crafting.cpp
    ./src/comm_gathering.cpp
    ./src/comm_general.cpp
    ./src/comm_gods.cpp
    ./src/comm_object.cpp
    ./src/commands.cpp
    ./src/continent.cpp
    ./src/defines.cpp
    ./src/effect.cpp
    ./src/exit.cpp
    ./src/faction.cpp
    ./src/generator.cpp
    ./src/item.cpp
    ./src/liquid.cpp
    ./src/lua/lua_script.cpp
    ./src/main.cpp
    ./src/material.cpp
    ./src/mobile.cpp
    ./src/model.cpp
    ./src/mud.cpp
    ./src/player.cpp
    ./src/production.cpp
    ./src/profession.cpp
    ./src/race.cpp
    ./src/room.cpp
    ./src/skill.cpp
    ./src/sqlite/SQLiteDbms.cpp
    ./src/sqlite/SQLiteWrapper.cpp
    ./src/updater.cpp
    ./src/utilities/coordinates.cpp
    ./src/utilities/table.cpp
    ./src/utils.cpp
    ./src/telnet.cpp
    ./src/combat.cpp
)

find_package(Lua51 REQUIRED)
find_package(ZLIB REQUIRED)
find_package(Sqlite3 REQUIRED)

include_directories(
	${LUA_INCLUDE_DIRS}
	${ZLIB_INCLUDE_DIRS}
	${SQLITE3_INCLUDE_DIRS}
	./src/luabridge
	./src/utilities
	./src/sqlite
	./src/lua
	./src
)

add_executable(
	${PROJECT_NAME}
	${PROJECT_SRC}
)

target_link_libraries(
	${PROJECT_NAME}
	pthread
	sqlite3
	lua
	dl
	z
)
