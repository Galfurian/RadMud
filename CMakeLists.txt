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
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weffc++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wpedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic-errors")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wconversion")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wcast-align")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wcast-qual")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wdisabled-optimization")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wfloat-equal")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wfloat-conversion")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wformat=2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wformat-nonliteral")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wformat-security")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wformat-y2k")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wimport")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Winit-self")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Winline")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Winvalid-pch")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-long-long")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wmissing-field-initializers")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wmissing-format-attribute")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wmissing-include-dirs")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wmissing-noreturn")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wpacked")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wpadded")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wpointer-arith")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wredundant-decls")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wshadow")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wstack-protector")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wstrict-aliasing=2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wswitch")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wswitch-default")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wswitch-enum")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunreachable-code")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-function")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-label")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-parameter")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-value")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-variable")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wvariadic-macros")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wwrite-strings")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wsign-compare")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wsign-conversion")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wuninitialized")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmessage-length=0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O1")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-non-virtual-dtor")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-switch-default")

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
find_package(Lua REQUIRED)
find_package(ZLIB REQUIRED)
find_package(Sqlite3 REQUIRED)

#################################################
# Add root project sources...
set(PROJECT_SRC
        ${CMAKE_SOURCE_DIR}/src/main.cpp
        ${CMAKE_SOURCE_DIR}/src/mud.cpp
        )

set(PROJECT_INC ${CMAKE_SOURCE_DIR}/src)

#################################################
# Add subdirectories...
add_subdirectory(${CMAKE_SOURCE_DIR}/src/enumerators)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/generation)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/character)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/utilities)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/structure)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/structure/algorithms)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/creation)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/command)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/updater)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/sqlite)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/action)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/action/combat)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/action/object)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/input)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/input/initialization)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/model)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/model/submodel)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/item)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/item/subitem)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/lua)

#################################################
# Add project includes...
include_directories(
        ${PROJECT_INC}
        ${LUA_INCLUDE_DIR}
        ${ZLIB_INCLUDE_DIR}
        ${SQLITE3_INCLUDE_DIR}
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
