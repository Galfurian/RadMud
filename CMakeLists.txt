# -----------------------------------------------------------------------------
# @brief  : Root cmake file.
# @author : Enrico Fraccaroli
# -----------------------------------------------------------------------------

# Set the minimum CMake version.
cmake_minimum_required(VERSION 3.1...3.18)

# Set the project name.
project(radmud CXX)

# Set the default build type to Debug.
if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "Setting build type to 'Debug' as none was specified.")
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)
endif()

# -----------------------------------------------------------------------------
# ENABLE FETCH CONTENT
# -----------------------------------------------------------------------------

# We need this in order to import external projects.
include(FetchContent)
# Hide fetchcontent variables.
mark_as_advanced(FORCE
    FETCHCONTENT_QUIET
    FETCHCONTENT_BASE_DIR
    FETCHCONTENT_FULLY_DISCONNECTED
    FETCHCONTENT_UPDATES_DISCONNECTED
)

# -----------------------------------------------------------------------------
# OPTIONS
# -----------------------------------------------------------------------------

option(STRICT_WARNINGS "Enable strict compiler warnings" ON)
option(WARNINGS_AS_ERRORS "Treat all warnings as errors" OFF)

# -----------------------------------------------------------------------------
# DEPENDENCY (SYSTEM LIBRARIES)
# -----------------------------------------------------------------------------

# Find the required packages.
find_package(Lua REQUIRED)
find_package(SQLite3 REQUIRED)

# Find optional packages.
find_package(Doxygen)
find_program(CLANG_TIDY_EXE NAMES clang-tidy)

# -----------------------------------------------------------------------------
# DEPENDENCY
# -----------------------------------------------------------------------------

# We need this in order to import external projects.
include(FetchContent)
# Retrieve the source.
FetchContent_Declare(luabridge
    GIT_REPOSITORY "https://github.com/kunitoki/LuaBridge3"
    GIT_TAG master
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)
# Retrieve the properties related to the content.
FetchContent_GetProperties(luabridge)
# If not populated, make the content available.
if(NOT luabridge_POPULATED)
    message(STATUS "Retrieving `luabridge`...")
    # Ensures the named dependencies have been populated.
    FetchContent_MakeAvailable(luabridge)
    # Hide fetchcontent variables, otherwise with ccmake it's a mess.
    mark_as_advanced(FORCE FETCHCONTENT_UPDATES_DISCONNECTED_LUABRIDGE FETCHCONTENT_SOURCE_DIR_LUABRIDGE)
endif()

# -----------------------------------------------------------------------------
# PROGRAM
# -----------------------------------------------------------------------------

file(GLOB_RECURSE PROJECT_HEADERS
    "${PROJECT_SOURCE_DIR}/include/**/*.hpp"
    "${PROJECT_SOURCE_DIR}/include/**/*.hh"
    "${PROJECT_SOURCE_DIR}/include/**/*.h"
    "${PROJECT_SOURCE_DIR}/include/*.hpp"
    "${PROJECT_SOURCE_DIR}/include/*.hh"
    "${PROJECT_SOURCE_DIR}/include/*.h"
)

