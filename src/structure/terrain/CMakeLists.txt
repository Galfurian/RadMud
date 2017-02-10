#############################################
# Include Directories
set(PROJECT_INC ${PROJECT_INC}
        ${CMAKE_SOURCE_DIR}/src/structure/terrain
        PARENT_SCOPE
        )

#############################################
# Source Files
set(PROJECT_SRC ${PROJECT_SRC}
        ${CMAKE_SOURCE_DIR}/src/structure/terrain/terrain.cpp
        ${CMAKE_SOURCE_DIR}/src/structure/terrain/terrainFactory.cpp
        PARENT_SCOPE
        )