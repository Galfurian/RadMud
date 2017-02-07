#############################################
# Add sub-directory
add_subdirectory(map_generation)
add_subdirectory(terrain)

#############################################
# Include Directories
set(PROJECT_INC ${PROJECT_INC}
        ${CMAKE_SOURCE_DIR}/src/structure
        PARENT_SCOPE
        )

#############################################
# Source Files
set(PROJECT_SRC ${PROJECT_SRC}
        ${CMAKE_SOURCE_DIR}/src/structure/exit.cpp
        ${CMAKE_SOURCE_DIR}/src/structure/room.cpp
        ${CMAKE_SOURCE_DIR}/src/structure/area.cpp
        ${CMAKE_SOURCE_DIR}/src/structure/generator.cpp
        ${CMAKE_SOURCE_DIR}/src/structure/coordinates.cpp
        ${CMAKE_SOURCE_DIR}/src/structure/roomFactory.cpp
        PARENT_SCOPE
        )