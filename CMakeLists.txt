#################################################
# Check CMAKE Version.
cmake_minimum_required(VERSION 2.8.4 FATAL_ERROR)

option(FLAG_EVERYTHING "set a higher warning level" OFF)

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
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmessage-length=0")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O1")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3")

if(FLAG_EVERYTHING)
	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-c++98-compat")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-c++98-compat-pedantic")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-global-constructors")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-exit-time-destructors")
	elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wcast-align")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wcast-qual")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wchar-subscripts")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wcomment")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wconversion")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wdisabled-optimization")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wfloat-equal")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wformat")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wformat=2")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wformat-nonliteral")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wformat-security")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wformat-y2k")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wimport")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Winit-self")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Winvalid-pch")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wlong-long")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wmissing-braces")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wmissing-field-initializers")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wmissing-format-attribute")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wmissing-include-dirs")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wmissing-noreturn")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wpacked")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wparentheses")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wpointer-arith")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wredundant-decls")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wreturn-type")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wsequence-point")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wshadow")
		#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Winline")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wredundant-decls")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wsign-compare")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wsign-conversion")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wstack-protector")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wstrict-aliasing")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wstrict-aliasing=2")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wswitch")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wswitch-default")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wswitch-enum")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wtrigraphs")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wuninitialized")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunknown-pragmas")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunreachable-code")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-function")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-label")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-parameter")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-value")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-variable")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wvariadic-macros")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wvolatile-register-var")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wwrite-strings")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wuninitialized")
	endif()
endif(FLAG_EVERYTHING)

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

add_subdirectory(src/lua)
add_subdirectory(src/item)
add_subdirectory(src/model)
add_subdirectory(src/sqlite)
add_subdirectory(src/action)
add_subdirectory(src/command)
add_subdirectory(src/utilities)

set(PROJECT_SRC
    ./src/area.cpp
    ./src/building.cpp
    ./src/character.cpp
    ./src/continent.cpp
    ./src/defines.cpp
    ./src/effect.cpp
    ./src/exit.cpp
    ./src/faction.cpp
    ./src/generator.cpp
    ./src/liquid.cpp
    ./src/main.cpp
    ./src/material.cpp
    ./src/mobile.cpp
    ./src/mud.cpp
    ./src/player.cpp
    ./src/production.cpp
    ./src/profession.cpp
    ./src/race.cpp
    ./src/room.cpp
    ./src/skill.cpp
    ./src/updater.cpp
    ./src/utils.cpp
    ./src/formatter.cpp
    ./src/opponent.cpp
    ./src/logger.cpp
    ./src/protocol.cpp
    ./src/luabridge/detail/LuaHelpers.cpp
    ${RADMUD_LUA_SRC}
    ${RADMUD_ITEM_SRC}
    ${RADMUD_MODEL_SRC}
    ${RADMUD_ACTION_SRC}
    ${RADMUD_SQLITE_SRC}
    ${RADMUD_COMMAND_SRC}
    ${RADMUD_UTILITIES_SRC}
)

find_package(Lua52 REQUIRED)
find_package(ZLIB REQUIRED)
find_package(Sqlite3 REQUIRED)

include_directories(
	${LUA_INCLUDE_DIR}
	${ZLIB_INCLUDE_DIR}
	${SQLITE3_INCLUDE_DIR}
	${RADMUD_UTILITIES_INC}
    ${RADMUD_COMMAND_INC}
	${RADMUD_SQLITE_INC}
    ${RADMUD_ACTION_INC}
	${RADMUD_MODEL_INC}
    ${RADMUD_ITEM_INC}
	${RADMUD_LUA_INC}
	./src/luabridge/detail
	./src/luabridge
	./src
)

add_executable(
	${PROJECT_NAME}
	${PROJECT_SRC}
)

target_link_libraries(
	${PROJECT_NAME}
	${LUA_LIBRARIES}
	${SQLITE3_LIBRARY}
	pthread
	dl
	z
)