file(GLOB_RECURSE PROJECT_SOURCES
    "${PROJECT_SOURCE_DIR}/src/**/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/**/*.cc"
    "${PROJECT_SOURCE_DIR}/src/**/*.c"
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/*.cc"
    "${PROJECT_SOURCE_DIR}/src/*.c"
)
set(PROJECT_HEADERS_AND_SOURCES ${PROJECT_SOURCES} ${PROJECT_HEADERS})

# Add the program
add_executable(
    ${PROJECT_NAME}
    ${PROJECT_HEADERS}
    ${PROJECT_SOURCES}
)
# Inlcude header directories and set the library.
target_include_directories(
    ${PROJECT_NAME} PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)
target_include_directories(
    ${PROJECT_NAME} SYSTEM PUBLIC
    ${luabridge_SOURCE_DIR}/Source
    ${LUA_INCLUDE_DIR}
    ${SQLite3_INCLUDE_DIRS}
)
# Link the required libraries.
target_link_libraries(
    ${PROJECT_NAME} PUBLIC
    ${LUA_LIBRARIES}
    ${SQLite3_LIBRARIES}
    pthread
    dl
    z
)
# Set compiler flags.
target_compile_features(
    ${PROJECT_NAME} PUBLIC
    cxx_std_17
)

# -----------------------------------------------------------------------------
# COMPILATION FLAGS
# -----------------------------------------------------------------------------

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # Disable warnings for MSVC-specific "safe" functions like strcpy_s, etc.,
    # which are not portable and may clutter warning logs.
    target_compile_definitions(${PROJECT_NAME} PUBLIC _CRT_SECURE_NO_WARNINGS)

    # Disable warning C4702: unreachable code.
    add_compile_options(/wd4702)

    if(WARNINGS_AS_ERRORS)
        # Treat all warnings as errors to enforce stricter code quality.
        target_compile_options(${PROJECT_NAME} PUBLIC /WX)
    endif()

    if(STRICT_WARNINGS)
        # Enable external header management to suppress warnings in system and
        # external headers, making it easier to focus on project-specific issues.
        target_compile_options(${PROJECT_NAME} PUBLIC /experimental:external)
        target_compile_options(${PROJECT_NAME} PUBLIC /external:I ${CMAKE_BINARY_DIR})
        target_compile_options(${PROJECT_NAME} PUBLIC /external:anglebrackets)
        target_compile_options(${PROJECT_NAME} PUBLIC /external:W0)

        # Use a high warning level to catch as many potential issues as possible.
        target_compile_options(${PROJECT_NAME} PUBLIC /W4)

        # Enforce standards-compliant behavior to avoid relying on MSVC-specific extensions.
        target_compile_options(${PROJECT_NAME} PUBLIC /permissive-)
    endif()

elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    if(WARNINGS_AS_ERRORS)
        # Treat all warnings as errors to enforce stricter code quality.
        target_compile_options(${PROJECT_NAME} PUBLIC -Werror)
    endif()

    if(STRICT_WARNINGS)
        # Enable a broad set of warnings to catch common and subtle issues:
        target_compile_options(${PROJECT_NAME} PUBLIC
            -Wall # Enable most general-purpose warnings.
            -Wextra # Enable extra warnings not included in -Wall.
            -Wconversion # Warn about implicit type conversions that may lose data.
            -pedantic # Enforce strict compliance with the C++ standard.
            -Wshadow # Warn about variable shadowing, which can cause subtle bugs.
            -Wnon-virtual-dtor # Warn when a class with virtual functions lacks a virtual destructor.
            -Wnull-dereference # Warn about potential null pointer dereferences.
            -Wformat=2 # Enable strict checks for printf/scanf format strings.
            -Woverloaded-virtual # Warn when a derived class function hides a base class virtual function.
            -Wfloat-equal # Warn about direct comparisons of floating-point values, which can be imprecise.
        )

    endif()
endif()

# -----------------------------------------------------------------------------
# CODE ANALYSIS
# -----------------------------------------------------------------------------

if(CLANG_TIDY_EXE)
    add_custom_target(
        ${PROJECT_NAME}_clang_tidy
        COMMAND ${CLANG_TIDY_EXE}
        --p=${CMAKE_BINARY_DIR}
        ${PROJECT_HEADERS_AND_SOURCES}
        COMMENT "Running clang-tidy"
        VERBATIM
    )
    add_custom_target(
        ${PROJECT_NAME}_clang_tidy_fix
        COMMAND ${CLANG_TIDY_EXE}
        --fix --fix-errors
        --p=${CMAKE_BINARY_DIR}
        ${PROJECT_HEADERS_AND_SOURCES}
        COMMENT "Running clang-tidy-fix"
        VERBATIM
    )
endif()

# -----------------------------------------------------------------------------
# DOCUMENTATION
# -----------------------------------------------------------------------------

if(DOXYGEN_FOUND)

    # FetchContent: Doxygen Awesome CSS
    FetchContent_Declare(doxygenawesome
        GIT_REPOSITORY https://github.com/jothepro/doxygen-awesome-css
        GIT_TAG main
    )
    FetchContent_MakeAvailable(doxygenawesome)

    # Hide FetchContent variables to avoid clutter in ccmake.
    mark_as_advanced(FORCE
        FETCHCONTENT_UPDATES_DISCONNECTED_DOXYGENAWESOME
        FETCHCONTENT_SOURCE_DIR_DOXYGENAWESOME
    )

    # Read the file with the version.
    file(READ ${PROJECT_SOURCE_DIR}/include/mud.hpp version_file)
    # Extract the version.
    string(REGEX MATCH "RADMUD_MAJOR_VERSION ([0-9]*)" _ ${version_file})
    set(RADMUD_MAJOR_VERSION ${CMAKE_MATCH_1})
    string(REGEX MATCH "RADMUD_MINOR_VERSION ([0-9]*)" _ ${version_file})
    set(RADMUD_MINOR_VERSION ${CMAKE_MATCH_1})
    string(REGEX MATCH "RADMUD_MICRO_VERSION ([0-9]*)" _ ${version_file})
    set(RADMUD_MICRO_VERSION ${CMAKE_MATCH_1})

    # Customization: Doxygen Configuration
    set(DOXYGEN_WARN_FORMAT "$file:$line:1: $text")
    set(DOXYGEN_PROJECT_NAME RadMud)
    set(DOXYGEN_PROJECT_BRIEF "A lightweight MUD engine written in C++.")
    set(DOXYGEN_PROJECT_NUMBER "${RADMUD_MAJOR_VERSION}.${RADMUD_MINOR_VERSION}.${RADMUD_MICRO_VERSION}")
    set(DOXYGEN_USE_MDFILE_AS_MAINPAGE ${PROJECT_SOURCE_DIR}/README.md)
    set(DOXYGEN_SHOW_INCLUDE_FILES NO)
    set(DOXYGEN_GENERATE_TREEVIEW YES)
    set(DOXYGEN_GENERATE_LATEX NO)
    set(DOXYGEN_GENERATE_MAN NO)

    # Styling and UX enhancements using Doxygen Awesome
    set(DOXYGEN_HTML_HEADER ${doxygenawesome_SOURCE_DIR}/doxygen-custom/header.html)
    set(DOXYGEN_HTML_EXTRA_STYLESHEET ${doxygenawesome_SOURCE_DIR}/doxygen-awesome.css)
    set(DOXYGEN_HTML_EXTRA_FILES
        ${doxygenawesome_SOURCE_DIR}/doxygen-awesome-fragment-copy-button.js
        ${doxygenawesome_SOURCE_DIR}/doxygen-awesome-paragraph-link.js
        ${doxygenawesome_SOURCE_DIR}/doxygen-awesome-darkmode-toggle.js
    )

    # Set stricter warnings for better documentation quality
    set(DOXYGEN_WARN_IF_UNDOCUMENTED YES)
    set(DOXYGEN_WARN_IF_DOC_ERROR YES)
    set(DOXYGEN_WARN_NO_PARAMDOC YES)
    set(DOXYGEN_WARN_AS_ERROR NO)

    # Exclude certain files or directories from documentation (if needed)
    set(DOXYGEN_EXCLUDE_PATTERNS "${PROJECT_SOURCE_DIR}/tests/*" "${PROJECT_SOURCE_DIR}/examples/*")

    doxygen_add_docs(
        radmud_documentation
        ${PROJECT_SOURCE_DIR}/README.md
        ${PROJECT_SOURCE_DIR}/LICENSE.md
        ${PROJECT_HEADERS_AND_SOURCES}
        COMMENT "Generating Doxygen documentation"
    )

endif()
